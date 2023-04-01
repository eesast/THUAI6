import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final, cast

import time


class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return False

    # 选手必须修改该函数的返回值来选择自己的阵营
    @staticmethod
    def playerType() -> THUAI6.PlayerType:
        return THUAI6.PlayerType.StudentPlayer

    # 选手需要将两个都定义，本份代码中不选择的阵营任意定义即可
    @staticmethod
    def studentType() -> THUAI6.StudentType:
        return THUAI6.StudentType.Athlete

    @staticmethod
    def trickerType() -> THUAI6.TrickerType:
        return THUAI6.TrickerType.Assassin


# 辅助函数
numOfGridPerCell: Final[int] = 1000


class AssistFunction:

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell


arrive: bool = False


class AI(IAI):
    # 选手在这里实现自己的逻辑，要求和上面选择的阵营保持一致
    def StudentPlay(self, api: IStudentAPI) -> None:
        api.Attack(float('nan'))
        time.sleep(0.5)
        api.PrintSelfInfo()
        # api.SendMessage(4, "Hello World!")
        # api.PrintSelfInfo()
        # global arrive
        # if not arrive:
        #     if api.GetSelfInfo().x < 25500:
        #         api.MoveDown(50)
        #         return
        #     if api.GetSelfInfo().y > 10500:
        #         api.MoveLeft(50)
        #         return
        #     arrive = True
        # else:
        #     api.SkipWindow()
        #     # time.sleep(1)

        # api.PrintSelfInfo()

        # if api.GetSelfInfo().y < 18500:
        #     api.MoveRight(50)
        #     return
        # api.StartLearning()

        # if api.GetSelfInfo().y > 7000:
        #     api.MoveLeft(50)
        #     return
        # if api.GetSelfInfo().x > 20500:
        #     api.MoveUp(50)
        #     return
        # if api.GetSelfInfo().y > 4500:
        #     api.MoveLeft(50)
        #     return

        api.PrintTricker()

        return

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        return
