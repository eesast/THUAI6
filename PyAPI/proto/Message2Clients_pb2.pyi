import MessageType_pb2 as _MessageType_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor


class BoolRes(_message.Message):
    __slots__ = ["act_success"]
    ACT_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    act_success: bool
    def __init__(self, act_success: bool = ...) -> None: ...


class MessageOfButcher(_message.Message):
    __slots__ = ["buff", "butcher_type", "damage", "guid", "movable", "place", "player_id",
                 "prop", "radius", "speed", "time_until_skill_available", "view_range", "x", "y"]
    BUFF_FIELD_NUMBER: _ClassVar[int]
    BUTCHER_TYPE_FIELD_NUMBER: _ClassVar[int]
    DAMAGE_FIELD_NUMBER: _ClassVar[int]
    GUID_FIELD_NUMBER: _ClassVar[int]
    MOVABLE_FIELD_NUMBER: _ClassVar[int]
    PLACE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    PROP_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    TIME_UNTIL_SKILL_AVAILABLE_FIELD_NUMBER: _ClassVar[int]
    VIEW_RANGE_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    buff: _containers.RepeatedScalarFieldContainer[_MessageType_pb2.ButcherBuffType]
    butcher_type: _MessageType_pb2.ButcherType
    damage: int
    guid: int
    movable: bool
    place: _MessageType_pb2.PlaceType
    player_id: int
    prop: _MessageType_pb2.PropType
    radius: int
    speed: int
    time_until_skill_available: float
    view_range: int
    x: int
    y: int
    def __init__(self, x: _Optional[int] = ..., y: _Optional[int] = ..., speed: _Optional[int] = ..., damage: _Optional[int] = ..., time_until_skill_available: _Optional[float] = ..., place: _Optional[_Union[_MessageType_pb2.PlaceType, str]] = ..., prop: _Optional[_Union[_MessageType_pb2.PropType, str]] = ...,
                 butcher_type: _Optional[_Union[_MessageType_pb2.ButcherType, str]] = ..., guid: _Optional[int] = ..., movable: bool = ..., player_id: _Optional[int] = ..., view_range: _Optional[int] = ..., radius: _Optional[int] = ..., buff: _Optional[_Iterable[_Union[_MessageType_pb2.ButcherBuffType, str]]] = ...) -> None: ...


class MessageOfHuman(_message.Message):
    __slots__ = ["buff", "chair_time", "ground_time", "guid", "hanged_time", "human_type", "life", "place",
                 "player_id", "prop", "radius", "speed", "state", "time_until_skill_available", "view_range", "x", "y"]
    BUFF_FIELD_NUMBER: _ClassVar[int]
    CHAIR_TIME_FIELD_NUMBER: _ClassVar[int]
    GROUND_TIME_FIELD_NUMBER: _ClassVar[int]
    GUID_FIELD_NUMBER: _ClassVar[int]
    HANGED_TIME_FIELD_NUMBER: _ClassVar[int]
    HUMAN_TYPE_FIELD_NUMBER: _ClassVar[int]
    LIFE_FIELD_NUMBER: _ClassVar[int]
    PLACE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    PROP_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    SPEED_FIELD_NUMBER: _ClassVar[int]
    STATE_FIELD_NUMBER: _ClassVar[int]
    TIME_UNTIL_SKILL_AVAILABLE_FIELD_NUMBER: _ClassVar[int]
    VIEW_RANGE_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    buff: _containers.RepeatedScalarFieldContainer[_MessageType_pb2.HumanBuffType]
    chair_time: float
    ground_time: float
    guid: int
    hanged_time: int
    human_type: _MessageType_pb2.HumanType
    life: int
    place: _MessageType_pb2.PlaceType
    player_id: int
    prop: _MessageType_pb2.PropType
    radius: int
    speed: int
    state: _MessageType_pb2.HumanState
    time_until_skill_available: float
    view_range: int
    x: int
    y: int
    def __init__(self, x: _Optional[int] = ..., y: _Optional[int] = ..., speed: _Optional[int] = ..., life: _Optional[int] = ..., hanged_time: _Optional[int] = ..., time_until_skill_available: _Optional[float] = ..., place: _Optional[_Union[_MessageType_pb2.PlaceType, str]] = ..., prop: _Optional[_Union[_MessageType_pb2.PropType, str]] = ..., human_type: _Optional[_Union[_MessageType_pb2.HumanType, str]]
                 = ..., guid: _Optional[int] = ..., state: _Optional[_Union[_MessageType_pb2.HumanState, str]] = ..., chair_time: _Optional[float] = ..., ground_time: _Optional[float] = ..., player_id: _Optional[int] = ..., view_range: _Optional[int] = ..., radius: _Optional[int] = ..., buff: _Optional[_Iterable[_Union[_MessageType_pb2.HumanBuffType, str]]] = ...) -> None: ...


class MessageOfMap(_message.Message):
    __slots__ = ["row"]

    class Row(_message.Message):
        __slots__ = ["col"]
        COL_FIELD_NUMBER: _ClassVar[int]
        col: _containers.RepeatedScalarFieldContainer[_MessageType_pb2.PlaceType]
        def __init__(
            self, col: _Optional[_Iterable[_Union[_MessageType_pb2.PlaceType, str]]] = ...) -> None: ...
    ROW_FIELD_NUMBER: _ClassVar[int]
    row: _containers.RepeatedCompositeFieldContainer[MessageOfMap.Row]
    def __init__(
        self, row: _Optional[_Iterable[_Union[MessageOfMap.Row, _Mapping]]] = ...) -> None: ...


class MessageOfPickedProp(_message.Message):
    __slots__ = ["facing_direction", "mapping_id", "type", "x", "y"]
    FACING_DIRECTION_FIELD_NUMBER: _ClassVar[int]
    MAPPING_ID_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    facing_direction: float
    mapping_id: int
    type: _MessageType_pb2.PropType
    x: int
    y: int
    def __init__(self, type: _Optional[_Union[_MessageType_pb2.PropType, str]] = ..., x: _Optional[int] = ...,
                 y: _Optional[int] = ..., facing_direction: _Optional[float] = ..., mapping_id: _Optional[int] = ...) -> None: ...


class MessageOfProp(_message.Message):
    __slots__ = ["facing_direction", "guid",
                 "is_moving", "place", "size", "type", "x", "y"]
    FACING_DIRECTION_FIELD_NUMBER: _ClassVar[int]
    GUID_FIELD_NUMBER: _ClassVar[int]
    IS_MOVING_FIELD_NUMBER: _ClassVar[int]
    PLACE_FIELD_NUMBER: _ClassVar[int]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    TYPE_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    facing_direction: float
    guid: int
    is_moving: bool
    place: _MessageType_pb2.PlaceType
    size: int
    type: _MessageType_pb2.PropType
    x: int
    y: int
    def __init__(self, type: _Optional[_Union[_MessageType_pb2.PropType, str]] = ..., x: _Optional[int] = ..., y: _Optional[int] = ..., facing_direction: _Optional[float]
                 = ..., guid: _Optional[int] = ..., place: _Optional[_Union[_MessageType_pb2.PlaceType, str]] = ..., size: _Optional[int] = ..., is_moving: bool = ...) -> None: ...


class MessageToClient(_message.Message):
    __slots__ = ["butcher_message", "game_state",
                 "human_message", "map_message", "prop_message"]
    BUTCHER_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    GAME_STATE_FIELD_NUMBER: _ClassVar[int]
    HUMAN_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    MAP_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    PROP_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    butcher_message: _containers.RepeatedCompositeFieldContainer[MessageOfButcher]
    game_state: _MessageType_pb2.GameState
    human_message: _containers.RepeatedCompositeFieldContainer[MessageOfHuman]
    map_message: MessageOfMap
    prop_message: _containers.RepeatedCompositeFieldContainer[MessageOfProp]
    def __init__(self, human_message: _Optional[_Iterable[_Union[MessageOfHuman, _Mapping]]] = ..., butcher_message: _Optional[_Iterable[_Union[MessageOfButcher, _Mapping]]] = ..., prop_message: _Optional[
                 _Iterable[_Union[MessageOfProp, _Mapping]]] = ..., map_message: _Optional[_Union[MessageOfMap, _Mapping]] = ..., game_state: _Optional[_Union[_MessageType_pb2.GameState, str]] = ...) -> None: ...


class MoveRes(_message.Message):
    __slots__ = ["act_success", "actual_angle", "actual_speed"]
    ACTUAL_ANGLE_FIELD_NUMBER: _ClassVar[int]
    ACTUAL_SPEED_FIELD_NUMBER: _ClassVar[int]
    ACT_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    act_success: bool
    actual_angle: float
    actual_speed: int
    def __init__(self, actual_speed: _Optional[int] = ...,
                 actual_angle: _Optional[float] = ..., act_success: bool = ...) -> None: ...


class MsgRes(_message.Message):
    __slots__ = ["from_player_id", "have_message", "message_received"]
    FROM_PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    HAVE_MESSAGE_FIELD_NUMBER: _ClassVar[int]
    MESSAGE_RECEIVED_FIELD_NUMBER: _ClassVar[int]
    from_player_id: int
    have_message: bool
    message_received: str

    def __init__(self, have_message: bool = ...,
                 from_player_id: _Optional[int] = ..., message_received: _Optional[str] = ...) -> None: ...
