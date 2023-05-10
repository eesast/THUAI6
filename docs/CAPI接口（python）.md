[toc]

# CAPI接口(python)

## 接口解释

### 主动指令
- 每帧最多发送 50 条主动指令
- EndAllAction 及 Move 指令调用数总和一帧内不超过 10 次

#### 移动

- `def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]`：移动，`timeInMilliseconds` 为移动时间，单位毫秒；`angleInRadian` 表示移动方向，单位弧度，使用极坐标，**竖直向下方向为 x 轴，水平向右方向为 y 轴**因为移动过程中你会受到多种干扰使得移动结果不符合你的预期；因此建议小步移动，边移动边考虑之后的行为。
  - 5ms 以内的移动指令会被禁止，你不应当使用过小的移动指令
- `def MoveRight(self, timeInMilliseconds: int) -> Future[bool]` 即向右移动, `MoveLeft`、`MoveDown`、`MoveUp` 同理  

#### 使用技能

- `def UseSkill(self, skillID: int) -> Future[bool]`：使用对应序号的主动技能

#### 人物

- `def EndAllAction(self) -> Future[bool]`：可以使不处在不可行动状态中的玩家终止当前行动
- 在指令仍在进行时，重复发出同一类型的交互指令和移动指令是无效的，你需要先发出 Stop 指令终止进行的指令
  - 实际上唤醒或勉励不同的人是有效的
- EndAllAction 及 Move 指令调用数总和一帧内不超过 10 次

#### 攻击

- `def Attack(self, angle: float) -> Future[bool]`：`angleInRadian` 为攻击方向

#### 学习与毕业

- `def StartLearning(self) -> Future[bool]`：在教室里开始做作业
- `def StartOpenGate(self) -> Future[bool]`：开始开启校门
- `def Graduate(self) -> Future[bool]`：从开启的校门或隐藏校门毕业。

#### 勉励与唤醒

- `def StartEncourageMate(self, mateID: int) -> Future[bool]`：勉励对应玩家 ID 的学生。
- `def StartRouseMate(self, mateID: int) -> Future[bool]`：唤醒对应玩家 ID 的沉迷的学生。

#### 地图互动

- `def OpenDoor(self) -> Future[bool]`：开门
- `def CloseDoor(self) -> Future[bool]`：关门
- `def SkipWindow(self) -> Future[bool]`：翻窗
- `def StartOpenChest(self) -> Future[bool]`：开箱

#### 道具

- `def PickProp(self, propType: THUAI6.PropType) -> Future[bool]`：捡起与自己处于同一个格子（cell）的道具。
- `def UseProp(self, propType: THUAI6.PropType) -> Future[bool]`：使用对应类型的道具
- `def ThrowProp(self, propType: THUAI6.PropType) -> Future[bool]`：将对应类型的道具扔在原地

### 信息获取

#### 队内信息

  - `def SendMessage(self, toID: int, message: Union[str, bytes]) -> Future[bool]`：给同队的队友发送消息,队友在下一帧收到。第一个参数指定发送的对象，第二个参数指定发送的内容，可以是Unicode字符串，也可以是二进制字符串；不得超过 256 字节。
  - `def HaveMessage(self) -> bool`：是否有队友发来的尚未接收的信息。
  - `def GetMessage(self) -> Tuple[int, Union[str, bytes]]`：按照消息发送顺序获取来自队友的信息，第一个参数为发送该消息的 PlayerID。
    > 需要注意，文本消息和二进制消息都会在这里接受，选手可以简单地使用 `isinstance()` 来判断消息类型。

#### 查询可视范围内的信息

  - `def GetStudents(self) -> List[THUAI6.Student]`：对于学生，返回所有学生的信息；对于捣蛋鬼，返回可视学生的信息。
  - `def GetTrickers(self) -> List[THUAI6.Tricker]`：返回所有可视捣蛋鬼的信息。
  - `def GetProps(self) -> List[THUAI6.Prop]`：返回所有可视道具的信息。
  - `def GetBullets(self) -> List[THUAI6.Bullet]`：返回所有可视子弹（攻击）的信息。
  - `def HaveView(self, gridX: int, gridY: int) -> bool`：判断坐标是否可见

#### 查询特定位置物体的信息

下面的 CellX 和 CellY 指的是地图格数，而非绝对坐标。

  - `def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType`：返回某一位置场地种类信息。场地种类详见 structure.h。
  - 以下指令，若查询物品当前在视野内，则返回最新进度/状态；若物品当前不在视野内、但曾经出现在视野内，则返回最后一次看到时的进度/状态；若物品从未出现在视野内，或查询位置没有对应的物品，则返回 -1。
    - `def GetChestProgress(self, cellX: int, cellY: int) -> int`：查询特定位置箱子开启进度
    - `def GetGateProgress(self, cellX: int, cellY: int) -> int`：查询特定位置校门开启进度
    - `def GetClassroomProgress(self, cellX: int, cellY: int) -> int`：查询特定位置教室作业完成进度
    - `def GetDoorProgress(self, cellX: int, cellY: int) -> int`：查询特定位置门开启状态
    - `def IsDoorOpen(self, cellX: int, cellY: int) -> bool`：查询特定位置门是否开启，没有门/不在视野内也返回false
    - `def GetHiddenGateState(self, cellX: int, cellY: int) -> THUAI6.HiddenGateState`：查询特定位置隐藏校门状态,没有隐藏校门/不在视野内返回 THUAI6::HiddenGateState::Null

#### 其他

  - `def GetGameInfo(self) -> THUAI6.GameInfo`：查询当前游戏状态
  - `def GetPlayerGUIDs(self) -> List[int]`：获取所有玩家的 GUID
  - `def GetFrameCount(self) -> int`：获取目前所进行的帧数
  - `def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]`：获取自己的信息
  - `def GetFullMap(self) -> List[List[THUAI6.PlaceType]]`：返回整张地图的地形信息。可以写成类似 `self.GetFullMap()[x][y]`，其中 x 为地图自上到下第几行，y 为自左向右第几列，注意从 0 开始

### 辅助函数

`def CellToGrid(cell: int) -> int`：将地图格数 cell 转换为绝对坐标 grid。

`def GridToCell(grid: int) -> int`：将绝对坐标 grid 转换为地图格数 cell。

下面为用于 DEBUG 的输出函数，选手仅在开启 Debug 模式的情况下可以使用

~~~python
    def Print(self, cont: str) -> None:
    def PrintStudent(self) -> None:
    def PrintTricker(self) -> None:
    def PrintProp(self) -> None:
    def PrintSelfInfo(self) -> None:
~~~

### 部分属性解释 stuctures.h
~~~python
class Player:
    def __init__(self, **kwargs) -> None:
        self.prop: List[PropType] = []//大小固定为3，空的位置为NullPropType
~~~

## 接口一览

~~~python
class IAPI(metaclass=ABCMeta):

    # 选手可执行的操作
    # 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴

    @abstractmethod
    def Move(self, timeInMilliseconds: int, angle: float) -> Future[bool]:
        pass

    # 向特定方向移动

    @abstractmethod
    def MoveRight(self, timeInMilliseconds: int) -> Future[bool]:
        pass

    @abstractmethod
    def MoveLeft(self, timeInMilliseconds: int) -> Future[bool]:
        pass

    @abstractmethod
    def MoveUp(self, timeInMilliseconds: int) -> Future[bool]:
        pass

    @abstractmethod
    def MoveDown(self, timeInMilliseconds: int) -> Future[bool]:
        pass

    # 道具和技能相关

    @abstractmethod
    def PickProp(self, propType: THUAI6.PropType) -> Future[bool]:
        pass

    @abstractmethod
    def UseProp(self, propType: THUAI6.PropType) -> Future[bool]:
        pass

    @abstractmethod
    def ThrowProp(self, propType: THUAI6.PropType) -> Future[bool]:
        pass

    @abstractmethod
    def UseSkill(self, skillID: int) -> Future[bool]:
        pass

    @abstractmethod
    def Attack(self, angle: float) -> Future[bool]:
        pass

    @abstractmethod
    def OpenDoor(self) -> Future[bool]:
        pass

    @abstractmethod
    def CloseDoor(self) -> Future[bool]:
        pass

    @abstractmethod
    def SkipWindow(self) -> Future[bool]:
        pass

    @abstractmethod
    def StartOpenGate(self) -> Future[bool]:
        pass

    @abstractmethod
    def StartOpenChest(self) -> Future[bool]:
        pass

    @abstractmethod
    def EndAllAction(self) -> Future[bool]:
        pass

    # 消息相关，接收消息时无消息则返回(-1, '')

    @abstractmethod
    def SendMessage(self, toID: int, message: Union[str, bytes]) -> Future[bool]:
        pass

    @abstractmethod
    def HaveMessage(self) -> bool:
        pass

    @abstractmethod
    def GetMessage(self) -> Tuple[int, Union[str, bytes]]:
        pass

    # 等待下一帧

    @abstractmethod
    def Wait(self) -> Future[bool]:
        pass

    # 获取各类游戏中的消息

    @abstractmethod
    def GetFrameCount(self) -> int:
        pass

    @abstractmethod
    def GetPlayerGUIDs(self) -> List[int]:
        pass

    @abstractmethod
    def GetTrickers(self) -> List[THUAI6.Tricker]:
        pass

    @abstractmethod
    def GetStudents(self) -> List[THUAI6.Student]:
        pass

    @abstractmethod
    def GetProps(self) -> List[THUAI6.Prop]:
        pass

    @abstractmethod
    def GetBullets(self) -> List[THUAI6.Bullet]:
        pass

    @abstractmethod
    def GetSelfInfo(self) -> Union[THUAI6.Student, THUAI6.Tricker]:
        pass

    @abstractmethod
    def GetFullMap(self) -> List[List[THUAI6.PlaceType]]:
        pass

    @abstractmethod
    def GetPlaceType(self, cellX: int, cellY: int) -> THUAI6.PlaceType:
        pass

    @abstractmethod
    def IsDoorOpen(self, cellX: int, cellY: int) -> bool:
        pass

    @abstractmethod
    def GetChestProgress(self, cellX: int, cellY: int) -> int:
        pass

    @abstractmethod
    def GetGateProgress(self, cellX: int, cellY: int) -> int:
        pass

    @abstractmethod
    def GetClassroomProgress(self, cellX: int, cellY: int) -> int:
        pass

    @abstractmethod
    def GetDoorProgress(self, cellX: int, cellY: int) -> int:
        pass

    @abstractmethod
    def GetHiddenGateState(self, cellX: int, cellY: int) -> THUAI6.HiddenGateState:
        pass

    @abstractmethod
    def GetGameInfo(self) -> THUAI6.GameInfo:
        pass
        
    @abstractmethod
    def HaveView(self, gridX: int, gridY: int) -> bool
        pass

    # 用于DEBUG的输出函数，仅在DEBUG模式下有效

    @abstractmethod
    def Print(self, cont: str) -> None:
        pass

    @abstractmethod
    def PrintStudent(self) -> None:
        pass

    @abstractmethod
    def PrintTricker(self) -> None:
        pass

    @abstractmethod
    def PrintProp(self) -> None:
        pass

    @abstractmethod
    def PrintSelfInfo(self) -> None:
        pass


class IStudentAPI(IAPI, metaclass=ABCMeta):

    # 人类阵营的特殊函数

    @abstractmethod
    def Graduate(self) -> Future[bool]:
        pass

    @abstractmethod
    def StartLearning(self) -> Future[bool]:
        pass

    @abstractmethod
    def StartEncourageMate(self, mateID: int) -> Future[bool]:
        pass

    @abstractmethod
    def StartRouseMate(self, mateID: int) -> Future[bool]:
        pass

    @abstractmethod
    def GetSelfInfo(self) -> THUAI6.Student:
        pass


class ITrickerAPI(IAPI, metaclass=ABCMeta):

    # 屠夫阵营的特殊函数

    @abstractmethod
    def GetSelfInfo(self) -> THUAI6.Tricker:
        pass
~~~
