#include <optional>
#include <string>
#include "AI.h"
#include "API.h"
#include "utils.hpp"
#include "structures.h"
#define PI 3.14159265358979323846

HumanDebugAPI::HumanDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
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

ButcherDebugAPI::ButcherDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
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

void HumanDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void ButcherDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void HumanDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

void ButcherDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

int HumanDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

int ButcherDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

std::future<bool> HumanDebugAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    logger->info("Move: timeInMilliseconds = {}, angleInRadian = {}, called at {}ms", timeInMilliseconds, angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Move(timeInMilliseconds, angleInRadian);
                        if (!result)
                            logger->warn("Move: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> HumanDebugAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> HumanDebugAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> HumanDebugAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> ButcherDebugAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    logger->info("Move: timeInMilliseconds = {}, angleInRadian = {}, called at {}ms", timeInMilliseconds, angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Move(timeInMilliseconds, angleInRadian);
                        if (!result)
                            logger->warn("Move: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> ButcherDebugAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> ButcherDebugAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> ButcherDebugAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> HumanDebugAPI::PickProp(THUAI6::PropType prop)
{
    logger->info("PickProp: prop = {}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.PickProp(prop);
                        if (!result)
                           logger->warn("PickProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::UseProp()
{
    logger->info("UseProp: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.UseProp();
                        if (!result)
                            logger->warn("UseProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::PickProp(THUAI6::PropType prop)
{
    logger->info("PickProp: prop = {}, called at {}ms", THUAI6::propTypeDict[prop], Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.PickProp(prop);
                        if (!result)
                            logger->warn("PickProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::UseProp()
{
    logger->info("UseProp: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseProp();
                        if (!result)
                            logger->warn("UseProp: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::UseSkill()
{
    logger->info("UseSkill: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseSkill();
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::UseSkill()
{
    logger->info("UseSkill: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.UseSkill();
                        if (!result)
                            logger->warn("UseSkill: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::SendMessage(int64_t toID, std::string message)
{
    logger->info("SendMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.SendMessage(toID, message);
                        if (!result)
                            logger->warn("SendMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::SendMessage(int64_t toID, std::string message)
{
    logger->info("SendMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.SendMessage(toID, message);
                        if (!result)
                            logger->warn("SendMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.HaveMessage();
                        if (!result)
                            logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.HaveMessage();
                        if (!result)
                            logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<std::optional<std::pair<int64_t, std::string>>> HumanDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.GetMessage();
                        if (result == std::nullopt)
                            logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<std::optional<std::pair<int64_t, std::string>>> ButcherDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.GetMessage();
                        if (result == std::nullopt)
                            logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, []()
                          { return false; });
    else
        return std::async(std::launch::async, [this]()
                          { return logic.WaitThread(); });
}

std::future<bool> ButcherDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, []()
                          { return false; });
    else
        return std::async(std::launch::async, [this]()
                          { return logic.WaitThread(); });
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> HumanDebugAPI::GetButchers() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> HumanDebugAPI::GetHumans() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> ButcherDebugAPI::GetButchers() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> ButcherDebugAPI::GetHumans() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> HumanDebugAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> ButcherDebugAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::vector<THUAI6::PlaceType>> HumanDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI6::PlaceType HumanDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

THUAI6::PlaceType ButcherDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

std::vector<std::vector<THUAI6::PlaceType>> ButcherDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

const std::vector<int64_t> HumanDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

const std::vector<int64_t> ButcherDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::future<bool> HumanDebugAPI::StartFixMachine()
{
    logger->info("StartFixMachine: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartFixMachine();
                        if (!result)
                            logger->warn("StartFixMachine: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::EndFixMachine()
{
    logger->info("EndFixMachine: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndFixMachine();
                        if (!result)
                            logger->warn("EndFixMachine: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::StartSaveHuman()
{
    logger->info("StartSaveHuman: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.StartSaveHuman();
                        if (!result)
                            logger->warn("StartSaveHuman: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::EndSaveHuman()
{
    logger->info("EndSaveHuman: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.EndSaveHuman();
                        if (!result)
                            logger->warn("EndSaveHuman: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> HumanDebugAPI::Escape()
{
    logger->info("Escape: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.Escape();
                        if (!result)
                            logger->warn("Escape: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::shared_ptr<const THUAI6::Human> HumanDebugAPI::GetSelfInfo() const
{
    return logic.HumanGetSelfInfo();
}

std::future<bool> ButcherDebugAPI::Attack(double angleInRadian)
{
    logger->info("Attack: angleInRadian = {}, called at {}ms", angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Attack(angleInRadian);
                        if (!result)
                            logger->warn("Attack: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::CarryHuman()
{
    logger->info("CarryHuman: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.CarryHuman();
                        if (!result)
                            logger->warn("CarryHuman: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::ReleaseHuman()
{
    logger->info("ReleaseHuman: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.ReleaseHuman();
                        if (!result)
                            logger->warn("ReleaseHuman: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ButcherDebugAPI::HangHuman()
{
    logger->info("HangHuman: called at {}ms", Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [this]()
                      { auto result = logic.HangHuman();
                        if (!result)
                            logger->warn("HangHuman: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::shared_ptr<const THUAI6::Butcher> ButcherDebugAPI::GetSelfInfo() const
{
    return logic.ButcherGetSelfInfo();
}

void HumanDebugAPI::PrintHuman() const
{
    for (auto human : logic.GetHumans())
    {
        logger->info("******Human Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", human->playerID, human->guid, human->x, human->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", human->speed, human->viewRange, human->timeUntilSkillAvailable, THUAI6::propTypeDict[human->prop], THUAI6::placeTypeDict[human->place]);
        logger->info("state={}, life={}, hangedTime={}", THUAI6::humanStateDict[human->state], human->life, human->hangedTime);
        std::string humanBuff = "buff=";
        for (auto buff : human->buff)
            humanBuff += THUAI6::humanBuffDict[buff] + ", ";
        logger->info(humanBuff);
        logger->info("**********************");
    }
}

void ButcherDebugAPI::PrintHuman() const
{
    for (auto human : logic.GetHumans())
    {
        logger->info("******Human Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", human->playerID, human->guid, human->x, human->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", human->speed, human->viewRange, human->timeUntilSkillAvailable, THUAI6::propTypeDict[human->prop], THUAI6::placeTypeDict[human->place]);
        logger->info("state={}, life={}, hangedTime={}", THUAI6::humanStateDict[human->state], human->life, human->hangedTime);
        std::string humanBuff = "buff=";
        for (auto buff : human->buff)
            humanBuff += THUAI6::humanBuffDict[buff] + ", ";
        logger->info(humanBuff);
        logger->info("**********************");
    }
}

void HumanDebugAPI::PrintButcher() const
{
    for (auto butcher : logic.GetButchers())
    {
        logger->info("******Butcher Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", butcher->playerID, butcher->guid, butcher->x, butcher->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", butcher->speed, butcher->viewRange, butcher->timeUntilSkillAvailable, THUAI6::propTypeDict[butcher->prop], THUAI6::placeTypeDict[butcher->place]);
        logger->info("damage={}, movable={}", butcher->damage, butcher->movable);
        std::string butcherBuff = "buff=";
        for (auto buff : butcher->buff)
            butcherBuff += THUAI6::butcherBuffDict[buff] + ", ";
        logger->info(butcherBuff);
        logger->info("************************");
    }
}

void ButcherDebugAPI::PrintButcher() const
{
    for (auto butcher : logic.GetButchers())
    {
        logger->info("******Butcher Info******");
        logger->info("playerID={}, GUID={}, x={}, y={}", butcher->playerID, butcher->guid, butcher->x, butcher->y);
        logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", butcher->speed, butcher->viewRange, butcher->timeUntilSkillAvailable, THUAI6::propTypeDict[butcher->prop], THUAI6::placeTypeDict[butcher->place]);
        logger->info("damage={}, movable={}", butcher->damage, butcher->movable);
        std::string butcherBuff = "buff=";
        for (auto buff : butcher->buff)
            butcherBuff += THUAI6::butcherBuffDict[buff] + ", ";
        logger->info(butcherBuff);
        logger->info("************************");
    }
}

void HumanDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, is moving={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->isMoving);
        logger->info("*********************");
    }
}

void ButcherDebugAPI::PrintProp() const
{
    for (auto prop : logic.GetProps())
    {
        logger->info("******Prop Info******");
        logger->info("GUID={}, x={}, y={}, place={}, is moving={}", prop->guid, prop->x, prop->y, THUAI6::placeTypeDict[prop->place], prop->isMoving);
        logger->info("*********************");
    }
}

void HumanDebugAPI::PrintSelfInfo() const
{
    auto self = logic.HumanGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", self->playerID, self->guid, self->x, self->y);
    logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", self->speed, self->viewRange, self->timeUntilSkillAvailable, THUAI6::propTypeDict[self->prop], THUAI6::placeTypeDict[self->place]);
    logger->info("state={}, life={}, hangedTime={}", THUAI6::humanStateDict[self->state], self->life, self->hangedTime);
    std::string humanBuff = "buff=";
    for (auto buff : self->buff)
        humanBuff += THUAI6::humanBuffDict[buff] + ", ";
    logger->info(humanBuff);
    logger->info("*********************");
}

void ButcherDebugAPI::PrintSelfInfo() const
{
    auto self = logic.ButcherGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, GUID={}, x={}, y={}", self->playerID, self->guid, self->x, self->y);
    logger->info("speed={}, view range={}, skill time={}, prop={}, place={}", self->speed, self->viewRange, self->timeUntilSkillAvailable, THUAI6::propTypeDict[self->prop], THUAI6::placeTypeDict[self->place]);
    logger->info("damage={}, movable={}", self->damage, self->movable);
    std::string butcherBuff = "buff=";
    for (auto buff : self->buff)
        butcherBuff += THUAI6::butcherBuffDict[buff] + ", ";
    logger->info(butcherBuff);
    logger->info("*********************");
}

void HumanDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}
