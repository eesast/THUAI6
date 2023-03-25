import proto.MessageType_pb2 as MessageType
import proto.Message2Server_pb2 as Message2Server
import proto.Message2Clients_pb2 as Message2Clients
import PyAPI.structures as THUAI6
from typing import Final, List

numOfGridPerCell: Final[int] = 1000

# 起到NameSpace的作用


class NoInstance:
    def __call__(self):
        raise TypeError("This class cannot be instantiated.")


class AssistFunction(NoInstance):

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell

    @staticmethod
    def HaveView(viewRange: int, x: int, y: int, newX: int, newY: int, myPlace: THUAI6.PlaceType, newPlace: THUAI6.PlaceType, map: List[List[THUAI6.PlaceType]]) -> bool:
        deltaX: int = newX - x
        deltaY: int = newY - y
        distance: float = deltaX**2 + deltaY**2
        if myPlace != THUAI6.PlaceType.Grass and newPlace == THUAI6.PlaceType.Grass:
            return False
        if distance <= viewRange * viewRange:
            divide: int = max(abs(deltaX), abs(deltaY)) // 100
            dx: float = deltaX / divide
            dy: float = deltaY / divide
            selfX: float = float(x)
            selfY: float = float(y)
            if newPlace == THUAI6.PlaceType.Grass and myPlace == THUAI6.PlaceType.Grass:
                for i in range(divide):
                    selfX += dx
                    selfY += dy
                    if map[AssistFunction.GridToCell(int(selfX))][AssistFunction.GridToCell(int(selfY))] != THUAI6.PlaceType.Grass:
                        return False
                else:
                    return True
            else:
                for i in range(divide):
                    selfX += dx
                    selfY += dy
                    if map[AssistFunction.GridToCell(int(selfX))][AssistFunction.GridToCell(int(selfY))] == THUAI6.PlaceType.Wall:
                        return False
                else:
                    return True
        else:
            return False


class Proto2THUAI6(NoInstance):
    placeTypeDict: Final[dict] = {
        MessageType.NULL_PLACE_TYPE: THUAI6.PlaceType.NullPlaceType,
        MessageType.WALL: THUAI6.PlaceType.Wall,
        MessageType.LAND: THUAI6.PlaceType.Land,
        MessageType.GRASS: THUAI6.PlaceType.Grass,
        MessageType.CLASSROOM: THUAI6.PlaceType.ClassRoom,
        MessageType.GATE: THUAI6.PlaceType.Gate,
        MessageType.HIDDEN_GATE: THUAI6.PlaceType.HiddenGate,
        MessageType.WINDOW: THUAI6.PlaceType.Window,
        MessageType.DOOR: THUAI6.PlaceType.Door,
    }

    shapeTypeDict: Final[dict] = {
        MessageType.NULL_SHAPE_TYPE: THUAI6.ShapeType.NullShapeType,
        MessageType.SQUARE: THUAI6.ShapeType.Square,
        MessageType.CIRCLE: THUAI6.ShapeType.Circle}

    propTypeDict: Final[dict] = {
        MessageType.NULL_PROP_TYPE: THUAI6.PropType.NullPropType,
        MessageType.PTYPE1: THUAI6.PropType.PropType1, }

    playerTypeDict: Final[dict] = {
        MessageType.NULL_PLAYER_TYPE: THUAI6.PlayerType.NullPlayerType,
        MessageType.STUDENT_PLAYER: THUAI6.PlayerType.StudentPlayer,
        MessageType.TRICKER_PLAYER: THUAI6.PlayerType.TrickerPlayer, }

    studentTypeDict: Final[dict] = {
        MessageType.NULL_STUDENT_TYPE: THUAI6.StudentType.NullStudentType,
        MessageType.STUDENTTYPE1: THUAI6.StudentType.StudentType1, }

    trickerTypeDict: Final[dict] = {
        MessageType.NULL_TRICKER_TYPE: THUAI6.TrickerType.NullTrickerType,
        MessageType.TRICKERTYPE1: THUAI6.TrickerType.TrickerType1, }

    studentBuffTypeDict: Final[dict] = {
        MessageType.NULL_SBUFF_TYPE: THUAI6.StudentBuffType.NullStudentBuffType,
        MessageType.SBUFFTYPE1: THUAI6.StudentBuffType.StudentBuffType1, }

    trickerBuffTypeDict: Final[dict] = {
        MessageType.NULL_TBUFF_TYPE: THUAI6.TrickerBuffType.NullTrickerBuffType,
        MessageType.TBUFFTYPE1: THUAI6.TrickerBuffType.TrickerBuffType1, }

    playerStateDict: Final[dict] = {
        MessageType.NULL_STATUS: THUAI6.PlayerState.NullState,
        MessageType.IDLE: THUAI6.PlayerState.Idle,
        MessageType.LEARNING: THUAI6.PlayerState.Learning,
        MessageType.ADDICTED: THUAI6.PlayerState.Addicted,
        MessageType.QUIT: THUAI6.PlayerState.Quit,
        MessageType.GRADUATED: THUAI6.PlayerState.Graduated,
        MessageType.RESCUED: THUAI6.PlayerState.Rescued,
        MessageType.TREATED: THUAI6.PlayerState.Treated,
        MessageType.STUNNED: THUAI6.PlayerState.Stunned,
        MessageType.RESCUING: THUAI6.PlayerState.Rescuing,
        MessageType.TREATING: THUAI6.PlayerState.Treating, }

    gameStateDict: Final[dict] = {
        MessageType.NULL_GAME_STATE: THUAI6.GameState.NullGameState,
        MessageType.GAME_START: THUAI6.GameState.GameStart,
        MessageType.GAME_RUNNING: THUAI6.GameState.GameRunning,
        MessageType.GAME_END: THUAI6.GameState.GameEnd}

    bulletTypeDict: Final[dict] = {
        MessageType.NULL_BULLET_TYPE: THUAI6.BulletType.NullBulletType,
        MessageType.COMMON_BULLET: THUAI6.BulletType.CommonBullet,
        MessageType.FAST_BULLET: THUAI6.BulletType.FastBullet,
        MessageType.LINE_BULLET: THUAI6.BulletType.LineBullet,
        MessageType.ORDINARY_BULLET: THUAI6.BulletType.OrdinaryBullet,
        MessageType.ATOM_BOMB: THUAI6.BulletType.AtomBomb, }

    # 用于将Proto的对象转为THUAI6的对象
    @staticmethod
    def Protobuf2THUAI6Tricker(trickerMsg: Message2Clients.MessageOfTricker) -> THUAI6.Tricker:
        tricker = THUAI6.Tricker()
        tricker.x = trickerMsg.x
        tricker.y = trickerMsg.y
        tricker.speed = trickerMsg.speed
        tricker.damage = trickerMsg.damage
        for time in trickerMsg.time_until_skill_available:
            tricker.timeUntilSkillAvailable.append(time)
        tricker.place = Proto2THUAI6.placeTypeDict[trickerMsg.place]
        tricker.playerState = Proto2THUAI6.playerStateDict[trickerMsg.player_state]
        for item in trickerMsg.prop:
            tricker.prop.append(Proto2THUAI6.propTypeDict[item])
        tricker.trickerType = Proto2THUAI6.trickerTypeDict[trickerMsg.tricker_type]
        tricker.guid = trickerMsg.guid
        tricker.playerID = trickerMsg.player_id
        tricker.viewRange = trickerMsg.view_range
        tricker.radius = trickerMsg.radius
        tricker.buff.clear()
        for buff in trickerMsg.buff:
            tricker.buff.append(Proto2THUAI6.trickerBuffTypeDict[buff])
        return tricker

    @staticmethod
    def Protobuf2THUAI6Student(studentMsg: Message2Clients.MessageOfStudent) -> THUAI6.Student:
        student = THUAI6.Student()
        student.x = studentMsg.x
        student.y = studentMsg.y
        student.speed = studentMsg.speed
        student.determination = studentMsg.determination
        student.addiction = studentMsg.addiction
        for time in studentMsg.time_until_skill_available:
            student.timeUntilSkillAvailable.append(time)
        student.damage = studentMsg.damage
        student.place = Proto2THUAI6.placeTypeDict[studentMsg.place]
        for item in studentMsg.prop:
            student.prop.append(Proto2THUAI6.propTypeDict[item])
        student.studentType = Proto2THUAI6.studentTypeDict[studentMsg.student_type]
        student.guid = studentMsg.guid
        student.playerState = Proto2THUAI6.playerStateDict[studentMsg.player_state]
        student.playerID = studentMsg.player_id
        student.viewRange = studentMsg.view_range
        student.radius = studentMsg.radius
        for buff in studentMsg.buff:
            student.buff.append(Proto2THUAI6.studentBuffTypeDict[buff])
        return student

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
    def Protobuf2THUAI6GameInfo(allMsg: Message2Clients.MessageOfAll):
        gameInfo = THUAI6.GameInfo()
        gameInfo.gameTime = allMsg.game_time
        gameInfo.subjectLeft = allMsg.subject_left
        gameInfo.studentGraduated = allMsg.student_graduated
        gameInfo.studentQuited = allMsg.student_quited
        gameInfo.studentScore = allMsg.student_score
        gameInfo.trickerScore = allMsg.tricker_score
        gameInfo.gateOpened = allMsg.gate_opened
        gameInfo.hiddenGateOpened = allMsg.hidden_gate_opened
        gameInfo.hiddenGateRefreshed = allMsg.hidden_gate_refreshed
        return gameInfo

    @staticmethod
    def Protobuf2THUAI6Bullet(bulletMsg: Message2Clients.MessageOfBullet) -> THUAI6.Bullet:
        bullet = THUAI6.Bullet()
        bullet.x = bulletMsg.x
        bullet.y = bulletMsg.y
        bullet.bulletType = Proto2THUAI6.bulletTypeDict[bulletMsg.type]
        bullet.facingDirection = bulletMsg.facing_direction
        bullet.guid = bulletMsg.guid
        bullet.team = Proto2THUAI6.playerTypeDict[bulletMsg.team]
        bullet.place = Proto2THUAI6.placeTypeDict[bulletMsg.place]
        bullet.bombRange = bulletMsg.bomb_range
        return bullet

    @staticmethod
    def Protobuf2THUAI6BombedBullet(bulletMsg: Message2Clients.MessageOfBombedBullet) -> THUAI6.BombedBullet:
        bullet = THUAI6.BombedBullet()
        bullet.x = bulletMsg.x
        bullet.y = bulletMsg.y
        bullet.bulletType = Proto2THUAI6.bulletTypeDict[bulletMsg.type]
        bullet.facingDirection = bulletMsg.facing_direction
        bullet.mappingID = bulletMsg.mapping_id
        bullet.bombRange = bulletMsg.bomb_range
        return bullet

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
        THUAI6.PlaceType.ClassRoom: MessageType.CLASSROOM,
        THUAI6.PlaceType.Gate: MessageType.GATE,
        THUAI6.PlaceType.HiddenGate: MessageType.HIDDEN_GATE,
        THUAI6.PlaceType.Door: MessageType.DOOR,
        THUAI6.PlaceType.Chest: MessageType.CHEST,
        THUAI6.PlaceType.Window: MessageType.WINDOW, }

    playerTypeDict: Final[dict] = {
        THUAI6.PlayerType.NullPlayerType: MessageType.NULL_PLAYER_TYPE,
        THUAI6.PlayerType.StudentPlayer: MessageType.STUDENT_PLAYER,
        THUAI6.PlayerType.TrickerPlayer: MessageType.TRICKER_PLAYER}

    studentTypeDict: Final[dict] = {
        THUAI6.StudentType.NullStudentType: MessageType.NULL_STUDENT_TYPE,
        THUAI6.StudentType.StudentType1: MessageType.STUDENTTYPE1, }

    trickerTypeDict: Final[dict] = {
        THUAI6.TrickerType.NullTrickerType: MessageType.NULL_TRICKER_TYPE,
        THUAI6.TrickerType.TrickerType1: MessageType.TRICKERTYPE1, }

    propTypeDict: Final[dict] = {
        THUAI6.PropType.NullPropType: MessageType.NULL_PROP_TYPE,
        THUAI6.PropType.PropType1: MessageType.PTYPE1, }

    # 用于将THUAI6的对象转为Proto的对象

    @staticmethod
    def THUAI62ProtobufPlayer(playerID: int, playerType: THUAI6.PlayerType, studentType: THUAI6.StudentType, trickerType: THUAI6.TrickerType) -> Message2Server.PlayerMsg:
        return Message2Server.PlayerMsg(player_id=playerID, player_type=THUAI62Proto.playerTypeDict[playerType], student_type=THUAI62Proto.studentTypeDict[studentType], tricker_type=THUAI62Proto.trickerTypeDict[trickerType])

    @staticmethod
    def THUAI62ProtobufID(playerID: int) -> Message2Server.IDMsg:
        return Message2Server.IDMsg(player_id=playerID)

    @staticmethod
    def THUAI62ProtobufMove(time: int, angle: float, id: int) -> Message2Server.MoveMsg:
        return Message2Server.MoveMsg(player_id=id, angle=angle, time_in_milliseconds=time)

    @staticmethod
    def THUAI62ProtobufPick(prop: THUAI6.PropType, id: int) -> Message2Server.PropMsg:
        return Message2Server.PropMsg(player_id=id, prop_type=THUAI62Proto.propTypeDict[prop])

    @staticmethod
    def THUAI62ProtobufSend(msg: str, toID: int, id: int) -> Message2Server.SendMsg:
        return Message2Server.SendMsg(player_id=id, to_player_id=toID, message=msg)

    @staticmethod
    def THUAI62ProtobufTrick(angle: float, id: int) -> Message2Server.AttackMsg:
        return Message2Server.AttackMsg(player_id=id, angle=angle)
