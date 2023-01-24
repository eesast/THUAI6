import MessageType_pb2 as _MessageType_pb2
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor


class AttackMsg(_message.Message):
    __slots__ = ["angle", "player_id"]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    angle: float
    player_id: int
    def __init__(
        self, player_id: _Optional[int] = ..., angle: _Optional[float] = ...) -> None: ...


class IDMsg(_message.Message):
    __slots__ = ["player_id"]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    player_id: int
    def __init__(self, player_id: _Optional[int] = ...) -> None: ...


class MoveMsg(_message.Message):
    __slots__ = ["angle", "player_id", "time_in_milliseconds"]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    TIME_IN_MILLISECONDS_FIELD_NUMBER: _ClassVar[int]
    angle: float
    player_id: int
    time_in_milliseconds: int
    def __init__(self, player_id: _Optional[int] = ..., angle: _Optional[float]
                 = ..., time_in_milliseconds: _Optional[int] = ...) -> None: ...


class PickMsg(_message.Message):
    __slots__ = ["player_id", "prop_type"]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    PROP_TYPE_FIELD_NUMBER: _ClassVar[int]
    player_id: int
    prop_type: _MessageType_pb2.PropType
    def __init__(self, player_id: _Optional[int] = ...,
                 prop_type: _Optional[_Union[_MessageType_pb2.PropType, str]] = ...) -> None: ...


class PlayerMsg(_message.Message):
    __slots__ = ["butcher_type", "human_type", "player_id", "player_type"]
    BUTCHER_TYPE_FIELD_NUMBER: _ClassVar[int]
    HUMAN_TYPE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    PLAYER_TYPE_FIELD_NUMBER: _ClassVar[int]
    butcher_type: _MessageType_pb2.ButcherType
    human_type: _MessageType_pb2.HumanType
    player_id: int
    player_type: _MessageType_pb2.PlayerType
    def __init__(self, player_id: _Optional[int] = ..., player_type: _Optional[_Union[_MessageType_pb2.PlayerType, str]] = ...,
                 human_type: _Optional[_Union[_MessageType_pb2.HumanType, str]] = ..., butcher_type: _Optional[_Union[_MessageType_pb2.ButcherType, str]] = ...) -> None: ...


class SendMsg(_message.Message):
    __slots__ = ["message", "player_id", "to_player_id"]
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    TO_PLAYER_ID_FIELD_NUMBER: _ClassVar[int]
    message: str
    player_id: int
    to_player_id: int

    def __init__(self, player_id: _Optional[int] = ..., to_player_id: _Optional[int]
                 = ..., message: _Optional[str] = ...) -> None: ...
