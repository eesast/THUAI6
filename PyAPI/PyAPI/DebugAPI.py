from math import pi
from concurrent.futures import ThreadPoolExecutor, Future
from typing import List, Union
import logging
import os
import datetime

import PyAPI.structures as THUAI6
from PyAPI.Interface import ILogic, IStudentAPI, ITrickerAPI, IGameTimer, IAI


class StudentDebugAPI(IStudentAPI, IGameTimer):

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
        for student in self.__logic.GetStudents():
            self.__logger.info("******Student Info******")
            self.__logger.info(
                f"playerID={student.playerID}, GUID={student.guid}, x={student.x}, y={student.y}")
            self.__logger.info(
                f"speed={student.speed}, view range={student.viewRange}, skill time={student.timeUntilSkillAvailable}, prop={student.prop.name}, place={student.place.name}")
            self.__logger.info(
                f"state={student.state.name}, determination={student.determination}, fail time={student.failTime}")
            self.__logger.info("buff=")
            studentBuff = ""
            for buff in student.buff:
                studentBuff += buff.name + ", "
            self.__logger.info(studentBuff)
            self.__logger.info("**********************")

    def PrintTricker(self) -> None:
        for tricker in self.__logic.GetTrickers():
            self.__logger.info("******Tricker Info******")
            self.__logger.info(
                f"playerID={tricker.playerID}, GUID={tricker.guid}, x={tricker.x}, y={tricker.y}")
            self.__logger.info(
                f"speed={tricker.speed}, view range={tricker.viewRange}, skill time={tricker.timeUntilSkillAvailable}, prop={tricker.prop.name}, place={tricker.place.name}")
            self.__logger.info(
                f"damage={tricker.damage}, movable={tricker.movable}")
            self.__logger.info("buff=")
            trickerBuff = ""
            for buff in tricker.buff:
                trickerBuff += buff.name + ", "
            self.__logger.info(trickerBuff)
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
        if isinstance(mySelf, THUAI6.Student):
            self.__logger.info(
                f"state={mySelf.state.name}, determination={mySelf.determination},fail num={mySelf.failNum}, fail time={mySelf.failTime}, emo time={mySelf.emoTime}")
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

    def Graduate(self) -> Future[bool]:
        self.__logger.info(
            f"Graduate: called at {self.__GetTime()}ms")

        def logGraduate() -> bool:
            result = self.__logic.Graduate()
            if not result:
                self.__logger.warning(
                    f"Graduate: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logGraduate)

    def StartLearning(self) -> Future[bool]:
        self.__logger.info(
            f"StartLearning: called at {self.__GetTime()}ms")

        def logStart() -> bool:
            result = self.__logic.StartLearning()
            if not result:
                self.__logger.warning(
                    f"StartLearning: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logStart)

    def EndLearning(self) -> Future[bool]:
        self.__logger.info(
            f"EndLearning: called at {self.__GetTime()}ms")

        def logEnd() -> bool:
            result = self.__logic.EndLearning()
            if not result:
                self.__logger.warning(
                    f"EndLearning: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logEnd)

    def StartHelpMate(self) -> Future[bool]:
        self.__logger.info(
            f"StartHelpMate: called at {self.__GetTime()}ms")

        def logStart() -> bool:
            result = self.__logic.StartHelpMate()
            if not result:
                self.__logger.warning(
                    f"StartHelpMate: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logStart)

    def EndHelpMate(self) -> Future[bool]:
        self.__logger.info(
            f"EndHelpMate: called at {self.__GetTime()}ms")

        def logEnd() -> bool:
            result = self.__logic.EndHelpMate()
            if not result:
                self.__logger.warning(
                    f"EndHelpMate: failed at {self.__GetTime()}ms")
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


class TrickerDebugAPI(ITrickerAPI, IGameTimer):

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
        for student in self.__logic.GetStudents():
            self.__logger.info("******Student Info******")
            self.__logger.info(
                f"playerID={student.playerID}, GUID={student.guid}, x={student.x}, y={student.y}")
            self.__logger.info(
                f"speed={student.speed}, view range={student.viewRange}, skill time={student.timeUntilSkillAvailable}, prop={student.prop.name}, place={student.place.name}")
            self.__logger.info(
                f"state={student.state.name}, determination={student.determination}, fail num={student.failNum}, fail time={student.failTime}, emo time={student.emoTime}")
            self.__logger.info("buff=")
            studentBuff = ""
            for buff in student.buff:
                studentBuff += buff.name + ", "
            self.__logger.info(studentBuff)
            self.__logger.info("**********************")

    def PrintTricker(self) -> None:
        for tricker in self.__logic.GetTrickers():
            self.__logger.info("******Tricker Info******")
            self.__logger.info(
                f"playerID={tricker.playerID}, GUID={tricker.guid}, x={tricker.x}, y={tricker.y}")
            self.__logger.info(
                f"speed={tricker.speed}, view range={tricker.viewRange}, skill time={tricker.timeUntilSkillAvailable}, prop={tricker.prop.name}, place={tricker.place.name}")
            self.__logger.info(
                f"damage={tricker.damage}, movable={tricker.movable}")
            self.__logger.info("buff=")
            trickerBuff = ""
            for buff in tricker.buff:
                trickerBuff += buff.name + ", "
            self.__logger.info(trickerBuff)
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
        if isinstance(mySelf, THUAI6.Student):
            self.__logger.info(
                f"state={mySelf.state.name}, determination={mySelf.determination}, fail time={mySelf.failTime}")
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

    def Trick(self, angle: float) -> Future[bool]:
        self.__logger.info(
            f"Trick: angle = {angle}, called at {self.__GetTime()}ms")

        def logTrick() -> bool:
            result = self.__logic.Trick(angle)
            if not result:
                self.__logger.warning(
                    f"Trick: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logTrick)

    def StartExam(self) -> Future[bool]:
        self.__logger.info(
            f"StartExam: called at {self.__GetTime()}ms")

        def logCarry() -> bool:
            result = self.__logic.StartExam()
            if not result:
                self.__logger.warning(
                    f"StartExam: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logCarry)

    def EndExam(self) -> Future[bool]:
        self.__logger.info(
            f"EndExam: called at {self.__GetTime()}ms")

        def logRelease() -> bool:
            result = self.__logic.EndExam()
            if not result:
                self.__logger.warning(
                    f"EndExam: failed at {self.__GetTime()}ms")
            return result

        return self.__pool.submit(logRelease)

    def MakeFail(self) -> Future[bool]:
        self.__logger.info(
            f"MakeFail: called at {self.__GetTime()}ms")

        def logHang() -> bool:
            result = self.__logic.MakeFail()
            if not result:
                self.__logger.warning(
                    f"MakeFail: failed at {self.__GetTime()}ms")
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
