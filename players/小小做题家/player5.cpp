#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include<string>

#ifndef PI
#define PI 3.14159265358979323846
#endif //


// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数
[[nodiscard]] constexpr inline int GridToCell(double grid) noexcept
{
    return int(grid) / numOfGridPerCell;
}
//x,y是学生自己的坐标，newXnewY是要查看视野的坐标
inline bool HaveView1(int64_t viewRange, int x, int y, int newX, int newY, std::vector<std::vector<THUAI6::PlaceType>>& map)
{
    int deltaX = newX - x;
    int deltaY = newY - y;
    double distance = double(deltaX) * double(deltaX) + double(deltaY) * double(deltaY);
    THUAI6::PlaceType myPlace = map[GridToCell(x)][GridToCell(y)];
    THUAI6::PlaceType newPlace = map[GridToCell(newX)][GridToCell(newY)];
    if (newPlace == THUAI6::PlaceType::Grass && myPlace != THUAI6::PlaceType::Grass)  // 草丛外必不可能看到草丛内
        return false;
    if (distance < viewRange * viewRange)
    {
        int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
        if (divide == 0)
            return true;
        double dx = deltaX / divide;
        double dy = deltaY / divide;
        double myX = double(x);
        double myY = double(y);
        if (newPlace == THUAI6::PlaceType::Grass && myPlace == THUAI6::PlaceType::Grass)  // 都在草丛内，要另作判断
            for (int i = 0; i < divide; i++)
            {
                myX += dx;
                myY += dy;
                if (map[GridToCell(myX)][GridToCell(myY)] != THUAI6::PlaceType::Grass)
                    return false;
            }
        else  // 不在草丛内，只需要没有墙即可
            for (int i = 0; i < divide; i++)
            {

                myX += dx;
                myY += dy;
                if (map[GridToCell(myX)][GridToCell(myY)] == THUAI6::PlaceType::Wall)
                    return false;
            }
        return true;
    }
    else
    {
        return false;
    }
}

std::pair<int32_t, int32_t> HaveView2(IStudentAPI& api, std::shared_ptr<const THUAI6::Student> self, int32_t newX, int32_t newY);

//查看（i，j）上的作业是否完成
bool checkstudyisdone(IStudentAPI& api, int i, int j)
{
    if (api.GetClassroomProgress(i, j) == 10000000)
    {
        return true;
    }
    return false;
}

//查看（i，j）上的箱子是否已经打开
bool checkchestisdone(IStudentAPI& api, int i, int j)
{
    if (api.GetChestProgress(i, j) == 10000000)
    {
        return true;
    }
    return false;
}

//X,Y是学生所在的坐标,返回值是教室的坐标或者能看到教室的坐标
std::pair<int32_t, int32_t> FindClassRoom(IStudentAPI& api, int32_t X, int32_t Y)
{
    int32_t i = 0, j = 0;
    auto map = api.GetFullMap();
    for (auto it1 = map.begin(); it1 != map.end(); it1++, i++)
    {
        j = 0;
        for (auto it2 = map[i].begin(); it2 != map[i].end(); it2++, j++)
        {
            if (map[i][j] != THUAI6::PlaceType::ClassRoom)
                ;
            else if (!checkstudyisdone(api, i, j))
            {
                if (HaveView1(api.GetSelfInfo()->viewRange, X, Y, api.CellToGrid(i), api.CellToGrid(j), map))
                {
                    return std::make_pair(api.CellToGrid(i), api.CellToGrid(j));
                }
                else if (HaveView2(api, api.GetSelfInfo(), api.CellToGrid(i), api.CellToGrid(j)) != std::make_pair(0, 0))
                {
                    return HaveView2(api, api.GetSelfInfo(), api.CellToGrid(i), api.CellToGrid(j));
                }
            }
        }
    }
    return std::make_pair(X, Y);
}

//X,Y是学生所在的坐标,返回值是箱子的坐标或者能看到箱子的坐标
std::pair<int32_t, int32_t> FindChest(IStudentAPI& api, int32_t X, int32_t Y)
{
    int32_t i = 0, j = 0;
    auto map = api.GetFullMap();
    for (auto it1 = map.begin(); it1 != map.end(); it1++, i++)
    {
        j = 0;
        for (auto it2 = map[i].begin(); it2 != map[i].end(); it2++, j++)
        {
            if (map[i][j] != THUAI6::PlaceType::Chest)
                ;
            else if (!checkchestisdone(api, i, j))
            {
                if (HaveView1(11000, X, Y, api.CellToGrid(i), api.CellToGrid(j), map))
                {
                    return std::make_pair(api.CellToGrid(i), api.CellToGrid(j));
                }
                else if (HaveView2(api, api.GetSelfInfo(), api.CellToGrid(i), api.CellToGrid(j)) != std::make_pair(0, 0))
                {
                    return HaveView2(api, api.GetSelfInfo(), api.CellToGrid(i), api.CellToGrid(j));
                }
            }
        }
    }
    return std::make_pair(X, Y);
}

void AvoidWall(IStudentAPI& api, std::shared_ptr<const THUAI6::Student> self)
{
    int X = api.GridToCell(self->x);
    int Y = api.GridToCell(self->y);
    if (api.GetFullMap()[X + 1][Y] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X + 1, Y) == 10000000))
    {
        api.MoveUp(50);           std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.MoveRight(50);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X][Y + 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X, Y + 1) == 10000000))
    {
        api.MoveLeft(50);             std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.MoveUp(50);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X - 1][Y] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X - 1, Y) == 10000000))
    {
        api.MoveDown(50);              std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.MoveLeft(50);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X][Y - 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X, Y - 1) == 10000000))
    {
        api.MoveRight(50);           std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.MoveDown(50);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X - 1][Y + 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X - 1, Y + 1) == 10000000))
    {
        api.Move(50, 7 * PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.Move(50, 5 * PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X + 1][Y + 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X + 1, Y + 1) == 10000000))
    {
        api.Move(50, 5 * PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.Move(50, 3 * PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X - 1][Y - 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X - 1, Y - 1) == 10000000))
    {
        api.Move(50, PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.Move(50, -PI / 4);    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    else if (api.GetFullMap()[X + 1][Y - 1] == THUAI6::PlaceType::Wall || (api.GetClassroomProgress(X + 1, Y - 1) == 10000000))
    {
        api.Move(50, 3 * PI / 4); std::this_thread::sleep_for(std::chrono::milliseconds(10));
        api.Move(50, PI / 4);     std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

}

void RandomMove(IStudentAPI& api)
{
    ;
}

//移动到该格子最中间
void CentreMove(IStudentAPI& api, std::shared_ptr<const THUAI6::Student> self)
{
    int X = api.GridToCell(self->x);
    int Y = api.GridToCell(self->y);
    int standardx, standardy;
    standardx = api.CellToGrid(X); standardy = api.CellToGrid(Y);
    if (standardx > self->x)
    {
        api.Move((standardx - self->x) * 1000 / self->speed, PI / 2); std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    else
    {
        api.Move((self->x - standardx) * 1000 / self->speed, -PI / 2); std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (standardy > self->y)
    {
        api.Move((standardy - self->y) * 1000 / self->speed, PI); std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    else
    {
        api.Move((self->y - standardy) * 1000 / self->speed, 0); std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

//穿过一格宽的缝隙
bool Penetrate(IStudentAPI& api, std::shared_ptr<const THUAI6::Student> self)
{
    int X = api.GridToCell(self->x);
    int Y = api.GridToCell(self->y);
    if ((api.GetFullMap()[X - 1][Y - 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X - 1][Y + 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X - 1][Y] == THUAI6::PlaceType::Land))
    {
        CentreMove(api, self);
        api.MoveUp(100); std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    if ((api.GetFullMap()[X - 1][Y + 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X + 1][Y + 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X][Y + 1] == THUAI6::PlaceType::Land))
    {
        CentreMove(api, self);
        api.MoveRight(100); std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    if ((api.GetFullMap()[X + 1][Y + 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X + 1][Y - 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X + 1][Y] == THUAI6::PlaceType::Land))
    {
        CentreMove(api, self);
        api.MoveDown(100); std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    if ((api.GetFullMap()[X + 1][Y - 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X - 1][Y - 1] == THUAI6::PlaceType::Wall) && (api.GetFullMap()[X][Y - 1] == THUAI6::PlaceType::Land))
    {
        CentreMove(api, self);
        api.MoveLeft(100); std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return true;
    }
    return false;
}

//HaveView2可以找到一个中继点来看到（newX,newY），返回值是中继点的pair，如果找不到就返回（0,0）
std::pair<int32_t, int32_t> HaveView2(IStudentAPI& api, std::shared_ptr<const THUAI6::Student> self, int32_t newX, int32_t newY)
{
    auto map = api.GetFullMap();
    int32_t i = 0, j = 0;
    for (auto it1 = map.begin(); it1 != map.end(); it1++, i++)
    {
        j = 0;
        for (auto it2 = map[i].begin(); it2 != map[i].end(); it2++, j++)
        {
            if (HaveView1(10 * self->viewRange, self->x, self->y, api.CellToGrid(i), api.CellToGrid(j), map) && HaveView1(self->viewRange, api.CellToGrid(i), api.CellToGrid(j), newX, newY, map))
            {
                return std::make_pair(api.CellToGrid(i), api.CellToGrid(j));
            }
        }
    }
    return std::make_pair(0, 0);
}

void AI::play(IStudentAPI& api)
{

    // 公共操作

    if (this->playerID == 0)
    {
        auto self = api.GetSelfInfo();

        int32_t speed = self->speed;
        auto classroom = FindClassRoom(api, self->x, self->y);
        if (classroom != std::make_pair(self->x, self->y))
        {
            int32_t distance = (classroom.first - self->x) * (classroom.first - self->x) + (classroom.second - self->y) * (classroom.second - self->y);
            if (!Penetrate(api, self)) AvoidWall(api, self);
            api.Move(sqrt(distance) * 1000 / speed, atan2(classroom.second - self->y, classroom.first - self->x));
            if (sqrt(distance) < 1500)
            {
                api.StartLearning(); std::this_thread::sleep_for(std::chrono::milliseconds(100));               
            }
        }
        else
        {
            api.Move(50, atan2(classroom.second - self->y, classroom.first - self->x) + 0.5 * PI);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (!Penetrate(api, self)) AvoidWall(api, self);
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        auto self = api.GetSelfInfo();
        int32_t speed = self->speed;
        auto classroom = FindClassRoom(api, self->x, self->y);
        if (!Penetrate(api, self)) AvoidWall(api, self);
        if (classroom != std::make_pair(self->x, self->y))
        {
            int32_t distance = (classroom.first - self->x) * (classroom.first - self->x) + (classroom.second - self->y) * (classroom.second - self->y);
            Penetrate(api, self);
            api.Move(sqrt(distance) * 1000 / speed, atan2(classroom.second - self->y, classroom.first - self->x));
            if (sqrt(distance) < 1000)
            {
                api.StartLearning(); std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        else
        {
            api.Move(50, atan2(classroom.second - self->y, classroom.first - self->x) + PI);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (!Penetrate(api, self)) AvoidWall(api, self);

        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        auto self = api.GetSelfInfo();
        int32_t speed = self->speed;
        auto classroom = FindClassRoom(api, self->x, self->y);
        auto chest = FindChest(api, self->x, self->y);
        if (!Penetrate(api, self)) AvoidWall(api, self);
        if (1 == 0)
        {
            if (classroom != std::make_pair(self->x, self->y))
            {
                int32_t distance = (classroom.first - self->x) * (classroom.first - self->x) + (classroom.second - self->y) * (classroom.second - self->y);
                if (!Penetrate(api, self)) AvoidWall(api, self);
                api.Move(sqrt(distance) * 1000 / speed, atan2(classroom.second - self->y, classroom.first - self->x));
                if (sqrt(distance) < 1500)
                {
                    api.StartLearning(); std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            else
            {
                api.Move(50, atan2(classroom.second - self->y, classroom.first - self->x) + PI);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        if (chest != std::make_pair(self->x, self->y))
        {
            int32_t distance = (chest.first - self->x) * (chest.first - self->x) + (chest.second - self->y) * (chest.second - self->y);
            if (!Penetrate(api, self)) AvoidWall(api, self);
            api.Move(sqrt(distance) * 1000 / speed, atan2(chest.second - self->y, chest.first - self->x));
            if (sqrt(distance) < 1500)
            {
                while (!checkchestisdone(api, api.GridToCell(chest.first), api.GridToCell(chest.second)))
                {
                    api.StartOpenChest(); std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
        else
        {
            api.Move(5000, rand());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (!Penetrate(api, self)) AvoidWall(api, self);
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        auto self = api.GetSelfInfo();
        int32_t speed = self->speed;
        auto classroom = FindClassRoom(api, self->x, self->y);
        if (!Penetrate(api, self)) AvoidWall(api, self);
        if (classroom != std::make_pair(self->x, self->y))
        {
            int32_t distance = (classroom.first - self->x) * (classroom.first - self->x) + (classroom.second - self->y) * (classroom.second - self->y);
            if (sqrt(distance) < 1300)
            {
                api.StartLearning(); std::this_thread::sleep_for(std::chrono::milliseconds(100));
                goto tag;
            }
            Penetrate(api, self);
            api.Move(sqrt(distance) * 1000 / speed, atan2(classroom.second - self->y, classroom.first - self->x));

        }
        else
        {
            api.Move(50, atan2(classroom.second - self->y, classroom.first - self->x) + PI);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (!Penetrate(api, self)) AvoidWall(api, self);
        // 玩家3执行操作
    tag:;
    }

    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
}