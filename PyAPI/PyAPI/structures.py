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


gameStateDict: Dict[GameState, str] = {
    GameState.NullGameState: "NullGameState",
    GameState.GameStart: "GameStart",
    GameState.GameRunning: "GameRunning",
    GameState.GameEnd: "GameEnd"}

humanStateDict: Dict[HumanState, str] = {
    HumanState.NullHumanState: "NullHumanState",
    HumanState.Idle: "Idle",
    HumanState.Fixing: "Fixing",
    HumanState.Dying: "Dying",
    HumanState.OnChair: "OnChair",
    HumanState.Dead: "Dead"}

playerTypeDict: Dict[PlayerType, str] = {
    PlayerType.NullPlayerType: "NullPlayerType",
    PlayerType.HumanPlayer: "HumanPlayer",
    PlayerType.ButcherPlayer: "ButcherPlayer"}

propTypeDict: Dict[PropType, str] = {
    PropType.NullPropType: "NullPropType",
    PropType.PropType1: "PropType1"}

humanTypeDict: Dict[HumanType, str] = {
    HumanType.NullHumanType: "NullHumanType",
    HumanType.HumanType1: "HumanType1"}

butcherTypeDict: Dict[ButcherType, str] = {
    ButcherType.NullButcherType: "NullButcherType",
    ButcherType.ButcherType1: "ButcherType1"}

humanBuffTypeDict: Dict[HumanBuffType, str] = {
    HumanBuffType.NullHumanBuffType: "NullHumanBuffType",
    HumanBuffType.HumanBuffType1: "HumanBuffType1"}

butcherBuffTypeDict: Dict[ButcherBuffType, str] = {
    ButcherBuffType.NullButcherBuffType: "NullButcherBuffType",
    ButcherBuffType.ButcherBuffType1: "ButcherBuffType1"}

placeTypeDict: Dict[PlaceType, str] = {
    PlaceType.NullPlaceType: "NullPlaceType",
    PlaceType.Land: "Land",
    PlaceType.Wall: "Wall",
    PlaceType.Grass: "Grass",
    PlaceType.Machine: "Machine",
    PlaceType.Gate: "Gate",
    PlaceType.HiddenGate: "HiddenGate"}

shapeTypeDict: Dict[ShapeType, str] = {
    ShapeType.NullShapeType: "NullShapeType",
    ShapeType.Square: "Square",
    ShapeType.Circle: "Circle"}
