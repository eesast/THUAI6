#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cstdint>
#include <array>
#include <map>

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
        BlackRoom = 5,
        Gate = 6,
        HiddenGate = 7,
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
        PropType1 = 1,
        PropType2 = 2,
        PropType3 = 3,
        PropType4 = 4,
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
        StudentType1 = 1,
        StudentType2 = 2,
        StudentType3 = 3,
        StudentType4 = 4,
    };

    // 捣蛋鬼类型
    enum class TrickerType : unsigned char
    {
        NullTrickerType = 0,
        TrickerType1 = 1,
        TrickerType2 = 2,
        TrickerType3 = 3,
        TrickerType4 = 4,
    };

    // 学生Buff类型
    enum class StudentBuffType : unsigned char
    {
        NullStudentBuffType = 0,
        StudentBuffType1 = 1,
        StudentBuffType2 = 2,
        StudentBuffType3 = 3,
        StudentBuffType4 = 4,
    };

    enum class TrickerBuffType : unsigned char
    {
        NullTrickerBuffType = 0,
        TrickerBuffType1 = 1,
        TrickerBuffType2 = 2,
        TrickerBuffType3 = 3,
        TrickerBuffType4 = 4,
    };

    // 学生状态枚举
    enum class StudentState : unsigned char
    {
        NullStudentState = 0,
        Idle = 1,
        Learning = 2,
        Addicted = 3,
        Quit = 4,
        Graduated = 5,
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
        int16_t radius;     // 圆形物体的半径或正方形物体的内切圆半径

        int32_t damage;                  // 攻击伤害
        double timeUntilSkillAvailable;  // 技能冷却时间

        PlayerType playerType;  // 玩家类型
        PropType prop;          // 手上的道具类型
        PlaceType place;        // 所处格子的类型
    };

    struct Student : public Player
    {
        StudentState state;     // 学生状态
        int32_t determination;  // 剩余毅力（本次Emo之前还能承受的伤害）
        int32_t failNum;        // 挂科数量
        double failTime;        // 挂科时间
        double emoTime;         // EMO时间

        StudentType studentType;            // 学生类型
        std::vector<StudentBuffType> buff;  // buff
    };

    struct Tricker : public Player
    {
        bool movable;  // 是否处在攻击后摇中

        TrickerType trickerType;            // 捣蛋鬼类型
        std::vector<TrickerBuffType> buff;  // buff
    };

    struct Prop
    {
        int32_t x;
        int32_t y;
        int32_t size;
        int64_t guid;
        PropType type;
        PlaceType place;
        double facingDirection;  // 朝向
        bool isMoving;
    };

    // 仅供DEBUG使用，名称可改动
    // 还没写完，后面待续

    inline std::map<GameState, std::string> gameStateDict{
        {GameState::NullGameState, "NullGameState"},
        {GameState::GameStart, "GameStart"},
        {GameState::GameRunning, "GameRunning"},
        {GameState::GameEnd, "GameEnd"},
    };

    inline std::map<StudentState, std::string> studentStateDict{
        {StudentState::NullStudentState, "NullStudentState"},
        {StudentState::Idle, "Idle"},
        {StudentState::Learning, "Learning"},
        {StudentState::Addicted, "Addicted"},
        {StudentState::Quit, "Quit"},
        {StudentState::Graduated, "Graduated"},
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
    };

    inline std::map<PropType, std::string> propTypeDict{
        {PropType::NullPropType, "NullPropType"},

    };

    inline std::map<StudentBuffType, std::string> studentBuffDict{
        {StudentBuffType::NullStudentBuffType, "NullStudentBuffType"},

    };

    inline std::map<TrickerBuffType, std::string> trickerBuffDict{
        {TrickerBuffType::NullTrickerBuffType, "NullTrickerBuffType"},

    };

}  // namespace THUAI6

#endif
