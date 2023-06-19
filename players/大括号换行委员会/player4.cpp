#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"

#define M_PI 3.1415926535

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
auto sec = std::chrono::duration_cast<std::chrono::milliseconds>
(std::chrono::system_clock::now().time_since_epoch()).count();

class Point
{
public:
    Point()
    {
        x = 0;
        y = 0;
        prex = 0;
        prey = 0;
        cost = 1;
        sumcost = 0;
    }
    Point(int tx, int ty, int tprex = 0, int tprey = 0, int tcost = 0, int tsumcost = 0)
    {
        x = tx;
        y = ty;
        prex = tprex;
        prey = tprey;
        cost = tcost;
        sumcost = tsumcost;
    }

    int x;
    int y;
    int prex;
    int prey;
    int cost;
    int sumcost;
    inline int32_t operator-(Point& p)
    {
        return (this->x - p.x) * (this->x - p.x) + (this->y - p.y) * (this->y - p.y);
    }
};

int direction[4][2] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
int moveStep = 50;
std::vector<Point> path;
int iPath = 0;  // 指向下一目标位置的指针
const int x = 50;
const int y = 50;
Point map[x][y];

bool hasLearned[10] = { false, false, false, false, false, false, false, false, false, false };

using Place = THUAI6::PlaceType;
const std::vector<std::vector<THUAI6::PlaceType>> Map =
{
    {Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Grass, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Wall, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Wall, Place::Land, Place::Wall, Place::Grass, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Wall, Place::Wall, Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Grass, Place::Grass,
     Place::Land, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Wall, Place::Wall, Place::Wall,  Place::Wall,
     Place::Wall, Place::Land, Place::Land, Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Wall,  Place::Wall,  Place::Wall,  Place::Wall,  Place::Grass,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Chest, Place::Wall,  Place::Wall,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Grass, Place::Grass, Place::Grass,
     Place::Grass, Place::Grass, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Wall,
     Place::Chest, Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall,  Place::Wall,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Gate,  Place::Wall,   Place::Wall, Place::Grass,
     Place::Land,  Place::Land,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Grass, Place::Grass, Place::Wall,   Place::Wall, Place::Grass,
     Place::Grass, Place::Wall,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall,  Place::Window, Place::Wall, Place::Grass,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Wall},
    {Place::Wall,  Place::Wall,  Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Wall,  Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Grass, Place::Grass, Place::Wall,  Place::Wall, Place::Grass,
     Place::Grass, Place::Wall,  Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall,  Place::Door5, Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall,  Place::Wall,  Place::Wall, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Wall,  Place::Grass, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Grass, Place::Grass, Place::Land,  Place::Land,
     Place::Land, Place::Grass, Place::Grass, Place::Grass, Place::Land,
     Place::Land, Place::Wall,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Wall,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Grass,     Place::Grass, Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Grass,     Place::Grass, Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Wall,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::ClassRoom, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Wall, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Land, Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land, Place::Land,      Place::Wall,
     Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,      Place::Wall,
     Place::Wall,  Place::Wall,  Place::Land, Place::Land,      Place::Land,
     Place::Land,  Place::Land,  Place::Land, Place::Land,      Place::Land,
     Place::Land,  Place::Land,  Place::Land, Place::Land,      Place::Land,
     Place::Land,  Place::Wall,  Place::Wall, Place::Wall,      Place::Wall,
     Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,      Place::Wall,
     Place::Land,  Place::Land,  Place::Land, Place::ClassRoom, Place::Land,
     Place::Land,  Place::Land,  Place::Land, Place::Land,      Place::Land,
     Place::Grass, Place::Grass, Place::Wall, Place::Land,      Place::Wall},
    {Place::Wall, Place::Land,  Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land,  Place::Land, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Grass, Place::Wall, Place::Land, Place::Wall},
    {Place::Wall,   Place::Land,  Place::Land, Place::Land,  Place::Wall,
     Place::Land,   Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land,   Place::Wall,  Place::Wall, Place::Door3, Place::Wall,
     Place::Wall,   Place::Wall,  Place::Wall, Place::Wall,  Place::Wall,
     Place::Window, Place::Wall,  Place::Wall, Place::Land,  Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land,  Place::Wall,
     Place::Land,   Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Grass,  Place::Grass, Place::Wall, Place::Land,  Place::Wall},
    {Place::Wall, Place::Land,  Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Chest, Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall,  Place::Wall, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land, Place::Window,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Wall, Place::Wall, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Grass,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Wall, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land,   Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Wall,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Grass,
     Place::Wall, Place::Land, Place::Land, Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Window, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land,   Place::Wall},
    {Place::Wall,  Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Wall, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Wall,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall, Place::Wall,
     Place::Grass, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall,   Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Wall,  Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Wall, Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Window, Place::Land,  Place::Land, Place::Land, Place::Land,
     Place::Wall,   Place::Wall,  Place::Wall, Place::Wall, Place::Land,
     Place::Land,   Place::Land,  Place::Land, Place::Land, Place::Wall,
     Place::Wall,   Place::Grass, Place::Land, Place::Land, Place::Wall},
    {Place::Wall,      Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::ClassRoom, Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,      Place::Wall,  Place::Land,   Place::Land, Place::Land,
     Place::Land,      Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Wall,      Place::Wall,  Place::Window, Place::Wall, Place::Land,
     Place::Land,      Place::Land,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Wall,      Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,      Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,      Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Wall,      Place::Grass, Place::Land,   Place::Land, Place::Wall},
    {Place::Wall, Place::Grass,     Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Wall,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Wall, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Door5, Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::ClassRoom, Place::Land,  Place::Land, Place::Land,
     Place::Wall, Place::Grass,     Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Grass, Place::Grass, Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Wall, Place::Wall,
     Place::Wall, Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Wall, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Grass, Place::Grass, Place::Land, Place::Land,
     Place::Wall, Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Wall, Place::Land,  Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Wall,  Place::Wall,  Place::Wall,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Wall, Place::Wall,  Place::Grass, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Grass, Place::Grass, Place::Wall,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Grass, Place::Wall,  Place::Wall,  Place::Wall,
     Place::Wall, Place::Chest, Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Land, Place::Land,  Place::Wall,      Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,      Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,      Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::ClassRoom, Place::Land,
     Place::Land, Place::Wall, Place::Grass, Place::Grass,     Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Grass,     Place::Wall,
     Place::Wall, Place::Wall, Place::Wall,  Place::Wall,      Place::Wall,
     Place::Wall, Place::Wall, Place::Wall,  Place::Land,      Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,      Place::Grass,
     Place::Wall, Place::Land, Place::Land,  Place::Land,      Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land,  Place::Wall,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Wall,  Place::Grass, Place::Grass, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Grass,
     Place::Grass, Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Grass, Place::Grass, Place::Grass, Place::Grass, Place::Wall,
     Place::Wall,  Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land, Place::Land,  Place::Wall, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Wall,  Place::Wall, Place::Grass, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land, Place::Land,
     Place::Grass, Place::Wall, Place::Wall,  Place::Wall, Place::Wall,
     Place::Grass, Place::Land, Place::Land,  Place::Land, Place::Wall},
    {Place::Wall, Place::Land,  Place::Wall,  Place::Wall, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Wall, Place::Grass, Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land, Place::Grass, Place::Grass, Place::Wall, Place::Grass,
     Place::Land, Place::Land,  Place::Land,  Place::Land, Place::Wall},
    {Place::Wall,   Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land,   Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Window, Place::Wall, Place::Wall, Place::Wall, Place::Land,
     Place::Land,   Place::Land, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall,   Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Wall,   Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall,   Place::Wall, Place::Wall, Place::Wall, Place::Window,
     Place::Wall,   Place::Wall, Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,   Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Wall, Place::Land,
     Place::Land, Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land,      Place::Window, Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,   Place::Land, Place::Wall,
     Place::Land, Place::Land,      Place::Land,   Place::Wall, Place::Door3,
     Place::Wall, Place::Wall,      Place::Wall,   Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Wall,   Place::Wall, Place::Wall,
     Place::Land, Place::ClassRoom, Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,   Place::Land, Place::Door6,
     Place::Land, Place::Land,      Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,      Place::Land,   Place::Land, Place::Wall},
    {Place::Wall, Place::Land,  Place::Wall, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Wall,
     Place::Wall, Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Chest, Place::Wall, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Wall,
     Place::Wall, Place::Wall,  Place::Wall, Place::Grass, Place::Grass,
     Place::Land, Place::Land,  Place::Land, Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land,  Place::Wall,      Place::Grass, Place::Land,
     Place::Land,  Place::Land,  Place::ClassRoom, Place::Land,  Place::Land,
     Place::Wall,  Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land,  Place::Wall,  Place::Wall,      Place::Grass, Place::Grass,
     Place::Grass, Place::Grass, Place::Land,      Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Wall,      Place::Wall,  Place::Grass,
     Place::Land,  Place::Land,  Place::Land,      Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Wall,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Grass, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall, Place::Wall, Place::Wall, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall, Place::Wall,
     Place::Grass, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Wall,  Place::Wall,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Wall,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Window, Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Grass,
     Place::Grass, Place::Grass, Place::Wall,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,   Place::Land, Place::Wall,
     Place::Grass, Place::Land,  Place::Land,   Place::Land, Place::Wall},
    {Place::Wall,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,      Place::Land, Place::Land, Place::Land, Place::Land,
     Place::ClassRoom, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Grass,     Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Grass,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall,  Place::Wall,  Place::Land,  Place::Land,  Place::Grass,
     Place::Grass, Place::Wall,  Place::Grass, Place::Grass, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Door6, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Wall,  Place::Wall,  Place::Wall,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Wall,  Place::Wall,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Wall,  Place::Chest,  Place::Land, Place::Land,
     Place::Land, Place::Grass, Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Wall,  Place::Wall,   Place::Wall, Place::Wall,
     Place::Wall, Place::Wall,  Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,   Place::Land, Place::Land,
     Place::Land, Place::Land,  Place::Land,   Place::Wall, Place::Wall,
     Place::Wall, Place::Wall,  Place::Window, Place::Wall, Place::Land,
     Place::Land, Place::Land,  Place::Land,   Place::Land, Place::Wall},
    {Place::Wall, Place::Wall, Place::Wall, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Wall, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Grass,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land, Place::Land, Place::Land, Place::Land, Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Wall,  Place::Wall,
     Place::Wall,  Place::Wall,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Wall,  Place::Land,  Place::Land,
     Place::Grass, Place::Grass, Place::Grass, Place::Grass, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Land, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land,      Place::Wall,  Place::Grass,
     Place::Land, Place::Land, Place::ClassRoom, Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Grass,     Place::Grass, Place::Grass,
     Place::Land, Place::Land, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Wall,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land, Place::Wall,      Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall, Place::Grass,
     Place::Grass, Place::Land, Place::Land, Place::Land, Place::Wall,
     Place::Wall,  Place::Wall, Place::Land, Place::Wall, Place::Land,
     Place::Land,  Place::Wall, Place::Wall, Place::Wall, Place::Grass,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Wall, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Land, Place::Land,
     Place::Land,  Place::Land, Place::Wall, Place::Land, Place::Wall},
    {Place::Wall,  Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Wall, Place::Wall,
     Place::Wall,  Place::Wall,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Land,  Place::Land,  Place::Land,  Place::Wall, Place::Land,
     Place::Wall,  Place::Grass, Place::Grass, Place::Wall, Place::Grass,
     Place::Land,  Place::Land,  Place::Land,  Place::Land, Place::Land,
     Place::Land,  Place::Land,  Place::Wall,  Place::Wall, Place::Wall,
     Place::Door6, Place::Wall,  Place::Wall,  Place::Wall, Place::Land,
     Place::Land,  Place::Land,  Place::Land,  Place::Land, Place::Grass,
     Place::Land,  Place::Land,  Place::Wall,  Place::Land, Place::Wall},
    {Place::Wall,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Wall,  Place::Wall,
     Place::Grass, Place::Land, Place::Grass, Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Grass, Place::Wall,
     Place::Land,  Place::Wall, Place::Wall,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Grass, Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,      Place::Wall,  Place::Grass,
     Place::Land, Place::Land,  Place::Grass,     Place::Wall,  Place::Land,
     Place::Land, Place::Land,  Place::Land,      Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::ClassRoom, Place::Land,  Place::Land,
     Place::Land, Place::Land,  Place::Land,      Place::Wall,  Place::Land,
     Place::Land, Place::Land,  Place::Land,      Place::Grass, Place::Wall,
     Place::Wall, Place::Wall,  Place::Land,      Place::Land,  Place::Wall},
    {Place::Wall, Place::Land, Place::Land,  Place::Land,   Place::Land,
     Place::Land, Place::Wall, Place::Grass, Place::Grass,  Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Grass,  Place::Land,
     Place::Land, Place::Land, Place::Grass, Place::Wall,   Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Window, Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,   Place::Land,
     Place::Land, Place::Land, Place::Land,  Place::Land,   Place::Wall},
    {Place::Wall, Place::Land,   Place::Land,  Place::Land,  Place::Land,
     Place::Wall, Place::Wall,   Place::Grass, Place::Grass, Place::Grass,
     Place::Land, Place::Land,   Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,   Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,   Place::Grass, Place::Wall,  Place::Wall,
     Place::Wall, Place::Window, Place::Wall,  Place::Wall,  Place::Land,
     Place::Land, Place::Land,   Place::Land,  Place::Land,  Place::Land,
     Place::Land, Place::Land,   Place::Land,  Place::Wall,  Place::Land,
     Place::Land, Place::Land,   Place::Land,  Place::Land,  Place::Land,
     Place::Gate, Place::Land,   Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land, Place::Land, Place::Land,  Place::Wall,
     Place::Wall,  Place::Wall, Place::Wall, Place::Wall,  Place::Grass,
     Place::Land,  Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall,  Place::Wall,
     Place::Grass, Place::Land, Place::Land, Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall,  Place::Wall,
     Place::Wall,  Place::Wall, Place::Wall, Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Land, Place::Grass, Place::Wall,
     Place::Wall,  Place::Land, Place::Land, Place::Land,  Place::Wall},
    {Place::Wall,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Wall,  Place::Wall,  Place::Grass,
     Place::Chest, Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Land,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Land,  Place::Wall,  Place::Land,
     Place::Land,  Place::Land, Place::Grass, Place::Grass, Place::Wall,
     Place::Wall,  Place::Land, Place::Land,  Place::Land,  Place::Wall},
    {Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall,
     Place::Wall, Place::Wall, Place::Wall, Place::Wall, Place::Wall},
};




std::vector<Point> classrooms = {
    {18, 5},  {30, 7},  {22, 18},           // Building 3
    {8, 31},  {10, 38}, {19, 41},           // Building 5
    {28, 26}, {33, 40}, {40, 12}, {44, 32}  // Building 6
};


void initmap()
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            map[i][j].x = i;
            map[i][j].y = j;
            switch (Map[i][j])
            {
            case Place::Wall:
                map[i][j].cost = 1000;
                break;
            default:
                map[i][j].cost = 1;
                break;
            }
        }
    }
}

void findPath(Point begin, Point end)
{
    path.clear();
    iPath = 0;
    std::vector<Point> extending;
    std::vector<Point> unextended;
    std::vector<Point> extended;
    int min = 0;

    extended.push_back(begin);

    for (int i = 0; i < 4; i++)
    {
        if (extended[0].x + direction[i][0] >= 0 && extended[0].x + direction[i][0] < x && extended[0].y + direction[i][1] < y && extended[0].y + direction[i][1] >= 0)
        {
            map[extended[0].x + direction[i][0]][extended[0].y + direction[i][1]].sumcost = extended[0].sumcost + map[extended[0].x + direction[i][0]][extended[0].y + direction[i][1]].cost;
            map[extended[0].x + direction[i][0]][extended[0].y + direction[i][1]].prex = begin.x;
            map[extended[0].x + direction[i][0]][extended[0].y + direction[i][1]].prey = begin.y;

            extending.push_back(map[extended[0].x + direction[i][0]][extended[0].y + direction[i][1]]);

        }
    }

    while (!extending.empty())
    {

        if (extending.size() > 1)
        {
            min = 0;
            for (int i = 1; i < extending.size(); i++)
            {
                //find the point in extending whose sumcost is least
                if (extending[i].sumcost < extending[min].sumcost)
                {
                    min = i;
                }
            }
        }
        else
        {
            min = 0;
        }
        int flag = 0;
        int flag1 = 0;
        for (int i = 0; i < 4; i++)
        {
            if (extending[min].x + direction[i][0] >= 0 && extending[min].x + direction[i][0] < x && extending[min].y + direction[i][1] < y && extending[min].y + direction[i][1] >= 0)
            {
                for (int j = 0; j < extended.size(); j++)
                {
                    if (extending[min].x + direction[i][0] == extended[j].x && extending[min].y + direction[i][1] == extended[j].y)
                    {

                        flag++;               //the point has added in extended
                    }
                }

                if (flag == 0)				//the point is not extended
                {
                    for (int k = 0; k < extending.size(); k++)
                    {
                        if (map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].x == extending[k].x && map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].y == extending[k].y)
                        {
                            if (extending[k].sumcost > extending[min].sumcost + extending[k].cost)
                            {
                                extending[k].sumcost = extending[min].sumcost + extending[k].cost;
                                extending[k].prex = extending[min].x;
                                extending[k].prey = extending[min].y;
                            }
                            flag1++;
                        }
                    }
                    if (flag1 == 0)
                    {
                        map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].sumcost = extending[min].sumcost + map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].cost;
                        map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].prex = extending[min].x;
                        map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]].prey = extending[min].y;
                        extending.push_back(map[extending[min].x + direction[i][0]][extending[min].y + direction[i][1]]);
                    }

                }
            }
            flag = 0;
            flag1 = 0;
        }
        extended.push_back(extending[min]);
        extending.erase(extending.begin() + min);
    }


    Point temp;
    for (int i = 0; i < extended.size(); i++)
    {
        if (extended[i].x == end.x && extended[i].y == end.y)
        {
            temp = extended[i];
            while (1)
            {
                if (temp.x != begin.x || temp.y != begin.y)
                {
                    path.push_back(temp);
                    temp = map[temp.prex][temp.prey];
                }
                else
                {
                    path.push_back(begin);
                    return;
                }
            }
            while (temp.prex != begin.x || temp.prey != begin.y);
            {

            }
            path.push_back(begin);

        }
    }

}


Point GetNearestClass(int xCell, int yCell)
{
    Point res, now{ xCell, yCell };
    int32_t min = 0x3f3f3f3f;
    for (int i = 0; i < classrooms.size(); ++i)
    {
        Point temp = classrooms[i];
        if (!hasLearned[i])
        {
            int32_t dist = temp - now;
            if (dist < min)
            {
                min = dist;
                res = temp;
            }
        }
    }
    return res;
}

Point GoToClass(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto target = GetNearestClass(api.GridToCell(self->x), api.GridToCell(self->y));

    double deg = atan2(target.y - api.GridToCell(self->y), target.x - api.GridToCell(self->x));
    api.Move(Constants::frameDuration, deg);
    return target;
}

void EscapeFromTricker(IStudentAPI& api)
{
    auto tricker = api.GetTrickers();
    if (!tricker.size())  return;

    auto player = api.GetSelfInfo();
    double deg = atan2(player->y - tricker[0]->y, player->x - tricker[0]->x);
    int xCell = api.GridToCell(player->x), yCell = api.GridToCell(player->y);
    auto gridType = api.GetPlaceType(xCell, yCell);

    //switch (gridType)
    //{
    //    case THUAI6::PlaceType::Land:
    //        api.Move(Constants::frameDuration, deg);
    //        break;
    //} 

    api.Move(Constants::frameDuration, deg);
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
        
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作

    if (api.GetFrameCount() == 1) {
        initmap();    // 初始化地图
        api.Print("地图初始化完毕");
    }

    EscapeFromTricker(api);
    Point classroom = GoToClass(api);
    api.StartLearning();
    if (api.GetClassroomProgress(classroom.x, classroom.y) == Constants::maxClassroomProgress)
    {
        for (int i = 0; i < classrooms.size(); ++i)
        {
            if (classrooms[i].x == classroom.x && classrooms[i].y == classroom.y)
            {
                hasLearned[i] = true;
                for (int j = 0; j < 4; ++j)
                {
                    api.SendMessage(j, fmt::format("{}", i));
                }
                break;
            }
        }
    }

    if (api.HaveMessage())
    {
        auto msg = api.GetMessage();
        int classroomID = msg.second[0] - '0';
        hasLearned[classroomID] = true;
    }

    api.UseSkill(0);
    api.UseSkill(1);
    api.UseSkill(2);
}



/*鬼相关函数*/ 

// 视野内有人
int isStudentinView(ITrickerAPI& api) {
    auto info = api.GetStudents();
    return (info.size() != 0);
}

void avoidObstacles(ITrickerAPI& api, double dirc) {
    const int selfRadius = 450;
    const int moveStep = 10; // 待定
    auto selfInfo = api.GetSelfInfo();
    auto selfX = selfInfo->x;
    auto selfY = selfInfo->y;

    // 定义左右点
    int leftPointX = selfX - (int)(selfRadius * sin(dirc));
    int leftPointY = selfY + (int)(selfRadius * cos(dirc));
    int rightPointX = selfX + (int)(selfRadius * sin(dirc));
    int rightPointY = selfY - (int)(selfRadius * cos(dirc));

    auto gridX = api.GridToCell(leftPointX + (int)(1000 * cos(dirc)));
    auto gridY = api.GridToCell(leftPointY + (int)(1000 * sin(dirc)));
    if (api.GetPlaceType(gridX, gridY) == THUAI6::PlaceType::Wall) {
        api.Move(moveStep, dirc - M_PI / 2);
        avoidObstacles(api, dirc + M_PI / 10);
        return;
    }
    gridX = api.GridToCell(rightPointX + (int)(1000 * cos(dirc)));
    gridY = api.GridToCell(rightPointY + (int)(1000 * sin(dirc)));
    if (api.GetPlaceType(gridX, gridY) == THUAI6::PlaceType::Wall) {
        api.Move(moveStep, dirc + M_PI / 2);
        avoidObstacles(api, dirc + M_PI / 10);
    }
    return;
}

void AI::play(ITrickerAPI& api)
{
    auto selfInfo = api.GetSelfInfo();
    auto myPos = Point(selfInfo->x, selfInfo->y);
    api.PrintSelfInfo();
    if (isStudentinView(api)) {

        // 追击
        auto studentInfo = api.GetStudents();
        auto selfInfo = api.GetSelfInfo();
        auto dx = studentInfo[0]->x - selfInfo->x;
        auto dy = studentInfo[0]->y - selfInfo->y;
        auto dirc = atan2(dy, dx);

        avoidObstacles(api, dirc);
        api.Move(moveStep, dirc);
        double distance = sqrt(dx * dx + dy + dy);
        if (distance < Constants::basicAttackShortRange) {
            api.Attack(dirc);
        }
    }

    api.UseSkill(0);
    api.UseSkill(1);
    api.UseSkill(2);
}