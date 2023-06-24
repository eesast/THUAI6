#include <vector>
#include <thread>
#include <array>
#include <chrono>
#include <sstream>
#include <list>
#include <cfloat>
#include "AI.h"
#include "constants.h"
#ifndef MapSenseH
#define MapSenseH


#include "API.h"

#include "structures.h"

#define PI 3.14159265358979323846
#define Default_Door_Status 10000000

using namespace std;

// 打包Cell对应的(x, y)坐标
struct CellPos
{
    // 依据Cell (x, y)生成CellPos
    CellPos(int _x, int _y);
    // 依据当前玩家位置生成CellPos
    CellPos(const THUAI6::Player& player, IAPI& api);
    bool operator==(CellPos p2) const;
    bool operator!=(CellPos p2) const;
    friend float GetDistance(CellPos p1, CellPos p2);
    friend string to_string(CellPos pos);
    friend pair<int, int> CellPosToGrid(const CellPos pos, IAPI& api);
    int x, y;
    int gridX, gridY;
    bool center;
};

// 用于提供地图感知信息的一组方法
class MapSense
{
public:
    // 地图尺寸
    int width, height;
    // 制图原点
    CellPos Origin = InvalidCellPos;
    // 缓存的地图(二维数组，数组单元为单元类型)
    vector<vector<THUAI6::PlaceType>> BufferedMap;
    // 处理后的结果(二维数组，数组单元为(距离，上个单元))
    vector<vector<pair<float, CellPos>>> DealtMap;
    // 无效的上个单元标志符
    const CellPos InvalidCellPos = CellPos(-1, -1);
    // 翻窗代价
    float Pay_Windows = 2.5f;
    // 贴墙代价
    float Pay_Wall = 0.0f;
    // 正常陆地代价
    float Pay_Land = 1.0f;
    // 草地代价
    float Pay_Grass = 1.0f;
    // 特殊数据集
    map<THUAI6::PlaceType, vector<pair<CellPos, int32_t>>*> SpecialDataMap;
    // 自定义墙数据
    vector<CellPos> ConfigWalls;
    // 校门数据(两个一维数组，分别表示位置和是否开启，顺序为3, 5, 6)
    vector<pair<CellPos, int32_t>> Doors3;
    vector<pair<CellPos, int32_t>> Doors5;
    vector<pair<CellPos, int32_t>> Doors6;
    vector<pair<CellPos, int32_t>> Gates;
    vector<pair<CellPos, THUAI6::HiddenGateState>> HiddenGates;
    // 宝箱数据
    vector<pair<CellPos, int32_t>> Chests;
    // 教室数据
    vector<pair<CellPos, int32_t>> Classrooms;
    // 窗户数据
    vector<CellPos> Windows;
    // 工作台位置
    pair<CellPos, int32_t> CraftingBenchPos = pair(InvalidCellPos, 0);
    // 初始化地图数据
    MapSense(vector<vector<THUAI6::PlaceType>> data);
    // 依据玩家当前位置处理地图
    void DealMap(CellPos origin, float max_depth = FLT_MAX);
    // 是否处理完毕(试图处理炸内存问题)
    bool MapDealt = false;
    // 形成边界数组（菱形，depth为中心到四个角的长度）
    vector<CellPos> GenerateBoundary(CellPos center, int depth) const;
    // 形成直线数组（像素直线）
    vector<CellPos> GenerateLine(CellPos start, CellPos end, int length = -1) const;
    // 获取距离（兼容不可到达点）
    float GetDistance(CellPos target, int depth = -1) const;
    // 获取最近地点索引(必须先处理地图)
    int GetNearestIndex(vector<CellPos> list, int depth = -1) const;
    int GetNearestIndex(vector<pair<CellPos, int32_t>> list, int maxValue, int depth = -1) const;
    int GetNearestIndex(vector<pair<CellPos, THUAI6::HiddenGateState>> list, int depth = -1) const;
    // 摆正路径
    void AdjustPath(IAPI& api, THUAI6::Player player, bool forced = false);
    // 直接前往目标
    bool MoveTo(CellPos dest, IAPI& api, THUAI6::Player player, int reachedRadius = 1);
protected:
};

#endif


template<typename T>
bool Contains(list<T> data, T target)
{
    for (auto itr = data.begin(); itr != data.end(); ++itr)
        if (*itr == target)
            return true;
    return false;
}

CellPos::CellPos(int _x, int _y)
{
    x = _x, y = _y;
    gridX = gridY = 0;
    center = true;
}

CellPos::CellPos(const THUAI6::Player& player, IAPI& api)
{
    x = api.GridToCell(player.x);
    y = api.GridToCell(player.y);
    gridX = api.CellToGrid(x) - player.x, gridY = api.CellToGrid(y) - player.y;
    const int maxDistance = Constants::numOfGridPerCell / 2 - player.radius;
    center = (abs(gridX) <= maxDistance) && (abs(gridY) <= maxDistance);
}

float GetDistance(CellPos p1, CellPos p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

pair<int, int> CellPosToGrid(const CellPos pos, IAPI& api)
{
    int x, y;
    x = api.CellToGrid(pos.x) - pos.gridX;
    y = api.CellToGrid(pos.y) - pos.gridY;
    return make_pair(x, y);
}

string to_string(CellPos pos)
{
    return '(' + to_string(pos.x) + ',' + to_string(pos.y) + ')';
}

bool CellPos::operator==(CellPos p2) const
{
    return (x == p2.x) && (y == p2.y);
}

bool CellPos::operator!=(CellPos p2) const
{
    return (x != p2.x) || (y != p2.y);
}

MapSense::MapSense(vector<vector<THUAI6::PlaceType>> data)
{
    height = data.size(), width = data[0].size();
    BufferedMap = vector<vector<THUAI6::PlaceType>>();
    DealtMap = vector<vector<pair<float, CellPos>>>();
    ConfigWalls = vector<CellPos>();
    Doors3 = vector<pair<CellPos, int32_t>>();
    Doors5 = vector<pair<CellPos, int32_t>>();
    Doors6 = vector<pair<CellPos, int32_t>>();
    Gates = vector<pair<CellPos, int32_t>>();
    HiddenGates = vector<pair<CellPos, THUAI6::HiddenGateState>>();
    Chests = vector<pair<CellPos, int32_t>>();
    Classrooms = vector<pair<CellPos, int32_t>>();
    Windows = vector<CellPos>();
    SpecialDataMap = map<THUAI6::PlaceType, vector<pair<CellPos, int32_t>>*>();
    SpecialDataMap.emplace(THUAI6::PlaceType::Door3, &Doors3);
    SpecialDataMap.emplace(THUAI6::PlaceType::Door5, &Doors5);
    SpecialDataMap.emplace(THUAI6::PlaceType::Door6, &Doors6);
    SpecialDataMap.emplace(THUAI6::PlaceType::Gate, &Gates);
    SpecialDataMap.emplace(THUAI6::PlaceType::Chest, &Chests);
    SpecialDataMap.emplace(THUAI6::PlaceType::ClassRoom, &Classrooms);
    for (auto i = 0; i < height; i++)
    {
        BufferedMap.push_back(vector<THUAI6::PlaceType>());
        DealtMap.push_back(vector<pair<float, CellPos>>());
        for (auto j = 0; j < width; j++)
        {
            BufferedMap[i].push_back(data[i][j]);
            // 初始化特殊数据
            if (data[i][j] == THUAI6::PlaceType::Door3)
                Doors3.push_back(pair(CellPos(i, j), Default_Door_Status));
            if (data[i][j] == THUAI6::PlaceType::Door5)
                Doors5.push_back(pair(CellPos(i, j), Default_Door_Status));
            if (data[i][j] == THUAI6::PlaceType::Door6)
                Doors6.push_back(pair(CellPos(i, j), Default_Door_Status));
            if (data[i][j] == THUAI6::PlaceType::Gate)
                Gates.push_back(pair(CellPos(i, j), 0));
            if (data[i][j] == THUAI6::PlaceType::HiddenGate)
                HiddenGates.push_back(pair(CellPos(i, j), THUAI6::HiddenGateState::Null));
            if (data[i][j] == THUAI6::PlaceType::Chest)
                Chests.push_back(pair(CellPos(i, j), 0));
            if (data[i][j] == THUAI6::PlaceType::ClassRoom)
                Classrooms.push_back(pair(CellPos(i, j), 0));
            if (data[i][j] == THUAI6::PlaceType::Window)
                Windows.push_back(CellPos(i, j));
            // 初始化地图
            DealtMap[i].push_back(pair<float, CellPos>(-1.0f, InvalidCellPos));
        }
    }
}

void MapSense::DealMap(CellPos origin, float max_depth)
{
    using namespace std;
    Origin = origin;
    // 处理的临时结果
    vector<vector<pair<float, CellPos>>> TempMap;
    // 清空临时地图数据
    for (auto i = 0; i < height; ++i)
    {
        TempMap.push_back(vector<pair<float, CellPos>>());
        for (auto j = 0; j < width; ++j)
            TempMap[i].push_back(pair(-1.0f, InvalidCellPos));
    }
    // 设置原点数据
    TempMap[origin.x][origin.y] = pair(0, InvalidCellPos);
    // 定义扩展集与被扩展集
    list<CellPos> fReach = {}, fReachNeighbor = {};
    fReach.push_back(origin);
    while (fReach.size() != 0)
    {
        // 遍历要扩展的中心点
        for (auto p = fReach.begin(); p != fReach.end(); ++p)
        {
            // 当前位置信息
            float val = TempMap[p->x][p->y].first;		// 值
            if (val >= max_depth)
                continue;
            bool blocked = false;						// 不可通过模块权重是否已处理
            // 待扩展点列初始化（左右上下判断）
            auto q_list = vector<pair<CellPos, THUAI6::PlaceType>>();
            if (p->x > 1)	q_list.push_back(pair(CellPos(p->x - 1, p->y), BufferedMap[p->x - 1][p->y]));
            if (p->x < height - 1)	q_list.push_back(pair(CellPos(p->x + 1, p->y), BufferedMap[p->x + 1][p->y]));
            if (p->y > 1)	q_list.push_back(pair(CellPos(p->x, p->y - 1), BufferedMap[p->x][p->y - 1]));
            if (p->y < width - 1)	q_list.push_back(pair(CellPos(p->x, p->y + 1), BufferedMap[p->x][p->y + 1]));
            // 第一次遍历：改变自身状态
            for (auto q = q_list.begin(); q != q_list.end(); ++q)
            {
                // 根据特殊块的状态将对应块转变为可处理块
                switch (q->second)
                {
                case THUAI6::PlaceType::Door3:
                    for (auto door = Doors3.begin(); door != Doors3.end(); ++door)
                        if (door->first == q->first && door->second == Constants::maxDoorProgress)
                            q->second = THUAI6::PlaceType::Land;
                        else
                            q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::Door5:
                    for (auto door = Doors5.begin(); door != Doors5.end(); ++door)
                        if (door->first == q->first && door->second == Constants::maxDoorProgress)
                            q->second = THUAI6::PlaceType::Land;
                        else
                            q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::Door6:
                    for (auto door = Doors6.begin(); door != Doors6.end(); ++door)
                        if (door->first == q->first && door->second == Constants::maxDoorProgress)
                            q->second = THUAI6::PlaceType::Land;
                        else
                            q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::Gate:
                    for (auto door = Gates.begin(); door != Gates.end(); ++door)
                        if (door->first == q->first && door->second == Constants::maxGateProgress)
                            q->second = THUAI6::PlaceType::Land;
                        else
                            q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::HiddenGate:
                    for (auto door = HiddenGates.begin(); door != HiddenGates.end(); ++door)
                        if (door->first == q->first && door->second == THUAI6::HiddenGateState::Opened)
                            q->second = THUAI6::PlaceType::Land;
                        else
                            q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::Chest:
                    q->second = THUAI6::PlaceType::Wall;
                    break;
                case THUAI6::PlaceType::ClassRoom:
                    q->second = THUAI6::PlaceType::Wall;
                    break;
                }
                // 处理用户自定义墙
                for (auto wall = ConfigWalls.begin(); wall != ConfigWalls.end(); ++wall)
                    if (q->first == *wall)
                        q->second = THUAI6::PlaceType::Wall;
                // 处理工作台块
                if (q->first == CraftingBenchPos.first)
                    q->second = THUAI6::PlaceType::Wall;
                // 如果附近存在不可通过块，那么本身优先级应当下降
                if (q->second == THUAI6::PlaceType::Wall && !blocked)
                {
                    TempMap[p->x][p->y].first = val + Pay_Wall;
                    val = val + Pay_Wall;
                    blocked = true;
                }
            }
            // 第二次遍历：改变周围状态
            for (auto q_item = q_list.begin(); q_item != q_list.end(); ++q_item)
            {
                auto q = q_item->first;
                auto q_point = q_item->second;

                float curVal = 0, curParentVal = 0, newVal = 0;
                CellPos curParent = InvalidCellPos;
                switch (q_point)
                {
                case THUAI6::PlaceType::Wall:
                    break;
                case THUAI6::PlaceType::Land:
                    curVal = TempMap[q.x][q.y].first;
                    curParent = TempMap[q.x][q.y].second;
                    curParentVal = (curParent == InvalidCellPos) ? -1 : TempMap[curParent.x][curParent.y].first;
                    newVal = val + Pay_Land;
                    if (curVal < 0 || curVal > newVal || (curVal == newVal && val < curParentVal))
                    {
                        TempMap[q.x][q.y] = pair(newVal, *p);
                        fReachNeighbor.push_back(q);
                    }
                    break;
                case THUAI6::PlaceType::Grass:
                    curVal = TempMap[q.x][q.y].first;
                    curParent = TempMap[q.x][q.y].second;
                    curParentVal = (curParent == InvalidCellPos) ? -1 : TempMap[curParent.x][curParent.y].first;
                    newVal = val + Pay_Grass;
                    if (curVal < 0 || curVal > newVal || (curVal == newVal && val < curParentVal))
                    {
                        TempMap[q.x][q.y] = pair(newVal, *p);
                        fReachNeighbor.push_back(q);
                    }
                    break;
                case THUAI6::PlaceType::Window:
                    curVal = TempMap[q.x][q.y].first;
                    curParent = TempMap[q.x][q.y].second;
                    curParentVal = (curParent == InvalidCellPos) ? -1 : TempMap[curParent.x][curParent.y].first;
                    newVal = val + Pay_Land;
                    if (curVal < 0 || curVal > newVal || (curVal == newVal && val < curParentVal))
                    {
                        TempMap[q.x][q.y] = pair(newVal, *p);
                        fReachNeighbor.push_back(q);
                    }
                    break;
                }
            }
        }
        // 当次被扩展的点变为下次扩展的点
        fReach.clear();
        for (auto p = fReachNeighbor.begin(); p != fReachNeighbor.end(); ++p)
            fReach.push_back(*p);
        fReachNeighbor.clear();
    };
    // 拷贝结果
    MapDealt = false;
    for (int i = 0; i < TempMap.size(); ++i)
        for (int j = 0; j < TempMap[i].size(); ++j)
            DealtMap[i][j] = TempMap[i][j];
    MapDealt = true;
}


vector<CellPos> MapSense::GenerateBoundary(CellPos center, int depth) const
{
    vector<CellPos> result = vector<CellPos>();
    // 生成初步边界
    vector<CellPos> temp = vector<CellPos>();
    temp.push_back(CellPos(center.x, center.y + depth));
    temp.push_back(CellPos(center.x, center.y - depth));
    temp.push_back(CellPos(center.x + depth, center.y));
    temp.push_back(CellPos(center.x - depth, center.y));
    for (int i = 1; i < depth; i++)
    {
        temp.push_back(CellPos(center.x + i, center.y + depth - i));
        temp.push_back(CellPos(center.x - i, center.y + depth - i));
        temp.push_back(CellPos(center.x + i, center.y - depth + i));
        temp.push_back(CellPos(center.x - i, center.y - depth + i));
    }
    // 检测边界并写入结果
    for (auto i = temp.begin(); i != temp.end(); ++i)
        if (i->x >= 0 && i->x < height && i->y >= 0 && i->y < width)
            result.push_back(*i);
    return result;
}

vector<CellPos> MapSense::GenerateLine(CellPos start, CellPos end, int length) const
{
    vector<CellPos> result = vector<CellPos>();
    int x1 = start.x, x2 = end.x, y1 = start.y, y2 = end.y;
    int a = (y1 - y2), b = (x2 - x1), c = x1 * y2 - x2 * y1;
    int d = 2 * a + b + c, d1 = 2 * a, d2 = 2 * (a + b);
    int x = x1, y = y1;
    result.push_back(CellPos(x, y));
    int dx = (x2 - x1 > 0) ? 1 : -1, dy = (y2 - y1 > 0) ? 1 : -1;
    if (x2 - x1 == 0)
        for (; y != y2 + dy && (length < 0 || abs(y - y1) <= length); y += dy)
            result.push_back(CellPos(x, y));
    else if (y2 - y1 == 0)
        for (; x != x2 + dx && (length < 0 || abs(x - x1) <= length); x += dx)
            result.push_back(CellPos(x, y));
    else
    {
        for (; x != x2 + dx && (length < 0 || abs(x - x1) <= length); x += dx)
        {
            if (d < 0)
                y += dy, d += d2;
            else
                d += d1;
            result.push_back(CellPos(x, y));
        }
    }
    return result;
}

float MapSense::GetDistance(CellPos target, int depth) const
{
    // 如果该点为原点
    if (target == Origin)
        return 0.0f;
    // 如果该点可达
    if (DealtMap[target.x][target.y].second != InvalidCellPos)
        return DealtMap[target.x][target.y].first;
    else
    {
        int i = 1;
        if (depth < 0)
            depth = width + height;
        while (i <= depth)
        {
            vector<CellPos> boundary = GenerateBoundary(target, i);
            for (auto j = boundary.begin(); j != boundary.end(); ++j)
            {
                if (DealtMap[j->x][j->y].second != InvalidCellPos)
                    return DealtMap[j->x][j->y].first + i;
            }
            i++;
        }
    }
    return FLT_MAX;
}

int MapSense::GetNearestIndex(vector<CellPos> list, int depth) const
{
    while (!MapDealt);
    int result = -1;
    float min = FLT_MAX;
    int index = 0;
    for (auto i = list.begin(); i != list.end(); ++i, ++index)
    {
        auto f = GetDistance(*i, depth);
        if (f < min)
            result = index, min = f;
    }
    return result;
}

int MapSense::GetNearestIndex(vector<pair<CellPos, int32_t>> list, int maxValue, int depth) const
{
    auto _list = vector<CellPos>();
    for (auto i = list.begin(); i != list.end(); ++i)
        if (i->second < maxValue)
            _list.push_back(i->first);
    CellPos temp = _list[GetNearestIndex(_list, depth)];
    for (int i = 0; i < list.size(); i++)
        if (list[i].first == temp)
            return i;
}

int MapSense::GetNearestIndex(vector<pair<CellPos, THUAI6::HiddenGateState>> list, int depth) const
{
    auto _list = vector<CellPos>();
    for (auto i = list.begin(); i != list.end(); ++i)
        _list.push_back(i->first);
    return GetNearestIndex(_list, depth);
}

void MapSense::AdjustPath(IAPI& api, THUAI6::Player player, bool forced)
{
    static chrono::system_clock::time_point MapLastUpdate = chrono::system_clock::from_time_t(0);
    static CellPos lastPos = CellPos(-1, -1);
    auto pos = CellPos(player, api);
    auto speed = player.speed / 1000.0f;
    if (forced || pos == lastPos)
    {
        if (!pos.center && (forced || chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - MapLastUpdate).count() > 2000))
        {
            auto face = player.facingDirection;
            auto time = (int64_t)(sqrt(pos.gridX * pos.gridX + pos.gridY * pos.gridY) / speed);
            api.EndAllAction();
            api.Move(time, atan2(pos.gridY, pos.gridX)).wait_for(chrono::milliseconds(time));
            api.Move(1, face).wait_for(chrono::milliseconds(1));
            MapLastUpdate = chrono::system_clock::now();
        }
    }
    else
    {
        lastPos = pos;
        MapLastUpdate = chrono::system_clock::now();
    }
}

bool MapSense::MoveTo(CellPos dest, IAPI& api, THUAI6::Player player, int reachedRadius)
{
    auto pos = CellPos(player, api);
    auto d = GetDistance(dest);
    if (dest == pos || d <= reachedRadius)
    {
        return true;
    }
    // 若目的地无法到达，则逐步扩展边界
    int index = -1, r = 1;
    CellPos p = DealtMap[dest.x][dest.y].second;
    if (p == InvalidCellPos)
    {
        while (index < 0 && r <= reachedRadius)
        {
            auto boundary = GenerateBoundary(dest, r);
            index = GetNearestIndex(boundary, 0);
            // 边界仍不可达
            if (index < 0)
                r++;
            else
                dest = boundary[index];
        }
        if (index < 0)
            return false;
    }
    if (dest == pos)
    {
        if (!pos.center)
        {
            auto speed = player.speed / 1000.0f;
            api.Move((int64_t)(sqrt(pos.gridX * pos.gridX + pos.gridY * pos.gridY) / speed), atan2(pos.gridY, pos.gridX)).wait();
        }
        return true;
    }
    p = DealtMap[dest.x][dest.y].second;
    while (p != pos)
    {
        dest = p;
        p = DealtMap[p.x][p.y].second;
    }
    float speed = player.speed / 1000.0f;
    auto time = (int64_t)(numOfGridPerCell / speed);
    if (BufferedMap[dest.x][dest.y] == THUAI6::PlaceType::Window)
    {
        api.SkipWindow().wait_for(chrono::milliseconds(300));
        return false;
    }
    switch (dest.x - pos.x)
    {
    case 1:
        api.MoveDown(time).wait_for(chrono::milliseconds(time));
        break;
    case -1:
        api.MoveUp(time).wait_for(chrono::milliseconds(time));
        break;
    case 0:
        switch (dest.y - pos.y)
        {
        case 1:
            api.MoveRight(time).wait_for(chrono::milliseconds(time));
            break;
        case -1:
            api.MoveLeft(time).wait_for(chrono::milliseconds(time));
            break;
        }
        break;
    }
    AdjustPath(api, player);
    return false;
}







// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::Sunshine};    

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Idol;


enum PlayerState {
    Walking,        // 正常状态
    Waiting,        // 等待状态
    Working,        // 工作状态
    SpecialSkill,   // 技能状态
    FindGate,       // 寻找校门状态
    FoundGate,      // 准备毕业状态
    Escaping,       // 逃亡状态（检测到周围存在捣蛋鬼）
    Aiding,         // 支援状态
    Finished        // 终止状态
};

static bool F_IsInited = false;

static bool F_TrickerRefresh = false;

static CellPos PlayerPos = CellPos(-1, -1);

MapSense* MyMap;
MapSense* EnemyMap;

chrono::system_clock::time_point LastChangeCharacter = chrono::system_clock::from_time_t(0);
chrono::system_clock::time_point LastUseSkill = chrono::system_clock::from_time_t(0);

static char CharBuffer[4096] = { 0 };

static int TargetIndex;

static PlayerState State;

#pragma region 消息处理模块
static class {
public:
    static string StuToString(const THUAI6::Student& stu, IAPI& api)
    {
        ostringstream result = ostringstream();
        result << "S ";
        CellPos pos = CellPos(stu, api);
        result << pos.x << ' ' << pos.y << ' ';
        result << ' ' << stu.playerID << ' ' << stu.speed << ' ' << stu.addiction << ' ' << stu.determination;
        result << ' ' << (int)stu.studentType << ' ' << (int)stu.playerState;
        result << ' ' << stu.timeUntilSkillAvailable.size();
        for (auto i = stu.timeUntilSkillAvailable.begin(); i != stu.timeUntilSkillAvailable.end(); ++i)
            result << ' ' << *i;
        return result.str();
    }
    static THUAI6::Student StringToStu(const string& str, IAPI& api)
    {
        istringstream stream = istringstream(str);
        char c; int i; float f;
        CellPos pos = CellPos(-1, -1);
        THUAI6::Student stu = THUAI6::Student();
        stream >> c >> pos.x >> pos.y;
        stu.x = api.CellToGrid(pos.x), stu.y = api.CellToGrid(pos.y);
        stream >> stu.playerID >> stu.speed >> stu.addiction >> stu.determination;
        stream >> i;
        stu.studentType = (THUAI6::StudentType)i;
        stream >> i;
        stu.playerState = (THUAI6::PlayerState)i;
        stream >> i;
        for (int j = 0, k = 0; j < i; j++)
        {
            stream >> k;
            stu.timeUntilSkillAvailable.push_back(k);
        }
        return stu;
    }
    static string TriToString(const THUAI6::Tricker& tri, IAPI& api)
    {
        ostringstream result = ostringstream();
        result << "T ";
        CellPos pos = CellPos(tri, api);
        result << pos.x << ' ' << pos.y << ' ';
        result << ' ' << tri.playerID << ' ' << tri.speed << ' ' << tri.facingDirection << ' ' << tri.timeUntilSkillAvailable[0];
        result << ' ' << (int)tri.trickerType << ' ' << (int)tri.playerState;
        return result.str();
    }
    static THUAI6::Tricker StringToTri(const string& str, IAPI& api)
    {
        istringstream stream = istringstream(str);
        char c; int i; float f;
        CellPos pos = CellPos(-1, -1);
        THUAI6::Tricker tri = THUAI6::Tricker();
        stream >> c >> pos.x >> pos.y;
        tri.x = api.CellToGrid(pos.x), tri.y = api.CellToGrid(pos.y);
        stream >> tri.playerID >> tri.speed >> tri.facingDirection >> f;
        tri.timeUntilSkillAvailable.push_back(f);
        stream >> i;
        tri.trickerType = (THUAI6::TrickerType)i;
        stream >> i;
        tri.playerState = (THUAI6::PlayerState)i;
        return tri;
    }
    static string PlaceToString(const CellPos& place, const int progress, IAPI& api)
    {
        ostringstream result = ostringstream();
        result << "P ";
        result << place.x << ' ' << place.y << ' ' << progress;
        return result.str();
    }
    static pair<CellPos, int> StringToPlace(const string& str, IAPI& api)
    {
        istringstream stream = istringstream(str);
        char c; int i; float f;
        CellPos pos = CellPos(-1, -1);
        stream >> c >> pos.x >> pos.y >> i;
        return pair<CellPos, int>(pos, i);
    }
} Message;
#pragma endregion

// 队友信息
static vector<THUAI6::Student> BufferedStudent;

// 捣蛋鬼信息
static THUAI6::Tricker BufferedTricker;

// 学生处理相关信息
void DealMessage(IStudentAPI& api, int64_t id)
{
    // S开头为学生信息，T为捣蛋鬼信息，P为特殊进度地点信息
    while (api.HaveMessage())
    {
        auto temp = api.GetMessage();
        int64_t id = temp.first; string str = temp.second;
        if (str[0] == 'S')
            BufferedStudent[id] = Message.StringToStu(str, api);
        else if (str[0] == 'T')
        {
            BufferedTricker = Message.StringToTri(str, api);
            F_TrickerRefresh = true;
        }
        else if (str[0] == 'P')
        {
            auto temp = Message.StringToPlace(str, api);
            auto type = MyMap->BufferedMap[temp.first.x][temp.first.y];
            auto i = MyMap->SpecialDataMap[type];
            for (auto j = i->begin(); j != i->end(); j++)
                if (j->first == temp.first)
                {
                    j->second = temp.second;
                    break;
                }
        }
    }
    // 广播消息
    for (auto j = 0; j < BufferedStudent.size() && BufferedStudent[j].playerID >= 0; j++)
        if (j == id)
            continue;
        else
            api.SendTextMessage(BufferedStudent[j].playerID, Message.StuToString(*api.GetSelfInfo(), api));
}

// 学生巡视周围，检测队友和捣蛋鬼情况
void ScanAround(IStudentAPI& api, int64_t id)
{
    // 更新门当前状态
    auto PlayerInfo = api.GetSelfInfo();
    auto pos = CellPos(*PlayerInfo, api);
    // 不可见的门默认为关闭状态
    for (auto m = MyMap->SpecialDataMap.begin(); m != MyMap->SpecialDataMap.end(); ++m)
    {
        for (auto d = m->second->begin(); d != m->second->end(); ++d)
        {
            if (GetDistance(d->first, pos) < PlayerInfo->viewRange / Constants::numOfGridPerCell * 0.9f)
            {
                switch (m->first)
                {
                case THUAI6::PlaceType::Door3:
                case THUAI6::PlaceType::Door5:
                case THUAI6::PlaceType::Door6:
                    d->second = api.GetDoorProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::Gate:
                    d->second = api.GetGateProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::Chest:
                    d->second = api.GetChestProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::ClassRoom:
                    d->second = api.GetClassroomProgress(d->first.x, d->first.y);
                    break;
                }
                // 广播消息
                for (auto j = 0; j < BufferedStudent.size() && BufferedStudent[j].playerID >= 0; j++)
                    if (j == id)
                        continue;
                    else
                        api.SendTextMessage(BufferedStudent[j].playerID, Message.PlaceToString(d->first, d->second, api));
            }
        }
    }
    auto stus = api.GetStudents();
    for (auto i = stus.begin(); i != stus.end(); ++i)
        if ((*i)->playerID > 4)
            BufferedStudent[((*i)->playerID - 0) % 5 + 4] = **i;
        else
            BufferedStudent[(*i)->playerID] = **i;
    auto tris = api.GetTrickers();
    if (tris.size() > 0)
    {
        BufferedTricker = *tris[0];
        F_TrickerRefresh = true;
        for (auto j = 0; j < BufferedStudent.size() && BufferedStudent[j].playerID >= 0; j++)
            if (j == id)
                continue;
            else
                api.SendTextMessage(BufferedStudent[j].playerID, Message.TriToString(BufferedTricker, api));
    }
}

int32_t GetLostHp(THUAI6::Student player)
{
    static map<THUAI6::StudentType, int32_t> maxHpMap = {
        {THUAI6::StudentType::Athlete, Constants::Athlete::maxHp},
        {THUAI6::StudentType::Robot, Constants::Robot::maxHp},
        {THUAI6::StudentType::StraightAStudent, Constants::StraightAStudent::maxHp},
        {THUAI6::StudentType::Sunshine, Constants::Sunshine::maxHp},
        {THUAI6::StudentType::Teacher, Constants::Teacher::maxHp},
        {THUAI6::StudentType::TechOtaku, Constants::TechOtaku::maxHp}
    };
    return maxHpMap[player.studentType] - player.determination;
}

void AI::play(IStudentAPI& api)
{
#pragma region 公共操作
    static int index = 0;                   // 目标索引
    int finished = 0;                       // 完成进度
    auto PlayerInfo = api.GetSelfInfo();
    PlayerPos = CellPos(*PlayerInfo, api);  // 个人位置
    F_TrickerRefresh = false;
    // 初始化地图缓存
    if (!F_IsInited)
    {
        MyMap = new MapSense(api.GetFullMap());
        EnemyMap = new MapSense(api.GetFullMap());
        for (int i = 0; i < 7; i++)
        {
            BufferedStudent.push_back({});
            BufferedStudent[i].playerID = -1;
            BufferedStudent[i].playerState = THUAI6::PlayerState::Quit;
        }
        BufferedTricker.playerID = -1;
        F_IsInited = true;
    }
    // 刷新队友信息
    DealMessage(api, playerID);
    ScanAround(api, playerID);
    static chrono::system_clock::time_point TrickerLastUpdate = chrono::system_clock::from_time_t(0);
    static pair<CellPos, int32_t>* InterruptedTarget;
    static THUAI6::Tricker BufferedTrickerOrigin;
    // 如果捣蛋鬼位置刷新，则将距离最近且距离较短的作业点设置为已做完
    if (F_TrickerRefresh)
    {
        TrickerLastUpdate = chrono::system_clock::now();
        BufferedTrickerOrigin = BufferedTricker;
        EnemyMap->DealMap(CellPos(BufferedTricker, api));
        auto pos = EnemyMap->Classrooms[EnemyMap->GetNearestIndex(EnemyMap->Classrooms, Constants::maxClassroomProgress)];
        if (EnemyMap->DealtMap[pos.first.x][pos.first.y].first <= 7)
        {
            bool F_InterruptedTarget = false;
            for (auto cl = MyMap->Classrooms.begin(); cl != MyMap->Classrooms.end(); ++cl)
                if (cl->first == pos.first && cl->second != Constants::maxClassroomProgress)
                    InterruptedTarget = &(*cl), F_InterruptedTarget = true;
            if (F_InterruptedTarget)
                InterruptedTarget->second = Constants::maxClassroomProgress;
            else
                InterruptedTarget = nullptr;
        }
    }
    else
    {
        // 如果5s没有收到捣蛋鬼消息就当捣蛋鬼已经没了吧
        int milli = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - TrickerLastUpdate).count();
        if (milli >= 5000)
        {
            BufferedTricker.playerID = -1;
            // 如果确实有因为捣蛋鬼打断的
            if (InterruptedTarget != nullptr)
            {
                InterruptedTarget->second = 0;
                InterruptedTarget = nullptr;
            }
        }
        // 否则假定捣蛋鬼向最后一次前进方向前去
        else
        {
            BufferedTricker.x = BufferedTrickerOrigin.x + BufferedTricker.speed / 1000 * milli * cos(BufferedTricker.facingDirection);
            BufferedTricker.y = BufferedTrickerOrigin.y + BufferedTricker.speed / 1000 * milli * sin(BufferedTricker.facingDirection);
            if (BufferedTricker.x < 0)
                BufferedTricker.x = 0;
            if (BufferedTricker.x > MyMap->height * Constants::numOfGridPerCell - 1)
                BufferedTricker.x = MyMap->height * Constants::numOfGridPerCell - 1;
            if (BufferedTricker.y < 0)
                BufferedTricker.y = 0;
            if (BufferedTricker.y > MyMap->width * Constants::numOfGridPerCell - 1)
                BufferedTricker.y = MyMap->width * Constants::numOfGridPerCell - 1;
        }
    }
    MyMap->ConfigWalls.clear();
    // 将捣蛋鬼周围1*1菱形区域标志为不可通过区域
    if (BufferedTricker.playerID >= 0)
    {
        if (PlayerInfo->playerID != 2)
        {
            auto tri = CellPos(BufferedTricker, api);
            auto boundary = MyMap->GenerateBoundary(tri, 1);
            for (auto i = boundary.begin(); i != boundary.end(); ++i)
                MyMap->ConfigWalls.push_back(*i);
        }
        else
            MyMap->ConfigWalls.push_back(CellPos(BufferedTricker, api));
    }
    // 将玩家本体标志为不可通过状态
    for (int i = 0; i < 4; i++)
        if (i != playerID)
            MyMap->ConfigWalls.push_back(CellPos(BufferedStudent[i], api));
    // 处理地图
    MyMap->DealMap(PlayerPos);

#pragma endregion

    api.Print(to_string(BufferedTricker.trickDesire));
    if (this->playerID == 0 || this->playerID == 1)
    {
        // 玩家0/1执行操作
        // 如果捣蛋鬼到达一定距离，则进入逃离状态
        if (PlayerInfo->dangerAlert > 8 || (BufferedTricker.playerID >= 0 && MyMap->GetDistance(CellPos(BufferedTricker, api)) <= 10 && State != Escaping))
            api.EndAllAction(), State = Escaping, TargetIndex = -1;
        switch (State)
        {
        case Walking:
        {
            api.EndAllAction();
            // 前往最近的教室学习
            index = MyMap->GetNearestIndex(MyMap->Classrooms, Constants::maxClassroomProgress);
            if (MyMap->MoveTo(MyMap->Classrooms[index].first, api, *PlayerInfo))
            {
                TargetIndex = index;
                State = Working;
            }
            // 检测作业完成状态，如果已经达到额度则进入寻找校门状态
            for (auto i = MyMap->Classrooms.begin(); i != MyMap->Classrooms.end(); ++i)
                finished += (i->second == Constants::maxClassroomProgress);
            if (finished >= Constants::numOfRequiredClassroomForGate)
                State = FindGate;
            break;
        }
        case Working:
        {
            // 开始学习，学习状态下检测到技能cd转好立刻使用技能
            if (PlayerInfo->playerState != THUAI6::PlayerState::Learning)
                if (!api.StartLearning().get())
                    State = Walking;
            if (PlayerInfo->timeUntilSkillAvailable[0] == 0)
                api.UseSkill(0).wait();
            // 检测学习进度并全队共享
            auto classroom = &(MyMap->Classrooms[TargetIndex]);
            classroom->second = api.GetClassroomProgress(classroom->first.x, classroom->first.y);
            for (int i = 0; i < 4; i++)
                if (i != playerID && classroom->second != -1)
                    api.SendTextMessage(i, Message.PlaceToString(classroom->first, classroom->second, api));
            // 检测全队作业完成状态，如果已经达到额度则进入寻找校门状态
            for (auto i = MyMap->Classrooms.begin(); i != MyMap->Classrooms.end(); ++i)
                finished += (i->second == Constants::maxClassroomProgress);
            if (finished >= Constants::numOfRequiredClassroomForGate)
                State = FindGate;
            // 检测当前作业完成状态，如果已经做完则前往下一个目标
            if (classroom->second == Constants::maxClassroomProgress || classroom->second == -1)
            {
                State = Walking;
                index = MyMap->GetNearestIndex(MyMap->Classrooms, Constants::maxClassroomProgress);
            }
            break;
        }
        case FindGate:
        {
            // 前往距离最近的校门
            index = MyMap->GetNearestIndex(MyMap->Gates, Constants::maxClassroomProgress);
            if (MyMap->MoveTo(MyMap->Gates[index].first, api, *PlayerInfo))
            {
                TargetIndex = index;
                State = FoundGate;
            }
            break;
        }
        case FoundGate:
        {
            // 尝试开门并毕业
            if (PlayerInfo->playerState != THUAI6::PlayerState::OpeningAGate)
                api.StartOpenGate();
            auto gate = &(MyMap->Gates[TargetIndex]);
            api.Graduate();
            if (gate->second == Constants::maxGateProgress)
            {
                api.Graduate();
                State = Finished;
            }
            break;
        }
        case Escaping:
        {
            // 优先考虑钻入草丛
            auto list = vector<pair<CellPos, int>>();
            auto index = -1;
            auto distance = MyMap->GetDistance(CellPos(BufferedTricker, api));
            if (BufferedTricker.playerState != THUAI6::PlayerState::Swinging && ((distance > 4 && distance < 16) || MyMap->BufferedMap[PlayerPos.x][PlayerPos.y] == THUAI6::PlaceType::Grass))
            {
                // 捣蛋鬼距离适中时考虑钻入草丛
                for (int r = 1; r < 5; r++)
                {
                    auto boundary = MyMap->GenerateBoundary(PlayerPos, r);
                    for (int i = 0; i < boundary.size(); i++)
                    {
                        if (MyMap->BufferedMap[boundary[i].x][boundary[i].y] == THUAI6::PlaceType::Grass)
                        {
                            list.push_back(pair(boundary[i], Constants::maxClassroomProgress + 10));
                            index = 0;
                        }
                    }
                }
            }
            if (index < 0)
            {
                // 否则考虑前往较远的教室
                for (auto l = MyMap->Classrooms.begin(); l != MyMap->Classrooms.end(); ++l)
                    list.push_back(*l);
                int j = 0;
                for (int i = 0; i < list.size() / 2; i++)
                {
                    j = EnemyMap->GetNearestIndex(list, Constants::maxClassroomProgress + 1);
                    list[j].second = Constants::maxClassroomProgress + 10;
                }
                index = j;
            }
            // 检测到使用技能后向90度方向闪避
            auto bullets = api.GetBullets();
            if (!bullets.empty())
            {
                auto bomb = bullets[0];
                auto dir = bomb->facingDirection + PI / 2;
                api.Move(100, dir);
            }
            else if (MyMap->MoveTo(list[index].first, api, *PlayerInfo, 10))
                State = Walking, index = -1;
            // 如果捣蛋鬼远离(使用缓存防止蹲草)且沉迷度和毅力值均健康
            if (MyMap->GetDistance(CellPos(BufferedTricker, api)) >= 20
                && PlayerInfo->addiction < 10000
                && GetLostHp(*PlayerInfo) == 0)
                State = Walking;
            break;
        }
        case Finished:
            break;
        }
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        // 如果队友沉迷或毅力值小于最大生命值则赶向队友
        for (int i = 0; i < BufferedStudent.size(); i++)
            if (i != 2 && BufferedStudent[i].playerState != THUAI6::PlayerState::Quit &&
                (BufferedStudent[i].addiction > 10000 || GetLostHp(BufferedStudent[i]) > 0))
                index = i, State = Aiding;
        // 如果处于漫游状态且捣蛋鬼位置刷新，则进入捣蛋鬼追踪状态（如果是开心果则优先级低，否则优先级高）
        if (BufferedTricker.playerID >= 0 && (State == Walking || State == Waiting || (State == Aiding && (index != 3 || BufferedStudent[3].playerState == THUAI6::PlayerState::Quit))))
            api.EndAllAction(), State = Working;
        if (GetDistance(PlayerPos, CellPos(BufferedTricker, api)) <= Constants::Teacher::viewRange / Constants::numOfGridPerCell)
        {
            switch (BufferedTricker.playerState)
            {
            case THUAI6::PlayerState::Attacking:
            case THUAI6::PlayerState::UsingSpecialSkill:
            case THUAI6::PlayerState::Swinging:
            case THUAI6::PlayerState::Climbing:
            case THUAI6::PlayerState::Locking:
                if (PlayerInfo->timeUntilSkillAvailable[0] == 0)
                    api.UseSkill(0);
            }
        }
        switch (State)
        {
        case Walking:
        {
            // 前往开心果所在区域
            if (BufferedStudent[3].playerID < 0)
                return;
            // 如果前面有墙体障碍则试图使用穿墙术
            auto target = CellPos(BufferedStudent[3], api);
            auto detectLine = MyMap->GenerateLine(PlayerPos, target, 2);
            bool hasWall = false;
            for (auto i = detectLine.begin(); i != detectLine.end(); ++i)
                if (MyMap->BufferedMap[i->x][i->y] == THUAI6::PlaceType::Wall)
                    hasWall = true;
            if (hasWall && PlayerInfo->timeUntilSkillAvailable[1] == 0)
            {
                api.Move(10, atan2(target.y - PlayerPos.y, target.x - PlayerPos.x));
                api.UseSkill(1);
            }
            // 已经来到开心果附近，进入等待状态
            if (MyMap->MoveTo(CellPos(BufferedStudent[3], api), api, *PlayerInfo, 12))
                State = Waiting, TargetIndex = -1;
            break;
        }
        case Waiting:
        {
            // TO DO：巡视草丛，更新门情报，开启箱子......
            // 前往最近的箱子
            TargetIndex = TargetIndex < 0 ? MyMap->GetNearestIndex(MyMap->Chests, Constants::maxChestProgress) : TargetIndex;
            if (MyMap->GetDistance(MyMap->Chests[TargetIndex].first) <= 24)
            {
                if (MyMap->MoveTo(MyMap->Chests[TargetIndex].first, api, *PlayerInfo))
                {
                    TargetIndex = index;
                    if (PlayerInfo->playerState != THUAI6::PlayerState::OpeningAChest)
                        api.StartOpenChest();
                }
                auto l = api.GetProps();
                if (!l.empty())
                {
                    auto p = l[0];
                    api.PickProp(p->type);
                    TargetIndex = -1;
                }
            }

            // 如果和开心果距离太远就重新前往
            if (MyMap->GetDistance(CellPos(BufferedStudent[3], api)) > 40)
                State = Walking;
            break;
        }
        case Aiding:
        {
            if (BufferedStudent[index].playerState == THUAI6::PlayerState::Quit)
                State = Walking;
            // 如果前面有墙体障碍则试图使用穿墙术
            auto target = CellPos(BufferedStudent[index], api);
            auto detectLine = MyMap->GenerateLine(PlayerPos, target, 2);
            bool hasWall = false;
            for (auto i = detectLine.begin(); i != detectLine.end(); ++i)
                if (MyMap->BufferedMap[i->x][i->y] == THUAI6::PlaceType::Wall)
                    hasWall = true;
            if (hasWall && PlayerInfo->timeUntilSkillAvailable[1] == 0)
            {
                api.Move(10, atan2(target.y - PlayerPos.y, target.x - PlayerPos.x));
                api.UseSkill(1);
            }
            // 如果已经来到队友身边
            if (MyMap->MoveTo(target, api, *PlayerInfo))
            {
                auto line = MyMap->GenerateLine(target, CellPos(BufferedTricker, api))[1];
                MyMap->MoveTo(line, api, *PlayerInfo);
                // 如果对方已经进入沉迷状态
                if (index != 2 && BufferedStudent[index].addiction > 10000)
                {
                    auto p = api.GetProps();
                    for (auto i = p.begin(); i != p.end(); ++i)
                    {
                        if ((*i)->type == THUAI6::PropType::RecoveryFromDizziness)
                            api.UseProp((*i)->type);
                    }
                    api.StartRouseMate(BufferedStudent[index].playerID);
                }
                // 如果对方生命值小于最大生命值
                else if (index != 2 && GetLostHp(BufferedStudent[index]) > 0)
                {
                    auto p = api.GetProps();
                    for (auto i = p.begin(); i != p.end(); ++i)
                    {
                        if ((*i)->type == THUAI6::PropType::AddLifeOrClairaudience)
                            api.UseProp((*i)->type);
                    }
                    api.StartEncourageMate(BufferedStudent[index].playerID);
                }
                // 两个条件均为触发，说明对方已休息完毕
                else
                {
                    api.EndAllAction();
                    State = Walking;
                }
            }
            break;
        }
        case Working:
        {
            // 如果捣蛋鬼已经消失则放弃追逐
            if (BufferedTricker.playerID < 0)
                State = Walking;
            // 在队友总视野一定范围内追逐捣蛋鬼
            auto stus = vector<CellPos>();
            for (auto i = BufferedStudent.begin(); i != BufferedStudent.end(); ++i)
                stus.push_back(CellPos(*i, api));
            int index1 = EnemyMap->GetNearestIndex(stus);
            if (EnemyMap->GetDistance(stus[index1]) <= 30)
            {
                auto tri_1 = BufferedTricker;
                tri_1.x = tri_1.x + cos(tri_1.facingDirection) * Constants::numOfGridPerCell;
                tri_1.y = tri_1.y + sin(tri_1.facingDirection) * Constants::numOfGridPerCell;
                auto target = CellPos(tri_1, api);
                if (target.x < 0 || target.x >= MyMap->height || target.y < 0 || target.y >= MyMap->width || 
                    !(MyMap->BufferedMap[target.x][target.y] == THUAI6::PlaceType::Land || MyMap->BufferedMap[target.x][target.y] == THUAI6::PlaceType::Grass))
                {
                    auto boundary = MyMap->GenerateBoundary(CellPos(BufferedTricker, api), 1);
                    int index2 = MyMap->GetNearestIndex(boundary, 1);
                    target = boundary[index2];   
                }
                MyMap->MoveTo(target, api, *PlayerInfo, 1);
                if (PlayerInfo->timeUntilSkillAvailable[1] == 0)
                {
                    api.Move(10, atan2(target.y - PlayerPos.y, target.x - PlayerPos.x));
                    api.UseSkill(1);
                }
            }
            else
            {
                State = Walking;
            }
            break;
        }
        }
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        // 如果队友沉迷或毅力值小于最大生命值则赶向队友
        for (int i = 0; i < BufferedStudent.size(); i++)
            if (i != 3 && BufferedStudent[i].playerState != THUAI6::PlayerState::Quit && 
                (BufferedStudent[i].addiction > 10000 || GetLostHp(BufferedStudent[i]) > 0))
            {
                index = i, State = Aiding;
                break;
            }
        // 如果捣蛋鬼到达一定距离，则进入捣蛋鬼处理状态（以Escape代称）
        if (PlayerInfo->dangerAlert > 8 || (BufferedTricker.playerID >= 0 && MyMap->GetDistance(CellPos(BufferedTricker, api)) <= 10 && State != Escaping && State != Aiding))
            api.EndAllAction(), State = Escaping, TargetIndex = -1;
        switch (State)
        {
        case Walking:
        {
            api.EndAllAction();
            // 前往最近的教室学习，该教室此时应当无人学习，如果已经到达进入学习模式
            index = MyMap->GetNearestIndex(MyMap->Classrooms, Constants::maxClassroomProgress);
            if (MyMap->MoveTo(MyMap->Classrooms[index].first, api, *PlayerInfo))
            {
                TargetIndex = index;
                State = Working;
            }
            // 检测作业完成状态，如果已经达到额度则进入寻找校门状态
            for (auto i = MyMap->Classrooms.begin(); i != MyMap->Classrooms.end(); ++i)
                finished += (i->second == Constants::maxClassroomProgress);
            if (finished >= Constants::numOfRequiredClassroomForGate)
                State = FindGate;
            break;
        }
        case Working:
        {
            // 开始学习，学习状态下检测到技能cd转好立刻使用技能
            if (PlayerInfo->playerState != THUAI6::PlayerState::Learning)
                if (!api.StartLearning().get())
                    State = Walking;
            if (PlayerInfo->timeUntilSkillAvailable[0] == 0)
                api.UseSkill(0).wait();
            // 检测学习进度并全队共享
            auto classroom = &(MyMap->Classrooms[TargetIndex]);
            classroom->second = api.GetClassroomProgress(classroom->first.x, classroom->first.y);
            for (int i = 0; i < 4; i++)
                if (i != playerID && classroom->second != -1)
                    api.SendTextMessage(i, Message.PlaceToString(classroom->first, classroom->second, api));
            // 检测全队作业完成状态，如果已经达到额度则进入寻找校门状态
            for (auto i = MyMap->Classrooms.begin(); i != MyMap->Classrooms.end(); ++i)
                finished += (i->second == Constants::maxClassroomProgress);
            if (finished >= Constants::numOfRequiredClassroomForGate)
                State = FindGate;
            // 检测当前作业完成状态，如果已经做完则前往下一个目标
            if (classroom->second == Constants::maxClassroomProgress || classroom->second == -1)
            {
                State = Walking;
                index = MyMap->GetNearestIndex(MyMap->Classrooms, Constants::maxClassroomProgress);
            }
            break;
        }
        case Aiding:
        {
            if (BufferedStudent[index].playerState == THUAI6::PlayerState::Quit)
                State = Walking;
            if (MyMap->MoveTo(CellPos(BufferedStudent[index], api), api, *PlayerInfo))
            {
                // 如果对方已经进入沉迷状态
                if (index != 3 && BufferedStudent[index].addiction > 10000)
                {
                    // 如果技能1cd转好
                    if (PlayerInfo->timeUntilSkillAvailable[0] == 0)
                        api.UseSkill(0);
                    api.StartRouseMate(BufferedStudent[index].playerID);
                }
                // 如果对方生命值小于最大生命值
                else if (index != 3 && GetLostHp(BufferedStudent[index]) > 0)
                {
                    // 如果技能2cd转好
                    if (PlayerInfo->timeUntilSkillAvailable[1] == 0)
                        api.UseSkill(1);
                    api.StartEncourageMate(BufferedStudent[index].playerID);
                }
                // 两个条件均为触发，说明对方已休息完毕
                else
                {
                    api.EndAllAction();
                    State = Walking;
                }
            }
            break;
        }
        case FindGate:
        {
            // 前往距离最近的校门
            index = MyMap->GetNearestIndex(MyMap->Gates, Constants::maxClassroomProgress);
            if (MyMap->MoveTo(MyMap->Gates[index].first, api, *PlayerInfo))
            {
                TargetIndex = index;
                State = FoundGate;
            }
            break;
        }
        case FoundGate:
        {
            // 尝试开门并毕业
            if (PlayerInfo->playerState != THUAI6::PlayerState::OpeningAGate)
                api.StartOpenGate();
            auto gate = &(MyMap->Gates[TargetIndex]);
            api.Graduate();
            if (gate->second == Constants::maxGateProgress)
            {
                api.Graduate();
                State = Finished;
            }
            break;
        }
        case Escaping:
        {
            api.UseSkill(2);
            auto list = vector<pair<CellPos, int>>();
            auto index = -1;
            auto distance = MyMap->GetDistance(CellPos(BufferedTricker, api));
            if (BufferedTricker.playerState != THUAI6::PlayerState::Swinging && ((distance > 4 && distance < 16) || MyMap->BufferedMap[PlayerPos.x][PlayerPos.y] == THUAI6::PlaceType::Grass))
            {
                // 捣蛋鬼距离较近或已经在草丛里时考虑钻入草丛
                for (int r = 1; r < 5; r++)
                {
                    auto boundary = MyMap->GenerateBoundary(PlayerPos, r);
                    for (int i = 0; i < boundary.size(); i++)
                    {
                        if (MyMap->BufferedMap[boundary[i].x][boundary[i].y] == THUAI6::PlaceType::Grass)
                        {
                            list.push_back(pair(boundary[i], Constants::maxClassroomProgress + 10));
                            index = 0;
                        }
                    }
                }
            }
            if (index < 0)
            {
                // 否则考虑前往较远的教室
                for (auto l = MyMap->Classrooms.begin(); l != MyMap->Classrooms.end(); ++l)
                    list.push_back(*l);
                int j = 0;
                for (int i = 0; i < list.size() / 2; i++)
                {
                    j = EnemyMap->GetNearestIndex(list, Constants::maxClassroomProgress + 1);
                    list[j].second = Constants::maxClassroomProgress + 10;
                }
                auto b = MyMap->GenerateLine(list[j].first, PlayerPos);
                list[j].first = b[1];
                index = j;
            }
            // 检测到使用技能后向90度方向闪避
            auto bullets = api.GetBullets();
            if (!bullets.empty())
            {
                auto bomb = bullets[0];
                auto dir = bomb->facingDirection + PI / 2;
                api.Move(100, dir);
            }
            else if (MyMap->BufferedMap[PlayerPos.x][PlayerPos.y] == THUAI6::PlaceType::Grass)
                return;
            else if (MyMap->MoveTo(list[index].first, api, *PlayerInfo, 0))
                State = Walking, index = -1;
            // 如果捣蛋鬼远离(使用缓存防止蹲草)且沉迷度和毅力值均健康
            if (MyMap->GetDistance(CellPos(BufferedTricker, api)) >= 20
                && PlayerInfo->playerState != THUAI6::PlayerState::Addicted
                && PlayerInfo->determination == Constants::Sunshine::maxHp)
                State = Walking;
            break;
        }
        }
    }
    // 公共操作
}



static vector<shared_ptr<const THUAI6::Student>> PlayerInfo_Tricker;
int X = 0, Y = 0; double S = 0, D = 0;
double xx = 0, yy = 0;
int pp = 0;
int vic = 0;
vector<CellPos> fixedpoint = vector<CellPos>();
double distance(double a, double b)
{
    return sqrt(a * a + b * b);
}
//计算距离  （）内为x的差值和y的差值

//追逐学生   （）里分别为x，y，direction，speed
double dir(int i)
{

    int x, y, z;
    double a, b, c, d;
    x = BufferedStudent[i].x;
    y = BufferedStudent[i].y;
    c = BufferedStudent[i].facingDirection;
    z = BufferedStudent[i].speed;
    a = z * cos(c) / 20 + (x - X);
    b = z * sin(c) / 20 + (y - Y);
    d = atan2(b, a);
    return d;

};

void gt(double x, double y, IAPI& api)
{
    double q;
    q = atan2((y - Y), (x - X));
    api.Move(100, q);
}

void hunt(int a, int b, double c, int d, ITrickerAPI& ap)
{
    CellPos A((a + d * cos(c) / 20) / 1000, (b + d * sin(c) / 20) / 1000);
    MyMap->MoveTo(A, ap, *ap.GetSelfInfo(), 0);
    xx = a + d * cos(c) / 20;
    yy = b + d * sin(c) / 20;
}

// 捣蛋鬼巡视周围
void ScanAround(ITrickerAPI& api, int64_t id)
{
    auto PlayerInfo = api.GetSelfInfo();
    auto pos = CellPos(*PlayerInfo, api);
    for (auto m = MyMap->SpecialDataMap.begin(); m != MyMap->SpecialDataMap.end(); ++m)
    {
        for (auto d = m->second->begin(); d != m->second->end(); ++d)
        {
            if (GetDistance(d->first, pos) < PlayerInfo->viewRange / Constants::numOfGridPerCell * 0.9f)
            {
                switch (m->first)
                {
                case THUAI6::PlaceType::Door3:
                case THUAI6::PlaceType::Door5:
                case THUAI6::PlaceType::Door6:
                    d->second = api.GetDoorProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::Gate:
                    d->second = api.GetGateProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::Chest:
                    d->second = api.GetChestProgress(d->first.x, d->first.y);
                    break;
                case THUAI6::PlaceType::ClassRoom:
                    d->second = api.GetClassroomProgress(d->first.x, d->first.y);
                    break;
                }
            }
        }
    }
    auto stus = api.GetStudents();
    for (auto i = stus.begin(); i != stus.end(); ++i)
        if ((*i)->playerID > 4)
            BufferedStudent[((*i)->playerID - 0) % 5 + 4] = **i;
        else
            BufferedStudent[(*i)->playerID] = **i;
}


int inertia = 30;
void AI::play(ITrickerAPI& api)
{
#pragma region 公共操作
    static int index = 0;                   // 目标索引
    int finished = 0;                       // 完成进度
    auto PlayerInfo = api.GetSelfInfo();
    PlayerPos = CellPos(*PlayerInfo, api);  // 个人位置
   
    // 初始化地图缓存
    if (!F_IsInited)
    {
        MyMap = new MapSense(api.GetFullMap());
        EnemyMap = new MapSense(api.GetFullMap());
        for (auto t = MyMap->Classrooms.begin(); t != MyMap->Classrooms.end(); ++t)
            fixedpoint.push_back(t->first);

        for (int i = 0; i < 7; i++)
        {
            BufferedStudent.push_back({});
            BufferedStudent[i].playerID = -1;
            BufferedStudent[i].playerState = THUAI6::PlayerState::Quit;
        }
        F_IsInited = true;
    }
    for (int i = 0; i < 7; i++)
    {
        BufferedStudent[i].playerID = -1;
        BufferedStudent[i].playerState = THUAI6::PlayerState::Quit;
    }
    ScanAround(api, playerID);
    MyMap->ConfigWalls.clear();
    // 将玩家本体标志为不可通过状态
    for (int i = 0; i < 4; i++)
        if (BufferedStudent[i].studentType == THUAI6::StudentType::Teacher)
            MyMap->ConfigWalls.push_back(CellPos(BufferedStudent[i], api));
    // 处理地图
    MyMap->DealMap(PlayerPos);
#pragma endregion


    int x0 = 100000, y0 = 100000, x1 = 100000, y1 = 100000, x2 = 100000, y2 = 100000, x3 = 100000, y3 = 100000, ifaim = 0;
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0, a = 0;
    double d0 = 0, d1 = 0, d2 = 0, d3 = 0;


    //普通攻击   （）里为学生ID
    api.UseSkill(0);
    if (!F_IsInited)
    {
        PlayerInfo_Tricker = vector<shared_ptr<const THUAI6::Student>>(4);
        for (int i = 0; i < 4; i++)
            PlayerInfo_Tricker.push_back(std::make_shared<const THUAI6::Student>());
        F_IsInited = true;
    };
    auto selfInfo = api.GetSelfInfo();
    X = selfInfo->x; Y = selfInfo->y; S = selfInfo->speed; D = selfInfo->facingDirection;
    vector  aim = api.GetStudents();
    ifaim = 0;
    inertia = inertia + 1;

    double n[20];
    for (int i = 0; i < 20; i++)
    {
        n[i] = 0;
    }

    
    double x[20], y[20], s[20], d[20];
    for (int i = 0; i < 20; i++)
    {
        x[i] = 100000;
    }
    for (int i = 0; i < 20; i++)
    {
        y[i] = 100000;
    }
    for (auto i = BufferedStudent.begin(); i != BufferedStudent.end(); ++i)
    {
        if ( i->playerState == THUAI6::PlayerState::Quit
            || i->studentType == THUAI6::StudentType::Teacher)
            //GetDistance(CellPos(*i, api), PlayerPos) > PlayerInfo->viewRange / Constants::numOfGridPerCell * 1.1f
        {
            continue;
        }
        if (i->playerID >= 0 && i->playerState != THUAI6::PlayerState::Quit && i->playerState != THUAI6::PlayerState::Addicted && i->playerState != THUAI6::PlayerState::Roused
            && i->studentType != THUAI6::StudentType::Teacher)
        {
            x[i->playerID] = BufferedStudent[i->playerID].x; y[i->playerID] = BufferedStudent[i->playerID].y;
            s[i->playerID] = BufferedStudent[i->playerID].speed; d[i->playerID] = BufferedStudent[i->playerID].facingDirection;
            ifaim = 1; inertia = 0;
        }
    }
    for (int i = 0; i < 20; i++)
    {   
        n[i] = distance((x[i] - X), (y[i] - Y));
    }
    int flag = 0;
    if (ifaim == 1)
    {
        for (int i = 0; (i < 20) && (flag != 20); i++)
        {
            flag = 0;
            for (int j = 0; j < 20; j++)
            {
                if (n[i] <= n[j])
                    flag = flag + 1;
            }
            if (flag == 20)
                hunt(x[i], y[i], s[i], d[i], api);
        }

        for (int i = 0; i < 20; i++, api)
        {
            if (n[i] < 2000)
            {
                api.Attack(dir(i));
                pp = pp + 1;

            }
        }
    }
    
  
    for (auto stu = aim.begin(); stu != aim.end(); ++stu)
    {
        if ((*stu)->playerState == THUAI6::PlayerState::Addicted)
            ifaim = 1; //不要巡逻
    }

    //视野里消失后向其消失地点再追一段时间

//巡逻 
   
    
    if (ifaim == 0)
    {
        
        auto trickerinfo = api.GetSelfInfo();
        CellPos trickerpos = CellPos(*trickerinfo, api);
      
        vic = vic % 10;

        if (ifaim == 0)//巡逻
        {
            if ((inertia <= 40) && (inertia >= 2))
            {
                CellPos A(xx / 1000, yy / 1000);
                MyMap->MoveTo(A, api, *api.GetSelfInfo(), 2);
            }
            else {
                int rt = MyMap->GetDistance(fixedpoint[vic]);
                if (MyMap->GetDistance(fixedpoint[vic]) <= 9)
                    vic = vic + 1;
                vic = vic % 10;
                MyMap->MoveTo(fixedpoint[vic], api, *api.GetSelfInfo(), 5);
            }
            
        }

        
    }

}

