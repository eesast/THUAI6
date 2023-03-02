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

std::future<bool> StudentDebugAPI::UseProp()
{
    logger->info("UseProp: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseProp();
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

std::future<bool> TrickerDebugAPI::UseProp()
{
    logger->info("UseProp: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseProp();
                        if (!result)
                            logger->warn("UseProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::UseSkill()
{
    logger->info("UseSkill: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseSkill();
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::UseSkill()
{
    logger->info("UseSkill: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseSkill();
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
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

std::future<bool> StudentDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.HaveMessage();
                        if (!result)
                            logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.HaveMessage();
                        if (!result)
                            logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<std::optional<std::pair<int64_t, std::string>>> StudentDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.GetMessage();
                        if (result == std::nullopt)
                            logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<std::optional<std::pair<int64_t, std::string>>> TrickerDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.GetMessage();
                        if (result == std::nullopt)
                            logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, []()
                          { return false; });
    else
        return std::async(std::launch::async, [this]()
                          { return logic.WaitThread(); });
}

std::future<bool> TrickerDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, []()
                          { return false; });
    else
        return std::async(std::launch::async, [this]()
                          { return logic.WaitThread(); });
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

const std::vector<int64_t> StudentDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

const std::vector<int64_t> TrickerDebugAPI::GetPlayerGUIDs() const
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

std::future<bool> StudentDebugAPI::EndLearning()
{
    logger->info("EndLearning: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndLearning();
                        if (!result)
                            logger->warn("EndLearning: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::StartHelpMate()
{
    logger->info("StartHelpMate: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartHelpMate();
                        if (!result)
                            logger->warn("StartHelpMate: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> StudentDebugAPI::EndHelpMate()
{
    logger->info("EndHelpMate: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndHelpMate();
                        if (!result)
                            logger->warn("EndHelpMate: failed at {}ms", Time::TimeSinceStart(startPoint));
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

std::future<bool> TrickerDebugAPI::Trick(double angleInRadian)
{
    logger->info("Trick: angleInRadian = {}, called at {}ms", angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Trick(angleInRadian);
                        if (!result)
                            logger->warn("Trick: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::StartExam()
{
    logger->info("StartExam: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartExam();
                        if (!result)
                            logger->warn("StartExam: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::EndExam()
{
    logger->info("EndExam: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndExam();
                        if (!result)
                            logger->warn("EndExam: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TrickerDebugAPI::MakeFail()
{
    logger->info("MakeFail: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.MakeFail();
                        if (!result)
                            logger->warn("MakeFail: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::shared_ptr<const THUAI6::Tricker> TrickerDebugAPI::GetSelfInfo() const
{
    return logic.TrickerGetSelfInfo();
}

void StudentDebugAPI::PrintStudent() const
{
    for (auto student : logic.GetStudents())
    {
        logger->info("******Student Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", student->playerID, student->guid, student->x, student->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", student->speed, student->viewRange, student->timeUntilSkillAvailable, THUAI6::propTypeDict[student->prop], THUAI6::placeTypeDict[student->place]);
        logger->info("state={}, determination={}, fail num={}, fail time={}, emo time={}", THUAI6::studentStateDict[student->state], student->determination, student->failNum, student->failTime, student->emoTime);
        std::string studentBuff = "buff=";
        for (auto buff : student->buff)
            studentBuff += THUAI6::studentBuffDict[buff] + ", ";
        logger->info(studentBuff);
        logger->info("**********************");
    }
}

void TrickerDebugAPI::PrintStudent() const
{
    for (auto student : logic.GetStudents())
    {
        logger->info("******Student Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", student->playerID, student->guid, student->x, student->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", student->speed, student->viewRange, student->timeUntilSkillAvailable, THUAI6::propTypeDict[student->prop], THUAI6::placeTypeDict[student->place]);
        logger->info("state={}, determination={}, fail num={}, fail time={}, emo time={}", THUAI6::studentStateDict[student->state], student->determination, student->failNum, student->failTime, student->emoTime);
        std::string studentBuff = "buff=";
        for (auto buff : student->buff)
            studentBuff += THUAI6::studentBuffDict[buff] + ", ";
        logger->info(studentBuff);
        logger->info("**********************");
    }
}

void StudentDebugAPI::PrintTricker() const
{
    for (auto tricker : logic.GetTrickers())
    {
        logger->info("******Tricker Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", tricker->playerID, tricker->guid, tricker->x, tricker->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", tricker->speed, tricker->viewRange, tricker->timeUntilSkillAvailable, THUAI6::propTypeDict[tricker->prop], THUAI6::placeTypeDict[tricker->place]);
        logger->info("damage={}, movable={}", tricker->damage, tricker->movable);
        std::string trickerBuff = "buff=";
        for (auto buff : tricker->buff)
            trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
        logger->info(trickerBuff);
        logger->info("************************");
    }
}

void TrickerDebugAPI::PrintTricker() const
{
    for (auto tricker : logic.GetTrickers())
    {
        logger->info("******Tricker Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", tricker->playerID, tricker->guid, tricker->x, tricker->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", tricker->speed, tricker->viewRange, tricker->timeUntilSkillAvailable, THUAI6::propTypeDict[tricker->prop], THUAI6::placeTypeDict[tricker->place]);
        logger->info("damage={}, movable={}", tricker->damage, tricker->movable);
        std::string trickerBuff = "buff=";
        for (auto buff : tricker->buff)
            trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
        logger->info(trickerBuff);
        logger->info("************************");
    }
}

void StudentDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, is moving={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->isMoving);
        logger->info("*********************");
    }
}

void TrickerDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, is moving={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->isMoving);
        logger->info("*********************");
    }
}

void StudentDebugAPI::PrintSelfInfo() const
{
    auto self = logic.StudentGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", self->playerID, self->guid, self->x, self->y);
    logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", self->speed, self->viewRange, self->timeUntilSkillAvailable, THUAI6::propTypeDict[self->prop], THUAI6::placeTypeDict[self->place]);
    logger->info("state={}, determination={}, fail num={}, fail time={}, emo time={}", THUAI6::studentStateDict[self->state], self->determination, self->failNum, self->failTime, self->emoTime);
    std::string studentBuff = "buff=";
    for (auto buff : self->buff)
        studentBuff += THUAI6::studentBuffDict[buff] + ", ";
    logger->info(studentBuff);
    logger->info("*********************");
}

void TrickerDebugAPI::PrintSelfInfo() const
{
    auto self = logic.TrickerGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", self->playerID, self->guid, self->x, self->y);
    logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", self->speed, self->viewRange, self->timeUntilSkillAvailable, THUAI6::propTypeDict[self->prop], THUAI6::placeTypeDict[self->place]);
    logger->info("damage={}, movable={}", self->damage, self->movable);
    std::string trickerBuff = "buff=";
    for (auto buff : self->buff)
        trickerBuff += THUAI6::trickerBuffDict[buff] + ", ";
    logger->info(trickerBuff);
    logger->info("*********************");
}

void StudentDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void TrickerDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}
