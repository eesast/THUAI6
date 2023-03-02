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
    bool Graduate(int64_t playerID);

    bool StartLearning(int64_t playerID);
    bool EndLearning(int64_t playerID);
    bool StartHelpMate(int64_t playerID);
    bool EndHelpMate(int64_t playerID);

    bool Trick(double angle, int64_t playerID);

    bool StartExam(int64_t playerID);
    bool EndExam(int64_t playerID);
    bool MakeFail(int64_t playerID);

    bool TryConnection(int64_t playerID);
    protobuf::MessageToClient GetMessage2Client();
    void AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::StudentType studentType, THUAI6::TrickerType trickerType);

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
