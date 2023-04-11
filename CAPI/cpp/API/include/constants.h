#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef SCCI
#define SCCI static const constexpr inline
#endif

namespace Constants
{
    // 地图相关
    SCCI int numOfGridPerCell = 1000;  // 单位坐标数
    SCCI int rows = 50;                // 地图行数
    SCCI int cols = 50;                // 地图列数

    SCCI int numOfClassroom = 10;  // 教室数量
    SCCI int numOfChest = 8;       // 宝箱数量

    SCCI int maxClassroomProgress = 10000000;  // 教室最大进度
    SCCI int maxDoorProgress = 10000000;       // 开关门最大进度
    SCCI int maxChestProgress = 10000000;      // 宝箱最大进度
    SCCI int maxGateProgress = 18000;          // 大门最大进度

    SCCI int numOfRequiredClassroomForGate = 7;        // 打开大门需要完成的教室数量
    SCCI int numOfRequiredClassroomForHiddenGate = 3;  // 打开隐藏门需要完成的教室数量

    // 人物属性相关
    SCCI int basicTreatSpeed = 100;
    SCCI int basicFixSpeed = 123;
    SCCI int basicSpeedOfOpeningOrLocking = 4000;
    SCCI int basicStudentSpeedOfClimbingThroughWindows = 611;
    SCCI int basicTrickerSpeedOfClimbingThroughWindows = 1270;
    SCCI int basicSpeedOfOpenChest = 1000;

    SCCI int basicHp = 3000000;
    SCCI int basicMaxGamingAddiction = 60000;
    SCCI int BeginGamingAddiction = 20900;
    SCCI int MidGamingAddiction = 40300;
    SCCI int basicTreatmentDegree = 1500000;
    SCCI int basicTimeOfRescue = 1000;

    SCCI int basicStudentSpeed = 1270;
    SCCI int basicTrickerSpeed = 1504;

    SCCI int maxSpeed = 12000;

    SCCI double basicConcealment = 1.0;
    SCCI int basicStudentAlertnessRadius = 15 * numOfGridPerCell;
    SCCI int basicTrickerAlertnessRadius = 17 * numOfGridPerCell;
    SCCI int basicStudentViewRange = 10 * numOfGridPerCell;
    SCCI int basicTrickerViewRange = 15 * numOfGridPerCell;

    SCCI int maxNumOfProp = 3;

    // 攻击相关

    SCCI int basicApOfTricker = 1500000;
    SCCI int basicCD = 3000;               // 初始子弹冷却
    SCCI int basicCastTime = 500;          // 基本前摇时间
    SCCI int basicBackswing = 800;         // 基本后摇时间
    SCCI int basicRecoveryFromHit = 3700;  // 基本命中攻击恢复时长
    SCCI int basicStunnedTimeOfStudent = 4300;

    SCCI int basicBulletMoveSpeed = 3700;       // 基本子弹移动速度
    SCCI double basicRemoteAttackRange = 3000;  // 基本远程攻击范围
    SCCI double basicAttackShortRange = 1100;   // 基本近程攻击范围
    SCCI double basicBulletBombRange = 1000;    // 基本子弹爆炸范围

    // 道具相关

    SCCI int apPropAdd = basicApOfTricker * 12 / 10;
    SCCI int apSpearAdd = basicApOfTricker * 6 / 10;
    SCCI int remainHpWhenAddLife = 100;

    // 职业相关
    struct Assassin
    {
        SCCI double moveSpeed = 1.1;
        SCCI double maxHp = 1.0;
        SCCI double concealment = 1.5;
        SCCI double alertnessRadius = 1.3;
        SCCI double viewRange = 1.2;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.0;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct Klee
    {
        SCCI double moveSpeed = 1.0;
        SCCI double maxHp = 1.0;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 1.0;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.0;
        SCCI double speedOfOpenChest = 1.1;
    };

    struct ANoisyPerson
    {
        SCCI double moveSpeed = 1.07;
        SCCI double maxHp = 1.2;
        SCCI double concealment = 0.8;
        SCCI double alertnessRadius = 0.9;
        SCCI double viewRange = 1.0;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.1;
        SCCI double speedOfOpenChest = 1.1;
    };

    struct Idol
    {
        SCCI double moveSpeed = 1.0;
        SCCI double maxHp = 1.0;
        SCCI double concealment = 0.75;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 1.1;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.0;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct Athlete
    {
        SCCI double moveSpeed = 1.1;
        SCCI double maxHp = 1.0;
        SCCI double maxAddiction = 0.9;
        SCCI double fixSpeed = 0.6;
        SCCI double treatSpeed = 0.8;
        SCCI double concealment = 0.9;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 1.1;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.2;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct Teacher
    {
        SCCI double moveSpeed = 0.75;
        SCCI double maxHp = 10.0;
        SCCI double maxAddiction = 10.0;
        SCCI double fixSpeed = 0.0;
        SCCI double treatSpeed = 0.7;
        SCCI double concealment = 0.5;
        SCCI double alertnessRadius = 0.5;
        SCCI double viewRange = 0.9;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 0.5;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct StraightAStudent
    {
        SCCI double moveSpeed = 0.8;
        SCCI double maxHp = 1.1;
        SCCI double maxAddiction = 1.3;
        SCCI double fixSpeed = 1.1;
        SCCI double treatSpeed = 0.8;
        SCCI double concealment = 0.9;
        SCCI double alertnessRadius = 0.9;
        SCCI double viewRange = 0.9;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 0.83333;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct Robot
    {
        SCCI double moveSpeed = 1.0;
        SCCI double maxHp = 0.4;
        SCCI double maxAddiction = 0.0;
        SCCI double fixSpeed = 1.0;
        SCCI double treatSpeed = 0.8;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 1.0;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 0.0016;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct TechOtaku
    {
        SCCI double moveSpeed = 0.75;
        SCCI double maxHp = 0.9;
        SCCI double maxAddiction = 1.1;
        SCCI double fixSpeed = 1.1;
        SCCI double treatSpeed = 0.9;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 0.9;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 0.75;
        SCCI double speedOfOpenChest = 1.0;
    };

    struct Sunshine
    {
        SCCI double moveSpeed = 1.0;
        SCCI double maxHp = 1.0667;
        SCCI double maxAddiction = 1.1;
        SCCI double fixSpeed = 1.0;
        SCCI double treatSpeed = 2.0;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = 1.0;
        SCCI double viewRange = 1.0;
        SCCI double speedOfOpeningOrLocking = 1.0;
        SCCI double speedOfClimbingThroughWindows = 1.0;
        SCCI double speedOfOpenChest = 1.0;
    };

    // 技能相关
    SCCI int maxNumOfSkill = 3;
    SCCI int commonSkillCD = 30000;    // 普通技能标准冷却时间
    SCCI int commonSkillTime = 10000;  // 普通技能标准持续时间

    SCCI int timeOfTrickerFaintingWhenCharge = 7220;
    SCCI int timeOfStudentFaintingWhenCharge = 2090;

    SCCI int timeOfTrickerFaintingWhenPunish = 3070;

    SCCI int timeOfTrickerSwingingAfterHowl = 3070;
    SCCI int timeOfStudentFaintingWhenHowl = 6110;

    SCCI int timeOfStunnedWhenJumpyDumpty = 3070;

    SCCI double addedTimeOfSpeedWhenInspire = 0.6;
    SCCI int timeOfAddingSpeedWhenInspire = 6000;
    struct CanBeginToCharge
    {
        SCCI double skillCD = 0.8;
        SCCI double durationTime = 0.5;
    };

    struct BecomeInvisible
    {
        SCCI double skillCD = 2.0;
        SCCI double durationTime = 0.6;
    };

    struct Punish
    {
        SCCI double skillCD = 1.0;
        SCCI double durationTime = 0.0;
    };

    struct Rouse
    {
        SCCI double skillCD = 2.0;
        SCCI double durationTime = 0.0;
    };

    struct Encourage
    {
        SCCI double skillCD = 2.0;
        SCCI double durationTime = 0.0;
    };

    struct Inspire
    {
        SCCI double skillCD = 2.0;
        SCCI double durationTime = 0.0;
    };

    struct Howl
    {
        SCCI double skillCD = 0.8333;
        SCCI double durationTime = 0.0;
    };

    struct ShowTime
    {
        SCCI double skillCD = 3.0;
        SCCI double durationTime = 1.0;
    };

    struct JumpyBomb
    {
        SCCI double skillCD = 0.5;
        SCCI double durationTime = 0.3;
    };

    struct UseKnife
    {
        SCCI double skillCD = 1.0;
        SCCI double durationTime = 0.1;
    };

    struct UseRobot
    {
        SCCI double skillCD = 0.0017;
        SCCI double durationTime = 0.0;
    };

    struct WriteAnswers
    {
        SCCI double skillCD = 1.0;
        SCCI double durationTime = 0.0;
    };

    struct SummonGolem
    {
        SCCI double skillCD = 1.0;
        SCCI double durationTime = 0.0;
    };
}  // namespace Constants
#endif
