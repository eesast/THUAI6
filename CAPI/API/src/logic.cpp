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

Logic::Logic(THUAI6::PlayerType type, int64_t ID, THUAI6::TrickerType tricker, THUAI6::StudentType student) :
    playerType(type),
    playerID(ID),
    trickerType(tricker),
    studentType(student)
{
    currentState = &state[0];
    bufferState = &state[1];
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> Logic::GetTrickers() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Tricker>> temp;
    temp.assign(currentState->trickers.begin(), currentState->trickers.end());
    logger->debug("Called GetTrickers");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Student>> Logic::GetStudents() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Student>> temp;
    temp.assign(currentState->students.begin(), currentState->students.end());
    logger->debug("Called GetStudents");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> Logic::GetProps() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Prop>> temp;
    temp.assign(currentState->props.begin(), currentState->props.end());
    logger->debug("Called GetProps");
    return temp;
}

std::shared_ptr<const THUAI6::Student> Logic::StudentGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called StudentGetSelfInfo");
    return currentState->studentSelf;
}

std::shared_ptr<const THUAI6::Tricker> Logic::TrickerGetSelfInfo() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called TrickerGetSelfInfo");
    return currentState->trickerSelf;
}

std::vector<std::vector<THUAI6::PlaceType>> Logic::GetFullMap() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetFullMap");
    return currentState->gamemap;
}

THUAI6::PlaceType Logic::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
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

bool Logic::Graduate()
{
    logger->debug("Called Graduate");
    return pComm->Graduate(playerID);
}

bool Logic::StartLearning()
{
    logger->debug("Called StartLearning");
    return pComm->StartLearning(playerID);
}

bool Logic::StartHelpMate()
{
    logger->debug("Called StartHelpMate");
    return pComm->StartHelpMate(playerID);
}

bool Logic::StartHealMate()
{
    logger->debug("Called StartHealMate");
    return pComm->StartHealMate(playerID);
}

bool Logic::Trick(double angle)
{
    logger->debug("Called Trick");
    return pComm->Trick(angle, playerID);
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
        pComm->AddPlayer(playerID, playerType, studentType, trickerType);
        logger->info("Join the player!");
        pComm->ReadMessage(playerID);
        while (gameState != THUAI6::GameState::GameEnd)
        {
            auto clientMsg = pComm->GetMessage2Client();  // 在获得新消息之前阻塞
            logger->debug("Get message from server!");
            gameState = Proto2THUAI6::gameStateDict[clientMsg.game_state()];
            switch (gameState)
            {
                case THUAI6::GameState::GameStart:
                    logger->info("Game Start!");

                    // 重新读取玩家的guid，guid确保人类在前屠夫在后
                    playerGUIDs.clear();
                    for (auto student : clientMsg.student_message())
                        playerGUIDs.push_back(student.guid());
                    for (auto tricker : clientMsg.tricker_message())
                        playerGUIDs.push_back(tricker.guid());
                    currentState->guids = playerGUIDs;
                    bufferState->guids = playerGUIDs;

                    LoadBuffer(clientMsg);

                    AILoop = true;
                    UnBlockAI();

                    break;
                case THUAI6::GameState::GameRunning:
                    // 重新读取玩家的guid，guid确保人类在前屠夫在后
                    playerGUIDs.clear();
                    for (auto student : clientMsg.student_message())
                        playerGUIDs.push_back(student.guid());
                    for (auto tricker : clientMsg.tricker_message())
                        playerGUIDs.push_back(tricker.guid());
                    currentState->guids = playerGUIDs;
                    bufferState->guids = playerGUIDs;

                    LoadBuffer(clientMsg);
                    break;
                default:
                    logger->debug("Unknown GameState!");
                    break;
            }
        }
        AILoop = false;
        {
            std::lock_guard<std::mutex> lock(mtxBuffer);
            bufferUpdated = true;
            counterBuffer = -1;
        }
        cvBuffer.notify_one();
        logger->info("Game End!");
    };
    std::thread(messageThread).detach();
}

void Logic::LoadBuffer(protobuf::MessageToClient& message)
{
    // 将消息读入到buffer中
    {
        std::lock_guard<std::mutex> lock(mtxBuffer);

        // 清空原有信息
        bufferState->students.clear();
        bufferState->trickers.clear();
        bufferState->props.clear();

        logger->debug("Buffer cleared!");
        // 读取新的信息
        bufferState->gamemap = Proto2THUAI6::Protobuf2THUAI6Map(message.map_message());
        if (playerType == THUAI6::PlayerType::StudentPlayer)
        {
            for (const auto& item : message.student_message())
            {
                if (item.player_id() == playerID)
                {
                    bufferState->studentSelf = Proto2THUAI6::Protobuf2THUAI6Student(item);
                }
                bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item));
                logger->debug("Add Student!");
            }
            for (const auto& item : message.tricker_message())
            {
                if (AssistFunction::HaveView(bufferState->studentSelf->viewRange, bufferState->studentSelf->x, bufferState->studentSelf->y, item.x(), item.y(), bufferState->studentSelf->place, Proto2THUAI6::placeTypeDict[item.place()], bufferState->gamemap))
                {
                    bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item));
                    logger->debug("Add Tricker!");
                }
            }
        }
        else
        {
            for (const auto& item : message.tricker_message())
            {
                if (item.player_id() == playerID)
                {
                    bufferState->trickerSelf = Proto2THUAI6::Protobuf2THUAI6Tricker(item);
                }
                bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item));
                logger->debug("Add Tricker!");
            }
            for (const auto& item : message.student_message())
                if (AssistFunction::HaveView(bufferState->trickerSelf->viewRange, bufferState->trickerSelf->x, bufferState->trickerSelf->y, item.x(), item.y(), bufferState->trickerSelf->place, Proto2THUAI6::placeTypeDict[item.place()], bufferState->gamemap))
                {
                    bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item));
                    logger->debug("Add Student!");
                }
        }
        for (const auto& item : message.prop_message())
        {
            bufferState->props.push_back(Proto2THUAI6::Protobuf2THUAI6Prop(item));
            logger->debug("Add Prop!");
        }
        for (const auto& item : message.bullet_message())
        {
            bufferState->bullets.push_back(Proto2THUAI6::Protobuf2THUAI6Bullet(item));
            logger->debug("Add Bullet!");
        }
        for (const auto& item : message.bombed_bullet_message())
        {
            bufferState->bombedBullets.push_back(Proto2THUAI6::Protobuf2THUAI6BombedBullet(item));
            logger->debug("Add BombedBullet!");
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
    return pComm->TryConnection(playerID);
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port, bool file, bool print, bool warnOnly)
{
    // 建立日志组件
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/logic-log.txt", true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[logic] [%H:%M:%S.%e] [%l] %v";
    fileLogger->set_pattern(pattern);
    printLogger->set_pattern(pattern);
    if (file)
        fileLogger->set_level(spdlog::level::trace);
    else
        fileLogger->set_level(spdlog::level::off);
    if (print)
        printLogger->set_level(spdlog::level::info);
    else
        printLogger->set_level(spdlog::level::off);
    if (warnOnly)
        printLogger->set_level(spdlog::level::warn);
    logger = std::make_unique<spdlog::logger>("logicLogger", spdlog::sinks_init_list{fileLogger, printLogger});

    // 打印当前的调试信息
    logger->info("*********Basic Info*********");
    logger->info("asynchronous: {}", asynchronous);
    logger->info("server: {}:{}", IP, port);
    logger->info("player ID: {}", playerID);
    logger->info("player type: {}", THUAI6::playerTypeDict[playerType]);
    logger->info("****************************");

    // 建立与服务器之间通信的组件
    pComm = std::make_unique<Communication>(IP, port);

    // 构造timer
    if (playerType == THUAI6::PlayerType::StudentPlayer)
    {
        if (!file && !print)
            timer = std::make_unique<StudentAPI>(*this);
        else
            timer = std::make_unique<StudentDebugAPI>(*this, file, print, warnOnly, playerID);
    }
    else if (playerType == THUAI6::PlayerType::TrickerPlayer)
    {
        if (!file && !print)
            timer = std::make_unique<TrickerAPI>(*this);
        else
            timer = std::make_unique<TrickerDebugAPI>(*this, file, print, warnOnly, playerID);
    }

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

    // 连接服务器
    if (TryConnection())
    {
        logger->info("Connect to the server successfully, AI thread will be started.");
        tAI = std::thread(AIThread);
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
        AILoop = false;
        logger->error("Connect to the server failed, AI thread will not be started.");
        return;
    }
}
