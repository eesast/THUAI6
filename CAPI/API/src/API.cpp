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
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, 0); });
}

std::future<bool> HumanAPI::MoveRight(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI * 0.5); });
}

std::future<bool> HumanAPI::MoveUp(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI); });
}

std::future<bool> HumanAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI * 1.5); });
}

std::future<bool> ButcherAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

std::future<bool> ButcherAPI::MoveDown(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, 0); });
}

std::future<bool> ButcherAPI::MoveRight(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI * 0.5); });
}

std::future<bool> ButcherAPI::MoveUp(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI); });
}

std::future<bool> ButcherAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, PI * 1.5); });
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

std::vector<std::shared_ptr<const THUAI6::Butcher>> HumanAPI::GetButcher() const
{
    return logic.GetButchers();
}

std::vector<std::shared_ptr<const THUAI6::Human>> HumanAPI::GetHuman() const
{
    return logic.GetHumans();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> HumanAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::vector<THUAI6::PlaceType>> HumanAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

void HumanAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherAPI::Play(IAI& ai)
{
    ai.play(*this);
}


