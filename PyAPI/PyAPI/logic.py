import os
from abc import abstractmethod
from typing import List, Union, Callable
import threading
import logging
import proto.MessageType_pb2 as MessageType
import proto.Message2Server_pb2 as Message2Server
import proto.Message2Clients_pb2 as Message2Clients
import PyAPI.structures as THUAI6
from PyAPI.utils import Proto2THUAI6, AssistFunction
from PyAPI.DebugAPI import HumanDebugAPI, ButcherDebugAPI
from PyAPI.API import HumanAPI, ButcherAPI
from PyAPI.AI import Setting
from PyAPI.Communication import Communication
from PyAPI.State import State
from PyAPI.Interface import ILogic, IGameTimer


class Logic(ILogic):
    def __init__(self, playerID: int) -> None:

        # ID
        self.__playerID: int = playerID
        self.__playerGUIDs: List[int] = []

        # 通信
        self.__comm: Communication

        # 存储状态
        self.__currentState: State
        self.__bufferState: State

        # timer，用于实际运行AI
        self.__timer: IGameTimer

        # AI线程
        self.__threadAI: threading.Thread

        # 互斥锁
        self.__mtxState: threading.Lock = threading.Lock()

        # 条件变量
        self.__cvBuffer: threading.Condition = threading.Condition()
        self.__cvAI: threading.Condition = threading.Condition()

        # 保存缓冲区数
        self.__counterState: int
        self.__counterBuffer: int

        # 记录游戏状态
        self.__gameState: THUAI6.GameState = THUAI6.GameState.NullGameState

        # 是否应该执行player()
        self.__AILoop: bool = True

        # buffer是否更新完毕
        self.__bufferUpdated: bool = False

        # 是否应当启动AI
        self.__AIStart: bool = False

        # asynchronous为True时控制内容更新的变量
        self.__freshed: bool = False

        self.__logger: logging.Logger = logging.getLogger("logic")

    # IAPI统一可用的接口

    def GetButchers(self) -> List[THUAI6.Butcher]:
        with self.__mtxState:
            return self.__currentState.butchers

    def GetHumans(self) -> List[THUAI6.Human]:
        with self.__mtxState:
            return self.__currentState.humans

    def GetProps(self) -> List[THUAI6.Prop]:
        with self.__mtxState:
            return self.__currentState.props

    def GetSelfInfo(self) -> Union[THUAI6.Human, THUAI6.Butcher]:
        with self.__mtxState:
            return self.__currentState.self

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        with self.__mtxState:
            return self.__currentState.map

    def GetPlaceType(self, x: int, y: int) -> THUAI6.PlaceType:
        with self.__mtxState:
            return self.__currentState.map[x][y]

    def Move(self, time: int, angle: float) -> bool:
        return self.__comm.Move(time, angle, self.__playerID)

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        return self.__comm.PickProp(propType, self.__playerID)

    def UseProp(self) -> bool:
        return self.__comm.UseProp(self.__playerID)

    def UseSkill(self) -> bool:
        return self.__comm.UseSkill(self.__playerID)

    def SendMessage(self, toID: int, message: str) -> bool:
        return self.__comm.SendMessage(toID, message, self.__playerID)

    def HaveMessage(self) -> bool:
        return self.__comm.HaveMessage()

    def GetMessage(self) -> tuple[int, str]:
        return self.__comm.GetMessage()

    def WaitThread(self) -> bool:
        self.__Update()
        return True

    def GetCounter(self) -> int:
        with self.__mtxState:
            return self.__counterState

    def GetPlayerGUIDs(self) -> List[int]:
        with self.__mtxState:
            return self.__playerGUIDs

    # IHumanAPI使用的接口

    def Escape(self) -> bool:
        return self.__comm.Escape(self.__playerID)

    def StartFixMachine(self) -> bool:
        return self.__comm.StartFixMachine(self.__playerID)

    def EndFixMachine(self) -> bool:
        return self.__comm.EndFixMachine(self.__playerID)

    def StartSaveHuman(self) -> bool:
        return self.__comm.StartSaveHuman(self.__playerID)

    def EndSaveHuman(self) -> bool:
        return self.__comm.EndSaveHuman(self.__playerID)

    # Butcher使用的接口

    def Attack(self, angle: float) -> bool:
        return self.__comm.Attack(angle, self.__playerID)

    def CarryHuman(self) -> bool:
        return self.__comm.CarryHuman(self.__playerID)

    def ReleaseHuman(self) -> bool:
        return self.__comm.ReleaseHuman(self.__playerID)

    def HangHuman(self) -> bool:
        return self.__comm.HangHuman(self.__playerID)

    # Logic内部逻辑
    def __TryConnection(self) -> bool:
        return self.__comm.TryConnection(self.__playerID)

    def __ProcessMessage(self) -> None:
        def messageThread():
            self.__comm.AddPlayer(self.__playerID)
            self.__comm.ReadMessage(self.__playerID)

            while self.__gameState != THUAI6.GameState.GameEnd:
                # 读取消息，无消息时此处阻塞
                clientMsg = self.__comm.GetMessage2Client()
                self.__gameState = Proto2THUAI6.gameStateDict[
                    clientMsg.game_state]

                if self.__gameState == THUAI6.GameState.GameStart:
                    # 读取玩家的GUID
                    self.__playerGUIDs.clear()
                    for human in clientMsg.human_message:
                        self.__playerGUIDs.append(human.guid)
                    for butcher in clientMsg.butcher_message:
                        self.__playerGUIDs.append(butcher.guid)
                    self.__currentState.guids = self.__playerGUIDs
                    self.__bufferState.guids = self.__playerGUIDs

                    self.__LoadBuffer(clientMsg)
                    self.__AILoop = True
                    self.__UnBlockAI()

                elif self.__gameState == THUAI6.GameState.GameRunning:
                    # 读取玩家的GUID
                    self.__playerGUIDs.clear()
                    for human in clientMsg.human_message:
                        self.__playerGUIDs.append(human.guid)
                    for butcher in clientMsg.butcher_message:
                        self.__playerGUIDs.append(butcher.guid)
                    self.__currentState.guids = self.__playerGUIDs
                    self.__bufferState.guids = self.__playerGUIDs
                    self.__LoadBuffer(clientMsg)
                else:
                    continue
            self.__AILoop = False
            with self.__cvBuffer:
                self.__bufferUpdated = True
                self.__counterBuffer = -1
                self.__cvBuffer.notify()

        threading.Thread(target=messageThread).start()

    def __LoadBuffer(self, message: Message2Clients.MessageToClient) -> None:
        with self.__cvBuffer:
            self.__bufferState.humans.clear()
            self.__bufferState.butchers.clear()
            self.__bufferState.props.clear()
            self.__bufferState.map = Proto2THUAI6.Protobuf2THUAI6Map(
                message.map_message)
            if Setting.playerType() == THUAI6.PlayerType.HumanPlayer:
                for human in message.human_message:
                    if human.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Human(
                            human)
                    self.__bufferState.humans.append(
                        Proto2THUAI6.Protobuf2THUAI6Human(human))
                for butcher in message.butcher_message:
                    viewRange: int = self.__bufferState.self.viewRange
                    deltaX: int = butcher.x - self.__bufferState.self.x
                    deltaY: int = butcher.y - self.__bufferState.self.y
                    if deltaX * deltaX + deltaY * deltaY <= viewRange * viewRange:
                        divide: int = max(abs(deltaX), abs(deltaY)) // 100
                        dx: float = deltaX / divide
                        dy: float = deltaY / divide
                        selfX: float = self.__bufferState.self.x
                        selfY: float = self.__bufferState.self.y
                        for i in range(divide):
                            selfX += dx
                            selfY += dy
                            if self.__bufferState.map[AssistFunction.GridToCell(int(selfX))][AssistFunction.GridToCell(int(selfY))]:
                                break
                        else:
                            self.__bufferState.butchers.append(
                                Proto2THUAI6.Protobuf2THUAI6Butcher(butcher))
            else:
                for butcher in message.butcher_message:
                    if butcher.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Butcher(
                            butcher)
                    self.__bufferState.butchers.append(
                        Proto2THUAI6.Protobuf2THUAI6Butcher(butcher))
                for human in message.human_message:
                    viewRange: int = self.__bufferState.self.viewRange
                    deltaX: int = human.x - self.__bufferState.self.x
                    deltaY: int = human.y - self.__bufferState.self.y
                    if deltaX * deltaX + deltaY * deltaY <= viewRange * viewRange:
                        divide: int = max(abs(deltaX), abs(deltaY)) // 100
                        dx: float = deltaX / divide
                        dy: float = deltaY / divide
                        selfX: float = self.__bufferState.self.x
                        selfY: float = self.__bufferState.self.y
                        for i in range(divide):
                            selfX += dx
                            selfY += dy
                            if self.__bufferState.map[AssistFunction.GridToCell(int(selfX))][AssistFunction.GridToCell(int(selfY))]:
                                break
                        else:
                            self.__bufferState.humans.append(
                                Proto2THUAI6.Protobuf2THUAI6Human(human))
            for prop in message.prop_message:
                self.__bufferState.props.append(
                    Proto2THUAI6.Protobuf2THUAI6Prop(prop))
            if Setting.asynchronous():
                with self.__mtxState:
                    self.__currentState, self.__bufferState = self.__bufferState, self.__currentState
                self.__freshed = True
            else:
                self.__bufferUpdated = True
            self.__counterBuffer += 1
            self.__cvBuffer.notify()

    def __UnBlockBuffer(self) -> None:
        with self.__cvBuffer:
            self.__bufferUpdated = True
            self.__cvBuffer.notify()

    def __UnBlockAI(self) -> None:
        with self.__cvAI:
            self.__AIStart = True
            self.__cvAI.notify()

    def __Update(self) -> None:
        if not Setting.asynchronous():
            with self.__cvBuffer:
                self.__cvBuffer.wait_for(lambda: self.__bufferUpdated)
                self.__bufferState, self.__currentState = self.__currentState, self.__bufferState
                self.__bufferUpdated = False
                self.__counterState = self.__counterBuffer

    def __Wait(self) -> None:
        self.__freshed = False
        with self.__cvBuffer:
            self.__cvBuffer.wait_for(lambda: self.__freshed)

    def Main(self, createAI: Callable, IP: str, port: str, file: bool, screen: bool, warnOnly: bool) -> None:

        # 建立日志组件
        self.__logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            "[%(asctime)s][%(name)s][%(levelname)s] %(message)s", "%H:%M:%S.%e")
        # 确保文件存在
        if not os.path.exists(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/logs"):
            os.makedirs(os.path.dirname(os.path.dirname(
                os.path.realpath(__file__))) + "/logs")

        fileHandler = logging.FileHandler(os.path.dirname(
            os.path.dirname(os.path.realpath(__file__))) + "/logs/logic-log.txt")
        screenHandler = logging.StreamHandler()
        if file:
            fileHandler.setLevel(logging.DEBUG)
            fileHandler.setFormatter(formatter)
            self.__logger.addHandler(fileHandler)
        if screen:
            if warnOnly:
                screenHandler.setLevel(logging.WARNING)
            else:
                screenHandler.setLevel(logging.INFO)
            screenHandler.setFormatter(formatter)
            self.__logger.addHandler(screenHandler)

        self.__logger.info("*********Basic Info*********")
        self.__logger.info("asynchronous: %s", Setting.asynchronous())
        self.__logger.info("server: %s:%s", IP, port)
        self.__logger.info("playerID: %s", self.__playerID)
        self.__logger.info("player type: %s",
                           THUAI6.playerTypeDict[Setting.playerType()])
        self.__logger.info("****************************")

        # 建立通信组件
        self.__comm = Communication(IP, port)

        # 构造timer
        if Setting.playerType() == THUAI6.PlayerType.HumanPlayer:
            if not file and not screen:
                self.__timer = HumanAPI(self)
            else:
                self.__timer = HumanDebugAPI(
                    self, file, screen, warnOnly, self.__playerID)
        elif Setting.playerType() == THUAI6.PlayerType.ButcherPlayer:
            if not file and not screen:
                self.__timer = ButcherAPI(self)
            else:
                self.__timer = ButcherDebugAPI(
                    self, file, screen, warnOnly, self.__playerID)

        # 构建AI线程
        def AIThread():
            with self.__cvAI:
                self.__cvAI.wait_for(lambda: self.__AIStart)

            ai = createAI()
            while self.__AILoop:
                if Setting.asynchronous():
                    self.__Wait()
                    self.__timer.StartTimer()
                    self.__timer.Play(ai)
                    self.__timer.EndTimer()
                else:
                    self.__Update()
                    self.__timer.StartTimer()
                    self.__timer.Play(ai)
                    self.__timer.EndTimer()

        if self.__TryConnection():
            self.__threadAI = threading.Thread(target=AIThread)
            self.__threadAI.start()
            self.__ProcessMessage()
            self.__threadAI.join()
        else:
            self.__AILoop = False
            return