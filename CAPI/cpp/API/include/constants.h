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
    SCCI int basicTreatmentDegree = 1500000;
    SCCI int basicTimeOfRescue = 1000;

    SCCI int basicStudentSpeed = 1270;
    SCCI int basicTrickerSpeed = 1504;

    SCCI double basicConcealment = 1.0;
    SCCI int basicStudentAlertnessRadius = 15 * numOfGridPerCell;
    SCCI int basicTrickerAlertnessRadius = 17 * numOfGridPerCell;
    SCCI int basicStudentViewRange = 10 * numOfGridPerCell;
    SCCI int basicTrickerViewRange = 15 * numOfGridPerCell;
    SCCI int PinningDownRange = 5 * numOfGridPerCell;

    SCCI int maxNumOfProp = 3;  // 人物道具栏容量

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

    // 职业相关
    struct Assassin
    {
        SCCI double moveSpeed = basicTrickerSpeed * 1.1;
        SCCI double concealment = 1.5;
        SCCI double alertnessRadius = basicTrickerAlertnessRadius * 1.3;
        SCCI double viewRange = basicTrickerViewRange * 1.2;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 1.0;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct Klee
    {
        SCCI double moveSpeed = basicTrickerSpeed * 1.0;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = basicTrickerAlertnessRadius * 1.0;
        SCCI double viewRange = basicTrickerViewRange * 1.0;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 1.0;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.1;
    };

    struct ANoisyPerson
    {
        SCCI double moveSpeed = basicTrickerSpeed * 1.07;
        SCCI double concealment = 0.8;
        SCCI double alertnessRadius = basicTrickerAlertnessRadius * 0.9;
        SCCI double viewRange = basicTrickerViewRange * 1.0;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 1.1;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.1;
    };

    struct Idol
    {
        SCCI double moveSpeed = basicTrickerSpeed * 1.0;
        SCCI double concealment = 0.75;
        SCCI double alertnessRadius = basicTrickerAlertnessRadius * 1.0;
        SCCI double viewRange = basicTrickerViewRange * 1.1;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 1.0;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct Athlete
    {
        SCCI double moveSpeed = basicStudentSpeed * 1.1;
        SCCI double maxHp = basicHp * 1.0;
        SCCI double maxAddiction = basicMaxGamingAddiction * 0.9;
        SCCI double fixSpeed = basicFixSpeed * 0.6;
        SCCI double treatSpeed = basicTreatSpeed * 0.8;
        SCCI double concealment = 0.9;
        SCCI double alertnessRadius = basicStudentAlertnessRadius * 1.0;
        SCCI double viewRange = basicStudentViewRange * 1.1;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 1.2;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct Teacher
    {
        SCCI double moveSpeed = basicStudentSpeed * 0.75;
        SCCI double maxHp = basicHp * 10.0;
        SCCI double maxAddiction = basicMaxGamingAddiction * 10.0;
        SCCI double fixSpeed = basicFixSpeed * 0.0;
        SCCI double treatSpeed = basicTreatSpeed * 0.7;
        SCCI double concealment = 0.5;
        SCCI double alertnessRadius = basicStudentAlertnessRadius * 0.5;
        SCCI double viewRange = basicStudentViewRange * 0.9;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 0.5;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct StraightAStudent
    {
        SCCI double moveSpeed = basicStudentSpeed * 0.8;
        SCCI double maxHp = basicHp * 1.1;
        SCCI double maxAddiction = basicMaxGamingAddiction * 1.3;
        SCCI double fixSpeed = basicFixSpeed * 1.1;
        SCCI double treatSpeed = basicTreatSpeed * 0.8;
        SCCI double concealment = 0.9;
        SCCI double alertnessRadius = basicStudentAlertnessRadius * 0.9;
        SCCI double viewRange = basicStudentViewRange * 0.9;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 0.83333;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct Robot
    {
        SCCI double moveSpeed = basicStudentSpeed * 1.0;
        SCCI double maxHp = basicHp * 0.4;
        SCCI double maxAddiction = basicMaxGamingAddiction * 0.0;
        SCCI double fixSpeed = basicFixSpeed * 1.0;
        SCCI double treatSpeed = basicTreatSpeed * 0.8;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = basicStudentAlertnessRadius * 1;
        SCCI double viewRange = basicStudentViewRange * 1.0;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 0.0016;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct TechOtaku
    {
        SCCI double moveSpeed = basicStudentSpeed * 0.75;
        SCCI double maxHp = basicHp * 0.9;
        SCCI double maxAddiction = basicMaxGamingAddiction * 1.1;
        SCCI double fixSpeed = basicFixSpeed * 1.1;
        SCCI double treatSpeed = basicTreatSpeed * 0.9;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = basicStudentAlertnessRadius;
        SCCI double viewRange = basicStudentViewRange * 0.9;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 0.75;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    struct Sunshine
    {
        SCCI double moveSpeed = basicStudentSpeed * 1.0;
        SCCI double maxHp = basicHp * 1.0667;
        SCCI double maxAddiction = basicMaxGamingAddiction * 1.1;
        SCCI double fixSpeed = basicFixSpeed * 1.0;
        SCCI double treatSpeed = basicTreatSpeed * 2.0;
        SCCI double concealment = 1.0;
        SCCI double alertnessRadius = basicStudentAlertnessRadius * 1.0;
        SCCI double viewRange = basicStudentViewRange * 1.0;
        SCCI double speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 1.0;
        SCCI double speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 1.0;
        SCCI double speedOfOpenChest = basicSpeedOfOpenChest * 1.0;
    };

    // 技能相关
    SCCI int maxNumOfSkill = 3;
    SCCI int commonSkillCD = 30000;    // 普通技能标准冷却时间
    SCCI int commonSkillTime = 10000;  // 普通技能标准持续时间

    SCCI int timeOfTrickerStunnedWhenCharge = 7220;
    SCCI int timeOfStudentStunnedWhenCharge = 2090;

    SCCI int timeOfTrickerStunnedWhenPunish = 3070;

    SCCI int timeOfTrickerSwingingAfterHowl = 800;
    SCCI int timeOfStudentStunnedWhenHowl = 5500;

    SCCI int timeOfStunnedWhenJumpyDumpty = 3070;

    SCCI double addedTimeOfSpeedWhenInspire = 0.6;
    SCCI int timeOfAddingSpeedWhenInspire = 6000;
    struct CanBeginToCharge
    {
        SCCI double skillCD = 4 / 3;
        SCCI double durationTime = 1;
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
