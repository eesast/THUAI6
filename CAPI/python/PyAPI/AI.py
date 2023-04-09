import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final, cast
import queue

import time


class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return True

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


path = []
cur = 0
fixedclass = []


class AI(IAI):
    # 选手在这里实现自己的逻辑，要求和上面选择的阵营保持一致
    def StudentPlay(self, api: IStudentAPI) -> None:
        # global fixedclass
        # selfInfo = api.GetSelfInfo()
        # available = [THUAI6.PlaceType.Land,
        #              THUAI6.PlaceType.Grass, THUAI6.PlaceType.Door3, THUAI6.PlaceType.Door6, THUAI6.PlaceType.Door5, THUAI6.PlaceType.Gate]

        # def bfs(x, y):
        #     if api.GetPlaceType(x, y) not in available:
        #         return []

        #     def GetSuccessors(x, y):
        #         successors = []
        #         if x > 0 and api.GetPlaceType(x - 1, y) in available:
        #             successors.append((x - 1, y))
        #         if x < 49 and api.GetPlaceType(x + 1, y) in available:
        #             successors.append((x + 1, y))
        #         if y > 0 and api.GetPlaceType(x, y - 1) in available:
        #             successors.append((x, y - 1))
        #         if y < 49 and api.GetPlaceType(x, y + 1) in available:
        #             successors.append((x, y + 1))
        #         return successors
        #     selfX = AssistFunction.GridToCell(api.GetSelfInfo().x)
        #     selfY = AssistFunction.GridToCell(api.GetSelfInfo().y)
        #     frontier = queue.Queue()
        #     frontier.put((selfX, selfY, []))
        #     visited = []
        #     while not frontier.empty():
        #         currentX, currentY, path = frontier.get()
        #         if currentX == x and currentY == y:
        #             return path
        #         for nextX, nextY in GetSuccessors(currentX, currentY):
        #             if (nextX, nextY) not in visited:
        #                 visited.append((nextX, nextY))
        #                 frontier.put((nextX, nextY, path + [(nextX, nextY)]))
        #     return []

        # def GoTo(x, y):
        #     global path, cur
        #     if path != [] and cur < len(path):
        #         selfX = api.GetSelfInfo().x
        #         selfY = api.GetSelfInfo().y
        #         nextX, nextY = path[cur]
        #         nextX = AssistFunction.CellToGrid(nextX)
        #         nextY = AssistFunction.CellToGrid(nextY)
        #         if selfX < nextX - 100:
        #             api.MoveDown(10)
        #             time.sleep(0.01)
        #             return
        #         if selfX > nextX + 100:
        #             api.MoveUp(10)
        #             time.sleep(0.01)
        #             return
        #         if selfY < nextY - 100:
        #             api.MoveRight(10)
        #             time.sleep(0.01)
        #             return
        #         if selfY > nextY + 100:
        #             api.MoveLeft(10)
        #             time.sleep(0.01)
        #             return
        #         cur += 1
        #         return
        #     else:
        #         path = bfs(x, y)
        #         cur = 0
        #         return

        # if (AssistFunction.GridToCell(api.GetSelfInfo().x), AssistFunction.GridToCell(api.GetSelfInfo().y)) == (6, 6) and api.GetGateProgress(5, 6) < 18000:
        #     api.StartOpenGate()
        #     return

        # if (AssistFunction.GridToCell(api.GetSelfInfo().x), AssistFunction.GridToCell(api.GetSelfInfo().y)) == (6, 6) and api.GetGateProgress(5, 6) >= 18000:
        #     api.Graduate()
        #     return

        # if len(fixedclass) == 7:
        #     GoTo(6, 6)
        #     return

        # if api.GetPlaceType(AssistFunction.GridToCell(api.GetSelfInfo().x) + 1, AssistFunction.GridToCell(api.GetSelfInfo().y)) == THUAI6.PlaceType.ClassRoom:
        #     api.Print("Trying to fix!")
        #     if api.GetClassroomProgress(AssistFunction.GridToCell(api.GetSelfInfo().x) + 1, AssistFunction.GridToCell(api.GetSelfInfo().y)) < 103000:
        #         api.StartLearning()
        #         return
        #     else:
        #         if (AssistFunction.GridToCell(api.GetSelfInfo().x) + 1, AssistFunction.GridToCell(api.GetSelfInfo().y)) not in fixedclass:
        #             fixedclass.append(
        #                 (AssistFunction.GridToCell(api.GetSelfInfo().x) + 1, AssistFunction.GridToCell(api.GetSelfInfo().y)))

        # for i in range(50):
        #     for j in range(50):
        #         if api.GetPlaceType(i, j) == THUAI6.PlaceType.ClassRoom and (i, j) not in fixedclass:
        #             if api.GetPlaceType(i - 1, j) in available:
        #                 GoTo(i - 1, j)
        #                 return
        api.PrintTricker()

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        return
