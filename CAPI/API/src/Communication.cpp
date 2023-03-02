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
    std::cout << "Move request sent" << std::endl;
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
    auto request = THUAI62Proto::THUAI62ProtobufPick(prop, playerID);
    auto status = THUAI6Stub->PickProp(&context, request, &pickPropResult);
    if (status.ok())
        return pickPropResult.act_success();
    else
        return false;
}

bool Communication::UseProp(int64_t playerID)
{
    protobuf::BoolRes usePropResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->UseProp(&context, request, &usePropResult);
    if (status.ok())
        return usePropResult.act_success();
    else
        return false;
}

bool Communication::UseSkill(int64_t playerID)
{
    protobuf::BoolRes useSkillResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
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

bool Communication::EndLearning(int64_t playerID)
{
    protobuf::BoolRes endLearningResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->EndLearning(&context, request, &endLearningResult);
    if (status.ok())
        return endLearningResult.act_success();
    else
        return false;
}

bool Communication::StartHelpMate(int64_t playerID)
{
    protobuf::BoolRes saveStudentResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->StartHelpMate(&context, request, &saveStudentResult);
    if (status.ok())
        return saveStudentResult.act_success();
    else
        return false;
}

bool Communication::EndHelpMate(int64_t playerID)
{
    protobuf::BoolRes saveStudentResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->EndHelpMate(&context, request, &saveStudentResult);
    if (status.ok())
        return saveStudentResult.act_success();
    else
        return false;
}

bool Communication::Trick(double angle, int64_t playerID)
{
    protobuf::BoolRes trickResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufTrick(angle, playerID);
    auto status = THUAI6Stub->Trick(&context, request, &trickResult);
    if (status.ok())
        return trickResult.act_success();
    else
        return false;
}

bool Communication::StartExam(int64_t playerID)
{
    protobuf::BoolRes startExamResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->StartExam(&context, request, &startExamResult);
    if (status.ok())
        return startExamResult.act_success();
    else
        return false;
}

bool Communication::EndExam(int64_t playerID)
{
    protobuf::BoolRes endExamResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->EndExam(&context, request, &endExamResult);
    if (status.ok())
        return endExamResult.act_success();
    else
        return false;
}

bool Communication::MakeFail(int64_t playerID)
{
    protobuf::BoolRes makeFailResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->MakeFail(&context, request, &makeFailResult);
    if (status.ok())
        return makeFailResult.act_success();
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
        std::cout << "Connection success!" << std::endl;
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

std::optional<std::pair<int64_t, std::string>> Communication::GetMessage()
{
    return messageQueue.tryPop();
}

bool Communication::HaveMessage()
{
    return !messageQueue.empty();
}

void Communication::ReadMessage(int64_t playerID)
{
    auto tRead = [=]()
    {
        auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
        ClientContext context;
        protobuf::MsgRes messageReceived;
        auto reader = THUAI6Stub->GetMessage(&context, request);
        while (reader->Read(&messageReceived))
        {
            messageQueue.emplace(messageReceived.from_player_id(), messageReceived.message_received());
        }
    };
    std::thread(tRead).detach();
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
