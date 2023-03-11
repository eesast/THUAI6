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
from PyAPI.DebugAPI import StudentDebugAPI, TrickerDebugAPI
from PyAPI.API import StudentAPI, TrickerAPI
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

    def GetTrickers(self) -> List[THUAI6.Tricker]:
        with self.__mtxState:
            self.__logger.debug("Called GetTrickers")
            return self.__currentState.trickers

    def GetStudents(self) -> List[THUAI6.Student]:
        with self.__mtxState:
            self.__logger.debug("Called GetStudents")
            return self.__currentState.students

    def GetProps(self) -> List[THUAI6.Prop]:
        with self.__mtxState:
            self.__logger.debug("Called GetProps")
            return self.__currentState.props

    def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]:
        with self.__mtxState:
            self.__logger.debug("Called GetSelfInfo")
            return self.__currentState.self

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        with self.__mtxState:
            self.__logger.debug("Called GetFullMap")
            return self.__currentState.map

    def GetPlaceType(self, x: int, y: int) -> THUAI6.PlaceType:
        with self.__mtxState:
            self.__logger.debug("Called GetPlaceType")
            return self.__currentState.map[x][y]

    def Move(self, time: int, angle: float) -> bool:
        self.__logger.debug("Called Move")
        return self.__comm.Move(time, angle, self.__playerID)

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        self.__logger.debug("Called PickProp")
        return self.__comm.PickProp(propType, self.__playerID)

    def UseProp(self) -> bool:
        self.__logger.debug("Called UseProp")
        return self.__comm.UseProp(self.__playerID)

    def UseSkill(self) -> bool:
        self.__logger.debug("Called UseSkill")
        return self.__comm.UseSkill(self.__playerID)

    def SendMessage(self, toID: int, message: str) -> bool:
        self.__logger.debug("Called SendMessage")
        return self.__comm.SendMessage(toID, message, self.__playerID)

    def HaveMessage(self) -> bool:
        self.__logger.debug("Called HaveMessage")
        return self.__comm.HaveMessage()

    def GetMessage(self) -> tuple[int, str]:
        self.__logger.debug("Called GetMessage")
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

    # IStudentAPI使用的接口

    def Graduate(self) -> bool:
        self.__logger.debug("Called Graduate")
        return self.__comm.Graduate(self.__playerID)

    def StartLearning(self) -> bool:
        self.__logger.debug("Called StartLearning")
        return self.__comm.StartLearning(self.__playerID)

    def EndLearning(self) -> bool:
        self.__logger.debug("Called EndLearning")
        return self.__comm.EndLearning(self.__playerID)

    def StartHelpMate(self) -> bool:
        self.__logger.debug("Called StartHelpMate")
        return self.__comm.StartHelpMate(self.__playerID)

    def EndHelpMate(self) -> bool:
        self.__logger.debug("Called EndHelpMate")
        return self.__comm.EndHelpMate(self.__playerID)

    # Tricker使用的接口

    def Trick(self, angle: float) -> bool:
        self.__logger.debug("Called Trick")
        return self.__comm.Trick(angle, self.__playerID)

    def StartExam(self) -> bool:
        self.__logger.debug("Called StartExam")
        return self.__comm.StartExam(self.__playerID)

    def EndExam(self) -> bool:
        self.__logger.debug("Called EndExam")
        return self.__comm.EndExam(self.__playerID)

    def MakeFail(self) -> bool:
        self.__logger.debug("Called MakeFail")
        return self.__comm.MakeFail(self.__playerID)

    # Logic内部逻辑
    def __TryConnection(self) -> bool:
        self.__logger.info("Try to connect to server...")
        return self.__comm.TryConnection(self.__playerID)

    def __ProcessMessage(self) -> None:
        def messageThread():
            self.__logger.info("Message thread start!")
            self.__comm.AddPlayer(self.__playerID)
            self.__logger.info("Join the player!")
            self.__comm.ReadMessage(self.__playerID)

            while self.__gameState != THUAI6.GameState.GameEnd:
                # 读取消息，无消息时此处阻塞
                clientMsg = self.__comm.GetMessage2Client()
                self.__logger.debug("Get message from server!")
                self.__gameState = Proto2THUAI6.gameStateDict[
                    clientMsg.game_state]

                if self.__gameState == THUAI6.GameState.GameStart:
                    # 读取玩家的GUID
                    self.__logger.info("Game start!")
                    self.__playerGUIDs.clear()
                    for student in clientMsg.student_message:
                        self.__playerGUIDs.append(student.guid)
                    for tricker in clientMsg.tricker_message:
                        self.__playerGUIDs.append(tricker.guid)
                    self.__currentState.guids = self.__playerGUIDs
                    self.__bufferState.guids = self.__playerGUIDs

                    self.__LoadBuffer(clientMsg)
                    self.__AILoop = True
                    self.__UnBlockAI()

                elif self.__gameState == THUAI6.GameState.GameRunning:
                    # 读取玩家的GUID
                    self.__playerGUIDs.clear()
                    for student in clientMsg.student_message:
                        self.__playerGUIDs.append(student.guid)
                    for tricker in clientMsg.tricker_message:
                        self.__playerGUIDs.append(tricker.guid)
                    self.__currentState.guids = self.__playerGUIDs
                    self.__bufferState.guids = self.__playerGUIDs
                    self.__LoadBuffer(clientMsg)
                else:
                    self.__logger.error("Unknown GameState!")
                    continue
            self.__AILoop = False
            with self.__cvBuffer:
                self.__bufferUpdated = True
                self.__counterBuffer = -1
                self.__cvBuffer.notify()
                self.__logger.info("Game End!")
            self.__logger.info("Message thread end!")

        threading.Thread(target=messageThread).start()

    def __LoadBuffer(self, message: Message2Clients.MessageToClient) -> None:
        with self.__cvBuffer:
            self.__bufferState.students.clear()
            self.__bufferState.trickers.clear()
            self.__bufferState.props.clear()
            self.__logger.debug("Buffer cleared!")
            self.__bufferState.map = Proto2THUAI6.Protobuf2THUAI6Map(
                message.map_message)
            if Setting.playerType() == THUAI6.PlayerType.StudentPlayer:
                for student in message.student_message:
                    if student.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Student(
                            student)
                    self.__bufferState.students.append(
                        Proto2THUAI6.Protobuf2THUAI6Student(student))
                    self.__logger.debug("Add Student!")
                for tricker in message.tricker_message:
                    if AssistFunction.HaveView(self.__bufferState.self.viewRange, self.__bufferState.self.x, self.__bufferState.self.y, tricker.x, tricker.y, self.__bufferState.map):
                        self.__bufferState.trickers.append(
                            Proto2THUAI6.Protobuf2THUAI6Tricker(tricker))
                        self.__logger.debug("Add Tricker!")
            else:
                for tricker in message.tricker_message:
                    if tricker.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Tricker(
                            tricker)
                    self.__bufferState.trickers.append(
                        Proto2THUAI6.Protobuf2THUAI6Tricker(tricker))
                    self.__logger.debug("Add Tricker!")
                for student in message.student_message:
                    if AssistFunction.HaveView(self.__bufferState.self.viewRange, self.__bufferState.self.x, self.__bufferState.self.y, student.x, student.y, self.__bufferState.map):
                        self.__bufferState.students.append(
                            Proto2THUAI6.Protobuf2THUAI6Student(student))
                        self.__logger.debug("Add Student!")
            for prop in message.prop_message:
                self.__bufferState.props.append(
                    Proto2THUAI6.Protobuf2THUAI6Prop(prop))
                self.__logger.debug("Add Prop!")
            if Setting.asynchronous():
                with self.__mtxState:
                    self.__currentState, self.__bufferState = self.__bufferState, self.__currentState
                    self.__logger.info("Update state!")
                self.__freshed = True
            else:
                self.__bufferUpdated = True
            self.__counterBuffer += 1
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
                self.__logger.info("Update state!")

    def __Wait(self) -> None:
        self.__freshed = False
        with self.__cvBuffer:
            self.__cvBuffer.wait_for(lambda: self.__freshed)

    def Main(self, createAI: Callable, IP: str, port: str, file: bool, screen: bool, warnOnly: bool) -> None:

        # 建立日志组件
        self.__logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            "[%(name)s] [%(asctime)s] [%(levelname)s] %(message)s", "%H:%M:%S.%e")
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
        self.__logger.info("player type: %s", Setting.playerType().name)
        self.__logger.info("****************************")

        # 建立通信组件
        self.__comm = Communication(IP, port)

        # 构造timer
        if Setting.playerType() == THUAI6.PlayerType.StudentPlayer:
            if not file and not screen:
                self.__timer = StudentAPI(self)
            else:
                self.__timer = StudentDebugAPI(
                    self, file, screen, warnOnly, self.__playerID)
        elif Setting.playerType() == THUAI6.PlayerType.TrickerPlayer:
            if not file and not screen:
                self.__timer = TrickerAPI(self)
            else:
                self.__timer = TrickerDebugAPI(
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
            self.__logger.info(
                "Connect to the server successfully, AI thread will be started.")
            self.__threadAI = threading.Thread(target=AIThread)
            self.__threadAI.start()
            self.__ProcessMessage()
            self.__logger.info("Join the AI thread.")
            self.__threadAI.join()
        else:
            self.__AILoop = False
            self.__logger.error("Failed to connect to the server.")
            return
