#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Message2Server.pb.h"
#include "Message2Clients.pb.h"
#include "MessageType.pb.h"
#include "Services.grpc.pb.h"
#include "Services.pb.h"
#include <grpcpp/grpcpp.h>
#include "structures.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include "ConcurrentQueue.hpp"

class Logic;

class Communication
{
public:
    Communication(std::string sIP, std::string sPort);
    ~Communication()
    {
    }
    bool Move(int64_t time, double angle, int64_t playerID);
    bool PickProp(THUAI6::PropType prop, int64_t playerID);
    bool UseProp(int64_t playerID);
    bool UseSkill(int64_t playerID);
    std::optional<std::pair<int64_t, std::string>> GetMessage();
    bool HaveMessage();
    bool SendMessage(int64_t toID, std::string message, int64_t playerID);
    bool Escape(int64_t playerID);

    bool StartFixMachine(int64_t playerID);
    bool EndFixMachine(int64_t playerID);
    bool StartSaveHuman(int64_t playerID);
    bool EndSaveHuman(int64_t playerID);

    bool Attack(double angle, int64_t playerID);

    bool CarryHuman(int64_t playerID);
    bool ReleaseHuman(int64_t playerID);
    bool HangHuman(int64_t playerID);

    bool TryConnection(int64_t playerID);
    protobuf::MessageToClient GetMessage2Client();
    void AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::HumanType humanType, THUAI6::ButcherType butcherType);

    void ReadMessage(int64_t playerID);

private:
    std::unique_ptr<protobuf::AvailableService::Stub> THUAI6Stub;
    bool haveNewMessage = false;
    protobuf::MessageToClient message2Client;
    ConcurrentQueue<std::pair<int64_t, std::string>> messageQueue;
    std::mutex mtxMessage;
    std::condition_variable cvMessage;
};

#endif
