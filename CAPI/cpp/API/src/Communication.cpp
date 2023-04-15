#include "Communication.h"
#include "utils.hpp"
#include "structures.h"
#include <thread>
#include <mutex>
#include <condition_variable>

using grpc::ClientContext;

Communication::Communication(std::string sIP, std::string sPort)
{
    std::string aim = sIP + ':' + sPort;
    auto channel = grpc::CreateChannel(aim, grpc::InsecureChannelCredentials());
    THUAI6Stub = protobuf::AvailableService::NewStub(channel);
}

bool Communication::Move(int64_t time, double angle, int64_t playerID)
{
    protobuf::MoveRes moveResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufMove(time, angle, playerID);
    auto status = THUAI6Stub->Move(&context, request, &moveResult);
    if (status.ok())
        return moveResult.act_success();
    else
        return false;
}

bool Communication::PickProp(THUAI6::PropType prop, int64_t playerID)
{
    protobuf::BoolRes pickPropResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufProp(prop, playerID);
    auto status = THUAI6Stub->PickProp(&context, request, &pickPropResult);
    if (status.ok())
        return pickPropResult.act_success();
    else
        return false;
}

bool Communication::UseProp(THUAI6::PropType prop, int64_t playerID)
{
    protobuf::BoolRes usePropResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufProp(prop, playerID);
    auto status = THUAI6Stub->UseProp(&context, request, &usePropResult);
    if (status.ok())
        return usePropResult.act_success();
    else
        return false;
}

bool Communication::ThrowProp(THUAI6::PropType prop, int64_t playerID)
{
    protobuf::BoolRes throwPropResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufProp(prop, playerID);
    auto status = THUAI6Stub->ThrowProp(&context, request, &throwPropResult);
    if (status.ok())
        return throwPropResult.act_success();
    else
        return false;
}

bool Communication::UseSkill(int32_t skillID, int64_t playerID)
{
    protobuf::BoolRes useSkillResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufSkill(skillID, playerID);
    auto status = THUAI6Stub->UseSkill(&context, request, &useSkillResult);
    if (status.ok())
        return useSkillResult.act_success();
    else
        return false;
}

bool Communication::SendMessage(int64_t toID, std::string message, int64_t playerID)
{
    protobuf::BoolRes sendMessageResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufSend(message, toID, playerID);
    auto status = THUAI6Stub->SendMessage(&context, request, &sendMessageResult);
    if (status.ok())
        return sendMessageResult.act_success();
    else
        return false;
}

bool Communication::OpenDoor(int64_t playerID)
{
    protobuf::BoolRes openDoorResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->OpenDoor(&context, request, &openDoorResult);
    if (status.ok())
        return openDoorResult.act_success();
    else
        return false;
}

bool Communication::CloseDoor(int64_t playerID)
{
    protobuf::BoolRes closeDoorResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->CloseDoor(&context, request, &closeDoorResult);
    if (status.ok())
        return closeDoorResult.act_success();
    else
        return false;
}

bool Communication::SkipWindow(int64_t playerID)
{
    protobuf::BoolRes skipWindowResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->SkipWindow(&context, request, &skipWindowResult);
    if (status.ok())
        return skipWindowResult.act_success();
    else
        return false;
}

bool Communication::StartOpenGate(int64_t playerID)
{
    protobuf::BoolRes startOpenGateResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->StartOpenGate(&context, request, &startOpenGateResult);
    if (status.ok())
        return startOpenGateResult.act_success();
    else
        return false;
}

bool Communication::StartOpenChest(int64_t playerID)
{
    protobuf::BoolRes startOpenChestResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->StartOpenChest(&context, request, &startOpenChestResult);
    if (status.ok())
        return startOpenChestResult.act_success();
    else
        return false;
}

bool Communication::EndAllAction(int64_t playerID)
{
    protobuf::BoolRes endAllActionResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->EndAllAction(&context, request, &endAllActionResult);
    if (status.ok())
        return endAllActionResult.act_success();
    else
        return false;
}

bool Communication::Graduate(int64_t playerID)
{
    protobuf::BoolRes graduateResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->Graduate(&context, request, &graduateResult);
    if (status.ok())
        return graduateResult.act_success();
    else
        return false;
}

bool Communication::StartLearning(int64_t playerID)
{
    protobuf::BoolRes startLearningResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->StartLearning(&context, request, &startLearningResult);
    if (status.ok())
        return startLearningResult.act_success();
    else
        return false;
}

bool Communication::StartRouseMate(int64_t playerID, int64_t mateID)
{
    protobuf::BoolRes saveStudentResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufTreatAndRescue(playerID, mateID);
    auto status = THUAI6Stub->StartRescueMate(&context, request, &saveStudentResult);
    if (status.ok())
        return saveStudentResult.act_success();
    else
        return false;
}

bool Communication::StartEncourageMate(int64_t playerID, int64_t mateID)
{
    protobuf::BoolRes healStudentResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufTreatAndRescue(playerID, mateID);
    auto status = THUAI6Stub->StartTreatMate(&context, request, &healStudentResult);
    if (status.ok())
        return healStudentResult.act_success();
    else
        return false;
}

bool Communication::Attack(double angle, int64_t playerID)
{
    protobuf::BoolRes attackResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufAttack(angle, playerID);
    auto status = THUAI6Stub->Attack(&context, request, &attackResult);
    if (status.ok())
        return attackResult.act_success();
    else
        return false;
}

bool Communication::TryConnection(int64_t playerID)
{
    protobuf::BoolRes reply;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->TryConnection(&context, request, &reply);
    if (status.ok())
    {
        return true;
    }
    else
        return false;
}

protobuf::MessageToClient Communication::GetMessage2Client()
{
    std::unique_lock<std::mutex> lock(mtxMessage);
    cvMessage.wait(lock, [this]()
                   { return haveNewMessage; });
    haveNewMessage = false;
    return message2Client;
}

void Communication::AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::StudentType studentType, THUAI6::TrickerType trickerType)
{
    auto tMessage = [=]()
    {
        protobuf::PlayerMsg playerMsg = THUAI62Proto::THUAI62ProtobufPlayer(playerID, playerType, studentType, trickerType);
        grpc::ClientContext context;
        auto MessageReader = THUAI6Stub->AddPlayer(&context, playerMsg);

        while (MessageReader->Read(&message2Client))
        {
            {
                std::lock_guard<std::mutex> lock(mtxMessage);
                haveNewMessage = true;
            }
            cvMessage.notify_one();
        }
    };
    std::thread(tMessage).detach();
}
