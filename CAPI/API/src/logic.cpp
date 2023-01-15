#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <functional>
#include "utils.hpp"
#include "Communication.h"

extern const bool asynchronous;

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
    logger->debug("Called GetButchers");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Human>> Logic::GetHumans() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    std::vector<std::shared_ptr<const THUAI6::Human>> temp;
    temp.assign(currentState->humans.begin(), currentState->humans.end());
    logger->debug("Called GetHumans");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> Logic::GetProps() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    std::vector<std::shared_ptr<const THUAI6::Prop>> temp;
    temp.assign(currentState->props.begin(), currentState->props.end());
    logger->debug("Called GetProps");
    return temp;
}

std::shared_ptr<const THUAI6::Human> Logic::HumanGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    logger->debug("Called HumanGetSelfInfo");
    return currentState->humanSelf;
}

std::shared_ptr<const THUAI6::Butcher> Logic::ButcherGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    logger->debug("Called ButcherGetSelfInfo");
    return currentState->butcherSelf;
}

std::vector<std::vector<THUAI6::PlaceType>> Logic::GetFullMap() const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    logger->debug("Called GetFullMap");
    return currentState->gamemap;
}

THUAI6::PlaceType Logic::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxBuffer);
    logger->debug("Called GetPlaceType");
    return currentState->gamemap[CellX][CellY];
}

bool Logic::Move(int64_t time, double angle)
{
    logger->debug("Called Move");
    return pComm->Move(time, angle, playerID);
}

bool Logic::PickProp(THUAI6::PropType prop)
{
    logger->debug("Called PickProp");
    return pComm->PickProp(prop, playerID);
}

bool Logic::UseProp()
{
    logger->debug("Called UseProp");
    return pComm->UseProp(playerID);
}

bool Logic::UseSkill()
{
    logger->debug("Called UseSkill");
    return pComm->UseSkill(playerID);
}

bool Logic::SendMessage(int64_t toID, std::string message)
{
    logger->debug("Called SendMessage");
    return pComm->SendMessage(toID, message, playerID);
}

bool Logic::HaveMessage()
{
    logger->debug("Called HaveMessage");
    return pComm->HaveMessage();
}

std::optional<std::pair<int64_t, std::string>> Logic::GetMessage()
{
    logger->debug("Called GetMessage");
    return pComm->GetMessage();
}

bool Logic::Escape()
{
    logger->debug("Called Escape");
    return pComm->Escape(playerID);
}

bool Logic::StartFixMachine()
{
    logger->debug("Called StartFixMachine");
    return pComm->StartFixMachine(playerID);
}

bool Logic::EndFixMachine()
{
    logger->debug("Called EndFixMachine");
    return pComm->EndFixMachine(playerID);
}

bool Logic::StartSaveHuman()
{
    logger->debug("Called StartSaveHuman");
    return pComm->StartSaveHuman(playerID);
}

bool Logic::EndSaveHuman()
{
    logger->debug("Called EndSaveHuman");
    return pComm->EndSaveHuman(playerID);
}

bool Logic::Attack(double angle)
{
    logger->debug("Called Attack");
    return pComm->Attack(angle, playerID);
}

bool Logic::CarryHuman()
{
    logger->debug("Called CarryHuman");
    return pComm->CarryHuman(playerID);
}

bool Logic::ReleaseHuman()
{
    logger->debug("Called ReleaseHuman");
    return pComm->ReleaseHuman(playerID);
}

bool Logic::HangHuman()
{
    logger->debug("Called HangHuman");
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
        logger->info("Message thread start!");
        pComm->AddPlayer(playerID, playerType, humanType, butcherType);
        logger->info("Join the player!");
        while (gameState != THUAI6::GameState::GameEnd)
        {
            if (pComm->HaveMessage2Client())
            {
                logger->debug("Get message from server!");
                auto clientMsg = pComm->GetMessage2Client();
                gameState = Proto2THUAI6::gameStateDict[clientMsg.game_state()];
                switch (gameState)
                {
                    case THUAI6::GameState::GameStart:
                        logger->info("Game Start!");

                        // 重新读取玩家的guid，guid确保人类在前屠夫在后
                        playerGUIDs.clear();
                        for (auto human : clientMsg.human_message())
                            playerGUIDs.push_back(human.guid());
                        for (auto butcher : clientMsg.butcher_message())
                            playerGUIDs.push_back(butcher.guid());
                        currentState->guids = playerGUIDs;
                        bufferState->guids = playerGUIDs;

                        LoadBuffer(clientMsg);

                        AILoop = true;
                        UnBlockAI();

                        break;
                    case THUAI6::GameState::GameRunning:
                        // 重新读取玩家的guid，guid确保人类在前屠夫在后
                        playerGUIDs.clear();
                        for (auto human : clientMsg.human_message())
                            playerGUIDs.push_back(human.guid());
                        for (auto butcher : clientMsg.butcher_message())
                            playerGUIDs.push_back(butcher.guid());
                        currentState->guids = playerGUIDs;
                        bufferState->guids = playerGUIDs;

                        LoadBuffer(clientMsg);
                        break;
                    case THUAI6::GameState::GameEnd:
                        AILoop = false;
                        {
                            std::lock_guard<std::mutex> lock(mtxBuffer);
                            bufferUpdated = true;
                            counterBuffer = -1;
                        }
                        cvBuffer.notify_one();
                        logger->info("Game End!");
                        break;
                    default:
                        logger->debug("Unknown GameState!");
                }
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

        logger->debug("Buffer cleared!");
        // 读取新的信息
        // 读取消息的选择待补充，之后需要另外判断；具体做法应该是先读到自己，然后按照自己的视野做处理。此处暂时全部读了进来
        bufferState->gamemap = Proto2THUAI6::Protobuf2THUAI6Map(message.map_message());
        if (playerType == THUAI6::PlayerType::HumanPlayer)
        {
            for (const auto& item : message.human_message())
            {
                if (item.player_id() == playerID)
                {
                    bufferState->humanSelf = Proto2THUAI6::Protobuf2THUAI6Human(item);
                }
                bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(item));
            }
            for (const auto& item : message.butcher_message())
            {
                int vr = this->bufferState->humanSelf->viewRange;
                int deltaX = item.x() - this->bufferState->humanSelf->x;
                int deltaY = item.y() - this->bufferState->humanSelf->y;
                double distance = deltaX * deltaX + deltaY * deltaY;
                if (distance > vr * vr)
                    continue;
                else
                {
                    int divide = abs(deltaX) > abs(deltaY) ? abs(deltaX) : abs(deltaY);
                    divide /= 100;
                    double dx = deltaX / divide;
                    double dy = deltaY / divide;
                    double myX = this->bufferState->humanSelf->x;
                    double myY = this->bufferState->humanSelf->y;
                    bool barrier = false;
                    for (int i = 0; i < divide; i++)
                    {
                        myX += dx;
                        myY += dy;
                        if (this->bufferState->gamemap[IAPI::GridToCell(myX)][IAPI::GridToCell(myY)] == THUAI6::PlaceType::Wall)
                        {
                            barrier = true;
                            break;
                        }
                    }
                    if (barrier)
                        continue;
                    bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(item));
                    logger->debug("Add Butcher!");
                }
            }
        }
        else
        {
            for (const auto& item : message.butcher_message())
            {
                if (item.player_id() == playerID)
                {
                    bufferState->butcherSelf = Proto2THUAI6::Protobuf2THUAI6Butcher(item);
                }
                bufferState->butchers.push_back(Proto2THUAI6::Protobuf2THUAI6Butcher(item));
            }
            for (const auto& item : message.human_message())
            {
                int vr = this->bufferState->butcherSelf->viewRange;
                int deltaX = item.x() - this->bufferState->butcherSelf->x;
                int deltaY = item.y() - this->bufferState->butcherSelf->y;
                double distance = deltaX * deltaX + deltaY * deltaY;
                if (distance > vr * vr)
                    continue;
                else
                {
                    int divide = abs(deltaX) > abs(deltaY) ? abs(deltaX) : abs(deltaY);
                    divide /= 100;
                    double dx = deltaX / divide;
                    double dy = deltaY / divide;
                    double myX = this->bufferState->butcherSelf->x;
                    double myY = this->bufferState->butcherSelf->y;
                    bool barrier = false;
                    for (int i = 0; i < divide; i++)
                    {
                        myX += dx;
                        myY += dy;
                        if (this->bufferState->gamemap[IAPI::GridToCell(myX)][IAPI::GridToCell(myY)] == THUAI6::PlaceType::Wall)
                        {
                            barrier = true;
                            break;
                        }
                    }
                    if (barrier)
                        continue;
                    bufferState->humans.push_back(Proto2THUAI6::Protobuf2THUAI6Human(item));
                    logger->debug("Add Human!");
                }
            }
        }
        for (const auto& item : message.prop_message())
        {
            bufferState->props.push_back(Proto2THUAI6::Protobuf2THUAI6Prop(item));
            logger->debug("Add Prop!");
        }
        if (asynchronous)
        {
            {
                std::lock_guard<std::mutex> lock(mtxState);
                std::swap(currentState, bufferState);
                logger->info("Update State!");
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
        logger->info("Update State!");
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

const std::vector<int64_t> Logic::GetPlayerGUIDs() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    return currentState->guids;
}

bool Logic::TryConnection()
{
    logger->info("Try to connect to server...");
    bool result = pComm->TryConnection(playerID);
    return result;
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port, bool debug, bool level)
{
    // 建立日志组件
    if (debug)
    {
        auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/logic-log.txt", true);
        auto stdout_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        if (level)
            stdout_logger->set_level(spdlog::level::warn);
        else
            stdout_logger->set_level(spdlog::level::info);
        file_logger->set_level(spdlog::level::trace);
        logger = std::make_shared<spdlog::logger>("logicLogger", spdlog::sinks_init_list{file_logger, stdout_logger});
    }
    else
    {
        auto logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/logic-log.txt", true);
        logger->set_level(spdlog::level::trace);
    }

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

        while (AILoop)
        {
            if (asynchronous)
            {
                Wait();
                timer->StartTimer();
                timer->Play(*ai);
                timer->EndTimer();
            }
            else
            {
                Update();
                timer->StartTimer();
                timer->Play(*ai);
                timer->EndTimer();
            }
        }
    };

    tAI = std::thread(AIThread);

    // 连接服务器
    if (TryConnection())
    {
        logger->info("Connect to the server successfully, AI thread will be start.");
        if (tAI.joinable())
        {
            logger->info("Join the AI thread!");

            // 首先开启处理消息的线程
            ProcessMessage();
            tAI.join();
        }
    }
    else
    {
        logger->error("Connect to the server failed, AI thread will not be start.");
        return;
    }
}
