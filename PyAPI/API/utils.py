import structures as THUAI6
from typing import Final, List
import proto.Message2Clients_pb2 as Message2Clients
import proto.Message2Server_pb2 as Message2Server
import proto.MessageType_pb2 as MessageType

numOfGridPerCell: Final[int] = 1000


# 起到NameSpace的作用
class NoInstance:
    def __call__(self, *args, **kwargs):
        raise TypeError("This class cannot be instantiated.")


class AssistFunction(NoInstance):
    # 辅助函数
    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell*numOfGridPerCell+numOfGridPerCell//2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid//numOfGridPerCell


class Proto2THUAI6(NoInstance):
    placeTypeDict: Final[dict] = {
        MessageType.NULL_PLACE_TYPE: THUAI6.PlaceType.NullPlaceType,
        MessageType.WALL: THUAI6.PlaceType.Wall,
        MessageType.LAND: THUAI6.PlaceType.Land,
        MessageType.GRASS: THUAI6.PlaceType.Grass,
        MessageType.MACHINE: THUAI6.PlaceType.Machine,
        MessageType.GATE: THUAI6.PlaceType.Gate,
        MessageType.HIDDEN_GATE: THUAI6.PlaceType.HiddenGate}

    shapeTypeDict: Final[dict] = {
        MessageType.NULL_SHAPE_TYPE: THUAI6.ShapeType.NullShapeType,
        MessageType.SQUARE: THUAI6.ShapeType.Square,
        MessageType.CIRCLE: THUAI6.ShapeType.Circle}

    propTypeDict: Final[dict] = {
        MessageType.NULL_PROP_TYPE: THUAI6.PropType.NullPropType,
        MessageType.PTYPE1: THUAI6.PropType.PropType1, }

    playerTypeDict: Final[dict] = {
        MessageType.NULL_PLAYER_TYPE: THUAI6.PlayerType.NullPlayerType,
        MessageType.HUMAN_PLAYER: THUAI6.PlayerType.HumanPlayer,
        MessageType.BUTCHER_PLAYER: THUAI6.PlayerType.ButcherPlayer}

    humanTypeDict: Final[dict] = {
        MessageType.NULL_HUMAN_TYPE: THUAI6.HumanType.NullHumanType,
        MessageType.HUMANTYPE1: THUAI6.HumanType.HumanType1, }

    butcherTypeDict: Final[dict] = {
        MessageType.NULL_BUTCHER_TYPE: THUAI6.ButcherType.NullButcherType,
        MessageType.BUTCHERTYPE1: THUAI6.ButcherType.ButcherType1, }

    humanBuffTypeDict: Final[dict] = {
        MessageType.NULL_HBUFF_TYPE: THUAI6.HumanBuffType.NullHumanBuffType,
        MessageType.HBUFFTYPE1: THUAI6.HumanBuffType.HumanBuffType1, }

    butcherBuffTypeDict: Final[dict] = {
        MessageType.NULL_BBUFF_TYPE: THUAI6.ButcherBuffType.NullButcherBuffType,
        MessageType.BBUFFTYPE1: THUAI6.ButcherBuffType.ButcherBuffType1, }

    humanStateDict: Final[dict] = {
        MessageType.NULL_STATUS: THUAI6.HumanState.NullHumanState,
        MessageType.IDLE: THUAI6.HumanState.Idle,
        MessageType.FIXING: THUAI6.HumanState.Fixing,
        MessageType.DYING: THUAI6.HumanState.Dying,
        MessageType.ON_CHAIR: THUAI6.HumanState.OnChair,
        MessageType.DEAD: THUAI6.HumanState.Dead}

    gameStateDict: Final[dict] = {
        MessageType.NULL_GAME_STATE: THUAI6.GameState.NullGameState,
        MessageType.GAME_START: THUAI6.GameState.GameStart,
        MessageType.GAME_RUNNING: THUAI6.GameState.GameRunning,
        MessageType.GAME_END: THUAI6.GameState.GameEnd}

    # 用于将Proto的对象转为THUAI6的对象
    @staticmethod
    def Protobuf2THUAI6Butcher(butcherMsg: Message2Clients.MessageOfButcher) -> THUAI6.Butcher:
        butcher = THUAI6.Butcher()
        butcher.x = butcherMsg.x
        butcher.y = butcherMsg.y
        butcher.speed = butcherMsg.speed
        butcher.damage = butcherMsg.damage
        butcher.timeUntilSkillAvailable = butcherMsg.time_until_skill_available
        butcher.place = Proto2THUAI6.placeTypeDict[butcherMsg.place]
        butcher.prop = Proto2THUAI6.propTypeDict[butcherMsg.prop]
        butcher.butcherType = Proto2THUAI6.butcherTypeDict[butcherMsg.butcher_type]
        butcher.guid = butcherMsg.guid
        butcher.movable = butcherMsg.movable
        butcher.playerID = butcherMsg.player_id
        butcher.viewRange = butcherMsg.view_range
        butcher.radius = butcherMsg.radius
        butcher.buff.clear()
        for buff in butcherMsg.buff:
            butcher.buff.append(Proto2THUAI6.butcherBuffTypeDict[buff])
        return butcher

    @staticmethod
    def Protobuf2THUAI6Human(humanMsg: Message2Clients.MessageOfHuman) -> THUAI6.Human:
        human = THUAI6.Human()
        human.x = humanMsg.x
        human.y = humanMsg.y
        human.speed = humanMsg.speed
        human.life = humanMsg.life
        human.hangedTime = humanMsg.hanged_time
        human.timeUntilSkillAvailable = humanMsg.time_until_skill_available
        human.place = Proto2THUAI6.placeTypeDict[humanMsg.place]
        human.prop = Proto2THUAI6.propTypeDict[humanMsg.prop]
        human.humanType = Proto2THUAI6.humanTypeDict[humanMsg.human_type]
        human.guid = humanMsg.guid
        human.state = Proto2THUAI6.humanStateDict[humanMsg.state]
        human.playerID = humanMsg.player_id
        human.viewRange = humanMsg.view_range
        human.radius = humanMsg.radius
        human.buff.clear()
        for buff in humanMsg.buff:
            human.buff.append(Proto2THUAI6.humanBuffTypeDict[buff])
        return human

    @staticmethod
    def Protobuf2THUAI6Prop(propMsg: Message2Clients.MessageOfProp) -> THUAI6.Prop:
        prop = THUAI6.Prop()
        prop.x = propMsg.x
        prop.y = propMsg.y
        prop.type = Proto2THUAI6.propTypeDict[propMsg.type]
        prop.guid = propMsg.guid
        prop.size = propMsg.size
        prop.facingDirection = propMsg.facing_direction
        prop.isMoving = propMsg.is_moving
        return prop

    @staticmethod
    def Protobuf2THUAI6Map(mapMsg: Message2Clients.MessageOfMap) -> List[List[THUAI6.PlaceType]]:
        map = []
        for row in mapMsg.row:
            newRow = []
            for place in row.col:
                newRow.append(Proto2THUAI6.placeTypeDict[place])
            map.append(newRow)
        return map


class THUAI62Proto(NoInstance):
    placeTypeDict: Final[dict] = {
        THUAI6.PlaceType.NullPlaceType: MessageType.NULL_PLACE_TYPE,
        THUAI6.PlaceType.Wall: MessageType.WALL,
        THUAI6.PlaceType.Land: MessageType.LAND,
        THUAI6.PlaceType.Grass: MessageType.GRASS,
        THUAI6.PlaceType.Machine: MessageType.MACHINE,
        THUAI6.PlaceType.Gate: MessageType.GATE,
        THUAI6.PlaceType.HiddenGate: MessageType.HIDDEN_GATE}

    shapeTypeDict: Final[dict] = {
        THUAI6.ShapeType.NullShapeType: MessageType.NULL_SHAPE_TYPE,
        THUAI6.ShapeType.Square: MessageType.SQUARE,
        THUAI6.ShapeType.Circle: MessageType.CIRCLE}

    propTypeDict: Final[dict] = {
        THUAI6.PropType.NullPropType: MessageType.NULL_PROP_TYPE,
        THUAI6.PropType.PropType1: MessageType.PTYPE1, }

    playerTypeDict: Final[dict] = {
        THUAI6.PlayerType.NullPlayerType: MessageType.NULL_PLAYER_TYPE,
        THUAI6.PlayerType.HumanPlayer: MessageType.HUMAN_PLAYER,
        THUAI6.PlayerType.ButcherPlayer: MessageType.BUTCHER_PLAYER}

    humanTypeDict: Final[dict] = {
        THUAI6.HumanType.NullHumanType: MessageType.NULL_HUMAN_TYPE,
        THUAI6.HumanType.HumanType1: MessageType.HUMANTYPE1, }

    butcherTypeDict: Final[dict] = {
        THUAI6.ButcherType.NullButcherType: MessageType.NULL_BUTCHER_TYPE,
        THUAI6.ButcherType.ButcherType1: MessageType.BUTCHERTYPE1, }

    humanBuffTypeDict: Final[dict] = {
        THUAI6.HumanBuffType.NullHumanBuffType: MessageType.NULL_HBUFF_TYPE,
        THUAI6.HumanBuffType.HumanBuffType1: MessageType.HBUFFTYPE1, }

    butcherBuffTypeDict: Final[dict] = {
        THUAI6.ButcherBuffType.NullButcherBuffType: MessageType.NULL_BBUFF_TYPE,
        THUAI6.ButcherBuffType.ButcherBuffType1: MessageType.BBUFFTYPE1, }

    humanStateDict: Final[dict] = {
        THUAI6.HumanState.NullHumanState: MessageType.NULL_STATUS,
        THUAI6.HumanState.Idle: MessageType.IDLE,
        THUAI6.HumanState.Fixing: MessageType.FIXING,
        THUAI6.HumanState.Dying: MessageType.DYING,
        THUAI6.HumanState.OnChair: MessageType.ON_CHAIR,
        THUAI6.HumanState.Dead: MessageType.DEAD}

    gameStateDict: Final[dict] = {
        THUAI6.GameState.NullGameState: MessageType.NULL_GAME_STATE,
        THUAI6.GameState.GameStart: MessageType.GAME_START,
        THUAI6.GameState.GameRunning: MessageType.GAME_RUNNING,
        THUAI6.GameState.GameEnd: MessageType.GAME_END}

    # 用于将THUAI6的对象转为Proto的对象
    @staticmethod
    def THUAI62ProtobufPlayer(playerID: int, playerType: THUAI6.PlayerType, humanType: THUAI6.HumanType, butcherType: THUAI6.ButcherType) -> Message2Server.PlayerMsg:
        return Message2Server.PlayerMsg(player_id=playerID, player_type=THUAI62Proto.playerTypeDict[playerType], human_type=THUAI62Proto.humanTypeDict[humanType], butcher_type=THUAI62Proto.butcherTypeDict[butcherType])

    @staticmethod
    def THUAI62ProtobufID(playerID: int) -> Message2Server.IDMsg:
        return Message2Server.IDMsg(player_id=playerID)

    @staticmethod
    def THUAI62ProtobufMove(time: int, angle: float, id: int) -> Message2Server.MoveMsg:
        return Message2Server.MoveMsg(player_id=id, angle=angle, time_in_milliseconds=time)

    @staticmethod
    def THUAI62ProtobufPick(prop: THUAI6.PropType, id: int) -> Message2Server.PickMsg:
        return Message2Server.PickMsg(player_id=id, prop_type=THUAI62Proto.propTypeDict[prop])

    @staticmethod
    def THUAI62ProtobufSend(msg: str, toID: int, id: int) -> Message2Server.SendMsg:
        return Message2Server.SendMsg(player_id=id, to_player_id=toID, message=msg)

    @staticmethod
    def THUAI62ProtobufAttack(angle: float, id: int) -> Message2Server.AttackMsg:
        return Message2Server.AttackMsg(player_id=id, angle=angle)
