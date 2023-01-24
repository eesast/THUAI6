from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from typing import ClassVar as _ClassVar

BBUFFTYPE1: ButcherBuffType
BBUFFTYPE2: ButcherBuffType
BBUFFTYPE3: ButcherBuffType
BBUFFTYPE4: ButcherBuffType
BUTCHERTYPE1: ButcherType
BUTCHERTYPE2: ButcherType
BUTCHERTYPE3: ButcherType
BUTCHERTYPE4: ButcherType
BUTCHER_PLAYER: PlayerType
CIRCLE: ShapeType
DEAD: HumanState
DESCRIPTOR: _descriptor.FileDescriptor
DYING: HumanState
FIXING: HumanState
GAME_END: GameState
GAME_RUNNING: GameState
GAME_START: GameState
GATE: PlaceType
GRASS: PlaceType
HBUFFTYPE1: HumanBuffType
HBUFFTYPE2: HumanBuffType
HBUFFTYPE3: HumanBuffType
HBUFFTYPE4: HumanBuffType
HIDDEN_GATE: PlaceType
HUMANTYPE1: HumanType
HUMANTYPE2: HumanType
HUMANTYPE3: HumanType
HUMANTYPE4: HumanType
HUMAN_PLAYER: PlayerType
IDLE: HumanState
LAND: PlaceType
MACHINE: PlaceType
NULL_BBUFF_TYPE: ButcherBuffType
NULL_BUTCHER_TYPE: ButcherType
NULL_GAME_STATE: GameState
NULL_HBUFF_TYPE: HumanBuffType
NULL_HUMAN_TYPE: HumanType
NULL_PLACE_TYPE: PlaceType
NULL_PLAYER_TYPE: PlayerType
NULL_PROP_TYPE: PropType
NULL_SHAPE_TYPE: ShapeType
NULL_STATUS: HumanState
ON_CHAIR: HumanState
PTYPE1: PropType
PTYPE2: PropType
PTYPE3: PropType
PTYPE4: PropType
SQUARE: ShapeType
WALL: PlaceType


class PlaceType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class ShapeType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class PropType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class HumanBuffType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class HumanState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class ButcherBuffType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class PlayerType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class HumanType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class ButcherType(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []


class GameState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
    __slots__ = []
