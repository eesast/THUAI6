from Interface import ILogic, IHumanAPI, IButcherAPI, IGameTimer, IAI
from typing import List, Union
import structures as THUAI6


class HumanAPI(IHumanAPI, IGameTimer):

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def __init__(self, logic: ILogic):
        self.__logic = logic

    def Move(self, timeInMilliseconds: int, angle: float) -> bool:
        pass

    # 向特定方向移动

    def MoveRight(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveLeft(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveUp(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveDown(self, timeInMilliseconds: int) -> bool:
        pass

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        pass

    def UseProp(self) -> bool:
        pass

    def UseSkill(self) -> bool:
        pass

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> bool:
        pass

    def HaveMessage(self) -> bool:
        pass

    def GetMessage(self) -> tuple[int, str]:
        pass

    # 等待下一帧

    def Wait(self) -> bool:
        pass

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        pass

    def GetPlayerGUIDs(self) -> List[int]:
        pass

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

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        pass

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def PrintHuman(self) -> None:
        pass

    def PrintButcher(self) -> None:
        pass

    def PrintProp(self) -> None:
        pass

    def PrintSelfInfo(self) -> None:
        pass

    # 人类阵营的特殊函数

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

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        pass


class ButcherAPI(IButcherAPI, IGameTimer):

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def Move(self, timeInMilliseconds: int, angle: float) -> bool:
        pass

    # 向特定方向移动

    def MoveRight(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveLeft(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveUp(self, timeInMilliseconds: int) -> bool:
        pass

    def MoveDown(self, timeInMilliseconds: int) -> bool:
        pass

    # 道具和技能相关

    def PickProp(self, propType: THUAI6.PropType) -> bool:
        pass

    def UseProp(self) -> bool:
        pass

    def UseSkill(self) -> bool:
        pass

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> bool:
        pass

    def HaveMessage(self) -> bool:
        pass

    def GetMessage(self) -> tuple[int, str]:
        pass

    # 等待下一帧

    def Wait(self) -> bool:
        pass

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        pass

    def GetPlayerGUIDs(self) -> List[int]:
        pass

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

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        pass

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def PrintHuman(self) -> None:
        pass

    def PrintButcher(self) -> None:
        pass

    def PrintProp(self) -> None:
        pass

    def PrintSelfInfo(self) -> None:
        pass

    # 屠夫阵营的特殊函数

    def Attack(self, angle: float) -> bool:
        pass

    def CarryHuman(self) -> bool:
        pass

    def ReleaseHuman(self) -> bool:
        pass

    def HangHuman(self) -> bool:
        pass

    # Timer用

    def StartTimer(self) -> None:
        pass

    def EndTimer(self) -> None:
        pass

    def Play(self, ai: IAI) -> None:
        pass
