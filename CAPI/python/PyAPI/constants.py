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
    basicTreatSpeed = 100
    basicFixSpeed = 123
    basicSpeedOfOpeningOrLocking = 4000
    basicStudentSpeedOfClimbingThroughWindows = 611
    basicTrickerSpeedOfClimbingThroughWindows = 1270
    basicSpeedOfOpenChest = 1000

    basicHp = 3000000
    basicMaxGamingAddiction = 60000
    BeginGamingAddiction = 20900
    MidGamingAddiction = 40300
    basicTreatmentDegree = 1500000
    basicTimeOfRescue = 1000

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
        moveSpeed = 1.1
        maxHp = 1.0
        concealment = 1.5
        alertnessRadius = 1.3
        viewRange = 1.2
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.0
        speedOfOpenChest = 1.0

    class Klee:
        moveSpeed = 1.0
        maxHp = 1.0
        concealment = 1.0
        alertnessRadius = 1.0
        viewRange = 1.0
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.0
        speedOfOpenChest = 1.1

    class ANoisyPerson:
        moveSpeed = 1.07
        maxHp = 1.2
        concealment = 0.8
        alertnessRadius = 0.9
        viewRange = 1.0
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.1
        speedOfOpenChest = 1.1

    class Idol:
        moveSpeed = 1.0
        maxHp = 1.0
        concealment = 0.75
        alertnessRadius = 1.0
        viewRange = 1.1
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.0
        speedOfOpenChest = 1.0

    class Athlete:
        moveSpeed = 1.1
        maxHp = 1.0
        maxAddiction = 0.9
        fixSpeed = 0.6
        treatSpeed = 0.8
        concealment = 0.9
        alertnessRadius = 1.0
        viewRange = 1.1
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.2
        speedOfOpenChest = 1.0

    class Teacher:
        moveSpeed = 0.75
        maxHp = 10.0
        maxAddiction = 10.0
        fixSpeed = 0.0
        treatSpeed = 0.7
        concealment = 0.5
        alertnessRadius = 0.5
        viewRange = 0.9
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 0.5
        speedOfOpenChest = 1.0

    class StraightAStudent:
        moveSpeed = 0.8
        maxHp = 1.1
        maxAddiction = 1.3
        fixSpeed = 1.1
        treatSpeed = 0.8
        concealment = 0.9
        alertnessRadius = 0.9
        viewRange = 0.9
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 0.83333
        speedOfOpenChest = 1.0

    class Robot:
        moveSpeed = 1.0
        maxHp = 0.4
        maxAddiction = 0.0
        fixSpeed = 1.0
        treatSpeed = 0.8
        concealment = 1.0
        alertnessRadius = 1.0
        viewRange = 1.0
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 0.0016
        speedOfOpenChest = 1.0

    class TechOtaku:
        moveSpeed = 0.75
        maxHp = 0.9
        maxAddiction = 1.1
        fixSpeed = 1.1
        treatSpeed = 0.9
        concealment = 1.0
        alertnessRadius = 1.0
        viewRange = 0.9
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 0.75
        speedOfOpenChest = 1.0

    class Sunshine:
        moveSpeed = 1.0
        maxHp = 1.0667
        maxAddiction = 1.1
        fixSpeed = 1.0
        treatSpeed = 2.0
        concealment = 1.0
        alertnessRadius = 1.0
        viewRange = 1.0
        speedOfOpeningOrLocking = 1.0
        speedOfClimbingThroughWindows = 1.0
        speedOfOpenChest = 1.0

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
        skillCD = 0.8
        durationTime = 0.5

    class BecomeInvisible:
        skillCD = 2.0
        durationTime = 0.6

    class Punish:
        skillCD = 1.0
        durationTime = 0.0

    class Rouse:
        skillCD = 2.0
        durationTime = 0.0

    class Encourage:
        skillCD = 2.0
        durationTime = 0.0

    class Inspire:
        skillCD = 2.0
        durationTime = 0.0

    class Howl:
        skillCD = 0.8333
        durationTime = 0.0

    class ShowTime:
        skillCD = 3.0
        durationTime = 1.0

    class JumpyBomb:
        skillCD = 0.5
        durationTime = 0.3

    class UseKnife:
        skillCD = 1.0
        durationTime = 0.1

    class UseRobot:
        skillCD = 0.0017
        durationTime = 0.0

    class WriteAnswers:
        skillCD = 1.0
        durationTime = 0.0

    class SummonGolem:
        skillCD = 1.0
        durationTime = 0.0
