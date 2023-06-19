#include <vector>
#include <thread>
#include <queue>
#include <array>
#include <chrono>
#include <cfloat>
#include "AI.h"
#include "constants.h"

#define PI 3.141592654
#define AI_VERSION 230526
#define FOR(CONTAINER, ITER) for(auto ITER = CONTAINER.begin(); ITER != CONTAINER.end(); ITER++)

int (*G2C)(int) = IAPI::GridToCell;
int (*C2G)(int) = IAPI::CellToGrid;

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义
extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;
static int alert_r = 17000;

// 记录函数

// 定义非常多的状态（有限状态机）
enum class status
{
    initial,
    watch,
    sorround,
    idle,
    reset,
    index,
    retreat,
    avoid,
    move
};

static status BotStatus = status::idle;
static status LastStatus = status::reset;
// 存储路径的常量
const int MAXN = 50;
int a[MAXN][MAXN];      // 存储地图
bool blank[MAXN][MAXN];  // 存储蒙版
int vis[MAXN][MAXN];    // 标记数组，记录每个点是否被访问过
int dis[MAXN][MAXN];    // 记录每个点到起点的最短距离
int pre[MAXN][MAXN][2]; // 记录每个点的前驱节点

// 定义坐标结构体
struct Point
{
    int x, y;
    Point() : x(0), y(0) {};
    Point(int x, int y) : x(x), y(y) {};
};

// 定义队列元素结构体
struct Node
{
    Point p;
    int dist;
    Node(int d) { dist = d; };
    Node(Point p, int dist) : p(p), dist(dist) {};
    bool operator<(const Node& other) const
    { // 定义优先级，距离更小的优先级更高
        return dist > other.dist;
    };
};

// 状态机函数
void playerBot(ITrickerAPI&);
void moveStatus(ITrickerAPI& api);
void retreatStatus(ITrickerAPI& api);
void initialStatus(ITrickerAPI& api);
void idleStatus(ITrickerAPI& api);


// 函数

// 循迹相关
double Distance(Point, Point);
std::queue<Point> bfs(Point, Point);
void Goto(ITrickerAPI&, double, double, double); // randAngle = 1，则取波动范围为-0.5pi-0.5pi
void InitMapForMove(IAPI&);
//void initHwGroup();
void searchingArrayClear();
int pathLen(Point, Point);

// 状态检查类
bool isSurround(ITrickerAPI&, double, double);
bool stuckCheck(ITrickerAPI&, int); // 注意，n必须在2-10之间
bool progressStuckCheck(int, int);
int boolArrayCount(bool[], int);
void boolArrayReset(bool[], int);
bool isTrigger(ITrickerAPI&, Point);

// debug相关
void printPathType(ITrickerAPI&, std::queue<Point>);
void printQueue(std::queue<Point> q);
void printPosition(ITrickerAPI&);
void printPointVector(std::vector<Point>);

// 决策相关
void antiJuan(ITrickerAPI& api);


// 爬窗相关
bool isWindowInPath(ITrickerAPI& api, std::queue<Point> q);
bool isSurroundWindow(ITrickerAPI& api);
bool isDelayedAfterWindow(ITrickerAPI& api);

// 搜草丛相关
void grassSearch(ITrickerAPI& api);
// 变量

// 循迹相关变量
static int64_t myPlayerID;
static THUAI6::PlaceType map[50][50];
static int steps;
static int hasinitmap;
static int hasBeenTo;
static bool hasInitMap = false;
static bool IHaveArrived = false;
static int lastX = 0, lastY = 0;
static int lastFrameCount = 0;
std::queue<Point> path;

// stuckCheck()相关变量
int32_t memoryX[10];
int32_t memoryY[10];
std::chrono::system_clock::time_point stuckCheckStartTime;

// progressStuckCheck()相关变量
int32_t memoryProgress[10];

// 目标坐标
Point targetP = Point(12, 3);

// 特殊点坐标
std::vector<Point> hw;
std::vector<Point> door;
std::vector<Point> window;
std::vector<Point> gate;
std::vector<Point> chest;

// 决策相关变量

// 总决策函数相关变量
int decision;
/*
    decision = 1 追击
    decision = 2 巡逻
...
*/

// 爬窗相关变量
static bool isCrossingWindow = 0;
static bool climbingCheck = 0;
static int climbingFrameCount = 0;

// 巡逻相关变量
bool hwIsFinished[10];
bool hwControl[10];// 判断是否已经到达过该点
int patrolMode = 1;
/*
patrolMode = 0 在Addicted巡逻
patrolMode = 1 在作业间巡逻
patrolMode = 2 在作业和校门间巡逻
patrolMode = 3 在校门间巡逻
*/
// 搜草丛相关变量
bool grassCheck = false;
std::vector<Point> grassToSearch;
Point addictedPosition[4] = { Point(0,0), Point(0,0), Point(0,0), Point(0,0) };
std::chrono::system_clock::time_point grassStuckTime;

// 追击相关变量
int chasingFrameCount = 0;
bool chasingProtect;

//========================================

void AI::play(IStudentAPI& api)
{
    myPlayerID = this->playerID;
    Point temp;
    Point now_loc(G2C(api.GetSelfInfo()->x), G2C(api.GetSelfInfo()->y));
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass) {
                if (Distance(Point(i, j), now_loc) < Distance(temp, now_loc)) {
                    temp = Point(i, j);
                }
            }
        }
    }
    while (true) {
        api.Move(50, atan2(temp.y - now_loc.y, temp.x - now_loc.x));
    }
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    playerBot(api);
}

// 决策函数核心，从idleStatus转来
void antiJuan(ITrickerAPI& api) {
    auto stus = api.GetStudents();
    Point now_loc_g = { api.GetSelfInfo()->x, api.GetSelfInfo()->y };   // 当前grid坐标
    Point now_loc = { G2C(now_loc_g.x), G2C(now_loc_g.y) };             // 当前cell坐标
    if (stus.size() != 0) {
        Point stu_loc = { -100,-100 };      // 目标学生坐标
        std::vector<Point> addi_locs;       // Addicted学生坐标组

        for (int i = 0; i < stus.size(); i++) {
            Point temp_loc_g = { stus[i]->x, stus[i]->y };              // stus[i]的grid坐标
            Point temp_loc = { G2C(temp_loc_g.x), G2C(temp_loc_g.y) };  // stus[i]的cell坐标
            if (stus[i]->playerState == THUAI6::PlayerState::Addicted) {
                addi_locs.push_back(temp_loc);
            }
            else {
                if (Distance(now_loc, temp_loc) < Distance(now_loc, stu_loc))
                    stu_loc = temp_loc; // 追踪更近的学生
                if (Distance(now_loc_g, temp_loc_g) <= 1200) {
                    if (api.GetSelfInfo()->timeUntilSkillAvailable[0] <= 0) {
                        api.UseSkill(0);    // 蹦蹦炸弹
                        //std::cout << "bengbengzhadan!!!!!" << std::endl;
                    }
                    api.Attack(atan2(temp_loc_g.y - now_loc_g.y, temp_loc_g.x - now_loc_g.x));
                    BotStatus = status::idle;
                    return;
                }
            }
        }

        // ------------------------------------------------<<
        // 均为Addicted
        if (stu_loc.x == -100 && stu_loc.y == -100) {
            //patrolMode = 0; // 守株待兔
        }
        // ------------------------------------------------->>

        // 确认追踪
        else {
            targetP = stu_loc;
            BotStatus = status::initial;
            return;
        }
    }
    /*
    // -----------------------------------------------------------------------------<<
    else {
        double desire = api.GetSelfInfo()->trickDesire;
        // 存在不可见的学生在附近
        if (desire > 1) {
            Point temp;
            Point temp_g;
            double hide_stu_dist = alert_r / desire + 1;
            // 近在咫尺
            if (hide_stu_dist <= 1200) {
                std::vector<Point> grasses;
                for (int i = now_loc.x - 1; i <= now_loc.x + 1; i++) {
                    for (int j = now_loc.y - 1; j <= now_loc.y + 1; j++) {
                        if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass) {
                            grasses.emplace_back(Point(i, j));
                        }
                    }
                }
                size_t grasses_size = grasses.size();
                std::cout << grasses_size << std::endl;
                // 尝试打一下
                if (grasses_size > 0) {
                    Point grasses_sum;
                    FOR(grasses, iter) {
                        grasses_sum.x += iter->x;
                        grasses_sum.y += iter->y;
                    }
                    grasses_sum.x = G2C(C2G(grasses_sum.x) / grasses_size);
                    grasses_sum.y = G2C(C2G(grasses_sum.y) / grasses_size);
                    double ang;
                    if (grasses_sum.x == now_loc.x && grasses_sum.y == now_loc.y) {
                        ang = atan2(grasses[0].y - now_loc.y, grasses[0].x - now_loc.x);
                    }
                    else {
                        ang = atan2(grasses_sum.y - now_loc.y, grasses_sum.x - now_loc.x);
                    }
                    if (api.GetSelfInfo()->timeUntilSkillAvailable[0] <= 0) {
                        api.UseSkill(0);    // 蹦蹦炸弹
                        std::cout << "bengbengzhadan!!!!!" << std::endl;
                    }
                    api.Attack(ang);
                    BotStatus = status::idle;
                    return;
                }
            }
            // 搜草丛
            for (int i = 0; i < 50; i++) {
                for (int j = 0; j < 50; j++) {
                    temp.x = i; temp.y = j;
                    temp_g.x = C2G(i); temp_g.y = C2G(j);
                    if (Distance(temp_g, now_loc_g) <= hide_stu_dist) {
                        if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass && !api.HaveView(temp_g.x, temp_g.y)) {
                            targetP = temp;
                            grassCheck = true;
                            BotStatus = status::initial;
                            return;
                        }
                    }
                }
            }
        }

    }
    */
    // --------------------------------------------------------------------------------------------------------------------->>

    decision = 2;   // 没有学生为正常巡逻，存在Addicted为守株待兔
    BotStatus = status::idle;
    return;
}

void patrolMode0(ITrickerAPI& api) {
    auto stus = api.GetStudents();
    bool isTeacher = true;
    for (int i = 0; i < stus.size(); i++) {
        if (stus[i]->studentType != THUAI6::StudentType::Teacher) {
            isTeacher = false;
            break;
        }
    }
    if (isTeacher) patrolMode = 1;
    return;
}

void patrolMode1(ITrickerAPI& api) {
    double dis = DBL_MAX;
    std::vector<int> temp;
    auto self = api.GetSelfInfo();
    int sx = self->x;
    int sy = self->y;
    int cellX = G2C(sx);
    int cellY = G2C(sy);
    for (int i = 0; i < hw.size(); i++)
    {
        temp.emplace_back(api.GetClassroomProgress(hw[i].x, hw[i].y));
        //std::cout << "temp" << i << ":" << temp[i] << std::endl;
    }
    for (int i = 0; i < hw.size(); i++)
    {
        if (hwIsFinished[i]) continue;
        if (isTrigger(api, hw[i]) && temp[i] < 10000000 && !hwIsFinished[i])
        {
            hwControl[i] = true;
        }
        if (temp[i] == 10000000)
        {
            hwIsFinished[i] = true;
        }
    }
    targetP = Point(0, 0);
    for (int i = 0; i < hw.size(); i++)
    {
        int tempDis = pathLen(hw[i], Point(cellX, cellY));
        if (tempDis < dis && !hwIsFinished[i] && !hwControl[i])
        {
            dis = tempDis;
            targetP.x = hw[i].x;
            targetP.y = hw[i].y;
        }
    }
    if (targetP.x == 0 && targetP.y == 0)
    {
        BotStatus = status::idle;
        return;
    }
    BotStatus = status::initial;
    return;
}

void patrolMode2(ITrickerAPI& api) {

}

void patrolMode3(ITrickerAPI& api) {

}

// 巡逻函数核心
void walkingAround(ITrickerAPI& api)
{
    if (patrolMode != 0) {
        patrolMode = 1;
        if (boolArrayCount(hwControl, 10) >= 10 - boolArrayCount(hwIsFinished, 10))
        {
            //std::cout << "hwReset!!!" << std::endl;
            boolArrayReset(hwControl, 10);
        }
    }
    switch (patrolMode)
    {
    case 0:
        patrolMode0(api);
        break;
    case 1:
        patrolMode1(api);
        break;
    case 2:
        patrolMode2(api);
        break;
    case 3:
        patrolMode3(api);
        break;
    default:
        break;
    }

}

// 判断两块草是否紧密连接
bool isConnected(ITrickerAPI& api, Point a, Point b)
{
    if (abs(a.x - b.x) + abs(a.y - b.y) <= 1)
        return true;
    else
        return false;
}


void grassSearch(ITrickerAPI& api)
{
    for (int i = 0; i < grassToSearch.size(); i++)
    {
        if (grassToSearch[i].x != 0 && grassToSearch[i].y != 0)
        {
            if (isSurround(api, grassToSearch[i].x + 0.5, grassToSearch[i].y + 0.5))
            {
                grassToSearch[i].x = 0;
                grassToSearch[i].y = 0;
                for (int j = i + 1; j < grassToSearch.size(); j++)
                {
                    if (isConnected(api, grassToSearch[i], grassToSearch[j]))
                    {
                        grassToSearch[j].x = 0;
                        grassToSearch[j].y = 0;
                    }
                }
            }
            else
            {
                targetP = grassToSearch[i];
                BotStatus = status::initial;
                return;
            }
        }
    }
    //std::cout << "searchgrass end!!!" << std::endl;
    grassCheck = false;
    BotStatus = status::idle;
    return;
}

int boolArrayCount(bool arr[], int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (arr[i])
        {
            count++;
        }
    }
    return count;
}

void boolArrayReset(bool arr[], int n)
{
    for (int i = 0; i < n; i++)
    {
        arr[i] = false;
    }
}

void blankReset()
{
    for (int i = 0; i < MAXN; i++)
    {
        for (int j = 0; j < MAXN; j++)
        {
            blank[i][j] = false;
        }
    }
}

void searchingArrayClear()
{
    int i, j;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            vis[i][j] = 0;
            dis[i][j] = 0;
            pre[i][j][0] = 0;
            pre[i][j][1] = 0;
        }
    }
}

void InitMapForMove(IAPI& api)
{
    int i, j;
    a[26][42] = 1;
    a[13][48] = 1;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            a[i][j] = (int)api.GetPlaceType(i, j) - 1;
            if (a[i][j] >= 10)
                a[i][j] = 9;
            if (a[i][j] == 2)
                a[i][j] = 0;
            //std::cout << a[i][j];
            if ((int)api.GetPlaceType(i, j) == 4)
            {
                hw.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 8)
            {
                door.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 9)
            {
                door.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 10)
            {
                door.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 7)
            {
                window.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 5)
            {
                gate.push_back(Point(i, j));
            }
            else if ((int)api.GetPlaceType(i, j) == 11)
            {
                chest.push_back(Point(i, j));
            }
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < door.size(); i++)
    {
        a[door[i].x][door[i].y] = 0;
    }
    for (int i = 0; i < window.size(); i++)
    {
        a[window[i].x][window[i].y] = 0;
    }
    api.Wait();
}

// 搜索最短路径
std::queue<Point> bfs(Point start, Point end)
{
    searchingArrayClear();
    std::queue<Point> path;
    std::priority_queue<Node> q;
    q.push(Node(start, 0));
    vis[start.x][start.y] = 1;
    //std::cout << " check1 " << std::endl;
    while (!q.empty())
    {
        Node cur = q.top();
        q.pop();
        Point p = cur.p;
        int dist = cur.dist;
        if (p.x == end.x && p.y == end.y)
        { // 找到终点
            path.push(p);
            while (p.x != start.x || p.y != start.y)
            { // 回溯路径
                Point pre_p = Point(pre[p.x][p.y][0], pre[p.x][p.y][1]);
                path.push(pre_p);
                p = pre_p;
            }
            break;
        }
        if (p.x - 1 >= 0 && !vis[p.x - 1][p.y] && a[p.x - 1][p.y] == 0 && blank[p.x - 1][p.y] == 0)
        { // 向上搜索
            vis[p.x - 1][p.y] = 1;
            dis[p.x - 1][p.y] = dist + 1;
            pre[p.x - 1][p.y][0] = p.x;
            pre[p.x - 1][p.y][1] = p.y;
            q.push(Node(Point(p.x - 1, p.y), dist + 1));
        }
        if (p.x + 1 < MAXN && !vis[p.x + 1][p.y] && a[p.x + 1][p.y] == 0 && blank[p.x + 1][p.y] == 0)
        { // 向下搜索
            vis[p.x + 1][p.y] = 1;
            dis[p.x + 1][p.y] = dist + 1;
            pre[p.x + 1][p.y][0] = p.x;
            pre[p.x + 1][p.y][1] = p.y;
            q.push(Node(Point(p.x + 1, p.y), dist + 1));
        }
        if (p.y - 1 >= 0 && !vis[p.x][p.y - 1] && a[p.x][p.y - 1] == 0 && blank[p.x][p.y - 1] == 0)
        { // 向左搜索
            vis[p.x][p.y - 1] = 1;
            dis[p.x][p.y - 1] = dist + 1;
            pre[p.x][p.y - 1][0] = p.x;
            pre[p.x][p.y - 1][1] = p.y;
            q.push(Node(Point(p.x, p.y - 1), dist + 1));
        }
        if (p.y + 1 < MAXN && !vis[p.x][p.y + 1] && a[p.x][p.y + 1] == 0 && blank[p.x][p.y + 1] == 0)
        { // 向右搜索
            vis[p.x][p.y + 1] = 1;
            dis[p.x][p.y + 1] = dist + 1;
            pre[p.x][p.y + 1][0] = p.x;
            pre[p.x][p.y + 1][1] = p.y;
            q.push(Node(Point(p.x, p.y + 1), dist + 1));
        }
    }
    printQueue(path);
    return path;
}

void printQueue(std::queue<Point> q)
{
    // printing content of queue
    while (!q.empty())
    {
        std::cout << "(" << q.front().x << "," << q.front().y << ")->";
        q.pop();
    }
    std::cout << std::endl;
}

void printPointVector(std::vector<Point> v)
{
    for (int i = 0; i < v.size(); i++)
    {
        //std::cout << "(" << v[i].x << "," << v[i].y << ")->";
    }
    //std::cout << std::endl;
}

bool isSurround(ITrickerAPI& api, double x, double y)
{
    double distance;
    auto self = api.GetSelfInfo();
    double sx = (double)(self->x) / 1000;
    double sy = (double)(self->y) / 1000;
    distance = sqrt((sx - x) * (sx - x) + (sy - y) * (sy - y));
    return distance <= 0.3;
}

bool isTrigger(ITrickerAPI& api, Point p)
{
    auto self = api.GetSelfInfo();
    auto sx = (self->x) / 1000;
    auto sy = (self->y) / 1000;
    return abs(sx - p.x) <= 1.5 && abs(sy - p.y) <= 1;
}

void Goto(ITrickerAPI& api, double destX, double destY, double randAngle = 0)
{
    // std::printf("goto %d,%d\n", destX, destY);
    auto self = api.GetSelfInfo();
    int sx = self->x;
    int sy = self->y;

    auto delta_x = (double)(destX * 1000 - sx);
    auto delta_y = (double)(destY * 1000 - sy);
    //std::cout << "dx" << delta_x << "dy" << delta_y << std::endl;
    double ang = 0;
    // 直接走
    ang = atan2(delta_y, delta_x);
    //std::cout << "angle:" << ang << std::endl;
    if (delta_x != 0 || delta_y != 0)
        api.Move(300, ang + (std::rand() % 10 - 5) * PI / 10 * randAngle);
}

// 判断实际速度是否为0（防止卡墙上）
bool stuckCheck(ITrickerAPI& api, int n)
{
    if (n >= 2 && n <= 10)
    {
        auto self = api.GetSelfInfo();
        auto sx = self->x;
        auto sy = self->y;
        for (int i = 0; i <= n - 2; i++)
        {
            memoryX[i] = memoryX[i + 1];
            memoryY[i] = memoryY[i + 1];
        }
        memoryX[n - 1] = sx;
        memoryY[n - 1] = sy;
        if (abs(memoryX[0] - sx) < 200 && abs(memoryY[0] - sy) < 200)
        {
            //std::cout << "stuck!" << std::endl;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool progressStuckCheck(int progress, int n) // 需要更新!
{
    /*
    if (n >= 2 && n <= 10)
    {
        for (int i = 0; i <= n - 2; i++)
        {
            memoryProgress[i] = memoryProgress[i + 1];
        }
        memoryProgress[n - 1] = progress;
        if (memoryProgress[0] == progress)
        {
            std::cout << "progressStuck!" << std::endl;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    */
    return false;
}

double Distance(Point a, Point b)
{
    return (sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)));
}

int pathLen(Point a, Point b)
{
    auto tempPath = bfs(a, b);
    return tempPath.size();
}

void printPathType(ITrickerAPI& api, std::queue<Point> q)
{

}

void printPosition(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto sx = self->x;
    auto sy = self->y;
    //std::cout << "position: (" << sx << "," << sy << ")" << std::endl;
}

bool isWindowInPath(ITrickerAPI& api, std::queue<Point> p)
{
    std::queue<Point> q = p;
    while (!q.empty())
    {
        if (api.GetPlaceType(q.front().x, q.front().y) == THUAI6::PlaceType::Window)
        {
            return true;
        }
        q.pop();
        //std::cout << "popping" << std::endl;
    }
    return false;
}

bool isSurroundWindow(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    int x = self->x / 1000;
    int y = self->y / 1000;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            if (i >= 1 && i <= 49 && j >= 1 && j <= 49)
            {
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Window)
                    return true;
            }
        }
    }
    return false;
}

//爬完窗后不会重复爬
bool isDelayedAfterWindow(ITrickerAPI& api)
{
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Climbing)
    {
        climbingCheck = true;
        climbingFrameCount = 0;
    }
    climbingFrameCount++;
    if (climbingFrameCount > 15)
    {
        climbingCheck = false;
    }
    return !climbingCheck;
}

bool isNearAddictedStudent(ITrickerAPI& api)
{
    for (int i = 0; i < 4; i++)
    {
        if (Distance(Point(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->x / 1000), addictedPosition[i]) < 6)
        {
            return true;
        }
    }
    return false;
}

void botInit(ITrickerAPI& api)      //状态机的初始化
{
    std::ios::sync_with_stdio(false);
    auto self = api.GetSelfInfo();
    // 初始化地图(第一次)
    if (!hasInitMap)
    {
        InitMapForMove(api);
        hasInitMap = true;
    }
    blankReset();
    auto currentTimeForGrass = std::chrono::system_clock::now();
    auto diffForGrass = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimeForGrass - grassStuckTime);
    if (diffForGrass.count() > 10000)
    {
        grassStuckTime = std::chrono::system_clock::now();
        // addictedPosition 清空
        for (int i = 0; i < 4; i++)
        {
            addictedPosition[i].x = 0;
            addictedPosition[i].y = 0;
        }
    }

    bool doorFlag = false;
    for (int itr = 0; itr < door.size(); itr++)
    {
        if (api.HaveView(door[itr].x, door[itr].y))
        {
            if (!api.IsDoorOpen(door[itr].x, door[itr].y))
            {
                a[door[itr].x][door[itr].y] = 1;
                doorFlag = true;
            }
        }
    }
    if (doorFlag) InitMapForMove(api);

    auto stus = api.GetStudents();
    int stuNum = 0;
    for (int i = 0; i < stus.size(); i++)
    {
        blank[G2C(stus[i]->x)][G2C(stus[i]->y)] = true;
        if (stus[i]->playerState != THUAI6::PlayerState::Addicted)
        {
            stuNum++;
            addictedPosition[i] = Point(0, 0);
        }
        else
        {
            addictedPosition[i] = Point(stus[i]->x / 1000, stus[i]->y / 1000);
        }
    }
    blank[G2C(self->x)][G2C(self->y)] = false;

    // -----------------------------------------------------------------------------------------<<
    std::vector<Point> tempGrass;
    if (1) {
        Point now_loc_g = { api.GetSelfInfo()->x, api.GetSelfInfo()->y };   // 当前grid坐标
        Point now_loc = { G2C(now_loc_g.x), G2C(now_loc_g.y) };             // 当前cell坐标
        double desire = api.GetSelfInfo()->trickDesire;
        Point temp;
        Point temp_g;
        double hide_stu_dist = alert_r / desire + 1;
        // 近在咫尺
        if (hide_stu_dist <= 1200 && stus.size() == 0 && !isNearAddictedStudent(api)) {

            std::vector<Point> grasses;
            for (int i = now_loc.x - 1; i <= now_loc.x + 1; i++) {
                for (int j = now_loc.y - 1; j <= now_loc.y + 1; j++) {
                    if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass) {
                        grasses.emplace_back(Point(i, j));
                    }
                }
            }
            size_t grasses_size = grasses.size();
            //std::cout << grasses_size << std::endl;
            // 尝试打一下
            if (grasses_size > 0) {
                Point grasses_sum;
                FOR(grasses, iter) {
                    grasses_sum.x += iter->x;
                    grasses_sum.y += iter->y;
                }
                grasses_sum.x = G2C(C2G(grasses_sum.x) / grasses_size);
                grasses_sum.y = G2C(C2G(grasses_sum.y) / grasses_size);
                double ang;
                if (grasses_sum.x == now_loc.x && grasses_sum.y == now_loc.y) {
                    ang = atan2(grasses[0].y - now_loc.y, grasses[0].x - now_loc.x);
                }
                else {
                    ang = atan2(grasses_sum.y - now_loc.y, grasses_sum.x - now_loc.x);
                }
                if (api.GetSelfInfo()->timeUntilSkillAvailable[0] <= 0) {
                    api.UseSkill(0);    // 蹦蹦炸弹
                    //std::cout << "bengbengzhadan!!!!!" << std::endl;
                }
                api.Attack(ang);
            }
        }
        else if (hide_stu_dist <= 5000 && stus.size() == 0 && !isNearAddictedStudent(api) && decision != 1)
        {
            for (int i = G2C(self->x) - 5; i <= G2C(self->x) + 5; i++) {
                for (int j = G2C(self->y) - 5; j <= G2C(self->y) + 5; j++) {
                    if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass) {
                        tempGrass.emplace_back(Point(i, j));
                    }
                }
            }
            if (!tempGrass.empty())
                grassCheck = 1;
        }
    }
    // -------------------------------------------------------------------------------->>

    //std::cout << "decision:" << decision << "stu:" << stuNum << std::endl;
    if (decision == 1)
    {
        chasingFrameCount++;
    }
    if (stuNum > 0) grassCheck = 0;
    // 存在且只存在Addicted学生

    // 存在非Addicted学生，且处于walkingAround状态或者超过10帧未更新路线
    if (stuNum != 0 && (decision != 1 || (decision == 1 && chasingFrameCount > 10)))
    {
        chasingFrameCount = 0;
        decision = 1;
        BotStatus = status::idle;
    }
    else if (grassCheck) {
        if (decision != 3)
        {
            grassToSearch = tempGrass;
            BotStatus = status::idle;
            decision = 3;
        }
    }
    // 没有非Addicted学生，且处于idle或initial状态
    else if (stuNum == 0 && (BotStatus != status::move && BotStatus != status::retreat))
    {
        decision = 2;
    }
}

void playerBot(ITrickerAPI& api)
{
    botInit(api);
    //std::cout << "isSurroundWindow:    " << isSurroundWindow(api) << std::endl;
    switch (BotStatus)
    {
        // 有限状态机的core
    case status::initial:
    {
        initialStatus(api);
        break;
    }
    case status::move:
    {
        moveStatus(api);
        // printPosition(api);
        // printQueue(path);
        // printPathType(api, path);
        break;
    }
    case status::retreat:
    {
        retreatStatus(api);
        break;
    }
    case status::idle:
    {
        idleStatus(api);
        break;
    }
    }
    api.Wait();
}

//=============================================

// ——————————————状态机函数—————————————————————————————

void moveStatus(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    int x = self->x / 1000;
    int y = self->y / 1000;
    //std::cout << "move!" << std::endl;
    if (isWindowInPath(api, path))
    {
        isCrossingWindow = 1;
    }
    else
        isCrossingWindow = 0;
    //std::cout << "isCrossingWindow" << isCrossingWindow << std::endl;
    //std::cout << "isSurroundWindow:" << isSurroundWindow(api) << std::endl;
    if (isCrossingWindow == 1)
    {
        if (isDelayedAfterWindow(api) == 1 && isSurroundWindow(api) == 1)
        {
            //std::cout << "my skipping window!" << std::endl;
            api.SkipWindow();
            api.SkipWindow();
            if (!path.empty())
            {
                path.pop();
            }
        }
    }
    if (!path.empty())
    {
        if (isSurround(api, path.front().x + 0.5, path.front().y + 0.5) && (!path.empty()))
            path.pop();
    }
    else
    {
        BotStatus = status::initial;
    }

    if (blank[targetP.x][targetP.y] != 0 || a[targetP.x][targetP.y] != 0)
    {
        if (!path.empty() && (!isTrigger(api, targetP) || !api.HaveView(targetP.x * 1000 + 500, targetP.y * 1000 + 500)))
        {
            // std::cout << path.front().x << path.front().y << std::endl;
            Goto(api, path.front().x + 0.5, path.front().y + 0.5);
        }
        else
        {
            BotStatus = status::idle;
        }
    }
    else
    {
        if (!path.empty())
        {
            // std::cout << path.front().x << path.front().y << std::endl;
            Goto(api, path.front().x + 0.5, path.front().y + 0.5);
        }
        else
        {
            BotStatus = status::idle;
        }
    }
    if (stuckCheck(api, 6))
    {

        BotStatus = status::retreat;
        stuckCheckStartTime = std::chrono::system_clock::now();
    }
}

void retreatStatus(ITrickerAPI& api)
{
    //std::cout << "retreat" << std::endl;



    if (isCrossingWindow)
    {
        if (isSurroundWindow(api) && isDelayedAfterWindow(api))
        {
            std::cout << "my skipping window!" << std::endl;
            api.SkipWindow();
        }
    }
    double randAngle = 1;
    auto currentTime = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - stuckCheckStartTime);
    if (diff.count() > 200)
    {
        //std::cout << "deep rand!!" << std::endl;
        randAngle = 3;
        BotStatus = status::initial;
    }
    if (!path.empty() && !isTrigger(api, targetP))
    {
        //std::cout << path.front().x << path.front().y << std::endl;
        Goto(api, path.front().x + 0.5, path.front().y + 0.5, randAngle);
        if (isSurround(api, path.front().x + 0.5, path.front().y + 0.5))
            path.pop();
    }
    else
    {
        BotStatus = status::idle;
    }
    if (!stuckCheck(api, 3))
    {
        BotStatus = status::move;
    }
}

void initialStatus(ITrickerAPI& api)
{
    //std::cout << "initial" << std::endl;
    climbingFrameCount = 21;
    int x = (api.GetSelfInfo()->x) / 1000;
    int y = (api.GetSelfInfo()->y) / 1000;
    path = bfs(Point(targetP.x, targetP.y), Point(x, y));
    if (path.empty())
    {
        //std::cout << "empty!" << std::endl;
        blankReset();
        path = bfs(Point(targetP.x, targetP.y), Point(x, y));
    }
    if (path.empty())
    {
        BotStatus = status::idle;
        return;
    }
    IHaveArrived = false;
    BotStatus = status::move;
    printQueue(path);
}

void idleStatus(ITrickerAPI& api)
{
    //std::cout << "idling!" << std::endl;

    switch (decision)
    {
    case 1:
    {
        antiJuan(api);
        break;
    }
    case 2:
    {
        walkingAround(api);
        break;
    }
    case 3:
    {
        grassSearch(api);
        break;
    }
    case 4:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}