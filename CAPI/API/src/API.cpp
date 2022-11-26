#include "AI.h"
#include "API.h"
#define PI 3.14159265358979323846

std::future<bool> HumanAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

std::future<bool> HumanAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> HumanAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> HumanAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> HumanAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> ButcherAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

std::future<bool> ButcherAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> ButcherAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> ButcherAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> ButcherAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> HumanAPI::PickProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [&]()
                      { return logic.PickProp(prop); });
}

std::future<bool> HumanAPI::UseProp()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseProp(); });
}

std::future<bool> ButcherAPI::PickProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [&]()
                      { return logic.PickProp(prop); });
}

std::future<bool> ButcherAPI::UseProp()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseProp(); });
}

std::future<bool> HumanAPI::UseSkill()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseSkill(); });
}

std::future<bool> ButcherAPI::UseSkill()
{
    return std::async(std::launch::async, [&]()
                      { return logic.UseSkill(); });
}

std::future<bool> HumanAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [&]()
                      { return logic.SendMessage(toID, message); });
}

std::future<bool> ButcherAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [&]()
                      { return logic.SendMessage(toID, message); });
}

std::future<bool> HumanAPI::HaveMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HaveMessage(); });
}

std::future<bool> ButcherAPI::HaveMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HaveMessage(); });
}

std::future<std::pair<int64_t, std::string>> HumanAPI::GetMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.GetMessage(); });
}

std::future<std::pair<int64_t, std::string>> ButcherAPI::GetMessage()
{
    return std::async(std::launch::async, [&]()
                      { return logic.GetMessage(); });
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> HumanAPI::GetButcher() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> HumanAPI::GetHuman() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> ButcherAPI::GetButcher() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> ButcherAPI::GetHuman() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> HumanAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> ButcherAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::vector<THUAI6::PlaceType>> HumanAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI6::PlaceType HumanAPI::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    return logic.GetPlaceType(CellX, CellY);
}

THUAI6::PlaceType ButcherAPI::GetPlaceType(int32_t CellX, int32_t CellY) const
{
    return logic.GetPlaceType(CellX, CellY);
}

std::vector<std::vector<THUAI6::PlaceType>> ButcherAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

const std::vector<int64_t> HumanAPI::GetPlayerGUIDs() const
{
    // todo
}

const std::vector<int64_t> ButcherAPI::GetPlayerGUIDs() const
{
    // todo
}

std::future<bool> HumanAPI::StartFixMachine()
{
    std::async(std::launch::async, [&]()
               { return logic.StartFixMachine(); });
}

std::future<bool> HumanAPI::EndFixMachine()
{
    std::async(std::launch::async, [&]()
               { return logic.EndFixMachine(); });
}

std::future<bool> HumanAPI::StartSaveHuman()
{
    std::async(std::launch::async, [&]()
               { return logic.StartSaveHuman(); });
}

std::future<bool> HumanAPI::EndSaveHuman()
{
    std::async(std::launch::async, [&]()
               { return logic.EndSaveHuman(); });
}

std::future<bool> HumanAPI::Escape()
{
    return std::async(std::launch::async, [&]()
                      { return logic.Escape(); });
}

std::shared_ptr<const THUAI6::Human> HumanAPI::GetSelfInfo() const
{
    return logic.HumanGetSelfInfo();
}

std::future<bool> ButcherAPI::Attack(double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Attack(angleInRadian); });
}

std::future<bool> ButcherAPI::CarryHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.CarryHuman(); });
}

std::future<bool> ButcherAPI::ReleaseHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.ReleaseHuman(); });
}

std::future<bool> ButcherAPI::HangHuman()
{
    return std::async(std::launch::async, [&]()
                      { return logic.HangHuman(); });
}

std::shared_ptr<const THUAI6::Butcher> ButcherAPI::GetSelfInfo() const
{
    return logic.ButcherGetSelfInfo();
}

void HumanAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherAPI::Play(IAI& ai)
{
    ai.play(*this);
}
