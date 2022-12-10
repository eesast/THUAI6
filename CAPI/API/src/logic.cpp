#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <functional>
#include "utils.hpp"
#include "Communication.h"

extern const bool asynchronous;
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
    std::lock_guard<std::mutex> lock(mtxBuffer);
    std::vector<std::shared_ptr<const THUAI6::Butcher>> temp;
    temp.assign(currentState->butchers.begin(), currentState->butchers.end());
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Human>> Logic::GetHumans() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    std::vector<std::shared_ptr<const THUAI6::Human>> temp;
    temp.assign(currentState->humans.begin(), currentState->humans.end());
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> Logic::GetProps() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    std::vector<std::shared_ptr<const THUAI6::Prop>> temp;
    temp.assign(currentState->props.begin(), currentState->props.end());
    return temp;
}

std::shared_ptr<const THUAI6::Human> Logic::HumanGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    return currentState->humanSelf;
}

std::shared_ptr<const THUAI6::Butcher> Logic::ButcherGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    return currentState->butcherSelf;
}

std::vector<std::vector<THUAI6::PlaceType>> Logic::GetFullMap() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    return currentState->gamemap;
}

THUAI6::PlaceType Logic::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
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
    return pComm->HaveMessage();
}

std::pair<int64_t, std::string> Logic::GetMessage()
{
    return pComm->GetMessage();
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

bool Logic::WaitThread()
{
    Update();
    return true;
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
                // here program should decide whether this human can be seen by AI
                if (playerType == THUAI6::PlayerType::HumanPlayer)
                {
                    bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(*itr));
                    std::cout << "Add Human!" << std::endl;
                }
                else
                {
                    int vr = this->currentState->butcherSelf->viewRange;
                    int deltaX = itr->x() - this->currentState->butcherSelf->x;
                    int deltaY = itr->y() - this->currentState->butcherSelf->y;
                    double distance = deltaX * deltaX + deltaY * deltaY;
                    if (distance > vr * vr)
                        continue;
                    else
                    {
                        int divide = abs(deltaX) > abs(deltaY) ? abs(deltaX) : abs(deltaY);
                        int dx = deltaX / divide;
                        int dy = deltaY / divide;
                        bool barrier = false;
                        if (deltaX > 0)
                        {
                            if (deltaY > 0)
                            {
                                for (int i = this->currentState->butcherSelf->x; i < itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->butcherSelf->y; j < itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                            else
                            {
                                for (int i = this->currentState->butcherSelf->x; i < itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->butcherSelf->y; j > itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                        }
                        else
                        {
                            if (deltaY > 0)
                            {
                                for (int i = this->currentState->butcherSelf->x; i > itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->butcherSelf->y; j < itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                            else
                            {
                                for (int i = this->currentState->butcherSelf->x; i > itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->butcherSelf->y; j > itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                        }
                        if (barrier)
                            continue;
                        bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(*itr));
                        std::cout << "Add Human!" << std::endl;
                    }
                }
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
            {
                // here program should decide whether this human can be seen by AI
                if (playerType == THUAI6::PlayerType::ButcherPlayer)
                {
                    bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(*itr));
                    std::cout << "Add Butcher!" << std::endl;
                }
                else
                {
                    int vr = this->currentState->humanSelf->viewRange;
                    int deltaX = itr->x() - this->currentState->humanSelf->x;
                    int deltaY = itr->y() - this->currentState->humanSelf->y;
                    double distance = deltaX * deltaX + deltaY * deltaY;
                    if (distance > vr * vr)
                        continue;
                    else
                    {
                        int divide = abs(deltaX) > abs(deltaY) ? abs(deltaX) : abs(deltaY);
                        int dx = deltaX / divide;
                        int dy = deltaY / divide;
                        bool barrier = false;
                        if (deltaX > 0)
                        {
                            if (deltaY > 0)
                            {
                                for (int i = this->currentState->humanSelf->x; i < itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->humanSelf->y; j < itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                            else
                            {
                                for (int i = this->currentState->humanSelf->x; i < itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->humanSelf->y; j > itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                        }
                        else
                        {
                            if (deltaY > 0)
                            {
                                for (int i = this->currentState->humanSelf->x; i > itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->humanSelf->y; j < itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                            else
                            {
                                for (int i = this->currentState->humanSelf->x; i > itr->x() && !barrier; i += dx)
                                    for (int j = this->currentState->humanSelf->y; j > itr->y(); j += dy)
                                    {
                                        int x_cell = IAPI::GridToCell(i);
                                        int y_cell = IAPI::GridToCell(j);
                                        if (this->currentState->gamemap[x_cell][y_cell] == THUAI6::PlaceType::Wall)
                                        {
                                            barrier = true;
                                            break;
                                        }
                                    }
                            }
                        }
                        if (barrier)
                            continue;
                        bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(*itr));
                        std::cout << "Add Butcher!" << std::endl;
                    }
                }
            }
        }

        bufferState->gamemap = Proto2THUAI6::Protobuf2THUAI6Map(message.map_message());
        if (asynchronous)
        {
            {
                std::lock_guard<std::mutex> lock(mtxState);
                std::swap(currentState, bufferState);
            }
            freshed = true;
        }
        else
            bufferUpdated = true;
        counterBuffer++;
    }
    // 唤醒其他线程
    cvBuffer.notify_one();
}

void Logic::Update() noexcept
{
    if (!asynchronous)
    {
        std::unique_lock<std::mutex> lock(mtxBuffer);

        // 缓冲区被更新之后才可以使用
        cvBuffer.wait(lock, [&]()
                      { return bufferUpdated; });

        std::swap(currentState, bufferState);
        bufferUpdated = false;
        counterState = counterBuffer;
    }
}

void Logic::Wait() noexcept
{
    freshed = false;
    {
        std::unique_lock<std::mutex> lock(mtxBuffer);
        cvBuffer.wait(lock, [&]()
                      { return freshed.load(); });
    }
}

void Logic::UnBlockAI()
{
    {
        std::lock_guard<std::mutex> lock(mtxAI);
        AIStart = true;
    }
    cvAI.notify_one();
}

void Logic::UnBlockBuffer()
{
    {
        std::lock_guard<std::mutex> lock(mtxBuffer);
        bufferUpdated = true;
    }
    cvBuffer.notify_one();
}

int Logic::GetCounter() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    return counterState;
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
        std::cout << "AI Start!" << std::endl;
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
            cvAI.notify_one();
            tAI.join();
        }
    }
    else
    {
        std::cout << "Connection error!" << std::endl;
        return;
    }
}