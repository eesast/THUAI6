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
    bool SendMessage(int64_t toID, std::string message, int64_t playerID);
    bool HaveMessage(int64_t playerID);
    std::pair<int64_t, std::string> GetMessage(int64_t playerID);
    bool Escape(int64_t playerID);

    void StartFixMachine(int64_t playerID);
    void EndFixMachine();
    bool GetFixStatus();
    void StartSaveHuman(int64_t playerID);
    void EndSaveHuman();
    bool GetSaveStatus();

    bool Attack(double angle, int64_t playerID);

    bool CarryHuman(int64_t playerID);
    bool ReleaseHuman(int64_t playerID);
    bool HangHuman(int64_t playerID);

    bool TryConnection(int64_t playerID);
    protobuf::MessageToClient GetMessage2Client();
    bool HaveMessage2Client();
    void AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::HumanType humanType, THUAI6::ButcherType butcherType);

private:
    void FixMachine(int64_t playerID);
    void SaveHuman(int64_t playerID);
    std::unique_ptr<protobuf::AvailableService::Stub> THUAI6Stub;
    bool haveNewMessage = false;
    protobuf::MessageToClient message2Client;
    bool isFixing = false;
    bool isSaving = false;
};

#endif