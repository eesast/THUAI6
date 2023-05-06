from ast import Constant
from asyncio import constants


class NoInstance:
    def __call__(self):
        raise TypeError("This class cannot be instantiated.")


class Constants(NoInstance):
    frameDuration = 50  # 每帧毫秒数
    numOfGridPerCell = 1000  # 单位坐标数
    rows = 50  # 地图行数
    cols = 50  # 地图列数

    numOfClassroom = 10  # 教室数量
    numOfChest = 8  # 宝箱数量

    maxClassroomProgress = 10000000  # 教室最大进度
    maxDoorProgress = 10000000  # 开关门最大进度
    maxChestProgress = 10000000  # 宝箱最大进度
    maxGateProgress = 18000  # 大门最大进度

    numOfRequiredClassroomForGate = 7  # 打开大门需要完成的教室数量
    numOfRequiredClassroomForHiddenGate = 3  # 打开隐藏门需要完成的教室数量

    # 人物属性相关
    basicEncourageSpeed = 100
    basicLearnSpeed = 123
    basicSpeedOfOpeningOrLocking = 4000
    basicStudentSpeedOfClimbingThroughWindows = 1222
    basicTrickerSpeedOfClimbingThroughWindows = 2540
    basicSpeedOfOpenChest = 1000

    basicHp = 3000000
    basicMaxGamingAddiction = 60000
    basicEncouragementDegree = 1500000
    basicTimeOfRouse = 1000

    basicStudentSpeed = 3000
    basicTrickerSpeed = 3600

    basicConcealment = 1.0
    basicStudentAlertnessRadius = 15 * numOfGridPerCell
    basicTrickerAlertnessRadius = 17 * numOfGridPerCell
    basicStudentViewRange = 10 * numOfGridPerCell
    basicTrickerViewRange = 13 * numOfGridPerCell
    PinningDownRange = 5 * numOfGridPerCell

    maxNumOfProp = 3  # 人物道具栏容量

    # 攻击相关

    basicApOfTricker = 1500000
    basicCD = 3000  # 初始子弹冷却
    basicCastTime = 500  # 基本前摇时间
    basicBackswing = 800  # 基本后摇时间
    basicRecoveryFromHit = 3700  # 基本命中攻击恢复时长
    basicStunnedTimeOfStudent = 4300

    basicBulletMoveSpeed = 7400  # 基本子弹移动速度
    basicRemoteAttackRange = 6000  # 基本远程攻击范围
    basicAttackShortRange = 2200  # 基本近程攻击范围
    basicBulletBombRange = 2000  # 基本子弹爆炸范围

    # 道具相关

    apPropAdd = basicApOfTricker * 12 / 10
    apSpearAdd = basicApOfTricker * 6 / 10

    # 技能相关
    maxNumOfSkill = 3
    commonSkillCD = 30000  # 普通技能标准冷却时间
    commonSkillTime = 10000  # 普通技能标准持续时间

    timeOfTrickerStunnedWhenCharge = 7220
    timeOfStudentStunnedWhenCharge = 2090

    timeOfTrickerStunnedWhenPunish = 3070

    timeOfTrickerSwingingAfterHowl = 800
    timeOfStudentStunnedWhenHowl = 5500

    timeOfStunnedWhenJumpyDumpty = 3070

    addedTimeOfSpeedWhenInspire = 1.6
    timeOfAddingSpeedWhenInspire = 6000
    addHpWhenEncourage = basicHp / 4

    checkIntervalWhenShowTime = 200
    addAddictionPer100msWhenShowTime = 300


class Assassin:
    moveSpeed = (int)(1.1 * Constants.basicTrickerSpeed)
    concealment = 1.5 * Constants.basicConcealment
    alertnessRadius = (int)(1.3 * Constants.basicTrickerAlertnessRadius)
    viewRange = (int)(1.2 * Constants.basicTrickerViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.0 * Constants.basicTrickerSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class Klee:
    moveSpeed = (int)(1.0 * Constants.basicTrickerSpeed)
    concealment = 1.0 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicTrickerAlertnessRadius)
    viewRange = (int)(1.0 * Constants.basicTrickerViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.0 * Constants.basicTrickerSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.1 * Constants.basicSpeedOfOpenChest)


class ANoisyPerson:
    moveSpeed = (int)(1.07 * Constants.basicTrickerSpeed)
    concealment = 0.8 * Constants.basicConcealment
    alertnessRadius = (int)(0.9 * Constants.basicTrickerAlertnessRadius)
    viewRange = (int)(1.0 * Constants.basicTrickerViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.1 * Constants.basicTrickerSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.1 * Constants.basicSpeedOfOpenChest)


class Idol:
    moveSpeed = (int)(1.0 * Constants.basicTrickerSpeed)
    concealment = 0.75 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicTrickerAlertnessRadius)
    viewRange = (int)(1.1 * Constants.basicTrickerViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.0 * Constants.basicTrickerSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class Athlete:
    moveSpeed = (int)(1.05 * Constants.basicStudentSpeed)
    maxHp = (int)(1.0 * Constants.basicHp)
    maxAddiction = (int)(0.9 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(0.6 * Constants.basicLearnSpeed)
    EncourageSpeed = (int)(0.9 * Constants.basicEncourageSpeed)
    concealment = 0.9 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(1.1 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.2 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class Teacher:
    moveSpeed = (int)(0.9 * Constants.basicStudentSpeed)
    maxHp = (int)(10.0 * Constants.basicHp)
    maxAddiction = (int)(10.0 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(0.0 * Constants.basicLearnSpeed)
    EncourageSpeed = (int)(0.8 * Constants.basicEncourageSpeed)
    concealment = 0.5 * Constants.basicConcealment
    alertnessRadius = (int)(0.5 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(0.9 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        0.5 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class StraightAStudent:
    moveSpeed = (int)(0.96 * Constants.basicStudentSpeed)
    maxHp = (int)(1.1 * Constants.basicHp)
    maxAddiction = (int)(1.3 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(1.1 * Constants.basicLearnSpeed)
    EncourageSpeed = (int)(Constants.basicEncourageSpeed)
    concealment = 0.9 * Constants.basicConcealment
    alertnessRadius = (int)(0.9 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(0.9 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        0.83333 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class Robot:
    moveSpeed = (int)(1.0 * Constants.basicStudentSpeed)
    maxHp = (int)(0.4 * Constants.basicHp)
    maxAddiction = (int)(0.0 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(1.0 * Constants.basicLearnSpeed)
    EncourageSpeed = 0
    concealment = 1.0 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(1.0 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        0.0016 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class TechOtaku:
    moveSpeed = (int)(0.75 * Constants.basicStudentSpeed)
    maxHp = (int)(0.9 * Constants.basicHp)
    maxAddiction = (int)(1.1 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(1.1 * Constants.basicLearnSpeed)
    EncourageSpeed = (int)(0.9 * Constants.basicEncourageSpeed)
    concealment = 1.0 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(0.9 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(1.0 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        0.75 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(1.0 * Constants.basicSpeedOfOpenChest)


class Sunshine:
    moveSpeed = (int)(1.0 * Constants.basicStudentSpeed)
    maxHp = (int)(1.0667 * Constants.basicHp)
    maxAddiction = (int)(1.1 * Constants.basicMaxGamingAddiction)
    LearnSpeed = (int)(1.0 * Constants.basicLearnSpeed)
    EncourageSpeed = (int)(1.2 * Constants.basicEncourageSpeed)
    concealment = 1.0 * Constants.basicConcealment
    alertnessRadius = (int)(1.0 * Constants.basicStudentAlertnessRadius)
    viewRange = (int)(1.0 * Constants.basicStudentViewRange)
    speedOfOpeningOrLocking = (int)(0.7 * Constants.basicSpeedOfOpeningOrLocking)
    speedOfClimbingThroughWindows = (int)(
        1.0 * Constants.basicStudentSpeedOfClimbingThroughWindows
    )
    speedOfOpenChest = (int)(0.9 * Constants.basicSpeedOfOpenChest)


class CanBeginToCharge:
    skillCD = (int)(2 * Constants.commonSkillCD)
    durationTime = (int)(0.3 * Constants.commonSkillTime)


class BecomeInvisible:
    skillCD = (int)(4 * Constants.commonSkillCD / 3)
    durationTime = (int)(Constants.commonSkillTime)


class Punish:
    skillCD = (int)(1.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class Rouse:
    skillCD = (int)(4.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class Encourage:
    skillCD = (int)(4.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class Inspire:
    skillCD = (int)(4.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class Howl:
    skillCD = (int)(0.8333 * Constants.commonSkillCD)
    durationTime = (int)(0 * Constants.commonSkillTime)


class ShowTime:
    skillCD = (int)(8 * Constants.commonSkillCD / 3)
    durationTime = (int)(1.0 * Constants.commonSkillTime)


class JumpyBomb:
    skillCD = (int)(0.5 * Constants.commonSkillCD)
    durationTime = (int)(0.3 * Constants.commonSkillTime)


class UseKnife:
    skillCD = (int)(1.0 * Constants.commonSkillCD)
    durationTime = (int)(0.1 * Constants.commonSkillTime)


class UseRobot:
    skillCD = (int)(0.0017 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class WriteAnswers:
    skillCD = (int)(1.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class SummonGolem:
    skillCD = (int)(1.0 * Constants.commonSkillCD)
    durationTime = (int)(0.0 * Constants.commonSkillTime)


class CommonAttackOfTricker:
    BulletBombRange = 0
    BulletAttackRange = Constants.basicAttackShortRange
    ap = Constants.basicApOfTricker
    Speed = Constants.basicBulletMoveSpeed
    IsRemoteAttack = False
    CastTime = BulletAttackRange * 1000 / Speed
    Backswing = Constants.basicBackswing
    RecoveryFromHit = Constants.basicRecoveryFromHit
    cd = Constants.basicBackswing
    maxBulletNum = 1


class FlyingKnife:
    BulletBombRange = 0
    BulletAttackRange = Constants.basicRemoteAttackRange * 13
    ap = Constants.basicApOfTricker * 4 / 5
    Speed = Constants.basicBulletMoveSpeed * 25 / 10
    IsRemoteAttack = True
    CastTime = Constants.basicCastTime * 4 / 5
    Backswing = 0
    RecoveryFromHit = 0
    cd = Constants.basicBackswing / 2
    maxBulletNum = 1


class BombBomb:
    BulletBombRange = Constants.basicBulletBombRange
    BulletAttackRange = Constants.basicAttackShortRange
    ap = Constants.basicApOfTricker * 6 / 5
    Speed = Constants.basicBulletMoveSpeed * 30 / 37
    IsRemoteAttack = False
    CastTime = BulletAttackRange * 1000 / Speed
    Backswing = Constants.basicRecoveryFromHit
    RecoveryFromHit = Constants.basicRecoveryFromHit
    cd = Constants.basicCD
    maxBulletNum = 1


class JumpyDumpty:
    BulletBombRange = Constants.basicBulletBombRange / 2
    BulletAttackRange = Constants.basicRemoteAttackRange * 2
    ap = (int)(Constants.basicApOfTricker * 0.6)
    Speed = Constants.basicBulletMoveSpeed * 43 / 37
    IsRemoteAttack = False
