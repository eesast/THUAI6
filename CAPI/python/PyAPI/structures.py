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
    ClassRoom = 4
    BlackRoom = 5
    Gate = 6
    HiddenGate = 7


class ShapeType(Enum):
    NullShapeType = 0
    Square = 1
    Circle = 2


class PlayerType(Enum):
    NullPlayerType = 0
    StudentPlayer = 1
    TrickerPlayer = 2


class PropType(Enum):
    NullPropType = 0
    PropType1 = 1


class StudentType(Enum):
    NullStudentType = 0
    StudentType1 = 1


class TrickerType(Enum):
    NullTrickerType = 0
    TrickerType1 = 1


class StudentBuffType(Enum):
    NullStudentBuffType = 0
    StudentBuffType1 = 1


class TrickerBuffType(Enum):
    NullTrickerBuffType = 0
    TrickerBuffType1 = 1


class StudentState(Enum):
    NullStudentState = 0
    Idle = 1
    Learning = 2
    Fail = 3
    Emotional = 4
    Quit = 5
    Graduated = 6


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


class Student(Player):
    state: StudentState
    determination: int
    failNum: int
    failTime: float
    emoTime: float
    studentType: StudentType
    buff: List[StudentBuffType]


class Tricker(Player):
    damage: int
    movable: bool
    trickerType: TrickerType
    buff: List[TrickerBuffType]


class Prop:
    x: int
    y: int
    size: int
    guid: int
    type: PropType
    place: PlaceType
    facingDirection: float
    isMoving: bool
