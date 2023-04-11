class NoInstance:
    def __call__(self):
        raise TypeError("This class cannot be instantiated.")


class Constants(NoInstance):
    numOfGridPerCell = 1000  # 单位坐标数
    rows = 50                # 地图行数
    cols = 50                # 地图列数

    numOfClassroom = 10  # 教室数量
    numOfChest = 8       # 宝箱数量

    maxClassroomProgress = 10000000  # 教室最大进度
    maxDoorProgress = 10000000       # 开关门最大进度
    maxChestProgress = 10000000      # 宝箱最大进度
    maxGateProgress = 18000          # 大门最大进度

    numOfRequiredClassroomForGate = 7        # 打开大门需要完成的教室数量
    numOfRequiredClassroomForHiddenGate = 3  # 打开隐藏门需要完成的教室数量

    # 人物属性相关
    basicEncourageSpeed = 100
    basicLearnSpeed = 123
    basicSpeedOfOpeningOrLocking = 4000
    basicStudentSpeedOfClimbingThroughWindows = 611
    basicTrickerSpeedOfClimbingThroughWindows = 1270
    basicSpeedOfOpenChest = 1000

    basicHp = 3000000
    basicMaxGamingAddiction = 60000
    BeginGamingAddiction = 20900
    MidGamingAddiction = 40300
    basicEncouragementDegree = 1500000
    basicTimeOfRouse = 1000

    basicStudentSpeed = 1270
    basicTrickerSpeed = 1504

    maxSpeed = 12000

    basicConcealment = 1.0
    basicStudentAlertnessRadius = 15 * numOfGridPerCell
    basicTrickerAlertnessRadius = 17 * numOfGridPerCell
    basicStudentViewRange = 10 * numOfGridPerCell
    basicTrickerViewRange = 15 * numOfGridPerCell

    maxNumOfProp = 3

    # 攻击相关

    basicApOfTricker = 1500000
    basicCD = 3000               # 初始子弹冷却
    basicCastTime = 500          # 基本前摇时间
    basicBackswing = 800         # 基本后摇时间
    basicRecoveryFromHit = 3700  # 基本命中攻击恢复时长
    basicStunnedTimeOfStudent = 4300

    basicBulletMoveSpeed = 3700       # 基本子弹移动速度
    basicRemoteAttackRange = 3000  # 基本远程攻击范围
    basicAttackShortRange = 1100   # 基本近程攻击范围
    basicBulletBombRange = 1000    # 基本子弹爆炸范围

    # 道具相关

    apPropAdd = basicApOfTricker * 12 / 10
    apSpearAdd = basicApOfTricker * 6 / 10
    remainHpWhenAddLife = 100

    class Assassin:
        moveSpeed = 1.1*basicTrickerSpeed
        concealment = 1.5*basicConcealment
        alertnessRadius = 1.3*basicTrickerAlertnessRadius
        viewRange = 1.2*basicTrickerViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.0*basicTrickerSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class Klee:
        moveSpeed = 1.0*basicTrickerSpeed
        concealment = 1.0*basicConcealment
        alertnessRadius = 1.0*basicTrickerAlertnessRadius
        viewRange = 1.0*basicTrickerViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.0*basicTrickerSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.1*basicSpeedOfOpenChest

    class ANoisyPerson:
        moveSpeed = 1.07*basicTrickerSpeed
        concealment = 0.8*basicConcealment
        alertnessRadius = 0.9*basicTrickerAlertnessRadius
        viewRange = 1.0*basicTrickerViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.1*basicTrickerSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.1*basicSpeedOfOpenChest

    class Idol:
        moveSpeed = 1.0*basicTrickerSpeed
        concealment = 0.75*basicConcealment
        alertnessRadius = 1.0*basicTrickerAlertnessRadius
        viewRange = 1.1*basicTrickerViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.0*basicTrickerSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class Athlete:
        moveSpeed = 1.1*basicStudentSpeed
        maxHp = 1.0*basicHp
        maxAddiction = 0.9*basicMaxGamingAddiction
        LearnSpeed = 0.6*basicLearnSpeed
        EncourageSpeed = 0.8*basicEncourageSpeed
        concealment = 0.9*basicConcealment
        alertnessRadius = 1.0*basicStudentAlertnessRadius
        viewRange = 1.1*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.2*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class Teacher:
        moveSpeed = 0.75*basicStudentSpeed
        maxHp = 10.0*basicHp
        maxAddiction = 10.0*basicMaxGamingAddiction
        LearnSpeed = 0.0*basicLearnSpeed
        EncourageSpeed = 0.7*basicEncourageSpeed
        concealment = 0.5*basicConcealment
        alertnessRadius = 0.5*basicStudentAlertnessRadius
        viewRange = 0.9*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 0.5*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class StraightAStudent:
        moveSpeed = 0.8*basicStudentSpeed
        maxHp = 1.1*basicHp
        maxAddiction = 1.3*basicMaxGamingAddiction
        LearnSpeed = 1.1*basicLearnSpeed
        EncourageSpeed = 0.8*basicEncourageSpeed
        concealment = 0.9*basicConcealment
        alertnessRadius = 0.9*basicStudentAlertnessRadius
        viewRange = 0.9*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 0.83333*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class Robot:
        moveSpeed = 1.0*basicStudentSpeed
        maxHp = 0.4*basicHp
        maxAddiction = 0.0*basicMaxGamingAddiction
        LearnSpeed = 1.0*basicLearnSpeed
        EncourageSpeed = 0.8*basicEncourageSpeed
        concealment = 1.0*basicConcealment
        alertnessRadius = 1.0*basicStudentAlertnessRadius
        viewRange = 1.0*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 0.0016*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class TechOtaku:
        moveSpeed = 0.75*basicStudentSpeed
        maxHp = 0.9*basicHp
        maxAddiction = 1.1*basicMaxGamingAddiction
        LearnSpeed = 1.1*basicLearnSpeed
        EncourageSpeed = 0.9*basicEncourageSpeed
        concealment = 1.0*basicConcealment
        alertnessRadius = 1.0*basicStudentAlertnessRadius
        viewRange = 0.9*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 0.75*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    class Sunshine:
        moveSpeed = 1.0*basicStudentSpeed
        maxHp = 1.0667*basicHp
        maxAddiction = 1.1*basicMaxGamingAddiction
        LearnSpeed = 1.0*basicLearnSpeed
        EncourageSpeed = 2.0*basicEncourageSpeed
        concealment = 1.0*basicConcealment
        alertnessRadius = 1.0*basicStudentAlertnessRadius
        viewRange = 1.0*basicStudentViewRange
        speedOfOpeningOrLocking = 1.0*basicSpeedOfOpeningOrLocking
        speedOfClimbingThroughWindows = 1.0*basicStudentSpeedOfClimbingThroughWindows
        speedOfOpenChest = 1.0*basicSpeedOfOpenChest

    # 技能相关
    maxNumOfSkill = 3
    commonSkillCD = 30000  # 普通技能标准冷却时间
    commonSkillTime = 10000  # 普通技能标准持续时间

    timeOfTrickerFaintingWhenCharge = 7220
    timeOfStudentFaintingWhenCharge = 2090

    timeOfTrickerFaintingWhenPunish = 3070

    timeOfTrickerSwingingAfterHowl = 3070
    timeOfStudentFaintingWhenHowl = 6110

    timeOfStunnedWhenJumpyDumpty = 3070

    addedTimeOfSpeedWhenInspire = 0.6
    timeOfAddingSpeedWhenInspire = 6000

    class CanBeginToCharge:
        skillCD = 0.8*commonSkillCD
        durationTime = 0.5*commonSkillTime

    class BecomeInvisible:
        skillCD = 2.0*commonSkillCD
        durationTime = 0.6*commonSkillTime

    class Punish:
        skillCD = 1.0*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class Rouse:
        skillCD = 2.0*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class Encourage:
        skillCD = 2.0*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class Inspire:
        skillCD = 2.0*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class Howl:
        skillCD = 0.8333*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class ShowTime:
        skillCD = 3.0*commonSkillCD
        durationTime = 1.0*commonSkillTime

    class JumpyBomb:
        skillCD = 0.5*commonSkillCD
        durationTime = 0.3*commonSkillTime

    class UseKnife:
        skillCD = 1.0*commonSkillCD
        durationTime = 0.1*commonSkillTime

    class UseRobot:
        skillCD = 0.0017*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class WriteAnswers:
        skillCD = 1.0*commonSkillCD
        durationTime = 0.0*commonSkillTime

    class SummonGolem:
        skillCD = 1.0*commonSkillCD
        durationTime = 0.0*commonSkillTime
