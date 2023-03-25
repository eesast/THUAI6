#include <vector>
#include <thread>
#include "AI.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手必须定义该变量来选择自己的阵营
extern const THUAI6::PlayerType playerType = THUAI6::PlayerType::StudentPlayer;

// 选手需要将两个都定义，本份代码中不选择的阵营任意定义即可
extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

extern const THUAI6::StudentType studentType = THUAI6::StudentType::Athlete;

// 选手只需写一个即可，为了调试方便写了两个的话也不会有影响

void AI::play(IStudentAPI& api)
{
    api.Move(100, 1);
    api.PrintSelfInfo();
}

void AI::play(ITrickerAPI& api)
{
    api.Move(100, 1);
}
