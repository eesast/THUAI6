#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef SCCI
#define SCCI static const constexpr inline
#endif

namespace Constants
{
    SCCI int frameDuration = 50;  // 每帧毫秒数
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
    SCCI int basicEncourageSpeed = 100;
    SCCI int basicFixSpeed = 123;
    SCCI int basicSpeedOfOpeningOrLocking = 4000;
    SCCI int basicStudentSpeedOfClimbingThroughWindows = 1222;
    SCCI int basicTrickerSpeedOfClimbingThroughWindows = 2540;
    SCCI int basicSpeedOfOpenChest = 1000;

    SCCI int basicHp = 3000000;
    SCCI int basicMaxGamingAddiction = 60000;
    SCCI int basicEncouragementDegree = 1500000;
    SCCI int basicTimeOfRouse = 1000;

    SCCI int basicStudentSpeed = 3000;
    SCCI int basicTrickerSpeed = 3600;

    SCCI double basicConcealment = 1;
    SCCI int basicStudentAlertnessRadius = 15 * numOfGridPerCell;
    SCCI int basicTrickerAlertnessRadius = 17 * numOfGridPerCell;
    SCCI int basicStudentViewRange = 10 * numOfGridPerCell;
    SCCI int basicTrickerViewRange = 13 * numOfGridPerCell;
    SCCI int PinningDownRange = 5 * numOfGridPerCell;

    SCCI int maxNumOfProp = 3;  // 人物道具栏容量

    // 攻击相关

    SCCI int basicApOfTricker = 1500000;
    SCCI int basicCD = 3000;               // 初始子弹冷却
    SCCI int basicCastTime = 500;          // 基本前摇时间
    SCCI int basicBackswing = 800;         // 基本后摇时间
    SCCI int basicRecoveryFromHit = 3700;  // 基本命中攻击恢复时长
    SCCI int basicStunnedTimeOfStudent = 4300;

    SCCI int basicBulletMoveSpeed = 7400;       // 基本子弹移动速度
    SCCI double basicRemoteAttackRange = 6000;  // 基本远程攻击范围
    SCCI double basicAttackShortRange = 2200;   // 基本近程攻击范围
    SCCI double basicBulletBombRange = 2000;    // 基本子弹爆炸范围

    // 道具相关

    SCCI int apPropAdd = basicApOfTricker * 12 / 10;
    SCCI int apSpearAdd = basicApOfTricker * 6 / 10;

    // 职业相关
    struct Assassin
    {
        SCCI int moveSpeed = basicTrickerSpeed * 11 / 10;
        SCCI double concealment = 1.5;
        SCCI int alertnessRadius = basicTrickerAlertnessRadius * 13 / 10;
        SCCI int viewRange = basicTrickerViewRange * 12 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Klee
    {
        SCCI int moveSpeed = basicTrickerSpeed;
        SCCI double concealment = 1;
        SCCI int alertnessRadius = basicTrickerAlertnessRadius;
        SCCI int viewRange = basicTrickerViewRange;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest * 11 / 10;
    };

    struct ANoisyPerson
    {
        SCCI int moveSpeed = (int)(basicTrickerSpeed * 1.07);
        SCCI double concealment = 0.8;
        SCCI int alertnessRadius = basicTrickerAlertnessRadius * 9 / 10;
        SCCI int viewRange = basicTrickerViewRange;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 11 / 10;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest * 11 / 10;
    };

    struct Idol
    {
        SCCI int moveSpeed = basicTrickerSpeed;
        SCCI double concealment = 0.75;
        SCCI int alertnessRadius = basicTrickerAlertnessRadius;
        SCCI int viewRange = basicTrickerViewRange * 11 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Athlete
    {
        SCCI int moveSpeed = basicStudentSpeed * 105 / 100;
        SCCI int maxHp = basicHp;
        SCCI int maxAddiction = basicMaxGamingAddiction * 9 / 10;
        SCCI int fixSpeed = basicFixSpeed * 6 / 10;
        SCCI int encourageSpeed = basicEncourageSpeed * 9 / 10;
        SCCI double concealment = 0.9;
        SCCI int alertnessRadius = basicStudentAlertnessRadius;
        SCCI int viewRange = basicStudentViewRange * 11 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Teacher
    {
        SCCI int moveSpeed = basicStudentSpeed * 9 / 10;
        SCCI int maxHp = basicHp * 10;
        SCCI int maxAddiction = basicMaxGamingAddiction * 10;
        SCCI int fixSpeed = basicFixSpeed * 0;
        SCCI int encourageSpeed = basicEncourageSpeed * 8 / 10;
        SCCI double concealment = 0.5;
        SCCI int alertnessRadius = basicStudentAlertnessRadius / 2;
        SCCI int viewRange = basicStudentViewRange * 9 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows / 2;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct StraightAStudent
    {
        SCCI int moveSpeed = basicStudentSpeed * 96 / 100;
        SCCI int maxHp = basicHp * 11 / 10;
        SCCI int maxAddiction = basicMaxGamingAddiction * 13 / 10;
        SCCI int fixSpeed = basicFixSpeed * 11 / 10;
        SCCI int encourageSpeed = basicEncourageSpeed;
        SCCI double concealment = 0.9;
        SCCI int alertnessRadius = basicStudentAlertnessRadius * 9 / 10;
        SCCI int viewRange = basicStudentViewRange * 9 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 10 / 12;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Robot
    {
        SCCI int moveSpeed = basicStudentSpeed;
        SCCI int maxHp = basicHp * 0.4;
        SCCI int maxAddiction = basicMaxGamingAddiction * 0;
        SCCI int fixSpeed = basicFixSpeed;
        SCCI int encourageSpeed = 0;
        SCCI double concealment = 1;
        SCCI int alertnessRadius = basicStudentAlertnessRadius * 1;
        SCCI int viewRange = basicStudentViewRange;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = 1;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct TechOtaku
    {
        SCCI int moveSpeed = basicStudentSpeed * 3 / 4;
        SCCI int maxHp = basicHp * 9 / 10;
        SCCI int maxAddiction = basicMaxGamingAddiction * 11 / 10;
        SCCI int fixSpeed = basicFixSpeed * 11 / 10;
        SCCI int encourageSpeed = basicEncourageSpeed * 9 / 10;
        SCCI double concealment = 1;
        SCCI int alertnessRadius = basicStudentAlertnessRadius;
        SCCI int viewRange = basicStudentViewRange * 9 / 10;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 3 / 4;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Sunshine
    {
        SCCI int moveSpeed = basicStudentSpeed;
        SCCI int maxHp = basicHp * 32 / 30;
        SCCI int maxAddiction = basicMaxGamingAddiction * 11 / 10;
        SCCI int fixSpeed = basicFixSpeed;
        SCCI int encourageSpeed = basicEncourageSpeed * 12 / 10;
        SCCI double concealment = 1;
        SCCI int alertnessRadius = basicStudentAlertnessRadius;
        SCCI int viewRange = basicStudentViewRange;
        SCCI int speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 7 / 10;
        SCCI int speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows;
        SCCI int speedOfOpenChest = basicSpeedOfOpenChest * 9 / 10;
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

    SCCI double addedTimeOfSpeedWhenInspire = 1.6;
    SCCI int timeOfAddingSpeedWhenInspire = 6000;

    SCCI int addHpWhenEncourage = basicHp / 4;

    SCCI int checkIntervalWhenShowTime = 200;
    SCCI int addAddictionPer100msWhenShowTime = 300;
    struct CanBeginToCharge
    {
        SCCI int skillCD = commonSkillCD * 2;
        SCCI int durationTime = commonSkillTime * 3 / 10;
    };

    struct BecomeInvisible
    {
        SCCI int skillCD = commonSkillCD * 4 / 3;
        SCCI int durationTime = commonSkillTime;
    };

    struct Punish
    {
        SCCI int skillCD = commonSkillCD * 1;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct Rouse
    {
        SCCI int skillCD = commonSkillCD * 4;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct Encourage
    {
        SCCI int skillCD = commonSkillCD * 4;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct Inspire
    {
        SCCI int skillCD = commonSkillCD * 4;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct Howl
    {
        SCCI int skillCD = commonSkillCD * 25 / 30;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct ShowTime
    {
        SCCI int skillCD = commonSkillCD * 8 / 3;
        SCCI int durationTime = commonSkillTime * 1;
    };

    struct JumpyBomb
    {
        SCCI int skillCD = commonSkillCD / 2;
        SCCI int durationTime = commonSkillTime * 3 / 10;
    };

    struct UseKnife
    {
        SCCI int skillCD = commonSkillCD * 1;
        SCCI int durationTime = commonSkillTime / 10;
    };

    struct UseRobot
    {
        SCCI int skillCD = commonSkillCD / 300;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct WriteAnswers
    {
        SCCI int skillCD = commonSkillCD * 1;
        SCCI int durationTime = commonSkillTime * 0;
    };

    struct SummonGolem
    {
        SCCI int skillCD = commonSkillCD * 1;
        SCCI int durationTime = commonSkillTime * 0;
    };
}  // namespace Constants
#endif
