#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <functional>
#include "utils.hpp"
using namespace protobuf;
using grpc::ClientContext;
using grpc::Status;

extern const THUAI6::PlayerType playerType;

Logic::Logic(THUAI6::PlayerType type, int64_t ID, THUAI6::ButcherType butcher, THUAI6::HumanType human) :
    playerType(type),
    playerID(ID),
    butcherType(butcher),
    humanType(human)
{
    currentState = &state[0];
    bufferState = &state[1];
}

std::vector<std::shared_ptr<THUAI6::Butcher>> Logic::GetButchers() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->butchers;
}

std::vector<std::shared_ptr<THUAI6::Human>> Logic::GetHumans() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->humans;
}

std::vector<std::shared_ptr<THUAI6::Prop>> Logic::GetProps() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->props;
}

std::shared_ptr<THUAI6::Human> Logic::HumanGetSelfInfo() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->humanSelf;
}

std::shared_ptr<THUAI6::Butcher> Logic::ButcherGetSelfInfo() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->butcherSelf;
}

std::vector<std::vector<THUAI6::PlaceType>> Logic::GetFullMap() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->gamemap;
}

bool Logic::Move(int64_t time, double angle)
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

bool Logic::PickProp(THUAI6::PropType prop)
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

bool Logic::UseProp()
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

bool Logic::UseSkill()
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

bool Logic::SendMessage(int64_t toID, std::string message)
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

bool Logic::HaveMessage()
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

std::pair<std::string, int64_t> Logic::GetMessage()
{
    protobuf::MsgRes getMessageResult;
    ClientContext context;
    auto request = THUAI62Proto::THUAI62ProtobufID(playerID);
    auto status = THUAI6Stub->GetMessage(&context, request, &getMessageResult);
    if (status.ok())
    {
        if (getMessageResult.have_message())
            return std::make_pair(getMessageResult.message_received(), getMessageResult.from_player_id());
        else
            return std::make_pair("", -1);
    }
    else
        return std::make_pair("", -1);
}

bool Logic::Escape()
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

void Logic::ProcessMessage()
{
    auto messageThread = [&]()
    {
        // 首先设置消息、通过加入游戏，开始与服务端建立联系
        protobuf::MessageToClient clientMsg;
        protobuf::PlayerMsg playerMsg = THUAI62Proto::THUAI62ProtobufPlayer(playerID, playerType, humanType, butcherType);
        grpc::ClientContext context;
        auto MessageReader = THUAI6Stub->AddPlayer(&context, playerMsg);

        // 持续读取服务端的消息
        while (MessageReader->Read(&clientMsg))
        {
            LoadBuffer(clientMsg);
        }
    };
    std::thread(messageThread).detach();
}

void Logic::LoadBuffer(protobuf::MessageToClient& message)
{
    // 将消息读入到buffer中
    {
        std::lock_guard<std::mutex> lock(mtxBuffer);

        // 清空原有信息
        bufferState->humans.clear();
        bufferState->butchers.clear();
        bufferState->props.clear();

        // 读取新的信息
        // 读取消息的选择待补充，之后需要另外判断；具体做法应该是先读到自己，然后按照自己的视野做处理。此处暂时全部读了进来
        for (auto itr = message.human_message().begin(); itr != message.human_message().end(); itr++)
        {
            if (itr->player_id() == playerID)
            {
                bufferState->humanSelf = Proto2THUAI6::Protobuf2THUAI6Human(*itr);
                bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(*itr));
            }
            else
                bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(*itr));
        }
        for (auto itr = message.butcher_message().begin(); itr != message.butcher_message().end(); itr++)
        {
            if (itr->player_id() == playerID)
            {
                bufferState->butcherSelf = Proto2THUAI6::Protobuf2THUAI6Butcher(*itr);
                bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(*itr));
            }
            else
                bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(*itr));
        }

        bufferState->gamemap = Proto2THUAI6::Protobuf2THUAI6Map(message.map_message());
        cvBuffer.notify_one();
    }
}

void Logic::Update() noexcept
{
}

void Logic::PlayerWrapper(std::function<void()> player)
{
    {
        std::unique_lock<std::mutex> lock(mtxAI);
        cvAI.wait(lock, [this]()
                  { return AIStart; });
    }
    player();
}

bool Logic::TryConnection()
{
    IDMsg request = THUAI62Proto::THUAI62ProtobufID(playerID);
    BoolRes reply;
    ClientContext context;
    auto status = THUAI6Stub->TryConnection(&context, request, &reply);
    if (status.ok())
        return true;
    else
        return false;
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port)
{
    // 构造AI
    pAI = createAI();

    // 建立与服务器之间通信的Stub
    std::string aim = IP + ':' + port;
    auto channel = grpc::CreateChannel(aim, grpc::InsecureChannelCredentials());
    THUAI6Stub = protobuf::AvailableService::NewStub(channel);

    // 构造timer
    if (playerType == THUAI6::PlayerType::HumanPlayer)
        timer = std::make_unique<HumanAPI>(*this);
    else if (playerType == THUAI6::PlayerType::ButcherPlayer)
        timer = std::make_unique<ButcherAPI>(*this);

    // 构造AI线程
    auto AIThread = [&, this]()
    {
        auto ai = createAI();
        while (AILoop)
        {
            Update();
            timer->StartTimer();
            timer->Play(*ai);
            timer->EndTimer();
        }
    };

    tAI = std::thread(&Logic::PlayerWrapper, this, AIThread);

    // 连接服务器
    if (TryConnection())
    {
        std::cout << "Connect to the server successfully, AI thread will be start." << std::endl;
        if (tAI.joinable())
        {
            std::cout << "Join the AI thread." << std::endl;
            tAI.join();
        }
    }
    else
    {
        std::cout << "Connection error!" << std::endl;
    }
}