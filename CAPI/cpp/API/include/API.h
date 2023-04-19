#pragma once
#ifndef API_H
#define API_H

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "Message2Server.pb.h"
#include "Message2Clients.pb.h"
#include "MessageType.pb.h"
#include "Services.grpc.pb.h"
#include "Services.pb.h"
#include <future>
#include <iostream>
#include <vector>
#include <optional>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "structures.h"

const constexpr int numOfGridPerCell = 1000;

class IAI;

class ILogic
{
    // API中依赖Logic的部分

public:
    // 获取服务器发来的消息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Student> StudentGetSelfInfo() const = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Tricker> TrickerGetSelfInfo() const = 0;

    [[nodiscard]] virtual std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const = 0;
    [[nodiscard]] virtual THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetChestProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetGateProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual bool IsDoorOpen(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const = 0;

    // 供IAPI使用的操作相关的部分
    virtual bool Move(int64_t time, double angle) = 0;
    virtual bool PickProp(THUAI6::PropType prop) = 0;
    virtual bool UseProp(THUAI6::PropType prop) = 0;
    virtual bool ThrowProp(THUAI6::PropType prop) = 0;
    virtual bool UseSkill(int32_t skillID) = 0;
    virtual bool SendMessage(int64_t toID, std::string message) = 0;
    virtual bool HaveMessage() = 0;
    virtual std::pair<int64_t, std::string> GetMessage() = 0;

    virtual bool WaitThread() = 0;

    virtual int GetCounter() const = 0;

    // IStudentAPI使用的部分
    virtual bool Graduate() = 0;

    virtual bool StartLearning() = 0;
    virtual bool StartEncourageMate(int64_t mateID) = 0;
    virtual bool StartRouseMate(int64_t mateID) = 0;

    virtual bool OpenDoor() = 0;
    virtual bool CloseDoor() = 0;
    virtual bool SkipWindow() = 0;
    virtual bool StartOpenGate() = 0;
    virtual bool StartOpenChest() = 0;
    virtual bool EndAllAction() = 0;

    // ITrickerAPI使用的部分
    virtual bool Attack(double angle) = 0;

    virtual std::vector<int64_t> GetPlayerGUIDs() const = 0;
};

class IAPI
{
public:
    // 选手可执行的操作，应当保证所有函数的返回值都应当为std::future，例如下面的移动函数：
    // 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴
    virtual std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) = 0;

    // 向特定方向移动
    virtual std::future<bool> MoveRight(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveUp(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveLeft(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveDown(int64_t timeInMilliseconds) = 0;

    // 捡道具、使用技能
    virtual std::future<bool> PickProp(THUAI6::PropType prop) = 0;
    virtual std::future<bool> UseProp(THUAI6::PropType prop) = 0;
    virtual std::future<bool> ThrowProp(THUAI6::PropType prop) = 0;
    virtual std::future<bool> UseSkill(int32_t skillID) = 0;
    virtual std::future<bool> Attack(double angleInRadian) = 0;

    virtual std::future<bool> OpenDoor() = 0;
    virtual std::future<bool> CloseDoor() = 0;
    virtual std::future<bool> SkipWindow() = 0;
    virtual std::future<bool> StartOpenGate() = 0;
    virtual std::future<bool> StartOpenChest() = 0;
    virtual std::future<bool> EndAllAction() = 0;

    // 发送信息、接受信息，注意收消息时无消息则返回nullopt
    virtual std::future<bool> SendMessage(int64_t, std::string) = 0;
    [[nodiscard]] virtual bool HaveMessage() = 0;
    [[nodiscard]] virtual std::pair<int64_t, std::string> GetMessage() = 0;

    // 等待下一帧
    virtual bool Wait() = 0;

    // 获取视野内可见的学生/捣蛋鬼的信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const = 0;

    // 获取视野内可见的道具信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;

    // 获取视野内可见的子弹信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const = 0;

    [[nodiscard]] virtual std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const = 0;
    [[nodiscard]] virtual THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual bool IsDoorOpen(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetChestProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetGateProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const = 0;

    // 获取所有玩家的GUID
    [[nodiscard]] virtual std::vector<int64_t> GetPlayerGUIDs() const = 0;

    // 获取游戏目前所进行的帧数
    [[nodiscard]] virtual int GetFrameCount() const = 0;

    /*****选手可能用的辅助函数*****/

    // 获取指定格子中心的坐标
    [[nodiscard]] static inline int CellToGrid(int cell) noexcept
    {
        return cell * numOfGridPerCell + numOfGridPerCell / 2;
    }

    // 获取指定坐标点所位于的格子的 X 序号
    [[nodiscard]] static inline int GridToCell(int grid) noexcept
    {
        return grid / numOfGridPerCell;
    }

    // 用于DEBUG的输出函数，选手仅在开启Debug模式的情况下可以使用

    virtual void Print(std::string str) const = 0;
    virtual void PrintStudent() const = 0;
    virtual void PrintTricker() const = 0;
    virtual void PrintProp() const = 0;
    virtual void PrintSelfInfo() const = 0;
};

class IStudentAPI : public IAPI
{
public:
    /*****学生阵营的特定函数*****/

    virtual std::future<bool> StartLearning() = 0;
    virtual std::future<bool> StartEncourageMate(int64_t mateID) = 0;
    virtual std::future<bool> StartRouseMate(int64_t mateID) = 0;
    virtual std::future<bool> Graduate() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Student> GetSelfInfo() const = 0;
};

class ITrickerAPI : public IAPI
{
public:
    /*****捣蛋鬼阵营的特定函数*****/

    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const = 0;
};

class IGameTimer
{
public:
    virtual ~IGameTimer() = default;
    virtual void StartTimer() = 0;
    virtual void EndTimer() = 0;
    virtual void Play(IAI& ai) = 0;
};

class StudentAPI : public IStudentAPI, public IGameTimer
{
public:
    StudentAPI(ILogic& logic) :
        logic(logic)
    {
    }
    void StartTimer() override
    {
    }
    void EndTimer() override
    {
    }
    void Play(IAI& ai) override;

    [[nodiscard]] int GetFrameCount() const override;

    std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) override;

    std::future<bool> MoveRight(int64_t timeInMilliseconds) override;
    std::future<bool> MoveUp(int64_t timeInMilliseconds) override;
    std::future<bool> MoveLeft(int64_t timeInMilliseconds) override;
    std::future<bool> MoveDown(int64_t timeInMilliseconds) override;

    std::future<bool> PickProp(THUAI6::PropType prop) override;
    std::future<bool> UseProp(THUAI6::PropType prop) override;
    std::future<bool> ThrowProp(THUAI6::PropType prop) override;
    std::future<bool> UseSkill(int32_t skillID) override;

    std::future<bool> Attack(double angleInRadian) override;

    std::future<bool> OpenDoor() override;
    std::future<bool> CloseDoor() override;
    std::future<bool> SkipWindow() override;
    std::future<bool> StartOpenGate() override;
    std::future<bool> StartOpenChest() override;
    std::future<bool> EndAllAction() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] bool HaveMessage() override;
    [[nodiscard]] std::pair<int64_t, std::string> GetMessage() override;

    bool Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] bool IsDoorOpen(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetChestProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetGateProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const override;

    [[nodiscard]] std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> StartLearning() override;
    std::future<bool> StartEncourageMate(int64_t mateID) override;
    std::future<bool> StartRouseMate(int64_t mateID) override;
    std::future<bool> Graduate() override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Student> GetSelfInfo() const override;

    void Print(std::string str) const override
    {
    }
    void PrintStudent() const override
    {
    }
    void PrintTricker() const override
    {
    }
    void PrintProp() const override
    {
    }
    void PrintSelfInfo() const override
    {
    }

private:
    ILogic& logic;
};

class TrickerAPI : public ITrickerAPI, public IGameTimer
{
public:
    TrickerAPI(ILogic& logic) :
        logic(logic)
    {
    }
    void StartTimer() override
    {
    }
    void EndTimer() override
    {
    }
    void Play(IAI& ai) override;

    [[nodiscard]] int GetFrameCount() const override;

    std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) override;
    std::future<bool> MoveRight(int64_t timeInMilliseconds) override;
    std::future<bool> MoveUp(int64_t timeInMilliseconds) override;
    std::future<bool> MoveLeft(int64_t timeInMilliseconds) override;
    std::future<bool> MoveDown(int64_t timeInMilliseconds) override;

    std::future<bool> PickProp(THUAI6::PropType prop) override;
    std::future<bool> UseProp(THUAI6::PropType prop) override;
    std::future<bool> ThrowProp(THUAI6::PropType prop) override;
    std::future<bool> UseSkill(int32_t skillID) override;

    std::future<bool> OpenDoor() override;
    std::future<bool> CloseDoor() override;
    std::future<bool> SkipWindow() override;
    std::future<bool> StartOpenGate() override;
    std::future<bool> StartOpenChest() override;
    std::future<bool> EndAllAction() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] bool HaveMessage() override;
    [[nodiscard]] std::pair<int64_t, std::string> GetMessage() override;

    bool Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] bool IsDoorOpen(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetChestProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetGateProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const override;

    [[nodiscard]] std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> Attack(double angleInRadian) override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const override;

    void Print(std::string str) const override
    {
    }
    void PrintStudent() const override
    {
    }
    void PrintTricker() const override
    {
    }
    void PrintProp() const override
    {
    }
    void PrintSelfInfo() const override
    {
    }

private:
    ILogic& logic;
};

class StudentDebugAPI : public IStudentAPI, public IGameTimer
{
public:
    StudentDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID);
    void StartTimer() override;
    void EndTimer() override;
    void Play(IAI& ai) override;

    [[nodiscard]] int GetFrameCount() const override;

    std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) override;
    std::future<bool> MoveRight(int64_t timeInMilliseconds) override;
    std::future<bool> MoveUp(int64_t timeInMilliseconds) override;
    std::future<bool> MoveLeft(int64_t timeInMilliseconds) override;
    std::future<bool> MoveDown(int64_t timeInMilliseconds) override;

    std::future<bool> PickProp(THUAI6::PropType prop) override;
    std::future<bool> UseProp(THUAI6::PropType prop) override;
    std::future<bool> ThrowProp(THUAI6::PropType prop) override;
    std::future<bool> UseSkill(int32_t skillID) override;

    std::future<bool> Attack(double angleInRadian) override;

    std::future<bool> OpenDoor() override;
    std::future<bool> CloseDoor() override;
    std::future<bool> SkipWindow() override;
    std::future<bool> StartOpenGate() override;
    std::future<bool> StartOpenChest() override;
    std::future<bool> EndAllAction() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] bool HaveMessage() override;
    [[nodiscard]] std::pair<int64_t, std::string> GetMessage() override;

    bool Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] bool IsDoorOpen(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetChestProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetGateProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const override;

    [[nodiscard]] std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> StartLearning() override;
    std::future<bool> StartEncourageMate(int64_t mateID) override;
    std::future<bool> StartRouseMate(int64_t mateID) override;
    std::future<bool> Graduate() override;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Student> GetSelfInfo() const override;

    void Print(std::string str) const override;
    void PrintStudent() const override;
    void PrintTricker() const override;
    void PrintProp() const override;
    void PrintSelfInfo() const override;

private:
    std::chrono::system_clock::time_point startPoint;
    std::unique_ptr<spdlog::logger> logger;
    ILogic& logic;
};

class TrickerDebugAPI : public ITrickerAPI, public IGameTimer
{
public:
    TrickerDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID);
    void StartTimer() override;
    void EndTimer() override;
    void Play(IAI& ai) override;

    [[nodiscard]] int GetFrameCount() const override;

    std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) override;
    std::future<bool> MoveRight(int64_t timeInMilliseconds) override;
    std::future<bool> MoveUp(int64_t timeInMilliseconds) override;
    std::future<bool> MoveLeft(int64_t timeInMilliseconds) override;
    std::future<bool> MoveDown(int64_t timeInMilliseconds) override;

    std::future<bool> PickProp(THUAI6::PropType prop) override;
    std::future<bool> UseProp(THUAI6::PropType prop) override;
    std::future<bool> ThrowProp(THUAI6::PropType prop) override;
    std::future<bool> UseSkill(int32_t skillID) override;

    std::future<bool> OpenDoor() override;
    std::future<bool> CloseDoor() override;
    std::future<bool> SkipWindow() override;
    std::future<bool> StartOpenGate() override;
    std::future<bool> StartOpenChest() override;
    std::future<bool> EndAllAction() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] bool HaveMessage() override;
    [[nodiscard]] std::pair<int64_t, std::string> GetMessage() override;

    bool Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] bool IsDoorOpen(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetChestProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetGateProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const override;

    [[nodiscard]] std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> Attack(double angleInRadian) override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const override;

    void Print(std::string str) const override;
    void PrintStudent() const override;
    void PrintTricker() const override;
    void PrintProp() const override;
    void PrintSelfInfo() const override;

private:
    std::chrono::system_clock::time_point startPoint;
    std::unique_ptr<spdlog::logger> logger;
    ILogic& logic;
};

#endif
