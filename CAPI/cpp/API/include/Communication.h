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

#undef GetMessage
#undef SendMessage
#undef PeekMessage

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
    bool UseProp(THUAI6::PropType prop, int64_t playerID);
    bool ThrowProp(THUAI6::PropType prop, int64_t playerID);
    bool UseSkill(int32_t skillID, int32_t skillParam, int64_t playerID);
    bool SendMessage(int64_t toID, std::string message, bool binary, int64_t playerID);
    bool OpenDoor(int64_t playerID);
    bool CloseDoor(int64_t playerID);
    bool SkipWindow(int64_t playerID);
    bool StartOpenGate(int64_t playerID);
    bool StartOpenChest(int64_t playerID);
    bool EndAllAction(int64_t playerID);

    bool Graduate(int64_t playerID);

    bool StartLearning(int64_t playerID);
    bool StartEncourageMate(int64_t playerID, int64_t mateID);
    bool StartRouseMate(int64_t playerID, int64_t mateID);

    bool Attack(double angle, int64_t playerID);

    bool TryConnection(int64_t playerID);
    protobuf::MessageToClient GetMessage2Client();
    void AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::StudentType studentType, THUAI6::TrickerType trickerType);

private:
    std::unique_ptr<protobuf::AvailableService::Stub> THUAI6Stub;
    bool haveNewMessage = false;
    protobuf::MessageToClient message2Client;
    std::mutex mtxMessage;
    std::mutex mtxLimit;
    int32_t counter;
    int32_t counterMove;
    static constexpr const int32_t limit = 50;
    static constexpr const int32_t moveLimit = 10;
    std::condition_variable cvMessage;
};

#endif
