#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <functional>
#include "utils.hpp"
#include "Communication.h"

// using grpc::ClientContext;
// using grpc::Status;

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

std::vector<std::shared_ptr<const THUAI6::Butcher>> Logic::GetButchers() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Butcher>> temp;
    temp.assign(currentState->butchers.begin(), currentState->butchers.end());
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Human>> Logic::GetHumans() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Human>> temp;
    temp.assign(currentState->humans.begin(), currentState->humans.end());
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> Logic::GetProps() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Prop>> temp;
    temp.assign(currentState->props.begin(), currentState->props.end());
    return temp;
}

std::shared_ptr<const THUAI6::Human> Logic::HumanGetSelfInfo() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->humanSelf;
}

std::shared_ptr<const THUAI6::Butcher> Logic::ButcherGetSelfInfo() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->butcherSelf;
}

std::vector<std::vector<THUAI6::PlaceType>> Logic::GetFullMap() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->gamemap;
}

THUAI6::PlaceType Logic::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    std::lock_guard<std::mutex> lock(mtxState);
    return currentState->gamemap[CellX][CellY];
}

bool Logic::Move(int64_t time, double angle)
{
    return pComm->Move(time, angle, playerID);
}

bool Logic::PickProp(THUAI6::PropType prop)
{
    return pComm->PickProp(prop, playerID);
}

bool Logic::UseProp()
{
    return pComm->UseProp(playerID);
}

bool Logic::UseSkill()
{
    return pComm->UseSkill(playerID);
}

bool Logic::SendMessage(int64_t toID, std::string message)
{
    return pComm->SendMessage(toID, message, playerID);
}

bool Logic::HaveMessage()
{
    return pComm->HaveMessage(playerID);
}

std::pair<int64_t, std::string> Logic::GetMessage()
{
    return pComm->GetMessage(playerID);
}

bool Logic::Escape()
{
    return pComm->Escape(playerID);
}

bool Logic::StartFixMachine()
{
    return pComm->StartFixMachine(playerID);
}

bool Logic::EndFixMachine()
{
    return pComm->EndFixMachine(playerID);
}

bool Logic::StartSaveHuman()
{
    return pComm->StartSaveHuman(playerID);
}

bool Logic::EndSaveHuman()
{
    return pComm->EndSaveHuman(playerID);
}

bool Logic::Attack(double angle)
{
    return pComm->Attack(angle, playerID);
}

bool Logic::CarryHuman()
{
    return pComm->CarryHuman(playerID);
}

bool Logic::ReleaseHuman()
{
    return pComm->ReleaseHuman(playerID);
}

bool Logic::HangHuman()
{
    return pComm->HangHuman(playerID);
}

void Logic::ProcessMessage()
{
    auto messageThread = [&]()
    {
        std::cout << "Join Player!" << std::endl;
        pComm->AddPlayer(playerID, playerType, humanType, butcherType);
        while (true)
        {
            if (pComm->HaveMessage2Client())
            {
                std::cout << "Get Message!" << std::endl;
                auto clientMsg = pComm->GetMessage2Client();
                LoadBuffer(clientMsg);
            }
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

        std::cout << "Buffer clear!" << std::endl;

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
            {
                bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(*itr));
                std::cout << "Add Human!" << std::endl;
            }
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

bool Logic::TryConnection()
{
    std::cout << "Trying to connect to server..." << std::endl;
    bool result = pComm->TryConnection(playerID);
    return result;
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port)
{
    // 构造AI
    pAI = createAI();

    // 建立与服务器之间通信的组件
    pComm = std::make_unique<Communication>(IP, port);

    // 构造timer
    if (playerType == THUAI6::PlayerType::HumanPlayer)
        timer = std::make_unique<HumanAPI>(*this);
    else if (playerType == THUAI6::PlayerType::ButcherPlayer)
        timer = std::make_unique<ButcherAPI>(*this);

    // 构造AI线程
    auto AIThread = [&]()
    {
        {
            std::unique_lock<std::mutex> lock(mtxAI);
            cvAI.wait(lock, [this]()
                      { return AIStart; });
        }
        auto ai = createAI();
        ProcessMessage();
        while (AILoop)
        {
            Update();
            timer->StartTimer();
            timer->Play(*ai);
            timer->EndTimer();
        }
    };

    tAI = std::thread(AIThread);

    // 连接服务器
    if (TryConnection())
    {
        std::cout << "Connect to the server successfully, AI thread will be start." << std::endl;
        if (tAI.joinable())
        {
            std::cout << "Join the AI thread." << std::endl;
            AIStart = true;
            tAI.join();
        }
    }
    else
    {
        std::cout << "Connection error!" << std::endl;
        return;
    }
}