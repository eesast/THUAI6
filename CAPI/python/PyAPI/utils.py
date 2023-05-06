import proto.MessageType_pb2 as MessageType
import proto.Message2Server_pb2 as Message2Server
import proto.Message2Clients_pb2 as Message2Clients
import PyAPI.structures as THUAI6
from typing import Final, List, Union

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
    def HaveView(
        viewRange: int,
        x: int,
        y: int,
        newX: int,
        newY: int,
        map: List[List[THUAI6.PlaceType]],
    ) -> bool:
        deltaX: int = newX - x
        deltaY: int = newY - y
        distance: float = deltaX**2 + deltaY**2
        myPlace = map[AssistFunction.GridToCell(x)][AssistFunction.GridToCell(y)]
        newPlace = map[AssistFunction.GridToCell(newX)][AssistFunction.GridToCell(newY)]
        if myPlace != THUAI6.PlaceType.Grass and newPlace == THUAI6.PlaceType.Grass:
            return False
        if distance <= viewRange * viewRange:
            divide: int = max(abs(deltaX), abs(deltaY)) // 100
            if divide == 0:
                return True
            dx: float = deltaX / divide
            dy: float = deltaY / divide
            selfX: float = float(x)
            selfY: float = float(y)
            if newPlace == THUAI6.PlaceType.Grass and myPlace == THUAI6.PlaceType.Grass:
                for i in range(divide):
                    selfX += dx
                    selfY += dy
                    if (
                        map[AssistFunction.GridToCell(int(selfX))][
                            AssistFunction.GridToCell(int(selfY))
                        ]
                        != THUAI6.PlaceType.Grass
                    ):
                        return False
                else:
                    return True
            else:
                for i in range(divide):
                    selfX += dx
                    selfY += dy
                    if (
                        map[AssistFunction.GridToCell(int(selfX))][
                            AssistFunction.GridToCell(int(selfY))
                        ]
                        == THUAI6.PlaceType.Wall
                    ):
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
        MessageType.DOOR3: THUAI6.PlaceType.Door3,
        MessageType.DOOR5: THUAI6.PlaceType.Door5,
        MessageType.DOOR6: THUAI6.PlaceType.Door6,
        MessageType.CHEST: THUAI6.PlaceType.Chest,
    }

    shapeTypeDict: Final[dict] = {
        MessageType.NULL_SHAPE_TYPE: THUAI6.ShapeType.NullShapeType,
        MessageType.SQUARE: THUAI6.ShapeType.Square,
        MessageType.CIRCLE: THUAI6.ShapeType.Circle,
    }

    propTypeDict: Final[dict] = {
        MessageType.NULL_PROP_TYPE: THUAI6.PropType.NullPropType,
        MessageType.KEY3: THUAI6.PropType.Key3,
        MessageType.KEY5: THUAI6.PropType.Key5,
        MessageType.KEY6: THUAI6.PropType.Key6,
        MessageType.ADD_SPEED: THUAI6.PropType.AddSpeed,
        MessageType.ADD_HP_OR_AP: THUAI6.PropType.AddHpOrAp,
        MessageType.ADD_LIFE_OR_CLAIRAUDIENCE: THUAI6.PropType.AddLifeOrClairaudience,
        MessageType.SHIELD_OR_SPEAR: THUAI6.PropType.ShieldOrSpear,
        MessageType.RECOVERY_FROM_DIZZINESS: THUAI6.PropType.RecoveryFromDizziness,
    }

    playerTypeDict: Final[dict] = {
        MessageType.NULL_PLAYER_TYPE: THUAI6.PlayerType.NullPlayerType,
        MessageType.STUDENT_PLAYER: THUAI6.PlayerType.StudentPlayer,
        MessageType.TRICKER_PLAYER: THUAI6.PlayerType.TrickerPlayer,
    }

    studentTypeDict: Final[dict] = {
        MessageType.NULL_STUDENT_TYPE: THUAI6.StudentType.NullStudentType,
        MessageType.ATHLETE: THUAI6.StudentType.Athlete,
        MessageType.TEACHER: THUAI6.StudentType.Teacher,
        MessageType.STRAIGHT_A_STUDENT: THUAI6.StudentType.StraightAStudent,
        MessageType.ROBOT: THUAI6.StudentType.Robot,
        MessageType.TECH_OTAKU: THUAI6.StudentType.TechOtaku,
        MessageType.SUNSHINE: THUAI6.StudentType.Sunshine,
    }

    trickerTypeDict: Final[dict] = {
        MessageType.NULL_TRICKER_TYPE: THUAI6.TrickerType.NullTrickerType,
        MessageType.ASSASSIN: THUAI6.TrickerType.Assassin,
        MessageType.KLEE: THUAI6.TrickerType.Klee,
        MessageType.A_NOISY_PERSON: THUAI6.TrickerType.ANoisyPerson,
        MessageType.IDOL: THUAI6.TrickerType.Idol,
    }

    studentBuffTypeDict: Final[dict] = {
        MessageType.NULL_SBUFF_TYPE: THUAI6.StudentBuffType.NullStudentBuffType,
        MessageType.STUDENT_ADD_SPEED: THUAI6.StudentBuffType.AddSpeed,
        MessageType.ADD_LIFE: THUAI6.StudentBuffType.AddLife,
        MessageType.SHIELD: THUAI6.StudentBuffType.Shield,
        MessageType.STUDENT_INVISIBLE: THUAI6.StudentBuffType.Invisible,
    }

    trickerBuffTypeDict: Final[dict] = {
        MessageType.NULL_TBUFF_TYPE: THUAI6.TrickerBuffType.NullTrickerBuffType,
        MessageType.TRICKER_ADD_SPEED: THUAI6.TrickerBuffType.AddSpeed,
        MessageType.SPEAR: THUAI6.TrickerBuffType.Spear,
        MessageType.ADD_AP: THUAI6.TrickerBuffType.AddAp,
        MessageType.CLAIRAUDIENCE: THUAI6.TrickerBuffType.Clairaudience,
        MessageType.TRICKER_INVISIBLE: THUAI6.TrickerBuffType.Invisible,
    }

    playerStateDict: Final[dict] = {
        MessageType.NULL_STATUS: THUAI6.PlayerState.NullState,
        MessageType.IDLE: THUAI6.PlayerState.Idle,
        MessageType.LEARNING: THUAI6.PlayerState.Learning,
        MessageType.ADDICTED: THUAI6.PlayerState.Addicted,
        MessageType.QUIT: THUAI6.PlayerState.Quit,
        MessageType.GRADUATED: THUAI6.PlayerState.Graduated,
        MessageType.RESCUED: THUAI6.PlayerState.Roused,
        MessageType.TREATED: THUAI6.PlayerState.Encouraged,
        MessageType.STUNNED: THUAI6.PlayerState.Stunned,
        MessageType.RESCUING: THUAI6.PlayerState.Rousing,
        MessageType.TREATING: THUAI6.PlayerState.Encouraging,
        MessageType.SWINGING: THUAI6.PlayerState.Swinging,
        MessageType.ATTACKING: THUAI6.PlayerState.Attacking,
        MessageType.LOCKING: THUAI6.PlayerState.Locking,
        # MessageType.RUMMAGING: THUAI6.PlayerState.Rummaging,
        MessageType.CLIMBING: THUAI6.PlayerState.Climbing,
        MessageType.OPENING_A_CHEST: THUAI6.PlayerState.OpeningAChest,
        MessageType.USING_SPECIAL_SKILL: THUAI6.PlayerState.UsingSpecialSkill,
        MessageType.OPENING_A_GATE: THUAI6.PlayerState.OpeningAGate,
    }

    gameStateDict: Final[dict] = {
        MessageType.NULL_GAME_STATE: THUAI6.GameState.NullGameState,
        MessageType.GAME_START: THUAI6.GameState.GameStart,
        MessageType.GAME_RUNNING: THUAI6.GameState.GameRunning,
        MessageType.GAME_END: THUAI6.GameState.GameEnd,
    }

    bulletTypeDict: Final[dict] = {
        MessageType.NULL_BULLET_TYPE: THUAI6.BulletType.NullBulletType,
        MessageType.FLYING_KNIFE: THUAI6.BulletType.FlyingKnife,
        MessageType.BOMB_BOMB: THUAI6.BulletType.BombBomb,
        MessageType.COMMON_ATTACK_OF_TRICKER: THUAI6.BulletType.CommonAttackOfTricker,
        MessageType.JUMPY_DUMPTY: THUAI6.BulletType.JumpyDumpty,
        MessageType.ATOM_BOMB: THUAI6.BulletType.AtomBomb,
    }

    # 用于将Proto的对象转为THUAI6的对象
    @staticmethod
    def Protobuf2THUAI6Tricker(
        trickerMsg: Message2Clients.MessageOfTricker,
    ) -> THUAI6.Tricker:
        tricker = THUAI6.Tricker()
        tricker.x = trickerMsg.x
        tricker.y = trickerMsg.y
        tricker.speed = trickerMsg.speed
        tricker.score = trickerMsg.score
        tricker.facingDirection = trickerMsg.facing_direction
        tricker.trickDesire = trickerMsg.trick_desire
        tricker.classVolume = trickerMsg.class_volume
        tricker.bulletType = Proto2THUAI6.bulletTypeDict[trickerMsg.bullet_type]
        for time in trickerMsg.time_until_skill_available:
            tricker.timeUntilSkillAvailable.append(time)
        tricker.playerState = Proto2THUAI6.playerStateDict[trickerMsg.player_state]
        for item in trickerMsg.prop:
            tricker.prop.append(Proto2THUAI6.propTypeDict[item])
        tricker.trickerType = Proto2THUAI6.trickerTypeDict[trickerMsg.tricker_type]
        tricker.guid = trickerMsg.guid
        tricker.playerID = trickerMsg.player_id
        tricker.viewRange = trickerMsg.view_range
        tricker.radius = trickerMsg.radius
        for buff in trickerMsg.buff:
            tricker.buff.append(Proto2THUAI6.trickerBuffTypeDict[buff])
        tricker.playerType = THUAI6.PlayerType.TrickerPlayer
        return tricker

    @staticmethod
    def Protobuf2THUAI6Student(
        studentMsg: Message2Clients.MessageOfStudent,
    ) -> THUAI6.Student:
        student = THUAI6.Student()
        student.x = studentMsg.x
        student.y = studentMsg.y
        student.speed = studentMsg.speed
        student.determination = studentMsg.determination
        student.addiction = studentMsg.addiction
        student.score = studentMsg.score
        student.facingDirection = studentMsg.facing_direction
        student.bulletType = Proto2THUAI6.bulletTypeDict[studentMsg.bullet_type]
        student.learningSpeed = studentMsg.learning_speed
        student.encourageSpeed = studentMsg.treat_speed
        student.encourageProgress = studentMsg.treat_progress
        student.rouseProgress = studentMsg.rescue_progress
        student.dangerAlert = studentMsg.danger_alert
        for time in studentMsg.time_until_skill_available:
            student.timeUntilSkillAvailable.append(time)
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
        student.playerType = THUAI6.PlayerType.StudentPlayer
        return student

    @staticmethod
    def Protobuf2THUAI6Prop(propMsg: Message2Clients.MessageOfProp) -> THUAI6.Prop:
        prop = THUAI6.Prop()
        prop.x = propMsg.x
        prop.y = propMsg.y
        prop.type = Proto2THUAI6.propTypeDict[propMsg.type]
        prop.guid = propMsg.guid
        prop.facingDirection = propMsg.facing_direction
        return prop

    @staticmethod
    def Protobuf2THUAI6GameInfo(allMsg: Message2Clients.MessageOfAll):
        gameInfo = THUAI6.GameInfo()
        gameInfo.gameTime = allMsg.game_time
        gameInfo.subjectFinished = allMsg.subject_finished
        gameInfo.studentGraduated = allMsg.student_graduated
        gameInfo.studentQuited = allMsg.student_quited
        gameInfo.studentScore = allMsg.student_score
        gameInfo.trickerScore = allMsg.tricker_score
        return gameInfo

    @staticmethod
    def Protobuf2THUAI6Bullet(
        bulletMsg: Message2Clients.MessageOfBullet,
    ) -> THUAI6.Bullet:
        bullet = THUAI6.Bullet()
        bullet.x = bulletMsg.x
        bullet.y = bulletMsg.y
        bullet.bulletType = Proto2THUAI6.bulletTypeDict[bulletMsg.type]
        bullet.facingDirection = bulletMsg.facing_direction
        bullet.guid = bulletMsg.guid
        bullet.team = Proto2THUAI6.playerTypeDict[bulletMsg.team]
        bullet.bombRange = bulletMsg.bomb_range
        return bullet

    @staticmethod
    def Protobuf2THUAI6BombedBullet(
        bulletMsg: Message2Clients.MessageOfBombedBullet,
    ) -> THUAI6.BombedBullet:
        bullet = THUAI6.BombedBullet()
        bullet.x = bulletMsg.x
        bullet.y = bulletMsg.y
        bullet.bulletType = Proto2THUAI6.bulletTypeDict[bulletMsg.type]
        bullet.facingDirection = bulletMsg.facing_direction
        bullet.mappingID = bulletMsg.mapping_id
        bullet.bombRange = bulletMsg.bomb_range
        return bullet

    @staticmethod
    def Bool2HiddenGateState(gateMsg: bool) -> THUAI6.HiddenGateState:
        if gateMsg:
            return THUAI6.HiddenGateState.Opened
        else:
            return THUAI6.HiddenGateState.Refreshed


class THUAI62Proto(NoInstance):
    placeTypeDict: Final[dict] = {
        THUAI6.PlaceType.NullPlaceType: MessageType.NULL_PLACE_TYPE,
        THUAI6.PlaceType.Wall: MessageType.WALL,
        THUAI6.PlaceType.Land: MessageType.LAND,
        THUAI6.PlaceType.Grass: MessageType.GRASS,
        THUAI6.PlaceType.ClassRoom: MessageType.CLASSROOM,
        THUAI6.PlaceType.Gate: MessageType.GATE,
        THUAI6.PlaceType.HiddenGate: MessageType.HIDDEN_GATE,
        THUAI6.PlaceType.Door3: MessageType.DOOR3,
        THUAI6.PlaceType.Door5: MessageType.DOOR5,
        THUAI6.PlaceType.Door6: MessageType.DOOR6,
        THUAI6.PlaceType.Chest: MessageType.CHEST,
        THUAI6.PlaceType.Window: MessageType.WINDOW,
    }

    playerTypeDict: Final[dict] = {
        THUAI6.PlayerType.NullPlayerType: MessageType.NULL_PLAYER_TYPE,
        THUAI6.PlayerType.StudentPlayer: MessageType.STUDENT_PLAYER,
        THUAI6.PlayerType.TrickerPlayer: MessageType.TRICKER_PLAYER,
    }

    studentTypeDict: Final[dict] = {
        THUAI6.StudentType.NullStudentType: MessageType.NULL_STUDENT_TYPE,
        THUAI6.StudentType.Athlete: MessageType.ATHLETE,
        THUAI6.StudentType.Teacher: MessageType.TEACHER,
        THUAI6.StudentType.StraightAStudent: MessageType.STRAIGHT_A_STUDENT,
        THUAI6.StudentType.Robot: MessageType.ROBOT,
        THUAI6.StudentType.TechOtaku: MessageType.TECH_OTAKU,
        THUAI6.StudentType.Sunshine: MessageType.SUNSHINE,
    }

    trickerTypeDict: Final[dict] = {
        THUAI6.TrickerType.NullTrickerType: MessageType.NULL_TRICKER_TYPE,
        THUAI6.TrickerType.Assassin: MessageType.ASSASSIN,
        THUAI6.TrickerType.Klee: MessageType.KLEE,
        THUAI6.TrickerType.ANoisyPerson: MessageType.A_NOISY_PERSON,
        THUAI6.TrickerType.Idol: MessageType.IDOL,
    }

    propTypeDict: Final[dict] = {
        THUAI6.PropType.NullPropType: MessageType.NULL_PROP_TYPE,
        THUAI6.PropType.Key3: MessageType.KEY3,
        THUAI6.PropType.Key5: MessageType.KEY5,
        THUAI6.PropType.Key6: MessageType.KEY6,
        THUAI6.PropType.AddHpOrAp: MessageType.ADD_HP_OR_AP,
        THUAI6.PropType.AddLifeOrClairaudience: MessageType.ADD_LIFE_OR_CLAIRAUDIENCE,
        THUAI6.PropType.AddSpeed: MessageType.ADD_SPEED,
        THUAI6.PropType.ShieldOrSpear: MessageType.SHIELD_OR_SPEAR,
    }

    # 用于将THUAI6的对象转为Proto的对象

    @staticmethod
    def THUAI62ProtobufPlayer(
        playerID: int,
        playerType: THUAI6.PlayerType,
        studentType: THUAI6.StudentType,
        trickerType: THUAI6.TrickerType,
    ) -> Message2Server.PlayerMsg:
        if playerType == THUAI6.PlayerType.StudentPlayer:
            return Message2Server.PlayerMsg(
                player_id=playerID,
                player_type=MessageType.STUDENT_PLAYER,
                student_type=THUAI62Proto.studentTypeDict[studentType],
            )
        else:
            return Message2Server.PlayerMsg(
                player_id=playerID,
                player_type=MessageType.TRICKER_PLAYER,
                tricker_type=THUAI62Proto.trickerTypeDict[trickerType],
            )

    @staticmethod
    def THUAI62ProtobufID(playerID: int) -> Message2Server.IDMsg:
        return Message2Server.IDMsg(player_id=playerID)

    @staticmethod
    def THUAI62ProtobufMove(time: int, angle: float, id: int) -> Message2Server.MoveMsg:
        return Message2Server.MoveMsg(
            player_id=id, angle=angle, time_in_milliseconds=time
        )

    @staticmethod
    def THUAI62ProtobufTreatAndRescue(
        playerID: int, mateID: int
    ) -> Message2Server.TreatAndRescueMsg:
        return Message2Server.TreatAndRescueMsg(player_id=playerID, to_player_id=mateID)

    @staticmethod
    def THUAI62ProtobufProp(prop: THUAI6.PropType, id: int) -> Message2Server.PropMsg:
        return Message2Server.PropMsg(
            player_id=id, prop_type=THUAI62Proto.propTypeDict[prop]
        )

    @staticmethod
    def THUAI62ProtobufSend(
        msg: Union[str, bytes], toID: int, id: int
    ) -> Message2Server.SendMsg:
        if isinstance(msg, str):
            return Message2Server.SendMsg(
                player_id=id, to_player_id=toID, text_message=msg
            )
        elif isinstance(msg, bytes):
            return Message2Server.SendMsg(
                player_id=id, to_player_id=toID, binary_message=msg
            )

    @staticmethod
    def THUAI62ProtobufAttack(angle: float, id: int) -> Message2Server.AttackMsg:
        return Message2Server.AttackMsg(player_id=id, angle=angle)

    @staticmethod
    def THUAI62ProtobufSkill(skillID: int, id: int) -> Message2Server.SkillMsg:
        return Message2Server.SkillMsg(player_id=id, skill_id=skillID)
