#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <array>
#include <map>
#include <vector>
#include <string>

namespace THUAI6
{

    // 游戏状态
    enum class GameState : unsigned char
    {
        NullGameState = 0,
        GameStart = 1,
        GameRunning = 2,
        GameEnd = 3,
    };
    // 所有NullXXXType均为错误类型，其余为可能出现的正常类型

    // 位置标志
    enum class PlaceType : unsigned char
    {
        NullPlaceType = 0,
        Land = 1,
        Wall = 2,
        Grass = 3,
        ClassRoom = 4,
        Gate = 5,
        HiddenGate = 6,
        Window = 7,
        Door3 = 8,
        Door5 = 9,
        Door6 = 10,
        Chest = 11,
    };

    // 形状标志
    enum class ShapeType : unsigned char
    {
        NullShapeType = 0,
        Circle = 1,
        Square = 2,
    };

    // 道具类型
    enum class PropType : unsigned char
    {
        NullPropType = 0,
        Key3 = 1,
        Key5 = 2,
        Key6 = 3,
        AddSpeed = 4,
        AddLifeOrClairaudience = 5,
        AddHpOrAp = 6,
        ShieldOrSpear = 7,
        RecoveryFromDizziness = 8,
    };

    enum class BulletType : unsigned char
    {
        NullBulletType = 0,
        FlyingKnife = 1,
        CommonAttackOfTricker = 2,
        BombBomb = 3,
        JumpyDumpty = 4,
        AtomBomb = 5,
    };

    // 玩家类型
    enum class PlayerType : unsigned char
    {
        NullPlayerType = 0,
        StudentPlayer = 1,
        TrickerPlayer = 2,
    };

    // 学生类型
    enum class StudentType : unsigned char
    {
        NullStudentType = 0,
        Athlete = 1,
        Teacher = 2,
        StraightAStudent = 3,
        Robot = 4,
        TechOtaku = 5,
        Sunshine = 6,
    };

    // 捣蛋鬼类型
    enum class TrickerType : unsigned char
    {
        NullTrickerType = 0,
        Assassin = 1,
        Klee = 2,
        ANoisyPerson = 3,
        Idol = 4,
    };

    // 学生Buff类型
    enum class StudentBuffType : unsigned char
    {
        NullStudentBuffType = 0,
        AddSpeed = 1,
        AddLife = 2,
        Shield = 3,
        Invisible = 4,
    };

    enum class TrickerBuffType : unsigned char
    {
        NullTrickerBuffType = 0,
        AddSpeed = 1,
        Spear = 2,
        AddAp = 3,
        Clairaudience = 4,
        Invisible = 5,
    };

    // 学生状态枚举
    enum class PlayerState : unsigned char
    {
        NullState = 0,
        Idle = 1,
        Learning = 2,
        Addicted = 3,
        Quit = 4,
        Graduated = 5,
        Encouraged = 6,
        Roused = 7,
        Stunned = 8,
        Encouraging = 9,
        Rousing = 10,
        Swinging = 11,
        Attacking = 12,
        Locking = 13,
        // Rummaging = 14,
        Climbing = 15,
        OpeningAChest = 16,
        UsingSpecialSkill = 17,
        OpeningAGate = 18,
    };

    enum class MessageOfObj : unsigned char
    {
        NullMessageOfObj = 0,
        StudentMessage = 1,
        TrickerMessage = 2,
        PropMessage = 3,
        BulletMessage = 4,
        BombedBulletMessage = 5,
        ClassroomMessage = 6,
        DoorMessage = 7,
        GateMessage = 8,
        ChestMessage = 9,
        MapMessage = 10,
        NewsMessage = 11,
        HiddenGateMessage = 12,
    };

    enum class HiddenGateState : unsigned char
    {
        Null = 0,
        Refreshed = 1,
        Opened = 2,
    };

    // 玩家类
    struct Player
    {
        int32_t x;          // x坐标
        int32_t y;          // y坐标
        int32_t speed;      // 移动速度
        int32_t viewRange;  // 视野范围
        int64_t playerID;   // 玩家ID
        int64_t guid;       // 全局唯一ID
        int32_t radius;     // 圆形物体的半径或正方形物体的内切圆半径
        int32_t score;      // 分数

        double facingDirection;  // 朝向

        std::vector<double> timeUntilSkillAvailable;  // 技能冷却时间

        PlayerType playerType;  // 玩家类型
        std::vector<PropType> props;
        PlaceType place;  // 所处格子的类型
        BulletType bulletType;

        PlayerState playerState;
    };

    struct Student : public Player
    {
        StudentType studentType;
        int32_t determination;  // 剩余毅力
        int32_t addiction;      // 沉迷程度
        int32_t learningSpeed;
        int32_t encourageSpeed;
        int32_t encourageProgress;
        int32_t rouseProgress;
        double dangerAlert;
        std::vector<StudentBuffType> buff;  // buff
    };

    struct Tricker : public Player
    {
        double trickDesire;
        double classVolume;
        TrickerType trickerType;            // 捣蛋鬼类型
        std::vector<TrickerBuffType> buff;  // buff
    };

    struct Bullet
    {
        BulletType bulletType;   // 子弹类型
        int32_t x;               // x坐标
        int32_t y;               // y坐标
        double facingDirection;  // 朝向
        int64_t guid;            // 全局唯一ID
        PlayerType team;         // 子弹所属队伍
        PlaceType place;         // 所处格子的类型
        double bombRange;        // 炸弹爆炸范围
        int32_t speed;           // 子弹速度
    };

    struct BombedBullet
    {
        BulletType bulletType;
        int32_t x;
        int32_t y;
        double facingDirection;
        int64_t mappingID;
        double bombRange;
    };

    struct Prop
    {
        int32_t x;
        int32_t y;
        int64_t guid;
        PropType type;
        PlaceType place;
        double facingDirection;  // 朝向
    };

    struct GameMap
    {
        std::map<std::pair<int32_t, int32_t>, int32_t> classRoomState;

        std::map<std::pair<int32_t, int32_t>, int32_t> gateState;

        std::map<std::pair<int32_t, int32_t>, bool> doorState;

        std::map<std::pair<int32_t, int32_t>, int32_t> doorProgress;

        std::map<std::pair<int32_t, int32_t>, int32_t> chestState;

        std::map<std::pair<int32_t, int32_t>, HiddenGateState> hiddenGateState;
    };

    struct GameInfo
    {
        int32_t gameTime;
        int32_t subjectFinished;
        int32_t studentGraduated;
        int32_t studentQuited;
        int32_t studentScore;
        int32_t trickerScore;
    };

    // 仅供DEBUG使用，名称可改动
    // 还没写完，后面待续

    inline std::map<GameState, std::string> gameStateDict{
        {GameState::NullGameState, "NullGameState"},
        {GameState::GameStart, "GameStart"},
        {GameState::GameRunning, "GameRunning"},
        {GameState::GameEnd, "GameEnd"},
    };

    inline std::map<StudentType, std::string> studentTypeDict{
        {StudentType::NullStudentType, "NullStudentType"},
        {StudentType::Athlete, "Athlete"},
        {StudentType::Teacher, "Teacher"},
        {StudentType::StraightAStudent, "StraightAStudent"},
        {StudentType::Robot, "Robot"},
        {StudentType::TechOtaku, "TechOtaku"},
        {StudentType::Sunshine, "Sunshine"},
    };

    inline std::map<TrickerType, std::string> trickerTypeDict{
        {TrickerType::NullTrickerType, "NullTrickerType"},
        {TrickerType::Assassin, "Assassin"},
        {TrickerType::Klee, "Klee"},
        {TrickerType::ANoisyPerson, "ANoisyPerson"},
        {TrickerType::Idol, "Idol"},
    };

    inline std::map<PlayerState, std::string> playerStateDict{
        {PlayerState::NullState, "NullState"},
        {PlayerState::Idle, "Idle"},
        {PlayerState::Learning, "Learning"},
        {PlayerState::Addicted, "Addicted"},
        {PlayerState::Quit, "Quit"},
        {PlayerState::Graduated, "Graduated"},
        {PlayerState::Encouraged, "Encouraged"},
        {PlayerState::Roused, "Roused"},
        {PlayerState::Stunned, "Stunned"},
        {PlayerState::Encouraging, "Encouraging"},
        {PlayerState::Rousing, "Rousing"},
        {PlayerState::Swinging, "Swinging"},
        {PlayerState::Attacking, "Attacking"},
        {PlayerState::Locking, "Locking"},
        // {PlayerState::Rummaging, "Rummaging"},
        {PlayerState::Climbing, "Climbing"},
        {PlayerState::OpeningAChest, "OpeningAChest"},
        {PlayerState::UsingSpecialSkill, "UsingSpecialSkill"},
        {PlayerState::OpeningAGate, "OpeningAGate"},
    };

    inline std::map<PlayerType, std::string> playerTypeDict{
        {PlayerType::NullPlayerType, "NullPlayerType"},
        {PlayerType::StudentPlayer, "StudentPlayer"},
        {PlayerType::TrickerPlayer, "TrickerPlayer"},
    };

    inline std::map<PlaceType, std::string> placeTypeDict{
        {PlaceType::NullPlaceType, "NullPlaceType"},
        {PlaceType::Land, "Land"},
        {PlaceType::Wall, "Wall"},
        {PlaceType::Grass, "Grass"},
        {PlaceType::ClassRoom, "ClassRoom"},
        {PlaceType::Gate, "Gate"},
        {PlaceType::HiddenGate, "HiddenGate"},
        {PlaceType::Door3, "Door3"},
        {PlaceType::Door5, "Door5"},
        {PlaceType::Door6, "Door6"},
        {PlaceType::Window, "Window"},
        {PlaceType::Chest, "Chest"},
    };

    inline std::map<PropType, std::string> propTypeDict{
        {PropType::NullPropType, "NullPropType"},
        {PropType::Key3, "Key3"},
        {PropType::Key5, "Key5"},
        {PropType::Key6, "Key6"},
        {PropType::AddSpeed, "AddSpeed"},
        {PropType::AddLifeOrClairaudience, "AddLifeOrClairaudience"},
        {PropType::AddHpOrAp, "AddHpOrAp"},
        {PropType::ShieldOrSpear, "ShieldOrSpear"},
        {PropType::RecoveryFromDizziness, "RecoveryFromDizziness"},

    };

    inline std::map<BulletType, std::string> bulletTypeDict{
        {BulletType::NullBulletType, "NullBulletType"},
        {BulletType::FlyingKnife, "FlyingKnife"},
        {BulletType::CommonAttackOfTricker, "CommonAttackOfTricker"},
        {BulletType::BombBomb, "BombBomb"},
        {BulletType::JumpyDumpty, "JumpyDumpty"},
        {BulletType::AtomBomb, "AtomBomb"},
    };

    inline std::map<StudentBuffType, std::string> studentBuffDict{
        {StudentBuffType::NullStudentBuffType, "NullStudentBuffType"},
        {StudentBuffType::AddSpeed, "AddSpeed"},
        {StudentBuffType::AddLife, "AddLife"},
        {StudentBuffType::Shield, "Shield"},
        {StudentBuffType::Invisible, "Invisible"},

    };

    inline std::map<TrickerBuffType, std::string> trickerBuffDict{
        {TrickerBuffType::NullTrickerBuffType, "NullTrickerBuffType"},
        {TrickerBuffType::AddSpeed, "AddSpeed"},
        {TrickerBuffType::Spear, "Spear"},
        {TrickerBuffType::Clairaudience, "Clairaudience"},
        {TrickerBuffType::AddAp, "AddAp"},
        {TrickerBuffType::Invisible, "Invisible"},

    };

    inline std::map<MessageOfObj, std::string> messageOfObjDict{
        {MessageOfObj::NullMessageOfObj, "NullMessageOfObj"},
        {MessageOfObj::StudentMessage, "StudentMessage"},
        {MessageOfObj::TrickerMessage, "TrickerMessage"},
        {MessageOfObj::PropMessage, "PropMessage"},
        {MessageOfObj::BulletMessage, "BulletMessage"},
        {MessageOfObj::BombedBulletMessage, "BombedBulletMessage"},
        {MessageOfObj::NullMessageOfObj, "NullMessageOfObj"},
        {MessageOfObj::ClassroomMessage, "ClassroomMessage"},
        {MessageOfObj::DoorMessage, "DoorMessage"},
        {MessageOfObj::GateMessage, "GateMessage"},
        {MessageOfObj::ChestMessage, "ChestMessage"},
        {MessageOfObj::MapMessage, "MapMessage"},
        {MessageOfObj::NewsMessage, "NewsMessage"},
        {MessageOfObj::HiddenGateMessage, "HiddenGateMessage"},
    };

}  // namespace THUAI6

#endif
