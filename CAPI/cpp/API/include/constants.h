#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef SCCI
#define SCCI static const constexpr inline
#endif

#undef GetMessage
#undef SendMessage
#undef PeekMessage

namespace Constants
{
    SCCI int32_t frameDuration = 50;  // 每帧毫秒数
    // 地图相关
    SCCI int32_t numOfGridPerCell = 1000;  // 单位坐标数
    SCCI int32_t rows = 50;                // 地图行数
    SCCI int32_t cols = 50;                // 地图列数

    SCCI int32_t numOfClassroom = 10;  // 教室数量
    SCCI int32_t numOfChest = 8;       // 宝箱数量

    SCCI int32_t maxClassroomProgress = 10000000;  // 教室最大进度
    SCCI int32_t maxDoorProgress = 10000000;       // 开关门最大进度
    SCCI int32_t maxChestProgress = 10000000;      // 宝箱最大进度
    SCCI int32_t maxGateProgress = 18000;          // 大门最大进度

    SCCI int32_t numOfRequiredClassroomForGate = 7;        // 打开大门需要完成的教室数量
    SCCI int32_t numOfRequiredClassroomForHiddenGate = 3;  // 打开隐藏门需要完成的教室数量

    // 人物属性相关
    SCCI int32_t basicEncourageSpeed = 100;
    SCCI int32_t basicFixSpeed = 123;
    SCCI int32_t basicSpeedOfOpeningOrLocking = 5000;
    SCCI int32_t basicStudentSpeedOfClimbingThroughWindows = 1222;
    SCCI int32_t basicTrickerSpeedOfClimbingThroughWindows = 2540;
    SCCI int32_t basicSpeedOfOpenChest = 1250;

    SCCI int32_t basicHp = 3000000;
    SCCI int32_t basicMaxGamingAddiction = 60000;
    SCCI int32_t basicEncouragementDegree = 1500000;
    SCCI int32_t basicTimeOfRouse = 1000;

    SCCI int32_t basicStudentSpeed = 3000;
    SCCI int32_t basicTrickerSpeed = 3600;

    SCCI double basicConcealment = 1;
    SCCI int32_t basicStudentAlertnessRadius = 15 * numOfGridPerCell;
    SCCI int32_t basicTrickerAlertnessRadius = 17 * numOfGridPerCell;
    SCCI int32_t basicStudentViewRange = 10 * numOfGridPerCell;
    SCCI int32_t basicTrickerViewRange = 13 * numOfGridPerCell;
    SCCI int32_t PinningDownRange = 5 * numOfGridPerCell;

    SCCI int32_t maxNumOfProp = 3;  // 人物道具栏容量

    // 攻击相关

    SCCI int32_t basicApOfTricker = 1500000;
    SCCI int32_t basicCD = 3000;               // 初始子弹冷却
    SCCI int32_t basicCastTime = 500;          // 基本前摇时间
    SCCI int32_t basicBackswing = 800;         // 基本后摇时间
    SCCI int32_t basicRecoveryFromHit = 3700;  // 基本命中攻击恢复时长
    SCCI int32_t basicStunnedTimeOfStudent = 4300;

    SCCI int32_t basicBulletMoveSpeed = 7400;   // 基本子弹移动速度
    SCCI double basicRemoteAttackRange = 6000;  // 基本远程攻击范围
    SCCI double basicAttackShortRange = 2200;   // 基本近程攻击范围
    SCCI double basicBulletBombRange = 2000;    // 基本子弹爆炸范围

    // 道具相关

    SCCI int32_t apPropAdd = basicApOfTricker * 12 / 10;
    SCCI int32_t apSpearAdd = basicApOfTricker * 6 / 10;

    // 职业相关
    struct Assassin
    {
        SCCI int32_t moveSpeed = basicTrickerSpeed * 11 / 10;
        SCCI double concealment = 1.5;
        SCCI int32_t alertnessRadius = basicTrickerAlertnessRadius * 13 / 10;
        SCCI int32_t viewRange = basicTrickerViewRange * 12 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Klee
    {
        SCCI int32_t moveSpeed = basicTrickerSpeed;
        SCCI double concealment = 1;
        SCCI int32_t alertnessRadius = basicTrickerAlertnessRadius;
        SCCI int32_t viewRange = basicTrickerViewRange;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest * 11 / 10;
    };

    struct ANoisyPerson
    {
        SCCI int32_t moveSpeed = (int32_t)(basicTrickerSpeed * 1.07);
        SCCI double concealment = 0.8;
        SCCI int32_t alertnessRadius = basicTrickerAlertnessRadius * 9 / 10;
        SCCI int32_t viewRange = basicTrickerViewRange;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows * 11 / 10;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest * 11 / 10;
    };

    struct Idol
    {
        SCCI int32_t moveSpeed = basicTrickerSpeed;
        SCCI double concealment = 0.75;
        SCCI int32_t alertnessRadius = basicTrickerAlertnessRadius;
        SCCI int32_t viewRange = basicTrickerViewRange * 11 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicTrickerSpeedOfClimbingThroughWindows;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Athlete
    {
        SCCI int32_t moveSpeed = basicStudentSpeed * 105 / 100;
        SCCI int32_t maxHp = basicHp;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction * 9 / 10;
        SCCI int32_t fixSpeed = basicFixSpeed * 6 / 10;
        SCCI int32_t encourageSpeed = basicEncourageSpeed * 9 / 10;
        SCCI double concealment = 0.9;
        SCCI int32_t alertnessRadius = basicStudentAlertnessRadius;
        SCCI int32_t viewRange = basicStudentViewRange * 11 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Teacher
    {
        SCCI int32_t moveSpeed = basicStudentSpeed * 9 / 10;
        SCCI int32_t maxHp = basicHp * 10;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction * 10;
        SCCI int32_t fixSpeed = (int32_t)(basicFixSpeed * 50 / 123);
        SCCI int32_t encourageSpeed = basicEncourageSpeed * 8 / 10;
        SCCI double concealment = 0.5;
        SCCI int32_t alertnessRadius = basicStudentAlertnessRadius * 2 / 3;
        SCCI int32_t viewRange = basicStudentViewRange * 8 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = (int32_t)(basicStudentSpeedOfClimbingThroughWindows * 1000 / 1222);
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct StraightAStudent
    {
        SCCI int32_t moveSpeed = basicStudentSpeed * 24 / 25;
        SCCI int32_t maxHp = basicHp * 11 / 10;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction * 13 / 10;
        SCCI int32_t fixSpeed = basicFixSpeed * 11 / 10;
        SCCI int32_t encourageSpeed = basicEncourageSpeed;
        SCCI double concealment = 0.9;
        SCCI int32_t alertnessRadius = basicStudentAlertnessRadius * 9 / 10;
        SCCI int32_t viewRange = basicStudentViewRange * 9 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows * 10 / 12;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest;
    };

    struct Robot
    {
        SCCI int32_t moveSpeed = basicStudentSpeed * 9 / 10;
        SCCI int32_t maxHp = basicHp * 3 / 10;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction * 0;
        SCCI int32_t fixSpeed = basicFixSpeed * 7 / 10;
        SCCI int32_t encourageSpeed = 0;
        SCCI double concealment = 0.8;
        SCCI int32_t alertnessRadius = 0;
        SCCI int32_t viewRange = 0;
        SCCI int32_t speedOfOpeningOrLocking = 0;
        SCCI int32_t speedOfClimbingThroughWindows = 1;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest * 4 / 5;
    };

    struct TechOtaku
    {
        SCCI int32_t moveSpeed = (int32_t)(basicStudentSpeed * 0.96);
        SCCI int32_t maxHp = basicHp * 9 / 10;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction;
        SCCI int32_t fixSpeed = (int32_t)(basicFixSpeed * 0.9);
        SCCI int32_t encourageSpeed = basicEncourageSpeed;
        SCCI double concealment = 1.1;
        SCCI int32_t alertnessRadius = basicStudentAlertnessRadius;
        SCCI int32_t viewRange = basicStudentViewRange * 9 / 10;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking;
        SCCI int32_t speedOfClimbingThroughWindows = (int32_t)(basicStudentSpeedOfClimbingThroughWindows * 0.9);
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest * 22 / 25;
    };

    struct Sunshine
    {
        SCCI int32_t moveSpeed = basicStudentSpeed;
        SCCI int32_t maxHp = basicHp * 16 / 15;
        SCCI int32_t maxAddiction = basicMaxGamingAddiction * 11 / 10;
        SCCI int32_t fixSpeed = basicFixSpeed;
        SCCI int32_t encourageSpeed = basicEncourageSpeed * 6 / 5;
        SCCI double concealment = 1;
        SCCI int32_t alertnessRadius = basicStudentAlertnessRadius;
        SCCI int32_t viewRange = basicStudentViewRange;
        SCCI int32_t speedOfOpeningOrLocking = basicSpeedOfOpeningOrLocking * 7 / 10;
        SCCI int32_t speedOfClimbingThroughWindows = basicStudentSpeedOfClimbingThroughWindows;
        SCCI int32_t speedOfOpenChest = basicSpeedOfOpenChest * 9 / 10;
    };

    // 技能相关
    SCCI int32_t maxNumOfSkill = 3;
    SCCI int32_t commonSkillCD = 30000;    // 普通技能标准冷却时间
    SCCI int32_t commonSkillTime = 10000;  // 普通技能标准持续时间

    SCCI int32_t timeOfTrickerStunnedWhenCharge = 7220;
    SCCI int32_t timeOfStudentStunnedWhenCharge = 2090;

    SCCI int32_t timeOfTrickerStunnedWhenPunish = 3070;

    SCCI int32_t timeOfTrickerSwingingAfterHowl = 800;
    SCCI int32_t timeOfStudentStunnedWhenHowl = 5500;

    SCCI int32_t timeOfStunnedWhenJumpyDumpty = 3070;

    SCCI double addedTimeOfSpeedWhenInspire = 1.6;
    SCCI int32_t timeOfAddingSpeedWhenInspire = 6000;

    SCCI int32_t addHpWhenEncourage = basicHp / 4;

    SCCI int32_t checkIntervalWhenShowTime = 200;
    SCCI int32_t addAddictionPer100msWhenShowTime = 300;
    struct CanBeginToCharge
    {
        SCCI int32_t skillCD = commonSkillCD * 2;
        SCCI int32_t durationTime = commonSkillTime * 3 / 10;
    };

    struct BecomeInvisible
    {
        SCCI int32_t skillCD = commonSkillCD * 4 / 3;
        SCCI int32_t durationTime = commonSkillTime;
    };

    struct Punish
    {
        SCCI int32_t skillCD = commonSkillCD * 3 / 2;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct SparksNSplash
    {
        SCCI int32_t skillCD = commonSkillCD * 3 / 2;
        SCCI int32_t durationTime = commonSkillTime * 1;
    };

    struct HaveTea
    {
        SCCI int32_t skillCD = commonSkillCD * 3;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct Rouse
    {
        SCCI int32_t skillCD = commonSkillCD * 4;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct Encourage
    {
        SCCI int32_t skillCD = commonSkillCD * 4;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct Inspire
    {
        SCCI int32_t skillCD = commonSkillCD * 4;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct Howl
    {
        SCCI int32_t skillCD = commonSkillCD * 25 / 30;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct ShowTime
    {
        SCCI int32_t skillCD = commonSkillCD * 8 / 3;
        SCCI int32_t durationTime = commonSkillTime * 1;
    };

    struct JumpyBomb
    {
        SCCI int32_t skillCD = commonSkillCD / 2;
        SCCI int32_t durationTime = commonSkillTime * 3 / 10;
    };

    struct UseKnife
    {
        SCCI int32_t skillCD = commonSkillCD * 1;
        SCCI int32_t durationTime = commonSkillTime / 10;
    };

    struct UseRobot
    {
        SCCI int32_t skillCD = commonSkillCD / 15;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct WriteAnswers
    {
        SCCI int32_t skillCD = commonSkillCD * 1;
        SCCI int32_t durationTime = commonSkillTime * 0;
    };

    struct SummonGolem
    {
        SCCI int32_t skillCD = commonSkillCD * 4 / 3;
        SCCI int32_t durationTime = commonSkillTime * 6;
    };

    struct CommonAttackOfTricker
    {
        SCCI double BulletBombRange = 0;
        SCCI double BulletAttackRange = basicAttackShortRange;
        SCCI int32_t ap = basicApOfTricker;
        SCCI int32_t Speed = basicBulletMoveSpeed;
        SCCI bool IsRemoteAttack = false;

        SCCI int32_t CastTime = (int32_t)BulletAttackRange * 1000 / Speed;
        SCCI int32_t Backswing = basicBackswing;
        SCCI int32_t RecoveryFromHit = basicRecoveryFromHit;
        SCCI int32_t cd = basicBackswing;
        SCCI int32_t maxBulletNum = 1;
    };

    struct FlyingKnife
    {
        SCCI double BulletBombRange = 0;
        SCCI double BulletAttackRange = basicRemoteAttackRange * 13;
        SCCI int32_t ap = basicApOfTricker * 4 / 5;
        SCCI int32_t Speed = basicBulletMoveSpeed * 25 / 10;
        SCCI bool IsRemoteAttack = true;

        SCCI int32_t CastTime = basicCastTime * 6 / 5;
        SCCI int32_t Backswing = 0;
        SCCI int32_t RecoveryFromHit = 0;
        SCCI int32_t cd = basicBackswing * 3 / 4;
        SCCI int32_t maxBulletNum = 1;
    };

    struct BombBomb
    {
        SCCI double BulletBombRange = basicBulletBombRange;
        SCCI double BulletAttackRange = basicAttackShortRange;
        SCCI int32_t ap = basicApOfTricker * 6 / 5;
        SCCI int32_t Speed = basicBulletMoveSpeed * 30 / 37;
        SCCI bool IsRemoteAttack = false;

        SCCI int32_t CastTime = (int32_t)BulletAttackRange * 1000 / Speed;
        SCCI int32_t Backswing = basicBackswing * 3 / 2;
        SCCI int32_t RecoveryFromHit = basicRecoveryFromHit;
        SCCI int32_t cd = basicCD;
        SCCI int32_t maxBulletNum = 1;
    };

    struct JumpyDumpty
    {
        SCCI double BulletBombRange = basicBulletBombRange / 2;
        SCCI double BulletAttackRange = basicAttackShortRange * 16 / 22;
        SCCI int32_t ap = (int32_t)(basicApOfTricker * 0.6);
        SCCI int32_t Speed = basicBulletMoveSpeed * 43 / 37;
        SCCI bool IsRemoteAttack = false;
    };

    struct Strike
    {
        SCCI double BulletBombRange = 0;
        SCCI double BulletAttackRange = basicAttackShortRange;
        SCCI int32_t ap = basicApOfTricker * 16 / 15;
        SCCI int32_t Speed = basicBulletMoveSpeed * 125 / 148;
        SCCI bool IsRemoteAttack = false;

        SCCI int32_t CastTime = basicCastTime * 16 / 25;
        SCCI int32_t Backswing = basicBackswing;
        SCCI int32_t RecoveryFromHit = basicRecoveryFromHit;
        SCCI int32_t cd = basicBackswing;
        SCCI int32_t maxBulletNum = 1;
    };
}  // namespace Constants
#endif
