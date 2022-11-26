#include "AI.h"
#include "logic.h"
#include "structures.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

int THUAI6Main(CreateAIFunc AIBuilder)
{
    // 仅供调试使用
    int playerID = 123;
    std::string sIP = "183.172.208.226";
    std::string sPort = "8888";
    extern const THUAI6::PlayerType playerType;
    extern const THUAI6::ButcherType butcherType;
    extern const THUAI6::HumanType humanType;
    THUAI6::PlayerType player = playerType;
    THUAI6::HumanType human = humanType;
    THUAI6::ButcherType butcher = butcherType;
    Logic logic(player, playerID, butcher, human);
    logic.Main(AIBuilder, sIP, sPort);
    return 0;
}

std::unique_ptr<IAI> CreateAI()
{
    return std::make_unique<AI>();
}

int main()
{
    return THUAI6Main(CreateAI);
}