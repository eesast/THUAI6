import PyAPI.structures as THUAI6
from PyAPI.Interface import ILogic, IStudentAPI, ITrickerAPI, IGameTimer, IAI
from math import pi
from concurrent.futures import ThreadPoolExecutor, Future
from typing import List, cast, Tuple, Union


class StudentAPI(IStudentAPI, IGameTimer):
    def __init__(self, logic: ILogic) -> None:
        self.__logic = logic
        self.__pool = ThreadPoolExecutor(20)

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]:
        return self.__pool.submit(self.__logic.Move, timeInMilliseconds, angle)

    # 向特定方向移动

    def MoveRight(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi * 0.5)

    def MoveLeft(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi * 1.5)

    def MoveUp(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi)

    def MoveDown(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, 0)

    def Attack(self, angle: float) -> Future[bool]:
        return self.__pool.submit(self.__logic.Attack, angle)

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.PickProp, propType)

    def UseProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseProp, propType)

    def ThrowProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.ThrowProp, propType)

    def UseSkill(self, skillID: int) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseSkill, skillID)

    # 与地图交互相关
    def OpenDoor(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.OpenDoor)

    def CloseDoor(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.CloseDoor)

    def SkipWindow(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.SkipWindow)

    def StartOpenGate(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartOpenGate)

    def StartOpenChest(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartOpenChest)

    def EndAllAction(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.EndAllAction)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: Union[str, bytes]) -> Future[bool]:
        return self.__pool.submit(self.__logic.SendMessage, toID, message)

    def HaveMessage(self) -> bool:
        return self.__logic.HaveMessage()

    def GetMessage(self) -> Tuple[int, Union[str, bytes]]:
        return self.__logic.GetMessage()

    # 等待下一帧

    def Wait(self) -> bool:
        if self.__logic.GetCounter() == -1:
            return False
        else:
            return self.__logic.WaitThread()

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        return self.__logic.GetCounter()

    def GetPlayerGUIDs(self) -> List[int]:
        return self.__logic.GetPlayerGUIDs()

    def GetTrickers(self) -> List[THUAI6.Tricker]:
        return self.__logic.GetTrickers()

    def GetStudents(self) -> List[THUAI6.Student]:
        return self.__logic.GetStudents()

    def GetProps(self) -> List[THUAI6.Prop]:
        return self.__logic.GetProps()

    def GetBullets(self) -> List[THUAI6.Bullet]:
        return self.__logic.GetBullets()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    def IsDoorOpen(self, cellX: int, cellY: int) -> bool:
        return self.__logic.IsDoorOpen(cellX, cellY)

    def GetChestProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetChestProgress(cellX, cellY)

    def GetGateProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetGateProgress(cellX, cellY)

    def GetClassroomProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetClassroomProgress(cellX, cellY)

    def GetDoorProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetDoorProgress(cellX, cellY)

    def GetHiddenGateState(self, cellX: int, cellY: int) -> THUAI6.HiddenGateState:
        return self.__logic.GetHiddenGateState(cellX, cellY)

    def GetGameInfo(self) -> THUAI6.GameInfo:
        return self.__logic.GetGameInfo()

    def HaveView(self, gridX: int, gridY: int) -> bool:
        return self.__logic.HaveView(
            gridX,
            gridY,
            self.GetSelfInfo().x,
            self.GetSelfInfo().y,
            self.GetSelfInfo().viewRange,
        )

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def Print(self, cont: str) -> None:
        pass

    def PrintStudent(self) -> None:
        pass

    def PrintTricker(self) -> None:
        pass

    def PrintProp(self) -> None:
        pass

    def PrintSelfInfo(self) -> None:
        pass

    # 人类阵营的特殊函数

    def Graduate(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.Graduate)

    def StartLearning(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartLearning)

    def StartEncourageMate(self, mateID: int) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartEncourageMate, mateID)

    def StartRouseMate(self, mateID: int) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartRouseMate, mateID)

    def GetSelfInfo(self) -> THUAI6.Student:
        return cast(THUAI6.Student, self.__logic.GetSelfInfo())

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        ai.StudentPlay(self)


class TrickerAPI(ITrickerAPI, IGameTimer):
    def __init__(self, logic: ILogic) -> None:
        self.__logic = logic
        self.__pool = ThreadPoolExecutor(20)

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]:
        return self.__pool.submit(self.__logic.Move, timeInMilliseconds, angle)

    # 向特定方向移动

    def MoveRight(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi * 0.5)

    def MoveLeft(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi * 1.5)

    def MoveUp(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi)

    def MoveDown(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, 0)

    def Attack(self, angle: float) -> Future[bool]:
        return self.__pool.submit(self.__logic.Attack, angle)

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.PickProp, propType)

    def UseProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseProp, propType)

    def ThrowProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.ThrowProp, propType)

    def UseSkill(self, skillID: int) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseSkill, skillID)

    # 与地图交互相关
    def OpenDoor(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.OpenDoor)

    def CloseDoor(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.CloseDoor)

    def SkipWindow(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.SkipWindow)

    def StartOpenGate(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartOpenGate)

    def StartOpenChest(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartOpenChest)

    def EndAllAction(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.EndAllAction)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: Union[str, bytes]) -> Future[bool]:
        return self.__pool.submit(self.__logic.SendMessage, toID, message)

    def HaveMessage(self) -> bool:
        return self.__logic.HaveMessage()

    def GetMessage(self) -> Tuple[int, Union[str, bytes]]:
        return self.__logic.GetMessage()

    # 等待下一帧

    def Wait(self) -> bool:
        if self.__logic.GetCounter() == -1:
            return False
        else:
            return self.__logic.WaitThread()

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        return self.__logic.GetCounter()

    def GetPlayerGUIDs(self) -> List[int]:
        return self.__logic.GetPlayerGUIDs()

    def GetTrickers(self) -> List[THUAI6.Tricker]:
        return self.__logic.GetTrickers()

    def GetStudents(self) -> List[THUAI6.Student]:
        return self.__logic.GetStudents()

    def GetProps(self) -> List[THUAI6.Prop]:
        return self.__logic.GetProps()

    def GetBullets(self) -> List[THUAI6.Bullet]:
        return self.__logic.GetBullets()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    def IsDoorOpen(self, cellX: int, cellY: int) -> bool:
        return self.__logic.IsDoorOpen(cellX, cellY)

    def GetChestProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetChestProgress(cellX, cellY)

    def GetGateProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetGateProgress(cellX, cellY)

    def GetClassroomProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetClassroomProgress(cellX, cellY)

    def GetDoorProgress(self, cellX: int, cellY: int) -> int:
        return self.__logic.GetDoorProgress(cellX, cellY)

    def GetHiddenGateState(self, cellX: int, cellY: int) -> THUAI6.HiddenGateState:
        return self.__logic.GetHiddenGateState(cellX, cellY)

    def GetGameInfo(self) -> THUAI6.GameInfo:
        return self.__logic.GetGameInfo()

    def HaveView(self, gridX: int, gridY: int) -> bool:
        return self.__logic.HaveView(
            gridX,
            gridY,
            self.GetSelfInfo().x,
            self.GetSelfInfo().y,
            self.GetSelfInfo().viewRange,
        )

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def Print(self, cont: str) -> None:
        pass

    def PrintStudent(self) -> None:
        pass

    def PrintTricker(self) -> None:
        pass

    def PrintProp(self) -> None:
        pass

    def PrintSelfInfo(self) -> None:
        pass

    # 屠夫阵营的特殊函数

    def GetSelfInfo(self) -> THUAI6.Tricker:
        return cast(THUAI6.Tricker, self.__logic.GetSelfInfo())

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        ai.TrickerPlay(self)
