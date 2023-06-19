#include <vector>
#include <thread>
#include <array>
#include<iostream>
#include<cmath>
#include <thread>
#include <queue>
#include "AI.h"
#include "constants.h"
using namespace std;

// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新，大致一帧更新一次
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Idol;

// 可以在AI.cpp内部声明变量与函数
int* Tricker_searchhw(ITrickerAPI& api1, int des, int typ)//搜索附近的作业
{
    int x = api1.GridToCell(api1.GetSelfInfo()->x);
    int y = api1.GridToCell(api1.GetSelfInfo()->y);
    int pos[2];
    pos[0] = 0;
    pos[1] = 0;
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
                    int p = api1.GetClassroomProgress(pos[0], pos[1]);
                    if (p != 100 && p > 10)
                        return pos;
                }
            if ((x + i) < 50 && (y - j) >= 0)//判断不越界
                if ((int)fullmap[x + i][y - j] == typ)
                {
                    pos[0] = x + i;//横坐标
                    pos[1] = y - j;//纵坐标
                    int p = api1.GetClassroomProgress(pos[0], pos[1]);
                    if (p != 100 && p > 10)
                        return pos;
                }
            if ((x - i) >= 0 && (y + j) < 50)//判断不越界
                if ((int)fullmap[x - i][y + j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y + j;//纵坐标
                    int p = api1.GetClassroomProgress(pos[0], pos[1]);
                    if (p != 100 && p > 10)
                        return pos;
                }
            if ((x - i) >= 0 && (y - j) >= 0)//判断不越界
                if ((int)fullmap[x - i][y - j] == typ)
                {
                    pos[0] = x - i;//横坐标
                    pos[1] = y - j;//纵坐标
                    int p = api1.GetClassroomProgress(pos[0], pos[1]);
                    if (p != 100 && p > 10)
                        return pos;
                }
        }
    }

    return pos;//什么都没找到返回（0，0）
}

void Tricker_CellByCell(ITrickerAPI& api1, int p)//一次走一格
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
            api1.MoveDown(20);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        break;
    }
    case 1:
    {   aim_g = (y + 1) * 1000 + 500;
    if ((aim_g - api1.GetSelfInfo()->y) > 100)
    {
        api1.MoveRight(20);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    break;
    }
    case 2:
    {
        aim_g = (x - 1) * 1000 + 500;
        if ((api1.GetSelfInfo()->x - aim_g) > 100)
        {
            api1.MoveUp(20);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        break;
    }
    case 3:
    {
        aim_g = (y - 1) * 1000 + 500;
        if ((api1.GetSelfInfo()->x - aim_g) > 100)
        {
            api1.MoveLeft(20);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        break;
    }
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



void AI::play(IStudentAPI& api)  // 每帧执行一次AI::play(IStudentAPI& api)或AI::play(ITrickerAPI& api)（除非执行该函数超过一帧50ms），获取的信息都是这一帧的开始的状态
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
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

int cal_distance(int x, int y, int xx, int yy)
{
    int distance = sqrt((x - xx) * (x - xx) + (y - yy) * (y - yy));
    return distance;
};//两点间的距离

double cal_angle(int x, int y, int xx, int yy) {
    double d = (double)cal_distance(x, y, xx, yy);
    if (yy >= y) return(acos((xx - x) / d));
    else return(4 * asin(1) - acos((xx - x) / d));
};//计算两点之间角度的函数




void AI::play(ITrickerAPI& api)
{
    void Tricker_attack(ITrickerAPI & api, int target_x, int target_y);

    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();


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


    int x = api.GetSelfInfo()->x;
    int y = api.GetSelfInfo()->y;

    auto stu = api.GetStudents();
    //auto hw = Tricker_searchhw(api, 14300, 2);
    api.Print(fmt::format("找到学生坐标（{}，{})", stu[0]->x, stu[0]->y));
    auto target_x = stu[0]->x;
    auto target_y = stu[0]->y;
    Tricker_attack(api, target_x, target_y);

}

void Tricker_attack(ITrickerAPI& api, int target_x, int target_y)
{
    int x = api.GetSelfInfo()->x;
    int y = api.GetSelfInfo()->y;
    api.Print(fmt::format("目前坐标（{}，{})", x, y));
    int dis = cal_distance(x, y, target_x, target_y);
    int k = 0;
    auto fullmap = api.GetFullMap();
    double a = acos(0.9);
    cout << a << endl;
    if (dis < 2200)
    {
        api.Print(fmt::format("the distance is{}", dis));
        double dir = cal_angle(x, y, target_x, target_y);
        api.Print(fmt::format("the angel is{}", dir));
        api.Attack(dir);
    }
    else
    {
        k = Findpath(x / 1000, y / 1000, target_x / 1000, target_y / 1000);
        Tricker_CellByCell(api, k);
    }
}