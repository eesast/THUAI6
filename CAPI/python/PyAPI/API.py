import PyAPI.structures as THUAI6
from PyAPI.Interface import ILogic, IStudentAPI, ITrickerAPI, IGameTimer, IAI
from math import pi
from concurrent.futures import ThreadPoolExecutor, Future
from typing import List, Union


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
        return self.Move(timeInMilliseconds, 0)

    def MoveDown(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi)

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.PickProp, propType)

    def UseProp(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseProp)

    def UseSkill(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseSkill)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> Future[bool]:
        return self.__pool.submit(self.__logic.SendMessage, toID, message)

    def HaveMessage(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.HaveMessage)

    def GetMessage(self) -> Future[tuple[int, str]]:
        return self.__pool.submit(self.__logic.GetMessage)

    # 等待下一帧

    def Wait(self) -> Future[bool]:
        if self.__logic.GetCounter() == -1:
            return self.__pool.submit(lambda: False)
        else:
            return self.__pool.submit(self.__logic.WaitThread)

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

    def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]:
        return self.__logic.GetSelfInfo()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

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

    def EndLearning(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.EndLearning)

    def StartHelpMate(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartHelpMate)

    def EndHelpMate(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.EndHelpMate)

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        pass


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
        return self.Move(timeInMilliseconds, 0)

    def MoveDown(self, timeInMilliseconds: int) -> Future[bool]:
        return self.Move(timeInMilliseconds, pi)

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> Future[bool]:
        return self.__pool.submit(self.__logic.PickProp, propType)

    def UseProp(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseProp)

    def UseSkill(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.UseSkill)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> Future[bool]:
        return self.__pool.submit(self.__logic.SendMessage, toID, message)

    def HaveMessage(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.HaveMessage)

    def GetMessage(self) -> Future[tuple[int, str]]:
        return self.__pool.submit(self.__logic.GetMessage)

    # 等待下一帧

    def Wait(self) -> Future[bool]:
        if self.__logic.GetCounter() == -1:
            return self.__pool.submit(lambda: False)
        else:
            return self.__pool.submit(self.__logic.WaitThread)

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

    def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]:
        return self.__logic.GetSelfInfo()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def PrintStudent(self) -> None:
        pass

    def PrintTricker(self) -> None:
        pass

    def PrintProp(self) -> None:
        pass

    def PrintSelfInfo(self) -> None:
        pass

    # 屠夫阵营的特殊函数

    def Trick(self, angle: float) -> Future[bool]:
        return self.__pool.submit(self.__logic.Trick, angle)

    def StartExam(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.StartExam)

    def EndExam(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.EndExam)

    def MakeFail(self) -> Future[bool]:
        return self.__pool.submit(self.__logic.MakeFail)

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        pass
