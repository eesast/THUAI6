from math import pi
from concurrent.futures import ThreadPoolExecutor, Future
from typing import List, Union
import logging
import os
import datetime

import PyAPI.structures as THUAI6
from PyAPI.Interface import ILogic, IHumanAPI, IButcherAPI, IGameTimer, IAI


class HumanDebugAPI(IHumanAPI, IGameTimer):

    def __init__(self, logic: ILogic, file: bool, screen: bool, warnOnly: bool, playerID: int) -> None:
        self.__logic = logic
        self.__pool = ThreadPoolExecutor(20)
        self.__startPoint = datetime.datetime.now()
        self.__logger = logging.getLogger("api " + str(playerID))
        self.__logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            "[%(name)s] [%(asctime)s] [%(levelname)s] %(message)s", "%H:%M:%S.%e")
        # 确保文件存在
        if not os.path.exists(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/logs"):
            os.makedirs(os.path.dirname(os.path.dirname(
                os.path.realpath(__file__))) + "/logs")

        fileHandler = logging.FileHandler(os.path.dirname(
            os.path.dirname(os.path.realpath(__file__))) + "/logs/api-" + str(playerID) + "-log.txt")
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

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]:
        self.__logger.info(
            f"Move: timeInMilliseconds = {timeInMilliseconds}, angle = {angle}, called at {self.__GetTime()}ms")

        def logMove() -> bool:
            result = self.__logic.Move(timeInMilliseconds, angle)
            if not result:
                self.__logger.warning(
                    f"Move: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logMove)

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
        self.__logger.info(
            f"PickProp: prop = {propType.name}, called at {self.__GetTime()}ms")

        def logPick() -> bool:
            result = self.__logic.PickProp(propType)
            if not result:
                self.__logger.warning(
                    f"PickProp: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logPick)

    def UseProp(self) -> Future[bool]:
        self.__logger.info(
            f"UseProp: called at {self.__GetTime()}ms")

        def logUse() -> bool:
            result = self.__logic.UseProp()
            if not result:
                self.__logger.warning(
                    f"UseProp: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logUse)

    def UseSkill(self) -> Future[bool]:
        self.__logger.info(
            f"UseSkill: called at {self.__GetTime()}ms")

        def logUse() -> bool:
            result = self.__logic.UseSkill()
            if not result:
                self.__logger.warning(
                    f"UseSkill: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logUse)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> Future[bool]:
        self.__logger.info(
            f"SendMessage: toID = {toID}, message = {message}, called at {self.__GetTime()}ms")

        def logSend() -> bool:
            result = self.__logic.SendMessage(toID, message)
            if not result:
                self.__logger.warning(
                    f"SendMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logSend)

    def HaveMessage(self) -> Future[bool]:
        self.__logger.info(
            f"HaveMessage: called at {self.__GetTime()}ms")

        def logHave() -> bool:
            result = self.__logic.HaveMessage()
            if not result:
                self.__logger.warning(
                    f"HaveMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logHave)

    def GetMessage(self) -> Future[tuple[int, str]]:
        self.__logger.info(
            f"GetMessage: called at {self.__GetTime()}ms")

        def logGet() -> tuple[int, str]:
            result = self.__logic.GetMessage()
            if result[0] == -1:
                self.__logger.warning(
                    f"GetMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logGet)

    # 等待下一帧

    def Wait(self) -> Future[bool]:
        self.__logger.info(
            f"Wait: called at {self.__GetTime()}ms")
        if self.__logic.GetCounter() == -1:
            return self.__pool.submit(lambda: False)
        else:
            return self.__pool.submit(self.__logic.WaitThread)

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        return self.__logic.GetCounter()

    def GetPlayerGUIDs(self) -> List[int]:
        return self.__logic.GetPlayerGUIDs()

    def GetButchers(self) -> List[THUAI6.Butcher]:
        return self.__logic.GetButchers()

    def GetHumans(self) -> List[THUAI6.Human]:
        return self.__logic.GetHumans()

    def GetProps(self) -> List[THUAI6.Prop]:
        return self.__logic.GetProps()

    def GetSelfInfo(self) -> Union[THUAI6.Human, THUAI6.Butcher]:
        return self.__logic.GetSelfInfo()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def PrintHuman(self) -> None:
        for human in self.__logic.GetHumans():
            self.__logger.info("******Human Info******")
            self.__logger.info(
                f"playerID={human.playerID}, GUID={human.guid}, x={human.x}, y={human.y}")
            self.__logger.info(
                f"speed={human.speed}, view range={human.viewRange}, skill time={human.timeUntilSkillAvailable}, prop={human.prop.name}, place={human.place.name}")
            self.__logger.info(
                f"state={human.state.name}, life={human.life}, hanged time={human.hangedTime}")
            self.__logger.info("buff=")
            humanBuff = ""
            for buff in human.buff:
                humanBuff += buff.name + ", "
            self.__logger.info(humanBuff)
            self.__logger.info("**********************")

    def PrintButcher(self) -> None:
        for butcher in self.__logic.GetButchers():
            self.__logger.info("******Butcher Info******")
            self.__logger.info(
                f"playerID={butcher.playerID}, GUID={butcher.guid}, x={butcher.x}, y={butcher.y}")
            self.__logger.info(
                f"speed={butcher.speed}, view range={butcher.viewRange}, skill time={butcher.timeUntilSkillAvailable}, prop={butcher.prop.name}, place={butcher.place.name}")
            self.__logger.info(
                f"damage={butcher.damage}, movable={butcher.movable}")
            self.__logger.info("buff=")
            butcherBuff = ""
            for buff in butcher.buff:
                butcherBuff += buff.name + ", "
            self.__logger.info(butcherBuff)
            self.__logger.info("************************")

    def PrintProp(self) -> None:
        for prop in self.__logic.GetProps():
            self.__logger.info("******Prop Info******")
            self.__logger.info(
                f"GUID={prop.guid}, x={prop.x}, y={prop.y}, place={prop.place.name}, is moving={prop.isMoving}")
            self.__logger.info("*********************")

    def PrintSelfInfo(self) -> None:
        mySelf = self.__logic.GetSelfInfo()
        self.__logger.info("******Self Info******")
        self.__logger.info(
            f"playerID={mySelf.playerID}, GUID={mySelf.guid}, x={mySelf.x}, y={mySelf.y}")
        self.__logger.info(
            f"speed={mySelf.speed}, view range={mySelf.viewRange}, skill time={mySelf.timeUntilSkillAvailable}, prop={mySelf.prop.name}, place={mySelf.place.name}")
        if isinstance(mySelf, THUAI6.Human):
            self.__logger.info(
                f"state={mySelf.state.name}, life={mySelf.life}, hanged time={mySelf.hangedTime}")
        else:
            self.__logger.info(
                f"damage={mySelf.damage}, movable={mySelf.movable}")
        self.__logger.info("buff=")
        mySelfBuff = ""
        for buff in mySelf.buff:
            mySelfBuff += buff.name + ", "
        self.__logger.info(mySelfBuff)
        self.__logger.info("*********************")

    # 人类阵营的特殊函数

    def Escape(self) -> Future[bool]:
        self.__logger.info(
            f"Escape: called at {self.__GetTime()}ms")

        def logEscape() -> bool:
            result = self.__logic.Escape()
            if not result:
                self.__logger.warning(
                    f"Escape: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logEscape)

    def StartFixMachine(self) -> Future[bool]:
        self.__logger.info(
            f"StartFixMachine: called at {self.__GetTime()}ms")

        def logStart() -> bool:
            result = self.__logic.StartFixMachine()
            if not result:
                self.__logger.warning(
                    f"StartFixMachine: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logStart)

    def EndFixMachine(self) -> Future[bool]:
        self.__logger.info(
            f"EndFixMachine: called at {self.__GetTime()}ms")

        def logEnd() -> bool:
            result = self.__logic.EndFixMachine()
            if not result:
                self.__logger.warning(
                    f"EndFixMachine: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logEnd)

    def StartSaveHuman(self) -> Future[bool]:
        self.__logger.info(
            f"StartSaveHuman: called at {self.__GetTime()}ms")

        def logStart() -> bool:
            result = self.__logic.StartSaveHuman()
            if not result:
                self.__logger.warning(
                    f"StartSaveHuman: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logStart)

    def EndSaveHuman(self) -> Future[bool]:
        self.__logger.info(
            f"EndSaveHuman: called at {self.__GetTime()}ms")

        def logEnd() -> bool:
            result = self.__logic.EndSaveHuman()
            if not result:
                self.__logger.warning(
                    f"EndSaveHuman: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logEnd)

    # Timer用

    def __GetTime(self) -> int:
        return int((datetime.datetime.now() - self.__startPoint).total_seconds() * 1000)

    def StartTimer(self) -> None:
        self.__startPoint = datetime.datetime.now()
        self.__logger.info("=== AI.play() ===")
        self.__logger.info(f"StartTimer: {self.__startPoint.time()}")

    def EndTimer(self) -> None:
        self.__logger.info(f"Time elapsed: {self.__GetTime()}ms")

    def Play(self, ai: IAI) -> None:
        ai.play(self)


class ButcherDebugAPI(IButcherAPI, IGameTimer):

    def __init__(self, logic: ILogic, file: bool, screen: bool, warnOnly: bool, playerID: int) -> None:
        self.__logic = logic
        self.__pool = ThreadPoolExecutor(20)
        self.__logger = logging.getLogger("api " + str(playerID))
        self.__logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter(
            "[%(name)s] [%(asctime)s] [%(levelname)s] %(message)s", "%H:%M:%S.%e")
        # 确保文件存在
        if not os.path.exists(os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/logs"):
            os.makedirs(os.path.dirname(os.path.dirname(
                os.path.realpath(__file__))) + "/logs")

        fileHandler = logging.FileHandler(os.path.dirname(
            os.path.dirname(os.path.realpath(__file__))) + "/logs/api-" + str(playerID) + "-log.txt")
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

    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]:
        self.__logger.info(
            f"Move: timeInMilliseconds = {timeInMilliseconds}, angle = {angle}, called at {self.__GetTime()}ms")

        def logMove() -> bool:
            result = self.__logic.Move(timeInMilliseconds, angle)
            if not result:
                self.__logger.warning(
                    f"Move: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logMove)

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
        self.__logger.info(
            f"PickProp: prop = {propType.name}, called at {self.__GetTime()}ms")

        def logPick() -> bool:
            result = self.__logic.PickProp(propType)
            if not result:
                self.__logger.warning(
                    f"PickProp: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logPick)

    def UseProp(self) -> Future[bool]:
        self.__logger.info(
            f"UseProp: called at {self.__GetTime()}ms")

        def logUse() -> bool:
            result = self.__logic.UseProp()
            if not result:
                self.__logger.warning(
                    f"UseProp: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logUse)

    def UseSkill(self) -> Future[bool]:
        self.__logger.info(
            f"UseSkill: called at {self.__GetTime()}ms")

        def logUse() -> bool:
            result = self.__logic.UseSkill()
            if not result:
                self.__logger.warning(
                    f"UseSkill: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logUse)

    # 消息相关，接收消息时无消息则返回(-1, '')

    def SendMessage(self, toID: int, message: str) -> Future[bool]:
        self.__logger.info(
            f"SendMessage: toID = {toID}, message = {message}, called at {self.__GetTime()}ms")

        def logSend() -> bool:
            result = self.__logic.SendMessage(toID, message)
            if not result:
                self.__logger.warning(
                    f"SendMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logSend)

    def HaveMessage(self) -> Future[bool]:
        self.__logger.info(
            f"HaveMessage: called at {self.__GetTime()}ms")

        def logHave() -> bool:
            result = self.__logic.HaveMessage()
            if not result:
                self.__logger.warning(
                    f"HaveMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logHave)

    def GetMessage(self) -> Future[tuple[int, str]]:
        self.__logger.info(
            f"GetMessage: called at {self.__GetTime()}ms")

        def logGet() -> tuple[int, str]:
            result = self.__logic.GetMessage()
            if result[0] == -1:
                self.__logger.warning(
                    f"GetMessage: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logGet)

    # 等待下一帧

    def Wait(self) -> Future[bool]:
        self.__logger.info(
            f"Wait: called at {self.__GetTime()}ms")
        if self.__logic.GetCounter() == -1:
            return self.__pool.submit(lambda: False)
        else:
            return self.__pool.submit(self.__logic.WaitThread)

    # 获取各类游戏中的消息

    def GetFrameCount(self) -> int:
        return self.__logic.GetCounter()

    def GetPlayerGUIDs(self) -> List[int]:
        return self.__logic.GetPlayerGUIDs()

    def GetButchers(self) -> List[THUAI6.Butcher]:
        return self.__logic.GetButchers()

    def GetHumans(self) -> List[THUAI6.Human]:
        return self.__logic.GetHumans()

    def GetProps(self) -> List[THUAI6.Prop]:
        return self.__logic.GetProps()

    def GetSelfInfo(self) -> Union[THUAI6.Human, THUAI6.Butcher]:
        return self.__logic.GetSelfInfo()

    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        return self.__logic.GetFullMap()

    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        return self.__logic.GetPlaceType(cellX, cellY)

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    def PrintHuman(self) -> None:
        for human in self.__logic.GetHumans():
            self.__logger.info("******Human Info******")
            self.__logger.info(
                f"playerID={human.playerID}, GUID={human.guid}, x={human.x}, y={human.y}")
            self.__logger.info(
                f"speed={human.speed}, view range={human.viewRange}, skill time={human.timeUntilSkillAvailable}, prop={human.prop.name}, place={human.place.name}")
            self.__logger.info(
                f"state={human.state.name}, life={human.life}, hanged time={human.hangedTime}")
            self.__logger.info("buff=")
            humanBuff = ""
            for buff in human.buff:
                humanBuff += buff.name + ", "
            self.__logger.info(humanBuff)
            self.__logger.info("**********************")

    def PrintButcher(self) -> None:
        for butcher in self.__logic.GetButchers():
            self.__logger.info("******Butcher Info******")
            self.__logger.info(
                f"playerID={butcher.playerID}, GUID={butcher.guid}, x={butcher.x}, y={butcher.y}")
            self.__logger.info(
                f"speed={butcher.speed}, view range={butcher.viewRange}, skill time={butcher.timeUntilSkillAvailable}, prop={butcher.prop.name}, place={butcher.place.name}")
            self.__logger.info(
                f"damage={butcher.damage}, movable={butcher.movable}")
            self.__logger.info("buff=")
            butcherBuff = ""
            for buff in butcher.buff:
                butcherBuff += buff.name + ", "
            self.__logger.info(butcherBuff)
            self.__logger.info("************************")

    def PrintProp(self) -> None:
        for prop in self.__logic.GetProps():
            self.__logger.info("******Prop Info******")
            self.__logger.info(
                f"GUID={prop.guid}, x={prop.x}, y={prop.y}, place={prop.place.name}, is moving={prop.isMoving}")
            self.__logger.info("*********************")

    def PrintSelfInfo(self) -> None:
        mySelf = self.__logic.GetSelfInfo()
        self.__logger.info("******Self Info******")
        self.__logger.info(
            f"playerID={mySelf.playerID}, GUID={mySelf.guid}, x={mySelf.x}, y={mySelf.y}")
        self.__logger.info(
            f"speed={mySelf.speed}, view range={mySelf.viewRange}, skill time={mySelf.timeUntilSkillAvailable}, prop={mySelf.prop.name}, place={mySelf.place.name}")
        if isinstance(mySelf, THUAI6.Human):
            self.__logger.info(
                f"state={mySelf.state.name}, life={mySelf.life}, hanged time={mySelf.hangedTime}")
        else:
            self.__logger.info(
                f"damage={mySelf.damage}, movable={mySelf.movable}")
        self.__logger.info("buff=")
        mySelfBuff = ""
        for buff in mySelf.buff:
            mySelfBuff += buff.name + ", "
        self.__logger.info(mySelfBuff)
        self.__logger.info("*********************")

    # 屠夫阵营的特殊函数

    def Attack(self, angle: float) -> Future[bool]:
        self.__logger.info(
            f"Attack: angle = {angle}, called at {self.__GetTime()}ms")

        def logAttack() -> bool:
            result = self.__logic.Attack(angle)
            if not result:
                self.__logger.warning(
                    f"Attack: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logAttack)

    def CarryHuman(self) -> Future[bool]:
        self.__logger.info(
            f"CarryHuman: called at {self.__GetTime()}ms")

        def logCarry() -> bool:
            result = self.__logic.CarryHuman()
            if not result:
                self.__logger.warning(
                    f"CarryHuman: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logCarry)

    def ReleaseHuman(self) -> Future[bool]:
        self.__logger.info(
            f"ReleaseHuman: called at {self.__GetTime()}ms")

        def logRelease() -> bool:
            result = self.__logic.ReleaseHuman()
            if not result:
                self.__logger.warning(
                    f"ReleaseHuman: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logRelease)

    def HangHuman(self) -> Future[bool]:
        self.__logger.info(
            f"HangHuman: called at {self.__GetTime()}ms")

        def logHang() -> bool:
            result = self.__logic.HangHuman()
            if not result:
                self.__logger.warning(
                    f"HangHuman: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logHang)

    # Timer用

    def __GetTime(self) -> int:
        return int((datetime.datetime.now() - self.__startPoint).total_seconds() * 1000)

    def StartTimer(self) -> None:
        self.__startPoint = datetime.datetime.now()
        self.__logger.info("=== AI.play() ===")
        self.__logger.info(f"StartTimer: {self.__startPoint.time()}")

    def EndTimer(self) -> None:
        self.__logger.info(f"Time elapsed: {self.__GetTime()}ms")

    def Play(self, ai: IAI) -> None:
        ai.play(self)
