#include <future>
#include "AI.h"
#include "constants.h"
// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数THUAI6::
struct node
{
    int x; int y;
};
int step = 1 * 1000 * 1000 / Constants::Athlete::moveSpeed;
void LeftTo(ITrickerAPI& api, int x)
{
    auto self = api.GetSelfInfo();
    while (x < self->y)
        api.MoveLeft(50);
}
//右移至x处
void RightTo(ITrickerAPI& api, int x)
{
    auto self = api.GetSelfInfo();
    while (x > self->y)
        api.MoveRight(50);
}

//上移至x处
void UpTo(ITrickerAPI& api, int x)
{
    auto self = api.GetSelfInfo();
    while (x < self->x)
        api.MoveUp(50);
}

//下移至x处
void DownTo(ITrickerAPI& api, int x)
{
    auto self = api.GetSelfInfo();
    while (x > self->x)
        api.MoveDown(50);
}

void moveto(int a, int b, IStudentAPI& api)//移动到cell(a,b)
{
    if ((a + 1) != 0)
    {
        while (api.GetSelfInfo()->x < 1000 * (a + 1) - 600)
        {
            api.MoveDown(50);
        }
        while (api.GetSelfInfo()->x > 1000 * (a + 1) - 400)
        {
            api.MoveUp(50);
        }
    }
    if ((b + 1) != 0)
    {
        while (api.GetSelfInfo()->y < 1000 * (b + 1) - 600)
        {
            api.MoveRight(50);
        }
        while (api.GetSelfInfo()->y > 1000 * (b + 1) - 400)
        {
            api.MoveLeft(50);
        }
    }
    api.EndAllAction();
}
void moveto3(int a, int b, IStudentAPI& api)
{
    while ((api.GetSelfInfo()->y < 1000 * (b + 1) - 600) || (api.GetSelfInfo()->y > 1000 * (b + 1) - 400) || (api.GetSelfInfo()->x < 1000 * (a + 1) - 600) || (api.GetSelfInfo()->x > 1000 * (a + 1) - 400))
    {
        while (api.GetSelfInfo()->y < 1000 * (b + 1) - 600)
        {
            api.MoveRight(50);
        }
        while (api.GetSelfInfo()->y > 1000 * (b + 1) - 400)
        {
            api.MoveLeft(50);
        }
        api.EndAllAction();
        while (api.GetSelfInfo()->x < 1000 * (a + 1) - 600)
        {
            api.MoveDown(50);
        }
        while (api.GetSelfInfo()->x > 1000 * (a + 1) - 400)
        {
            api.MoveUp(50);
        }
        api.EndAllAction();
    }
}
void slide1(IStudentAPI& api)
{
    auto p1 = api.GetSelfInfo();
    int x1 = api.GridToCell(p1->x);
    int y1 = api.GridToCell(p1->y);
    auto judge1 = api.GetPlaceType(x1 + 1, y1);
    auto judge2 = api.GetPlaceType(x1 - 1, y1);
    auto judge3 = api.GetPlaceType(x1, y1 + 1);
    auto judge4 = api.GetPlaceType(x1, y1 - 1);
    if (judge1 == THUAI6::PlaceType::Window || judge2 == THUAI6::PlaceType::Window || judge3 == THUAI6::PlaceType::Window || judge4 == THUAI6::PlaceType::Window)
    {
        api.SkipWindow();
    }
    api.EndAllAction();
}
void learn1(IStudentAPI& api)
{
    auto p1 = api.GetSelfInfo();
    int x1 = api.GridToCell(p1->x);
    int y1 = api.GridToCell(p1->y);
    auto judge1 = api.GetPlaceType(x1 + 1, y1);
    auto judge2 = api.GetPlaceType(x1 - 1, y1);
    auto judge3 = api.GetPlaceType(x1, y1 + 1);
    auto judge4 = api.GetPlaceType(x1, y1 - 1);
    if (judge1 == THUAI6::PlaceType::ClassRoom || judge2 == THUAI6::PlaceType::ClassRoom || judge3 == THUAI6::PlaceType::ClassRoom || judge4 == THUAI6::PlaceType::ClassRoom)
    {
        api.StartLearning();
    }
    api.EndAllAction();
}
void moveto1(IStudentAPI& api, node b)
{
    int x2, y2;
    int x0 = api.GetSelfInfo()->x;
    int y0 = api.GetSelfInfo()->y;
    x2 = api.CellToGrid(b.x);
    y2 = api.CellToGrid(b.y);
    api.Move(step, atan2(y2 - y0, x2 - x0));
    api.EndAllAction();
}

void moveto2(int a, int b, IStudentAPI& api)
{
    if ((b + 1) != 0)
    {
        while (api.GetSelfInfo()->y < 1000 * (b + 1) - 600)
        {
            api.MoveRight(50);
        }
        while (api.GetSelfInfo()->y > 1000 * (b + 1) - 400)
        {
            api.MoveLeft(50);
        }
    }
    if ((a + 1) != 0)
    {
        while (api.GetSelfInfo()->x < 1000 * (a + 1) - 600)
        {
            api.MoveDown(50);
        }
        while (api.GetSelfInfo()->x > 1000 * (a + 1) - 400)
        {
            api.MoveUp(50);
        }
    }
    api.EndAllAction();
}
void moveto2(int a, int b, ITrickerAPI& api)
{
    if ((b + 1) != 0)
    {
        while (api.GetSelfInfo()->y < 1000 * (b + 1) - 600)
        {
            api.MoveRight(50);
        }
        while (api.GetSelfInfo()->y > 1000 * (b + 1) - 400)
        {
            api.MoveLeft(50);
        }
    }
    if ((a + 1) != 0)
    {
        while (api.GetSelfInfo()->x < 1000 * (a + 1) - 600)
        {
            api.MoveDown(50);
        }
        while (api.GetSelfInfo()->x > 1000 * (a + 1) - 400)
        {
            api.MoveUp(50);
        }
    }
    api.EndAllAction();
}
void slide(IStudentAPI& api, node b)
{
    int x2, y2;
    int x0 = api.GetSelfInfo()->x;
    int y0 = api.GetSelfInfo()->y;
    x2 = api.CellToGrid(b.x);
    y2 = api.CellToGrid(b.y);
    if (api.GetPlaceType(b.x, b.y) == THUAI6::PlaceType::Window)
    {
        api.Move(step, atan2(y2 - y0, x2 - x0));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        api.Wait();
        x0 = api.GetSelfInfo()->x;
        y0 = api.GetSelfInfo()->y;
    }
}
void learn(IStudentAPI& api, node b)
{
    int x2, y2;
    int x0 = api.GetSelfInfo()->x;
    int y0 = api.GetSelfInfo()->y;
    x2 = api.CellToGrid(b.x);
    y2 = api.CellToGrid(b.y);
    if (api.GetPlaceType(b.x, b.y) == THUAI6::PlaceType::ClassRoom)
    {
        api.Move(step, atan2(y2 - y0, x2 - x0));
        x0 = api.GetSelfInfo()->x;
        y0 = api.GetSelfInfo()->y;
    }
    api.StartLearning();
    api.EndAllAction();
}

void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        if (api.GetClassroomProgress(10, 38) < 100)
        {
            moveto(12, 25, api);
            moveto(4, 37, api);
            moveto(10, 37, api);
        }
        while (api.GetClassroomProgress(10, 38) < 10000000)
        {
            learn1(api);
        }
        if (api.GetClassroomProgress(8, 31) < 100)
        {
            moveto(8, 32, api);
        }
        while (api.GetClassroomProgress(8, 31) < 10000000)
        {
            learn1(api);
        }
        if (api.GetClassroomProgress(19, 41) < 100)
        {
            moveto2(19, 42, api);
        }
        while (api.GetClassroomProgress(19, 41) < 10000000)
        {
            learn1(api);
        }
        moveto2(8, 42, api);
        moveto2(4, 37, api);
        moveto2(4, 18, api);
        moveto(2, 7, api);
        moveto2(2, 4, api);
        moveto(5, 5, api);
        while (api.GetGateProgress(5, 6) < 18000)
        {
            api.StartOpenGate();
        }
        api.EndAllAction();
        api.Graduate();
        api.EndAllAction();
        // 玩家2执行操作
    }


    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
}
