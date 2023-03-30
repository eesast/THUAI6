from enum import Enum
from typing import List, Dict, Tuple


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
    Gate = 5
    HiddenGate = 6
    Window = 7
    Door3 = 8
    Door5 = 9
    Door6 = 10
    Chest = 11


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
    Key3 = 1
    Key5 = 2
    Key6 = 3
    AddSpeed = 4
    AddLifeOrAp = 5
    AddHpOrAp = 6
    ShieldOrSpear = 7


class BulletType(Enum):
    NullBulletType = 0
    FlyingKnife = 1
    CommonAttackOfTricker = 2
    FastBullet = 3
    OrdinaryBullet = 4
    AtomBomb = 5


class StudentType(Enum):
    NullStudentType = 0
    Athlete = 1


class TrickerType(Enum):
    NullTrickerType = 0
    Assassin = 1


class StudentBuffType(Enum):
    NullStudentBuffType = 0
    StudentBuffType1 = 1


class TrickerBuffType(Enum):
    NullTrickerBuffType = 0
    TrickerBuffType1 = 1


class PlayerState(Enum):
    NullState = 0
    Idle = 1
    Learning = 2
    Addicted = 3
    Quit = 4
    Graduated = 5
    Treated = 6
    Rescued = 7
    Stunned = 8
    Treating = 9
    Rescuing = 10
    Swinging = 11
    Attacking = 12
    Locking = 13
    Rummaging = 14
    Climbing = 15
    OpeningAChest = 16
    UsingSpecialSkill = 17
    OpeningAGate = 18


class MessageOfObj(Enum):
    NullMessageOfObj = 0
    StudentMessage = 1
    TrickerMessage = 2
    PropMessage = 3
    BulletMessage = 4
    BombedBulletMessage = 5
    ClassroomMessage = 6
    GateMessage = 7
    ChestMessage = 8
    DoorMessage = 9
    MapMessage = 10
    NewsMessage = 11
    HiddenGateMessage = 12


class HiddenGateState(Enum):
    Null = 0
    Refreshed = 1
    Opened = 2


class Player:
    x: int
    y: int
    speed: int
    viewRange: int
    playerID: int
    guid: int
    radius: int
    score: int
    facingDirection: float
    timeUntilSkillAvailable: List[float] = []
    playerType: PlayerType
    prop: List[PropType] = []
    place: PlaceType
    bulletType: BulletType
    playerState: PlayerState


class Student(Player):
    studentType: StudentType
    determination: int
    addiction: int
    treatProgress: int
    rescueProgress: int
    learningSpeed: int
    treatSpeed: int
    dangerAlert: float
    buff: List[StudentBuffType] = []


class Tricker(Player):
    trickerType: TrickerType
    trickDesire: float
    classVolume: float
    buff: List[TrickerBuffType] = []


class Prop:
    x: int
    y: int
    guid: int
    type: PropType
    place: PlaceType
    facingDirection: float


class Bullet:
    bulletType: BulletType
    x: int
    y: int
    facingDirection: float
    guid: int
    team: PlayerType
    place: PlaceType
    bombRange: float
    speed: int


class BombedBullet:
    bulletType: BulletType
    x: int
    y: int
    facingDirection: float
    mappingID: int
    bombRange: float


class GameMap:
    classroomState: Dict[tuple[int, int], int] = {}
    gateState: Dict[tuple[int, int], int] = {}
    chestState: Dict[tuple[int, int], int] = {}
    doorState: Dict[tuple[int, int], bool] = {}
    doorProgress: Dict[tuple[int, int], int] = {}
    hiddenGateState: Dict[tuple[int, int], HiddenGateState] = {}


class GameInfo:
    gameTime: int
    subjectLeft: int
    studentGraduated: int
    studentQuited: int
    studentScore: int
    trickerScore: int
