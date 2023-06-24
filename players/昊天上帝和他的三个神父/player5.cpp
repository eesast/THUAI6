#include <vector>
#include <thread>
#include <array>
#include <queue>
#include "AI.h"
#include "constants.h"
// 注意不要使用conio.h，Windows.h等非标准库

#include<chrono>
#include<iostream>
#include<cmath>
#include <string>
#include <sstream>
#define MAXINT 2147483647

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;

// 可以在AI.cpp内部声明变量与函数
const double PI = 3.1415926535897932384626433832795;

double sqr(double x) { return x * x; }
double getAngle(double x, double y) { return atan2(y, x); }
double getDis(double x, double y) { return sqrt(sqr(x) + sqr(y)); }
bool inNearAngle(double angle, double beta)
{
    int t = floor((angle - beta) / 2 / PI);
    beta += t * 2 * PI;
    return (angle - beta) <= 0.6 * PI || (2 * PI - (angle - beta) <= 0.6 * PI);
}

bool moveFlag = false;
const int maxN = 50; //地图cell大小

THUAI6::PlaceType map[maxN][maxN]; //记录地图信息
bool canMove[maxN][maxN]; //记录该cell是否可以移动
bool trickerCanMove[maxN][maxN]; //记录该cell是否可以移动
int canMovePre[maxN][maxN];
int area[maxN][maxN]; //记录该cell所属区域

int dis[maxN][maxN], vis[maxN][maxN], idx, trickerDis[maxN][maxN];
struct node //记录cell坐标
{
    int x, y;
    node(int x = 0, int y = 0) : x(x), y(y) {}
    node(const node& a) : x(a.x), y(a.y) {}
    bool operator == (const node& a) const
    {
        return x == a.x && y == a.y;
    }
    bool operator != (const node& a) const
    {
        return !(*this == a);
    }
    bool operator < (const node& a) const
    {
        return dis[x][y] < dis[a.x][a.y];
    }
    node operator + (const node& a) const
    {
        return node(x + a.x, y + a.y);
    }
}Q[maxN * maxN], pre[maxN][maxN]; // 实际是cell的坐标

std::vector<node> doorNode;
std::vector<node> classroomNode;
std::vector<node> chestNode;
std::vector<node> grassNode;
std::vector<node> hideNode;
std::vector<node> gatenode;
std::vector<node> hiddengatenode;
node hidenode;

const node emptynode = node(-1, -1);
const node dir[8] = { node(0, 1), node(0, -1), node(1, 0), node(-1, 0), node(1, 1), node(1, -1), node(-1, 1), node(-1, -1) };

bool Cross[maxN][maxN][maxN][maxN];

node findNearPlace(const node& st, THUAI6::PlaceType placeType)
{
    for (int i = 0; i < 8; i++)
    {
        node ed = st + dir[i];
        if (map[ed.x][ed.y] == placeType) return ed;
    }
    return emptynode;
}

bool IsNearPlace(const node& st, const node& dst)
{
    if (st == dst) return true;
    for (int i = 0; i < 8; i++)
    {
        node ed = st + dir[i];
        if (ed == dst) return true;
    }
    return false;
}

bool nearCell(const node& a, const node& b, int t) // t:方向数
{
    for (int i = 0; i < t; i++)
    {
        if (b == a + dir[i]) return true;
    }
    return false;
}

void init_Cross(bool Cro[maxN][maxN][maxN][maxN])
{
    for (int i = 1; i < 49; i++)
        for (int j = 1; j < 49; j++)
            for (int u = i; u < 49; u++)
                for (int v = 1; v < 49; v++)
                {
                    double beta = atan2(v - j, u - i) + PI / 2;
                    double S = sin(beta), C = cos(beta);
                    bool crossflg = 0;
                    if (i < u && j != v) {
                        double L = 1.0 * (v - j) / (u - i);
                        for (int o = i + 1; o <= u && !crossflg; o++)
                        {
                            int x = o, y = j + 0.5 + (o - i - 0.5) * L;
                            if (!canMove[x - 1][y] || !canMove[x][y]) crossflg = 1;
                        }
                        for (int o = (int)(i + 0.5 + C * 0.5) + 1; o <= (int)(u + 0.5 + C * 0.5) && !crossflg; o++)
                        {
                            int x = o, y = j + 0.5 + S * 0.5 + (o - i - 0.5 - C * 0.5) * L;
                            if (!canMove[x - 1][y] || !canMove[x][y]) crossflg = 1;
                        }
                        for (int o = (int)(i + 0.5 - C * 0.5) + 1; o <= (int)(u + 0.5 - C * 0.5) && !crossflg; o++)
                        {
                            int x = o, y = j + 0.5 - S * 0.5 + (o - i - 0.5 + C * 0.5) * L;
                            if (!canMove[x - 1][y] || !canMove[x][y]) crossflg = 1;
                        }
                        L = 1.0 * (u - i) / (v - j);
                        int I = (j < v ? i : u), J = (j < v ? j : v);
                        for (int o = std::min(v, j) + 1, lim = std::max(v, j); o <= lim && !crossflg; o++)
                        {
                            int y = o, x = I + 0.5 + (o - J - 0.5) * L;
                            if (!canMove[x][y - 1] || !canMove[x][y]) crossflg = 1;
                        }
                        for (int o = (int)(std::min(v, j) + 0.5 + S * 0.5) + 1; o <= (int)(std::max(v, j) + 0.5 + S * 0.5) && !crossflg; o++)
                        {
                            int y = o, x = I + 0.5 + C * 0.5 + (o - J - 0.5 - S * 0.5) * L;
                            if (!canMove[x][y - 1] || !canMove[x][y]) crossflg = 1;
                        }
                        for (int o = (int)(std::min(v, j) + 0.5 - S * 0.5) + 1; o <= (int)(std::max(v, j) + 0.5 - S * 0.5) && !crossflg; o++)
                        {
                            int y = o, x = I + 0.5 - C * 0.5 + (o - J - 0.5 + S * 0.5) * L;
                            if (!canMove[x][y - 1] || !canMove[x][y]) crossflg = 1;
                        }
                    }
                    else if (i < u) //竖直线特判
                    {
                        for (int o = i; o <= u && !crossflg; o++)
                            if (!canMove[o][j]) crossflg = 1;
                    }
                    else if (j != v) //水平线特判
                    {
                        for (int o = std::min(j, v), lim = std::max(j, v); o <= lim && !crossflg; o++)
                            if (!canMove[i][o]) crossflg = 1;
                    }
                    Cro[i][j][u][v] = Cro[u][v][i][j] = crossflg;
                    if (map[u][v] == THUAI6::PlaceType::Window && nearCell(node(i, j), node(u, v), 4))
                        Cro[i][j][u][v] = 0;
                    if (map[i][j] == THUAI6::PlaceType::Window && nearCell(node(i, j), node(u, v), 4))
                        Cro[u][v][i][j] = 0;
                    if ((map[u][v] == THUAI6::PlaceType::Door3 || map[i][j] == THUAI6::PlaceType::Door3) && nearCell(node(i, j), node(u, v), 4))
                        Cro[i][j][u][v] = Cro[u][v][i][j] = 0;
                    if ((map[u][v] == THUAI6::PlaceType::Door5 || map[i][j] == THUAI6::PlaceType::Door5) && nearCell(node(i, j), node(u, v), 4))
                        Cro[i][j][u][v] = Cro[u][v][i][j] = 0;
                    if ((map[u][v] == THUAI6::PlaceType::Door6 || map[i][j] == THUAI6::PlaceType::Door6) && nearCell(node(i, j), node(u, v), 4))
                        Cro[i][j][u][v] = Cro[u][v][i][j] = 0;
                    //增加对窗户和门Cross判断
                }
}

void InitArea(node st, int areaId) //使用bfs将封闭区域划分成不同的编号
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    vis[st.x][st.y] = idx;
    area[st.x][st.y] = areaId;
    while (head < tail)
    {
        node now = Q[head++];
        int len = dis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (map[nxt.x][nxt.y] != THUAI6::PlaceType::Wall)) // 判断窗户
            {
                vis[nxt.x][nxt.y] = idx;
                dis[nxt.x][nxt.y] = len + 1;
                Q[tail++] = nxt;
            }
        }
    }

}

bool Direct(node a, node b)
{
    return !Cross[a.x][a.y][b.x][b.y];
}

node findNearCanMovePlaceWithMinDis(const node& dst) // 需要先计算Dis
{
    node minNode = dst;
    int minDis = MAXINT;
    for (int i = 0; i < 8; i++)
    {
        node ed = dst + dir[i];
        if (canMove[ed.x][ed.y] && dis[ed.x][ed.y] < minDis)
        {
            minDis = dis[ed.x][ed.y];
            minNode = ed;
        }
    }
    return minNode;
}

void BFSPath(node st) //BFS寻路
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    dis[st.x][st.y] = 0;
    while (head < tail)
    {
        node now = Q[head++];
        int len = dis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (canMove[nxt.x][nxt.y] || map[nxt.x][nxt.y] == THUAI6::PlaceType::Window)) // 判断窗户
            {
                vis[nxt.x][nxt.y] = idx;
                dis[nxt.x][nxt.y] = len + 1;
                pre[nxt.x][nxt.y] = now;
                Q[tail++] = nxt;
            }
        }
    }
}

void BFSTrickerPath(node st) //BFS寻路
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    trickerDis[st.x][st.y] = 0;
    while (head < tail)
    {
        node now = Q[head++];
        int len = trickerDis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (trickerCanMove[nxt.x][nxt.y] || map[nxt.x][nxt.y] == THUAI6::PlaceType::Window)) // 判断窗户
            {
                vis[nxt.x][nxt.y] = idx;
                trickerDis[nxt.x][nxt.y] = len + 1;
                pre[nxt.x][nxt.y] = now;
                Q[tail++] = nxt;
            }
        }
    }
}

void BFSTarget(node st) //BFS寻目标
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    dis[st.x][st.y] = 0;
    while (head < tail)
    {
        node now = Q[head++];
        int len = dis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (canMove[nxt.x][nxt.y] || map[nxt.x][nxt.y] == THUAI6::PlaceType::Window // 判断窗户
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::ClassRoom || map[nxt.x][nxt.y] == THUAI6::PlaceType::Chest
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::Gate || map[nxt.x][nxt.y] == THUAI6::PlaceType::HiddenGate))
            {
                vis[nxt.x][nxt.y] = idx;
                dis[nxt.x][nxt.y] = len + 1;
                pre[nxt.x][nxt.y] = now;
                Q[tail++] = nxt;
            }
        }
    }
}

void BFSTrickerTarget(node st) //BFS寻目标
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    trickerDis[st.x][st.y] = 0;
    while (head < tail)
    {
        node now = Q[head++];
        int len = trickerDis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (trickerCanMove[nxt.x][nxt.y] || map[nxt.x][nxt.y] == THUAI6::PlaceType::Window // 判断窗户
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::ClassRoom || map[nxt.x][nxt.y] == THUAI6::PlaceType::Chest
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::Gate || map[nxt.x][nxt.y] == THUAI6::PlaceType::HiddenGate))
            {
                vis[nxt.x][nxt.y] = idx;
                trickerDis[nxt.x][nxt.y] = len + 1;
                pre[nxt.x][nxt.y] = now;
                Q[tail++] = nxt;
            }
        }
    }
}

void BFSHide(node st)
{
    idx++;
    int head = 0, tail = 0;
    Q[tail++] = st;
    dis[st.x][st.y] = 0;
    while (head < tail)
    {
        node now = Q[head++];
        int len = dis[now.x][now.y];
        for (int i = 0; i < 4; i++)
        {
            node nxt = now + dir[i];
            if (nxt.x < 0 || nxt.x >= maxN || nxt.y < 0 || nxt.y >= maxN) continue;
            if (vis[nxt.x][nxt.y] < idx && (canMove[nxt.x][nxt.y] || map[nxt.x][nxt.y] == THUAI6::PlaceType::Window // 判断窗户
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::Door3 || map[nxt.x][nxt.y] == THUAI6::PlaceType::Door5
                || map[nxt.x][nxt.y] == THUAI6::PlaceType::Door6 || map[nxt.x][nxt.y] == THUAI6::PlaceType::Grass))
            {
                vis[nxt.x][nxt.y] = idx;
                dis[nxt.x][nxt.y] = len + 1;
                pre[nxt.x][nxt.y] = now;
                Q[tail++] = nxt;
            }
        }
    }
}

void MoveToCenter(IAPI& api, std::shared_ptr<const THUAI6::Player>self)
{
    node st = node(api.CellToGrid(self->x), api.CellToGrid(self->y));
    node now = node(self->x, self->y);
    bool Around = false;
    for (int i = -1; i <= 1 && !Around; i++)
        for (int j = -1; j <= 1; j++)
            if (!canMove[st.x + i][st.y + j])
            {
                Around = true;
                break;
            }
    if (Around)
    {
        api.EndAllAction();
        int mx = self->x % 1000, my = self->y % 1000;
        if (now.x < st.x) {
            int xx = api.GridToCell(self->x - 1000);
            if (my < 500) {
                int yy = api.GridToCell(self->y - 1000);
                if (!canMove[xx][yy]) api.MoveRight((500 - my) * 1000.0 / self->speed + 0.9);
            }
            else if (my > 500) {
                int yy = api.GridToCell(self->y + 1000);
                if (!canMove[xx][yy]) api.MoveLeft((my - 500) * 1000.0 / self->speed + 0.9);
            }
            //api.MoveUp(200);
        }
        if (now.x > st.x) {
            int xx = api.GridToCell(self->x + 1000);
            if (my < 500) {
                int yy = api.GridToCell(self->y - 1000);
                if (!canMove[xx][yy]) api.MoveRight((500 - my) * 1000.0 / self->speed + 0.9);
            }
            else if (my > 500) {
                int yy = api.GridToCell(self->y + 1000);
                if (!canMove[xx][yy]) api.MoveLeft((my - 500) * 1000.0 / self->speed + 0.9);
            }
            //api.MoveDown(200);
        }
        if (now.y < st.y) {
            int yy = api.GridToCell(self->y - 1000);
            if (mx < 500) {
                int xx = api.GridToCell(self->x - 1000);
                if (!canMove[xx][yy]) api.MoveDown((500 - mx) * 1000.0 / self->speed + 0.9);
            }
            else if (mx > 500) {
                int xx = api.GridToCell(self->x + 1000);
                if (!canMove[xx][yy]) api.MoveUp((mx - 500) * 1000.0 / self->speed + 0.9);
            }
            //api.MoveLeft(200);
        }
        if (now.y > st.y) {
            int yy = api.GridToCell(self->y + 1000);
            if (mx < 500) {
                int xx = api.GridToCell(self->x - 1000);
                if (!canMove[xx][yy]) api.MoveDown((500 - mx) * 1000.0 / self->speed + 0.9);
            }
            else if (mx > 500) {
                int xx = api.GridToCell(self->x + 1000);
                if (!canMove[xx][yy]) api.MoveUp((mx - 500) * 1000.0 / self->speed + 0.9);
            }
            //api.MoveRight(200);
        }
    }
}

void Move(IAPI& api, std::shared_ptr<const THUAI6::Player>self, node dst)
{
    node goal = dst; // 设定移动目标（待设定）
    node st = node(api.GridToCell(self->x), api.GridToCell(self->y));

    for (auto& u : doorNode)
    {
        if (api.HaveView(api.CellToGrid(u.x), api.CellToGrid(u.y)))
            canMove[u.x][u.y] = api.IsDoorOpen(u.x, u.y), trickerCanMove[u.x][u.y] = api.IsDoorOpen(u.x, u.y); //含视野，注意队间通信
    }

    BFSPath(st);
    node now = goal;
    while (!Direct(st, now)) now = pre[now.x][now.y];

    if (now != st)
    {
        bool Around = false;
        for (int i = -1; i <= 1 && !Around; i++)
            for (int j = -1; j <= 1; j++)
                if (!canMove[st.x + i][st.y + j])
                {
                    Around = true;
                    break;
                }
        api.EndAllAction();
        if (Around)
        {
            int mx = self->x % 1000, my = self->y % 1000;
            if (now.x < st.x) {
                int xx = api.GridToCell(self->x - 1000);
                if (my < 500) {
                    int yy = api.GridToCell(self->y - 1000);
                    if (!canMove[xx][yy]) api.MoveRight((500 - my) * 1000.0 / self->speed + 0.9);
                }
                else if (my > 500) {
                    int yy = api.GridToCell(self->y + 1000);
                    if (!canMove[xx][yy]) api.MoveLeft((my - 500) * 1000.0 / self->speed + 0.9);
                }
                //api.MoveUp(200);
            }
            if (now.x > st.x) {
                int xx = api.GridToCell(self->x + 1000);
                if (my < 500) {
                    int yy = api.GridToCell(self->y - 1000);
                    if (!canMove[xx][yy]) api.MoveRight((500 - my) * 1000.0 / self->speed + 0.9);
                }
                else if (my > 500) {
                    int yy = api.GridToCell(self->y + 1000);
                    if (!canMove[xx][yy]) api.MoveLeft((my - 500) * 1000.0 / self->speed + 0.9);
                }
                //api.MoveDown(200);
            }
            if (now.y < st.y) {
                int yy = api.GridToCell(self->y - 1000);
                if (mx < 500) {
                    int xx = api.GridToCell(self->x - 1000);
                    if (!canMove[xx][yy]) api.MoveDown((500 - mx) * 1000.0 / self->speed + 0.9);
                }
                else if (mx > 500) {
                    int xx = api.GridToCell(self->x + 1000);
                    if (!canMove[xx][yy]) api.MoveUp((mx - 500) * 1000.0 / self->speed + 0.9);
                }
                //api.MoveLeft(200);
            }
            if (now.y > st.y) {
                int yy = api.GridToCell(self->y + 1000);
                if (mx < 500) {
                    int xx = api.GridToCell(self->x - 1000);
                    if (!canMove[xx][yy]) api.MoveDown((500 - mx) * 1000.0 / self->speed + 0.9);
                }
                else if (mx > 500) {
                    int xx = api.GridToCell(self->x + 1000);
                    if (!canMove[xx][yy]) api.MoveUp((mx - 500) * 1000.0 / self->speed + 0.9);
                }
                //api.MoveRight(200);
            }
        }
        // 考虑翻窗
        api.Move(80, getAngle(now.x - st.x, now.y - st.y));
        if (map[now.x][now.y] == THUAI6::PlaceType::Window && nearCell(now, st, 4)) api.SkipWindow();
    }
    //std::cout << "ENDMOVE" << std::endl;
}

void Init(IAPI& api, std::shared_ptr<const THUAI6::Player>self)
{
    auto tmpMap = api.GetFullMap();
    for (int i = 0; i < maxN; i++)
        for (int j = 0; j < maxN; j++)
        {
            map[i][j] = tmpMap[i][j];
            if (map[i][j] != THUAI6::PlaceType::Wall && map[i][j] != THUAI6::PlaceType::Chest
                && map[i][j] != THUAI6::PlaceType::Gate && map[i][j] != THUAI6::PlaceType::HiddenGate
                && map[i][j] != THUAI6::PlaceType::Window && map[i][j] != THUAI6::PlaceType::ClassRoom)
            {
                canMove[i][j] = 1;
                trickerCanMove[i][j] = 1;
                canMovePre[i][j] = 0;
            }
            else {
                canMove[i][j] = 0;
                trickerCanMove[i][j] = 0;
                canMovePre[i][j] = MAXINT;
            }
            if (map[i][j] == THUAI6::PlaceType::Door3 || map[i][j] == THUAI6::PlaceType::Door5 || map[i][j] == THUAI6::PlaceType::Door6)
                doorNode.push_back(node(i, j));
            else if (map[i][j] == THUAI6::PlaceType::ClassRoom)
                classroomNode.push_back(node(i, j));
            else if (map[i][j] == THUAI6::PlaceType::Gate)
                gatenode.push_back(node(i, j));
            else if (map[i][j] == THUAI6::PlaceType::HiddenGate)
                hiddengatenode.push_back(node(i, j));
            else if (!(i == 49 && j == 20) && map[i][j] == THUAI6::PlaceType::Grass || map[i][j] == THUAI6::PlaceType::Door3
                || map[i][j] == THUAI6::PlaceType::Door5 || map[i][j] == THUAI6::PlaceType::Door6 || map[i][j] == THUAI6::PlaceType::Window)
                hideNode.push_back(node(i, j));          //隐蔽点

        }// 注意门的开闭和翻窗
    init_Cross(Cross);
}

bool noTrickerNear(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto tricker = api.GetTrickers();
    if (!tricker.empty()) return false;
    double disToTricker = Constants::StraightAStudent::alertnessRadius / self->dangerAlert;
    if (disToTricker < 5000)
    {
        return false;
    }
    return true;
}

bool canReach(IAPI& api, int sx, int sy, int ex, int ey, double maxdis, int r)
{
    double dis = getDis(ex - sx, ey - sy);
    double angle = getAngle(ex - sx, ey - sy);
    for (int i = 0; i <= dis / 10; i++)
    {
        double ndis = i * 10;
        int nx = sx + ndis * cos(angle), ny = sy + ndis * sin(angle);
        //std::cout << nx << " " << ny << std::endl;
        node now = node(api.GridToCell(nx), api.GridToCell(ny));
        if (r == 0)
        {
            if (!trickerCanMove[now.x][now.y])	return false;
        }
        else {
            if (!trickerCanMove[now.x][now.y])	return false;
            for (int k = 0; k < 8; k++)
            {
                node t = now + dir[k];
                if (t.x < 0 || t.x > maxN || t.y < 0 || t.y > maxN)	continue;
                int lx = (api.CellToGrid(t.x) + api.CellToGrid(now.x)) / 2, ly = (api.CellToGrid(t.y) + api.CellToGrid(now.y)) / 2;
                if (api.CellToGrid(t.x) == api.CellToGrid(now.x)) lx = nx;
                if (api.CellToGrid(t.y) == api.CellToGrid(now.y)) ly = ny;
                if (getDis(nx - lx, ny - ly) < double(r) && !trickerCanMove[t.x][t.y]) return false;
            }
        }
    }
    return true;
}

bool canReachAngle(IAPI& api, int sx, int sy, double angle, double maxdis, int r)
{
    for (int i = 0; i <= int(maxdis / 10) + 1; i++)
    {
        double ndis = i * 10;
        int nx = sx + ndis * cos(angle), ny = sy + ndis * sin(angle);
        if (i == int(maxdis / 10) + 1) nx = sx, ny = sy;
        //std::cout << nx << " " << ny << std::endl;
        node now = node(api.GridToCell(nx), api.GridToCell(ny));
        if (r == 0)
        {
            if (!trickerCanMove[now.x][now.y])	return false;
        }
        else {
            if (!trickerCanMove[now.x][now.y])	return false;
            for (int k = 0; k < 8; k++)
            {
                node t = now + dir[k];
                if (t.x < 0 || t.x > maxN || t.y < 0 || t.y > maxN)	continue;
                int lx = (api.CellToGrid(t.x) + api.CellToGrid(now.x)) / 2, ly = (api.CellToGrid(t.y) + api.CellToGrid(now.y)) / 2;
                if (api.CellToGrid(t.x) == api.CellToGrid(now.x)) lx = nx;
                if (api.CellToGrid(t.y) == api.CellToGrid(now.y)) ly = ny;
                if (getDis(nx - lx, ny - ly) < double(r) && !trickerCanMove[t.x][t.y]) return false;
            }
        }
    }
    return true;
}

bool edge(const node& st)
{
    return st.x == 0 || st.x == maxN - 1 || st.y == 0 || st.y == maxN - 1
        || st.x == 1 || st.x == maxN - 2 || st.y == 1 || st.y == maxN - 2
        || st.x == 2 || st.x == maxN - 3 || st.y == 2 || st.y == maxN - 3;
}

node findBestPlace(IStudentAPI& api, node tricker)
{
    auto self = api.GetSelfInfo();
    node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
    BFSPath(st);
    if (tricker != emptynode) BFSTrickerPath(tricker);
    int maxDis = 0;
    node ans = emptynode;
    for (int u = 0; u <= 10; u++)
        for (int v = 0; v <= 10; v++)
            for (int ud = -1; ud <= 1; ud += 2)
                for (int vd = -1; vd <= 1; vd += 2)
                {
                    int i = u * ud, j = v * vd;
                    int X = st.x, Y = st.y;
                    //bool ca = canReach(api, st.x, st.y, api.CellToGrid(tricker.x), api.CellToGrid(tricker.y), 5000, 0);
                    if (tricker != emptynode && (trickerDis[X + i][Y + j] <= trickerDis[st.x][st.y]
                        || sqr(dis[X + i][Y + j]) > sqr(trickerDis[X + i][Y + j]) + sqr(trickerDis[st.x][st.y]))) continue;
                    if (tricker != emptynode && map[st.x][st.y] == THUAI6::PlaceType::Grass && trickerDis[st.x][st.y] >= 4 && trickerDis[st.x][st.y] <= 12)
                        if (map[tricker.x][tricker.y] != THUAI6::PlaceType::Grass && map[X + i][Y + j] != THUAI6::PlaceType::Grass)
                            continue;
                    if (X + i >= 0 && X + i < 50 && Y + j >= 0 && Y + j < 50 && (canMove[X + i][Y + j] || map[X + i][Y + j] == THUAI6::PlaceType::Window))
                    {
                        node end = node(X + i, Y + j);
                        //int nowDis = (tricker != emptynode) * (trickerDis[X + i][Y + j] - dis[X + i][Y + j]) +
                            //(8 + (tricker == emptynode) * 3 - 2 * (tricker != emptynode) ) * (map[X + i][Y + j] == THUAI6::PlaceType::Window)
                            //+ 6 * (tricker != emptynode) * trickerDis[X + i][Y + j] * (int)(map[X + i][Y + j] == THUAI6::PlaceType::Grass)
                            //+ (tricker != emptynode) * trickerDis[X + i][Y + j] * (!ca);
                        int nowDis = (tricker != emptynode) * 2 * trickerDis[X + i][Y + j] - dis[X + i][Y + j] +
                            (1 + (tricker == emptynode) * 3 - 2 * (tricker != emptynode) * (Cross[tricker.x][tricker.y][X + i][Y + j] * 2 - 1)) * (map[X + i][Y + j] == THUAI6::PlaceType::Window)
                            + !edge(end) * (5 + trickerDis[X + i][Y + j] - dis[X + i][Y + j]) * (tricker != emptynode) * (int)(map[X + i][Y + j] == THUAI6::PlaceType::Grass);
                        //+ (tricker != emptynode) * 5 * (!ca);
                        if (nowDis > maxDis)
                        {
                            maxDis = nowDis;
                            ans = node(X + i, Y + j);
                        }
                    }

                }
    if (ans != emptynode) return ans;
    maxDis = 0;
    ans = emptynode;
    for (int u = 0; u <= 10; u++)
        for (int v = 0; v <= 10; v++)
            for (int ud = -1; ud <= 1; ud += 2)
                for (int vd = -1; vd <= 1; vd += 2)
                {
                    int i = u * ud, j = v * vd;
                    int X = st.x, Y = st.y;
                    if (X + i >= 0 && X + i < 50 && Y + j >= 0 && Y + j < 50 && (canMove[X + i][Y + j] || map[X + i][Y + j] == THUAI6::PlaceType::Window))
                    {
                        int nowDis = (tricker != emptynode) * 2 * trickerDis[X + i][Y + j] - dis[X + i][Y + j] +
                            (1 + (tricker == emptynode) * 3 - 2 * (tricker != emptynode) * (Cross[tricker.x][tricker.y][X + i][Y + j] * 2 - 1)) * (map[X + i][Y + j] == THUAI6::PlaceType::Window)
                            + (20 - trickerDis[X + i][Y + j]) * (tricker != emptynode) * (int)(map[X + i][Y + j] == THUAI6::PlaceType::Grass);
                        if (nowDis > maxDis)
                        {
                            maxDis = nowDis;
                            ans = node(X + i, Y + j);
                        }
                    }

                }
    return ans;    
}

node findBestTarget(IStudentAPI& api, node tricker)
{
    auto self = api.GetSelfInfo();
    node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
    BFSTarget(st);
    BFSTrickerTarget(tricker);
    int maxDis = 0;
    node ans = emptynode;
    for (int u = 0; u <= 15; u++)
        for (int v = 0; v <= 15; v++)
            for (int ud = -1; ud <= 1; ud += 2)
                for (int vd = -1; vd <= 1; vd += 2)
                {
                    int i = u * ud, j = v * vd;
                    int X = st.x, Y = st.y;
                    if (X + i >= 0 && X + i < 50 && Y + j >= 0 && Y + j < 50 && (canMove[X + i][Y + j] || map[X + i][Y + j] == THUAI6::PlaceType::Window
                        || map[X + i][Y + j] == THUAI6::PlaceType::ClassRoom || map[X + i][Y + j] == THUAI6::PlaceType::Gate || map[X + i][Y + j] == THUAI6::PlaceType::HiddenGate))
                    {
                        int nowDis = trickerDis[X + i][Y + j] - dis[X + i][Y + j];
                        if (nowDis > maxDis)
                        {
                            nowDis = maxDis;
                            ans = node(X + i, Y + j);
                        }
                    }

                }
    return ans;
}

std::queue<std::pair<node, int>> inTrickerViewNode;

void UpdateInTrickerViewNode(IStudentAPI& api, std::vector<std::shared_ptr<const THUAI6::Student>> student, int pre) //实际改为student
{
    auto self = api.GetSelfInfo();
    node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
    /*
    if (!tri.empty())
    {
        auto self = api.GetSelfInfo();
        node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
        node tr = node(api.GridToCell(tri[0]->x), api.GridToCell(tri[0]->y));
        int dx = std::min(abs(st.x - tr.x), 8), dy = std::min(abs(st.y - tr.y), 8);
        for (int i = tr.x - dx + 1; i <= tr.x + dx - 1; i++)
            for (int j = tr.y - dy + 1; j <= tr.y + dy - 1; j++)
                if (canMove[i][j] == 1 && canMovePre[i][j] == 0 && node(i, j) != st && canReach(api, tri[0]->x, tri[0]->y,
                    api.CellToGrid(i), api.CellToGrid(j), 5000, 0))
                {
                    inTrickerViewNode.push(std::make_pair(node(i, j), pre));
                    canMovePre[i][j] = pre;
                    canMove[i][j] = 0;
                }
                else if (canMove[i][j] == 0 && canMovePre[i][j] < pre)
                {
                    inTrickerViewNode.push(std::make_pair(node(i, j), pre));
                    canMovePre[i][j] = pre;
                }
        if (dx == 0)
        {
            for (int j = tr.y - dy + 1; j <= tr.y + dy - 1; j++)
                if (canMove[tr.x][j] == 1 && canMovePre[tr.x][j] == 0)
                {
                    inTrickerViewNode.push(std::make_pair(node(tr.x, j), pre));
                    canMovePre[tr.x][j] = pre;
                    canMove[tr.x][j] = 0;
                }
                else if (canMove[tr.x][j] == 0 && canMovePre[tr.x][j] < pre)
                {
                    inTrickerViewNode.push(std::make_pair(node(tr.x, j), pre));
                    canMovePre[tr.x][j] = pre;
                }
        }
        if (dy == 0)
        {
            for (int i = tr.x - dx + 1; i <= tr.x + dx - 1; i++)
                if (canMove[i][tr.y] == 1 && canMovePre[i][tr.y] == 0)
                {
                    inTrickerViewNode.push(std::make_pair(node(i, tr.y), pre));
                    canMovePre[i][tr.y] = pre;
                    canMove[i][tr.y] = 0;
                }
                else if (canMove[i][tr.y] == 0 && canMovePre[i][tr.y] < pre)
                {
                    inTrickerViewNode.push(std::make_pair(node(i, tr.y), pre));
                    canMovePre[i][tr.y] = pre;
                }
        }
    }*/
    int i = st.x, j = st.y;
    if (canMove[i][j] == 1 && canMovePre[i][j] == 0)
    {
        inTrickerViewNode.push(std::make_pair(node(i, j), pre));
        canMovePre[i][j] = pre;
        canMove[i][j] = 0;
    }
    else if (canMove[i][j] == 0 && canMovePre[i][j] < pre)
    {
        inTrickerViewNode.push(std::make_pair(node(i, j), pre));
        canMovePre[i][j] = pre;
    }
    while (!inTrickerViewNode.empty() && pre - inTrickerViewNode.front().second > 200)
    {
        auto now = inTrickerViewNode.front();
        inTrickerViewNode.pop();
        if (canMovePre[now.first.x][now.first.y] == now.second)
        {
            canMove[now.first.x][now.first.y] = 1;
            canMovePre[now.first.x][now.first.y] = 0;
        }
    }
}

void AI::play(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
    node goal = emptynode;
    auto disToSelf = [self](int x, int y) {return sqrt(sqr(x - self->x) + sqr(y - self->y)); };
    auto angleToSelf = [self](int x, int y) {return atan2(y - self->y, x - self->x); };
    static bool escape = false;

#   pragma region 预处理
    static int prepare = 0;
    prepare++;
    if (prepare == 1) Init(api, self);

    //std::cout << canReach(api, api.CellToGrid(2), api.CellToGrid(5), api.CellToGrid(2), api.CellToGrid(4), 10000, 600) << std::endl;
#pragma endregion

// 公共操作
    if (this->playerID == 0)
    {
        //玩家0执行操作

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
    static node lstTrickersVisibleNode = emptynode;
    static int lstTrickersVisibleNodeUse = 0;

#pragma region 通信
    static int classroomState[10] = { 0 };
    int nearhomework[10] = { 0 };
    node trickerNode = emptynode;
    static int hiddengateState[3] = { 0 };
    int finishwN = 0;
    auto trickersVisible = api.GetTrickers();
    for (int i = 0; i < classroomNode.size(); i++)
    {
        if (api.HaveView(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y)))
        {
            classroomState[i] = api.GetClassroomProgress(classroomNode[i].x, classroomNode[i].y);
            for (int j = 0; j < 4; j++)
            {
                if (j != this->playerID)
                {
                    std::string type = std::to_string(0);
                    std::string number = std::to_string(i);
                    std::string state = std::to_string(classroomState[i]);
                    api.SendBinaryMessage(j, type + " " + number + " " + state);
                }
            }
        }
    }
    for (int i = 0; i < hiddengatenode.size(); i++)
    {
        if (api.HaveView(api.CellToGrid(hiddengatenode[i].x), api.CellToGrid(hiddengatenode[i].y)))
        {
            if (api.GetHiddenGateState(classroomNode[i].x, classroomNode[i].y) == THUAI6::HiddenGateState::Null)
                hiddengateState[i] = 0;
            else if (api.GetHiddenGateState(classroomNode[i].x, classroomNode[i].y) == THUAI6::HiddenGateState::Refreshed)
                hiddengateState[i] = 1;
            else if (api.GetHiddenGateState(classroomNode[i].x, classroomNode[i].y) == THUAI6::HiddenGateState::Refreshed)
                hiddengateState[i] = 2;
            for (int j = 0; j < 4; j++)
            {
                if (j != this->playerID)
                {
                    std::string type = std::to_string(2);
                    std::string number = std::to_string(i);
                    std::string state = std::to_string(hiddengateState[i]);
                    api.SendBinaryMessage(j, type + " " + number + " " + state);
                }
            }
        }
    }
    if (!trickersVisible.empty())
    {
        trickerNode = node(api.GridToCell(trickersVisible[0]->x), api.GridToCell(trickersVisible[0]->y));
        for (int j = 0; j < 4; j++)
        {
            if (j != this->playerID)
            {
                std::string type = std::to_string(1);
                std::string x = std::to_string(trickerNode.x);
                std::string y = std::to_string(trickerNode.y);
                api.SendBinaryMessage(j, type + " " + x + " " + y);
            }
        }
    }
    while (api.HaveMessage())
    {
        auto message = api.GetMessage();
        std::stringstream ss(message.second);
        int type, message1, message2;
        ss >> type >> message1 >> message2;
        if (type == 0)
        {
            classroomState[message1] = message2;
            nearhomework[message1]++;
        }
        if (type == 1)
        {
            trickerNode = node(message1, message2);
        }
        if (type == 2)
        {
            hiddengateState[message1] = message2;
        }
    }
    for (int i = 0; i < classroomNode.size(); i++)
    {
        if (classroomState[i] == 10000000)
            finishwN++;
    }
    //std::cout << " asdsawda" << std::endl;
#pragma endregion

#pragma region 技能
    //有些技能写在躲避、唤醒里面
    if (self->studentType == THUAI6::StudentType::Teacher)
    {
        auto tri = api.GetTrickers();
        if (!tri.empty() && getDis(tri[0]->x - self->x, tri[0]->y - self->y) < Constants::Teacher::viewRange / 3
            && (tri[0]->playerState == THUAI6::PlayerState::Attacking || tri[0]->playerState == THUAI6::PlayerState::Climbing ||
                tri[0]->playerState == THUAI6::PlayerState::Swinging || tri[0]->playerState == THUAI6::PlayerState::Locking ||
                tri[0]->playerState == THUAI6::PlayerState::UsingSpecialSkill) && self->timeUntilSkillAvailable[0] < 1e-6)
            api.UseSkill(0);
    }
    static int athleteSkill0Pre = -1000;
    if (self->studentType == THUAI6::StudentType::Athlete)
    {
        auto tri = api.GetTrickers();
        if (prepare - athleteSkill0Pre >= 40)
        {
            if (prepare - athleteSkill0Pre >= 50) athleteSkill0Pre = -1000;
            else if (!tri.empty())
            {
                api.EndAllAction();
                api.Move(300, PI + angleToSelf(tri[0]->x, tri[0]->y));
                return;
            }
            while (!inTrickerViewNode.empty())
            {
                auto now = inTrickerViewNode.front();
                inTrickerViewNode.pop();
                if (canMovePre[now.first.x][now.first.y] == now.second)
                {
                    canMove[now.first.x][now.first.y] = 1;
                    canMovePre[now.first.x][now.first.y] = 0;
                }
            }
        }
        else if (prepare - athleteSkill0Pre < 40 && !tri.empty() //&& tri[0]->playerState != THUAI6::PlayerState::Attacking
            && canReach(api, self->x, self->y, tri[0]->x, tri[0]->y, 2500, 400))
        {
            api.EndAllAction();
            api.Move(500, angleToSelf(tri[0]->x, tri[0]->y));
            return;
        }
        else if (prepare - athleteSkill0Pre < 40 && !tri.empty()) //&& tri[0]->playerState == THUAI6::PlayerState::Attacking)
        {
            api.EndAllAction();
        }
        if (!tri.empty() && getDis(tri[0]->x - self->x, tri[0]->y - self->y) < 3000 && self->timeUntilSkillAvailable[0] < 1e-6
            //&& tri[0]->playerState == THUAI6::PlayerState::Attacking 
            && canReach(api, self->x, self->y, tri[0]->x, tri[0]->y, 2500, 400))
        {
            api.EndAllAction();
            api.UseSkill(0);
            athleteSkill0Pre = prepare;
            api.Move(200, angleToSelf(tri[0]->x, tri[0]->y));
            return;
        }
    }
    if (self->studentType == THUAI6::StudentType::Sunshine)
    {
        if (self->determination <= Constants::Sunshine::maxHp - 750000 && self->timeUntilSkillAvailable[1] < 1e-6)
            api.UseSkill(1);
    }
#pragma endregion

#pragma region 躲避
    // 唤醒队友暂时忽略躲避
    std::shared_ptr<const THUAI6::Student> addictedStu = nullptr;
    std::shared_ptr<const THUAI6::Student> goToRouse = nullptr;
    if (self->playerState == THUAI6::PlayerState::Rousing || self->playerState == THUAI6::PlayerState::Roused) return;
    for (auto u : api.GetStudents())
    {
        if (u->playerState == THUAI6::PlayerState::Addicted)
        {
            addictedStu = u;
            break;

        }
    }
    double minDis = 1e15;
    if (addictedStu != nullptr && self->playerState != THUAI6::PlayerState::Addicted)
    {
        //std::cout << addictedStu->playerID << std::endl;
        bool tea = false;
        if (trickerNode != emptynode && canReach(api, api.CellToGrid(trickerNode.x), api.CellToGrid(trickerNode.y), addictedStu->x,
            addictedStu->y, 3000, 0))
        {
            for (auto u : api.GetStudents())
                if (u->playerState != THUAI6::PlayerState::Addicted && u->studentType == THUAI6::StudentType::Teacher)
                    goToRouse = u, tea = true;
        }
        if (!tea)
        {
            for (auto u : api.GetStudents())
                if (u->playerState != THUAI6::PlayerState::Addicted)
                {
                    if (getDis((u->x - addictedStu->x), u->y - addictedStu->y) < minDis)
                    {
                        minDis = getDis((u->x - addictedStu->x), u->y - addictedStu->y);
                        goToRouse = u;
                    }

                }
        }
    }

    if (self->studentType == THUAI6::StudentType::Sunshine && self->timeUntilSkillAvailable[1] < 1e-6 && self->timeUntilSkillAvailable[0] < 1e-6
        && addictedStu != nullptr && api.HaveView(addictedStu->x, addictedStu->y))
    {
        api.UseSkill(1);
        if (self->timeUntilSkillAvailable[0] < 1e-6)
        {
            api.UseSkill(0);
        }
    }

    if (self != goToRouse)
    {
        auto trickersVisible = api.GetTrickers();
        auto getStudent = api.GetStudents();
        //if (trickerNode != emptynode && !canReach(api, self->x, self->y, api.CellToGrid(trickerNode.x), api.CellToGrid(trickerNode.y), 10000, 0)) goto rouse;
        if (trickersVisible.empty() && trickerNode == emptynode)
        {
            if (lstTrickersVisibleNode != emptynode)
            {
                BFSTrickerPath(lstTrickersVisibleNode);
                if (trickerDis[st.x][st.y] < 15 - (self->studentType == THUAI6::StudentType::Teacher) * 7) goal = findBestPlace(api, lstTrickersVisibleNode);
                if (prepare - lstTrickersVisibleNodeUse > 200)
                {
                    lstTrickersVisibleNodeUse = 0;
                    lstTrickersVisibleNode = emptynode;
                }
            }
            else
            {
                //goal = findBestPlace(api, emptynode);
                goal = emptynode;
            }
            if (self->studentType != THUAI6::StudentType::Teacher) UpdateInTrickerViewNode(api, getStudent, prepare);
            if (goal != emptynode)
            {
                api.EndAllAction();
                escape = true;
                //std::cout << "EMove goal: " << goal.x << " " << goal.y << std::endl;
                Move(api, self, goal);
                return;
            }
        }
        else
        {
            if (!trickersVisible.empty())
            {
                lstTrickersVisibleNode = node(api.GridToCell(trickersVisible[0]->x), api.GridToCell(trickersVisible[0]->y));
                lstTrickersVisibleNodeUse = prepare;
            }
            else if (trickerNode != emptynode)
            {
                lstTrickersVisibleNode = trickerNode;
                lstTrickersVisibleNodeUse = prepare;
            }
            while (!inTrickerViewNode.empty())
            {
                auto now = inTrickerViewNode.front();
                inTrickerViewNode.pop();
                if (canMovePre[now.first.x][now.first.y] == now.second)
                {
                    canMove[now.first.x][now.first.y] = 1;
                    canMovePre[now.first.x][now.first.y] = 0;
                }
            }
            if (self->studentType != THUAI6::StudentType::Teacher) UpdateInTrickerViewNode(api, getStudent, prepare);
            BFSTrickerPath(lstTrickersVisibleNode);
            if (trickerDis[st.x][st.y] < 15 - (self->studentType == THUAI6::StudentType::Teacher) * 4) goal = findBestPlace(api, lstTrickersVisibleNode);
            if (goal != emptynode)
            {
                api.EndAllAction();
                auto getTricker = api.GetTrickers();
                if (self->studentType == THUAI6::StudentType::Sunshine && self->timeUntilSkillAvailable[2] < 1e-6 && !getTricker.empty()
                    && disToSelf(getTricker[0]->x, getTricker[0]->y) < 3200)
                    api.UseSkill(2);
                //std::cout << "TMove goal: " << goal.x << " " << goal.y << std::endl;
                escape = true;
                Move(api, self, goal);
                return;
            }
        }
    }
#pragma endregion

#pragma region 救人
    rouse :
    if (goToRouse == self)
    {
        minDis = disToSelf(addictedStu->x, addictedStu->y);
        auto trickersVisible = api.GetTrickers();
        if (self->playerState != THUAI6::PlayerState::Rousing && IsNearPlace(st, node(api.GridToCell(addictedStu->x), api.GridToCell(addictedStu->y))))
        {
            api.EndAllAction();
            //if (self->timeUntilSkillAvailable[1] < 1e-6)
                //api.UseSkill(1);
            api.StartRouseMate(addictedStu->playerID);
            return;
        }
        else if (minDis < 1500 && self->playerState != THUAI6::PlayerState::Rousing && !IsNearPlace(st, node(api.GridToCell(addictedStu->x), api.GridToCell(addictedStu->y))))
        {
            api.EndAllAction();
            api.Move(80, angleToSelf(addictedStu->x, addictedStu->y));
            return;
        }
        else {
            api.EndAllAction();
            Move(api, self, node(api.GridToCell(addictedStu->x), api.GridToCell(addictedStu->y)));
            return;
        }
    }
#pragma endregion

    //std::cout << api.GetClassroomProgress(findNearPlace(st, THUAI6::PlaceType::ClassRoom).x, findNearPlace(st, THUAI6::PlaceType::ClassRoom).y) << std::endl;
#pragma region 修机或毕业没有捣蛋鬼就继续修机
    int rem = 4;
    for (auto& u : api.GetStudents())
        if (u->playerState == THUAI6::PlayerState::Quit) rem--;
    if (self->playerState == THUAI6::PlayerState::Rousing) return;
    if (finishwN < 7 && rem > 1)
    {
        if (noTrickerNear(api) && self->playerState == THUAI6::PlayerState::Learning
            && api.GetClassroomProgress(findNearPlace(st, THUAI6::PlaceType::ClassRoom).x, findNearPlace(st, THUAI6::PlaceType::ClassRoom).y) < 10000000)
            return;
        if (self->playerState == THUAI6::PlayerState::Learning
            && api.GetClassroomProgress(findNearPlace(st, THUAI6::PlaceType::ClassRoom).x, findNearPlace(st, THUAI6::PlaceType::ClassRoom).y) == 10000000)
            api.EndAllAction();
        if (noTrickerNear(api))
            if (self->playerState == THUAI6::PlayerState::OpeningAGate
                || self->playerState == THUAI6::PlayerState::Encouraging)
                return;
    }

#pragma endregion

#pragma region 移动防止学生间碰撞    
    auto studentVisible = api.GetStudents();
    //if (noTrickerNear(api))
    {
        for (auto& stu : studentVisible)
        {
            if (stu == self) continue;
            if (disToSelf(stu->x, stu->y) <= 805 && self->playerState != THUAI6::PlayerState::Rousing
                && self->playerState != THUAI6::PlayerState::Encouraging && self->playerState != THUAI6::PlayerState::Encouraged)
            {
                static int f = 1;
                api.EndAllAction();
                if (!canReachAngle(api, self->x, self->y, PI + f * PI / 3 + angleToSelf(stu->x, stu->y), 200, 400)) f = -f;
                auto m2 = api.Move(45 - double(self->playerID), PI + f * (PI / (3.0 - double(self->playerID - 1.5) * 0.6)) + angleToSelf(stu->x, stu->y));
                std::this_thread::sleep_for(std::chrono::milliseconds(46));
                return;
            }
        }
    }
#pragma endregion


    //std::cout << goal.x << " " << goal.y << std::endl;
    //std::cout << lstTrickersVisibleNode.x << " " << lstTrickersVisibleNode.y << std::endl;


#pragma region 修机与毕业
    double minclassroomDis = 1e15;
    int minclassroomN = 0;
    double maxclassroompriority = 0;
    double classroompriority = 0;
    int firstclassroomN = 0;
    int remain = 0;
    int firstgateN = 0;
    double gatepriority = 0;
    double maxgatepriority = 0;
    static int hiddengatenum = 0;
    BFSTarget(st);
    auto u = api.GetStudents();
    for (int i = 0; i < 4; i++)
    {
        if ((u[i]->playerState != THUAI6::PlayerState::Quit))
            remain++;
    }
    if ((remain > 1) || (finishwN <= 3))
    {
        if (lstTrickersVisibleNode != emptynode)
        {
            BFSTrickerTarget(lstTrickersVisibleNode);
        }
        for (int i = 0; i < classroomNode.size(); i++)
        {
            double classroomdistancefromTricker = 0;
            if ((classroomNode[8].x == 40) && (classroomNode[8].y == 12) && (nearhomework[8] > 0))
                dis[classroomNode[8].x][classroomNode[8].y] = minclassroomDis;
            classroomdistancefromTricker = sqrt(sqr(classroomNode[i].x - trickerNode.x) + sqr(classroomNode[i].y - trickerNode.y));
            classroompriority = (api.GetClassroomProgress(classroomNode[i].x, classroomNode[i].y) + 0.1 * classroomdistancefromTricker + 1000.0) / (dis[classroomNode[i].x][classroomNode[i].y] + 5);
            if (lstTrickersVisibleNode != emptynode)
            {
                if (trickerDis[classroomNode[i].x][classroomNode[i].y] < 15) classroompriority = 0;
            }
            if (classroompriority >= maxclassroompriority && classroomState[i] < 10000000)
            {
                maxclassroompriority = classroompriority;
                firstclassroomN = i;
            }
        }
        BFSPath(st);
        //std::cout << firstclassroomN << " " << classroomNode[firstclassroomN].x << " " << classroomNode[firstclassroomN].y << " " << api.GetClassroomProgress(classroomNode[firstclassroomN].x, classroomNode[firstclassroomN].y) << std::endl;
        if (finishwN < 7 || (finishwN >= 7 && self->studentType == THUAI6::StudentType::Teacher) || (rem == 1 && self->studentType == THUAI6::StudentType::Teacher))
        {
            if (findNearPlace(st, THUAI6::PlaceType::ClassRoom) == emptynode || findNearPlace(st, THUAI6::PlaceType::ClassRoom) != classroomNode[firstclassroomN])
            {
                //std::cout << "goal: " << goal.x << " " << goal.y << std::endl;
                goal = findNearCanMovePlaceWithMinDis(classroomNode[firstclassroomN]);
                //std::cout << "goal: " << goal.x << " " << goal.y << std::endl;
            }

            else if (api.GetClassroomProgress(findNearPlace(st, THUAI6::PlaceType::ClassRoom).x, findNearPlace(st, THUAI6::PlaceType::ClassRoom).y) < 10000000)
            {
                api.EndAllAction();
                api.StartLearning();
                return;
            }
        }
        else
        {
            for (int i = 0; i < gatenode.size(); i++)
            {
                double gatedistancefromTricker = 0;
                gatedistancefromTricker = sqrt(sqr(gatenode[i].x - trickerNode.x) + sqr(gatenode[i].y - trickerNode.y));
                gatepriority = (api.GetGateProgress(gatenode[i].x, gatenode[i].y) + 0.1 * gatedistancefromTricker + 1000.0) / (dis[gatenode[i].x][gatenode[i].y] + 5);
                if (lstTrickersVisibleNode != emptynode)
                {
                    if (canReach(api, api.CellToGrid(lstTrickersVisibleNode.x), api.CellToGrid(lstTrickersVisibleNode.y),
                        api.CellToGrid(gatenode[i].x), api.CellToGrid(gatenode[i].y), 10000, 0)) gatepriority = 0;
                }
                if (gatepriority >= maxgatepriority)
                {
                    maxgatepriority = gatepriority;
                    firstgateN = i;
                }
                if (api.GetGateProgress(gatenode[i].x, gatenode[i].y) == 18000)
                    firstgateN = i;
            }
            if (findNearPlace(st, THUAI6::PlaceType::Gate) == emptynode)
                goal = findNearCanMovePlaceWithMinDis(gatenode[firstgateN]);
            else if (api.GetGateProgress(gatenode[firstgateN].x, gatenode[firstgateN].y) < 18000)
                api.StartOpenGate();
            else
            {
                api.EndAllAction();
                api.Graduate();
                return;
            }
        }
    }
    //隐藏校门毕业
    else
    {
        node hiddengateplace;
        static int findgate = 0;
        if (findgate == 0)
        {
            if (api.HaveView(api.CellToGrid(hiddengatenode[hiddengatenum].x), api.CellToGrid(hiddengatenode[hiddengatenum].y)) && hiddengateState[hiddengatenum] == 0)
            {
                hiddengatenum++;
            }
            hiddengateplace = node(hiddengatenode[hiddengatenum].x, hiddengatenode[hiddengatenum].y);
            for (int i = 0; i < hiddengatenode.size(); i++)
            {
                if ((hiddengateState[i] == 2) || (hiddengateState[i] == 1))
                {
                    hiddengatenum = i;
                    hiddengateplace = node(hiddengatenode[i].x, hiddengatenode[i].y);
                    findgate = 1;
                }
            }
        }
        else
        {
            hiddengateplace = node(hiddengatenode[hiddengatenum].x, hiddengatenode[hiddengatenum].y);
        }
        if (findNearPlace(st, THUAI6::PlaceType::HiddenGate) == emptynode)
            goal = findNearCanMovePlaceWithMinDis(hiddengateplace);
        else
        {
            api.EndAllAction();
            api.Graduate();
            return;
        }
    }
#pragma endregion
    api.EndAllAction();
    //std::cout << "final goal: " << goal.x << " " << goal.y << std::endl;
    Move(api, self, goal);
}

int STUdata(THUAI6::Student stu, int type)
{
    //type类型：0——最大沉迷度，1——追击价值，2——
    int addictstage;
    double cost;//学生追击的成本
    double value;//学生的价值

    if (stu.studentType == THUAI6::StudentType::Sunshine) {
        switch (type) {
        case 0:return Constants::Sunshine::maxAddiction;
        case 1:return 3;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            addictstage = (stu.addiction / STUdata(stu, 0)) * 30;
            return (addictstage + 10) * value / cost;
        }
        }
    }
    else if (stu.studentType == THUAI6::StudentType::Teacher) {
        switch (type) {
        case 0:return Constants::Teacher::maxAddiction;
        case 1:return 1;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            addictstage = (stu.addiction / STUdata(stu, 0)) * 30;
            return (addictstage + 10) * value / cost;
        }
        }
    }
    else if (stu.studentType == THUAI6::StudentType::Athlete) {
        switch (type) {
        case 0:return Constants::Athlete::maxAddiction;
        case 1:return 2;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            addictstage = (stu.addiction / STUdata(stu, 0)) * 30;
            return (addictstage + 10) * value / cost;
        }
        }
    }
    else if (stu.studentType == THUAI6::StudentType::StraightAStudent) {
        switch (type) {
        case 0:return Constants::StraightAStudent::maxAddiction;
        case 1:return 4;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            addictstage = (stu.addiction / STUdata(stu, 0)) * 30;
            return (addictstage + 10) * value / cost;
        }
        }
    }
    else if (stu.studentType == THUAI6::StudentType::Robot) {
        switch (type) {
        case 0:return Constants::Robot::maxAddiction;
        case 1:return 6;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            return 40 * value / cost;
        }
        }
    }
    else if (stu.studentType == THUAI6::StudentType::TechOtaku) {
        switch (type) {
        case 0:return Constants::TechOtaku::maxAddiction;
        case 1:return 5;
        case 2: {
            cost = stu.speed * (stu.determination + 1) / 1e9;
            value = stu.learningSpeed + 0.8 * stu.encourageSpeed;
            addictstage = (stu.addiction / STUdata(stu, 0)) * 30;
            return (addictstage + 10) * value / cost;
        }
        }
    }
    return 1;
}//返回学生的各项基本参数
node Getcoord(node A, node B, double b, double a, bool type)
{
    if (A == B)return emptynode;
    double c = getDis(A.x - B.x, A.y - B.y);
    double cosA = (sqr(b) + sqr(c) - sqr(a)) / (2 * b * c);
    if (cosA < -1 || cosA>1) return emptynode;
    double Anglex;
    if (A.x == B.x) Anglex = A.y < B.y ? PI / 2 : -PI / 2;
    else Anglex = atan((A.y - B.y) / (A.x - B.x));
    double Detangle = acos(cosA);
    double Angle;
    if (type) Angle = Anglex + Detangle;
    else Angle = Anglex - Detangle;
    node C(A.x + cos(Angle) * b, A.y + sin(Angle) * b);
    return C;
}
std::queue<node> Croodself;
std::queue<node> Croodstu;
std::queue<node> Dirnode;
std::queue<double> Nearstu;
std::queue<double> Classvolume;
std::queue<THUAI6::Student> Accidentstu;
std::queue<THUAI6::PlayerState> selfState;
int Haveview[50][50] = { 0 };
void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto studentVisible = api.GetStudents();
    auto Bullet = api.GetBullets();
    auto disToSelf = [self](int x, int y) {return sqrt(sqr(x - self->x) + sqr(y - self->y)); };
    auto angleToSelf = [self](int x, int y) {return atan2(y - self->y, x - self->x); };
    std::vector<THUAI6::Student> Viewstu;;
    static THUAI6::Student Goalstu;

    static int Timeofgoalstu = 0;
    int Classcopmlete = 0;
    static node st = node(api.GridToCell(self->x), api.GridToCell(self->y));
    node goal = emptynode;
    double neareststu = 25000;
    if (self->trickDesire != 0) neareststu = Constants::Klee::alertnessRadius / self->trickDesire;
    double prioritystu[20] = { 0.0 };//学生优先级
    double maxstupri = 0;//学生最大优先级
    double priority[10] = { 0.0 }; //10个课程的优先级
    double disofclass[10] = { 0.0 }; //到10个教室的距离
    static double timeofclass[10] = { 0.0 }; //记录去过教室以来的时间
    static double timeofGate[3] = { 0.0 }; //记录去过大门以来的时间
    static double classporgress[10] = { 0.0 }; //记录课程完成情况
    double priorityGate[2] = { 0 };//大门优先级
    double maxpriclass = 0;
    double maxpriGate = 0;
    int goalclass = 0;
    int goalgate = 0;
#pragma region 预处理
    static int prepare = 0;
    prepare++;
    
    if (prepare == 1) Init(api, self);
    if (Nearstu.size() < 5)
    {
        Classvolume.push(self->classVolume);
        Nearstu.push(neareststu);
        Croodself.push(node(self->x, self->y));
        selfState.push(self->playerState);
        Dirnode.push(node(0, 0));
    }
    else if (prepare % 10 == 1)
    {
        selfState.push(self->playerState);
        selfState.pop();
        Croodself.push(node(self->x, self->y));
        Croodself.pop();
        Nearstu.push(neareststu);
        Nearstu.pop();
        Classvolume.push(self->classVolume);
        Classvolume.pop();
        double Gridx = 1000*(self->x - Croodself.front().x) * (Nearstu.front() - neareststu) / sqr(disToSelf(Croodself.front().x, Croodself.front().y));
        double Gridy = 1000*(self->y - Croodself.front().y) * (Nearstu.front() - neareststu) / sqr(disToSelf(Croodself.front().x, Croodself.front().y));
        node Direction(Gridx, Gridy);
        Dirnode.push(Direction);
        Dirnode.pop();
    }
    for (int i = 0; i < Accidentstu.size(); i++)
    {
        THUAI6::Student Accstu = Accidentstu.front();
        if (!api.HaveView(Accstu.x, Accstu.y)) {
            Accstu.addiction += 50;
            if (Accstu.addiction < STUdata(Accstu, 0))
                Accidentstu.push(Accstu);
        }
        Accidentstu.pop();
    }
    for (int i = 0; i < 50; i++)
        for (int j = 0; j < 50; j++)
        {
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Window
                || api.GetPlaceType(i, j) == THUAI6::PlaceType::Door3
                || api.GetPlaceType(i, j) == THUAI6::PlaceType::Door5
                || api.GetPlaceType(i, j) == THUAI6::PlaceType::Door6
                || api.GetPlaceType(i, j) == THUAI6::PlaceType::Land
                || api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass)
            {
                canMove[i][j] = 1;
            }
            if (api.HaveView(api.CellToGrid(i), api.CellToGrid(j)))
                Haveview[i][j] = 0;
            else Haveview[i][j]++;
        }
    node A = Croodself.front();
    node B(self->x, self->y);
    double a = Nearstu.front();
    double b = neareststu;
    node Possiblenode0 = Getcoord(A, B, a, b, 0);
    if (Possiblenode0.x < 500 || Possiblenode0.y < 500 || Possiblenode0.x>49500 || Possiblenode0.y>49500)
        Possiblenode0 = emptynode;
    node Possiblenode1 = Getcoord(A, B, a, b, 1);
    if (Possiblenode1.x < 500 || Possiblenode1.y < 500 || Possiblenode1.x>49500 || Possiblenode1.y>49500)
        Possiblenode1 = emptynode;
    double Accury0 = 0;
    double Accury1 = 0;
    for (int i = 0; i < Dirnode.size(); i++)
    {
        Accury0 = 1000 * (Dirnode.front().x * (Possiblenode0.x - self->x) + Dirnode.front().y * (Possiblenode0.y - self->y)) / neareststu;
        Accury1 = 1000 * (Dirnode.front().x * (Possiblenode1.x - self->x) + Dirnode.front().y * (Possiblenode1.y - self->y)) / neareststu;
        if (Possiblenode0 == emptynode) Accury0 = 0;
        if (Possiblenode1 == emptynode) Accury1 = 0;
        Dirnode.push(Dirnode.front());
        Dirnode.pop();
    }
    double DirectionAccury = abs((a - b) / disToSelf(A.x, A.y));
    for (auto& stu : studentVisible)
    {
        if (stu->playerState == THUAI6::PlayerState::Addicted)
            Accidentstu.push(*stu);
        else {
            Viewstu.push_back(*stu);
            prioritystu[stu->playerID] = STUdata(*stu, 1) / disToSelf(stu->x, stu->y);
            double prioritystuGoal = 0;
            /*if (Goalstu.playerID != -1)prioritystuGoal = STUdata(Goalstu, 1) / disToSelf(Goalstu.x, Goalstu.y);*/
            if (prioritystu[stu->playerID] > maxstupri && prioritystu[stu->playerID]> prioritystuGoal) {
                maxstupri = prioritystu[stu->playerID];
                Goalstu = *stu;
                Timeofgoalstu = prepare;
            }
        }
    }
#pragma endregion
#pragma region 优先级
    for (int i = 0; i < classroomNode.size(); i++)
    {
        timeofclass[i] = timeofclass[i] + 1;
        if (api.HaveView(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y)))
        {
            if (classporgress[i] > 1500000 && classporgress[i] < 10000000 && self->timeUntilSkillAvailable[0] < 2000)
            {
                int a = classporgress[i] < 15e5 ? 0 : classporgress[i];
                timeofclass[i] = timeofclass[i] + a / 1e7;
            }
            else timeofclass[i] = 0;
        }
        disofclass[i] = disToSelf(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y)); //记录坐标
        //教室可视，如果机子进度增加，则权重大大增加，否则归零
        classporgress[i] = api.GetClassroomProgress(classroomNode[i].x, classroomNode[i].y);
        priority[i] = (1000000 + classporgress[i]) * timeofclass[i] / (4000 + disofclass[i]); //计算优先级
        if (classporgress[i] >= 10000000) {
            priority[i] = 0;
            Classcopmlete++;
        }//如果修完机子,将其优先级清零
        if (priority[i] > maxpriclass) maxpriclass = priority[i], goalclass = i;
    }//机子优先级
    for (int i = 0; i < gatenode.size(); i++)
    {
        timeofGate[i] += 1;
        if (api.HaveView(gatenode[i].x, gatenode[i].y)) timeofGate[i] = 0;
        priorityGate[i] = timeofGate[i] * api.GetGateProgress(gatenode[i].x, gatenode[i].y) / disToSelf(api.CellToGrid(gatenode[i].x), api.CellToGrid(gatenode[i].y));
        if (maxpriGate < priorityGate[i]) {
            goalgate = i;
            maxpriGate = priorityGate[i];
        }
    }

    if (prepare - Timeofgoalstu > 200
        || disToSelf(Goalstu.x, Goalstu.y)<1200
        || Goalstu.playerState == THUAI6::PlayerState::Addicted || prepare==1)
        Goalstu.playerID = -1;
    
#pragma endregion
#pragma region 防沉迷
    for (int i = 0; i < Accidentstu.size(); i++)
    {
        THUAI6::Student Accstu = Accidentstu.front();
        node Accidentnode(api.GridToCell(Accstu.x), api.GridToCell(Accstu.y));
        int Gridx = (Accstu.x % 1000 - 500) > 0 ? 1 : -1;
        int Gridy = (Accstu.y % 1000 - 500) > 0 ? 1 : -1;
        node a[4] = { Accidentnode,
            node(Accidentnode.x ,Accidentnode.y + Gridy),
            node(Accidentnode.x + Gridx,Accidentnode.y),
            node(Accidentnode.x + Gridx,Accidentnode.y + Gridy) };
        for (int i = 0; i < 4; i++)
            if (st != a[i]) canMove[a[i].x][a[i].y] = 0;
        if (st != Accidentnode)
            canMove[Accidentnode.x][Accidentnode.y] = 0;
        if (disToSelf(Accstu.x, Accstu.y) < neareststu + 100)
        {
            neareststu = 25000;
            DirectionAccury = 0;
        }
        Accidentstu.push(Accstu);
        Accidentstu.pop();
    }
#pragma endregion
#pragma region 攻击
    for (auto& stu : studentVisible)
    {
        if (disToSelf(stu->x, stu->y) <= 3000 && (stu->playerState == THUAI6::PlayerState::Climbing || stu->playerState == THUAI6::PlayerState::Learning) && self->timeUntilSkillAvailable[0] < 1e-6)
        {
            if (self->timeUntilSkillAvailable[1] < 1e-6)
                api.UseSkill(1, stu->playerID);
            api.UseSkill(0);
            api.Attack(angleToSelf(stu->x, stu->y));
            break;

        }//学生翻窗、学习时攻击
        if (disToSelf(stu->x, stu->y) <= 2600 && stu->playerState != THUAI6::PlayerState::Addicted
            && canReach(api, self->x, self->y, stu->x, stu->y, disToSelf(stu->x, stu->y), 100))
        {
            if (self->timeUntilSkillAvailable[0] < 1e-6) {
                if (self->timeUntilSkillAvailable[1] < 1e-6)
                    api.UseSkill(1, stu->playerID);
                api.UseSkill(0);
            }
            api.Attack(angleToSelf(stu->x, stu->y)); // 攻击
            break;
        }
        if (disToSelf(stu->x, stu->y) <= 2600 && canReachAngle(api, self->x, self->y, angleToSelf(stu->x, stu->y), 2600, 100))
        {
            if (stu->playerState != THUAI6::PlayerState::Addicted && canReachAngle(api, self->x, self->y, angleToSelf(stu->x, stu->y), 2600, 100))
                api.Attack(angleToSelf(stu->x, stu->y)); // 攻击
            break;
        }//普通攻击
    }
    double Angle;
    if (neareststu < 1000 && Viewstu.empty())
    {
        if (self->timeUntilSkillAvailable[0] < 1e-6 || 1)
        {
            double Angle = angleToSelf(A.x, A.y);
            double Distost = 10000;
            if (Possiblenode0 != emptynode && api.GetPlaceType(api.GridToCell(Possiblenode0.x), api.GridToCell(Possiblenode0.y)) == THUAI6::PlaceType::Grass 
                && disToSelf(Possiblenode0.x, Possiblenode0.y)< neareststu+1000)
                Angle = angleToSelf(Possiblenode0.x, Possiblenode0.y);
            else if (Possiblenode0 != emptynode && api.GetPlaceType(api.GridToCell(Possiblenode1.x), api.GridToCell(Possiblenode1.y)) == THUAI6::PlaceType::Grass
                && disToSelf(Possiblenode0.x, Possiblenode0.y) < neareststu + 1000)
                double Angle = angleToSelf(Possiblenode0.x, Possiblenode0.y);
            else {
                for (int i = 0; i < 8; i++)
                {
                    node a = node(api.GridToCell(self->x), api.GridToCell(self->y)) + dir[i];
                    double DIS = disToSelf(api.CellToGrid(a.x), api.CellToGrid(a.y));
                    if (api.GetPlaceType(a.x, a.y) == THUAI6::PlaceType::Grass)
                    if (api.GetPlaceType(a.x, a.y) == THUAI6::PlaceType::Grass && DIS < 1700 && DIS<Distost && abs(DIS- neareststu)<700)
                    {
                        Distost = DIS;
                        Angle = angleToSelf(api.CellToGrid(a.x), api.CellToGrid(a.y));
                    }
                }
            }
            if (selfState.front() != THUAI6::PlayerState::Swinging)
            {
                api.UseSkill(0);
                api.Attack(Angle);
            }

        }
    }
    for (int i = 0; i < classroomNode.size(); i++)
    {
        if (disToSelf(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y)) < 3000)
        {
            if (classporgress[i] > 1500000 && classporgress[i] < 10000000 && Viewstu.empty() && self->timeUntilSkillAvailable[0] < 1e-6)
            {
                node Class_ndoe(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y));
                double Attackangle = angleToSelf(Class_ndoe.x, Class_ndoe.y);

                api.UseSkill(0);
                api.Attack(Attackangle);
                break;
            }
        }
    }
#pragma endregion
#pragma region 防卡位
   /* if ((self->playerState == THUAI6::PlayerState::Idle || self->playerState == THUAI6::PlayerState::Climbing)
        && (selfState.front() == THUAI6::PlayerState::Idle || selfState.front() == THUAI6::PlayerState::Climbing)
        && disToSelf(A.x,A.y)<1200  && st != findNearCanMovePlaceWithMinDis(node(25, 25)) && prepare>100)
    {
        goal = findNearCanMovePlaceWithMinDis(node(25, 25));
    }*/
#pragma endregion
#pragma region 追人
    if (!Viewstu.empty())
    {
        if (Goalstu.playerState == THUAI6::PlayerState::Stunned)
            api.UseSkill(1, Goalstu.playerID);
        goal = node(api.GridToCell(Goalstu.x), api.GridToCell(Goalstu.y));
    }
#pragma endregion
#pragma region 丢失视野后寻找
    else if (Goalstu.playerID != -1)
    {
        for (int i = 0; i < 8; i++)
        {
            node a = node(api.GridToCell(Goalstu.x), api.GridToCell(Goalstu.y)) + dir[i];
            if (!api.HaveView(api.CellToGrid(a.x), api.CellToGrid(a.y)) && canMove[a.x][a.y] && Haveview[a.x][a.y] > 10)
            {
                goal = a;
            }
        }
        if (Classcopmlete < 7 && goal == emptynode)
            goal = findNearCanMovePlaceWithMinDis(classroomNode[goalclass]);
        else if (goal == emptynode)
            goal = findNearCanMovePlaceWithMinDis(gatenode[goalgate]);
    }
#pragma endregion
//#pragma region 控电机
//    else if (self->classVolume > 0.3)
//    {
//        api.Print(fmt::format("电机声音:{}", 1e2 * neareststu * self->classVolume / Constants::Klee::alertnessRadius));
//        for (int i = 0; i < 10; i++)
//        {
//            double Progress = 1e2 * disofclass[i] * self->classVolume / Constants::Klee::alertnessRadius;
//            double dislastclass = getDis(A.x - api.CellToGrid(classroomNode[i].x), A.y - api.CellToGrid(classroomNode[i].y));
//            double Det_progress = 200 * (disofclass[i] * self->classVolume - Classvolume.front() * dislastclass) / Constants::Klee::alertnessRadius;
//            double a;
//            if (abs(neareststu - disofclass[i]) < 2000)
//                goalclass = i;
//            else if (!api.HaveView(api.CellToGrid(classroomNode[i].x), api.CellToGrid(classroomNode[i].y)))
//            {
//                if (Progress - 1 > classporgress[i] / 1e5 && Progress < 99
//                    && Progress < classporgress[i] / 1e5 + 0.1 * timeofclass[i]
//                    && (Det_progress > 10 || Det_progress < 50)
//                    && disofclass[i] < Constants::Klee::alertnessRadius)
//                {
//                    goalclass = i;
//                    a = Det_progress;
//                }
//            }
//            else if (abs(Progress - classporgress[i] / 1e5) < 0.1) goalclass = i;
//            if (Classcopmlete < 7)
//                goal = findNearCanMovePlaceWithMinDis(classroomNode[goalclass]);
//            else
//                goal = findNearCanMovePlaceWithMinDis(gatenode[goalgate]);
//        }
//    }
//#pragma endregion
#pragma region 通过bgm寻找可能学生
    //else if (neareststu < 7000 || (DirectionAccury>0.8 && DirectionAccury<1.1))
    //{
    //    api.Print(fmt::format("bgm"));
    //    if ((Accury0 < 1000 || Accury1 < 1000) && (Possiblenode0 != emptynode || Possiblenode1 != emptynode))
    //    {
    //        if (Accury0 < Accury1 && Possiblenode0 != emptynode) goal = node(api.GridToCell(Possiblenode0.x), api.GridToCell(Possiblenode0.y));
    //        else  goal = findNearCanMovePlaceWithMinDis(node(api.GridToCell(Possiblenode1.x), api.GridToCell(Possiblenode1.y)));
    //    }
    //    else if (Classcopmlete < 7)
    //        goal = findNearCanMovePlaceWithMinDis(classroomNode[goalclass]);
    //    else
    //        goal = findNearCanMovePlaceWithMinDis(gatenode[goalgate]);
    //}
    else if (DirectionAccury > 0.95 && DirectionAccury < 1.1 && neareststu<17000 )
    {
        if (Possiblenode0 != emptynode || Possiblenode1 != emptynode)
        {
            if (Accury0 < Accury1 && Possiblenode0 != emptynode) {
                goal = findNearCanMovePlaceWithMinDis(node(api.GridToCell(Possiblenode0.x), api.GridToCell(Possiblenode0.y)));
                Goalstu.playerID = 4;
                Timeofgoalstu = prepare;
                Goalstu.x = api.CellToGrid(goal.x);
                Goalstu.y = api.CellToGrid(goal.y);
            }
            else if (Possiblenode0 != emptynode) {
                goal = findNearCanMovePlaceWithMinDis(node(api.GridToCell(Possiblenode1.x), api.GridToCell(Possiblenode1.y)));
                Goalstu.playerID = 4;
                Timeofgoalstu = prepare;
                Goalstu.x = api.CellToGrid(goal.x);
                Goalstu.y = api.CellToGrid(goal.y);
            }
        }
        else if (Classcopmlete < 7)
            goal = findNearCanMovePlaceWithMinDis(classroomNode[goalclass]);
        else
            goal = findNearCanMovePlaceWithMinDis(gatenode[goalgate]);
    }
#pragma endregion
#pragma region 巡逻
    else
    {
        if (Classcopmlete < 7)
            goal = findNearCanMovePlaceWithMinDis(classroomNode[goalclass]);
        else
            goal = findNearCanMovePlaceWithMinDis(gatenode[goalgate]);
    }
#pragma endregion
    Move(api, self, goal);
}
