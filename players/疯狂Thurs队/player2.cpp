
/*
  Version 1.0.
*/
#include <vector>
#include <thread>
#include <array>
#include <chrono>
#include "AI.h"
#include "constants.h"
// 注意不要使用conio.h，Windows.h等非标准库


struct mapPoint {
    int cellX;
    int cellY;
    int f;
    mapPoint(int x = 0, int y = 0) :cellX(x), cellY(y) { f = 0; };
    bool operator==(const mapPoint& p)
    {
        if ((this->cellX == p.cellX) && (this->cellY == p.cellY)) return true;
        return false;
    }
    bool operator!=(const mapPoint& p)
    {
        if ((this->cellX != p.cellX) || (this->cellY != p.cellY)) return true;
        return false;
    }
};

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent
};

// 刺客职业
extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 全局变量
mapPoint nextDstPoint(0, 0), curPoint, lstPoint, dstPoint;      // 目标点，当前点，下步点
std::vector<std::vector<THUAI6::PlaceType> > map;               // 地图信息
std::vector<mapPoint> Homeworks;                                // 地图中所有作业的坐标
std::vector<mapPoint> Door3;                                    // 地图中三教门的坐标
std::vector<mapPoint> Door5;                                    // 地图中五教门的坐标
std::vector<mapPoint> Door6;                                    // 地图中六教门的坐标
std::vector<mapPoint> Chests;                                   // 地图中所有箱子的坐标
std::vector<mapPoint> Grass;                                    // 地图中所有草地的坐标
std::vector<mapPoint> Gates;                                    // 校门坐标
std::vector<mapPoint> HiddenGates;                              // 隐藏校门坐标
std::vector<std::vector<bool> > canArrivel;                     // 判断方格能否进入
std::vector<bool> isHomeworkFinished;                           // 判断作业是否做完
int finishedHomework = 0;                                       // 做完的作业数量
std::vector<std::vector<mapPoint> >roadMap;                     // 寻迹图
std::vector<mapPoint> Road;                                     // 道路
int roadRank = 0;                                               // 目前在道路第rank个点
mapPoint patrol[20] = { {15,18}, {15,7}, {25,7}, {25,10}, 
                        {27,10}, {32,16}, {32,18}, {40,18}, 
                        {40,28}, {33,34}, {24,34}, {21,46}, 
                        {15,46}, {15,41}, {6,32}, {4,32}, 
                        {4,25}, {7,25}, {7,20}, {13,20} };      // 巡逻点
int patrolIndex = -1;                                           // 当前巡逻点
// 可以在AI.cpp内部声明变量与函数

// 得到相对角度
double getAngle(double x, double y)
{
    return (atan2(y, x));
}

// 获取两cell点距离平方
int getDistance(mapPoint a, mapPoint b)
{
    return pow((a.cellX - b.cellX), 2) + pow((a.cellY - b.cellY), 2);
}

// 判断是否可达
bool canArrive(mapPoint p, mapPoint now)
{
    if (map[p.cellX][p.cellY] == THUAI6::PlaceType::Wall ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::Chest ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::HiddenGate ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::Gate ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::ClassRoom) return false;
    else if ((now.cellX != p.cellX) && (now.cellY != p.cellY))
    {
        if ((map[p.cellX][now.cellY] == THUAI6::PlaceType::Wall ||
             map[p.cellX][now.cellY] == THUAI6::PlaceType::Chest ||
             map[p.cellX][now.cellY] == THUAI6::PlaceType::HiddenGate ||
             map[p.cellX][now.cellY] == THUAI6::PlaceType::Gate ||
             map[p.cellX][now.cellY] == THUAI6::PlaceType::ClassRoom) &&
            (map[now.cellX][p.cellY] == THUAI6::PlaceType::Wall ||
             map[now.cellX][p.cellY] == THUAI6::PlaceType::Chest ||
             map[now.cellX][p.cellY] == THUAI6::PlaceType::HiddenGate ||
             map[now.cellX][p.cellY] == THUAI6::PlaceType::Gate ||
             map[now.cellX][p.cellY] == THUAI6::PlaceType::ClassRoom)) return false;
        else return true;
    }
    else return true;
}
bool canArrive(mapPoint p)
{
    if (map[p.cellX][p.cellY] == THUAI6::PlaceType::Wall ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::Chest ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::HiddenGate ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::Gate ||
        map[p.cellX][p.cellY] == THUAI6::PlaceType::ClassRoom) return false;  
    else return true;
}
std::vector<mapPoint> findSubRoad(IAPI& api, mapPoint dst, mapPoint now)
{
    std::vector<mapPoint> road;
    for (int i = 0; i < 50; ++i)
    {
        for (int j = 0; j < 50; ++j)
        {
            roadMap[i][j].f = 0;
            canArrivel[i][j] = true;
        }
    }
    
    auto student = api.GetStudents();
    auto tricker = api.GetTrickers();

    for (int i = 0; i < student.size(); ++i)
    {
        canArrivel[api.GridToCell(student[i]->x)][api.GridToCell(student[i]->y)] = false;
    }
    for (int i = 0; i < tricker.size(); ++i)
    {
        canArrivel[api.GridToCell(tricker[i]->x)][api.GridToCell(tricker[i]->y)] = false;
    }
    
    road.push_back(now);
    if (!canArrive(dst))
    {
        if (canArrive(roadMap[dst.cellX][dst.cellY + 1]))
            dst = roadMap[dst.cellX][dst.cellY + 1];
        else if (canArrive(roadMap[dst.cellX][dst.cellY - 1]))
            dst = roadMap[dst.cellX][dst.cellY - 1];
        else if (canArrive(roadMap[dst.cellX + 1][dst.cellY]))
            dst = roadMap[dst.cellX + 1][dst.cellY];
        else
            dst = roadMap[dst.cellX - 1][dst.cellY];
    }
    while (now != dst)
    {
        mapPoint nextPoint(0, 0);
        int mini = INT_MAX;
        for (int i = now.cellX - 1; i < now.cellX + 2; ++i)
        {
            for (int j = now.cellY - 1; j < now.cellY + 2; ++j)
            {
                if (i == now.cellX && j == now.cellY) continue;
                if (canArrive(roadMap[i][j], now))
                {
                    roadMap[i][j].f += ((std::abs(roadMap[i][j].cellX - dstPoint.cellX) +
                                         std::abs(roadMap[i][j].cellY - dstPoint.cellY)) * 10 + 
                                        roadMap[now.cellX][now.cellY].f);
                    if (i == now.cellX || j == now.cellY) roadMap[i][j].f += 10;
                    else roadMap[i][j].f += 14;
                    if(!canArrivel[i][j]) roadMap[i][j].f += 10000;
                    // if (map[i][j] == THUAI6::PlaceType::Window)  roadMap[i][j].f += 10;
                    if (map[i - 1][j] == THUAI6::PlaceType::Wall ||
                        map[i + 1][j] == THUAI6::PlaceType::Wall ||
                        map[i][j + 1] == THUAI6::PlaceType::Wall ||
                        map[i][j - 1] == THUAI6::PlaceType::Wall)
                    {
                        roadMap[i][j].f += 100;
                        if(map[i][j] == THUAI6::PlaceType::Window)
                            roadMap[i][j].f -= 87;
                    }
                    if (map[i][j] == THUAI6::PlaceType::Door3 ||
                        map[i][j] == THUAI6::PlaceType::Door5 ||
                        map[i][j] == THUAI6::PlaceType::Door6 ||
                        map[i][j] == THUAI6::PlaceType::Grass ||
                        map[i][j] == THUAI6::PlaceType::Land)
                    {
                        if ((map[i - 1][j] == THUAI6::PlaceType::Wall &&
                             map[i + 1][j] == THUAI6::PlaceType::Wall) ||
                            (map[i][j + 1] == THUAI6::PlaceType::Wall &&
                             map[i][j - 1] == THUAI6::PlaceType::Wall))
                        {
                            roadMap[i][j].f -= 87;
                        }
                    }
                    if (roadMap[i][j].f < mini)
                    {
                        nextPoint = roadMap[i][j];
                        mini = roadMap[i][j].f;
                    }
                }
            }
        }
        canArrivel[now.cellX][now.cellY] = false;
        now = nextPoint;
        road.push_back(now);      
    }
    for (int i = 0; i < road.size() - 1; ++i)
    {
        for (int j = i + 1; j < road.size(); ++j)
        {
            if (road[i] == road[j])
            {
                road.erase(road.begin() + i, road.begin() + j);
            }
        }
    }
    return road;
}
void findRoad(IAPI& api)
{
    Road.clear();
    roadRank = 0;
    std::vector<mapPoint> road = findSubRoad(api, dstPoint, curPoint);
    if (road.size() < 10)
    {
        Road = road;
        return;
    }
    mapPoint st_1 = curPoint, ed_1 = road[road.size() / 2],
             st_2 = road[road.size() / 2 + 1], ed_2 = dstPoint;
    std::vector<mapPoint> road_1 = findSubRoad(api, ed_1, st_1);
    std::vector<mapPoint> road_2 = findSubRoad(api, ed_2, st_2);
    api.Print("ed_1:" + std::to_string(ed_1.cellX) + " ," + std::to_string(ed_1.cellY));
    api.Print("st_2:" + std::to_string(st_2.cellX) + " ," + std::to_string(st_2.cellY));
    Road.insert(Road.end(), road_1.begin(), road_1.end());
    Road.insert(Road.end(), road_2.begin(), road_2.end());

    for (int i = 0; i < Road.size() - 1; ++i)
    {
        for (int j = i + 1; j < Road.size(); ++j)
        {
            if (Road[i] == Road[j])
            {
                Road.erase(Road.begin() + i, Road.begin() + j);
            }
        }
    }
    return;
}

// 学生移动到指定地图点
void moveToPoint(IStudentAPI& api, mapPoint p)
{
    auto self = api.GetSelfInfo();
    auto dst = mapPoint(api.CellToGrid(p.cellX), api.CellToGrid(p.cellY));
    if (map[p.cellX][p.cellY] == THUAI6::PlaceType::Window || abs(p.cellX - curPoint.cellX) > 1 || abs(p.cellY - curPoint.cellY) > 1)
    {
        api.SkipWindow();
        /*api.MoveUp(20);
        api.MoveLeft(20);
        api.MoveDown(20);
        api.MoveRight(20);*/
        /*std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
        return;
    }
    if ((abs(self->x - dst.cellX) > 50) || (abs(self->y - dst.cellY) > 50))
    {
        if (self->x - dst.cellX < -50) {
            api.MoveDown(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->x - dst.cellX > 50) {
            api.MoveUp(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->y - dst.cellY < -50) {
            api.MoveRight(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->y - dst.cellY > 50) {
            api.MoveLeft(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
    
}

// 捣蛋鬼移动到指定地图点
void moveToPoint(ITrickerAPI& api, mapPoint p)
{
    auto self = api.GetSelfInfo();
    auto dst = mapPoint(api.CellToGrid(p.cellX), api.CellToGrid(p.cellY));
    if (map[p.cellX][p.cellY] == THUAI6::PlaceType::Window || abs(p.cellX - curPoint.cellX) > 1 || abs(p.cellY - curPoint.cellY) > 1)
    {
        api.SkipWindow();
        /*api.MoveUp(20);
        api.MoveLeft(20);
        api.MoveDown(20);
        api.MoveRight(20);*/
        /*std::this_thread::sleep_for(std::chrono::milliseconds(100));*/
        return;
    }
    if ((abs(self->x - dst.cellX) > 50) || (abs(self->y - dst.cellY) > 50))
    {
        if (self->x - dst.cellX < -50) {
            api.MoveDown(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->x - dst.cellX > 50) {
            api.MoveUp(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->y - dst.cellY < -50) {
            api.MoveRight(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        if (self->y - dst.cellY > 50) {
            api.MoveLeft(30);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
    
}

//给一坐标点，找另一向草丛的点********NEW(done)
mapPoint findGrassPoint(IStudentAPI& api, mapPoint funcName)
{
    mapPoint grass;
    auto self = api.GetSelfInfo();
    int index = 0;
    int mini = INT_MAX;
    int innerProduct;
    for (int i = 0; i < Grass.size(); ++i)
    {
        innerProduct = (api.CellToGrid(Grass[i].cellX) - self->x) * (api.CellToGrid(funcName.cellX) - self->x) + 
                       (api.CellToGrid(Grass[i].cellY) - self->y) * (api.CellToGrid(funcName.cellY) - self->y);
        int dis = getDistance(Grass[i], funcName);
        if (dis < mini && innerProduct < 0)
        {
            index = i;
            mini = dis;
        }
    }
    grass = Grass[index];
    return grass;
}

void AI::play(IStudentAPI& api)
{ 
    if (map.empty())
    {
        map = api.GetFullMap();
        canArrivel.resize(map.size());
        roadMap.resize(map.size());
        for (int i = 0; i < map.size(); ++i)
        {
            canArrivel[i].resize(map[i].size());
            roadMap[i].resize(map[i].size());
            for (int j = 0; j < map[i].size(); ++j)
            {
                roadMap[i][j].cellX = i;
                roadMap[i][j].cellY = j;
                canArrivel[i][j] = true;
                switch (map[i][j])
                {
                case THUAI6::PlaceType::ClassRoom: {
                    Homeworks.push_back(mapPoint(i, j));
                    isHomeworkFinished.push_back(false);
                    break;
                }
                case THUAI6::PlaceType::Door3: {
                    Door3.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Door5: {
                    Door5.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Door6: {
                    Door6.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Chest: {
                    Chests.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Grass: {
                    Grass.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Gate: {
                    Gates.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::HiddenGate: {
                    HiddenGates.push_back(mapPoint(i, j));
                    break;
                }
                default: break;
                }
            }
        }
    }
    auto self = api.GetSelfInfo();
    curPoint = mapPoint(api.GridToCell(self->x), api.GridToCell(self->y));
    if (api.HaveMessage())
    {
        auto mes = api.GetMessage();
        if (mes.second[0] == 'F')
        {
            if (!isHomeworkFinished[mes.second[2] - '0'])
            {
                isHomeworkFinished[mes.second[2] - '0'] = true;
                finishedHomework++;
            }
            if (finishedHomework >= 7)
            {
                api.EndAllAction();
                nextDstPoint = mapPoint(0, 0);
            }
        }
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
        api.Print("current:" + std::to_string(curPoint.cellX) + "," + std::to_string(curPoint.cellY));// 玩家2执行操作
        api.Print("next:" + std::to_string(nextDstPoint.cellX) + "," + std::to_string(nextDstPoint.cellY));
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
    auto tricker = api.GetTrickers();
    if (tricker.empty())
    {
        if (!(curPoint == nextDstPoint) && !(nextDstPoint == mapPoint(0, 0)))
        {
            moveToPoint(api, nextDstPoint);
            return;
        }
        if (this->playerID == 2)
        {
            if (patrolIndex == -1)
            {
                int mini = INT_MAX;
                for (int i = 0; i < 20; ++i)
                {
                    int dis = pow((patrol[i].cellX - curPoint.cellX), 2) +
                        pow((patrol[i].cellY - curPoint.cellY), 2);
                    if (dis < mini)
                    {
                        patrolIndex = i;
                        mini = dis;
                    }
                }
            }
            mapPoint temp = dstPoint;
            dstPoint = patrol[patrolIndex];
            if (dstPoint != temp) findRoad(api);
            if (roadRank < Road.size())
            {
                lstPoint = nextDstPoint;
                nextDstPoint = Road[roadRank++];
            }
            else
            {
                if (patrolIndex < 19) patrolIndex++;
                else patrolIndex = 0;
                return;
            }
            moveToPoint(api, nextDstPoint);
            return;
        }
        
        if (finishedHomework < 7)
        {
            int index = 0;
            int mini = INT_MAX;
            for (int i = 0; i < Homeworks.size(); ++i)
            {
                if (!isHomeworkFinished[i])
                {
                    int dis = getDistance(Homeworks[i], curPoint);
                    if (dis < mini)
                    {
                        mini = dis;
                        index = i;
                    }
                }
            }
            mapPoint temp = dstPoint;
            dstPoint = Homeworks[index];
            if (dstPoint != temp) findRoad(api);
            
            if(getDistance(curPoint, Homeworks[index]) < 3)
            {
                if (api.GetClassroomProgress(Homeworks[index].cellX, Homeworks[index].cellY) == 10000000)
                {
                    isHomeworkFinished[index] = true;
                    finishedHomework++;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (i != self->playerID) api.SendTextMessage(i, "F:" + std::to_string(index));
                    }
                    return;
                }
                if (self->timeUntilSkillAvailable[0] == 0)
                {
                    api.UseSkill(0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }
                if (self->playerState == THUAI6::PlayerState::Learning) return;
                else api.StartLearning();
            }
            else if (roadRank < Road.size())
            {
                lstPoint = nextDstPoint;
                nextDstPoint = Road[roadRank++];
                api.Print("current:" + std::to_string(curPoint.cellX) + "," + std::to_string(curPoint.cellY));// 玩家2执行操作
                api.Print("next:" + std::to_string(nextDstPoint.cellX) + "," + std::to_string(nextDstPoint.cellY));
            }
            moveToPoint(api, nextDstPoint);
        }
        else
        {
            int index = 0;
            int mini = INT_MAX;
            for (int i = 0; i < Gates.size(); ++i)
            {
                int dis = getDistance(Gates[i], curPoint);
                if (dis < mini)
                {
                    mini = dis;
                    index = i;
                }    
            }
            mapPoint temp = dstPoint;
            dstPoint = Gates[index];
            if (dstPoint != temp) findRoad(api);
            if (getDistance(curPoint, Homeworks[index]) < 3)
            {
                if (api.GetGateProgress(Gates[index].cellX, Gates[index].cellY) == 18000) api.Graduate();
                if (self->playerState != THUAI6::PlayerState::OpeningAGate) api.StartOpenGate();
                else return;
            }
            if (roadRank < Road.size()) nextDstPoint = Road[roadRank ++];
            moveToPoint(api, nextDstPoint);
        }
        return;
    }
    else
    {
        mapPoint trickerPoint(api.GridToCell(tricker[0]->x), api.GridToCell(tricker[0]->y));
        if (!(curPoint == nextDstPoint) && !(nextDstPoint == mapPoint(0, 0)))
        {
            api.Print("destinationh:" + std::to_string(dstPoint.cellX) + "," + std::to_string(dstPoint.cellY));

            if (std::abs(curPoint.cellX - dstPoint.cellX) < 2 && std::abs(curPoint.cellY - dstPoint.cellY) < 2)
                moveToPoint(api, dstPoint);
            else moveToPoint(api, nextDstPoint);
            return;
        }
        if (this->playerID == 2)
        {
            api.UseSkill(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        
        mapPoint temp = dstPoint;
        dstPoint = findGrassPoint(api, trickerPoint);
        if (dstPoint != temp) findRoad(api);
        Road.push_back(dstPoint);
        if (roadRank < Road.size())
        {
            lstPoint = nextDstPoint;
            nextDstPoint = Road[roadRank++];
        }
        
        moveToPoint(api, nextDstPoint);
    }
}

void AI::play(ITrickerAPI& api)

{
    if (map.empty())
    {
        map = api.GetFullMap();
        canArrivel.resize(map.size());
        roadMap.resize(map.size());
        for (int i = 0; i < map.size(); ++i)
        {
            canArrivel[i].resize(map[i].size());
            roadMap[i].resize(map[i].size());
            for (int j = 0; j < map[i].size(); ++j)
            {
                roadMap[i][j].cellX = i;
                roadMap[i][j].cellY = j;
                canArrivel[i][j] = true;
                switch (map[i][j])
                {
                case THUAI6::PlaceType::ClassRoom: {
                    Homeworks.push_back(mapPoint(i, j));
                    isHomeworkFinished.push_back(false);
                    break;
                }
                case THUAI6::PlaceType::Door3: {
                    Door3.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Door5: {
                    Door5.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Door6: {
                    Door6.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Chest: {
                    Chests.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Grass: {
                    Grass.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::Gate: {
                    Gates.push_back(mapPoint(i, j));
                    break;
                }
                case THUAI6::PlaceType::HiddenGate: {
                    HiddenGates.push_back(mapPoint(i, j));
                    break;
                }
                default: break;
                }
            }
        }
    }
    auto self = api.GetSelfInfo();
    curPoint = mapPoint(api.GridToCell(self->x), api.GridToCell(self->y));
    auto students = api.GetStudents();
    bool studentAlive = false;
    for (int i = 0; i < students.size(); ++i)
    {

        if (students[i]->determination > 0)
        {
            studentAlive = true;
            break;
        }   
    }
    if (!students.empty() && studentAlive)
    {
        /*api.UseSkill(0);
        api.UseSkill(1);*/

        patrolIndex = -1;
        int index = -1;
        long long mini = LLONG_MAX;
        if (!(curPoint == nextDstPoint) && !(nextDstPoint == mapPoint(0, 0))) {
            moveToPoint(api, nextDstPoint);
            return;
        }
        
        for (int i = 0; i < students.size(); ++i)
        {
            if (students[i]->determination == 0) continue;
            long long dis = pow((self->x - students[i]->x), 2) + 
                            pow((self->y - students[i]->y), 2);
            if (dis < mini)
            {
                index = i;
                mini = dis;
            }
        }
        if (index == -1) return;
        mapPoint temp = dstPoint;
        dstPoint = mapPoint(api.GridToCell(students[index]->x),
                            api.GridToCell(students[index]->y));
        if (mini < pow(2000, 2))
        {
            double angle = getAngle((double)students[index]->x - (double)self->x, (double)students[index]->y - (double)self->y);
            api.EndAllAction();
            api.Attack(angle);
            api.Print("Attack!!!");
            return;
        }
        if (dstPoint != temp) findRoad(api);
        if (roadRank < Road.size())
        {
            lstPoint = nextDstPoint;
            nextDstPoint = Road[roadRank++];
        }
        else return;
        moveToPoint(api, nextDstPoint);
        return;
    }
    
    else 
    {
        if (!(curPoint == nextDstPoint) && !(nextDstPoint == mapPoint(0, 0))) {
            moveToPoint(api, nextDstPoint);
            return;
        }
        if (patrolIndex == -1)
        {
            int mini = INT_MAX;
            for (int i = 0; i < 20; ++i)
            {
                int dis = pow((patrol[i].cellX - curPoint.cellX), 2) +
                          pow((patrol[i].cellY - curPoint.cellY), 2);
                if (dis < mini)
                {
                    patrolIndex = i;
                    mini = dis;
                }
            }
        }     
        mapPoint temp = dstPoint;
        dstPoint = patrol[patrolIndex];
        if(dstPoint != temp) findRoad(api);
        if (roadRank < Road.size())
        {
            lstPoint = nextDstPoint;
            nextDstPoint = Road[roadRank++];
        }
        else
        {
            if (patrolIndex < 19) patrolIndex++;
            else patrolIndex = 0;
            return;
        }
        moveToPoint(api, nextDstPoint);
    }
}



