#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <functional>
#include "utils.hpp"
using namespace protobuf;
using grpc::ClientContext;
using grpc::Status;

extern const THUAI6::PlayerType playerType;

Logic::Logic(THUAI6::PlayerType type, int ID, THUAI6::ButcherType butcher, THUAI6::HumanType human) :
    playerType(type),
    playerID(ID),
    butcherType(butcher),
    humanType(human)
{
    currentState = &state[0];
    bufferState = &state[1];
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