from abc import abstractmethod
from typing import List, Union, Callable
import threading

from Interface import ILogic, IGameTimer
from State import State
import structures as THUAI6
import proto.Message2Clients_pb2 as Message2Clients
import proto.Message2Server_pb2 as Message2Server
import proto.MessageType_pb2 as MessageType


class Logic(ILogic):
    def __init__(self, playerType: THUAI6.PlayerType, playerID: int, humanType: THUAI6.HumanType, butcherType: THUAI6.ButcherType) -> None:

        # 类型、ID
        self.__playerType: THUAI6.PlayerType = playerType
        self.__playerID: int = playerID
        self.__humanType: THUAI6.HumanType = humanType
        self.__butcherType: THUAI6.ButcherType = butcherType

        # 存储状态
        self.__currentState: State
        self.__bufferState: State

        # timer，用于实际运行AI
        self.__timer: IGameTimer

        # AI线程
        self.__threadAI: threading.Thread

        # 互斥锁
        self.__mtxAI: threading.Lock
        self.__mtxBuffer: threading.Lock
        self.__mtxTimer: threading.Lock

        # 条件变量
        self.__cvBuffer: threading.Condition
        self.__cvAI: threading.Condition

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

        # asynchronous为true时控制内容更新的变量
        self.__freshed: bool = False

    # IAPI统一可用的接口

    def GetButchers(self) -> List[THUAI6.Butcher]:
        pass

    def GetHumans(self) -> List[THUAI6.Human]:
        pass

    def GetProps(self) -> List[THUAI6.Prop]:
        pass

    def GetSelfInfo(self) -> Union[THUAI6.Human, THUAI6.Butcher]:
        pass

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        pass

    def GetPlaceType(self, x: int, y: int) -> THUAI6.PlaceType:
        pass

    def Move(self, time: int, angle: float) -> bool:
        pass

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        pass

    def UseProp(self) -> bool:
        pass

    def UseSkill(self) -> bool:
        pass

    def SendMessage(self, toID: int, message: str) -> bool:
        pass

    def HaveMessage(self) -> bool:
        pass

    def GetMessage(self) -> tuple[int, str]:
        pass

    def WaitThread(self) -> bool:
        pass

    def GetCounter(self) -> int:
        pass

    def GetPlayerGUIDs(self) -> List[int]:
        pass

    # IHumanAPI使用的接口

    def Escape(self) -> bool:
        pass

    def StartFixMachine(self) -> bool:
        pass

    def EndFixMachine(self) -> bool:
        pass

    def StartSaveHuman(self) -> bool:
        pass

    def EndSaveHuman(self) -> bool:
        pass

    # Butcher使用的接口

    def Attack(self, angle: float) -> bool:
        pass

    def CarryHuman(self) -> bool:
        pass

    def ReleaseHuman(self) -> bool:
        pass

    def HangHuman(self) -> bool:
        pass

    # Logic内部逻辑
    def __TryConnection(self) -> bool:
        pass

    def __ProcessMessage(self) -> None:
        pass

    def __LoadBuffer(self) -> None:
        pass

    def __UnBlockBuffer(self) -> None:
        pass

    def __UnBlockAI(self) -> None:
        pass

    def __Update(self) -> None:
        pass

    def __Wait(self) -> None:
        pass

    def Main(self, createAI: Callable, IP: str, port: str, file: bool, print: bool, warnOnly: bool) -> None:
        pass
