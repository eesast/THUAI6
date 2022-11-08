#include "AI.h"
#include "API.h"

void HumanDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}