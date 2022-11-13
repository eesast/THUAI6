#include "AI.h"
#include "API.h"

std::future<bool> HumanAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [&]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

void HumanAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherAPI::Play(IAI& ai)
{
    ai.play(*this);
}
