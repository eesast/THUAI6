import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final


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
        return THUAI6.StudentType.StudentType1

    @staticmethod
    def trickerType() -> THUAI6.TrickerType:
        return THUAI6.TrickerType.TrickerType1


# 辅助函数
numOfGridPerCell: Final[int] = 1000


class AssistFunction:

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell


class AI(IAI):
    def play(self, api: Union[IStudentAPI, ITrickerAPI]) -> None:
        # 选手在这里实现自己的逻辑，要求和上面选择的阵营保持一致，否则会发生错误
        return
