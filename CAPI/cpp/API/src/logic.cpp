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
    return currentState->gameMap;
}

THUAI6::PlaceType Logic::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetPlaceType");
    return currentState->gameMap[CellX][CellY];
}

bool Logic::IsDoorOpen(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called IsDoorOpen");
    auto pos = std::make_pair(CellX, CellY);
    if (currentState->mapInfo->doorState.count(pos) == 0)
        return false;
    else
        return currentState->mapInfo->doorState[pos];
}

int32_t Logic::GetClassroomProgress(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetClassroomProgress");
    auto pos = std::make_pair(CellX, CellY);
    if (currentState->mapInfo->classRoomState.count(pos) == 0)
        return 0;
    else
        return currentState->mapInfo->classRoomState[pos];
}

int32_t Logic::GetChestProgress(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetChestProgress");
    auto pos = std::make_pair(CellX, CellY);
    if (currentState->mapInfo->chestState.count(pos) == 0)
        return 0;
    else
        return currentState->mapInfo->chestState[pos];
}

int32_t Logic::GetGateProgress(int32_t CellX, int32_t CellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetGateProgress");
    auto pos = std::make_pair(CellX, CellY);
    if (currentState->mapInfo->gateState.count(pos) == 0)
        return 0;
    else
        return currentState->mapInfo->gateState[pos];
}

std::shared_ptr<const THUAI6::GameInfo> Logic::GetGameInfo() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetGameInfo");
    return currentState->gameInfo;
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

bool Logic::UseProp(THUAI6::PropType prop)
{
    logger->debug("Called UseProp");
    return pComm->UseProp(prop, playerID);
}

bool Logic::UseSkill(int32_t skill)
{
    logger->debug("Called UseSkill");
    return pComm->UseSkill(skill, playerID);
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

bool Logic::StartTreatMate()
{
    logger->debug("Called StartTreatMate");
    return pComm->StartTreatMate(playerID);
}

bool Logic::StartRescueMate()
{
    logger->debug("Called StartRescueMate");
    return pComm->StartRescueMate(playerID);
}

bool Logic::Attack(double angle)
{
    logger->debug("Called Attack");
    return pComm->Attack(angle, playerID);
}

bool Logic::OpenDoor()
{
    logger->debug("Called OpenDoor");
    return pComm->OpenDoor(playerID);
}

bool Logic::CloseDoor()
{
    logger->debug("Called CloseDoor");
    return pComm->CloseDoor(playerID);
}

bool Logic::SkipWindow()
{
    logger->debug("Called SkipWindow");
    return pComm->SkipWindow(playerID);
}

bool Logic::StartOpenGate()
{
    logger->debug("Called StartOpenGate");
    return pComm->StartOpenGate(playerID);
}

bool Logic::StartOpenChest()
{
    logger->debug("Called StartOpenChest");
    return pComm->StartOpenChest(playerID);
}

bool Logic::EndAllAction()
{
    logger->debug("Called EndAllAction");
    return pComm->EndAllAction(playerID);
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
        currentState->gameMap = pComm->GetMap(playerID);
        bufferState->gameMap = currentState->gameMap;
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

                    // 重新读取玩家的guid，保证人类在前屠夫在后
                    playerGUIDs.clear();
                    for (const auto& obj : clientMsg.obj_message())
                        if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
                            playerGUIDs.push_back(obj.student_message().guid());
                    for (const auto& obj : clientMsg.obj_message())
                        if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
                            playerGUIDs.push_back(obj.tricker_message().guid());
                    currentState->guids = playerGUIDs;
                    bufferState->guids = playerGUIDs;

                    LoadBuffer(clientMsg);

                    AILoop = true;
                    UnBlockAI();

                    break;
                case THUAI6::GameState::GameRunning:
                    // 重新读取玩家的guid，guid确保人类在前屠夫在后
                    playerGUIDs.clear();
                    for (const auto& obj : clientMsg.obj_message())
                        if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
                            playerGUIDs.push_back(obj.student_message().guid());
                    for (const auto& obj : clientMsg.obj_message())
                        if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
                            playerGUIDs.push_back(obj.tricker_message().guid());
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
        bufferState->gameInfo = Proto2THUAI6::Protobuf2THUAI6GameInfo(message.all_message());
        if (playerType == THUAI6::PlayerType::StudentPlayer)
        {
            for (const auto& item : message.obj_message())
                if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
                {
                    if (item.student_message().player_id() == playerID)
                    {
                        bufferState->studentSelf = Proto2THUAI6::Protobuf2THUAI6Student(item.student_message());
                    }
                    bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item.student_message()));
                    logger->debug("Add Student!");
                }
            for (const auto& item : message.obj_message())
                if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
                    if (AssistFunction::HaveView(bufferState->studentSelf->viewRange, bufferState->studentSelf->x, bufferState->studentSelf->y, item.tricker_message().x(), item.tricker_message().y(), bufferState->studentSelf->place, Proto2THUAI6::placeTypeDict[item.tricker_message().place()], bufferState->gameMap))
                    {
                        bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message()));
                        logger->debug("Add Tricker!");
                    }
        }
        else
        {
            for (const auto& item : message.obj_message())
            {
                if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
                {
                    if (item.tricker_message().player_id() == playerID)
                    {
                        bufferState->trickerSelf = Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message());
                    }
                    bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message()));
                    logger->debug("Add Tricker!");
                }
            }
            for (const auto& item : message.obj_message())
                if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
                    if (AssistFunction::HaveView(bufferState->trickerSelf->viewRange, bufferState->trickerSelf->x, bufferState->trickerSelf->y, item.student_message().x(), item.student_message().y(), bufferState->trickerSelf->place, Proto2THUAI6::placeTypeDict[item.student_message().place()], bufferState->gameMap))
                    {
                        bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item.student_message()));
                        logger->debug("Add Student!");
                    }
        }
        for (const auto& item : message.obj_message())
            switch (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()])
            {
                case THUAI6::MessageOfObj::PropMessage:
                    bufferState->props.push_back(Proto2THUAI6::Protobuf2THUAI6Prop(item.prop_message()));
                    logger->debug("Add Prop!");
                    break;
                case THUAI6::MessageOfObj::BulletMessage:
                    bufferState->bullets.push_back(Proto2THUAI6::Protobuf2THUAI6Bullet(item.bullet_message()));
                    logger->debug("Add Bullet!");
                    break;
                case THUAI6::MessageOfObj::BombedBulletMessage:
                    bufferState->bombedBullets.push_back(Proto2THUAI6::Protobuf2THUAI6BombedBullet(item.bombed_bullet_message()));
                    logger->debug("Add BombedBullet!");
                    break;
                case THUAI6::MessageOfObj::ClassroomMessage:
                    {
                        auto pos = std::make_pair(item.classroom_message().x(), item.classroom_message().y());
                        if (bufferState->mapInfo->classRoomState.count(pos) == 0)
                        {
                            bufferState->mapInfo->classRoomState.emplace(pos, item.classroom_message().progress());
                            logger->debug("Add Classroom!");
                        }
                        else
                        {
                            bufferState->mapInfo->classRoomState[pos] = item.classroom_message().progress();
                            logger->debug("Update Classroom!");
                        }
                        break;
                    }
                case THUAI6::MessageOfObj::ChestMessage:
                    {
                        auto pos = std::make_pair(item.chest_message().x(), item.chest_message().y());
                        if (bufferState->mapInfo->chestState.count(pos) == 0)
                        {
                            bufferState->mapInfo->chestState.emplace(pos, item.chest_message().progress());
                            logger->debug("Add Chest!");
                        }
                        else
                        {
                            bufferState->mapInfo->chestState[pos] = item.chest_message().progress();
                            logger->debug("Update Chest!");
                        }
                        break;
                    }
                case THUAI6::MessageOfObj::DoorMessage:
                    {
                        auto pos = std::make_pair(item.door_message().x(), item.door_message().y());
                        if (bufferState->mapInfo->doorState.count(pos) == 0)
                        {
                            bufferState->mapInfo->doorState.emplace(pos, item.door_message().is_open());
                            logger->debug("Add Door!");
                        }
                        else
                        {
                            bufferState->mapInfo->doorState[pos] = item.door_message().is_open();
                            logger->debug("Update Door!");
                        }
                        break;
                    }
                case THUAI6::MessageOfObj::GateMessage:
                    {
                        auto pos = std::make_pair(item.gate_message().x(), item.gate_message().y());
                        if (bufferState->mapInfo->gateState.count(pos) == 0)
                        {
                            bufferState->mapInfo->gateState.emplace(pos, item.gate_message().progress());
                            logger->debug("Add Gate!");
                        }
                        else
                        {
                            bufferState->mapInfo->gateState[pos] = item.gate_message().progress();
                            logger->debug("Update Gate!");
                        }
                        break;
                    }
                case THUAI6::MessageOfObj::NullMessageOfObj:
                default:
                    break;
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
