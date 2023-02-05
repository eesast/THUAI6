from enum import Enum
from typing import List, Dict


class GameState(Enum):
    NullGameState = 0
    GameStart = 1
    GameRunning = 2
    GameEnd = 3


class PlaceType(Enum):
    NullPlaceType = 0
    Land = 1
    Wall = 2
    Grass = 3
    Machine = 4
    Gate = 5
    HiddenGate = 6


class ShapeType(Enum):
    NullShapeType = 0
    Square = 1
    Circle = 2


class PlayerType(Enum):
    NullPlayerType = 0
    HumanPlayer = 1
    ButcherPlayer = 2


class PropType(Enum):
    NullPropType = 0
    PropType1 = 1


class HumanType(Enum):
    NullHumanType = 0
    HumanType1 = 1


class ButcherType(Enum):
    NullButcherType = 0
    ButcherType1 = 1


class HumanBuffType(Enum):
    NullHumanBuffType = 0
    HumanBuffType1 = 1


class ButcherBuffType(Enum):
    NullButcherBuffType = 0
    ButcherBuffType1 = 1


class HumanState(Enum):
    NullHumanState = 0
    Idle = 1
    Fixing = 2
    Dying = 3
    OnChair = 4
    Dead = 5


class Player:
    x: int
    y: int
    speed: int
    viewRange: int
    playerID: int
    guid: int
    radius: int
    timeUntilSkillAvailable: float
    playerType: PlayerType
    prop: PropType
    place: PlaceType


class Human(Player):
    state: HumanState
    life: int
    hangedTime: int
    humanType: HumanType
    buff: List[HumanBuffType]


class Butcher(Player):
    damage: int
    movable: bool
    butcherType: ButcherType
    buff: List[ButcherBuffType]


class Prop:
    x: int
    y: int
    size: int
    guid: int
    type: PropType
    place: PlaceType
    facingDirection: float
    isMoving: bool
