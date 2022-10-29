#include <vector>
#include <thread>
#include "AI.h"

// 选手必须定义该变量来选择自己的阵营
extern const THUAI6::PlayerType playerType = THUAI6::PlayerType::HumanPlayer;

// 选手只需要定义两者中自己选中的那个即可，定义两个也不会有影响。
extern const THUAI6::ButcherType butcherType = THUAI6::ButcherType::ButcherType1;

extern const THUAI6::HumanType humanType = THUAI6::HumanType::HumanType1;

void AI::play(IAPI& api)
{
}