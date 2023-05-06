from enum import Enum
from typing import List, Dict
import sys

if sys.version_info < (3, 9):
    from typing import Tuple
else:
    Tuple = tuple


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
    AddLifeOrClairaudience = 5
    AddHpOrAp = 6
    ShieldOrSpear = 7
    RecoveryFromDizziness = 8


class BulletType(Enum):
    NullBulletType = 0
    FlyingKnife = 1
    CommonAttackOfTricker = 2
    BombBomb = 3
    JumpyDumpty = 4
    AtomBomb = 5


class StudentType(Enum):
    NullStudentType = 0
    Athlete = 1
    Teacher = 2
    StraightAStudent = 3
    Robot = 4
    TechOtaku = 5
    Sunshine = 6


class TrickerType(Enum):
    NullTrickerType = 0
    Assassin = 1
    Klee = 2
    ANoisyPerson = 3
    Idol = 4


class StudentBuffType(Enum):
    NullStudentBuffType = 0
    AddSpeed = 1
    AddLife = 2
    Shield = 3
    Invisible = 4


class TrickerBuffType(Enum):
    NullTrickerBuffType = 0
    AddSpeed = 1
    Spear = 2
    AddAp = 3
    Clairaudience = 4
    Invisible = 5


class PlayerState(Enum):
    NullState = 0
    Idle = 1
    Learning = 2
    Addicted = 3
    Quit = 4
    Graduated = 5
    Encouraged = 6
    Roused = 7
    Stunned = 8
    Encouraging = 9
    Rousing = 10
    Swinging = 11
    Attacking = 12
    Locking = 13
    # Rummaging = 14
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
    def __init__(self, **kwargs) -> None:
        self.x: int = 0
        self.y: int = 0
        self.speed: int = 0
        self.viewRange: int = 0
        self.playerID: int = 0
        self.guid: int = 0
        self.radius: int = 0
        self.score: int = 0
        self.facingDirection: float = 0.0
        self.timeUntilSkillAvailable: List[float] = []
        self.playerType: PlayerType = PlayerType.NullPlayerType
        self.prop: List[PropType] = []
        self.bulletType: BulletType = BulletType.NullBulletType
        self.playerState: PlayerState = PlayerState.NullState


class Student(Player):
    def __init__(self, **kwargs) -> None:
        super().__init__()
        self.studentType: StudentType = StudentType.NullStudentType
        self.determination: int = 0
        self.addiction: int = 0
        self.encourageProgress: int = 0
        self.rouseProgress: int = 0
        self.learningSpeed: int = 0
        self.encourageSpeed: int = 0
        self.dangerAlert: float = 0.0
        self.buff: List[StudentBuffType] = []


class Tricker(Player):
    def __init__(self, **kwargs) -> None:
        super().__init__()
        self.trickerType: TrickerType = TrickerType.NullTrickerType
        self.trickDesire: float = 0.0
        self.classVolume: float = 0.0
        self.buff: List[TrickerBuffType] = []


class Prop:
    def __init__(self, **kwargs) -> None:
        self.x: int = 0
        self.y: int = 0
        self.guid: int = 0
        self.type: PropType = PropType.NullPropType
        self.facingDirection: float = 0.0


class Bullet:
    def __init__(self, **kwargs) -> None:
        self.bulletType: BulletType = BulletType.NullBulletType
        self.x: int = 0
        self.y: int = 0
        self.facingDirection: float = 0.0
        self.guid: int = 0
        self.team: PlayerType = PlayerType.NullPlayerType
        self.bombRange: float = 0.0
        self.speed: int = 0


class BombedBullet:
    def __init__(self, **kwargs) -> None:
        self.bulletType: BulletType = BulletType.NullBulletType
        self.x: int = 0
        self.y: int = 0
        self.facingDirection: float = 0.0
        self.mappingID: int = 0
        self.bombRange: float = 0.0


class GameMap:
    def __init__(self, **kwargs) -> None:
        self.classroomState: Dict[Tuple[int, int], int] = {}
        self.gateState: Dict[Tuple[int, int], int] = {}
        self.chestState: Dict[Tuple[int, int], int] = {}
        self.doorState: Dict[Tuple[int, int], bool] = {}
        self.doorProgress: Dict[Tuple[int, int], int] = {}
        self.hiddenGateState: Dict[Tuple[int, int], HiddenGateState] = {}


class GameInfo:
    def __init__(self, **kwargs) -> None:
        self.gameTime: int = 0
        self.subjectFinished: int = 0
        self.studentGraduated: int = 0
        self.studentQuited: int = 0
        self.studentScore: int = 0
        self.trickerScore: int = 0
