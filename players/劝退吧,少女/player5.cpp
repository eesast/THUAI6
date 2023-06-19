#include <vector>
#include <queue>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <cmath>
#define PI 3.141592653589793
using namespace std;

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义
extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;

// 可以在AI.cpp内部声明变量与函数

// 地图大小
const int N = 50;

//人物半径
const int R = 400;

// 方格大小
const int Ss = 1000;

// Square类
class Square {
public:
    int x, y;
    Square() {}
    Square(int _x, int _y) : x(_x), y(_y) {}
    bool operator==(const Square& s) const {
        return x == s.x && y == s.y;
    }
    bool operator!=(const Square& s) const {
        return x != s.x || y != s.y;
    }
};

// 哈希函数
namespace std {
    template<>
    struct hash<Square> {
        size_t operator()(const Square& s) const {
            return s.x * 100 + s.y;
        }
    };
}

// Edge类
class Edge {
public:
    Square start, end;
    int weight;
    Edge(Square _start, Square _end, int _weight) : start(_start), end(_end), weight(_weight) {}
};

// 比较函数，用于优先队列
struct cmp {
    bool operator() (pair<Square, int>& a, pair<Square, int>& b) {
        return a.second > b.second;
    }
};

int student_state[4];
//state为0表示未沉迷,state为1表示沉迷,state为2表示死亡

//one_move全局参量
int all_nextx;
int all_nexty;
std::chrono::milliseconds::rep time1;

//全局运行次数
int all_times = 0;

//学生和捣蛋鬼状态信息
int student_condition = 1;
int tricker_condition = 1;

int Maping[50][50];            //A*使用的数据，0代表可以走，1代表不可以走，实时更新的
int Maptypeing[50][50];   //地图详细数据，1代表空地，2代表草，3代表窗子，4代表教学楼门，5代表隐藏校门，6代表校门，7代表作业，8代表箱子，9代表墙

//格点坐标类,构造或设置时之间输入坐标即可，会自动识别格点坐标或xy坐标,设置函数是Set
class SquareOut
{
public:
    SquareOut(const SquareOut& s)
    {
        x = s.x;
        y = s.y;
        Cellx = s.Cellx;
        Celly = s.Celly;
    }
    SquareOut(int a = 0, int b = 0)
    {
        if (a <= 50)
        {
            Cellx = a;
            Celly = b;
            x = 1000 * a + 500;
            y = 1000 * b + 500;
        }
        else
        {
            x = a;
            y = b;
            Cellx = a / 1000;
            Celly = b / 1000;
        }
    }
    void Set(const SquareOut& s)
    {
        x = s.x;
        y = s.y;
        Cellx = s.Cellx;
        Celly = s.Celly;
    }
    void Set(int a, int b)
    {
        if (a <= 50)
        {
            Cellx = a;
            Celly = b;
            x = 1000 * a + 500;
            y = 1000 * b + 500;
        }
        else
        {
            x = a;
            y = b;
            Cellx = a / 1000;
            Celly = b / 1000;
        }
    }
    bool operator==(const SquareOut& s) const {
        return x == s.x && y == s.y && Cellx == s.Cellx && Celly == s.Celly;
    }
    int x;
    int y;
    int Cellx;
    int Celly;
};

//存储学生位置，S[i]表示学生i,对捣蛋鬼而言是学生最后位置
SquareOut S[4];
//存储捣蛋鬼位置，对学生而言是捣蛋鬼最后出现位置
SquareOut T;
SquareOut CopyT;
int copy_aimleave;

//校门格点类
class Gate
{
public:
    Gate(int x, int y, int a, bool b)
    {
        cellx = x;
        celly = y;
        completeness = a;
        isdown = b;
    }
    void Set(int x, int y, int a, bool b)
    {
        cellx = x;
        celly = y;
        completeness = a;
        isdown = b;
    }
    void Set(int a)
    {
        completeness = a;
        if (a == 18000) isdown = true;
    }
    int cellx;
    int celly;
    int completeness;
    bool isdown;
};

//储存校门位置
std::vector<Gate> gate;

//隐藏校门类
class HideGate
{
public:
    HideGate(int a, int b, int c)
    {
        cellx = a;
        celly = b;
        condition = c;
    }
    void Set(int a, int b, int c)
    {
        cellx = a;
        celly = b;
        condition = c;
    }
    void Set(int a)
    {
        condition = a;
    }
    int cellx;
    int celly;
    int condition;//0是不在视野或未刷新，1是已刷新未开启，2是已开启
};

//隐藏校门
std::vector<HideGate> hidegate;

//作业、箱子、教室门格点类
class Task
{
public:
    Task(int x = 0, int y = 0, int a = 0, bool b = 0)
    {
        cellx = x;
        celly = y;
        completeness = a;
        isdown = b;
    }
    void Set(int x, int y, int a, bool b)
    {
        cellx = x;
        celly = y;
        completeness = a;
        isdown = b;
    }
    void Set(int a)
    {
        completeness = a;
        if (a == 10000000) isdown = true;
    }
    int cellx;//X坐标
    int celly;//Y坐标
    int completeness;//完成、开启情况
    bool isdown;
};

//作业存储器，每个对应一个作业。
std::vector<Task> homework;
//箱子存储器，每个对应一个箱子。
std::vector<Task> box;
//教室门储存器 
std::vector<Task> door;

//地图数据下载
void MapLoad_first(ITrickerAPI& api)
{
    auto maping = api.GetFullMap();
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            switch (maping[i][j])
            {
            case THUAI6::PlaceType::Land://空地
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 1;
                break;
            case THUAI6::PlaceType::Grass://草地
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 2;
                break;
            case THUAI6::PlaceType::Wall://墙
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 9;
                break;
            case THUAI6::PlaceType::Chest://箱子
                box.emplace_back(Task(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 8;
                break;
            case THUAI6::PlaceType::ClassRoom://作业
                homework.emplace_back(Task(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 7;
                break;
            case THUAI6::PlaceType::Door3://三教门
            case THUAI6::PlaceType::Door5://五教门
            case THUAI6::PlaceType::Door6://六教门
                door.emplace_back(Task(i, j, 10000000, 1));
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 4;
                break;
            case THUAI6::PlaceType::Gate://校门
                gate.emplace_back(Gate(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 6;
                break;
            case THUAI6::PlaceType::HiddenGate://隐藏校门
                hidegate.emplace_back(HideGate(i, j, 0));
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 5;
                break;
            case THUAI6::PlaceType::Window://窗子
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 3;
                break;
            }
        }
    }
}
void MapLoad_first(IStudentAPI& api)
{
    auto maping = api.GetFullMap();
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            switch (maping[i][j])
            {
            case THUAI6::PlaceType::Land://空地
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 1;
                break;
            case THUAI6::PlaceType::Grass://草地
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 2;
                break;
            case THUAI6::PlaceType::Wall://墙
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 9;
                break;
            case THUAI6::PlaceType::Chest://箱子
                box.emplace_back(Task(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 8;
                break;
            case THUAI6::PlaceType::ClassRoom://作业
                homework.emplace_back(Task(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 7;
                break;
            case THUAI6::PlaceType::Door3://三教门
            case THUAI6::PlaceType::Door5://五教门
            case THUAI6::PlaceType::Door6://六教门
                door.emplace_back(Task(i, j, 10000000, 1));
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 4;
                break;
            case THUAI6::PlaceType::Gate://校门
                gate.emplace_back(Gate(i, j, 0, 0));
                Maping[i][j] = 1;//不可以走
                Maptypeing[i][j] = 6;
                break;
            case THUAI6::PlaceType::HiddenGate://隐藏校门
                hidegate.emplace_back(HideGate(i, j, 0));
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 5;
                break;
            case THUAI6::PlaceType::Window://窗子
                Maping[i][j] = 0;//可以走
                Maptypeing[i][j] = 3;
                break;
            }
        }
    }
}

//地图数据更新
void MapUpDate(int** a, int cellx, int celly, int b)
{
    a[cellx][celly] = b;
}

//距离计算器，计算两点之间距离平方,参数为x，y变化值,重载
int distance2(int a, int b)
{
    return a * a + b * b;
}
double distance2(double a, double b)
{
    return a * a + b * b;
}
//计算两点间距离
double distance1(int x1, int x2, int y1, int y2)
{
    double distance;
    distance = sqrt((double)distance2(x1 - x2, y1 - y2));
    return distance;
}
double distance1(int x1, double x2, int y1, double y2)
{
    double distance;
    distance = sqrt((double)distance2(x1 - x2, y1 - y2));
    return distance;
}
double distance1(double x1, double x2, double y1, double y2)
{
    double distance;
    distance = sqrt((double)distance2(x1 - x2, y1 - y2));
    return distance;
}

//Manhattan距离计算器，计算两点之间曼哈顿距离，参量x，y为变化值,重载
int Manhattan(int a, int b)
{
    return abs(a) + abs(b);
}
double Manhattan(double a, double b)
{
    return abs(a) + abs(b);
}

//角度计算器，符合本游戏的计算,第一个和第二个参量分别为x，y的预期变化值，考虑正负，重载
double angle(int x, int y)
{
    if (x == 0)
    {
        if (y > 0) return PI / 2;
        else return PI * 3 / 2;
    }
    if (y == 0)
    {
        if (x > 0) return 0;
        else return PI;
    }
    if (x > 0 && y > 0) return atan(y / x);
    else if (x < 0) return atan(y / x) + PI;
    else return atan(y / x) + PI * 2;
}
double angle(double x, double y)
{
    if (x == 0)
    {
        if (y > 0) return PI / 2;
        else return PI * 3 / 2;
    }
    if (y == 0)
    {
        if (x > 0) return 0;
        else return PI;
    }
    if (x > 0 && y > 0) return atan(y / x);
    else if (x < 0) return atan(y / x) + PI;
    else return atan(y / x) + PI * 2;
}

//安全判断器，判断能否处于某个位置,安全返回真，不能处于返回假,重载
bool Issafe(double nextx, double nexty, IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    auto trickers = api.GetStudents();

    //不靠近人
    for (int i = 0; i < students.size(); i++)
    {
        if (students[i]->playerID != self->playerID)
        {
            if (distance1(students[i]->x, nextx, students[i]->y, nexty) <= 1000)
                return false;
        }
    }
    if (self->playerID != trickers[0]->playerID)
    {
        if (distance1(trickers[0]->x, nextx, trickers[0]->y, nexty) <= 1000)
            return false;
    }

    //不撞墙
    int Cellx = nextx / 1000;
    int Celly = nexty / 1000;
    double Extrax = nextx - 1000 * Cellx;
    double Extray = nexty - 1000 * Celly;
    if (Extrax >= 600 && Maping[Cellx + 1][Celly] == 1)
        return false;
    if (Extrax <= 400 && Maping[Cellx - 1][Celly] == 1)
        return false;
    if (Extray >= 600 && Maping[Cellx][Celly + 1] == 1)
        return false;
    if (Extray <= 400 && Maping[Cellx][Celly - 1] == 1)
        return false;
    if (Extrax >= 600 && Extray >= 600 && Maping[Cellx + 1][Celly + 1] == 1)
        return false;
    if (Extrax >= 600 && Extray <= 400 && Maping[Cellx + 1][Celly - 1] == 1)
        return false;
    if (Extrax <= 400 && Extray >= 600 && Maping[Cellx - 1][Celly + 1] == 1)
        return false;
    if (Extrax <= 400 && Extray <= 400 && Maping[Cellx - 1][Celly - 1] == 1)
        return false;
    return true;
}
bool Issafe(int nextx, int nexty, IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    auto trickers = api.GetStudents();

    //不靠近人
    for (int i = 0; i < students.size(); i++)
    {
        if (students[i]->playerID != self->playerID)
        {
            if (distance1(students[i]->x, nextx, students[i]->y, nexty) <= 1000)
                return false;
        }
    }
    if (self->playerID != trickers[0]->playerID)
    {
        if (distance1(trickers[0]->x, nextx, trickers[0]->y, nexty) <= 1000)
            return false;
    }

    //不撞墙
    int Cellx = nextx / 1000;
    int Celly = nexty / 1000;
    double Extrax = nextx - 1000 * Cellx;
    double Extray = nexty - 1000 * Celly;
    if (Extrax >= 600 && Maping[Cellx + 1][Celly] == 1)
        return false;
    if (Extrax <= 400 && Maping[Cellx - 1][Celly] == 1)
        return false;
    if (Extray >= 600 && Maping[Cellx][Celly + 1] == 1)
        return false;
    if (Extray <= 400 && Maping[Cellx][Celly - 1] == 1)
        return false;
    if (Extrax >= 600 && Extray >= 600 && Maping[Cellx + 1][Celly + 1] == 1)
        return false;
    if (Extrax >= 600 && Extray <= 400 && Maping[Cellx + 1][Celly - 1] == 1)
        return false;
    if (Extrax <= 400 && Extray >= 600 && Maping[Cellx - 1][Celly + 1] == 1)
        return false;
    if (Extrax <= 400 && Extray <= 400 && Maping[Cellx - 1][Celly - 1] == 1)
        return false;
    return true;
}
bool Issafe(double nextx, double nexty, ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();

    //不靠近人
    for (int i = 0; i < students.size(); i++)
    {
        if (distance1(students[i]->x, nextx, students[i]->y, nexty) <= 1000)
            return false;
    }

    //不撞墙
    int Cellx = nextx / 1000;
    int Celly = nexty / 1000;
    double Extrax = nextx - 1000 * Cellx;
    double Extray = nexty - 1000 * Celly;
    if (Extrax >= 600 && Maping[Cellx + 1][Celly] == 1)
        return false;
    if (Extrax <= 400 && Maping[Cellx - 1][Celly] == 1)
        return false;
    if (Extray >= 600 && Maping[Cellx][Celly + 1] == 1)
        return false;
    if (Extray <= 400 && Maping[Cellx][Celly - 1] == 1)
        return false;
    if (Extrax >= 600 && Extray >= 600 && Maping[Cellx + 1][Celly + 1] == 1)
        return false;
    if (Extrax >= 600 && Extray <= 400 && Maping[Cellx + 1][Celly - 1] == 1)
        return false;
    if (Extrax <= 400 && Extray >= 600 && Maping[Cellx - 1][Celly + 1] == 1)
        return false;
    if (Extrax <= 400 && Extray <= 400 && Maping[Cellx - 1][Celly - 1] == 1)
        return false;
    return true;
}
bool Issafe(int nextx, int nexty, ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();

    //不靠近人
    for (int i = 0; i < students.size(); i++)
    {
        if (distance1(students[i]->x, nextx, students[i]->y, nexty) <= 1000)
            return false;
    }

    //不撞墙
    int Cellx = nextx / 1000;
    int Celly = nexty / 1000;
    double Extrax = nextx - 1000 * Cellx;
    double Extray = nexty - 1000 * Celly;
    if (Extrax >= 600 && Maping[Cellx + 1][Celly] == 1)
        return false;
    if (Extrax <= 400 && Maping[Cellx - 1][Celly] == 1)
        return false;
    if (Extray >= 600 && Maping[Cellx][Celly + 1] == 1)
        return false;
    if (Extray <= 400 && Maping[Cellx][Celly - 1] == 1)
        return false;
    if (Extrax >= 600 && Extray >= 600 && Maping[Cellx + 1][Celly + 1] == 1)
        return false;
    if (Extrax >= 600 && Extray <= 400 && Maping[Cellx + 1][Celly - 1] == 1)
        return false;
    if (Extrax <= 400 && Extray >= 600 && Maping[Cellx - 1][Celly + 1] == 1)
        return false;
    if (Extrax <= 400 && Extray <= 400 && Maping[Cellx - 1][Celly - 1] == 1)
        return false;
    return true;
}
bool is_in_self_condition(int cellx, int celly)
{
    for (int i = 0; i < 4; i++)
    {
        if (S[i].Cellx == cellx && S[i].Celly == celly)
            return false;
    }
    if (T.Cellx == cellx && T.Celly == celly)
        return false;
    return true;
}

// 定义dwa,给定目标，返回最佳移动的角度。
double student_dwa(int aimx, int aimy, IStudentAPI& api)
{
    //信息修正，可以从Cell坐标到x，y坐标
    if (aimx <= 50) aimx = 1000 * aimx + 500;
    if (aimy <= 50) aimy = aimy * 1000 + 500;

    //信息读取与保存
    auto now = api.GetSelfInfo();
    auto trickers = api.GetTrickers();
    auto students = api.GetStudents();
    auto bullets = api.GetBullets();
    auto nowx = now->x;
    auto nowy = now->y;
    auto speed = now->speed;

    std::vector<double> allowdirection;
    for (int direction = 0; direction < 180; direction++)
    {
        double realdirection = PI * 2 * direction / 180;
        double nextx[5] = {
            nowx + speed * 0.01 * cos(realdirection),
            nowx + speed * 0.02 * cos(realdirection),
            nowx + speed * 0.03 * cos(realdirection),
            nowx + speed * 0.04 * cos(realdirection),
            nowx + speed * 0.05 * cos(realdirection)
        };
        double nexty[5] = {
            nowy + speed * 0.01 * sin(realdirection),
            nowy + speed * 0.02 * sin(realdirection),
            nowy + speed * 0.03 * sin(realdirection),
            nowy + speed * 0.04 * sin(realdirection),
            nowy + speed * 0.05 * sin(realdirection)
        };
        for (int i = 0; i < 5; i++)
        {
            if (Issafe(nextx[i], nexty[i], api))
                allowdirection.emplace_back(realdirection);
        }
    }
    if (allowdirection.size() == 0)
        return 0;

    double bestangle = allowdirection[0];//最佳角度
    double maxgain = 0;//最大获得值
    double gain;//获得值
    for (int i = 0; i < (int)allowdirection.size(); ++i)
    {
        gain = 0;
        double realdirection = allowdirection[i];
        double nextx = nowx + speed * 0.05 * cos(realdirection);
        double nexty = nowy + speed * 0.05 * sin(realdirection);
        gain = distance2(aimx - nowx, aimy - nowy) - distance2(aimx - nextx, aimy - nexty);
        if (!api.GetTrickers().empty())
        {
            for (int i = 0; i < trickers.size(); ++i)
                gain += 2 * distance2(trickers[i]->x - nextx, trickers[i]->y - nexty);
        }
        if (!api.GetBullets().empty())
        {
            for (int i = 0; i < bullets.size(); ++i)
                gain += 10 * Manhattan(bullets[i]->x - nextx, bullets[i]->y - nexty);
        }
        if (gain > maxgain)
        {
            maxgain = gain;
            bestangle = realdirection;
        }
    }
    return bestangle;
}
double tricker_dwa(int aimx, int aimy, ITrickerAPI& api)
{
    //信息修正，可以从Cell坐标到x，y坐标
    if (aimx <= 50) aimx = 1000 * aimx + 500;
    if (aimy <= 50) aimy = aimy * 1000 + 500;

    //信息采集
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    auto nowx = self->x;
    auto nowy = self->y;
    auto speed = self->speed;

    std::vector<double> allowdirection;
    for (int direction = 0; direction < 180; direction++)
    {
        double realdirection = PI * 2 * direction / 180;
        double nextx[5] = {
            nowx + speed * 0.01 * cos(realdirection),
            nowx + speed * 0.02 * cos(realdirection),
            nowx + speed * 0.03 * cos(realdirection),
            nowx + speed * 0.04 * cos(realdirection),
            nowx + speed * 0.05 * cos(realdirection)
        };
        double nexty[5] = {
            nowy + speed * 0.01 * sin(realdirection),
            nowy + speed * 0.02 * sin(realdirection),
            nowy + speed * 0.03 * sin(realdirection),
            nowy + speed * 0.04 * sin(realdirection),
            nowy + speed * 0.05 * sin(realdirection)
        };
        for (int i = 0; i < 5; i++)
        {
            if (Issafe(nextx[i], nexty[i], api))
                allowdirection.emplace_back(realdirection);
        }
    }
    if (allowdirection.size() == 0)
        return 0;

    double bestangle = allowdirection[0];//最佳角度
    double maxgain = 0;//最大获得值
    double gain;//获得值
    for (int i = 0; i < (int)allowdirection.size(); ++i)
    {
        gain = 0;
        double realdirection = allowdirection[i];
        double nextx = nowx + speed * 0.05 * cos(realdirection);
        double nexty = nowy + speed * 0.05 * sin(realdirection);
        gain = distance2(aimx - nowx, aimy - nowy) - distance2(aimx - nextx, aimy - nexty);
        for (int i = 0; i < students.size(); i++)
        {
            if (students[i]->playerState != THUAI6::PlayerState::Addicted)
            {
                gain += distance2(students[i]->x - nowx, students[i]->y - nowy) - distance2(students[i]->x - nextx, students[i]->y - nexty);
            }
        }
        if (gain > maxgain)
        {
            maxgain = gain;
            bestangle = realdirection;
        }
    }
    return bestangle;
}

//沉睡函数，_后为单位，参数为时间.
void sleep_ms(int i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(i));
}
void sleep_s(int i)
{
    std::this_thread::sleep_for(std::chrono::seconds(i));
}

// 计算启发函数
int heuristic(int x, int y, int ex, int ey) {
    return abs(x - ex) + abs(y - ey);
}

//碰撞箱判断，完善路径
vector<Square> Repath(vector<Square>& path)
{
    int PersonalMap[50][50] = { 0 };
    int RealPersonalMap[50][50];
    for (int i = 0; i < 4; i++)
    {
        PersonalMap[S[i].Cellx][S[i].Celly] = 1;
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            RealPersonalMap[i][j] = Maping[i][j] + PersonalMap[i][j];
        }
    }

    vector<Square>::iterator squ = path.begin();
    while (squ + 1 != path.end())
    {
        if ((abs(squ->x - (squ + 1)->x)) + (abs(squ->y - (squ + 1)->y)) == 1)
            squ++;
        else
        {
            int xd[] = { -1,  1, -1,  1 };
            int yd[] = { -1, -1,  1,  1 };
            for (int i = 0; i < 4; i++)
            {
                if (squ->x + xd[i] == (squ + 1)->x && squ->y + yd[i] == (squ + 1)->y)
                {
                    if (RealPersonalMap[squ->x + xd[i]][squ->y] == 1)
                    {
                        int Ix = squ->x;
                        int Iy = squ->y + yd[i];
                        squ = path.insert(squ + 1, Square(Ix, Iy));
                    }
                    else if (RealPersonalMap[squ->x][squ->y + yd[i]] == 1)
                    {
                        int Ix = squ->x + xd[i];
                        int Iy = squ->y;
                        squ = path.insert(squ + 1, Square(Ix, Iy));
                    }
                }
            }
            squ++;
        }
    }
    return path;
}
// A*寻路算法初步
vector<Square> AStar(Square Start, Square End)
{
    int PersonalMap[50][50] = { 0 };
    int RealPersonalMap[50][50];
    for (int i = 0; i < 4; i++)
    {
        PersonalMap[S[i].Cellx][S[i].Celly] = 1;
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            RealPersonalMap[i][j] = Maping[i][j] + PersonalMap[i][j];
        }
    }

    vector<Square> path;
    if (RealPersonalMap[Start.x][Start.y] == 1 || RealPersonalMap[End.x][End.y] == 1) {
        return path;
    }

    // 8个方向的偏移量
    int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };

    // 记录每个位置到起点的距离
    int dis[N][N];

    // 标记每个位置是否已经访问过
    bool vis[N][N];

    // 记录每个位置的前驱节点
    Square pre[N][N];

    memset(dis, 0x3f, sizeof(dis));
    memset(vis, false, sizeof(vis));

    dis[Start.x][Start.y] = 0;
    pre[Start.x][Start.y] = Start;

    priority_queue<pair<Square, int>, vector<pair<Square, int>>, cmp> pq;
    pq.push(make_pair(Start, 0));

    while (!pq.empty()) {
        Square cur = pq.top().first;
        pq.pop();

        if (cur == End) {
            while (cur != Start) {
                path.push_back(cur);
                cur = pre[cur.x][cur.y];
            }

            path.push_back(Start);
            reverse(path.begin(), path.end());
            return path;
        }

        vis[cur.x][cur.y] = true;

        for (int i = 0; i < 8; i++) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];

            if (nx < 0 || nx >= N || ny < 0 || ny >= N || RealPersonalMap[nx][ny] == 1 || vis[nx][ny]) {
                continue;
            }

            // 当前位置到起点的距离
            int ndis = dis[cur.x][cur.y] + Ss;

            // 避免穿过障碍物
            for (int j = 0; j < 8; j++) {
                int obsx = cur.x + dx[j];
                int obsy = cur.y + dy[j];

                if (obsx < 0 || obsx >= N || obsy < 0 || obsy >= N) {
                    continue;
                }

                if (Maping[obsx][obsy] == 1) {
                    // 判定圆心到障碍物中心距离是否小于半径
                    int cx = cur.x * Ss + Ss / 2;
                    int cy = cur.y * Ss + Ss / 2;

                    int obs_cx = obsx * Ss + Ss / 2;
                    int obs_cy = obsy * Ss + Ss / 2;

                    int dis = (cx - obs_cx) * (cx - obs_cx) + (cy - obs_cy) * (cy - obs_cy);

                    if (dis < R * R) {
                        goto skip;
                    }
                }
            }

            if (ndis < dis[nx][ny]) {
                dis[nx][ny] = ndis;
                pre[nx][ny] = cur;
                pq.push(make_pair(Square(nx, ny), ndis + heuristic(nx, ny, End.x, End.y)));
            }
        }

    skip:
        continue;
    }
    return path;
}
//完善碰撞箱的A*
vector<Square> A_star(Square Start, Square End)
{
    vector<Square> SQ = AStar(Start, End);
    vector<Square> KA = Repath(SQ);
    KA.emplace_back(Square(0, 0));
    return(Repath(KA));
}
vector<Square> A_aimstar(Square Start, Square End)
{
    Square s[] = {
        Square(End.x - 1,End.y - 1),
        Square(End.x - 1,End.y),
        Square(End.x - 1,End.y + 1),
        Square(End.x,End.y - 1),
        Square(End),
        Square(End.x,End.y + 1),
        Square(End.x + 1,End.y - 1),
        Square(End.x + 1,End.y),
        Square(End.x + 1,End.y + 1)
    };
    int leastsize = 1000;
    int distance = 100;
    int num;
    for (int i = 0; i < 9; i++)
    {
        if (distance > abs(Start.x - s[i].x) + abs(Start.y - s[i].y) && Maping[s[i].x][s[i].y] == 0 && is_in_self_condition(s[i].x, s[i].y))
        {
            distance = abs(Start.x - s[i].x) + abs(Start.y - s[i].y);
            num = i;
        }
    }
    return A_star(Start, s[num]);
}

vector<Square> astar_for_one_move;
int Cost(Square a, Square b) {
    vector<Square> path = A_star(a, b);
    return path.size();
}
int findIndex(vector<Square> targets, Square a);
int find(vector<int>& parent, int i);

// 最小生成树算法
vector<Edge> MinimumSpanningTree(vector<Square> targets) {
    // 初始化边列表
    vector<Edge> edges;
    for (int i = 0; i < targets.size(); i++) {
        for (int j = i + 1; j < targets.size(); j++) {
            int cost = Cost(targets[i], targets[j]);
            if (cost != 0) {
                edges.push_back(Edge(targets[i], targets[j], cost));
            }
        }
    }
    cout << "Step 1" << endl;

    // 按边权重由小到大排序
    sort(edges.begin(), edges.end(), [](Edge a, Edge b) { return a.weight < b.weight; });
    cout << "Step 2" << endl;

    // 初始化并查集
    vector<int> parent(targets.size());
    for (int i = 0; i < parent.size(); i++) {
        parent[i] = i;
    }
    cout << "Step 3" << endl;

    // Kruskal算法求最小生成树
    vector<Edge> MST;
    for (int i = 0; i < edges.size(); i++) {
        int root1 = parent[find(parent, findIndex(targets, edges[i].start))];
        int root2 = parent[find(parent, findIndex(targets, edges[i].end))];
        if (root1 == root2) {
            continue;
        }
        parent[root2] = root1;
        MST.push_back(edges[i]);
        if (MST.size() == targets.size() - 1) {
            break;
        }
        cout << "Step K" << endl;
    }
    return MST;
}

// 找出指定Square所在的targets中的下标
int findIndex(vector<Square> targets, Square a) {
    for (int i = 0; i < targets.size(); i++) {
        if (targets[i].x == a.x && targets[i].y == a.y) {
            return i;
        }
    }
    return -1;
}

// 并查集的find操作
int find(vector<int>& parent, int i) {
    if (parent[i] != i) {
        parent[i] = find(parent, parent[i]);
    }
    return parent[i];
}

vector<bool> Detection(ITrickerAPI& api);

//信息函数组
void student_sendmessage(IStudentAPI& api)
{
    //信息读取
    auto self = api.GetSelfInfo();
    auto tricker = api.GetTrickers();

    //信息发送
    //捣蛋鬼位置信息-12
    {
        if (!tricker.empty())
        {
            student_condition = 3;
            T.Set(tricker[0]->x, tricker[0]->y);
            std::string s(12, '\0');
            char* p1 = &s[0];
            *(int*)p1 = 12;
            char* p2 = &s[4];
            *(int*)p2 = tricker[0]->x;
            char* p3 = &s[8];
            *(int*)p3 = tricker[0]->y;
            for (int i = 0; i < 4; i++)
            {
                if (self->playerID != i)
                {
                    api.SendBinaryMessage(i, s);
                }
            }
        }
    }
    //作业完成情况-13
    {
        for (int i = 0; i < homework.size(); ++i)
        {
            if (api.HaveView(homework[i].cellx * 1000 + 500, homework[i].celly * 1000 + 500))
            {
                int homeworking = api.GetClassroomProgress(homework[i].cellx, homework[i].celly);
                homework[i].Set(homeworking);
                std::string s(12, '\0');
                char* p1 = &s[0];
                *(int*)p1 = 13;
                char* p2 = &s[4];
                *(int*)p2 = i;
                char* p3 = &s[8];
                *(int*)p3 = homeworking;
                for (int k = 0; k < 4; k++)
                {
                    if (self->playerID != k)
                    {
                        api.SendBinaryMessage(k, s);
                    }
                }
            }
        }
    }
    //箱子开启情况-14
    {
        for (int k = 0; k < box.size(); ++k)
        {
            if (api.HaveView(box[k].cellx * 1000 + 500, box[k].celly * 1000 + 500))
            {
                int boxing = api.GetChestProgress(box[k].cellx, box[k].celly);
                box[k].Set(boxing);
                std::string s(12, '\0');
                char* p1 = &s[0];
                *(int*)p1 = 14;
                char* p2 = &s[4];
                *(int*)p2 = k;
                char* p3 = &s[8];
                *(int*)p3 = boxing;
                for (int i = 0; i < 4; i++)
                {
                    if (self->playerID != i)
                    {
                        api.SendBinaryMessage(i, s);
                    }
                }
            }
        }
    }
    //校门开启情况-15
    {
        for (int k = 0; k < gate.size(); k++)
        {
            if (api.HaveView(gate[k].cellx * 1000 + 500, gate[k].celly * 1000 + 500))
            {
                int gating = api.GetGateProgress(gate[k].cellx, gate[k].celly);
                gate[k].Set(gating);
                std::string s(12, '\0');
                char* p1 = &s[0];
                *(int*)p1 = 15;
                char* p2 = &s[4];
                *(int*)p2 = k;
                char* p3 = &s[8];
                *(int*)p3 = gating;
                for (int i = 0; i < 4; i++)
                {
                    if (self->playerID != i)
                    {
                        api.SendBinaryMessage(i, s);
                    }
                }
            }
        }
    }
    //隐藏校门情况-16
    {
        for (int k = 0; k < hidegate.size(); k++)
        {
            if (api.HaveView(hidegate[k].cellx * 1000 + 500, hidegate[k].celly * 1000 + 500))
            {
                auto hidegating = api.GetHiddenGateState(hidegate[k].cellx, hidegate[k].celly);
                int cing;
                if (hidegating == THUAI6::HiddenGateState::Null)//不在视野或者未刷新
                {
                    cing = 0;
                    Maping[hidegate[k].cellx][hidegate[k].celly] = 0;
                }
                if (hidegating == THUAI6::HiddenGateState::Refreshed)//已刷新未开启
                {
                    cing = 1;
                    Maping[hidegate[k].cellx][hidegate[k].celly] = 1;
                }
                if (hidegating == THUAI6::HiddenGateState::Opened)//已开启
                {
                    cing = 2;
                    Maping[hidegate[k].cellx][hidegate[k].celly] = 1;
                }
                hidegate[k].Set(cing);
                std::string s(12, '\0');
                char* p1 = &s[0];
                *(int*)p1 = 16;
                char* p2 = &s[4];
                *(int*)p2 = k;
                char* p3 = &s[8];
                *(int*)p3 = cing;
                for (int i = 0; i < 4; i++)
                {
                    if (self->playerID != i)
                    {
                        api.SendBinaryMessage(i, s);
                    }
                }
            }
        }
    }
    //门状态信息-17
    {
        for (int k = 0; k < door.size(); k++)
        {
            if (api.HaveView(door[k].cellx * 1000 + 500, door[k].celly * 1000 + 500))
            {
                int dooring = api.GetDoorProgress(door[k].cellx, door[k].celly);
                door[k].Set(dooring);
                Maping[door[k].cellx][door[k].celly] = !door[k].isdown;
                std::string s(12, '\0');
                char* p1 = &s[0];
                *(int*)p1 = 17;
                char* p2 = &s[4];
                *(int*)p2 = k;
                char* p3 = &s[8];
                *(int*)p3 = dooring;
                for (int i = 0; i < 4; i++)
                {
                    if (self->playerID != i)
                    {
                        api.SendBinaryMessage(i, s);
                    }
                }
            }
        }
    }
    {
        if (self->playerState == THUAI6::PlayerState::Addicted)
        {

        }
    }
}
void student_renewmessage(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();

    for (int i = 0; i < students.size(); i++)
    {
        if (i != self->playerID)
        {
            S[students[i]->playerID].Set(students[i]->x, students[i]->y);
        }
    }

    //获取、刷新信息
    while (api.HaveMessage())
    {
        auto message = api.GetMessage();
        char* q1 = &message.second[0];
        int i1 = *(int*)q1;
        char* q2 = &message.second[4];
        int i2 = *(int*)q2;
        char* q3 = &message.second[8];
        int i3 = *(int*)q3;
        switch (i1)
        {
        case 12:
            T.Set(i2, i3);
            break;
        case 13:
            homework[i2].Set(i3);
            break;
        case 14:
            box[i2].Set(i3);
            break;
        case 15:
            gate[i2].Set(i3);
            break;
        case 16:
            hidegate[i2].Set(i3);
            if (i3 == 1 || i3 == 2)
                Maping[hidegate[i2].cellx][hidegate[i2].celly] = 1;
            break;
        case 17:
            door[i2].Set(i3);
            Maping[door[i2].cellx][door[i2].celly] = !door[i2].isdown;
            break;
        }
    }
}
void student_message(IStudentAPI& api)
{
    student_sendmessage(api);
    student_renewmessage(api);
}
void tricker_message(ITrickerAPI& api)
{
    if (api.GetSelfInfo()->trickDesire != 0)
    {
        tricker_condition = 2;
        auto students = api.GetStudents();
        if (!students.empty())
        {
            cout << "bool" << endl;
            vector<bool> wether = Detection(api);
            cout << "student found" << endl;
            auto ptr = students.begin();
            int ID;
            int flag = 0;
            while (ptr != students.end())
            {
                ID = ptr->get()->playerID;
                int sx = (ptr->get()->x);
                int sy = (ptr->get()->y);
                int tx = api.GetSelfInfo()->x;
                int ty = api.GetSelfInfo()->y;
                if (wether[ID])
                {
                    flag = 1;
                }
                if (wether[ID] && (sqrt(abs((sx - tx) * (sx - tx) + (sy - ty) * (sy - ty)))) <= 2100)
                {
                    tricker_condition = 3;
                }
                ptr++;
                cout << endl << endl << (sqrt(abs((sx - tx) * (sx - tx) + (sy - ty) * (sy - ty)))) << endl << endl;
                cout << "condition is " << tricker_condition << endl;
            }
            if (flag == 0)
                tricker_condition = 1;
            cout << "condition is " << tricker_condition << endl;
        }
        return;
    }
    else
    {
        tricker_condition = 1;
        return;
    }
}
int windowspeed(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    switch (self->studentType)
    {
    case THUAI6::StudentType::Athlete:
        return 1466;
    case THUAI6::StudentType::Robot:
        return 1;
    case THUAI6::StudentType::StraightAStudent:
        return 1018;
    case THUAI6::StudentType::Sunshine:
        return 1222;
    case THUAI6::StudentType::Teacher:
        return 1000;
    case THUAI6::StudentType::TechOtaku:
        return 1100;
    }
}
int windowspeed(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    switch (self->trickerType)
    {
    case THUAI6::TrickerType::ANoisyPerson:
        return 2540;
    case THUAI6::TrickerType::Assassin:
        return 2540;
    case THUAI6::TrickerType::Idol:
        return 2794;
    case THUAI6::TrickerType::Klee:
        return 2540;
    }
}

//技能使用
void skill_using(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto bullet = api.GetBullets();
    auto tricker = api.GetTrickers();
    switch (self->studentType)
    {
    case THUAI6::StudentType::Athlete:
        if (self->timeUntilSkillAvailable[0] == 0)
        {
            if (!tricker.empty()) { //加一个向捣蛋鬼移动。
                if (distance1(tricker[0]->x, tricker[1]->y, self->x, self->y) < 7000) { api.UseSkill(0, 0); }//向捣蛋鬼移动3s（技能持续时间）
            }
        }
        break;
    case THUAI6::StudentType::Robot:
    case THUAI6::StudentType::StraightAStudent:
        if (self->timeUntilSkillAvailable[0] == 0)
        {
            int i = self->x / 1000;
            int j = self->y / 1000;
            for (i = i - 1; i <= self->x / 1000 + 1; i++)
                for (j = j - 1; j <= self->y / 1000 + 1; j++)
                {
                    if (Maptypeing[i][j] == 7)
                    {
                        api.UseSkill(0);
                    }
                }
        }
        break;
    case THUAI6::StudentType::Sunshine:
        if (self->timeUntilSkillAvailable[0] == 0)
        {
            if (tricker[0] != NULL || bullet[0]->speed != 0)  api.UseSkill(0, 0);
        }
        break;
    case THUAI6::StudentType::Teacher:
        if (self->timeUntilSkillAvailable[0] == 0)
        {
            if (!bullet.empty() && !tricker.empty())
            {
                api.UseSkill(0);
            }
        }
        if (self->timeUntilSkillAvailable[1] == 0)
        {

            api.UseSkill(1);
        }
        break;
    case THUAI6::StudentType::TechOtaku:
        break;
    }
}
void skill_using(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    auto bullet = api.GetBullets();
    switch (self->trickerType)
    {
    case THUAI6::TrickerType::ANoisyPerson:
        break;
    case THUAI6::TrickerType::Assassin:
        break;
    case THUAI6::TrickerType::Idol:
        break;
    case THUAI6::TrickerType::Klee:
        if (!students.empty())
        {
            int ini = distance1(students[0]->x, self->x, students[0]->y, self->y);
            int target = 0;
            for (int i = 0; i < students.size(); i++)
            {
                if (distance1(students[i]->x, self->x, students[i]->y, self->y)/1000 < ini)
                {
                    ini = distance1(students[i]->x, self->x, students[i]->y, self->y)/1000;
                    target = i;
                }//确定离自己最近的学生并用target标记
            }
            if (distance1(students[target]->x, self->x, students[target]->y, self->y)/1000 < 3) {
                api.UseProp(self->props[0]);//把送的东西用了（不管是啥）
                api.UseSkill(0);
            } //相隔距离近的话，开技能准备秒杀
            else
            {
                int count = 0;
                int record = 0;
                int p = 0;
                for (int i = 0; i < students.size(); i++)
                {
                    for (int j = 0; j < bullet.size(); j++)
                    {
                        if (distance1(students[i]->x, bullet[j]->x, students[i]->y, bullet[j]->y)/1000 < 5) p += 1;
                    }
                    if (p > count) { count = p; record = i; }
                    p = 0;
                }			//确定视野中周边子弹最多的学生。
                if (count > 3) { api.UseSkill(1, students[record]->playerID); } //如果周边子弹多于3个，发动技能炸死他
            }
        }
        break;
    }
}

//目标返回, 返回目标作业编号
int least_aim(IStudentAPI& api)
{
    int i;
    int leastdis = 10000;
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    for (int j = 0; j < homework.size(); j++)
    {
        if (Manhattan(homework[j].cellx - self->x / 1000, homework[j].celly - self->y / 1000) < leastdis)
        {
            leastdis = Manhattan(homework[j].cellx - self->x / 1000, homework[j].celly - self->y / 1000);
            i = j;
        }
    }
    if (leastdis != 10000)
        return i;
}
int student_aim(IStudentAPI& api)
{
    int i;
    int leastdis = 10000;
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    for (int j = 0; j < homework.size(); j++)
    {
        if (homework[j].isdown == 0)
        {
            auto astar = A_aimstar(Square(self->x / 1000, self->y / 1000), Square(homework[j].cellx, homework[j].celly));
            if (astar.size() < leastdis)
            {
                leastdis = astar.size();
                i = j;
            }
        }
    }
    if (leastdis != 10000)
        return i;
    student_condition = 9;
    return 0;
}
int student_aimleave(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    int i;
    int largest = -10000;
    for (int j = 0; j < homework.size(); j++)
    {
        if (CopyT == T)
        {
            return copy_aimleave;
        }
        else
        {
            CopyT.Set(T);
        }
        if (homework[j].isdown == 0)
        {
            auto astarstudent = A_aimstar(Square(self->x / 1000, self->y / 1000), Square(homework[j].cellx, homework[j].celly));
            auto astartricker = A_aimstar(Square(T.Cellx, T.Celly), Square(homework[j].cellx, homework[j].celly));
            if (astartricker.size() - astarstudent.size() > largest)
            {
                largest = astartricker.size() - astarstudent.size();
                i = j;
            }
        }
    }
    if (largest != -10000)
    {
        copy_aimleave = i;
        return i;
    }
    student_condition = 9;
    return 2;
}
int aimgate(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    if (Manhattan(gate[1].cellx - self->x / 1000, gate[1].celly - self->y / 1000) > Manhattan(gate[0].cellx - self->x / 1000, gate[0].celly - self->y / 1000))
        return 0;
    return 1;
}

//沿路行走函数,只能走一步，第一个参数是目标点，第二个参数是api
void one_move(Square a, IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    if (all_times == 0)
    {
        Square b(self->x / 1000, self->y / 1000);
        astar_for_one_move = A_aimstar(b, a);
        api.Move(50, student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 + distance1(astar_for_one_move[1].x * 1000 + 500, self->x, astar_for_one_move[1].y * 1000 + 500, self->y) * 1000 / windowspeed(api) + 1000000 / windowspeed(api))));
        auto newself = api.GetSelfInfo();
        all_nextx = newself->x;
        all_nexty = newself->y;
    }
    else
    {
        cout << all_nextx << "   " << all_nexty << endl;
        Square b(all_nextx / 1000, all_nexty / 1000);
        astar_for_one_move = A_aimstar(b, a);
        astar_for_one_move.emplace_back(Square(0, 0));
        if (astar_for_one_move.size() <= 3)
        {
            if (astar_for_one_move.size() == 1)
            {
                return;
            }
            if (distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) <= 10)
            {
                student_condition = 6;
                //更换状态-student
            }
            else {
                student_condition = 6;
                api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y));
                std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
            }
        }
        else if (Maptypeing[astar_for_one_move[1].x][astar_for_one_move[1].y] == 3)
        {
            api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y));
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
            api.SkipWindow();
            time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 + distance1(astar_for_one_move[1].x * 1000 + 500, self->x, astar_for_one_move[1].y * 1000 + 500, self->y) * 1000 / windowspeed(api) + 1000000 / windowspeed(api))));
            auto news = api.GetSelfInfo();
            all_nextx = news->x;
            all_nexty = news->y;
        }
        else
        {
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time1 <= 50 || api.GetSelfInfo()->playerState != THUAI6::PlayerState::Idle)
            {

            }
            api.Move(50, student_dwa(astar_for_one_move[1].x, astar_for_one_move[1].y, api));
            time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            all_nextx = self->x + self->speed * 0.05 * cos(student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
            all_nexty = self->y + self->speed * 0.05 * sin(student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
        }
    }
}
void one_leave(Square a, IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    if (all_times == 0)
    {
        Square b(self->x / 1000, self->y / 1000);
        astar_for_one_move = A_aimstar(b, a);
        api.Move(50, student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 + distance1(astar_for_one_move[1].x * 1000 + 500, self->x, astar_for_one_move[1].y * 1000 + 500, self->y) * 1000 / windowspeed(api) + 1000000 / windowspeed(api))));
        auto newself = api.GetSelfInfo();
        all_nextx = newself->x;
        all_nexty = newself->y;
    }
    else
    {
        cout << all_nextx << "   " << all_nexty << endl;
        Square b(all_nextx / 1000, all_nexty / 1000);
        astar_for_one_move = A_aimstar(b, a);
        astar_for_one_move.emplace_back(Square(0, 0));
        if (astar_for_one_move.size() <= 3)
        {
            if (astar_for_one_move.size() == 1)
            {
                return;
            }
            if (distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) <= 10)
            {
                student_condition = 10;
                //更换状态-student
            }
            else {
                student_condition = 10;
                api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y));
                std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
            }
        }
        else if (Maptypeing[astar_for_one_move[1].x][astar_for_one_move[1].y] == 3)
        {
            api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y));
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
            api.SkipWindow();
            time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 + distance1(astar_for_one_move[1].x * 1000 + 500, self->x, astar_for_one_move[1].y * 1000 + 500, self->y) * 1000 / windowspeed(api) + 1000000 / windowspeed(api))));
            auto news = api.GetSelfInfo();
            all_nextx = news->x;
            all_nexty = news->y;
        }
        else
        {
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time1 <= 50 || api.GetSelfInfo()->playerState != THUAI6::PlayerState::Idle)
            {

            }
            api.Move(50, student_dwa(astar_for_one_move[1].x, astar_for_one_move[1].y, api));
            time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            all_nextx = self->x + self->speed * 0.05 * cos(student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
            all_nexty = self->y + self->speed * 0.05 * sin(student_dwa(astar_for_one_move[1].x * 1000 + 500, astar_for_one_move[1].y * 1000 + 500, api));
        }
    }
}
//void one_move(Square a, ITrickerAPI& api)
//{
//    cout << "move" << endl;
//    auto self = api.GetSelfInfo();
//    if (all_times == 0)
//    {
//        Square b(self->x / 1000, self->y / 1000);
//        auto astar = A_aimstar(b, a);
//        api.Move(50, tricker_dwa(astar[1].x * 1000 + 500, astar[1].y * 1000 + 500, api));
//        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//        all_nextx = self->x + self->speed * 0.05 * cos(tricker_dwa(astar[1].x * 1000 + 500, astar[1].y * 1000 + 500, api));
//        all_nexty = self->y + self->speed * 0.05 * sin(tricker_dwa(astar[1].x * 1000 + 500, astar[1].y * 1000 + 500, api));
//        return;
//    }
//    else
//    {
//        cout << "move1" << endl;;
//        //skill_using(api);
//        Square b(all_nextx / 1000, all_nexty / 1000);
//        auto astar = A_aimstar(b, a);
//        if (astar.size() <= 1)
//        {
//            cout << "move2" << endl;
//            if (distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) <= 10)
//            {
//                return;
//                //更换状态-tricker
//            }
//            else
//            {
//                cout << "move3" << endl;
//                api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y));
//                std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
//                return;
//            }
//        }
//        else if (Maptypeing[astar[1].x][astar[1].y] == 3)
//        {
//            cout << "move4" << endl;
//            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time1 <= 50 || self->playerState != THUAI6::PlayerState::Idle)
//            {
//
//            }
//            api.SkipWindow();
//            return;
//        }
//        else
//        {
//            cout << "move5" << endl;
//
//            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - time1 <= 50 || self->playerState != THUAI6::PlayerState::Idle)
//            {
//
//            }
//            cout << "move6" << endl;
//            api.Move(50, tricker_dwa(astar[1].x, astar[1].y, api));
//            cout << "move7" << endl;
//            time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//            all_nextx = self->x + self->speed * 0.05 * cos(tricker_dwa(astar[1].x * 1000 + 500, astar[1].y * 1000 + 500, api));
//            all_nexty = self->y + self->speed * 0.05 * sin(tricker_dwa(astar[1].x * 1000 + 500, astar[1].y * 1000 + 500, api));
//            cout << "move8" << endl;
//            return;
//        }
//    }
//}

void one_move(Square a, ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    Square b(self->x / 1000, self->y / 1000);
    auto astar = A_aimstar(b, a);
    skill_using(api);
    if (astar.size() <= 3)
    {
        if (distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) <= 10)
        {

        }
        else {
            api.Move(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed, angle(b.x * 1000 + 500 - self->x, b.y * 1000 + 500 - self->y)+3);
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * distance1(self->x, b.x * 1000 + 500, self->y, b.y * 1000 + 500) / self->speed + 1)));
        }
    }
    else if (Maptypeing[astar[1].x][astar[1].y] == 3)
    {
        api.SkipWindow();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(10 + distance1(astar[1].x * 1000 + 500, self->x, astar[1].y * 1000 + 500, self->y) * 1000 / windowspeed(api) + 1000000 / windowspeed(api))));
    }
    else
    {
        api.Move(55, tricker_dwa(astar[1].x, astar[1].y, api));
        std::this_thread::sleep_for(std::chrono::milliseconds(52));
    }
}

//工作
void one_dohw(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    if (distance1(self->x, homework[least_aim(api)].cellx * 1000 + 500, self->y, homework[least_aim(api)].celly * 1000 + 500) >= 1600)
    {
        student_condition = 2;
        return;
    }
    if (homework[least_aim(api)].isdown == 0)
    {
        if (self->playerState != THUAI6::PlayerState::Learning)
        {
            api.StartLearning();
        }
    }
    else student_condition = 2;
}
void one_dole(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    if (distance1(self->x, gate[aimgate(api)].cellx * 1000 + 500, self->y, gate[aimgate(api)].celly * 1000 + 500) >= 1600)
    {
        student_condition = 9;
        return;
    }
    if (gate[aimgate(api)].isdown == 0)
    {
        if (self->playerState != THUAI6::PlayerState::OpeningAGate)
        {
            api.StartOpenGate();
        }
    }
    else api.Graduate();
}

void AI::play(IStudentAPI& api)
{
    if (all_times == 0)
    {
        MapLoad_first(api);
    }
    all_times += 1;
    student_message(api);
    skill_using(api);
    switch (student_condition)
    {
    case 1://开局未集合状态
        student_condition = 2;
        break;
    case 2://前往目标
        one_move(Square(homework[student_aim(api)].cellx, homework[student_aim(api)].celly), api);
        break;
    case 3://发现捣蛋鬼状态,并远离状态
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n" << "sdadka" << "\n\n\n\n\n\n\n\n\n";
        one_move(Square(homework[student_aimleave(api)].cellx, homework[student_aimleave(api)].celly), api);
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n" << "sdahkbaoadka" << "\n\n\n\n\n\n\n\n\n";
        break;
    case 6://工作zuoye状态
        one_dohw(api);
        break;
    case 8://营救状态
    case 9://前往毕业状态
        if (api.GetSelfInfo()->studentType == THUAI6::StudentType::Teacher)
        {
            student_condition = 11;
            break;
        }
        one_leave(Square(gate[aimgate(api)].cellx, gate[aimgate(api)].celly), api);
        break;
    case 10://开校门状态
        one_dole(api);
        break;
    case 11:
        break;
    }
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

        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {

        // 玩家3执行操作
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

//标记最后出现的位置
int Last[2] = { -1,-1 };

//检测函数
vector<bool> Detection(ITrickerAPI& api)
{
    vector<bool>which;
    auto Stu = api.GetStudents();
    if (Stu.empty())
    {
        for (int i = 0; i < 4; i++)
        {
            student_state[i] = -1;
            which.push_back(true);
        }
        return which;
    }
    auto Sptr = Stu.begin();
    int ID;
    while (Sptr != Stu.end())
    {
        ID = Sptr->get()->playerID;
        //switch (Sptr->get()->playerState)
        //{
        //case(THUAI6::PlayerState::Addicted):
        //{
        //    student_state[ID] = 1;
        //    break;
        //}
        if (Sptr->get()->playerState == THUAI6::PlayerState::Addicted)
        {
            student_state[ID] = 1;
        }
        else
            student_state[ID] = 0;
        Sptr++;
    }
    for (int i = 0; i < 4; i++)
    {
        if (student_state[i] == 1)
        {
            which.push_back(false);
        }
        else
        {
            which.push_back(true);
        }
    }
    return(which);
}

vector<Square> Targets;
vector<Square> Already;
int STEP=0;

//巡逻函数
void Patrol(ITrickerAPI& api)
{
    cout << "STEP is" <<STEP<< endl;
    if (STEP == Targets.size())
    {
        auto A = Already.begin();
        while (!Already.empty())
        {
            A = Already.erase(A);
        }
        STEP = 0;
    }
    int tx = Targets[STEP].x+0.5;
    int ty = Targets[STEP].y+0.5;
    int sx = api.GetSelfInfo()->x/1000;
    int sy = api.GetSelfInfo()->y/1000;

    double Dis = sqrt(abs(double((tx - sx) * (sx - tx) + (ty - sy) * (ty - sy))));
    cout << Dis << endl;
    while ((tricker_condition == 1 || tricker_condition == 2) &&Dis>2.5)
    {
        tx = Targets[STEP].x;
        ty = Targets[STEP].y;
        sx = api.GetSelfInfo()->x / 1000;
        sy = api.GetSelfInfo()->y / 1000;
        Dis = sqrt(abs((tx - sx) * (sx - tx) + (ty - sy) * (ty - sy)));
        one_move(Targets[STEP], api);
        if (Dis < 1.5)
            break;
        cout << "STEP IS" << STEP << endl;

        tricker_message(api);
        if (tricker_condition == 3)
            return;
    }
    Already.push_back(Targets[STEP]);
    STEP++;
    return;
};

void Tricker_Attack(ITrickerAPI& api)
{
    int ID;
    vector<bool>wether = Detection(api);
    auto Stu = api.GetStudents();
    auto Sptr = Stu.begin();
    while (tricker_condition == 3)
    { 
        Stu = api.GetStudents();
        Sptr = Stu.begin();
        while (Sptr != Stu.end())
        {
            ID = Sptr->get()->playerID;
            if (wether[ID] && student_state[ID] == 0)
            {
                api.Attack(angle(Sptr->get()->x - api.GetSelfInfo()->x, Sptr->get()->y - api.GetSelfInfo()->y));
            }
            Sptr++;
        }
        tricker_message(api);
    }
}

void AI::play(ITrickerAPI& api)
{
    //vector<Square> W = A_aimstar(Square((api.GetSelfInfo()->y)/1000,(api.GetSelfInfo()->y)/1000),Square(47,35));
    //vector<Square>::iterator Walk = W.begin();
    //while (Walk+1 != W.end())
    //{
    //    cout << Walk->x << "  " << Walk->y << endl;
    //    if (Maptypeing[(Walk + 1)->x][(Walk + 1)->y] == 3)
    //    {
    //        api.SkipWindow();
    //        Walk += 2;
    //        continue;
    //    }
    //    A_Move(*Walk,*(Walk+1),api);
    //    Walk++;
    //}
    //捣蛋鬼执行操作
    auto self = api.GetSelfInfo();
    if (all_times == 0)
    {
        MapLoad_first(api);
        vector<Task>::iterator T = homework.begin();
        while (T != homework.end())
        {
            Targets.push_back(Square(T->cellx, T->celly));
            cout << T->cellx << " " << T->celly << endl;
            T++;
        }

        for (int i = 0; i < 4; i++)
        {
            student_state[i] = 0;
        };
    }
    switch (tricker_condition)
    {
    case 1://
        Patrol(api);
        tricker_message(api);
        break;
    case 2://警觉
    {
        Patrol(api);
        tricker_message(api);
        break;
    }
    case 3://在视野范围内并攻击
    {
        Tricker_Attack(api);
        break;
    }
    case 4://分开撤离状态
    case 5://前往远离点状态
    case 6://工作状态
    case 7://转移工作点状态
    case 8://营救状态
    case 9://毕业状态
    case 10://开校门状态
        break;


    }
    if (all_times <= 100000)
    {
        all_times ++;
    }
}