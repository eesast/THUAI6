#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <functional>
#include "utils.hpp"
#include "Communication.h"

#undef GetMessage
#undef SendMessage
#undef PeekMessage

extern const bool asynchronous;

Logic::Logic(THUAI6::PlayerType type, int64_t ID, THUAI6::TrickerType tricker, THUAI6::StudentType student) :
    playerType(type),
    playerID(ID),
    trickerType(tricker),
    studentType(student)
{
    currentState = &state[0];
    bufferState = &state[1];
    currentState->gameInfo = std::make_shared<THUAI6::GameInfo>();
    currentState->mapInfo = std::make_shared<THUAI6::GameMap>();
    bufferState->gameInfo = std::make_shared<THUAI6::GameInfo>();
    bufferState->mapInfo = std::make_shared<THUAI6::GameMap>();
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> Logic::GetTrickers() const
{
    std::lock_guard<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Tricker>> temp(currentState->trickers.begin(), currentState->trickers.end());
    logger->debug("Called GetTrickers");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Student>> Logic::GetStudents() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Student>> temp(currentState->students.begin(), currentState->students.end());
    logger->debug("Called GetStudents");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> Logic::GetProps() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Prop>> temp(currentState->props.begin(), currentState->props.end());
    logger->debug("Called GetProps");
    return temp;
}

std::vector<std::shared_ptr<const THUAI6::Bullet>> Logic::GetBullets() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    std::vector<std::shared_ptr<const THUAI6::Bullet>> temp(currentState->bullets.begin(), currentState->bullets.end());
    logger->debug("Called GetBullets");
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

THUAI6::PlaceType Logic::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    if (cellX < 0 || uint64_t(cellX) >= currentState->gameMap.size() || cellY < 0 || uint64_t(cellY) >= currentState->gameMap[0].size())
    {
        logger->warn("Invalid position!");
        return THUAI6::PlaceType::NullPlaceType;
    }
    logger->debug("Called GetPlaceType");
    return currentState->gameMap[cellX][cellY];
}

bool Logic::IsDoorOpen(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called IsDoorOpen");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->doorState.count(pos) == 0)
    {
        logger->warn("Door not found");
        return false;
    }
    else
        return currentState->mapInfo->doorState[pos];
}

int32_t Logic::GetDoorProgress(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetDoorProgress");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->doorProgress.count(pos) == 0)
    {
        logger->warn("Door not found");
        return -1;
    }
    else
        return currentState->mapInfo->doorProgress[pos];
}

int32_t Logic::GetClassroomProgress(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetClassroomProgress");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->classRoomState.count(pos) == 0)
    {
        logger->warn("Classroom not found");
        return -1;
    }
    else
        return currentState->mapInfo->classRoomState[pos];
}

int32_t Logic::GetChestProgress(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetChestProgress");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->chestState.count(pos) == 0)
    {
        logger->warn("Chest not found");
        return -1;
    }
    else
        return currentState->mapInfo->chestState[pos];
}

int32_t Logic::GetGateProgress(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetGateProgress");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->gateState.count(pos) == 0)
    {
        logger->warn("Gate not found");
        return -1;
    }
    else
        return currentState->mapInfo->gateState[pos];
}

THUAI6::HiddenGateState Logic::GetHiddenGateState(int32_t cellX, int32_t cellY) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    logger->debug("Called GetHiddenGateState");
    auto pos = std::make_pair(cellX, cellY);
    if (currentState->mapInfo->hiddenGateState.count(pos) == 0)
    {
        logger->warn("HiddenGate not found");
        return THUAI6::HiddenGateState::Null;
    }
    else
        return currentState->mapInfo->hiddenGateState[pos];
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

bool Logic::ThrowProp(THUAI6::PropType prop)
{
    logger->debug("Called ThrowProp");
    return pComm->ThrowProp(prop, playerID);
}

bool Logic::UseSkill(int32_t skill, int32_t skillParam)
{
    logger->debug("Called UseSkill");
    return pComm->UseSkill(skill, skillParam, playerID);
}

bool Logic::SendMessage(int64_t toID, std::string message, bool binary)
{
    logger->debug("Called SendMessage");
    return pComm->SendMessage(toID, std::move(message), binary, playerID);
}

bool Logic::HaveMessage()
{
    logger->debug("Called HaveMessage");
    return !messageQueue.empty();
}

std::pair<int64_t, std::string> Logic::GetMessage()
{
    logger->debug("Called GetMessage");
    auto msg = messageQueue.tryPop();
    if (msg.has_value())
        return msg.value();
    else
    {
        logger->warn("No message");
        return std::make_pair(-1, "");
    }
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

bool Logic::StartEncourageMate(int64_t mateID)
{
    logger->debug("Called StartEncourageMate");
    return pComm->StartEncourageMate(playerID, mateID);
}

bool Logic::StartRouseMate(int64_t mateID)
{
    logger->debug("Called StartRouseMate");
    return pComm->StartRouseMate(playerID, mateID);
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
    auto messageThread = [this]()
    {
        try
        {
            logger->info("Message thread start!");
            pComm->AddPlayer(playerID, playerType, studentType, trickerType);
            while (gameState != THUAI6::GameState::GameEnd)
            {
                auto clientMsg = pComm->GetMessage2Client();  // 在获得新消息之前阻塞
                logger->debug("Get message from server!");
                gameState = Proto2THUAI6::gameStateDict[clientMsg.game_state()];
                switch (gameState)
                {
                    case THUAI6::GameState::GameStart:
                        logger->info("Game Start!");

                        // 读取地图
                        for (const auto& item : clientMsg.obj_message())
                            if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::MapMessage)
                            {
                                auto map = std::vector<std::vector<THUAI6::PlaceType>>();
                                auto mapResult = item.map_message();
                                for (int32_t i = 0; i < item.map_message().row_size(); i++)
                                {
                                    std::vector<THUAI6::PlaceType> row;
                                    for (int32_t j = 0; j < mapResult.row(i).col_size(); j++)
                                    {
                                        if (Proto2THUAI6::placeTypeDict.count(mapResult.row(i).col(j)) == 0)
                                            logger->error("Unknown place type!");
                                        row.push_back(Proto2THUAI6::placeTypeDict[mapResult.row(i).col(j)]);
                                    }
                                    map.push_back(std::move(row));
                                }
                                bufferState->gameMap = std::move(map);
                                currentState->gameMap = bufferState->gameMap;
                                logger->info("Map loaded!");
                                break;
                            }
                        if (currentState->gameMap.empty())
                        {
                            logger->error("Map not loaded!");
                            throw std::runtime_error("Map not loaded!");
                        }
                        LoadBuffer(clientMsg);

                        AILoop = true;
                        UnBlockAI();

                        break;
                    case THUAI6::GameState::GameRunning:

                        LoadBuffer(clientMsg);
                        break;
                    default:
                        logger->debug("Unknown GameState!");
                        break;
                }
            }
            {
                std::lock_guard<std::mutex> lock(mtxBuffer);
                bufferUpdated = true;
                counterBuffer = -1;
            }
            cvBuffer.notify_one();
            logger->info("Game End!");
            AILoop = false;
        }
        catch (const std::exception& e)
        {
            std::cerr << "C++ Exception: " << e.what() << std::endl;
            AILoop = false;
        }
        catch (...)
        {
            std::cerr << "Unknown Exception!" << std::endl;
            AILoop = false;
        }
    };
    std::thread(messageThread).detach();
}

void Logic::LoadBufferSelf(const protobuf::MessageToClient& message)
{
    if (playerType == THUAI6::PlayerType::StudentPlayer)
    {
        for (const auto& item : message.obj_message())
            if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
            {
                if (item.student_message().player_id() == playerID)
                {
                    bufferState->studentSelf = Proto2THUAI6::Protobuf2THUAI6Student(item.student_message());
                    bufferState->students.push_back(bufferState->studentSelf);
                }
                else
                    bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item.student_message()));
                logger->debug("Add Student!");
            }
    }
    else
    {
        for (const auto& item : message.obj_message())
            if (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
            {
                if (item.tricker_message().player_id() == playerID)
                {
                    bufferState->trickerSelf = Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message());
                    bufferState->trickers.push_back(bufferState->trickerSelf);
                }
                else
                    bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message()));
                logger->debug("Add Tricker!");
            }
    }
}

void Logic::LoadBufferCase(const protobuf::MessageOfObj& item)
{
    int32_t x, y, viewRange;
    if (playerType == THUAI6::PlayerType::StudentPlayer)
        x = bufferState->studentSelf->x, y = bufferState->studentSelf->y, viewRange = bufferState->studentSelf->viewRange;
    else
        x = bufferState->trickerSelf->x, y = bufferState->trickerSelf->y, viewRange = bufferState->trickerSelf->viewRange;

    if (playerType == THUAI6::PlayerType::StudentPlayer && Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
    {
        for (int32_t i = 0; i < item.tricker_message().buff_size(); i++)
            if (Proto2THUAI6::trickerBuffTypeDict[item.tricker_message().buff(i)] == THUAI6::TrickerBuffType::Invisible)
                return;
        if (AssistFunction::HaveView(viewRange, x, y, item.tricker_message().x(), item.tricker_message().y(), bufferState->gameMap))
        {
            bufferState->trickers.push_back(Proto2THUAI6::Protobuf2THUAI6Tricker(item.tricker_message()));
            logger->debug("Add Tricker!");
        }
        return;
    }
    else if (playerType == THUAI6::PlayerType::TrickerPlayer && Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
    {
        for (const auto& buff : bufferState->trickerSelf->buff)
            if (buff == THUAI6::TrickerBuffType::Clairaudience)
            {
                bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item.student_message()));
                logger->debug("Add Student!");
                return;
            }
        for (int32_t i = 0; i < item.student_message().buff_size(); i++)
            if (Proto2THUAI6::studentBuffTypeDict[item.student_message().buff(i)] == THUAI6::StudentBuffType::Invisible)
                return;
        if (AssistFunction::HaveView(viewRange, x, y, item.student_message().x(), item.student_message().y(), bufferState->gameMap))
        {
            bufferState->students.push_back(Proto2THUAI6::Protobuf2THUAI6Student(item.student_message()));
            logger->debug("Add Student!");
        }
        return;
    }

    switch (Proto2THUAI6::messageOfObjDict[item.message_of_obj_case()])
    {
        case THUAI6::MessageOfObj::PropMessage:
            if (AssistFunction::HaveView(viewRange, x, y, item.prop_message().x(), item.prop_message().y(), bufferState->gameMap))
            {
                bufferState->props.push_back(Proto2THUAI6::Protobuf2THUAI6Prop(item.prop_message()));
                logger->debug("Add Prop!");
            }
            break;
        case THUAI6::MessageOfObj::BulletMessage:
            if (AssistFunction::HaveView(viewRange, x, y, item.prop_message().x(), item.prop_message().y(), bufferState->gameMap))
            {
                bufferState->bullets.push_back(Proto2THUAI6::Protobuf2THUAI6Bullet(item.bullet_message()));
                logger->debug("Add Bullet!");
            }
            break;
        case THUAI6::MessageOfObj::ClassroomMessage:
            {
                if (AssistFunction::HaveView(viewRange, x, y, item.classroom_message().x(), item.classroom_message().y(), bufferState->gameMap))
                {
                    auto pos = std::make_pair(AssistFunction::GridToCell(item.classroom_message().x()), AssistFunction::GridToCell(item.classroom_message().y()));
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
                }
                break;
            }
        case THUAI6::MessageOfObj::ChestMessage:
            {
                if (AssistFunction::HaveView(viewRange, x, y, item.chest_message().x(), item.chest_message().y(), bufferState->gameMap))
                {
                    auto pos = std::make_pair(AssistFunction::GridToCell(item.chest_message().x()), AssistFunction::GridToCell(item.chest_message().y()));
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
                }
                break;
            }
        case THUAI6::MessageOfObj::DoorMessage:
            {
                if (AssistFunction::HaveView(viewRange, x, y, item.door_message().x(), item.door_message().y(), bufferState->gameMap))
                {
                    auto pos = std::make_pair(AssistFunction::GridToCell(item.door_message().x()), AssistFunction::GridToCell(item.door_message().y()));
                    if (bufferState->mapInfo->doorState.count(pos) == 0)
                    {
                        bufferState->mapInfo->doorState.emplace(pos, item.door_message().is_open());
                        bufferState->mapInfo->doorProgress.emplace(pos, item.door_message().progress());
                        logger->debug("Add Door!");
                    }
                    else
                    {
                        bufferState->mapInfo->doorState[pos] = item.door_message().is_open();
                        bufferState->mapInfo->doorProgress[pos] = item.door_message().progress();
                        logger->debug("Update Door!");
                    }
                }
                break;
            }
        case THUAI6::MessageOfObj::HiddenGateMessage:
            {
                if (AssistFunction::HaveView(viewRange, x, y, item.hidden_gate_message().x(), item.hidden_gate_message().y(), bufferState->gameMap))
                {
                    auto pos = std::make_pair(AssistFunction::GridToCell(item.hidden_gate_message().x()), AssistFunction::GridToCell(item.hidden_gate_message().y()));
                    if (bufferState->mapInfo->hiddenGateState.count(pos) == 0)
                    {
                        bufferState->mapInfo->hiddenGateState.emplace(pos, Proto2THUAI6::Bool2HiddenGateState(item.hidden_gate_message().opened()));
                        logger->debug("Add HiddenGate!");
                    }
                    else
                    {
                        bufferState->mapInfo->hiddenGateState[pos] = Proto2THUAI6::Bool2HiddenGateState(item.hidden_gate_message().opened());
                        logger->debug("Update HiddenGate!");
                    }
                }
                break;
            }
        case THUAI6::MessageOfObj::GateMessage:
            {
                if (AssistFunction::HaveView(viewRange, x, y, item.gate_message().x(), item.gate_message().y(), bufferState->gameMap))
                {
                    auto pos = std::make_pair(AssistFunction::GridToCell(item.gate_message().x()), AssistFunction::GridToCell(item.gate_message().y()));
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
                }
                break;
            }
        case THUAI6::MessageOfObj::NewsMessage:
            {
                auto news = item.news_message();
                if (news.to_id() == playerID)
                {
                    if (Proto2THUAI6::newsTypeDict[news.news_case()] == THUAI6::NewsType::TextMessage)
                    {
                        messageQueue.emplace(std::make_pair(news.from_id(), news.text_message()));
                        logger->debug("Add News!");
                    }
                    else if (Proto2THUAI6::newsTypeDict[news.news_case()] == THUAI6::NewsType::BinaryMessage)
                    {
                        messageQueue.emplace(std::make_pair(news.from_id(), news.binary_message()));
                        logger->debug("Add Binary News!");
                    }
                    else
                        logger->error("Unknown NewsType!");
                }
                break;
            }
        case THUAI6::MessageOfObj::NullMessageOfObj:
        default:
            break;
    }
}

void Logic::LoadBuffer(const protobuf::MessageToClient& message)
{
    // 将消息读入到buffer中
    {
        std::lock_guard<std::mutex> lock(mtxBuffer);

        // 清空原有信息
        bufferState->students.clear();
        bufferState->trickers.clear();
        bufferState->props.clear();
        bufferState->bullets.clear();
        bufferState->bombedBullets.clear();
        bufferState->guids.clear();

        logger->debug("Buffer cleared!");
        // 读取新的信息
        for (const auto& obj : message.obj_message())
            if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::StudentMessage)
                bufferState->guids.push_back(obj.student_message().guid());
        for (const auto& obj : message.obj_message())
            if (Proto2THUAI6::messageOfObjDict[obj.message_of_obj_case()] == THUAI6::MessageOfObj::TrickerMessage)
                bufferState->guids.push_back(obj.tricker_message().guid());
        bufferState->gameInfo = Proto2THUAI6::Protobuf2THUAI6GameInfo(message.all_message());
        LoadBufferSelf(message);
        for (const auto& item : message.obj_message())
            LoadBufferCase(item);
    }
    if (asynchronous)
    {
        {
            std::lock_guard<std::mutex> lock(mtxState);
            std::swap(currentState, bufferState);
            counterState = counterBuffer;
            logger->info("Update State!");
        }
        freshed = true;
    }
    else
        bufferUpdated = true;
    counterBuffer++;
    // 唤醒其他线程
    cvBuffer.notify_one();
}

void Logic::Update() noexcept
{
    if (!asynchronous)
    {
        std::unique_lock<std::mutex> lock(mtxBuffer);

        // 缓冲区被更新之后才可以使用
        cvBuffer.wait(lock, [this]()
                      { return bufferUpdated; });
        {
            std::lock_guard<std::mutex> stateLock(mtxState);
            std::swap(currentState, bufferState);
            counterState = counterBuffer;
        }
        bufferUpdated = false;

        logger->info("Update State!");
    }
}

void Logic::Wait() noexcept
{
    freshed = false;
    {
        std::unique_lock<std::mutex> lock(mtxBuffer);
        cvBuffer.wait(lock, [this]()
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

int32_t Logic::GetCounter() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    return counterState;
}

std::vector<int64_t> Logic::GetPlayerGUIDs() const
{
    std::unique_lock<std::mutex> lock(mtxState);
    return currentState->guids;
}

bool Logic::TryConnection()
{
    logger->info("Try to connect to server...");
    return pComm->TryConnection(playerID);
}

bool Logic::HaveView(int32_t gridX, int32_t gridY, int32_t selfX, int32_t selfY, int32_t viewRange) const
{
    std::unique_lock<std::mutex> lock(mtxState);
    return AssistFunction::HaveView(viewRange, selfX, selfY, gridX, gridY, currentState->gameMap);
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port, bool file, bool print, bool warnOnly)
{
    // 建立日志组件
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fmt::format("logs/logic-{}-log.txt", playerID), true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[logic] [%H:%M:%S.%e] [%l] %v";
    fileLogger->set_pattern(pattern);
    printLogger->set_pattern(pattern);
    if (file)
        fileLogger->set_level(spdlog::level::debug);
    else
        fileLogger->set_level(spdlog::level::off);
    if (print)
        printLogger->set_level(spdlog::level::info);
    else
        printLogger->set_level(spdlog::level::off);
    if (warnOnly)
        printLogger->set_level(spdlog::level::warn);
    logger = std::make_unique<spdlog::logger>("logicLogger", spdlog::sinks_init_list{fileLogger, printLogger});
    logger->flush_on(spdlog::level::warn);

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
        try
        {
            {
                std::unique_lock<std::mutex> lock(mtxAI);
                cvAI.wait(lock, [this]()
                          { return AIStart; });
            }
            auto ai = createAI(playerID);

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
        }
        catch (const std::exception& e)
        {
            std::cerr << "C++ Exception: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Unknown Exception!" << std::endl;
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
