import os
from typing import List, Union, Callable, Tuple
import threading
import logging
import copy
import platform
import proto.MessageType_pb2 as MessageType
import proto.Message2Server_pb2 as Message2Server
import proto.Message2Clients_pb2 as Message2Clients
from queue import Queue
import PyAPI.structures as THUAI6
from PyAPI.utils import Proto2THUAI6, AssistFunction
from PyAPI.DebugAPI import StudentDebugAPI, TrickerDebugAPI
from PyAPI.API import StudentAPI, TrickerAPI
from PyAPI.AI import Setting
from PyAPI.Communication import Communication
from PyAPI.State import State
from PyAPI.Interface import ILogic, IGameTimer


class Logic(ILogic):
    def __init__(self, playerID: int, playerType: THUAI6.PlayerType) -> None:
        # ID
        self.__playerID: int = playerID

        self.__playerType: THUAI6.PlayerType = playerType

        # 通信
        self.__comm: Communication

        # 存储状态
        self.__currentState: State = State()
        self.__bufferState: State = State()

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
        self.__counterState: int = 0
        self.__counterBuffer: int = 0

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

        self.__messageQueue: Queue = Queue()

    # IAPI统一可用的接口

    def GetTrickers(self) -> List[THUAI6.Tricker]:
        with self.__mtxState:
            self.__logger.debug("Called GetTrickers")
            return copy.deepcopy(self.__currentState.trickers)

    def GetStudents(self) -> List[THUAI6.Student]:
        with self.__mtxState:
            self.__logger.debug("Called GetStudents")
            return copy.deepcopy(self.__currentState.students)

    def GetProps(self) -> List[THUAI6.Prop]:
        with self.__mtxState:
            self.__logger.debug("Called GetProps")
            return copy.deepcopy(self.__currentState.props)

    def GetBullets(self) -> List[THUAI6.Bullet]:
        with self.__mtxState:
            self.__logger.debug("Called GetBullets")
            return copy.deepcopy(self.__currentState.bullets)

    def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]:
        with self.__mtxState:
            self.__logger.debug("Called GetSelfInfo")
            return copy.deepcopy(self.__currentState.self)

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        with self.__mtxState:
            self.__logger.debug("Called GetFullMap")
            return copy.deepcopy(self.__currentState.gameMap)

    def GetPlaceType(self, x: int, y: int) -> THUAI6.PlaceType:
        with self.__mtxState:
            if (
                x < 0
                or x >= len(self.__currentState.gameMap)
                or y < 0
                or y >= len(self.__currentState.gameMap[0])
            ):
                self.__logger.warning("Invalid position")
                return THUAI6.PlaceType.NullPlaceType
            self.__logger.debug("Called GetPlaceType")
            return copy.deepcopy(self.__currentState.gameMap[x][y])

    def IsDoorOpen(self, x: int, y: int) -> bool:
        with self.__mtxState:
            self.__logger.debug("Called IsDoorOpen")
            if (x, y) in self.__currentState.mapInfo.doorState:
                return copy.deepcopy(self.__currentState.mapInfo.doorState[(x, y)])
            else:
                self.__logger.warning("Door not found")
                return False

    def GetClassroomProgress(self, x: int, y: int) -> int:
        with self.__mtxState:
            self.__logger.debug("Called GetClassroomProgress")
            if (x, y) in self.__currentState.mapInfo.classroomState:
                return copy.deepcopy(self.__currentState.mapInfo.classroomState[(x, y)])
            else:
                self.__logger.warning("Classroom not found")
                return -1

    def GetChestProgress(self, x: int, y: int) -> int:
        with self.__mtxState:
            self.__logger.debug("Called GetChestProgress")
            if (x, y) in self.__currentState.mapInfo.chestState:
                return copy.deepcopy(self.__currentState.mapInfo.chestState[(x, y)])
            else:
                self.__logger.warning("Chest not found")
                return -1

    def GetGateProgress(self, x: int, y: int) -> int:
        with self.__mtxState:
            self.__logger.debug("Called GetGateProgress")
            if (x, y) in self.__currentState.mapInfo.gateState:
                return copy.deepcopy(self.__currentState.mapInfo.gateState[(x, y)])
            else:
                self.__logger.warning("Gate not found")
                return -1

    def GetHiddenGateState(self, x: int, y: int) -> THUAI6.HiddenGateState:
        with self.__mtxState:
            self.__logger.debug("Called GetHiddenGateState")
            if (x, y) in self.__currentState.mapInfo.hiddenGateState:
                return copy.deepcopy(
                    self.__currentState.mapInfo.hiddenGateState[(x, y)]
                )
            else:
                self.__logger.warning("HiddenGate not found")
                return THUAI6.HiddenGateState.Null

    def GetDoorProgress(self, x: int, y: int) -> int:
        with self.__mtxState:
            self.__logger.debug("Called GetDoorProgress")
            if (x, y) in self.__currentState.mapInfo.doorProgress:
                return copy.deepcopy(self.__currentState.mapInfo.doorProgress[(x, y)])
            else:
                self.__logger.warning("Door not found")
                return -1

    def GetGameInfo(self) -> THUAI6.GameInfo:
        with self.__mtxState:
            self.__logger.debug("Called GetGameInfo")
            return copy.deepcopy(self.__currentState.gameInfo)

    def Move(self, time: int, angle: float) -> bool:
        self.__logger.debug("Called Move")
        return self.__comm.Move(time, angle, self.__playerID)

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        self.__logger.debug("Called PickProp")
        return self.__comm.PickProp(propType, self.__playerID)

    def UseProp(self, propType: THUAI6.PropType) -> bool:
        self.__logger.debug("Called UseProp")
        return self.__comm.UseProp(propType, self.__playerID)

    def ThrowProp(self, propType: THUAI6.PropType) -> bool:
        self.__logger.debug("Called ThrowProp")
        return self.__comm.ThrowProp(propType, self.__playerID)

    def UseSkill(self, skillID: int) -> bool:
        self.__logger.debug("Called UseSkill")
        return self.__comm.UseSkill(skillID, self.__playerID)

    def SendMessage(self, toID: int, message: Union[str, bytes]) -> bool:
        self.__logger.debug("Called SendMessage")
        return self.__comm.SendMessage(toID, message, self.__playerID)

    def HaveMessage(self) -> bool:
        self.__logger.debug("Called HaveMessage")
        return not self.__messageQueue.empty()

    def GetMessage(self) -> Tuple[int, Union[str, bytes]]:
        self.__logger.debug("Called GetMessage")
        if self.__messageQueue.empty():
            self.__logger.warning("Message queue is empty!")
            return -1, ""
        else:
            return self.__messageQueue.get()

    def WaitThread(self) -> bool:
        self.__Update()
        return True

    def GetCounter(self) -> int:
        with self.__mtxState:
            return copy.deepcopy(self.__counterState)

    def GetPlayerGUIDs(self) -> List[int]:
        with self.__mtxState:
            return copy.deepcopy(self.__currentState.guids)

    # IStudentAPI使用的接口

    def Graduate(self) -> bool:
        self.__logger.debug("Called Graduate")
        return self.__comm.Graduate(self.__playerID)

    def StartLearning(self) -> bool:
        self.__logger.debug("Called StartLearning")
        return self.__comm.StartLearning(self.__playerID)

    def StartEncourageMate(self, mateID: int) -> bool:
        self.__logger.debug("Called StartEncourageMate")
        return self.__comm.StartEncourageMate(self.__playerID, mateID)

    def StartRouseMate(self, mateID: int) -> bool:
        self.__logger.debug("Called StartRouseMate")
        return self.__comm.StartRouseMate(self.__playerID, mateID)

    def Attack(self, angle: float) -> bool:
        self.__logger.debug("Called Trick")
        return self.__comm.Attack(angle, self.__playerID)

    def CloseDoor(self) -> bool:
        self.__logger.debug("Called CloseDoor")
        return self.__comm.CloseDoor(self.__playerID)

    def OpenDoor(self) -> bool:
        self.__logger.debug("Called OpenDoor")
        return self.__comm.OpenDoor(self.__playerID)

    def SkipWindow(self) -> bool:
        self.__logger.debug("Called SkipWindow")
        return self.__comm.SkipWindow(self.__playerID)

    def StartOpenGate(self) -> bool:
        self.__logger.debug("Called StartOpenGate")
        return self.__comm.StartOpenGate(self.__playerID)

    def StartOpenChest(self) -> bool:
        self.__logger.debug("Called StartOpenChest")
        return self.__comm.StartOpenChest(self.__playerID)

    def EndAllAction(self) -> bool:
        self.__logger.debug("Called EndAllAction")
        return self.__comm.EndAllAction(self.__playerID)

    def HaveView(
        self, gridX: int, gridY: int, selfX: int, selfY: int, viewRange: int
    ) -> bool:
        with self.__mtxState:
            return AssistFunction.HaveView(
                viewRange, selfX, selfY, gridX, gridY, self.__currentState.gameMap
            )

    # Logic内部逻辑
    def __TryConnection(self) -> bool:
        self.__logger.info("Try to connect to server...")
        return self.__comm.TryConnection(self.__playerID)

    def __ProcessMessage(self) -> None:
        def messageThread():
            self.__logger.info("Message thread start!")
            self.__comm.AddPlayer(self.__playerID, self.__playerType)
            self.__logger.info("Join the player!")

            while self.__gameState != THUAI6.GameState.GameEnd:
                # 读取消息，无消息时此处阻塞
                clientMsg = self.__comm.GetMessage2Client()
                self.__logger.debug("Get message from server!")
                self.__gameState = Proto2THUAI6.gameStateDict[clientMsg.game_state]

                if self.__gameState == THUAI6.GameState.GameStart:
                    # 读取玩家的GUID
                    self.__logger.info("Game start!")

                    for obj in clientMsg.obj_message:
                        if obj.WhichOneof("message_of_obj") == "map_message":
                            gameMap: List[List[THUAI6.PlaceType]] = []
                            for row in obj.map_message.row:
                                col: List[THUAI6.PlaceType] = []
                                for place in row.col:
                                    col.append(Proto2THUAI6.placeTypeDict[place])
                                gameMap.append(col)
                            self.__currentState.gameMap = gameMap
                            self.__bufferState.gameMap = gameMap
                            self.__logger.info("Game map loaded!")
                            break
                    else:
                        self.__logger.error("Map not found!")

                    self.__LoadBuffer(clientMsg)
                    self.__AILoop = True
                    self.__UnBlockAI()

                elif self.__gameState == THUAI6.GameState.GameRunning:
                    # 读取玩家的GUID
                    self.__LoadBuffer(clientMsg)
                else:
                    self.__logger.error("Unknown GameState!")
                    continue
            with self.__cvBuffer:
                self.__bufferUpdated = True
                self.__counterBuffer = -1
                self.__cvBuffer.notify()
                self.__logger.info("Game End!")
            self.__logger.info("Message thread end!")
            self.__AILoop = False

        threading.Thread(target=messageThread).start()

    def __LoadBufferSelf(self, message: Message2Clients.MessageToClient) -> None:
        if self.__playerType == THUAI6.PlayerType.StudentPlayer:
            for item in message.obj_message:
                if item.WhichOneof("message_of_obj") == "student_message":
                    if item.student_message.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Student(
                            item.student_message
                        )
                        self.__bufferState.students.append(self.__bufferState.self)
                    else:
                        self.__bufferState.students.append(
                            Proto2THUAI6.Protobuf2THUAI6Student(item.student_message)
                        )
                    self.__logger.debug("Add Student!")
        else:
            for item in message.obj_message:
                if item.WhichOneof("message_of_obj") == "tricker_message":
                    if item.tricker_message.player_id == self.__playerID:
                        self.__bufferState.self = Proto2THUAI6.Protobuf2THUAI6Tricker(
                            item.tricker_message
                        )
                        self.__bufferState.trickers.append(self.__bufferState.self)
                    else:
                        self.__bufferState.trickers.append(
                            Proto2THUAI6.Protobuf2THUAI6Tricker(item.tricker_message)
                        )
                    self.__logger.debug("Add Tricker!")

    def __LoadBufferCase(self, item: Message2Clients.MessageOfObj) -> None:
        if (
            self.__playerType == THUAI6.PlayerType.StudentPlayer
            and item.WhichOneof("message_of_obj") == "tricker_message"
        ):
            if MessageType.TRICKER_INVISIBLE in item.tricker_message.buff:
                return
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.tricker_message.x,
                item.tricker_message.y,
                self.__bufferState.gameMap,
            ):
                self.__bufferState.trickers.append(
                    Proto2THUAI6.Protobuf2THUAI6Tricker(item.tricker_message)
                )
                self.__logger.debug("Add Tricker!")
        elif (
            self.__playerType == THUAI6.PlayerType.TrickerPlayer
            and item.WhichOneof("message_of_obj") == "student_message"
        ):
            if THUAI6.TrickerBuffType.Clairaudience in self.__bufferState.self.buff:
                self.__bufferState.students.append(
                    Proto2THUAI6.Protobuf2THUAI6Student(item.student_message)
                )
                self.__logger.debug("Add Student!")
                return
            if MessageType.STUDENT_INVISIBLE in item.student_message.buff:
                return
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.student_message.x,
                item.student_message.y,
                self.__bufferState.gameMap,
            ):
                self.__bufferState.students.append(
                    Proto2THUAI6.Protobuf2THUAI6Student(item.student_message)
                )
                self.__logger.debug("Add Student!")
        elif item.WhichOneof("message_of_obj") == "prop_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.prop_message.x,
                item.prop_message.y,
                self.__bufferState.gameMap,
            ):
                self.__bufferState.props.append(
                    Proto2THUAI6.Protobuf2THUAI6Prop(item.prop_message)
                )
                self.__logger.debug("Add Prop!")
        elif item.WhichOneof("message_of_obj") == "bullet_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.bullet_message.x,
                item.bullet_message.y,
                self.__bufferState.gameMap,
            ):
                self.__bufferState.bullets.append(
                    Proto2THUAI6.Protobuf2THUAI6Bullet(item.bullet_message)
                )
                self.__logger.debug("Add Bullet!")
        elif item.WhichOneof("message_of_obj") == "classroom_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.classroom_message.x,
                item.classroom_message.y,
                self.__bufferState.gameMap,
            ):
                pos = (
                    AssistFunction.GridToCell(item.classroom_message.x),
                    AssistFunction.GridToCell(item.classroom_message.y),
                )
                if pos not in self.__bufferState.mapInfo.classroomState:
                    self.__bufferState.mapInfo.classroomState[
                        pos
                    ] = item.classroom_message.progress
                    self.__logger.debug("Add Classroom!")
                else:
                    self.__bufferState.mapInfo.classroomState[
                        pos
                    ] = item.classroom_message.progress
                    self.__logger.debug("Update Classroom!")
        elif item.WhichOneof("message_of_obj") == "chest_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.chest_message.x,
                item.chest_message.y,
                self.__bufferState.gameMap,
            ):
                pos = (
                    AssistFunction.GridToCell(item.chest_message.x),
                    AssistFunction.GridToCell(item.chest_message.y),
                )
                if pos not in self.__bufferState.mapInfo.chestState:
                    self.__bufferState.mapInfo.chestState[
                        pos
                    ] = item.chest_message.progress
                    self.__logger.debug(f"Add Chest at {pos[0]}, {pos[1]}")
                else:
                    self.__bufferState.mapInfo.chestState[
                        pos
                    ] = item.chest_message.progress
                    self.__logger.debug(f"Update Chest at {pos[0]}, {pos[1]}")
        elif item.WhichOneof("message_of_obj") == "door_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.door_message.x,
                item.door_message.y,
                self.__bufferState.gameMap,
            ):
                pos = (
                    AssistFunction.GridToCell(item.door_message.x),
                    AssistFunction.GridToCell(item.door_message.y),
                )
                if pos not in self.__bufferState.mapInfo.doorState:
                    self.__bufferState.mapInfo.doorState[
                        pos
                    ] = item.door_message.is_open
                    self.__bufferState.mapInfo.doorProgress[
                        pos
                    ] = item.door_message.progress
                    self.__logger.debug("Add Door!")
                else:
                    self.__bufferState.mapInfo.doorState[
                        pos
                    ] = item.door_message.is_open
                    self.__bufferState.mapInfo.doorProgress[
                        pos
                    ] = item.door_message.progress
                    self.__logger.debug("Update Door!")
        elif item.WhichOneof("message_of_obj") == "hidden_gate_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.hidden_gate_message.x,
                item.hidden_gate_message.y,
                self.__bufferState.gameMap,
            ):
                pos = (
                    AssistFunction.GridToCell(item.hidden_gate_message.x),
                    AssistFunction.GridToCell(item.hidden_gate_message.y),
                )
                if pos not in self.__bufferState.mapInfo.hiddenGateState:
                    self.__bufferState.mapInfo.hiddenGateState[
                        pos
                    ] = Proto2THUAI6.Bool2HiddenGateState(
                        item.hidden_gate_message.opened
                    )
                    self.__logger.debug("Add HiddenGate!")
                else:
                    self.__bufferState.mapInfo.hiddenGateState[
                        pos
                    ] = Proto2THUAI6.Bool2HiddenGateState(
                        item.hidden_gate_message.opened
                    )
                    self.__logger.debug("Update HiddenGate!")
        elif item.WhichOneof("message_of_obj") == "gate_message":
            if AssistFunction.HaveView(
                self.__bufferState.self.viewRange,
                self.__bufferState.self.x,
                self.__bufferState.self.y,
                item.gate_message.x,
                item.gate_message.y,
                self.__bufferState.gameMap,
            ):
                pos = (
                    AssistFunction.GridToCell(item.gate_message.x),
                    AssistFunction.GridToCell(item.gate_message.y),
                )
                if pos not in self.__bufferState.mapInfo.gateState:
                    self.__bufferState.mapInfo.gateState[
                        pos
                    ] = item.gate_message.progress
                    self.__logger.debug("Add Gate!")
                else:
                    self.__bufferState.mapInfo.gateState[
                        pos
                    ] = item.gate_message.progress
                    self.__logger.debug("Update Gate!")
        elif item.WhichOneof("message_of_obj") == "news_message":
            if item.news_message.to_id == self.__playerID:
                if item.news_message.WhichOneof("news") == "text_message":
                    self.__messageQueue.put(
                        (item.news_message.from_id, item.news_message.text_message)
                    )
                    self.__logger.debug("Add News!")
                elif item.news_message.WhichOneof("news") == "binary_message":
                    self.__messageQueue.put(
                        (item.news_message.from_id, item.news_message.binary_message)
                    )
                    self.__logger.debug("Add News!")
                else:
                    self.__logger.error("Unknown News!")
        else:
            self.__logger.debug("Unknown Message!")

    def __LoadBuffer(self, message: Message2Clients.MessageToClient) -> None:
        with self.__cvBuffer:
            self.__bufferState.students.clear()
            self.__bufferState.trickers.clear()
            self.__bufferState.props.clear()
            self.__bufferState.bullets.clear()
            self.__bufferState.bombedBullets.clear()
            self.__bufferState.guids.clear()
            self.__logger.debug("Buffer cleared!")

            for obj in message.obj_message:
                if obj.WhichOneof("message_of_obj") == "student_message":
                    self.__bufferState.guids.append(obj.student_message.guid)
            for obj in message.obj_message:
                if obj.WhichOneof("message_of_obj") == "tricker_message":
                    self.__bufferState.guids.append(obj.tricker_message.guid)

            self.__bufferState.gameInfo = Proto2THUAI6.Protobuf2THUAI6GameInfo(
                message.all_message
            )

            self.__LoadBufferSelf(message)
            for item in message.obj_message:
                self.__LoadBufferCase(item)
            if Setting.asynchronous():
                with self.__mtxState:
                    self.__currentState, self.__bufferState = (
                        self.__bufferState,
                        self.__currentState,
                    )
                    self.__counterState = self.__counterBuffer
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
                with self.__mtxState:
                    self.__bufferState, self.__currentState = (
                        self.__currentState,
                        self.__bufferState,
                    )
                    self.__counterState = self.__counterBuffer
                self.__bufferUpdated = False
                self.__logger.info("Update state!")

    def __Wait(self) -> None:
        self.__freshed = False
        with self.__cvBuffer:
            self.__cvBuffer.wait_for(lambda: self.__freshed)

    def Main(
        self,
        createAI: Callable,
        IP: str,
        port: str,
        file: bool,
        screen: bool,
        warnOnly: bool,
    ) -> None:
        # 建立日志组件
        self.__logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            "[%(name)s] [%(asctime)s.%(msecs)03d] [%(levelname)s] %(message)s",
            "%H:%M:%S",
        )
        # 确保文件存在
        # if not os.path.exists(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/logs"):
        #     os.makedirs(os.path.dirname(os.path.dirname(
        #         os.path.realpath(__file__))) + "/logs")

        if platform.system().lower() == "windows":
            os.system(
                f'mkdir "{os.path.dirname(os.path.dirname(os.path.realpath(__file__)))}\\logs"'
            )
        else:
            os.system(
                f'mkdir -p "{os.path.dirname(os.path.dirname(os.path.realpath(__file__)))}/logs"'
            )

        fileHandler = logging.FileHandler(
            os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
            + "/logs/logic"
            + str(self.__playerID)
            + "-log.txt",
            "w+",
            encoding="utf-8",
        )
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
        self.__logger.info("player type: %s", self.__playerType.name)
        self.__logger.info("****************************")

        # 建立通信组件
        self.__comm = Communication(IP, port)

        # 构造timer
        if self.__playerType == THUAI6.PlayerType.StudentPlayer:
            if not file and not screen:
                self.__timer = StudentAPI(self)
            else:
                self.__timer = StudentDebugAPI(
                    self, file, screen, warnOnly, self.__playerID
                )
        elif self.__playerType == THUAI6.PlayerType.TrickerPlayer:
            if not file and not screen:
                self.__timer = TrickerAPI(self)
            else:
                self.__timer = TrickerDebugAPI(
                    self, file, screen, warnOnly, self.__playerID
                )

        # 构建AI线程
        def AIThread():
            with self.__cvAI:
                self.__cvAI.wait_for(lambda: self.__AIStart)

            ai = createAI(self.__playerID)
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
                "Connect to the server successfully, AI thread will be started."
            )
            self.__threadAI = threading.Thread(target=AIThread)
            self.__threadAI.start()
            self.__ProcessMessage()
            self.__logger.info("Join the AI thread.")
            self.__threadAI.join()
        else:
            self.__AILoop = False
            self.__logger.error("Failed to connect to the server.")
            return
