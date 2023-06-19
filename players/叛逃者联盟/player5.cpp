#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <chrono>
#include<sstream>
#include<numeric>
#include<strstream>
using namespace std;
// 全局变量区
bool ifchecked = false;
std::vector<std::pair<int32_t, int32_t>> Windows;
std::vector<std::vector<THUAI6::PlaceType>> Map;//全局地图
std::vector<std::pair<int32_t, int32_t>> Door3Position;// 3门的位置（x, y）
std::vector<std::pair<int32_t, int32_t>> Door5Position; // 5门的位置
std::vector<std::pair<int32_t, int32_t>> Door6Position; // 6门的位置
std::vector<std::pair<int32_t, int32_t>> ClassroomPosition;//教室的位置
std::vector<std::pair<int32_t, int32_t>> Chest;//箱子的位置
std::vector<std::pair<int32_t, int32_t>> Gate;//校门
std::vector<std::pair<int32_t, int32_t>> GrassPos;//草地
std::vector<int32_t>LearningProcess;//学习进度，如果学到100LearningProcess变成1
int Grass[50][50] = { 0 };
int iffind = 1;

vector<pair<int, int>>StuPosition(4,pair<int,int>(0,0));
class TrickersInfo//记录捣蛋鬼位置/时间，以后改进算法用
{
public:
    TrickersInfo(int X = 0, int Y = 0, int Time = 0)
    {
        x = X;
        y = Y;
        time = Time;
    }
    void reset(int X = 0, int Y = 0, int Time = 0)
    {
        x = X;
        y = Y;
        time = Time;
    }
    int x;
    int y;
    int time;//帧数
}TrickerInfo;
struct Node
{
    int x, y;
    int Gcost, Hcost, Fcost;//0 1 2 空地，openlist，closelist
    int flag;//初值为0，墙是2
    struct Node* Parent;
};
Node NodeMap[50][50] = { 0,0,0,0,0,0,nullptr};
int Target[50][50] = { 0 };//需要进行初始化//后来可能用不到
int obstacleMap[50][50] = { 0 };//障碍物，包括箱子、教室和墙
int RunAwayCoolTime = 0;//冷却用的
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
        if (!isInsideofcell(api, cellx, celly)) 
        {
            if (isLeftoutofcell(api, cellx, celly)) api.MoveRight(50);
            else if (isRightoutofcell(api, cellx, celly))   api.MoveLeft(50);
            if (isUpoutofcell(api, cellx, celly))   api.MoveDown(50);
            else if (isDownoutofcell(api, cellx, celly))    api.MoveUp(50);
        }

        /*auto self = api.GetSelfInfo();
        auto sx = self->x;
        auto sy = self->y;
        int destx = api.CellToGrid(cellx);
        int desty = api.CellToGrid(celly);
        api.Print(fmt::format("({},{})->({},{})", sx, sy, destx, desty));
        int delta_x = destx - sx;
        int delta_y = desty - sy;
        auto angle = atan2(delta_y, delta_x);
        api.Move(20, angle + std::rand() % 10 / 10);
        return;*/
    }
    //使人的碰撞箱进入一整格内，避免卡死等问题:只用于人在该格子附近的微调!

    //ArriveAt函数没用过
    bool ArriveAt(IStudentAPI& api, int32_t desX, int32_t desY)
    {
        auto self = api.GetSelfInfo();
        int32_t sx = self->x;
        int32_t sy = self->y;
        auto cellx = api.GridToCell(sx);
        auto celly = api.GridToCell(sy);
        if (cellx == desX && celly == desY) return true;
        else return false;
    }//到达grid

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

        for (int indexnow = 0; indexnow < 4; indexnow++)
        {
            if (api.GetStudents()[indexnow]->playerID != api.GetSelfInfo()->playerID)
            {
                int ox = api.GetStudents()[indexnow]->x, oy = api.GetStudents()[indexnow]->y;
                int otherx = api.GridToCell(ox), othery=api.GridToCell(oy);
                /*nodemap[otherx][othery].flag = 2;*/
                /*nodemap[otherx + 1][othery].flag = 2;
                nodemap[otherx][othery+1].flag = 2;
                nodemap[otherx - 1][othery].flag = 2;
                nodemap[otherx][othery - 1].flag = 2;*/
                if (isInsideofcell(api, otherx, othery))
                {
                    nodemap[otherx][othery].flag = 2;
                }
                else
                {
                    if (ox > api.CellToGrid(otherx))
                    {
                        if (oy > api.CellToGrid(othery))
                        {
                            nodemap[otherx][othery].flag = 2;
                            nodemap[otherx+1][othery].flag = 2;
                            nodemap[otherx][othery+1].flag = 2;
                            nodemap[otherx - 1][othery].flag = 2;
                            nodemap[otherx][othery - 1].flag = 2;
                            nodemap[otherx+1][othery+1].flag = 2;
                        }
                        else//oy <= api.CellToGrid(othery)
                        {
                            nodemap[otherx][othery].flag = 2;
                            nodemap[otherx + 1][othery].flag = 2;
                            nodemap[otherx][othery - 1].flag = 2;
                            nodemap[otherx - 1][othery].flag = 2;
                            nodemap[otherx][othery + 1].flag = 2;
                            nodemap[otherx + 1][othery - 1].flag = 2;
                        }
                    }
                    else
                    {
                        if (oy > api.CellToGrid(othery))
                       {
                            nodemap[otherx][othery].flag = 2;
                            nodemap[otherx][othery + 1].flag = 2;
                            nodemap[otherx - 1][othery].flag = 2;
                            nodemap[otherx + 1][othery].flag = 2;
                            nodemap[otherx][othery - 1].flag = 2;
                            nodemap[otherx - 1][othery + 1].flag = 2;
                        }
                        else//oy <= api.CellToGrid(othery)
                        {
                            nodemap[otherx][othery].flag = 2;
                            nodemap[otherx - 1][othery].flag = 2;
                            nodemap[otherx][othery - 1].flag = 2;
                            nodemap[otherx][othery+1].flag = 2;
                            nodemap[otherx+1][othery].flag = 2;
                            nodemap[otherx - 1][othery - 1].flag = 2;
                        }
                    }
                }
            }
        }
        auto trickers = api.GetTrickers();
        if (!trickers.empty())
        {
            auto tcellx = trickers[0]->x;
            auto tcelly = trickers[0]->y;
            nodemap[api.GridToCell(tcellx)][api.GridToCell(tcelly)].flag = 2;
        }
        api.Print("Astar被调用");
        std::priority_queue<Node, std::vector<Node>, Compare> openList;
        auto self = api.GetSelfInfo();
        nodemap[api.GridToCell(self->x)][api.GridToCell(self->y)].flag=0;
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
                    if (!(i * j) && !(i == 0 && j == 0))//代表不是梅花四格和中心格
                    {
                        if (isInMap(currentNode->x + i, currentNode->y + j))
                        {
                            Node* logicNode = &nodemap[currentNode->x + i][currentNode->y + j];
                            if (logicNode->x == destX && logicNode->y == destY)
                            {
                                logicNode->flag = 2;
                                logicNode->Parent = currentNode;
                                currentNode = logicNode;//备注:
                                goto here;
                            }
                            else if ( logicNode->flag == 0)
                            {
                                logicNode->Gcost = currentNode->Gcost + 1;
                                logicNode->flag = 1;//flag=1:加入到openlist
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
        std::vector<std::pair<int, int>> path;
        Node* node = endNode;
        while (node->Parent)
        {
            path.push_back(std::pair<int, int>(node->x, node->y));
            node = node->Parent;
        }
        path.push_back(std::pair<int, int>(node->x, node->y)); // 添加起点
        std::reverse(path.begin(), path.end()); // 路径反转
        return path;
    }
    bool isWall(int x, int y)
    {
        return (Map.at(x).at(y) == THUAI6::PlaceType::Wall || Map.at(x).at(y) == THUAI6::PlaceType::ClassRoom);
    }
    bool isInMap(int x, int y)
    {
        return 0 <= x && x < 50 && 0 <= y && y < 50;
    }
    //前往该集合最近点，适用于点较少，2、3个左右

    //ToNear暂时没有用到
    bool ToNear(IStudentAPI& api, std::vector<std::pair<int32_t, int32_t>>set)
    {
        if (set.empty())return false;//空集或者去不了
        int size = set.size();
        int flag = -1;
        int path_len = 50 * 50;
        std::vector<std::pair<int32_t, int32_t>> path;
        auto self = api.GetSelfInfo();
        for (int i = 0; i < size; i++)
        {
            if (GetManhattanDistance(set[i].first, set[i].second,api.GridToCell(self->x),api.GridToCell(self->y)>10))continue;
            auto cur_path = Astar(api, set[i].first, set[i].second, NodeMap);
            if (cur_path.empty())
            {
                api.Print("当前路径为空？");
                continue;
            }
            else api.Print(fmt::format("{}", cur_path.size()));
            if (cur_path.size() < path_len && cur_path.size() != 0)
            {
                path_len = cur_path.size();
                path = cur_path;
                flag = i;
            }
        }
        if (flag == -1)
        {
            api.Print("无法到达");
            return false;
        }
        else
        {
            api.Print(fmt::format("{}", path.size()));
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

    //焦神算法，寻找当前最近目标
    //寻找最近的目标点
    bool Adjacent(pair<int, int>point1, pair<int, int>point2)
    {
        int x = abs(point1.first - point2.first);
        int y = abs(point1.second - point2.second);
        return (x + y == 1);
    }//是否是四周的格

    //尚未考虑把学生当做障碍物，但是应该不影响后续操作
    pair<int, int> FindNearestPoint(int x, int y, int target[50][50])//实际还得一个参量：issTudent
    {
        int Accept[50][50] = { 0 };
        pair<int, int>StartPoint = { x,y };//举例子用

        //初始化PointList
        vector<vector<pair<int, int>>>PointList(400, vector<pair<int, int>>(400, { 99,99 }));

        int distance = 0;
        PointList[0][0] = StartPoint;
        Accept[StartPoint.first][StartPoint.second] = 1;//起点已经加入
        int flag = 1;//循环条件
        pair<int, int>DestPoint;//寻找终点，{0,0}只是初始化，无意义

        /*Print(PointList[0][0]);*/
        while (flag)//对第一层循环
        {
            int temp = distance;
            distance++;
            int stop = 0;
            int cnt = 0;
            for (int i = 0; i < 400 && !stop; i++)//对第二层循环
            {
                if (PointList[temp][i].first == 99)
                {
                    stop = 1;
                    break;
                }
                else
                {
                    pair<int, int>tempPoint = PointList[temp][i];
                    int x = tempPoint.first, y = tempPoint.second;
                    for (int deltax = -1; deltax <= 1; deltax++)
                    {
                        for (int deltay = -1; deltay <= 1; deltay++)
                        {
                            pair<int, int>tempPoint2(x + deltax, y + deltay);
                            if (Adjacent(tempPoint2, tempPoint))
                            {
                                int x2 = tempPoint2.first, y2 = tempPoint2.second;
                                if (target[tempPoint2.first][tempPoint2.second] == 1)
                                {
                                    flag = 0;
                                    DestPoint = tempPoint2;
                                    break;
                                }
                                else if ((0 <= x2 && x2 < 50 && 0 <= y2 && y2 < 50 && obstacleMap[x2][y2] == 0 && Accept[x2][y2] == 0))
                                {
                                    Accept[tempPoint2.first][tempPoint2.second] = 1;
                                    PointList[distance][cnt] = tempPoint2;
                                    cnt++;
                                }
                            }
                            if (!flag)	break;
                        }
                        if (!flag)	break;
                    }
                }
            }
        }
        return DestPoint;
    }

    //获取当前状态下的技能冷却时间，返回一个一维数组指针
    double* skillTime(IStudentAPI& api)
    {
        auto student = api.GetSelfInfo();
        string string_skillTime[3] = { "" };
        for (const auto& time : student->timeUntilSkillAvailable)
        {
            int i = 0;
            string_skillTime[i] += std::to_string(time);
            i++;
        }
        double* double_skillTime = new double[3];
        double* point = double_skillTime;
        stringstream ss;
        for (int i = 0; i < 3; i++)
        {
            ss << string_skillTime[i];
            ss >> double_skillTime[i];
        }
        return(double_skillTime);
    }

    //检测是否有捣蛋鬼靠近，是否大于危险阈值，目前设置的3
    bool IfDanger(IStudentAPI& api)
    {
        auto self = api.GetSelfInfo();
        auto alert = self->dangerAlert;
        auto trickers = api.GetTrickers();
        if (!trickers.empty())return true;
        else
        {
            if (alert > 4)return true;
            else return false;
        }
    }

    void GoToClassAndLearning(IStudentAPI& api)
    {
        int x = api.GridToCell(api.GetSelfInfo()->x), y = api.GridToCell(api.GetSelfInfo()->y);
        int target[50][50] = { 0 };
        for (int i = 0; i < 10; i++)
        {
            if (LearningProcess[i] == 0)   target[ClassroomPosition[i].first][ClassroomPosition[i].second] = 1;
        }
        pair<int, int>NearestClassRoom = FindNearestPoint(x, y, target);
        int index = (find(ClassroomPosition.begin(), ClassroomPosition.end(), pair<int, int>(NearestClassRoom.first, NearestClassRoom.second)) - ClassroomPosition.begin());
        auto path = Astar(api, NearestClassRoom.first, NearestClassRoom.second, NodeMap);
        int learning = 0;
        while (path.size() >= 2 && !learning)
        {
            if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
            {
                moveinto(api, path[1].first, path[1].second);
                if (ArriveAt(api, path[1].first, path[1].second))
                {
                    path.erase(path.begin());
                }
            }
            else//下一步就是窗户了
            {
                api.SkipWindow();
                path.erase(path.begin());
                path.erase(path.begin());
            }
            if (path.size() <= 2)
            {
                learning = 1;
                break;
            }
        }

        if (learning)
        {
            api.StartLearning();
        }


        //*****************************Try
        if (api.GetClassroomProgress(NearestClassRoom.first, NearestClassRoom.second) >= 10000000)
        {
            for (int indexnow = 0; indexnow < 4; indexnow++)
            {
                if (api.GetStudents()[indexnow]->playerID != myId(api))
                {
                    api.SendMessage(indexnow, fmt::format("{}", index));
                }
            }
            LearningProcess[index] = 1;
            api.EndAllAction();
        }
        //*****************************Try
    }

    int myId(IStudentAPI& api)
    {
        return api.GetSelfInfo()->playerID;
    }

    bool OpenTheGate()
    {
        return(accumulate(LearningProcess.begin(), LearningProcess.end(), 0) >= 7);
    }

    bool OpenTheHiddenGate()
    {
        return(accumulate(LearningProcess.begin(), LearningProcess.end(), 0) >= 3);
    }

    void GoToGateAndGraduate(IStudentAPI& api)
    {
        if (!(OpenTheGate()))    return;
        else
        {
            int x = api.GridToCell(api.GetSelfInfo()->x), y = api.GridToCell(api.GetSelfInfo()->y);
            int target[50][50] = { 0 };
            for (int i = 0; i < 2; i++)
            {
                target[Gate[i].first][Gate[i].second] = 1;
            }
            pair<int, int>NearestGate = FindNearestPoint(x, y, target);
            auto path = Astar(api, NearestGate.first, NearestGate.second, NodeMap);
            int Graduate = 0;
            while (path.size() >= 2 && !Graduate)
            {
                if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
                {
                    moveinto(api, path[1].first, path[1].second);
                    if (ArriveAt(api, path[1].first, path[1].second))
                    {
                        path.erase(path.begin());
                    }
                }
                else//下一步就是窗户了
                {
                    api.SkipWindow();
                    path.erase(path.begin());
                    path.erase(path.begin());
                }
                if (path.size() <= 2)
                {
                    Graduate = 1;
                    break;
                }
            }
            if (Graduate)
            {
                api.Graduate();
                api.StartOpenGate();
            }
        }
    }

    void MoveToThePlace(IStudentAPI& api, pair<int, int>Terminal)
    {
        int x = api.GridToCell(api.GetSelfInfo()->x), y = api.GridToCell(api.GetSelfInfo()->y);
        auto path = Astar(api, Terminal.first, Terminal.second, NodeMap);
        while (path.size() >= 2)
        {
            if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
            {
                moveinto(api, path[1].first, path[1].second);
                if (isInsideofcell(api, path[1].first, path[1].second))
                {
                    path.erase(path.begin());
                }
            }
            else
            {
                api.SkipWindow();
                path.erase(path.begin());
                path.erase(path.begin());
                /*api.EndAllAction();*/
            }
            if (path.size() <= 2)
            {
                moveinto(api, path[1].first, path[1].second);
                /*api.EndAllAction();*/
                break;
            }
            else if (path.size() == 1)
            {
                moveinto(api, path[0].first, path[0].second);
            }
            else break;
        }
    }
    bool ToNearestGrass(IStudentAPI& api)
    {
        auto self = api.GetSelfInfo();
        int xcell = api.GridToCell(self->x);
        int ycell = api.GridToCell(self->y);
        int GrassChecked[50][50] = { 0 };
        auto students = api.GetStudents();
        for (int i = 0; i < 49; i++)
        {
            for (int j = 0; j < 49; j++)
            {
                GrassChecked[i][j] = Grass[i][j];
            }
        }
        for (auto stu : students)
        {
            if (stu->playerID == self->playerID)continue;
            else GrassChecked[api.GridToCell(stu->x)][api.GridToCell(stu->y)] = 0;
        }
        auto target = FindNearestPoint(xcell, ycell, Grass);
        if (target.second==0 && target.first==0)
        {
            return false;
        }
        api.Print("去草丛");
        auto path = Astar(api, target.first, target.second, NodeMap);
        while (path.size() >= 2)
        {
            if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
            {
                moveinto(api, path[1].first, path[1].second);
                if (ArriveAt(api, path[1].first, path[1].second))
                {
                    path.erase(path.begin());
                }
            }
            else//下一步就是窗户了
            {
                api.SkipWindow();
                path.erase(path.begin());
                path.erase(path.begin());
            }
        }
        return true;
    }
    bool RunAway(IStudentAPI& api)
    {
        auto self = api.GetSelfInfo();
        auto trickers = api.GetTrickers();
        if (trickers.empty())//看不见，盲去草丛
        {
            api.Print("runaway!");
            ToNearestGrass(api);
            return true;
        }
        else//看的见捣蛋鬼，更新捣蛋鬼信息，建议astar增加一个参数obstacles，用vector储存当前别去的地方
        {
            auto tx = trickers[0]->x;
            auto ty = trickers[0]->y;
            TrickerInfo.x = tx;
            TrickerInfo.y = ty;
            TrickerInfo.time = api.GetFrameCount();
            api.Print("runaway!");
            ToNearestGrass(api);
            return true;
        }
    }
    //检测是否有墙
    bool IfWall(IStudentAPI& api, int x1, int y1, int x2, int y2)//默认输入的是实际坐标（*1000了的那个）
    {
        int deltaX = x1 - x2;
        int deltaY = y1 - y2;
        int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
        if (divide == 0)
        {
            return isWall((int)x1 / 1000, (int)y1 / 1000);
        }
        else
        {
            double dx = deltaX / divide;
            double dy = deltaY / divide;
            double myX = double(x2);
            double myY = double(y2);
            for (int i = 0; i < divide; i++)
            {
                myX += dx;
                myY += dy;
                if (isWall(myX, myY))
                    return true;
            }
            return false;
        }
    }

    //获取BGM警戒信息，返回一个int值（警戒半径/二者距离）
    int getAlert(IStudentAPI& api)
    {
        auto self = api.GetSelfInfo();
        int alertData = self->dangerAlert;
        return alertData;
    }

    void StraightAStudentSkill(IStudentAPI& api)
    {
        int x = api.GridToCell(api.GetSelfInfo()->x), y = api.GridToCell(api.GetSelfInfo()->y);
        int target[50][50] = { 0 };
        for (int i = 0; i < 10; i++)
        {
            if (LearningProcess[i] == 0)   target[ClassroomPosition[i].first][ClassroomPosition[i].second] = 1;
        }
        pair<int, int>NearestClassRoom = FindNearestPoint(x, y, target);
        int index = (find(ClassroomPosition.begin(), ClassroomPosition.end(), pair<int, int>(NearestClassRoom.first, NearestClassRoom.second)) - ClassroomPosition.begin());
        auto path = Astar(api, NearestClassRoom.first, NearestClassRoom.second, NodeMap);
        double* skilltime = skillTime(api);
        if ((path.size() <= 2) && (!skilltime[0]))
            api.UseSkill(0);
    }

};

class MyTrickerFunction
{
public:
    //移动函数
    bool isLeftoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y - 400 < celly * 1000) return true;
        else return false;
    }//判断是否在目标格左边一定范围
    bool isRightoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->y + 400 > (celly + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格右边一定范围
    bool isUpoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x - 400 < cellx * 1000) return true;
        else return false;
    }//判断是否在目标格上边一定范围
    bool isDownoutofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (api.GetSelfInfo()->x + 400 > (cellx + 1) * 1000) return true;
        else return false;
    }//判断是否在目标格下边一定范围
    bool isInsideofcellx(ITrickerAPI& api, int32_t cellx) {
        if (!isUpoutofcell(api, cellx, -1) && !isDownoutofcell(api, cellx, -1)) return true;
        else return false;
    }
    //判断人是否整个在某行内
    bool isInsideofcelly(ITrickerAPI& api, int32_t celly) {
        if (!isLeftoutofcell(api, -1, celly) && !isRightoutofcell(api, -1, celly)) return true;
        else return false;
    }
    //判断人是否整个在某列内
    bool isInsideofcell(ITrickerAPI& api, int32_t cellx, int32_t celly) {
        if (isInsideofcellx(api, cellx) && isInsideofcelly(api, celly)) return true;
        else return false;
    }
    //判断人是否整个在某格子内
    void moveinto(ITrickerAPI& api, int32_t cellx, int32_t celly)
    {
        if (!isInsideofcell(api, cellx, celly)) {
            if (isLeftoutofcell(api, cellx, celly)) api.MoveRight(50);
            else if (isRightoutofcell(api, cellx, celly)) api.MoveLeft(50);
            if (isUpoutofcell(api, cellx, celly)) api.MoveDown(50);
            else if (isDownoutofcell(api, cellx, celly))
                api.MoveUp(50);
        }
    }
    //使人的碰撞箱进入一整格内，避免卡死等问题:只用于人在该格子附近的微调!
    bool ArriveAt(ITrickerAPI& api, int32_t desX, int32_t desY)
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
    std::vector<std::pair<int32_t, int32_t>>Astar(ITrickerAPI& api, int destX, int destY, Node Nodemap[50][50])
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
        std::vector<std::pair<int, int>> path;
        Node* node = endNode;
        while (node->Parent)
        {
            path.push_back(std::pair<int, int>(node->x, node->y));
            node = node->Parent;
        }
        path.push_back(std::pair<int, int>(node->x, node->y)); // 添加起点
        std::reverse(path.begin(), path.end()); // 路径反转
        return path;
    }
    bool isWall(int x, int y)
    {
        return Map.at(x).at(y) == THUAI6::PlaceType::Wall;
    }
    bool isInMap(int x, int y)
    {
        return 0 <= x && x < 50 && 0 <= y && y < 50;
    }
    //前往该集合最近点，适用于点较少，2、3个左右
    bool ToNear(ITrickerAPI& api, std::vector<std::pair<int32_t, int32_t>>set)
    {
        if (set.empty())return false;//空集或者去不了
        int size = set.size();
        int flag = -1;
        int path_len = 50 * 50;
        std::vector<std::pair<int32_t, int32_t>> path;
        for (int i = 0; i < size; i++)
        {
            auto cur_path = Astar(api, set[i].first, set[i].second, NodeMap);
            if (cur_path.empty())
            {
                api.Print("当前路径为空？");
                continue;
            }
            else api.Print(fmt::format("{}", cur_path.size()));
            if (cur_path.size() < path_len && cur_path.size() != 0)
            {
                path_len = cur_path.size();
                path = cur_path;
                flag = i;
            }
        }
        if (flag == -1)
        {
            api.Print("无法到达");
            return false;
        }
        else
        {
            api.Print(fmt::format("{}", path.size()));
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
    //焦神算法，寻找当前最近目标
    //寻找最近的目标点
    bool Adjacent(pair<int, int>point1, pair<int, int>point2)
    {
        int x = abs(point1.first - point2.first);
        int y = abs(point1.second - point2.second);
        return (x + y == 1);
    }//是否是四周的格
    pair<int, int> FindNearestPoint(int x, int y, int target[50][50])//实际还得一个参量：issTudent
    {
        int Accept[50][50] = { 0 };
        pair<int, int>StartPoint = { x,y };//举例子用

        //初始化PointList
        vector<vector<pair<int, int>>>PointList(400, vector<pair<int, int>>(400, { 99,99 }));

        int distance = 0;
        PointList[0][0] = StartPoint;
        Accept[StartPoint.first][StartPoint.second] = 1;//起点已经加入
        int flag = 1;//循环条件
        pair<int, int>DestPoint;//寻找终点，{0,0}只是初始化，无意义

        /*Print(PointList[0][0]);*/
        while (flag)//对第一层循环
        {
            int temp = distance;
            distance++;
            int stop = 0;
            int cnt = 0;
            for (int i = 0; i < 400 && !stop; i++)//对第二层循环
            {
                if (PointList[temp][i].first == 99)
                {
                    stop = 1;
                    break;
                }
                else
                {
                    pair<int, int>tempPoint = PointList[temp][i];
                    int x = tempPoint.first, y = tempPoint.second;
                    for (int deltax = -1; deltax <= 1; deltax++)
                    {
                        for (int deltay = -1; deltay <= 1; deltay++)
                        {
                            pair<int, int>tempPoint2(x + deltax, y + deltay);
                            if (Adjacent(tempPoint2, tempPoint))
                            {
                                int x2 = tempPoint2.first, y2 = tempPoint2.second;
                                if (target[tempPoint2.first][tempPoint2.second] == 1)
                                {
                                    flag = 0;
                                    DestPoint = tempPoint2;
                                    break;
                                }
                                else if ((0 <= x2 && x2 < 50 && 0 <= y2 && y2 < 50 && obstacleMap[x2][y2] == 0 && Accept[x2][y2] == 0))
                                {
                                    Accept[tempPoint2.first][tempPoint2.second] = 1;
                                    PointList[distance][cnt] = tempPoint2;
                                    cnt++;
                                }
                            }
                            if (!flag)	break;
                        }
                        if (!flag)	break;
                    }
                }
            }
        }
        return DestPoint;
    }
    //含有翻窗的移动函数
    void trickerMove(ITrickerAPI& api, int x, int y)
    {
        //auto path = myt.Astar(api, x, y, NodeMap);
        //while (path.size() > 2)
        //{
        //    if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
        //    {
        //        myt.moveinto(api, path[1].first, path[1].second);
        //        if (myt.isInsideofcell(api, path[1].first, path[1].second))
        //        {
        //            path.erase(path.begin());
        //        }
        //    }
        //    else
        //    {
        //        api.SkipWindow();
        //        path.erase(path.begin());
        //        path.erase(path.begin());
        //        /*api.EndAllAction();*/
        //    }
        //}
        std::vector<std::pair<int32_t, int32_t>> path;
        auto tempPath = Astar(api, x, y, NodeMap);
        if (tempPath.size() > 2)
        {
            path.push_back({ tempPath[0].first, tempPath[0].second });
            path.push_back({ tempPath[1].first, tempPath[1].second });
            while (!path.empty())
            {
                if (path.size() > 1)
                {
                    if (Map[path[1].first][path[1].second] != THUAI6::PlaceType::Window)
                    {
                        moveinto(api, path[1].first, path[1].second);
                        if (isInsideofcell(api, path[1].first, path[1].second))
                        {
                            path.erase(path.begin());
                        }
                    }
                    else
                    {
                        api.SkipWindow();
                        path.erase(path.begin());
                        path.erase(path.begin());
                        /*api.EndAllAction();*/
                    }
                }
                else
                {
                    while (!path.empty())
                    {
                        moveinto(api, path[0].first, path[0].second);
                        if (isInsideofcell(api, path[0].first, path[0].second))
                        {
                            path.erase(path.begin());
                        }
                    }
                }
            }

        }
    }
    //获取当前状态下的技能冷却时间，返回一个一维数组指针
    double* skillTime(ITrickerAPI& api)
    {
        auto tricker = api.GetSelfInfo();
        string string_skillTime[3] = { "" };
        for (const auto& time : tricker->timeUntilSkillAvailable)
        {
            int i = 0;
            string_skillTime[i] += std::to_string(time);
            i++;
        }
        double* double_skillTime = new double[3];
        double* point = double_skillTime;
        stringstream ss;
        for (int i = 0; i < 3; i++)
        {
            ss << string_skillTime[i];
            ss >> double_skillTime[i];
        }
        return(double_skillTime);
    }
    //找到视野中最近的学生并追击
    void chaseStudents(ITrickerAPI& api)
    {
        api.Print("Chasing!");
        int temp = 1000000;
        bool StudentAddicted = true;
        std::pair<int32_t, int32_t> nearestStudentPositon;
        auto self = api.GetSelfInfo();
        auto students = api.GetStudents();
        //找到最近的学生
        for (int i = 0; i < students.size(); i++)
        {
            //判断不是老师才进行追击
           /* if (students[i]->studentType != THUAI6::StudentType::Teacher)
            {*/
            if (students[i]->playerState != THUAI6::PlayerState::Addicted && ((abs(students[i]->x - self->x) + abs(students[i]->y - self->y)) < temp))
            {
                nearestStudentPositon.first = students[i]->x; nearestStudentPositon.second = students[i]->y;
                temp = abs(students[i]->x - self->x) + abs(students[i]->y - self->y);
                StudentAddicted = false;
            }
            if (!StudentAddicted)
            {
                if (students[i]->playerState != THUAI6::PlayerState::Addicted && ((abs(students[i]->x - self->x) + abs(students[i]->y - self->y)) < temp))
                {
                    nearestStudentPositon.first = students[i]->x; nearestStudentPositon.second = students[i]->y;
                    temp = abs(students[i]->x - self->x) + abs(students[i]->y - self->y);
                    StudentAddicted = false;
                }
                //  }
            }
        }
        /* if (students.size() == 1 && students[0]->studentType == THUAI6::StudentType::Teacher)
         {
             moveWithoutGoal(api);
         }*/
        int targetX = api.GridToCell(nearestStudentPositon.first);
        int targetY = api.GridToCell(nearestStudentPositon.second);
        trickerMove(api, targetX, targetY);
        auto path = Astar(api, targetX, targetY, NodeMap);
        if (path.size() < 3)
        {
            attackStudents(api);
        }
    }
    //攻击函数
    void attackStudents(ITrickerAPI& api)
    {
        int temp;
        std::pair<int32_t, int32_t> nearestStudentPositon;
        auto self = api.GetSelfInfo();
        auto students = api.GetStudents();
        //找到最近的学生
        api.Print("Attacking");
        for (int i = 0; i < students.size(); i++)
        {
            if (i == 0)
            {
                temp = abs(students[i]->x - self->x) + abs(students[i]->y - self->y);
                nearestStudentPositon.first = students[i]->x; nearestStudentPositon.second = students[i]->y;
            }
            else if (((abs(students[i]->x - self->x) + abs(students[i]->y - self->y)) < temp))
            {
                nearestStudentPositon.first = students[i]->x; nearestStudentPositon.second = students[i]->y;
            }
        }
        double attackangle = atan2((nearestStudentPositon.second - self->y), (nearestStudentPositon.first - self->x));
        api.Attack(attackangle);
    }
    //随机挑选教室前往，并在附近停下来
    void moveWithoutGoal(ITrickerAPI& api)
    {
        api.Print("MovingToClass");
        //随机找一件教室前往
        int targetX, targetY;
        /*srand(time(0));
        int dest = rand() % ClassroomPosition.size();
        targetX = ClassroomPosition[dest].first;
        targetY = ClassroomPosition[dest].second;*/
        for (int i = 0; i < ClassroomPosition.size(); i++)
        {
            if (api.GetClassroomProgress(ClassroomPosition[i].first, ClassroomPosition[i].second) == -1)
            {
                targetX = ClassroomPosition[i].first; targetY = ClassroomPosition[i].second;
            }
        }
        trickerMove(api, targetX, targetY);
    }
    //选择最近的教室前往
    void goToNearestClassRoom(ITrickerAPI& api)
    {
        int temp; int targetX, targetY;
        auto self = api.GetSelfInfo();
        std::pair<int32_t, int32_t> nearestRoomPositon;
        int CurX = api.GridToCell(self->x);	int CurY = api.GridToCell(self->y);
        //找到附近的教室
        for (int i = 0; i < ClassroomPosition.size(); i++)
        {
            if (i == 0)
            {
                temp = abs(ClassroomPosition[i].first - CurX) + abs(ClassroomPosition[i].second - CurY);
                targetX = ClassroomPosition[i].first; targetY = ClassroomPosition[i].second;
            }
            else if ((abs(ClassroomPosition[i].first - CurX) + abs(ClassroomPosition[i].second - CurY)) < temp)
            {
                temp = (abs(ClassroomPosition[i].first - CurX) + abs(ClassroomPosition[i].second - CurY));
                targetX = ClassroomPosition[i].first; targetY = ClassroomPosition[i].second;
            }
        }
        trickerMove(api, targetX, targetY);
    }
    //有学生在附近但不在视野内,就探寻附近的草地或空地
    void trickerInAlert(ITrickerAPI& api)
    {
        api.Print("Alert!");
        auto self = api.GetSelfInfo();
        int x = api.GridToCell(self->x);
        int y = api.GridToCell(self->y);
        int targetX = 0; int targetY = 0;
        //找最近的门或者草地或者空地，如果人在空地，就找草地，如果人在
        if (x > 2 && y > 2)
        {
            for (int i = -2; i++; i < 3)
            {
                for (int j = -2; j++; j < 3)
                {
                    //自身在空地，探寻附近是否有草地
                    if (api.GetSelfInfo()->place == THUAI6::PlaceType::Land)
                    {
                        if (api.GetFullMap()[x + i][x + j] == THUAI6::PlaceType::Grass)
                        {
                            targetX = x + i; targetY = y + j;
                        }
                    }
                    //自身在草地，探寻附近是否有空地
                    if (api.GetSelfInfo()->place == THUAI6::PlaceType::Grass)
                    {
                        if (api.GetFullMap()[x + i][x + j] == THUAI6::PlaceType::Land)
                        {
                            targetX = x + i; targetY = y + j;
                        }
                    }
                }
            }
        }
        //前往最近的空地或者是草丛
        if (targetX != 0 && targetY != 0)
        {
            trickerMove(api, targetX, targetY);
        }
        else { goToNearestClassRoom(api); }
    }
    //获取警戒范围内的学生信息，返回一个int值（警戒半径/可被发现的最近的学生的距离）
    int getDesire(ITrickerAPI& api)
    {
        auto self = api.GetSelfInfo();
        int desireData = self->trickDesire;
        return desireData;
    }
    //获取警戒范围内学生学习的声音，返回一个int值（警戒半径*学习进度百分比）/ 二者距离）
    int gerVolume(ITrickerAPI& api)
    {
        auto self = api.GetSelfInfo();
        int volumeData;
        return volumeData;
    }
    //用于tricker去打开箱子,返回值：0->附近没有箱子；1->朝可开箱子前进中；2->已到达可开箱子附近
    int findChestClosed(ITrickerAPI& api)
    {
        api.Print("findChest");
        auto self = api.GetSelfInfo();
        int propsNum = 0;
        int targetX = 0, targetY = 0;
        for (int i = 0; i < 3; i++)
        {
            if (api.GetSelfInfo()->props[i] != THUAI6::PropType::NullPropType)
                propsNum++;
        }
        int x = api.GridToCell(self->x);
        int y = api.GridToCell(self->y);
        api.Print(fmt::format("{}", api.GetSelfInfo()->props.size()));
        if (propsNum <= 1)
        {
            for (int j = -2; j < 3; j++)
            {
                for (int i = -2; i < 3; i++)
                {
                    if (!isInMap(x + i, y + j))
                    {
                        continue;
                    }
                    api.Print(fmt::format("{},{}", x, y));
                    api.Print(fmt::format("({},{}),{}", x + i, y + j, THUAI6::placeTypeDict[Map[x + i][y + j]]));
                    if (Map[x + i][y + j] == THUAI6::PlaceType::Chest)
                    {
                        api.Print("ChestFound");
                        if (api.GetChestProgress(x + i, y + j) == 10000000);
                        else
                        {
                            targetX = x + i; targetY = y + j;
                            api.Print("ChestChecked");
                        }
                    }
                }
            }
        }
        if (targetX != 0 || targetY != 0)
        {
            auto tempPath = Astar(api, targetX, targetY, NodeMap);
            std::vector<std::pair<int32_t, int32_t>> path;
            if (tempPath.size() > 2)
            {
                api.Print("Moving");
                path.push_back({ tempPath[0].first, tempPath[0].second });
                path.push_back({ tempPath[1].first, tempPath[1].second });
                while (!path.empty())
                {
                    api.Print("GoToChest");
                    moveinto(api, path[0].first, path[0].second);
                    if (isInsideofcell(api, path[0].first, path[0].second))
                    {
                        path.erase(path.begin());
                    }
                }
                return 1;
            }
            else return 2;
        }
        return 0;
    }
    //捡道具
    void trickerPickProp(ITrickerAPI& api)
    {
        if (api.GetProps().size() != 0)
        {
            api.Print("OpenChest");
            auto props = api.GetProps();
            api.PickProp(props[0]->type);
            api.PickProp(props[1]->type);
        }
    }
};


class StudentCommunication
{
public:
    pair<int, int>ExtractNum2(string& str)
    {
        int index = str.find(',');
        string str1(str.begin(), str.begin() + index);
        string str2(str.begin() + index + 1, str.end());
        int x1 = atoi(str1.c_str()), x2 = atoi(str2.c_str());
        return pair<int, int>(x1, x2);
    }
    
    int ExtractNum1(string& str)
    {
        return atoi(str.c_str());
    }
}StuCom;

extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

void AI::play(IStudentAPI& api)
{
    
    if (!ifchecked)
    {
        api.Print("check map!");
        Map.reserve(50 * 50);
        Map = api.GetFullMap();
        for (int i = 0; i < 10; i++)
        {
            LearningProcess.push_back(0);
        }
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                switch (Map.at(i).at(j))
                {
                case(THUAI6::PlaceType::Window):Windows.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Wall):NodeMap[i][j] = { i,j,0,0,0,2,nullptr }; obstacleMap[i][j] = 1; break;
                case(THUAI6::PlaceType::NullPlaceType):NodeMap[i][j] = { i,j,0,0,0,0,nullptr };break;
                case(THUAI6::PlaceType::Land):NodeMap[i][j] = { i,j,0,0,0,0,nullptr };break;
                case(THUAI6::PlaceType::Grass):NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Grass[i][j] = 1; GrassPos.push_back({ i,j }); break;
                case(THUAI6::PlaceType::Chest):Chest.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door3):Door3Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door5):Door5Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door6):Door6Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Gate):Gate.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::ClassRoom):ClassroomPosition.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,2,nullptr }; Target[i][j] = 1; obstacleMap[i][j] = 1; break;
                }
            }
        }//弄个大地图
        ifchecked = true;
    }
    // 公共操作
    class MyFunction myf;
    if (this->playerID == 0)
    {
        if (api.HaveMessage())
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != myf.myId(api))
                {
                    string ClassMessage = api.GetMessage().second;
                    int classindex=StuCom.ExtractNum1(ClassMessage);
                    LearningProcess[classindex] = 1;
                }
            }
        }
        auto time_now = api.GetFrameCount();
        if (myf.IfDanger(api)&&time_now- RunAwayCoolTime >20)
        {
            RunAwayCoolTime = time_now;
            myf.RunAway(api);
        }
        else
        {
            if (myf.OpenTheGate())
            {
                myf.GoToGateAndGraduate(api);
            }
            else
            {
                myf.GoToClassAndLearning(api);
                myf.StraightAStudentSkill(api);
            }

        }
    }
    else if (this->playerID == 1)
    {
        if (api.HaveMessage())
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != myf.myId(api))
                {
                    string ClassMessage = api.GetMessage().second;
                    int classindex = StuCom.ExtractNum1(ClassMessage);
                    LearningProcess[classindex] = 1;
                }
            }
        }
        auto time_now = api.GetFrameCount();
        if (myf.IfDanger(api) && time_now - RunAwayCoolTime > 20)
        {
            RunAwayCoolTime = time_now;
            myf.RunAway(api);
        }
        else
        {
            if (myf.OpenTheGate())
            {
                myf.GoToGateAndGraduate(api);
            }
            else
            {
                myf.GoToClassAndLearning(api);
                myf.StraightAStudentSkill(api);
            }
        }
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        if (api.HaveMessage())
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != myf.myId(api))
                {
                    string ClassMessage = api.GetMessage().second;
                    int classindex = StuCom.ExtractNum1(ClassMessage);
                    LearningProcess[classindex] = 1;
                }
            }
        }
        auto time_now = api.GetFrameCount();
        if (myf.IfDanger(api) && time_now - RunAwayCoolTime > 20)
        {
            RunAwayCoolTime = time_now;
            myf.RunAway(api);
        }
        else
        {
            if (myf.OpenTheGate())
            {
                myf.GoToGateAndGraduate(api);
            }
            else
            {
                myf.GoToClassAndLearning(api);
                myf.StraightAStudentSkill(api);
            }
        }
    }
    else if (this->playerID == 3)
    {
        if (api.HaveMessage())
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != myf.myId(api))
                {
                    string ClassMessage = api.GetMessage().second;
                    int classindex = StuCom.ExtractNum1(ClassMessage);
                    LearningProcess[classindex] = 1;
                }
            }
        }
        auto time_now = api.GetFrameCount();
        if (myf.IfDanger(api) && time_now - RunAwayCoolTime > 20)
        {
            RunAwayCoolTime = time_now;
            myf.RunAway(api);
        }
        else
        {
            if (myf.OpenTheGate())
            {
                myf.GoToGateAndGraduate(api);
            }
            else
            {
                myf.GoToClassAndLearning(api);
                myf.StraightAStudentSkill(api);
            }
        }
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    MyTrickerFunction myt;
    if (!ifchecked)
    {
        api.Print("check map!");
        Map.reserve(50 * 50);
        Map = api.GetFullMap();
        for (int i = 0; i < 10; i++)
        {
            LearningProcess.push_back(0);
        }
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                switch (Map.at(i).at(j))
                {
                case(THUAI6::PlaceType::Window):Windows.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Wall):obstacleMap[i][j] = 1; break;
                case(THUAI6::PlaceType::NullPlaceType):;
                case(THUAI6::PlaceType::Land):;
                case(THUAI6::PlaceType::Grass):NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::Chest):Chest.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door3):Door3Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door5):Door5Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Door6):Door6Position.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; break;
                case(THUAI6::PlaceType::Gate):Gate.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; break;
                case(THUAI6::PlaceType::ClassRoom):ClassroomPosition.push_back({ i,j }); NodeMap[i][j] = { i,j,0,0,0,0,nullptr }; Target[i][j] = 1; obstacleMap[i][j] = 1; break;
                }
            }
        }//弄个大地图
        ifchecked = true;
    }
    //视野内有学生，进行追击
    if (!api.GetStudents().empty())
    {
        api.Print("Chasing");
        myt.chaseStudents(api);
    }
    //视野内没学生，但警惕值很高，学生就在很近的位置。
    else if (self->trickDesire > 7)
    {
        api.Print("Alert");
        myt.trickerInAlert(api);
    }
    //进行游走
    else
    {
        api.Print("WithoutGoal");
        myt.moveWithoutGoal(api);
    }
}
