#include "AI.h"
#include "API.h"

void HumanAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void ButcherAPI::Play(IAI& ai)
{
    ai.play(*this);
}

std::vector<std::shared_ptr<const THUAI6::Butcher>> HumanAPI::GetButcher() const
{
    return logic.GetButchers();
}
