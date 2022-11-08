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

#include "Message2Server.pb.h"
#include "Message2Clients.pb.h"
#include "MessageType.pb.h"
#include "Services.grpc.pb.h"
#include "Services.pb.h"
#include "API.h"
#include "AI.h"
#include "structures.h"
#include "state.h"

// 封装了通信组件和对AI对象进行操作
class Logic : public ILogic
{
private:
    // gRPC客户端的stub，所有与服务端之间的通信操作都需要基于stub完成。
    std::unique_ptr<protobuf::AvailableService::Stub> THUAI6Stub;
    // ID、阵营记录
    int playerID;
    THUAI6::PlayerType playerType;

    // 类型记录
    THUAI6::HumanType humanType;
    THUAI6::ButcherType butcherType;

    // GUID信息
    std::vector<int64_t> playerGUIDs;

    // THUAI5中的通信组件可以完全被我们的stub取代，故无须再写

    std::unique_ptr<IAI> pAI;

    std::unique_ptr<IGameTimer> timer;

    std::thread tAI;

    mutable std::mutex mtxAI;
    mutable std::mutex mtxState;
    mutable std::mutex mtxBuffer;

    std::condition_variable cvBuffer;
    std::condition_variable cvAI;

    // 信息队列目前可能会不用？具体待定

    // 存储状态，分别是现在的状态和缓冲区的状态。
    State state[2];
    State* currentState;
    State* bufferState;

    // 是否应该执行player()
    std::atomic_bool AILoop = true;

    // buffer是否更新完毕
    bool bufferUpdated = true;

    // 是否可以启用当前状态
    bool currentStateAccessed = false;

    // 是否应当启动AI
    bool AIStart = false;

    // 控制内容更新的变量
    std::atomic_bool freshed = false;

    // 提供给API使用的函数

    // 获取服务器发来的所有消息，要注意线程安全问题
    protobuf::MessageToClient GetFullMessage() override;

    // 供IAPI使用的操作相关的部分
    bool Move(protobuf::MoveMsg) override
    {
    }
    bool PickProp(protobuf::PickMsg) override
    {
    }
    bool UseProp(protobuf::IDMsg) override
    {
    }
    bool UseSkill(protobuf::IDMsg) override
    {
    }
    void SendMessage(protobuf::SendMsg) override
    {
    }
    bool HaveMessage(protobuf::IDMsg) override
    {
    }
    protobuf::MsgRes GetMessage(protobuf::IDMsg) override
    {
    }

    bool Escape(protobuf::IDMsg) override
    {
    }

    // 说明：双向stream由三个函数共同实现，两个记录开始和结束，结果由Logic里的私有的成员变量记录，获得返回值则另调函数
    bool StartFixMachine(protobuf::IDMsg) override
    {
    }
    bool EndFixMachine(protobuf::IDMsg) override
    {
    }
    bool GetFixStatus() override
    {
    }

    bool StartSaveHuman(protobuf::IDMsg) override
    {
    }
    bool EndSaveHuman(protobuf::IDMsg) override
    {
    }
    bool GetSaveStatus() override
    {
    }

    bool Attack(protobuf::AttackMsg) override
    {
    }
    bool CarryHuman(protobuf::IDMsg) override
    {
    }
    bool ReleaseHuman(protobuf::IDMsg) override
    {
    }
    bool HangHuman(protobuf::IDMsg) override
    {
    }

    bool WaitThread() override
    {
    }

    int GetCounter() override
    {
    }

    bool TryConnection();

    // 执行AI线程
    void PlayerWrapper(std::function<void()> player);

    // THUAI5中的一系列用于处理信息的函数可能也不会再用

    // 将信息加载到buffer
    void LoadBuffer(std::shared_ptr<protobuf::MessageToClient>);

    // 解锁状态更新线程
    void UnBlockBuffer();

    // 解锁AI线程
    void UnBlockAI();

    // 更新状态
    void Update() noexcept;

    // 等待
    void Wait() noexcept;

public:
    // 构造函数还需要传更多参数，有待补充
    Logic(THUAI6::PlayerType type, int playerID, THUAI6::ButcherType butcher, THUAI6::HumanType human);

    ~Logic() = default;

    // Main函数同上
    void Main(CreateAIFunc createAI, std::string IP, std::string port);
};

#endif