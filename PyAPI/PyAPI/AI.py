import PyAPI.structures as THUAI6
from PyAPI.Interface import IHumanAPI, IButcherAPI, IAI
from typing import Union


class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return False

    # 选手必须修改该函数的返回值来选择自己的阵营
    @staticmethod
    def playerType() -> THUAI6.PlayerType:
        return THUAI6.PlayerType.HumanPlayer

    # 选手需要将两个都定义，本份代码中不选择的阵营任意定义即可
    @staticmethod
    def humanType() -> THUAI6.HumanType:
        return THUAI6.HumanType.HumanType1

    @staticmethod
    def butcherType() -> THUAI6.ButcherType:
        return THUAI6.ButcherType.ButcherType1


class AI(IAI):
    def play(self, api: Union[IHumanAPI, IButcherAPI]) -> None:
        # 选手在这里实现自己的逻辑，要求和上面选择的阵营保持一致，否则会发生错误
        return
