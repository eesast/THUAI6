#include "Communication.h"
#include "utils.hpp"
#include "structures.h"
#include <thread>

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

bool Communication::HaveMessage(int64_t playerID)
{
    protobuf::BoolRes haveMessageResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->HaveMessage(&context, request, &haveMessageResult);
    if (status.ok())
        return haveMessageResult.act_success();
    else
        return false;
}

std::pair<int64_t, std::string> Communication::GetMessage(int64_t playerID)
{
    protobuf::MsgRes getMessageResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->GetMessage(&context, request, &getMessageResult);
    if (status.ok())
    {
        if (getMessageResult.have_message())
            return std::make_pair(getMessageResult.from_player_id(), getMessageResult.message_received());
        else
            return std::make_pair(-1, "");
    }
    else
        return std::make_pair(-1, "");
}

bool Communication::Escape(int64_t playerID)
{
    protobuf::BoolRes escapeResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->Escape(&context, request, &escapeResult);
    if (status.ok())
        return escapeResult.act_success();
    else
        return false;
}

void Communication::FixMachine(int64_t playerID)
{
    protobuf::BoolRes fixMachineResult;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    ClientContext context;
    while (isFixing)
    {
        auto fixStream = THUAI6Stub->FixMachine(&context);
        fixStream->Write(request);
        fixStream->Read(&fixMachineResult);
        if (!fixMachineResult.act_success())
        {
            isFixing = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 防止巨量发信
    }
}

void Communication::StartFixMachine(int64_t playerID)
{
    isFixing = true;
    FixMachine(playerID);
}

void Communication::EndFixMachine()
{
    isFixing = false;
}

bool Communication::GetFixStatus()
{
    return isFixing;
}

void Communication::SaveHuman(int64_t playerID)
{
    protobuf::BoolRes saveHumanResult;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    ClientContext context;
    while (isSaving)
    {
        auto saveStream = THUAI6Stub->SaveHuman(&context);
        saveStream->Write(request);
        saveStream->Read(&saveHumanResult);
        if (!saveHumanResult.act_success())
        {
            isSaving = false;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 防止巨量发信
    }
}

void Communication::StartSaveHuman(int64_t playerID)
{
    isSaving = true;
    SaveHuman(playerID);
}

void Communication::EndSaveHuman()
{
    isSaving = false;
}

bool Communication::GetSaveStatus()
{
    return isSaving;
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

bool Communication::CarryHuman(int64_t playerID)
{
    protobuf::BoolRes carryHumanResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->CarryHuman(&context, request, &carryHumanResult);
    if (status.ok())
        return carryHumanResult.act_success();
    else
        return false;
}

bool Communication::ReleaseHuman(int64_t playerID)
{
    protobuf::BoolRes releaseHumanResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->ReleaseHuman(&context, request, &releaseHumanResult);
    if (status.ok())
        return releaseHumanResult.act_success();
    else
        return false;
}

bool Communication::HangHuman(int64_t playerID)
{
    protobuf::BoolRes hangHumanResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->HangHuman(&context, request, &hangHumanResult);
    if (status.ok())
        return hangHumanResult.act_success();
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
        return true;
    else
        return false;
}

protobuf::MessageToClient Communication::GetMessage2Client()
{
    haveNewMessage = false;
    return message2Client;
}

bool Communication::HaveMessage2Client()
{
    return haveNewMessage;
}

void Communication::AddPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::HumanType humanType, THUAI6::ButcherType butcherType)
{
    auto msgThread = [&]()
    {
        protobuf::PlayerMsg playerMsg = THUAI62Proto::THUAI62ProtobufPlayer(playerID, playerType, humanType, butcherType);
        grpc::ClientContext context;
        auto MessageReader = THUAI6Stub->AddPlayer(&context, playerMsg);

        while (MessageReader->Read(&message2Client))
            haveNewMessage = true;
    };
    std::thread(msgThread).detach();
}