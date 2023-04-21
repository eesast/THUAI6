#include <optional>
#include <string>
#include "AI.h"
#include "API.h"
#include "utils.hpp"
#include "structures.h"
#define PI 3.14159265358979323846

StudentDebugAPI::StudentDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
    logic(logic)
{
    std::string fileName = "logs/api-" + std::to_string(playerID) + "-log.txt";
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[api " + std::to_string(playerID) + "] [%H:%M:%S.%e] [%l] %v";
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
    logger = std::make_unique<spdlog::logger>("apiLogger", spdlog::sinks_init_list{fileLogger, printLogger});
    logger->flush_on(spdlog::level::warn);
}

TrickerDebugAPI::TrickerDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
    logic(logic)
{
    std::string fileName = "logs/api-" + std::to_string(playerID) + "-log.txt";
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[api" + std::to_string(playerID) + "] [%H:%M:%S.%e] [%l] %v";
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
    logger = std::make_unique<spdlog::logger>("apiLogger", spdlog::sinks_init_list{fileLogger, printLogger});
}

void StudentDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void TrickerDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void StudentDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

void TrickerDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

int StudentDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

int TrickerDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

std::future<bool> StudentDebugAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    logger->info("Move: timeInMilliseconds = {}, angleInRadian = {}, called at {}ms", timeInMilliseconds, angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Move(timeInMilliseconds, angleInRadian);
                        if (!result)
                            logger->warn("Move: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> StudentDebugAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> StudentDebugAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> StudentDebugAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> TrickerDebugAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    logger->info("Move: timeInMilliseconds = {}, angleInRadian = {}, called at {}ms", timeInMilliseconds, angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Move(timeInMilliseconds, angleInRadian);
                        if (!result)
                            logger->warn("Move: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> TrickerDebugAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> TrickerDebugAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> TrickerDebugAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> StudentDebugAPI::PickProp(THUAI6::PropType prop)
{
    logger->info("PickProp: prop = {}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.PickProp(prop);
                        if (!result)
                           logger->warn("PickProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::UseProp(THUAI6::PropType prop)
{
    logger->info("UseProp: prop={}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseProp(prop);
                        if (!result)
                            logger->warn("UseProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::PickProp(THUAI6::PropType prop)
{
    logger->info("PickProp: prop = {}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.PickProp(prop);
                        if (!result)
                            logger->warn("PickProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::UseProp(THUAI6::PropType prop)
{
    logger->info("UseProp: prop={}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseProp(prop);
                        if (!result)
                            logger->warn("UseProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::ThrowProp(THUAI6::PropType prop)
{
    logger->info("ThrowProp: prop={}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.ThrowProp(prop);
                        if (!result)
                            logger->warn("ThrowProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::ThrowProp(THUAI6::PropType prop)
{
    logger->info("ThrowProp: prop={}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.ThrowProp(prop);
                        if (!result)
                            logger->warn("ThrowProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::UseSkill(int32_t skillID)
{
    logger->info("UseSkill: skillID={}, called at {}ms", skillID, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseSkill(skillID);
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::UseSkill(int32_t skillID)
{
    logger->info("UseSkill: skillID={}, called at {}ms", skillID, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseSkill(skillID);
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::OpenDoor()
{
    logger->info("OpenDoor: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.OpenDoor();
                        if (!result)
                            logger->warn("OpenDoor: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::OpenDoor()
{
    logger->info("OpenDoor: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.OpenDoor();
                        if (!result)
                            logger->warn("OpenDoor: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::CloseDoor()
{
    logger->info("CloseDoor: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.CloseDoor();
                        if (!result)
                            logger->warn("CloseDoor: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::CloseDoor()
{
    logger->info("CloseDoor: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.CloseDoor();
                        if (!result)
                            logger->warn("CloseDoor: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::SkipWindow()
{
    logger->info("SkipWindow: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.SkipWindow();
                        if (!result)
                            logger->warn("SkipWindow: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::SkipWindow()
{
    logger->info("SkipWindow: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.SkipWindow();
                        if (!result)
                            logger->warn("SkipWindow: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::StartOpenGate()
{
    logger->info("StartOpenGate: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartOpenGate();
                        if (!result)
                            logger->warn("StartOpenGate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::StartOpenGate()
{
    logger->info("StartOpenGate: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartOpenGate();
                        if (!result)
                            logger->warn("StartOpenGate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::StartOpenChest()
{
    logger->info("StartOpenChest: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartOpenChest();
                        if (!result)
                            logger->warn("StartOpenChest: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::StartOpenChest()
{
    logger->info("StartOpenChest: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartOpenChest();
                        if (!result)
                            logger->warn("StartOpenChest: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::EndAllAction()
{
    logger->info("EndAllAction: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndAllAction();
                        if (!result)
                            logger->warn("EndAllAction: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::EndAllAction()
{
    logger->info("EndAllAction: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndAllAction();
                        if (!result)
                            logger->warn("EndAllAction: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::SendMessage(int64_t toID, std::string message)
{
    logger->info("SendMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.SendMessage(toID, message);
                        if (!result)
                            logger->warn("SendMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::SendMessage(int64_t toID, std::string message)
{
    logger->info("SendMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.SendMessage(toID, message);
                        if (!result)
                            logger->warn("SendMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

bool StudentDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.HaveMessage();
    if (!result)
        logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

bool TrickerDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.HaveMessage();
    if (!result)
        logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

std::pair<int64_t, std::string> StudentDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.GetMessage();
    if (result.first == -1)
        logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

std::pair<int64_t, std::string> TrickerDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.GetMessage();
    if (result.first == -1)
        logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

bool StudentDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}

bool TrickerDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> StudentDebugAPI::GetTrickers() const
{
    return logic.GetTrickers();
}

std::vector<std::shared_ptr<const THUAI6::Student>> StudentDebugAPI::GetStudents() const
{
    return logic.GetStudents();
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> TrickerDebugAPI::GetTrickers() const
{
    return logic.GetTrickers();
}

std::vector<std::shared_ptr<const THUAI6::Student>> TrickerDebugAPI::GetStudents() const
{
    return logic.GetStudents();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> StudentDebugAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> TrickerDebugAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Bullet>> StudentDebugAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::shared_ptr<const THUAI6::Bullet>> TrickerDebugAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::vector<THUAI6::PlaceType>> StudentDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI6::PlaceType StudentDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

THUAI6::PlaceType TrickerDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

std::vector<std::vector<THUAI6::PlaceType>> TrickerDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

bool StudentDebugAPI::IsDoorOpen(int32_t cellX, int32_t cellY) const
{
    return logic.IsDoorOpen(cellX, cellY);
}

bool TrickerDebugAPI::IsDoorOpen(int32_t cellX, int32_t cellY) const
{
    return logic.IsDoorOpen(cellX, cellY);
}

int32_t StudentDebugAPI::GetClassroomProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetClassroomProgress(cellX, cellY);
}

int32_t TrickerDebugAPI::GetClassroomProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetClassroomProgress(cellX, cellY);
}

int32_t StudentDebugAPI::GetChestProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetChestProgress(cellX, cellY);
}

int32_t TrickerDebugAPI::GetChestProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetChestProgress(cellX, cellY);
}

int32_t StudentDebugAPI::GetDoorProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetDoorProgress(cellX, cellY);
}

int32_t TrickerDebugAPI::GetDoorProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetDoorProgress(cellX, cellY);
}

THUAI6::HiddenGateState StudentDebugAPI::GetHiddenGateState(int32_t cellX, int32_t cellY) const
{
    return logic.GetHiddenGateState(cellX, cellY);
}

THUAI6::HiddenGateState TrickerDebugAPI::GetHiddenGateState(int32_t cellX, int32_t cellY) const
{
    return logic.GetHiddenGateState(cellX, cellY);
}

int32_t StudentDebugAPI::GetGateProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetGateProgress(cellX, cellY);
}

int32_t TrickerDebugAPI::GetGateProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetGateProgress(cellX, cellY);
}

std::shared_ptr<const THUAI6::GameInfo> StudentDebugAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::shared_ptr<const THUAI6::GameInfo> TrickerDebugAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::vector<int64_t> StudentDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::vector<int64_t> TrickerDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::future<bool> StudentDebugAPI::StartLearning()
{
    logger->info("StartLearning: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartLearning();
                        if (!result)
                            logger->warn("StartLearning: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::StartRouseMate(int64_t mateID)
{
    logger->info("StartRouseMate: mate id={}, called at {}ms", mateID, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.StartRouseMate(mateID);
                        if (!result)
                            logger->warn("StartRouseMate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::StartEncourageMate(int64_t mateID)
{
    logger->info("StartEncourageMate: mate id={}, called at {}ms", mateID, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.StartEncourageMate(mateID);
                        if (!result)
                            logger->warn("StartEncourageMate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::Graduate()
{
    logger->info("Graduate: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.Graduate();
                        if (!result)
                            logger->warn("Graduate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::shared_ptr<const THUAI6::Student> StudentDebugAPI::GetSelfInfo() const
{
    return logic.StudentGetSelfInfo();
}

std::future<bool> TrickerDebugAPI::Attack(double angleInRadian)
{
    logger->info("Attack: angleInRadian = {}, called at {}ms", angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Attack(angleInRadian);
                        if (!result)
                            logger->warn("Attack: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::Attack(double angleInRadian)
{
    logger->info("Attack: angleInRadian = {}, called at {}ms", angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Attack(angleInRadian);
                        if (!result)
                            logger->warn("Attack: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::shared_ptr<const THUAI6::Tricker> TrickerDebugAPI::GetSelfInfo() const
{
    return logic.TrickerGetSelfInfo();
}

void StudentDebugAPI::Print(std::string str) const
{
    logger->info(str);
}

void TrickerDebugAPI::Print(std::string str) const
{
    logger->info(str);
}

void StudentDebugAPI::PrintStudent() const
{
    for (const auto& student : logic.GetStudents())
    {
        logger->info("******Student Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", student->playerID, student->guid, student->x, student->y);
        logger->info("speed={}, view range={}, place={}, radius={}", student->speed, student->viewRange, THUAI6::placeTypeDict[student->place], student->radius);
        std::string skillTime = "";
        for (const auto& time : student->timeUntilSkillAvailable)
            skillTime += std::to_string(time) + ", ";
        logger->info("score={}, facing direction={}, skill time={}", student->score, student->facingDirection, skillTime);
        std::string props = "";
        for (const auto& prop : student->props)
            props += THUAI6::propTypeDict[prop] + ", ";
        logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[student->playerState], THUAI6::bulletTypeDict[student->bulletType], props);
        logger->info("type={}, determination={}, addiction={}, danger alert={}", THUAI6::studentTypeDict[student->studentType], student->determination, student->addiction, student->dangerAlert);
        logger->info("learning speed={}, encourage speed={}, encourage progress={}, rouse progress={}", student->learningSpeed, student->encourageSpeed, student->encourageProgress, student->rouseProgress);
        std::string studentBuff = "";
        for (const auto& buff : student->buff)
            studentBuff += THUAI6::studentBuffDict[buff] + ", ";
        logger->info("buff={}", studentBuff);
        logger->info("************************\n");
    }
}

void TrickerDebugAPI::PrintStudent() const
{
    for (const auto& student : logic.GetStudents())
    {
        logger->info("******Student Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", student->playerID, student->guid, student->x, student->y);
        logger->info("speed={}, view range={}, place={}, radius={}", student->speed, student->viewRange, THUAI6::placeTypeDict[student->place], student->radius);
        std::string skillTime = "";
        for (const auto& time : student->timeUntilSkillAvailable)
            skillTime += std::to_string(time) + ", ";
        logger->info("score={}, facing direction={}, skill time={}", student->score, student->facingDirection, skillTime);
        std::string props = "";
        for (const auto& prop : student->props)
            props += THUAI6::propTypeDict[prop] + ", ";
        logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[student->playerState], THUAI6::bulletTypeDict[student->bulletType], props);
        logger->info("type={}, determination={}, addiction={}, danger alert={}", THUAI6::studentTypeDict[student->studentType], student->determination, student->addiction, student->dangerAlert);
        logger->info("learning speed={}, encourage speed={}, encourage progress={}, rouse progress={}", student->learningSpeed, student->encourageSpeed, student->encourageProgress, student->rouseProgress);
        std::string studentBuff = "";
        for (const auto& buff : student->buff)
            studentBuff += THUAI6::studentBuffDict[buff] + ", ";
        logger->info("buff={}", studentBuff);
        logger->info("************************\n");
    }
}

void StudentDebugAPI::PrintTricker() const
{
    for (const auto& tricker : logic.GetTrickers())
    {
        logger->info("******Tricker Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", tricker->playerID, tricker->guid, tricker->x, tricker->y);
        logger->info("speed={}, view range={}, place={}, radius={}", tricker->speed, tricker->viewRange, THUAI6::placeTypeDict[tricker->place], tricker->radius);
        std::string skillTime = "";
        for (const auto& time : tricker->timeUntilSkillAvailable)
            skillTime += std::to_string(time) + ", ";
        logger->info("score={}, facing direction={}, skill time={}", tricker->score, tricker->facingDirection, skillTime);
        std::string props = "";
        for (const auto& prop : tricker->props)
            props += THUAI6::propTypeDict[prop] + ", ";
        logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[tricker->playerState], THUAI6::bulletTypeDict[tricker->bulletType], props);
        logger->info("type={}, trick desire={}, class volume={}", THUAI6::trickerTypeDict[tricker->trickerType], tricker->trickDesire, tricker->classVolume);
        std::string trickerBuff = "";
        for (const auto& buff : tricker->buff)
            trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
        logger->info("buff={}", trickerBuff);
        logger->info("************************\n");
    }
}

void TrickerDebugAPI::PrintTricker() const
{
    for (auto tricker : logic.GetTrickers())
    {
        logger->info("******Tricker Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", tricker->playerID, tricker->guid, tricker->x, tricker->y);
        logger->info("speed={}, view range={}, place={}, radius={}", tricker->speed, tricker->viewRange, THUAI6::placeTypeDict[tricker->place], tricker->radius);
        std::string skillTime = "";
        for (const auto& time : tricker->timeUntilSkillAvailable)
            skillTime += std::to_string(time) + ", ";
        logger->info("score={}, facing direction={}, skill time={}", tricker->score, tricker->facingDirection, skillTime);
        std::string props = "";
        for (const auto& prop : tricker->props)
            props += THUAI6::propTypeDict[prop] + ", ";
        logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[tricker->playerState], THUAI6::bulletTypeDict[tricker->bulletType], props);
        logger->info("type={}, trick desire={}, class volume={}", THUAI6::trickerTypeDict[tricker->trickerType], tricker->trickDesire, tricker->classVolume);
        std::string trickerBuff = "";
        for (const auto& buff : tricker->buff)
            trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
        logger->info("buff={}", trickerBuff);
        logger->info("************************\n");
    }
}

void StudentDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, facing direction={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->facingDirection);
        logger->info("*********************\n");
    }
}

void TrickerDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, facing direction={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->facingDirection);
        logger->info("*********************\n");
    }
}

void StudentDebugAPI::PrintSelfInfo() const
{
    auto student = logic.StudentGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", student->playerID, student->guid, student->x, student->y);
    logger->info("speed={}, view range={}, place={}, radius={}", student->speed, student->viewRange, THUAI6::placeTypeDict[student->place], student->radius);
    std::string skillTime = "";
    for (const auto& time : student->timeUntilSkillAvailable)
        skillTime += std::to_string(time) + ", ";
    logger->info("score={}, facing direction={}, skill time={}", student->score, student->facingDirection, skillTime);
    std::string props = "";
    for (const auto& prop : student->props)
        props += THUAI6::propTypeDict[prop] + ", ";
    logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[student->playerState], THUAI6::bulletTypeDict[student->bulletType], props);
    logger->info("type={}, determination={}, addiction={}, danger alert={}", THUAI6::studentTypeDict[student->studentType], student->determination, student->addiction, student->dangerAlert);
    logger->info("learning speed={}, encourage speed={}, encourage progress={}, rouse progress={}", student->learningSpeed, student->encourageSpeed, student->encourageProgress, student->rouseProgress);
    std::string studentBuff = "";
    for (const auto& buff : student->buff)
        studentBuff += THUAI6::studentBuffDict[buff] + ", ";
    logger->info("buff={}", studentBuff);
    logger->info("*********************\n");
}

void TrickerDebugAPI::PrintSelfInfo() const
{
    auto tricker = logic.TrickerGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", tricker->playerID, tricker->guid, tricker->x, tricker->y);
    logger->info("speed={}, view range={}, place={}, radius={}", tricker->speed, tricker->viewRange, THUAI6::placeTypeDict[tricker->place], tricker->radius);
    std::string skillTime = "";
    for (const auto& time : tricker->timeUntilSkillAvailable)
        skillTime += std::to_string(time) + ", ";
    logger->info("score={}, facing direction={}, skill time={}", tricker->score, tricker->facingDirection, skillTime);
    std::string props = "";
    for (const auto& prop : tricker->props)
        props += THUAI6::propTypeDict[prop] + ", ";
    logger->info("state={}, bullet={}, props={}", THUAI6::playerStateDict[tricker->playerState], THUAI6::bulletTypeDict[tricker->bulletType], props);
    logger->info("type={}, trick desire={}, class volume={}", THUAI6::trickerTypeDict[tricker->trickerType], tricker->trickDesire, tricker->classVolume);
    std::string trickerBuff = "";
    for (const auto& buff : tricker->buff)
        trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
    logger->info("buff={}", trickerBuff);
    logger->info("*********************\n");
}

void StudentDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void TrickerDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}
