#include <vector>
#include <thread>
#include <array>
#include <chrono>
#include <queue>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>
#include "AI.h"
#include "constants.h"
using namespace std;
#define PI 3.14159

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

// 可以在AI.cpp内部声明变量与函数

// 全局变量区
bool ifchecked = false;
std::vector<std::vector<THUAI6::PlaceType>> Map;//全局地图
std::vector<std::pair<int32_t, int32_t>> Door3Position;// 3门的位置（x, y）
std::vector<std::pair<int32_t, int32_t>> Door5Position; // 5门的位置
std::vector<std::pair<int32_t, int32_t>> Door6Position; // 6门的位置
std::vector<std::pair<int32_t, int32_t>> ClassroomPosition;//教室的位置
std::vector<std::pair<int32_t, int32_t>> Chest;//箱子的位置
std::vector<std::pair<int32_t, int32_t>> Gate;//校门
static int num = 0;
struct Node
{
    int x, y;
    int Gcost, Hcost, Fcost;//0 1 2 空地，openlist，closelist
    int flag;
    struct Node* Parent;
};
Node NodeMap[50][50] = { 0 };
static int current = 0;
class MyFunction
{
public:

    //移动函数
    bool isLeftoutofcell(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y - 400 < celly * 1000) return true;
        else return false;
    }//判断是否在目标格左边一定范围

    bool isRightoutofcell(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y + 400 > (celly + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格右边一定范围

    bool isUpoutofcell(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x - 400 < cellx * 1000) return true;
        else return false;
    }//判断是否在目标格上边一定范围

    bool isDownoutofcell(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x + 400 > (cellx + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格下边一定范围

    bool isInsideofcellx(IStudentAPI& api, int32_t cellx) {
        if (!isUpoutofcell(api, cellx, -1) && !isDownoutofcell(api, cellx, -1)) return true;
        else return false;
    }
    //判断人是否整个在某行内

    bool isInsideofcelly(IStudentAPI& api, int32_t celly) {
        if (!isLeftoutofcell(api, -1, celly) && !isRightoutofcell(api, -1, celly)) return true;
        else return false;
    }
    //判断人是否整个在某列内
    bool isInsideofcell(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (isInsideofcellx(api, cellx) && isInsideofcelly(api, celly)) return true;
        else return false;
    }
    //判断人是否整个在某格子内
    void moveinto(IStudentAPI& api, int32_t cellx, int32_t celly) {
        if (!isInsideofcell(api, cellx, celly)) {
            if (isLeftoutofcell(api, cellx, celly)) api.MoveRight(30);
            else if (isRightoutofcell(api, cellx, celly)) api.MoveLeft(30);
            if (isUpoutofcell(api, cellx, celly)) api.MoveDown(30);
            else if (isDownoutofcell(api, cellx, celly)) api.MoveUp(30);
        }
    }
    //使人的碰撞箱进入一整格内，避免卡死等问题:只用于人在该格子附近的微调!

    bool ArriveAt(IStudentAPI& api, int32_t desX, int32_t desY)
    {
        auto self = api.GetSelfInfo();
        int32_t sx = self->x;
        int32_t sy = self->y;
        if (std::abs(sx - 1000 * desX - 500) + std::abs(sy - 1000 * desY - 500) < 800) return true;
        return false;
    }
    //Astar算法
    class Compare
    {
    public:
        Compare() {};
        bool operator()(const Node& node1, const Node& node2) const
        {
            return node1.Fcost > node2.Fcost;
        }
    }compare_by_cost;

    std::vector<std::pair<int32_t, int32_t>>Astar(IStudentAPI& api, int destX, int destY, Node Nodemap[50][50])
    {
        Node nodemap[50][50];
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                nodemap[i][j] = Nodemap[i][j];
            }
        }
        api.Print("Astar被调用");
        std::priority_queue<Node, std::vector<Node>, Compare> openList;
        auto self = api.GetSelfInfo();
        if (self->x / 1000 == destX && self->y / 1000 == destY)
        {
            std::vector<std::pair<int, int>> path{ std::pair<int, int>(destX,destY) };
            return path;//返回自身坐标是已经到了
        }
        auto& startNode = nodemap[self->x / 1000][self->y / 1000];
        startNode.Gcost = 0;
        startNode.Hcost = GetManhattanDistance(self->x / 1000, self->y / 1000, destX, destY);
        startNode.Fcost = GetManhattanDistance(self->x / 1000, self->y / 1000, destX, destY);
        startNode.flag = 1;
        startNode.Parent = nullptr;
        auto& endNode = nodemap[destX][destY];
        endNode.Gcost = 0; endNode.Hcost = 0, endNode.Fcost = 0; endNode.flag = 0; endNode.Parent = nullptr;
        openList.push(startNode);
        Node* currentNode = &startNode;
        while (!openList.empty() && !(currentNode->x == destX && currentNode->y == destY))
        {
            currentNode->flag = 2;
            openList.pop();
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    if (!(i * j) && !(i == 0 && j == 0))
                    {
                        if (isInMap(currentNode->x + i, currentNode->y + j))
                        {
                            Node* logicNode = &nodemap[currentNode->x + i][currentNode->y + j];
                            if (logicNode->x == destX && logicNode->y == destY)
                            {
                                logicNode->flag = 2;
                                logicNode->Parent = currentNode;
                                currentNode = logicNode;
                                goto here;
                            }
                            else if (!isWall((*logicNode).x, (*logicNode).y) && logicNode->flag == 0)
                            {
                                logicNode->Gcost = currentNode->Gcost + 1;
                                logicNode->flag = 1;
                                logicNode->Parent = currentNode;
                                logicNode->Hcost = GetManhattanDistance(logicNode->x, logicNode->y, destX, destY);
                                logicNode->Fcost = logicNode->Gcost + logicNode->Hcost;
                                openList.push(*logicNode);
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
            if (!openList.empty())
            {
                currentNode = &nodemap[openList.top().x][openList.top().y];
            }
        }
    here:
        if (currentNode->x == self->x / 1000 && currentNode->y == self->y / 1000)
        {
            std::vector<std::pair<int, int>> path;
            api.Print("到不了，返回空路径");
            return path;//返回空路径
        }
        auto path = GetPath(currentNode);
        api.Print(fmt::format("Astar返回值：path.size()={}", path.size()));
        return path;
    }

    int32_t GetManhattanDistance(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        return abs(x1 - x2) + abs(y1 - y2);
    }
    std::vector<std::pair<int, int>> GetPath(Node* endNode)
    {
        std::unique_ptr<std::vector<std::pair<int, int>>> path(new std::vector<std::pair<int, int>>);
        Node* node = endNode;
        while (node->Parent)
        {
            path->emplace_back(node->x, node->y);
            node = node->Parent;
        }
        path->emplace_back(node->x, node->y); // 添加起点
        std::reverse(path->begin(), path->end()); // 路径反转
        return *path;
    }
    bool isWall(int x, int y)
    {
        return Map.at(x).at(y) == THUAI6::PlaceType::Wall||Map.at(x).at(y)==THUAI6::PlaceType::ClassRoom;
    }
    bool isInMap(int x, int y)
    {
        return 0 <= x && x < 50 && 0 <= y && y < 50;
    }
    /*bool MoveToCertainPlace();*/
    bool ToNear(IStudentAPI& api, std::vector<std::pair<int32_t, int32_t>>set)
    {
        if (set.empty())return false;//空集或者去不了
        int size = set.size();
        int flag = -1;
        int path_len = 50 * 50;
        for (int i = 0; i < size; i++)
        {
            auto cur_path = Astar(api, set[i].first, set[i].second, NodeMap);
            if (cur_path.empty())continue;
            if (cur_path.size() < path_len)
            {
                flag = i;
            }
        }
        if (flag == -1)
        {
            return false;
        }
        else
        {
            auto path = Astar(api, set[flag].first, set[flag].second, NodeMap);
            if (path.size() > 1)
            {
                moveinto(api, path[1].first, path[1].second);
                api.Print("path>1");
                return true;
            }
            else
            {
                moveinto(api, path[0].first, path[0].second);
                api.Print("path=1");
                return true;
            }
        }
    }
}myf;
void delete_classroomposition(int i)
{
    ClassroomPosition.erase(ClassroomPosition.begin() + i);
}
//Gao:一个搜索的代码（可扩充）:04-24
std::array<int, 2> find(IStudentAPI& api, THUAI6::PlaceType type)
{
    //Gao:获取位置与视野:04-24
    int cellx, celly;
    cellx = (api.GetSelfInfo()->x) / 1000; celly = (api.GetSelfInfo()->y) / 1000;
    //Gao:从人物所在的格子向外搜索:04-24
    array<int, 2> start = { cellx,celly };
    queue<array<int, 2>> q;
    bool** visited = new bool* [50];
    for (int i = 0; i < 50; i++)
    {
        visited[i] = new bool[50];
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            visited[i][j] = { false };
        }
    }
    int direction[4][2] = { {0,1},{0,-1},{1,0},{-1,0} };
    q.push(start);
    visited[cellx][celly] = true;
    while (!q.empty())
    {
        array<int, 2> p = q.front();
        q.pop();
        for (int i = 0; i < 4; i++) {
            int nx = p[0] + direction[i][0];
            int ny = p[1] + direction[i][1];
            array<int, 2> current = { nx, ny };
            if (nx >= 0 && nx < 50 && ny >= 0 && ny < 50 && visited[nx][ny] == false)
            {
                if (api.GetPlaceType(nx, ny) == type)
                {
                    switch (type)
                    {
                    case THUAI6::PlaceType::NullPlaceType:
                        break;
                    case THUAI6::PlaceType::Land:
                        break;
                    case THUAI6::PlaceType::Wall:
                        break;
                    case THUAI6::PlaceType::Grass:
                        break;
                    case THUAI6::PlaceType::ClassRoom:
                    {
                        int j = 0;
                        for (j = 0; j < (int)ClassroomPosition.size(); j++)
                        {
                            if (ClassroomPosition[j].first == nx && ClassroomPosition[j].second == ny)
                            {
                                break;
                            }
                        }
                        if ((api.GetClassroomProgress(nx, ny) < 10000000|| api.GetClassroomProgress(nx, ny)==-1) && j != (int)ClassroomPosition.size()) { return current; }
                    }
                    break;
                    case THUAI6::PlaceType::Gate:
                        break;
                    case THUAI6::PlaceType::HiddenGate:
                        break;
                    case THUAI6::PlaceType::Window:
                    {
                        return current;
                    }
                        break;
                    case THUAI6::PlaceType::Door3:
                        break;
                    case THUAI6::PlaceType::Door5:
                        break;
                    case THUAI6::PlaceType::Door6:
                        break;
                    case THUAI6::PlaceType::Chest:
                        break;
                    default:
                        break;
                    }
                }
                q.push(current);
                visited[nx][ny] = true;
            }
        }
    }
    for (int i = 0; i < 50; i++) {
        delete[] visited[i];
    }
    delete[] visited;
    return array<int, 2>{-1, -1};
}

std::vector<std::vector<int>> getMessages(IStudentAPI & api)
{
    vector<vector<int>>messages;
    while (api.HaveMessage())
    {
        auto a = api.GetMessage();
        int x = ((int)a.second[1]-48) * 10 + (int)a.second[2]-48;
        int y = ((int)a.second[3]-48) * 10 + (int)a.second[4]-48;
        messages.push_back({ (int)a.first,(int)a.second[0] - 48,x,y });
        //1 代表一台机子修好；2 代表7台机子修好；3 代表门开好；4代表可以毕业
    }
    return messages;
}
void sendMessage(IStudentAPI& api,int a, int b, int x, int y)
{
    string str1 = fmt::format("{:d}{:02}{:02}", b, x, y);
    api.SendMessage(a,str1);
}

std::array<int, 2> hw0 = { 0,0 };
std::array<int, 2> hw1 = { 0,0 };
std::array<int, 2> hw2 = { 0,0 };
std::array<int, 2> hw3 = { 0,0 };
std::array<int, 2> lo0 = { 0,0 };
std::array<int, 2> lo1 = { 0,0 };
std::array<int, 2> lo2 = { 0,0 };
std::array<int, 2> lo3 = { 0,0 };

void AI::play(IStudentAPI& api)
{
    if (!ifchecked)
    {
        api.Print("check map!");
        Map.resize(50 * 50);
        Map = api.GetFullMap();
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                switch (api.GetPlaceType(i,j))
                {
                case(THUAI6::PlaceType::NullPlaceType):;
                case(THUAI6::PlaceType::Land):;
                case(THUAI6::PlaceType::Grass):NodeMap[i][j] = { i,j,0,0,0,0,nullptr };; break;
                case(THUAI6::PlaceType::Chest):Chest.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door3):Door3Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door5):Door5Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door6):Door6Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Gate):Gate.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::ClassRoom):ClassroomPosition.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                }
            }
        }//弄个大地图
        ifchecked = true;
    }
    if (this->playerID == 0)
    {
        // 玩家0执行操作
        bool gra = false;
        if (api.GetSelfInfo()->x == lo0[0] && api.GetSelfInfo()->y == lo0[1])
        {
            myf.moveinto(api, (api.GetSelfInfo()->x) / 1000, (api.GetSelfInfo()->y) / 1000);
        }
        lo0 = { api.GetSelfInfo()->x ,api.GetSelfInfo()->y };
        if (api.HaveMessage())
        {
            vector<vector<int>> mes;
            mes = getMessages(api);
            for (int i = 0; i<(int)mes.size(); ++i)
            {
                switch (mes[i][1])
                {
                case 1: 
                {
                    for (int j = 0; j < (int)ClassroomPosition.size();++j)
                    {
                        if (ClassroomPosition[j].first == mes[i][2] && ClassroomPosition[j].second == mes[i][3])ClassroomPosition.erase(ClassroomPosition.begin() + j);
                    }
                }
                break;
                case 2: break;
                case 3: break;
                case 4: gra = true; break;
                default:break;
                }
            }
        }
        if (!gra)
        {
            if (hw0 == find(api, THUAI6::PlaceType::ClassRoom )|| api.GetClassroomProgress(hw0[0], hw0[1]) == 10000000 || (hw0[0] == 0 && hw0[1] == 0))
            {
                hw0 = find(api, THUAI6::PlaceType::ClassRoom);
            }
            if ((int)myf.Astar(api, hw0[0], hw0[1], NodeMap).size() != 0) {
                auto hw20 = myf.Astar(api, hw0[0], hw0[1], NodeMap);
                myf.moveinto(api, hw20[1].first, hw20[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - hw0[0]) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - hw0[1]) <= 1)
                {
                    api.UseSkill(0);
                    api.StartLearning();
                    sendMessage(api, 1, 1, hw0[0], hw0[1]);
                }
            }
        }
        else
        {
            if ((int)myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap).size() != 0) {
                auto gate = myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap);
                myf.moveinto(api, gate[1].first, gate[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - Gate[1].first) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - Gate[1].second) <= 1) api.Graduate();
            }
        }
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
        auto trickers = api.GetTrickers();
        if (api.GetGameInfo()->studentGraduated==3)
        {
            if ((int)myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap).size() != 0) {
                auto gate = myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap);
                myf.moveinto(api, gate[1].first, gate[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - Gate[1].first) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - Gate[1].second) <= 1) api.Graduate();
            }
        }
        if (trickers.size() != 0)
        {
            api.EndAllAction();
            if ((int)myf.Astar(api, trickers[0]->x / 1000, trickers[0]->y / 1000, NodeMap).size() != 0) {
                auto follow_Trickers = myf.Astar(api, trickers[0]->x / 1000, trickers[0]->y / 1000, NodeMap);
                myf.ToNear(api, follow_Trickers);
                api.UseSkill(0);
            }
        }
        else
        {
            if ((hw1 == find(api, THUAI6::PlaceType::ClassRoom) || api.GetClassroomProgress(hw1[0], hw1[1]) == 10000000) || (hw1[0] == 0 && hw1[1] == 0))
            {
                hw1 = find(api, THUAI6::PlaceType::ClassRoom);
            }
            if ((int)api.GetStudents().size() != 0 && abs(api.GetStudents()[0]->x - api.GetSelfInfo()->x) <= 1 && abs(api.GetStudents()[0]->y - api.GetSelfInfo()->y) <= 1)
            {
                api.MoveLeft(100);
                api.MoveUp(100);
                api.MoveDown(100);
                api.MoveRight(100);
            }
            if ((int)myf.Astar(api, hw1[0], hw1[1], NodeMap).size() != 0) {
                auto hw21 = myf.Astar(api, hw1[0], hw1[1], NodeMap);
                myf.moveinto(api, hw21[1].first, hw21[1].second);
            }
        }
        if (api.HaveMessage())
        {
            vector<vector<int>> mes;
            mes = getMessages(api);
            for (int i = 0;i< (int)mes.size(); ++i)
            {
                switch (mes[i][1])
                {
                case 1:
                {
                    for (int j = 0; j < (int)ClassroomPosition.size(); ++j)
                    {
                        if (ClassroomPosition[j].first == mes[i][2] && ClassroomPosition[j].second == mes[i][3])ClassroomPosition.erase(ClassroomPosition.begin() + j);
                    }
                    sendMessage(api, 0, 1, mes[i][2], mes[i][3]);
                    sendMessage(api, 2, 1, mes[i][2], mes[i][3]);
                    sendMessage(api, 3, 1, mes[i][2], mes[i][3]);
                }
                break;
                case 2:break;
                case 3:sendMessage(api,0,4,0,0); break;
                case 4:break;
                default:break;
                }
            }
        }
        if ((int)ClassroomPosition.size()<=3)
        {
            sendMessage(api, 2, 2, 0, 0);
            sendMessage(api, 3, 2, 0, 0);
        }
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        bool opendoorRequest2 = false;
        if (api.GetSelfInfo()->x == lo2[0] && api.GetSelfInfo()->y == lo2[1])
        {
            myf.moveinto(api, (api.GetSelfInfo()->x) / 1000, (api.GetSelfInfo()->y) / 1000);
        }
        lo2 = { api.GetSelfInfo()->x ,api.GetSelfInfo()->y };
        if (api.HaveMessage())
        {
            vector<vector<int>> mes;
            mes = getMessages(api);
            for (int i = 0; i<(int)mes.size(); ++i)
            {
                switch (mes[i][1])
                {
                case 1: 
                {
                    for (int j = 0; j < (int)ClassroomPosition.size(); ++j)
                    {
                        if (ClassroomPosition[j].first == mes[i][2] && ClassroomPosition[j].second == mes[i][3])ClassroomPosition.erase(ClassroomPosition.begin() + j);
                    }
                }
                break;
                case 2: opendoorRequest2 = true; break;
                case 3: break;
                case 4: break;
                default:break;
                }
            }
        }
        if (opendoorRequest2&&(api.GetClassroomProgress(hw2[0], hw2[1]) == 10000000 || api.GetClassroomProgress(hw2[0], hw2[1]) == -1))
        {
            if ((int)myf.Astar(api, Gate[0].first, Gate[0].second, NodeMap).size() != 0) {
                auto gate22 = myf.Astar(api, Gate[0].first, Gate[0].second, NodeMap);
                myf.moveinto(api, gate22[1].first, gate22[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - Gate[0].first) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - Gate[0].second) <= 1)
                {
                    api.StartOpenGate();
                }
                if (api.GetGateProgress(Gate[0].first, Gate[0].second) == 18000)
                {
                    sendMessage(api, 1, 3, Gate[0].first, Gate[0].second);
                    api.Graduate();
                }
            }
        }
        else
        {
            if ((hw2 == find(api, THUAI6::PlaceType::ClassRoom) || api.GetClassroomProgress(hw2[0], hw2[1]) == 10000000) || (hw2[0] == 0 && hw2[1] == 0))
            {
                hw2 = find(api, THUAI6::PlaceType::ClassRoom);
            }
            if (api.GetStudents().size() != 0 && abs(api.GetStudents()[0]->x - api.GetSelfInfo()->x) <= 1 && abs(api.GetStudents()[0]->y - api.GetSelfInfo()->y) <= 1)
            {
                api.MoveLeft(100);
                api.MoveDown(100);
                api.MoveUp(100);
                api.MoveRight(100);

            }
            if ((int)myf.Astar(api, hw2[0], hw2[1], NodeMap).size() != 0) {
                auto hw22 = myf.Astar(api, hw2[0], hw2[1], NodeMap);
                myf.moveinto(api, hw22[1].first, hw22[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - hw2[0]) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - hw2[1]) <= 1)
                {
                    api.UseSkill(0);
                    api.StartLearning();
                    sendMessage(api, 1, 1, hw2[0], hw2[1]);
                }
            }
        }
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        bool opendoorRequest3 = false;
        if (api.GetSelfInfo()->x == lo3[0] && api.GetSelfInfo()->y == lo3[1])
        {
            myf.moveinto(api, (api.GetSelfInfo()->x) / 1000, (api.GetSelfInfo()->y) / 1000);
            lo3 = { api.GetSelfInfo()->x ,api.GetSelfInfo()->y };
        }
        lo3 = { api.GetSelfInfo()->x ,api.GetSelfInfo()->y };
        api.UseSkill(2);
        if (api.HaveMessage())
        {
            vector<vector<int>> mes;
            mes = getMessages(api);
            for (int i = 0; i<(int)mes.size(); ++i)
            {
                switch (mes[i][1])
                {
                case 1:
                {
                    for (int j = 0; j < (int)ClassroomPosition.size(); ++j)
                    {
                        if (ClassroomPosition[j].first == mes[i][2] && ClassroomPosition[j].second == mes[i][3])ClassroomPosition.erase(ClassroomPosition.begin() + j);
                    }
                }
                break;
                case 2: opendoorRequest3 = true; break;
                case 3: break;
                case 4: break;
                default:break;
                }
            }
        }
        if (opendoorRequest3&& (api.GetClassroomProgress(hw3[0], hw3[1]) == 10000000|| api.GetClassroomProgress(hw3[0], hw3[1]) == -1))
        {
            if ((int)myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap).size() != 0) {
                auto gate23 = myf.Astar(api, Gate[1].first, Gate[1].second, NodeMap);
                myf.moveinto(api, gate23[1].first, gate23[1].second);

                if (abs((api.GetSelfInfo()->x) / 1000 - Gate[1].first) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - Gate[1].second) <= 1)
                {
                    api.StartOpenGate();
                }
                if (api.GetGateProgress(Gate[1].first, Gate[1].second) == 18000)
                {
                    sendMessage(api, 1, 3, Gate[1].first, Gate[1].second);
                    api.Graduate();
                }
            }
        }
        else
        {
            if ((hw3 == find(api, THUAI6::PlaceType::ClassRoom) || api.GetClassroomProgress(hw3[0], hw3[1]) == 10000000) || (hw3[0] == 0 && hw3[1] == 0))
            {
                hw3 = find(api, THUAI6::PlaceType::ClassRoom);
            }
            if (abs(api.GetStudents()[0]->x - api.GetSelfInfo()->x) <= 1 && abs(api.GetStudents()[0]->y - api.GetSelfInfo()->y)<=1)
            {
                api.MoveLeft(200);
                api.MoveUp(200);
                api.MoveRight(200);
                api.MoveDown(200);
            }
            if ((int)myf.Astar(api, hw3[0], hw3[1], NodeMap).size() != 0) {
                auto hw23 = myf.Astar(api, hw3[0], hw3[1], NodeMap);
                myf.moveinto(api, hw23[1].first, hw23[1].second);
                if (abs((api.GetSelfInfo()->x) / 1000 - hw3[0]) <= 1 && abs((api.GetSelfInfo()->y) / 1000 - hw3[1]) <= 1)
                {
                    api.StartLearning();
                    sendMessage(api, 1, 1, hw3[0], hw3[1]);
                }
            }
        }
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
    for (int i = 0; i < (int)ClassroomPosition.size(); ++i)
    {
        if (ClassroomPosition.size()!=0 && api.GetClassroomProgress(ClassroomPosition[i].first, ClassroomPosition[i].second) == 10000000)
        {
            delete_classroomposition(i);
        }
    }
}

class MyTrickerFunction
{
public:

    //移动函数
    bool t_isLeftoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y - 400 < celly * 1000) return true;
        else return false;
    }//判断是否在目标格左边一定范围

    bool t_isRightoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y + 400 > (celly + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格右边一定范围

    bool t_isUpoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x - 400 < cellx * 1000) return true;
        else return false;
    }//判断是否在目标格上边一定范围

    bool t_isDownoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x + 400 > (cellx + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格下边一定范围

    bool t_isInsideofcellx(ITrickerAPI& api, int32_t cellx) {
        if (!t_isUpoutofcell(api, cellx, -1) && !t_isDownoutofcell(api, cellx, -1)) return true;
        else return false;
    }
    //判断人是否整个在某行内

    bool t_isInsideofcelly(ITrickerAPI& api, int32_t celly) {
        if (!t_isLeftoutofcell(api, -1, celly) && !t_isRightoutofcell(api, -1, celly)) return true;
        else return false;
    }
    //判断人是否整个在某列内
    bool t_isInsideofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (t_isInsideofcellx(api, cellx) && t_isInsideofcelly(api, celly)) return true;
        else return false;
    }
    //判断人是否整个在某格子内
    void t_moveinto(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (!t_isInsideofcell(api, cellx, celly)) {
            if (t_isLeftoutofcell(api, cellx, celly)) api.MoveRight(30);
            else if (t_isRightoutofcell(api, cellx, celly)) api.MoveLeft(30);
            if (t_isUpoutofcell(api, cellx, celly)) api.MoveDown(30);
            else if (t_isDownoutofcell(api, cellx, celly)) api.MoveUp(30);
        }
    }
    //使人的碰撞箱进入一整格内，避免卡死等问题:只用于人在该格子附近的微调!

    bool t_ArriveAt(ITrickerAPI& api, int32_t desX, int32_t desY)
    {
        auto self = api.GetSelfInfo();
        int32_t sx = self->x;
        int32_t sy = self->y;
        if (std::abs(sx - 1000 * desX - 500) + std::abs(sy - 1000 * desY - 500) < 800) return true;
        return false;
    }
    //Astar算法
    class t_Compare
    {
    public:
        t_Compare() {};
        bool operator()(const Node& node1, const Node& node2) const
        {
            return node1.Fcost > node2.Fcost;
        }
    }compare_by_cost;

    std::vector<std::pair<int32_t, int32_t>>t_Astar(ITrickerAPI& api, int destX, int destY, Node Nodemap[50][50])
    {
        Node nodemap[50][50];
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                nodemap[i][j] = Nodemap[i][j];
            }
        }
        api.Print("t_Astar被调用");
        std::priority_queue<Node, std::vector<Node>, t_Compare> openList;
        auto self = api.GetSelfInfo();
        if (self->x / 1000 == destX && self->y / 1000 == destY)
        {
            std::vector<std::pair<int, int>> path{ std::pair<int, int>(destX,destY) };
            return path;//返回自身坐标是已经到了
        }
        auto& startNode = nodemap[self->x / 1000][self->y / 1000];
        startNode.Gcost = 0;
        startNode.Hcost = GetManhattanDistance(self->x / 1000, self->y / 1000, destX, destY);
        startNode.Fcost = GetManhattanDistance(self->x / 1000, self->y / 1000, destX, destY);
        startNode.flag = 1;
        startNode.Parent = nullptr;
        auto& endNode = nodemap[destX][destY];
        endNode.Gcost = 0; endNode.Hcost = 0, endNode.Fcost = 0; endNode.flag = 0; endNode.Parent = nullptr;
        openList.push(startNode);
        Node* currentNode = &startNode;
        while (!openList.empty() && !(currentNode->x == destX && currentNode->y == destY))
        {
            currentNode->flag = 2;
            openList.pop();
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    if (!(i * j) && !(i == 0 && j == 0))
                    {
                        if (isInMap(currentNode->x + i, currentNode->y + j))
                        {
                            Node* logicNode = &nodemap[currentNode->x + i][currentNode->y + j];
                            if (logicNode->x == destX && logicNode->y == destY)
                            {
                                logicNode->flag = 2;
                                logicNode->Parent = currentNode;
                                currentNode = logicNode;
                                goto here;
                            }
                            else if (!isWall((*logicNode).x, (*logicNode).y) && logicNode->flag == 0)
                            {
                                logicNode->Gcost = currentNode->Gcost + 1;
                                logicNode->flag = 1;
                                logicNode->Parent = currentNode;
                                logicNode->Hcost = GetManhattanDistance(logicNode->x, logicNode->y, destX, destY);
                                logicNode->Fcost = logicNode->Gcost + logicNode->Hcost;
                                openList.push(*logicNode);
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
            if (!openList.empty())
            {
                currentNode = &nodemap[openList.top().x][openList.top().y];
            }
        }
    here:
        if (currentNode->x == self->x / 1000 && currentNode->y == self->y / 1000)
        {
            std::vector<std::pair<int, int>> path;
            api.Print("到不了，返回空路径");
            return path;//返回空路径
        }
        auto path = GetPath(currentNode);
        api.Print(fmt::format("Astar返回值：path.size()={}", path.size()));
        return path;
    }

    int32_t GetManhattanDistance(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
    {
        return abs(x1 - x2) + abs(y1 - y2);
    }
    std::vector<std::pair<int, int>> GetPath(Node* endNode)
    {
        std::unique_ptr<std::vector<std::pair<int, int>>> path(new std::vector<std::pair<int, int>>);
        Node* node = endNode;
        while (node->Parent)
        {
            path->emplace_back(node->x, node->y);
            node = node->Parent;
        }
        path->emplace_back(node->x, node->y); // 添加起点
        std::reverse(path->begin(), path->end()); // 路径反转
        return *path;
    }
    bool isWall(int x, int y)
    {
        return Map.at(x).at(y) == THUAI6::PlaceType::Wall || Map.at(x).at(y) == THUAI6::PlaceType::ClassRoom;
    }
    bool isInMap(int x, int y)
    {
        return 0 <= x && x < 50 && 0 <= y && y < 50;
    }
    /*bool MoveToCertainPlace();*/
    bool t_ToNear(ITrickerAPI& api, std::vector<std::pair<int32_t, int32_t>>set)
    {
        if (set.empty())return false;//空集或者去不了
        int size = set.size();
        int flag = -1;
        int path_len = 50 * 50;
        for (int i = 0; i < size; i++)
        {
            auto cur_path = t_Astar(api, set[i].first, set[i].second, NodeMap);
            if (cur_path.empty())continue;
            if (cur_path.size() < path_len)
            {
                flag = i;
            }
        }
        if (flag == -1)
        {
            return false;
        }
        else
        {
            auto path = t_Astar(api, set[flag].first, set[flag].second, NodeMap);
            if (path.size() > 1)
            {
                t_moveinto(api, path[1].first, path[1].second);
                api.Print("path>1");
                return true;
            }
            else
            {
                t_moveinto(api, path[0].first, path[0].second);
                api.Print("path=1");
                return true;
            }
        }
    }
}mtf;

std::array<int, 2> t_find(ITrickerAPI& api, THUAI6::PlaceType type)
{
    int cellx, celly;
    cellx = (api.GetSelfInfo()->x) / 1000; celly = (api.GetSelfInfo()->y) / 1000;
    array<int, 2> start = { cellx,celly };
    queue<array<int, 2>> q;
    bool** visited = new bool* [50];
    for (int i = 0; i < 50; i++)
    {
        visited[i] = new bool[50];
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            visited[i][j] = { false };
        }
    }
    int direction[4][2] = { {0,1},{0,-1},{1,0},{-1,0} };
    q.push(start);
    visited[cellx][celly] = true;
    while (!q.empty())
    {
        array<int, 2> p = q.front();
        q.pop();
        for (int i = 0; i < 4; i++) {
            int nx = p[0] + direction[i][0];
            int ny = p[1] + direction[i][1];
            array<int, 2> current = { nx, ny };
            if (nx >= 0 && nx < 50 && ny >= 0 && ny < 50 && visited[nx][ny] == false)
            {
                if (api.GetPlaceType(nx, ny) == type)
                {
                    switch (type)
                    {
                    case THUAI6::PlaceType::NullPlaceType:
                        break;
                    case THUAI6::PlaceType::Land:
                        break;
                    case THUAI6::PlaceType::Wall:
                        break;
                    case THUAI6::PlaceType::Grass:
                        break;
                    case THUAI6::PlaceType::ClassRoom:
                    {
                        int j = 0;
                        for (j = 0; j < (int)ClassroomPosition.size(); j++)
                        {
                            if (ClassroomPosition[j].first == nx && ClassroomPosition[j].second == ny)
                            {
                                break;
                            }
                        }
                        if ((api.GetClassroomProgress(nx, ny) < 10000000 || api.GetClassroomProgress(nx, ny) == -1) && j != (int)ClassroomPosition.size()) { return current; }
                    }
                    break;
                    case THUAI6::PlaceType::Gate:
                        break;
                    case THUAI6::PlaceType::HiddenGate:
                        break;
                    case THUAI6::PlaceType::Window:
                        break;
                    case THUAI6::PlaceType::Door3:
                        break;
                    case THUAI6::PlaceType::Door5:
                        break;
                    case THUAI6::PlaceType::Door6:
                        break;
                    case THUAI6::PlaceType::Chest:
                        break;
                    default:
                        break;
                    }
                }
                q.push(current);
                visited[nx][ny] = true;
            }
        }
    }
    for (int i = 0; i < 50; i++) {
        delete[] visited[i];
    }
    delete[] visited;
    return array<int, 2>{-1, -1};
}
std::array<int, 2> t_hw = { 0,0 };
std::array<int, 2> t_lo = { 0,0 };


void t_delete_classroomposition(int i)
{
    ClassroomPosition.erase(ClassroomPosition.begin() + i);
}
void FindWork(ITrickerAPI& api)
{
    t_lo = { api.GetSelfInfo()->x ,api.GetSelfInfo()->y };
    int i = 0;
    if ((int)ClassroomPosition.size() != 0)
    {
        for (i = 0; i < (int)ClassroomPosition.size(); i++)
        {
            if (t_hw[0] == ClassroomPosition[i].first && t_hw[1] == ClassroomPosition[i].second)
            {
                break;
            }
        }
        if (t_hw == t_find(api, THUAI6::PlaceType::ClassRoom) || (t_hw[0] == 0 && t_hw[1] == 0) || i == (int)ClassroomPosition.size())
        {
            t_hw = t_find(api, THUAI6::PlaceType::ClassRoom);
        }
        auto work_road = mtf.t_Astar(api, t_hw[0], t_hw[1], NodeMap);
        mtf.t_moveinto(api, work_road[1].first, work_road[1].second);
    }
}
void AttackWork(ITrickerAPI& api, int t_x1, int t_y1, int h_x2, int h_y2, double ts, double dist)
{
    ts = atan2((h_y2 - t_y1), (h_x2 - t_x1));
    dist = sqrt(pow(t_x1 - h_x2, 2) + pow(t_y1 - h_y2, 2));
    if (((t_x1 < h_x2) && (t_y1 < h_y2)) || ((t_x1 < h_x2) && (t_y1 > h_y2)) || ((t_x1 > h_x2) && (t_y1 < h_y2)) || ((t_x1 > h_x2) && (t_y1 > h_y2)))
    {
        api.Attack(ts);
    }
    else if ((t_x1 == h_x2) && (t_y1 < h_y2))
    {
        api.Attack(3 * PI / 2);
    }
    else if ((t_x1 < h_x2) && (t_y1 == h_y2))
    {
        api.Attack(PI);
    }
    else if ((t_x1 > h_x2) && (t_y1 == h_y2))
    {
        api.Attack(0);
    }
    else if ((t_x1 == h_x2) && (t_y1 > h_y2))
    {
        api.Attack(PI / 2);
    }
}
void AI::play(ITrickerAPI& api)
{
    if (!ifchecked)
    {
        api.Print("check map!");
        Map.resize(50 * 50);
        Map = api.GetFullMap();
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                switch (api.GetPlaceType(i, j))
                {
                case(THUAI6::PlaceType::NullPlaceType):;
                case(THUAI6::PlaceType::Land):;
                case(THUAI6::PlaceType::Grass):NodeMap[i][j] = { i,j,0,0,0,0,nullptr };; break;
                case(THUAI6::PlaceType::Chest):Chest.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door3):Door3Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door5):Door5Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Door6):Door6Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Gate):Gate.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::ClassRoom):ClassroomPosition.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                }
            }
        }//地图
        ifchecked = true;
    }
    if ((int)ClassroomPosition.size() == 0)
    {
        ifchecked = false;
    }
    else
    {
        for (int i = 0; i < (int)ClassroomPosition.size(); ++i)
        {
            if (abs(api.GetSelfInfo()->x / 1000 - ClassroomPosition[i].first) <= 2 && abs(api.GetSelfInfo()->y / 1000 - ClassroomPosition[i].second) <= 2)
            {
                t_delete_classroomposition(i);
            }
        }
    }
    bool if_attack = false;//是否追击
    FindWork(api);
    auto students = api.GetStudents();
    int i;
    int StraightAStudent_Number = 0;
    int Sunshine_Number = 0;
    int Athlete_Number = 0;
    int addicted_students_number = 0;
    int k = 0;
    if ((int)students.size() == 0)
    {
        FindWork(api);
        int t_x = api.GetSelfInfo()->x;
        int t_y = api.GetSelfInfo()->y;
        int h_x = t_hw[0];
        int h_y = t_hw[1];
        double twangle = atan2((h_y - t_y), (h_x - t_x));
        double twdistance = sqrt(pow(t_x - h_x, 2) + pow(t_y - h_y, 2));
        if (twdistance <= 2200 && api.GetClassroomProgress(h_x, h_y) > 0 && api.GetClassroomProgress(h_x, h_y) < 10000000)
            AttackWork(api, t_x, t_y, h_x, h_y, twangle, twdistance);
    }
    else if ((int)students.size() != 0)
    {
        for (i = 0; i < (int)students.size(); i++)
        {
            switch (students[i]->studentType)
            {
            case THUAI6::StudentType::StraightAStudent:
            {
                StraightAStudent_Number++;
                break;
            }
            case THUAI6::StudentType::Sunshine:
            {
                Sunshine_Number++;
                break;
            }
            case THUAI6::StudentType::Athlete:
            {
                Athlete_Number++;
                break;
            }
            }
        }//优先级：学霸>开心果>体育生>老师
        if (StraightAStudent_Number != 0)
        {
            for (i = 0; i < (int)students.size(); i++)
            {
                if (students[i]->studentType == THUAI6::StudentType::StraightAStudent && students[i]->playerState != THUAI6::PlayerState::Addicted)
                    break;
            }
            if (i != (int)students.size())
            {
                if_attack = true;
                auto students_i_road = mtf.t_Astar(api, students[i]->x / 1000, students[i]->y / 1000, NodeMap);
                mtf.t_moveinto(api, students_i_road[1].first, students_i_road[1].second);
            }
        }
        else if (StraightAStudent_Number == 0 && Sunshine_Number != 0)
        {
            for (i = 0; i < (int)students.size(); i++)
            {
                if (students[i]->studentType == THUAI6::StudentType::Sunshine && students[i]->playerState != THUAI6::PlayerState::Addicted)
                    break;
            }
            if (i != (int)students.size())
            {
                if_attack = true;
                auto students_i_road = mtf.t_Astar(api, students[i]->x / 1000, students[i]->y / 1000, NodeMap);
                mtf.t_moveinto(api, students_i_road[1].first, students_i_road[1].second);
            }
        }
        else if (StraightAStudent_Number == 0 && Sunshine_Number == 0 && Athlete_Number != 0)
        {
            for (i = 0; i < (int)students.size(); i++)
            {
                if (students[i]->studentType == THUAI6::StudentType::Athlete && students[i]->playerState != THUAI6::PlayerState::Addicted)
                    break;
            }
            if (i != (int)students.size())
            {
                if_attack = true;
                auto students_i_road = mtf.t_Astar(api, students[i]->x / 1000, students[i]->y / 1000, NodeMap);
                mtf.t_moveinto(api, students_i_road[1].first, students_i_road[1].second);
            }
        }
        else if (StraightAStudent_Number == 0 && Sunshine_Number == 0 && Athlete_Number == 0)
        {
            if_attack = false;
        }
        if (if_attack)
        {
            int x1 = api.GetSelfInfo()->x; int y1 = api.GetSelfInfo()->y;
            auto x2 = students[i]->x;
            auto y2 = students[i]->y;
            double s = atan2((y2 - y1), (x2 - x1));
            double dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
            if (dist <= 8000)
            {
                api.UseSkill(0);

                if (dist <= 1700)
                {
                    if (((x1 < x2) && (y1 < y2)) || ((x1 < x2) && (y1 > y2)) || ((x1 > x2) && (y1 < y2)) || ((x1 > x2) && (y1 > y2)))
                        api.Attack(s);

                    else if ((x1 == x2) && (y1 < y2))
                    {
                        api.Attack(3 * PI / 2);
                    }
                    else if ((x1 < x2) && (y1 == y2))
                    {
                        api.Attack(PI);
                    }
                    else if ((x1 > x2) && (y1 == y2))
                    {
                        api.Attack(0);
                    }
                    else if ((x1 == x2) && (y1 > y2))
                    {
                        api.Attack(PI / 2);
                    }
                }
            }
        }
    }
}
