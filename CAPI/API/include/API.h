#pragma once
#ifndef API_H
#define API_H

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include <Message2Server.pb.h>
#include <Message2Clients.pb.h>
#include <MessageType.pb.h>
#include <Message2Server.grpc.pb.h>
#include <Message2Clients.grpc.pb.h>
#include <MessageType.grpc.pb.h>
#include <future>
#include <iostream>
#include <vector>

#include "structures.h"

const constexpr int num_of_grid_per_cell = 1000;

class ILogic
{
    // API中依赖Logic的部分

public:
    // 获取服务器发来的所有消息，要注意线程安全问题
    virtual protobuf::MessageToClient GetFullMessage() = 0;

    // 供IAPI使用的操作相关的部分
    virtual bool Move(protobuf::MoveMsg) = 0;
    virtual bool PickProp(protobuf::PickMsg) = 0;
    virtual bool UseProp(protobuf::IDMsg) = 0;
    virtual bool UseSkill(protobuf::IDMsg) = 0;
    virtual void SendMessage(protobuf::SendMsg) = 0;
    virtual bool HaveMessage(protobuf::IDMsg) = 0;
    virtual protobuf::MsgRes GetMessage(protobuf::IDMsg) = 0;

    virtual bool Escape(protobuf::IDMsg) = 0;

    // 说明：双向stream由三个函数共同实现，两个记录开始和结束，结果由Logic里的私有的成员变量记录，获得返回值则另调函数
    virtual bool StartFixMachine(protobuf::IDMsg) = 0;
    virtual bool EndFixMachine(protobuf::IDMsg) = 0;
    virtual bool GetFixStatus() = 0;

    virtual bool StartSaveHuman(protobuf::IDMsg) = 0;
    virtual bool EndSaveHuman(protobuf::IDMsg) = 0;
    virtual bool GetSaveStatus() = 0;

    virtual bool Attack(protobuf::AttackMsg) = 0;
    virtual bool CarryHuman(protobuf::IDMsg) = 0;
    virtual bool ReleaseHuman(protobuf::IDMsg) = 0;
    virtual bool HangHuman(protobuf::IDMsg) = 0;

    virtual bool WaitThread() = 0;

    virtual int GetCounter() = 0;
};

class IAPI
{
public:
    // 选手可执行的操作，应当保证所有函数的返回值都应当为std::future，例如下面的移动函数：
    // 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴
    virtual std::future<bool> Move(uint32_t timeInMilliseconds, double angleInRadian) = 0;

    // 向特定方向移动
    virtual std::future<bool> MoveRight(uint32_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveUp(uint32_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveLeft(uint32_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveDown(uint32_t timeInMilliseconds) = 0;

    // 捡道具、使用技能
    virtual std::future<bool> PickProp() = 0;
    virtual std::future<bool> UseProp() = 0;
    virtual std::future<bool> UseSkill() = 0;

    // 发送信息、接受信息
    virtual std::future<bool> SendMessage(int, std::string) = 0;
    [[nodiscard]] virtual std::future<bool> HaveMessage() = 0;
    [[nodiscard]] virtual std::future<std::pair<int, std::string>> GetMessage() = 0;

    // 等待下一帧
    virtual std::future<bool> Wait() = 0;

    // 获取视野内可见的人类/屠夫的信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Human>> GetHuman() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Buthcer>> GetButcher() const = 0;

    // 获取视野内可见的道具信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;

    // 获取地图信息，视野外的地图统一为Land
    [[nodiscard]] virtual std::array<std::array<THUAI6::PlaceType, 50>, 50> GetFullMap() const = 0;
    [[nodiscard]] virtual THUAI6::PlaceType GetPlaceType(int32_t CellX, int32_t CellY) const = 0;

    // 获取所有玩家的GUID
    [[nodiscard]] virtual const std::vector<int64_t> GetPlayerGUIDs() const = 0;

    // 获取游戏目前所进行的帧数
    [[nodiscard]] virtual int GetFrameCount() const = 0;

    /*****选手可能用的辅助函数*****/

    // 获取指定格子中心的坐标
    [[nodiscard]] static inline int CellToGrid(int cell) noexcept
    {
        return cell * num_of_grid_per_cell + num_of_grid_per_cell / 2;
    }

    // 获取指定坐标点所位于的格子的 X 序号
    [[nodiscard]] static inline int GridToCell(int grid) noexcept
    {
        return grid / num_of_grid_per_cell;
    }

    IAPI(ILogic& logic) :
        logic(logic)
    {
    }

    virtual ~IAPI()
    {
    }

protected:
    ILogic& logic;
};

class IHumanAPI : public IAPI
{
public:
    virtual std::future<bool> StartFixMachine() = 0;
    virtual std::future<bool> EndFixMachine() = 0;
    virtual std::future<bool> GetFixStatus() = 0;
    virtual std::future<bool> StartSaveHuman() = 0;
    virtual std::future<bool> EndSaveHuman() = 0;
    virtual std::future<bool> GetSaveStatus() = 0;
    virtual std::future<bool> Escape() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Human> GetSelfInfo() const = 0;
};

class IButcherAPI : public IAPI
{
public:
    virtual std::future<bool> Attack(double angleInRadian) = 0;
    virtual std::future<bool> CarryHuman() = 0;
    virtual std::future<bool> ReleaseHuman() = 0;
    virtual std::future<bool> HangHuman() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Buthcer> GetSelfInfo() const = 0;
};

#endif