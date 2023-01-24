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
    virtual std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const = 0;
    virtual std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const = 0;
    virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;
    virtual std::shared_ptr<const THUAI6::Human> HumanGetSelfInfo() const = 0;
    virtual std::shared_ptr<const THUAI6::Butcher> ButcherGetSelfInfo() const = 0;

    virtual std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const = 0;
    virtual THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const = 0;

    // 供IAPI使用的操作相关的部分
    virtual bool Move(int64_t time, double angle) = 0;
    virtual bool PickProp(THUAI6::PropType prop) = 0;
    virtual bool UseProp() = 0;
    virtual bool UseSkill() = 0;
    virtual bool SendMessage(int64_t toID, std::string message) = 0;
    virtual bool HaveMessage() = 0;
    virtual std::optional<std::pair<int64_t, std::string>> GetMessage() = 0;

    virtual bool WaitThread() = 0;

    virtual int GetCounter() const = 0;

    // IHumanAPI使用的部分
    virtual bool Escape() = 0;

    virtual bool StartFixMachine() = 0;
    virtual bool EndFixMachine() = 0;

    virtual bool StartSaveHuman() = 0;
    virtual bool EndSaveHuman() = 0;

    // IButcherAPI使用的部分
    virtual bool Attack(double angle) = 0;
    virtual bool CarryHuman() = 0;
    virtual bool ReleaseHuman() = 0;
    virtual bool HangHuman() = 0;

    virtual const std::vector<int64_t> GetPlayerGUIDs() const = 0;
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
    virtual std::future<bool> UseProp() = 0;
    virtual std::future<bool> UseSkill() = 0;

    // 发送信息、接受信息，注意收消息时无消息则返回nullopt
    virtual std::future<bool> SendMessage(int64_t, std::string) = 0;
    [[nodiscard]] virtual std::future<bool> HaveMessage() = 0;
    [[nodiscard]] virtual std::future<std::optional<std::pair<int64_t, std::string>>> GetMessage() = 0;

    // 等待下一帧
    virtual std::future<bool> Wait() = 0;

    // 获取视野内可见的人类/屠夫的信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const = 0;

    // 获取视野内可见的道具信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;

    // 获取地图信息，视野外的地图统一为Land
    [[nodiscard]] virtual std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const = 0;
    [[nodiscard]] virtual THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const = 0;

    // 获取所有玩家的GUID
    [[nodiscard]] virtual const std::vector<int64_t> GetPlayerGUIDs() const = 0;

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

    virtual void PrintHuman() const = 0;
    virtual void PrintButcher() const = 0;
    virtual void PrintProp() const = 0;
    virtual void PrintSelfInfo() const = 0;
};

class IHumanAPI : public IAPI
{
public:
    /*****人类阵营的特定函数*****/

    virtual std::future<bool> StartFixMachine() = 0;
    virtual std::future<bool> EndFixMachine() = 0;
    virtual std::future<bool> StartSaveHuman() = 0;
    virtual std::future<bool> EndSaveHuman() = 0;
    virtual std::future<bool> Escape() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Human> GetSelfInfo() const = 0;
};

class IButcherAPI : public IAPI
{
public:
    /*****屠夫阵营的特定函数*****/

    virtual std::future<bool> Attack(double angleInRadian) = 0;
    virtual std::future<bool> CarryHuman() = 0;
    virtual std::future<bool> ReleaseHuman() = 0;
    virtual std::future<bool> HangHuman() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Butcher> GetSelfInfo() const = 0;
};

class IGameTimer
{
public:
    virtual ~IGameTimer() = default;
    virtual void StartTimer() = 0;
    virtual void EndTimer() = 0;
    virtual void Play(IAI& ai) = 0;
};

class HumanAPI : public IHumanAPI, public IGameTimer
{
public:
    HumanAPI(ILogic& logic) :
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
    std::future<bool> UseProp() override;
    std::future<bool> UseSkill() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] std::future<bool> HaveMessage() override;
    [[nodiscard]] std::future<std::optional<std::pair<int64_t, std::string>>> GetMessage() override;

    std::future<bool> Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const override;

    [[nodiscard]] const std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> StartFixMachine() override;
    std::future<bool> EndFixMachine() override;
    std::future<bool> StartSaveHuman() override;
    std::future<bool> EndSaveHuman() override;
    std::future<bool> Escape() override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Human> GetSelfInfo() const override;

    void PrintHuman() const override
    {
    }
    void PrintButcher() const override
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

class ButcherAPI : public IButcherAPI, public IGameTimer
{
public:
    ButcherAPI(ILogic& logic) :
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
    std::future<bool> UseProp() override;
    std::future<bool> UseSkill() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] std::future<bool> HaveMessage() override;
    [[nodiscard]] std::future<std::optional<std::pair<int64_t, std::string>>> GetMessage() override;

    std::future<bool> Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const override;

    [[nodiscard]] const std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> Attack(double angleInRadian) override;
    std::future<bool> CarryHuman() override;
    std::future<bool> ReleaseHuman() override;
    std::future<bool> HangHuman() override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Butcher> GetSelfInfo() const override;

    void PrintHuman() const override
    {
    }
    void PrintButcher() const override
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

class HumanDebugAPI : public IHumanAPI, public IGameTimer
{
public:
    HumanDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID);
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
    std::future<bool> UseProp() override;
    std::future<bool> UseSkill() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] std::future<bool> HaveMessage() override;
    [[nodiscard]] std::future<std::optional<std::pair<int64_t, std::string>>> GetMessage() override;

    std::future<bool> Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const override;

    [[nodiscard]] const std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> StartFixMachine() override;
    std::future<bool> EndFixMachine() override;
    std::future<bool> StartSaveHuman() override;
    std::future<bool> EndSaveHuman() override;
    std::future<bool> Escape() override;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Human> GetSelfInfo() const override;

    void PrintHuman() const override;
    void PrintButcher() const override;
    void PrintProp() const override;
    void PrintSelfInfo() const override;

private:
    std::chrono::system_clock::time_point startPoint;
    std::unique_ptr<spdlog::logger> logger;
    ILogic& logic;
};

class ButcherDebugAPI : public IButcherAPI, public IGameTimer
{
public:
    ButcherDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID);
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
    std::future<bool> UseProp() override;
    std::future<bool> UseSkill() override;

    std::future<bool> SendMessage(int64_t, std::string) override;
    [[nodiscard]] std::future<bool> HaveMessage() override;
    [[nodiscard]] std::future<std::optional<std::pair<int64_t, std::string>>> GetMessage() override;

    std::future<bool> Wait() override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Human>> GetHumans() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Butcher>> GetButchers() const override;

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const override;

    [[nodiscard]] const std::vector<int64_t> GetPlayerGUIDs() const override;

    std::future<bool> Attack(double angleInRadian) override;
    std::future<bool> CarryHuman() override;
    std::future<bool> ReleaseHuman() override;
    std::future<bool> HangHuman() override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Butcher> GetSelfInfo() const override;

    void PrintHuman() const override;
    void PrintButcher() const override;
    void PrintProp() const override;
    void PrintSelfInfo() const override;

private:
    std::chrono::system_clock::time_point startPoint;
    std::unique_ptr<spdlog::logger> logger;
    ILogic& logic;
};

#endif
