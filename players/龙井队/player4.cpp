#include <vector>
#include <thread>
#include <array>
#include<iostream>
#include<math.h>
#include <thread>
#include <queue>
#include "AI.h"
#include "constants.h"
using namespace std;
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Idol;

// 可以在AI.cpp内部声明变量与函数

struct Resorce
{
    int gate[3];//x*100+y,开了就存
    int gateopen;
    int homework[10];//x*100+y,写完了就存
    int workdone;
};

Resorce ours = { {0} ,0,{0,0,0,0,0,0,0,0,0,0} ,0 };
int p0[2];
int p1[2];
int p2[2];
int p3[2];
int hw0[2] = { 0,0 };//角色0作业目标的初始化
int hw1[2] = { 0,0 };
int hw2[2] = { 0,0 };
int hw3[2] = { 0,0 };
int gt[2] = { 0,0 };
int p_0 = 0;//方向指示
int p_1 = 0;
int p_2 = 0;
int p_3 = 0;

int* student_search(IStudentAPI& api1, int des, int typ, int* pos, int* homework, int s)//搜索附近的东西（type），写完了作业互相通知一声！
{
    int x = api1.GridToCell(api1.GetSelfInfo()->x);
    int y = api1.GridToCell(api1.GetSelfInfo()->y);
    auto fullmap = api1.GetFullMap();
    for (int d = 1; d <= des; d++)
    {
        for (int i = 0; i <= d; i++)
        {
            int j = d - i;
            if ((x + i) < 50 && (y + j) < 50)//判断不越界
                if ((int)fullmap[x + i][y + j] == typ)
                {
                    pos[0] = x + i;//横坐标
                    pos[1] = y + j;//纵坐标
                    bool valid = true;
                    for (int i = 0; i < s; i++)//看看这个作业写完没有
                        if (homework[i] == pos[0] * 100 + pos[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        api1.Print(fmt::format("here is a homework:({},{})", x + i, y + j));
                        return pos;
                    }
                }
            if ((x + i) < 50 && (y - j) >= 0)//判断不越界
                if ((int)fullmap[x + i][y - j] == typ)
                {
                    pos[0] = x + i;//横坐标
                    pos[1] = y - j;//纵坐标
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (homework[i] == pos[0] * 100 + pos[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        api1.Print(fmt::format("here is a homework:({},{})", x + i, y - j));
                        return pos;
                    }
                }
            if ((x - i) >= 0 && (y + j) < 50)//判断不越界
                if ((int)fullmap[x - i][y + j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y + j;//纵坐标
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (homework[i] == pos[0] * 100 + pos[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        api1.Print(fmt::format("here is a homework:({},{})", x - i, y + j));
                        return pos;
                    }
                }
            if ((x - i) >= 0 && (y - j) >= 0)//判断不越界
                if ((int)fullmap[x - i][y - j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y - j;//纵坐标
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (homework[i] == pos[0] * 100 + pos[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        api1.Print(fmt::format("here is a homework:({},{})", x - i, y - j));
                        return pos;
                    }
                }
        }
    }
    pos[0] = 0;
    pos[1] = 0;
    return pos;//什么都没找到返回（0，0）
}

bool JudgeScope_9(IStudentAPI& api, int target_x, int target_y) //判断是否在九宫格范围的函数
{
    int x = api.GridToCell(api.GetSelfInfo()->x);
    int y = api.GridToCell(api.GetSelfInfo()->y);
    int b = 0, c = 0;
    b = abs(x - target_x);
    c = abs(y - target_y);
    if ((b == 1 && c == 1) || (b == 1 && c == 0) || (b == 0 && c == 1))
    {
        api.Print("摸到作业辣！");
        return true;
    }
    else
    {
        api.Print("附近没有作业qwq");
        return false;
    }
};

void CellByCell(IStudentAPI& api1, int p)//一次走一格
{
    int x = api1.GridToCell(api1.GetSelfInfo()->x);
    int y = api1.GridToCell(api1.GetSelfInfo()->y);
    switch (p)
    {
        int aim_g;
    case 0:
    {
        aim_g = (x + 1) * 1000 + 500;
        if ((aim_g - api1.GetSelfInfo()->x) > 100)//如果没有整个人都在格子里的话
        {
            api1.MoveDown(40);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    }
    case 1:
    {   aim_g = (y + 1) * 1000 + 500;
    if ((aim_g - api1.GetSelfInfo()->y) > 100)
    {
        api1.MoveRight(40);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    break;
    }
    case 2:
    {
        aim_g = (x - 1) * 1000 + 500;
        if ((api1.GetSelfInfo()->x - aim_g) > 100)
        {
            api1.MoveUp(40);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    }
    case 3:
    {
        aim_g = (y - 1) * 1000 + 500;
        if ((api1.GetSelfInfo()->x - aim_g) > 100)
        {
            api1.MoveLeft(40);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    }
    }
}

void PosCheck(IStudentAPI& api, int p)
{
    auto s = api.GetSelfInfo();
    int inx = api.GetSelfInfo()->x;
    int iny = api.GetSelfInfo()->y;
    int ax = api.GridToCell(api.GetSelfInfo()->x);
    int ay = api.GridToCell(api.GetSelfInfo()->y);
    switch (p)
    {
    case 0:
        if (ay * 1000 + 500 - iny > 0)
        {
            api.MoveRight(20);
            api.Print(fmt::format("right,nuonuo"));
            api.MoveUp(20);
            api.Print(fmt::format("up,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            api.MoveLeft(20);
            api.Print(fmt::format("left,nuonuo"));
            api.MoveUp(20);
            api.Print(fmt::format("up,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    case 1:
        if (ax * 1000 + 500 - inx > 0)
        {
            api.MoveDown(20);
            api.Print(fmt::format("down,nuonuo"));
            api.MoveLeft(20);
            api.Print(fmt::format("left,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            api.MoveUp(20);
            api.Print(fmt::format("up,nuonuo"));
            api.MoveLeft(20);
            api.Print(fmt::format("left,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    case 2:
        if (ay * 1000 + 500 - iny > 0)
        {
            api.MoveRight(20);
            api.Print(fmt::format("right,nuonuo"));
            api.MoveDown(20);
            api.Print(fmt::format("down,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            api.MoveLeft(20);
            api.Print(fmt::format("left,nuonuo"));
            api.MoveDown(20);
            api.Print(fmt::format("down,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    case 3:
        if (ax * 1000 + 500 - inx > 0)
        {
            api.MoveDown(20);
            api.Print(fmt::format("down,nuonuo"));
            api.MoveRight(20);
            api.Print(fmt::format("right,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            api.MoveUp(20);
            api.Print(fmt::format("up,nuonuo"));
            api.MoveRight(20);
            api.Print(fmt::format("right,nuonuo"));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        break;
    }
}


//以下是找路径的函数
static int FullMap[50][50];
static int WalkMap[50][50];
static int rows = 50, cols = 50;
class Point {
public:
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {}
    Point& operator= (Point S);
};
Point& Point::operator=(Point S)
{
    x = S.x;
    y = S.y;
    return *this;
}
// 判断点是否在二维数组中，并且是否可通行
bool isValid(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols && WalkMap[x][y] == 1;
}

// 计算两点之间的曼哈顿距离
int getDistance(Point a, Point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// 广度优先搜索
int bfs(Point start, Point end, int rows, int cols, vector<vector<Point>>& prev) {
    // 定义一个队列，存储搜索到但未处理的点
    queue<Point> q;
    // 存储已经处理过的点
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    // 定义方向数组
    int dx[] = { -1, 0, 0, 1 };
    int dy[] = { 0, -1, 1, 0 };

    // 将起点加入队列
    q.push(start);
    // 记录起点已经被访问
    visited[start.x][start.y] = true;

    // 广度优先搜索
    while (!q.empty()) { // 当队列不为空时
        int size = q.size(); // 获取当前层的节点数
        for (int i = 0; i < size; i++) { // 遍历当前层的所有节点
            Point curr = q.front(); // 取出队首节点
            q.pop(); // 弹出队首节点
            // 找到终点，返回前驱数组
            if (curr.x == end.x && curr.y == end.y) {
                return getDistance(start, end);
            }
            // 否则，继续向周围可通行的点进行搜索
            for (int j = 0; j < 4; j++) { // 遍历当前节点的四个方向
                int next_x = curr.x + dx[j];
                int next_y = curr.y + dy[j];
                if (isValid(next_x, next_y, rows, cols) && !visited[next_x][next_y]) {
                    Point next(next_x, next_y); // 定义下一个节点
                    visited[next_x][next_y] = true; // 记录节点已经被访问
                    q.push(next); // 将节点加入队列
                    prev[next_x][next_y] = curr; // 记录前驱节点（每一个点在prev数组中位置记录了其前驱节点的信息）
                }
            }
        }
    }

    // 无法到达终点，返回 -1
    return -1;
}

// 根据前驱数组还原出最短路径
vector<Point> getPath(Point start, Point end, vector<vector<Point>> prev) {
    vector<Point> path; // 定义存储路径的向量
    // 如果终点没有前驱节点，则无法到达终点
    if (prev[end.x][end.y].x == -1 && prev[end.x][end.y].y == -1) {
        return path;
    }
    Point curr = end;
    while (!(curr.x == start.x && curr.y == start.y)) { // 从终点到起点，找到所有前驱节点
        path.push_back(curr);
        curr = prev[curr.x][curr.y];
    }
    path.push_back(start);
    reverse(path.begin(), path.end()); // 路径记录的是起点到终点，需要反转一遍
    return path;
}
int Findpath(int start_x, int start_y, int end_x, int end_y)
{
    // 进行广度优先搜索
    Point start(start_x, start_y), end(end_x, end_y);
    vector<vector<Point>> prev(rows, vector<Point>(cols, Point(-1, -1))); // 定义前驱数组，并初始化为 -1
    int distance = bfs(start, end, rows, cols, prev); // 计算最短距离
    cout << distance << endl;
    // 还原出最短路径
    vector<Point> path = getPath(start, end, prev);
    // 输出结果
    if (path.empty()) {
        cout << "无法到达终点" << endl;
    }
    else {
        cout << "起点到终点的最短距离为：" << distance << endl;
        //cout << path[1].x<<path[1].y << endl;
        cout << "路径为：";
        for (auto p : path) {
            cout << "(" << p.x << "," << p.y << ") ";
        }
        cout << endl;
    }
    if (start_x == path[1].x)
    {
        if (start_y > path[1].y)
            return 3;//向左走
        else
            return 1;//向右走
    }
    else
        if (start_x > path[1].x)
            return 2;//向上走
        else
            return 0;//向下走
}


//int messagetrans(IStudentAPI& api, string m)
//{
//    int g = stoi(m);
//    if
//}


int Judgedo(IStudentAPI& api)
{
    //if (api.GetSelfInfo()->addiction > 0) // 求救1//其实快死了就不用求救了
       // return 1;
    //先判断监管者是否在旁边2
    //判断救队友3
    if (ours.workdone < 7)//写作业4
        return 1;
    else if (ours.workdone >= 7)//开门5
        return 2;
}

void AI::play(IStudentAPI& api)
{
    // 公共操作
    auto mapinfo = api.GetFullMap();

    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            FullMap[i][j] = (int)mapinfo[i][j];
            if (FullMap[i][j] == 1 || FullMap[i][j] == 3 || FullMap[i][j] == 7 || FullMap[i][j] == 8 || FullMap[i][j] == 9 || FullMap[i][j] == 10)
                WalkMap[i][j] = 1;
            else
                WalkMap[i][j] = 0;
            //cout << THUAI6::placeTypeDict[mapinfo[i][j]];
            //api.Print(fmt::format("{} ", THUAI6::placeTypeDict[mapinfo[i][j]]));
        }
        //api.Print(fmt::format("\n"));
    }


    if (this->playerID == 0)
    {
        auto selfInfo = api.GetSelfInfo();//获取自己的各种信息
        int judge = 0;//决策
        int state = (int)selfInfo->playerState;
        if (state != 2 && selfInfo->x == p0[0] && selfInfo->y == p0[1])
        {
            PosCheck(api, p_0);
            //api.EndAllAction();
        }
        p0[0] = selfInfo->x;
        p0[1] = selfInfo->y;
        judge = Judgedo(api);
        api.Print(fmt::format("I want to do:{}", judge));
        switch (judge)
        {
        case 1:
            if (api.HaveMessage())
            {
                auto msg = api.GetMessage();
                api.Print(fmt::format("Get Message from{},message:{}", msg.first, msg.second));
                //messagetrans(api, msg.second);
            }
            //以下为修机程序
            judge = Judgedo(api);

            //行动
            if (hw0[0] == 0 && hw0[1] == 0)
                student_search(api, 60, 4, hw0, ours.homework, 10);
            api.Print(fmt::format("向x={},y={}出发！", hw0[0], hw0[1]));
            if (!JudgeScope_9(api, hw0[0], hw0[1]))//!!!需要一个判断是否在判定范围的函数！！！：1-九宫格；2-正前后
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), hw0[0] - 1, hw0[1]);///////这里减了1
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }
            else
            {
                if (api.GetClassroomProgress(hw0[0], hw0[1]) != 10000000)
                {
                    api.Print(fmt::format("this homework is written {}/10000000", api.GetClassroomProgress(hw0[0], hw0[1])));
                    api.StartLearning();
                }
                else
                {
                    api.Print(fmt::format("作业（{}，{}）写完辣！！！", hw0[0], hw0[1]));
                    int po = 100 * hw0[0] + hw0[1];
                    bool flag = true;
                    for (int i = 0; i < 10; i++)
                        if (ours.homework[i] == po)
                            flag = false;
                    if (flag)
                    {
                        ours.homework[ours.workdone] = po;
                        ours.workdone++;
                    }
                    //api.SendMessage(1, fmt::format("{}", 100 * hw0[0] + hw0[1]));
                    student_search(api, 60, 4, hw0, ours.homework, 10);
                }
            }
            break;//作业没做完不搜大门
        case 2:
        {
            if (gt[0] == 0 && gt[1] == 0)
                student_search(api, 60, 5, gt, ours.gate, 2);
            api.Print(fmt::format("向x={},y={}出发！", gt[0], gt[1]));//搜大门
            if (!JudgeScope_9(api, gt[0], gt[1]))//判断是否在大门九宫格内
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), gt[0], gt[1]);
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }//不在，走去
            else
            {
                if (api.GetClassroomProgress(gt[0], gt[1]) != 10000000)
                {
                    api.Print(fmt::format("progess of opening the gate  {}/10000000", api.GetClassroomProgress(gt[0], gt[1])));
                    api.StartOpenGate();
                }//在，但没开完，继续开
                else
                {
                    api.Print(fmt::format("大门（{}，{}）打开辣！！！", gt[0], gt[1]));
                    api.SendMessage(1, fmt::format("{}", 100 * gt[0] + gt[1]));
                    ours.gate[ours.gateopen++];
                    api.Graduate();
                }//在，开完了，通知队友并逃离
            }
        }
        }
    }
    else if (this->playerID == 1)// 玩家1执行操作
    {
        auto selfInfo = api.GetSelfInfo();//获取自己的各种信息
        int judge = 0;//决策
        int state = (int)selfInfo->playerState;
        if (state != 2 && selfInfo->x == p0[0] && selfInfo->y == p0[1])
        {
            PosCheck(api, p_0);
            //api.EndAllAction();
        }
        p0[0] = selfInfo->x;
        p0[1] = selfInfo->y;
        judge = Judgedo(api);
        api.Print(fmt::format("I want to do:{}", judge));
        switch (judge)
        {
        case 1:
            if (api.HaveMessage())
            {
                auto msg = api.GetMessage();
                api.Print(fmt::format("Get Message from{},message:{}", msg.first, msg.second));
                //messagetrans(api, msg.second);
            }
            //以下为修机程序
            judge = Judgedo(api);

            //行动
            if (hw0[0] == 0 && hw0[1] == 0)
                student_search(api, 60, 4, hw0, ours.homework, 10);
            api.Print(fmt::format("向x={},y={}出发！", hw0[0], hw0[1]));
            if (!JudgeScope_9(api, hw0[0], hw0[1]))//!!!需要一个判断是否在判定范围的函数！！！：1-九宫格；2-正前后
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), hw0[0] - 1, hw0[1]);///////这里减了1
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }
            else
            {
                if (api.GetClassroomProgress(hw0[0], hw0[1]) != 10000000)
                {
                    api.Print(fmt::format("this homework is written {}/10000000", api.GetClassroomProgress(hw0[0], hw0[1])));
                    api.StartLearning();
                }
                else
                {
                    api.Print(fmt::format("作业（{}，{}）写完辣！！！", hw0[0], hw0[1]));
                    int po = 100 * hw0[0] + hw0[1];
                    bool flag = true;
                    for (int i = 0; i < 10; i++)
                        if (ours.homework[i] == po)
                            flag = false;
                    if (flag)
                    {
                        ours.homework[ours.workdone] = po;
                        ours.workdone++;
                    }
                    //api.SendMessage(1, fmt::format("{}", 100 * hw0[0] + hw0[1]));
                    student_search(api, 60, 4, hw0, ours.homework, 10);
                }
            }
            break;//作业没做完不搜大门
        case 2:
        {
            if (gt[0] == 0 && gt[1] == 0)
                student_search(api, 60, 5, gt, ours.gate, 2);
            api.Print(fmt::format("向x={},y={}出发！", gt[0], gt[1]));//搜大门
            if (!JudgeScope_9(api, gt[0], gt[1]))//判断是否在大门九宫格内
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), gt[0], gt[1]);
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }//不在，走去
            else
            {
                if (api.GetClassroomProgress(gt[0], gt[1]) != 10000000)
                {
                    api.Print(fmt::format("progess of opening the gate  {}/10000000", api.GetClassroomProgress(gt[0], gt[1])));
                    api.StartOpenGate();
                }//在，但没开完，继续开
                else
                {
                    api.Print(fmt::format("大门（{}，{}）打开辣！！！", gt[0], gt[1]));
                    api.SendMessage(1, fmt::format("{}", 100 * gt[0] + gt[1]));
                    ours.gate[ours.gateopen++];
                    api.Graduate();
                }//在，开完了，通知队友并逃离
            }
        }
        }
    }
    else if (this->playerID == 2)// 玩家2执行操作
    {
        auto selfInfo = api.GetSelfInfo();//获取自己的各种信息
        int judge = 0;//决策
        int state = (int)selfInfo->playerState;
        if (state != 2 && selfInfo->x == p0[0] && selfInfo->y == p0[1])
        {
            PosCheck(api, p_0);
            //api.EndAllAction();
        }
        p0[0] = selfInfo->x;
        p0[1] = selfInfo->y;
        judge = Judgedo(api);
        api.Print(fmt::format("I want to do:{}", judge));
        switch (judge)
        {
        case 1:
            if (api.HaveMessage())
            {
                auto msg = api.GetMessage();
                api.Print(fmt::format("Get Message from{},message:{}", msg.first, msg.second));
                //messagetrans(api, msg.second);
            }
            //以下为修机程序
            judge = Judgedo(api);

            //行动
            if (hw0[0] == 0 && hw0[1] == 0)
                student_search(api, 60, 4, hw0, ours.homework, 10);
            api.Print(fmt::format("向x={},y={}出发！", hw0[0], hw0[1]));
            if (!JudgeScope_9(api, hw0[0], hw0[1]))//!!!需要一个判断是否在判定范围的函数！！！：1-九宫格；2-正前后
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), hw0[0] - 1, hw0[1]);///////这里减了1
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }
            else
            {
                if (api.GetClassroomProgress(hw0[0], hw0[1]) != 10000000)
                {
                    api.Print(fmt::format("this homework is written {}/10000000", api.GetClassroomProgress(hw0[0], hw0[1])));
                    api.StartLearning();
                }
                else
                {
                    api.Print(fmt::format("作业（{}，{}）写完辣！！！", hw0[0], hw0[1]));
                    int po = 100 * hw0[0] + hw0[1];
                    bool flag = true;
                    for (int i = 0; i < 10; i++)
                        if (ours.homework[i] == po)
                            flag = false;
                    if (flag)
                    {
                        ours.homework[ours.workdone] = po;
                        ours.workdone++;
                    }
                    //api.SendMessage(1, fmt::format("{}", 100 * hw0[0] + hw0[1]));
                    student_search(api, 60, 4, hw0, ours.homework, 10);
                }
            }
            break;//作业没做完不搜大门
        case 2:
        {
            if (gt[0] == 0 && gt[1] == 0)
                student_search(api, 60, 5, gt, ours.gate, 2);
            api.Print(fmt::format("向x={},y={}出发！", gt[0], gt[1]));//搜大门
            if (!JudgeScope_9(api, gt[0], gt[1]))//判断是否在大门九宫格内
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), gt[0], gt[1]);
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }//不在，走去
            else
            {
                if (api.GetClassroomProgress(gt[0], gt[1]) != 10000000)
                {
                    api.Print(fmt::format("progess of opening the gate  {}/10000000", api.GetClassroomProgress(gt[0], gt[1])));
                    api.StartOpenGate();
                }//在，但没开完，继续开
                else
                {
                    api.Print(fmt::format("大门（{}，{}）打开辣！！！", gt[0], gt[1]));
                    api.SendMessage(1, fmt::format("{}", 100 * gt[0] + gt[1]));
                    ours.gate[ours.gateopen++];
                    api.Graduate();
                }//在，开完了，通知队友并逃离
            }
        }
        }
    }
    else if (this->playerID == 3)
    {
        auto selfInfo = api.GetSelfInfo();//获取自己的各种信息
        int judge = 0;//决策
        int state = (int)selfInfo->playerState;
        if (state != 2 && selfInfo->x == p0[0] && selfInfo->y == p0[1])
        {
            PosCheck(api, p_0);
            //api.EndAllAction();
        }
        p0[0] = selfInfo->x;
        p0[1] = selfInfo->y;
        judge = Judgedo(api);
        api.Print(fmt::format("I want to do:{}", judge));
        switch (judge)
        {
        case 1:
            if (api.HaveMessage())
            {
                auto msg = api.GetMessage();
                api.Print(fmt::format("Get Message from{},message:{}", msg.first, msg.second));
                //messagetrans(api, msg.second);
            }
            //以下为修机程序
            judge = Judgedo(api);

            //行动
            if (hw0[0] == 0 && hw0[1] == 0)
                student_search(api, 60, 4, hw0, ours.homework, 10);
            api.Print(fmt::format("向x={},y={}出发！", hw0[0], hw0[1]));
            if (!JudgeScope_9(api, hw0[0], hw0[1]))//!!!需要一个判断是否在判定范围的函数！！！：1-九宫格；2-正前后
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), hw0[0] - 1, hw0[1]);///////这里减了1
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }
            else
            {
                if (api.GetClassroomProgress(hw0[0], hw0[1]) != 10000000)
                {
                    api.Print(fmt::format("this homework is written {}/10000000", api.GetClassroomProgress(hw0[0], hw0[1])));
                    api.StartLearning();
                }
                else
                {
                    api.Print(fmt::format("作业（{}，{}）写完辣！！！", hw0[0], hw0[1]));
                    int po = 100 * hw0[0] + hw0[1];
                    bool flag = true;
                    for (int i = 0; i < 10; i++)
                        if (ours.homework[i] == po)
                            flag = false;
                    if (flag)
                    {
                        ours.homework[ours.workdone] = po;
                        ours.workdone++;
                    }
                    //api.SendMessage(1, fmt::format("{}", 100 * hw0[0] + hw0[1]));
                    student_search(api, 60, 4, hw0, ours.homework, 10);
                }
            }
            break;//作业没做完不搜大门
        case 2:
        {
            if (gt[0] == 0 && gt[1] == 0)
                student_search(api, 60, 5, gt, ours.gate, 2);
            api.Print(fmt::format("向x={},y={}出发！", gt[0], gt[1]));//搜大门
            if (!JudgeScope_9(api, gt[0], gt[1]))//判断是否在大门九宫格内
            {
                p_0 = Findpath(api.GridToCell(p0[0]), api.GridToCell(p0[1]), gt[0], gt[1]);
                api.Print(fmt::format("方向：{}", p_0));
                CellByCell(api, p_0);
            }//不在，走去
            else
            {
                if (api.GetClassroomProgress(gt[0], gt[1]) != 10000000)
                {
                    api.Print(fmt::format("progess of opening the gate  {}/10000000", api.GetClassroomProgress(gt[0], gt[1])));
                    api.StartOpenGate();
                }//在，但没开完，继续开
                else
                {
                    api.Print(fmt::format("大门（{}，{}）打开辣！！！", gt[0], gt[1]));
                    api.SendMessage(1, fmt::format("{}", 100 * gt[0] + gt[1]));
                    ours.gate[ours.gateopen++];
                    api.Graduate();
                }//在，开完了，通知队友并逃离
            }
        }

        }
    }
}

bool ismove = false;
int face = 0;
int sight = 2;//可以定义一个结构体

bool block(int landty)//用来返回一个格子可不可以通过 0-可以，1-不可以
{
    if (landty == 2 || landty == 4 || landty == 5 || landty == 6 || landty == 7 || landty == 11)
        return true;
    else return false;
}

void AI::play(ITrickerAPI& api)
{
    /*auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    auto selfinfo = api.GetSelfInfo();
    auto mapinfo = api.GetFullMap();
    int x = api.GridToCell(selfinfo->x);
    int dwne = api.GridToCell(selfinfo->x + 400);
    int uppe = api.GridToCell(selfinfo->x - 400);
    int y = api.GridToCell(selfinfo->y);
    int rgte = api.GridToCell(selfinfo->y + 400);
    int lfte = api.GridToCell(selfinfo->y - 400);
    bool move;
    switch (face)
    {
    case 0:
    {
        move = true;
        for (int i = 1; i <= sight && 0 <= (x + i) < 50; i++)//一个四处游荡的程序，一会把它分成函数更便于使用
        {
            int rgt = (int)api.GetFullMap()[x + i][rgte];//坐标轴正方向是什么x-向下，y-向右,逆时针
            int lft = (int)api.GetFullMap()[x + i][lfte];
            if (block(rgt) || block(lft))
            {
                move = false;
                break;
            }
        }
        ismove = move;
        if (ismove)
        {
            api.Move(50, face * 3.141592 / 2);
        }
        else face += 1;
        break;
    }
    case 1:
    {
        move = true;
        for (int i = 1; i <= sight && 0 <= (y + i) < 50; i++)//一个四处游荡的程序，一会把它分成函数更便于使用
        {
            int upp = (int)api.GetFullMap()[uppe][y + i];//坐标轴正方向是什么x-向下，y-向右,逆时针
            int dwn = (int)api.GetFullMap()[dwne][y + i];
            if (block(upp) || block(dwn))//仅列出所有不能通过的物块，不考虑是否有用
            {
                move = false;
                break;
            }
        }
        ismove = move;
        if (ismove)
        {
            api.Move(50, face * 3.141592 / 2);
        }
        else face += 1;
        break;
    }
    case 2:
    {
        move = true;
        for (int i = 1; i <= sight && 0 <= (x - i) < 50; i++)//一个四处游荡的程序，一会把它分成函数更便于使用
        {
            int rgt = (int)api.GetFullMap()[x - i][rgte];//坐标轴正方向是什么x-向下，y-向右,逆时针
            int lft = (int)api.GetFullMap()[x - i][lfte];
            if (block(rgt) || block(lft))//仅列出所有不能通过的物块，不考虑是否有用
            {
                move = false;
                break;
            }
        }
        ismove = move;
        if (ismove)
        {
            api.Move(50, face * 3.141592 / 2);
        }
        else face += 1;
        break;
    }
    case 3:
    {
        move = true;
        for (int i = 1; i <= sight && 0 <= (y - i) < 50; i++)//一个四处游荡的程序，一会把它分成函数更便于使用
        {
            int upp = (int)api.GetFullMap()[uppe][y - i];//坐标轴正方向是什么x-向下，y-向右,逆时针
            int dwn = (int)api.GetFullMap()[dwne][y - i];
            if (block(upp) || block(dwn))//仅列出所有不能通过的物块，不考虑是否有用
            {
                move = false;
                break;
            }
        }
        ismove = move;
        if (ismove)
        {
            api.Move(50, face * 3.141592 / 2);
        }
        else face = 0;
        break;
    }
    }*/
}

int* Triker_find(ITrickerAPI& api)
{
    int result[2] = { 0,0 };
    //找人
    //result[0]=api.GetStudents()->x;
    //找没写完的作业
    api.GetClassroomProgress(22, 18);//这里乱加了一点东西
    //没找到就算了

    return result;
}

int* Triker_search(ITrickerAPI& api1, int des, int typ)//搜索附近的东西（type），写完了作业互相通知一声！
{
    int x = api1.GridToCell(api1.GetSelfInfo()->x);
    int y = api1.GridToCell(api1.GetSelfInfo()->y);
    int pos[2] = { 0 };
    auto fullmap = api1.GetFullMap();
    for (int d = 1; d <= des; d++)
        for (int i = 0; i <= d; i++)
        {
            int j = d - i;
            if ((x + i) < 50 && (y + j) < 50)//判断不越界
                if ((int)fullmap[x + i][y + j] == typ)
                {
                    pos[0] = x + i;//横坐标
                    pos[1] = y + j;//纵坐标
                    api1.Print(fmt::format("here is a homework:({},{})", x + i, y + j));
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (ours.homework[i] == hw0[0] * 100 + hw0[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid) return pos;
                }
            if ((x + i) < 50 && (y - j) >= 0)//判断不越界 
                if ((int)fullmap[x + i][y - j] == typ)
                {
                    pos[0] = x + i;//横坐标
                    pos[1] = y - j;//纵坐标
                    api1.Print(fmt::format("here is a homework:({},{})", x + i, y - j));
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (ours.homework[i] == hw0[0] * 100 + hw0[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid) return pos;
                }
            if ((x - i) >= 0 && (y + j) < 50)//判断不越界
                if ((int)fullmap[x - i][y + j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y + j;//纵坐标
                    api1.Print(fmt::format("here is a homework:({},{})", x - i, y + j));
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (ours.homework[i] == hw0[0] * 100 + hw0[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid) return pos;
                }
            if ((x - i) >= 0 && (y - j) >= 0)//判断不越界
                if ((int)fullmap[x - i][y - j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y - j;//纵坐标
                    api1.Print(fmt::format("here is a homework:({},{})", x - i, y - j));
                    bool valid = true;
                    for (int i = 0; i < 10; i++)//看看这个作业写完没有
                        if (ours.homework[i] == hw0[0] * 100 + hw0[1])
                        {
                            valid = false;
                            break;
                        }
                    if (valid) return pos;
                }
        }
    return pos;//什么都没找到返回（0，0）
}
