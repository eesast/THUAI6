#pragma once

#ifndef LOGIC_H
#define LOGIC_H

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Message2Server.pb.h"
#include "Message2Clients.pb.h"
#include "MessageType.pb.h"
#include "Services.grpc.pb.h"
#include "Services.pb.h"
#include "API.h"
#include "AI.h"
#include "structures.h"
#include "state.h"
#include "Communication.h"
#include "ConcurrentQueue.hpp"

// 封装了通信组件和对AI对象进行操作
class Logic : public ILogic
{
private:
    // 日志组件
    std::unique_ptr<spdlog::logger> logger;

    // 通信组件
    std::unique_ptr<Communication> pComm;

    // ID、阵营记录
    THUAI6::PlayerType playerType;
    int64_t playerID;

    // 类型记录
    THUAI6::TrickerType trickerType;
    THUAI6::StudentType studentType;

    // GUID信息
    std::vector<int64_t> playerGUIDs;

    std::unique_ptr<IGameTimer> timer;

    std::thread tAI;  // 用于运行AI的线程

    mutable std::mutex mtxAI;
    mutable std::mutex mtxState;
    mutable std::mutex mtxBuffer;

    std::condition_variable cvBuffer;
    std::condition_variable cvAI;

    // 信息队列
    ConcurrentQueue<std::pair<int64_t, std::string>> messageQueue;

    // 存储状态，分别是现在的状态和缓冲区的状态。
    State state[2];
    State* currentState;
    State* bufferState;

    // 保存缓冲区数
    int counterState = 0;
    int counterBuffer = 0;

    THUAI6::GameState gameState = THUAI6::GameState::NullGameState;

    // 是否应该执行player()
    std::atomic_bool AILoop = true;

    // buffer是否更新完毕
    bool bufferUpdated = true;

    // 是否应当启动AI
    bool AIStart = false;

    // asynchronous = true 时控制内容更新的变量
    std::atomic_bool freshed = false;

    // 提供给API使用的函数

    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const override;
    [[nodiscard]] std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Student> StudentGetSelfInfo() const override;
    [[nodiscard]] std::shared_ptr<const THUAI6::Tricker> TrickerGetSelfInfo() const override;
    [[nodiscard]] THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const override;
    [[nodiscard]] THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] bool IsDoorOpen(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetChestProgress(int32_t cellX, int32_t cellY) const override;
    [[nodiscard]] int32_t GetGateProgress(int32_t cellX, int32_t cellY) const override;

    [[nodiscard]] std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const override;

    // 供IAPI使用的操作相关的部分
    bool Move(int64_t time, double angle) override;
    bool PickProp(THUAI6::PropType prop) override;
    bool UseProp(THUAI6::PropType prop) override;
    bool ThrowProp(THUAI6::PropType prop) override;
    bool UseSkill(int32_t skillID) override;

    bool SendMessage(int64_t toID, std::string message) override;
    bool HaveMessage() override;
    std::pair<int64_t, std::string> GetMessage() override;

    bool Graduate() override;

    bool StartLearning() override;

    bool StartEncourageMate(int64_t mateID) override;
    bool StartRouseMate(int64_t mateID) override;

    bool Attack(double angle) override;

    bool OpenDoor() override;
    bool CloseDoor() override;
    bool SkipWindow() override;
    bool StartOpenGate() override;
    bool StartOpenChest() override;
    bool EndAllAction() override;

    bool WaitThread() override;

    int GetCounter() const override;

    std::vector<int64_t> GetPlayerGUIDs() const override;

    bool TryConnection();

    void ProcessMessage();

    // 将信息加载到buffer
    void LoadBuffer(protobuf::MessageToClient&);

    // 解锁AI线程
    void UnBlockAI();

    // 更新状态
    void Update() noexcept;

    // 等待
    void Wait() noexcept;

public:
    // 构造函数还需要传更多参数，有待补充
    Logic(THUAI6::PlayerType type, int64_t ID, THUAI6::TrickerType tricker, THUAI6::StudentType student);

    ~Logic()
    {
    }

    // Main函数同上
    void Main(CreateAIFunc createAI, std::string IP, std::string port, bool file, bool print, bool warnOnly);
};

#endif
