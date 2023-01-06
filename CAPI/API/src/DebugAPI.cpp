#include "AI.h"
#include "API.h"
#define PI 3.14159265358979323846

void HumanDebugAPI::StartTimer()
{
    StartPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(StartPoint);
}

void ButcherDebugAPI::StartTimer()
{
    StartPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(StartPoint);
}

void HumanDebugAPI::EndTimer()
{
}

void ButcherDebugAPI::EndTimer()
{
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
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
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
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
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
    return std::async(std::launch::async, [&]()
                      { return logic.PickProp(prop); });
}

std::future<bool> HumanDebugAPI::UseProp()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseProp(); });
}

std::future<bool> ButcherDebugAPI::PickProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [&]()
                      { return logic.PickProp(prop); });
}

std::future<bool> ButcherDebugAPI::UseProp()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseProp(); });
}

std::future<bool> HumanDebugAPI::UseSkill()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseSkill(); });
}

std::future<bool> ButcherDebugAPI::UseSkill()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseSkill(); });
}

std::future<bool> HumanDebugAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [&]()
                      { return logic.SendMessage(toID, message); });
}

std::future<bool> ButcherDebugAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [&]()
                      { return logic.SendMessage(toID, message); });
}

std::future<bool> HumanDebugAPI::HaveMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HaveMessage(); });
}

std::future<bool> ButcherDebugAPI::HaveMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HaveMessage(); });
}

std::future<std::optional<std::pair<int64_t, std::string>>> HumanDebugAPI::GetMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.GetMessage(); });
}

std::future<std::optional<std::pair<int64_t, std::string>>> ButcherDebugAPI::GetMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.GetMessage(); });
}

std::future<bool> HumanDebugAPI::Wait()
{
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, [&]()
                          { return false; });
    else
        return std::async(std::launch::async, [&]()
                          { return logic.WaitThread(); });
}

std::future<bool> ButcherDebugAPI::Wait()
{
    if (logic.GetCounter() == -1)
        return std::async(std::launch::async, [&]()
                          { return false; });
    else
        return std::async(std::launch::async, [&]()
                          { return logic.WaitThread(); });
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> HumanDebugAPI::GetButcher() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> HumanDebugAPI::GetHuman() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> ButcherDebugAPI::GetButcher() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> ButcherDebugAPI::GetHuman() const
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

THUAI6::PlaceType HumanDebugAPI::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    return logic.GetPlaceType(CellX, CellY);
}

THUAI6::PlaceType ButcherDebugAPI::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    return logic.GetPlaceType(CellX, CellY);
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
    return std::async(std::launch::async, [&]()
                      { return logic.StartFixMachine(); });
}

std::future<bool> HumanDebugAPI::EndFixMachine()
{
    return std::async(std::launch::async, [&]()
                      { return logic.EndFixMachine(); });
}

std::future<bool> HumanDebugAPI::StartSaveHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.StartSaveHuman(); });
}

std::future<bool> HumanDebugAPI::EndSaveHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.EndSaveHuman(); });
}

std::future<bool> HumanDebugAPI::Escape()
{
    return std::async(std::launch::async, [&]()
                      { return logic.Escape(); });
}

std::shared_ptr<const THUAI6::Human> HumanDebugAPI::GetSelfInfo() const
{
    return logic.HumanGetSelfInfo();
}

std::future<bool> ButcherDebugAPI::Attack(double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Attack(angleInRadian); });
}

std::future<bool> ButcherDebugAPI::CarryHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.CarryHuman(); });
}

std::future<bool> ButcherDebugAPI::ReleaseHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.ReleaseHuman(); });
}

std::future<bool> ButcherDebugAPI::HangHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HangHuman(); });
}

std::shared_ptr<const THUAI6::Butcher> ButcherDebugAPI::GetSelfInfo() const
{
    return logic.ButcherGetSelfInfo();
}

void HumanDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}
