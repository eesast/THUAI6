#include <vector>
#include <thread>
#include <queue>
#include <array>
#include<cmath>
#include "AI.h"
#include "constants.h"
using namespace std;

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4>  studentType = {
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine }
;

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数
int D3x, D5x, D6x, D3y, D5y, D6y;
int Gatex[3], Gatey[3];
int hidx[4], hidy[4];
int Boxx[8], Boxy[8];
int mapp[51][51];
bool finished[11] = { false,false,false,false,false,false,false,false,false,false };
vector<int>pathx;
vector<int>pathy;
int Workx[11], Worky[11];

typedef struct Node
{
    int x, y;
    int g; 
    int h;
    int f;
    Node* father;
    Node(int x, int y)
    {
        this->x = x;
        this->y = y;
        this->g = 0;
        this->h = 0;
        this->f = 0;
        this->father = NULL;
    }
    Node(int x, int y, Node* father)
    {
        this->x = x;
        this->y = y;
        this->g = 0;
        this->h = 0;
        this->f = 0;
        this->father = father;
    }
}Node;

class Astar {
public:
    Astar();
    ~Astar();
    void search(Node* startPos, Node* endPos);
    void checkPoint(int x, int y, Node* father, int g);
    void NextStep(Node* currentPoint);
    int isContains(vector<Node*>* Nodelist, int x, int y);
    void countGHF(Node* sNode, Node* eNode, int g);
    static bool compare(Node* n1, Node* n2);
    bool unWalk(int x, int y);
    void getPath(Node* current);
    vector<Node*> openList;
    vector<Node*> closeList;
    Node* startPos;
    Node* endPos;
    static const int WeightW = 10;
    static const int WeightWH = 14;
    static const int row = 50;
    static const int col = 50;
};


Astar::Astar()
{
}
Astar::~Astar()
{
}
void Astar::search(Node* startPos, Node* endPos)
{
    if (startPos->x < 0 || startPos->x > row || startPos->y < 0 || startPos->y >col
        ||
        endPos->x < 0 || endPos->x > row || endPos->y < 0 || endPos->y > col)
        return;
    Node* current;
    this->startPos = startPos;
    this->endPos = endPos;
    openList.push_back(startPos);
  
    while (openList.size() > 0)
    {
        current = openList[0];
        if (current->x == endPos->x && current->y == endPos->y)
        {
            getPath(current);
            openList.clear();
            closeList.clear();
            break;
        }
        NextStep(current);
        closeList.push_back(current);
        openList.erase(openList.begin());
        sort(openList.begin(), openList.end(), compare);
    }
}
void Astar::checkPoint(int x, int y, Node* father, int g)
{
    if (x < 0 || x > row || y < 0 || y > col)
        return;
    if (this->unWalk(x, y))
        return;
    if (isContains(&closeList, x, y) != -1)
        return;
    int index;
    if ((index = isContains(&openList, x, y)) != -1)
    {
        Node* point = openList[index];
        if (point->g > father->g + g)
        {
            point->father = father;
            point->g = father->g + g;
            point->f = point->g + point->h;
        }
    }
    else
    {
        Node* point = new Node(x, y, father);
        countGHF(point, endPos, g);
        openList.push_back(point);
    }
}
void Astar::NextStep(Node* current)
{
    checkPoint(current->x - 1, current->y, current, WeightW);//左
    checkPoint(current->x + 1, current->y, current, WeightW);//右
    checkPoint(current->x, current->y + 1, current, WeightW);//上
    checkPoint(current->x, current->y - 1, current, WeightW);//下
    checkPoint(current->x - 1, current->y + 1, current, WeightWH);//左上
    checkPoint(current->x - 1, current->y - 1, current, WeightWH);//左下
    checkPoint(current->x + 1, current->y - 1, current, WeightWH);//右下
    checkPoint(current->x + 1, current->y + 1, current, WeightWH);//右上
}
int Astar::isContains(vector<Node*>* Nodelist, int x, int y)
{
    for (int i = 0;i < Nodelist->size();i++)
    {
        if (Nodelist->at(i)->x == x && Nodelist->at(i)->y == y)
        {
            return i;
        }
    }
    return -1;
}
void Astar::countGHF(Node* sNode, Node* eNode, int g)
{
    int h = abs(sNode->x - eNode->x) * WeightW + abs(sNode->y - eNode->y) * WeightW;
    int currentg = sNode->father->g + g;
    int f = currentg + h;
    sNode->f = f;
    sNode->h = h;
    sNode->g = currentg;
}
bool Astar::compare(Node* n1, Node* n2)
{
   
    return n1->f < n2->f;
}
bool Astar::unWalk(int x, int y)
{
    if (mapp[x][y] == 1)
        return true;
    return false;
}
void Astar::getPath(Node* current)
{
    if (current->father != NULL)
        getPath(current->father);
    pathx.push_back(current->x);
    pathy.push_back(current->y);
    
}
void AI::play(IStudentAPI& api)
{
    // 公共操作

    static bool FlagMessage = false;
    static bool aimed0 = false;
    if (!FlagMessage)
    {
        int gatecnt = 0;
        int hidcnt = 0;
        int boxcnt = 0;
        int workcnt = 0;
        for (register int i = 0;i < 50;i++)
        {
            for (register int j = 0;j < 50;j++)
            {
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door3)//获取三教门的坐标
                {
                    D3x = i;
                    D3y = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door5)//获取五教门的坐标
                {
                    D5x = i;
                    D5y = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door6)//获取六教门的坐标                
                {
                    D6x = i;
                    D6y = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Gate)//获取大门的坐标                
                {
                    Gatex[gatecnt] = i;
                    Gatey[gatecnt++] = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::HiddenGate)//获取隐藏门刷新点的坐标                
                {
                    hidx[hidcnt] = i;
                    hidy[hidcnt++] = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Chest)//获取箱子的坐标                
                {
                    Boxx[boxcnt] = i;
                    Boxy[boxcnt++] = j;
                }
                if ((api.GetFullMap())[i][j] == THUAI6::PlaceType::ClassRoom)//获取作业的坐标                
                {
                    Workx[workcnt] = i;
                    Worky[workcnt++] = j;
                }
            }
        }
        for(register int i=0;i<50;i++)//还需要得到一个地图信息用于A*算法
            for (register int j = 0;j <50;j++)
            {
                if (((api.GetFullMap())[i][j] == THUAI6::PlaceType::Window) || ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Wall)|| ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door3)|| ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door6)|| ((api.GetFullMap())[i][j] == THUAI6::PlaceType::Door5))
                {
                    mapp[i][j] = 1;
                }
                else mapp[i][j] = 0;
            }
        FlagMessage = true;
    }

    if (this->playerID == 0)//0号是开心果，负责修机和加buff
    {
        // 玩家0执行操作
        static double dmin = 1000000000000;
        static bool pathed = false;
        static bool got = false;
        static int aimworkx, aimworky;
        static double angle = 0.0;
        static bool stuck = false;
        static int beforex, beforey;

        if (api.GetSelfInfo()->timeUntilSkillAvailable[2] == 0)//现在是加速技能一好就要用
            api.UseSkill(2);

        
        for (register int i = 0;i < 10;i++)//首先搜索最近的作业
        {
            if (sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) < dmin&&!finished[i])
            {
                dmin = sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i]));
                aimworkx = Workx[i];
                aimworky = Worky[i];

            }
        }

        //实现用A*寻路
        if (!got)
        {
            Astar astar;
            Node* stPos = new Node(api.GridToCell((api.GetSelfInfo())->x), api.GridToCell((api.GetSelfInfo())->y));
            Node* edPos = new Node(aimworkx, aimworky);
            astar.search(stPos, edPos);
            //至此，路径已被存在path[]中        
        // ------------------------------------------------------------------------
        //根据path进行移动:
            if (!stuck)
            {
            int Dir_x = pathx[1] - api.GridToCell((api.GetSelfInfo())->x);
            int Dir_y = pathy[1] - api.GridToCell((api.GetSelfInfo())->y);
            if (Dir_y > 0)
                api.MoveRight(100);
            else if (Dir_y < 0)
                api.MoveLeft(100);
            else if (Dir_y == 0)
            {
                if (Dir_x > 0)
                    api.MoveDown(100);
                else if (Dir_x < 0)
                    api.MoveUp(100);
            }
            pathx.clear();
            pathy.clear();
            if ((api.GetSelfInfo()->x) == beforex &&(api.GetSelfInfo()->y) == beforey&& (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1|| mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1|| mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1|| mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1))stuck = true;
            if (sqrt((api.GridToCell(api.GetSelfInfo()->x) - aimworkx) * (api.GridToCell(api.GetSelfInfo()->x) - aimworkx) + (api.GridToCell(api.GetSelfInfo()->y) - aimworky) * (api.GridToCell(api.GetSelfInfo()->y) - aimworky)) <= 1.5)
            {
                got = true;
            }
            }
            else
            {
                if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//右下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, 1));//右上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y)  -1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//左上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) -1] == 1)
                {
                    api.Move(100, atan2(1, -1));//左下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y)  ] == 1)
                {
                    api.MoveUp(100);//正下有墙
                    stuck = false;

                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y)  ] == 1)
                {
                    api.MoveUp(100);//正上有墙
                    stuck = false;

                }
                api.Print("stuck");
            }
        }
        if (got)
        {
            api.StartLearning();
            if (api.GetClassroomProgress(aimworkx, aimworky) == 10000000)
            {
                got = false;
                for (register int i = 0;i < 10;i++)
                {
                    if (aimworkx == Workx[i])
                    {
                        finished[i] = true;
                        api.SendMessage(1, fmt::format("F {:}", i));
                        api.SendMessage(2, fmt::format("F {:}", i));
                        api.SendMessage(3, fmt::format("F {:}", i));
                       
                    }
                }
               
            }

        }
        beforex = (api.GetSelfInfo()->x);
        beforey = (api.GetSelfInfo()->y);

        
    }
    else if (this->playerID == 1)
    {
       
        // 玩家0执行操作
        static double dmin = 1000000000000;
        static bool pathed = false;
        static bool got = false;
        static int aimworkx, aimworky;
        static double angle = 0.0;
        static bool stuck = false;
        static int beforex, beforey;

        if (api.GetSelfInfo()->timeUntilSkillAvailable[2] == 0)//现在是加速技能一好就要用
            api.UseSkill(2);


        for (register int i = 0;i < 10;i++)//首先搜索最近的作业
        {
            if (sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) < dmin && !finished[i])
            {
                dmin = sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i]));
                aimworkx = Workx[i];
                aimworky = Worky[i];

            }
        }

        //实现用A*寻路
        if (!got)
        {
            Astar astar;
            Node* stPos = new Node(api.GridToCell((api.GetSelfInfo())->x), api.GridToCell((api.GetSelfInfo())->y));
            Node* edPos = new Node(aimworkx, aimworky);
            astar.search(stPos, edPos);
            //至此，路径已被存在path[]中        
        // ------------------------------------------------------------------------
        //根据path进行移动:
            if (!stuck)
            {
                int Dir_x = pathx[1] - api.GridToCell((api.GetSelfInfo())->x);
                int Dir_y = pathy[1] - api.GridToCell((api.GetSelfInfo())->y);
                if (Dir_y > 0)
                    api.MoveRight(100);
                else if (Dir_y < 0)
                    api.MoveLeft(100);
                else if (Dir_y == 0)
                {
                    if (Dir_x > 0)
                        api.MoveDown(100);
                    else if (Dir_x < 0)
                        api.MoveUp(100);
                }
                pathx.clear();
                pathy.clear();
                if ((api.GetSelfInfo()->x) == beforex && (api.GetSelfInfo()->y) == beforey && (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1))stuck = true;
                if (sqrt((api.GridToCell(api.GetSelfInfo()->x) - aimworkx) * (api.GridToCell(api.GetSelfInfo()->x) - aimworkx) + (api.GridToCell(api.GetSelfInfo()->y) - aimworky) * (api.GridToCell(api.GetSelfInfo()->y) - aimworky)) <= 1.5)
                {
                    got = true;
                }
            }
            else
            {
                if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//右下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, 1));//右上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//左上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(1, -1));//左下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正下有墙
                    stuck = false;

                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正上有墙
                    stuck = false;

                }
                api.Print("stuck");
            }
        }
        if (got)
        {
            api.StartLearning();
            if (api.GetClassroomProgress(aimworkx, aimworky) == 10000000)
            {
                got = false;
                for (register int i = 0;i < 10;i++)
                {
                    if (aimworkx == Workx[i])
                    {
                        finished[i] = true;
                        api.SendMessage(1, fmt::format("F {:}", i));
                        api.SendMessage(2, fmt::format("F {:}", i));
                        api.SendMessage(3, fmt::format("F {:}", i));

                    }
                }

            }

        }
        beforex = (api.GetSelfInfo()->x);
        beforey = (api.GetSelfInfo()->y);

    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        
        static double dmin = 1000000000000;
        static bool pathed = false;
        static bool got = false;
        static int aimworkx, aimworky;
        static double angle = 0.0;
        static bool stuck = false;
        static int beforex, beforey;

        if (api.GetSelfInfo()->timeUntilSkillAvailable[2] == 0)//现在是加速技能一好就要用
            api.UseSkill(2);


        for (register int i = 0;i < 10;i++)//首先搜索最近的作业
        {
            if (sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) < dmin && !finished[i])
            {
                dmin = sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i]));
                aimworkx = Workx[i];
                aimworky = Worky[i];

            }
        }

        //实现用A*寻路
        if (!got)
        {
            Astar astar;
            Node* stPos = new Node(api.GridToCell((api.GetSelfInfo())->x), api.GridToCell((api.GetSelfInfo())->y));
            Node* edPos = new Node(aimworkx, aimworky);
            astar.search(stPos, edPos);
            //至此，路径已被存在path[]中        
        // ------------------------------------------------------------------------
        //根据path进行移动:
            if (!stuck)
            {
                int Dir_x = pathx[1] - api.GridToCell((api.GetSelfInfo())->x);
                int Dir_y = pathy[1] - api.GridToCell((api.GetSelfInfo())->y);
                if (Dir_y > 0)
                    api.MoveRight(100);
                else if (Dir_y < 0)
                    api.MoveLeft(100);
                else if (Dir_y == 0)
                {
                    if (Dir_x > 0)
                        api.MoveDown(100);
                    else if (Dir_x < 0)
                        api.MoveUp(100);
                }
                pathx.clear();
                pathy.clear();
                if ((api.GetSelfInfo()->x) == beforex && (api.GetSelfInfo()->y) == beforey && (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1))stuck = true;
                if (sqrt((api.GridToCell(api.GetSelfInfo()->x) - aimworkx) * (api.GridToCell(api.GetSelfInfo()->x) - aimworkx) + (api.GridToCell(api.GetSelfInfo()->y) - aimworky) * (api.GridToCell(api.GetSelfInfo()->y) - aimworky)) <= 1.5)
                {
                    got = true;
                }
            }
            else
            {
                if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//右下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, 1));//右上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//左上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(1, -1));//左下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正下有墙
                    stuck = false;

                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正上有墙
                    stuck = false;

                }
                api.Print("stuck");
            }
        }
        if (got)
        {
            api.StartLearning();
            if (api.GetClassroomProgress(aimworkx, aimworky) == 10000000)
            {
                got = false;
                for (register int i = 0;i < 10;i++)
                {
                    if (aimworkx == Workx[i])
                    {
                        finished[i] = true;
         

                    }
                }

            }

        }
        beforex = (api.GetSelfInfo()->x);
        beforey = (api.GetSelfInfo()->y);

    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        
        static double dmin = 1000000000000;
        static bool pathed = false;
        static bool got = false;
        static int aimworkx, aimworky;
        static double angle = 0.0;
        static bool stuck = false;
        static int beforex, beforey;

        if (api.GetSelfInfo()->timeUntilSkillAvailable[2] == 0)//现在是加速技能一好就要用
            api.UseSkill(2);


        for (register int i = 0;i < 10;i++)//首先搜索最近的作业
        {
            if (sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) < dmin && !finished[i])
            {
                dmin = sqrt((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) * ((api.GetSelfInfo())->x - api.CellToGrid(Workx[i])) + ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i])) * ((api.GetSelfInfo())->y - api.CellToGrid(Worky[i]));
                aimworkx = Workx[i];
                aimworky = Worky[i];

            }
        }

        //实现用A*寻路
        if (!got)
        {
            Astar astar;
            Node* stPos = new Node(api.GridToCell((api.GetSelfInfo())->x), api.GridToCell((api.GetSelfInfo())->y));
            Node* edPos = new Node(aimworkx, aimworky);
            astar.search(stPos, edPos);
            //至此，路径已被存在path[]中        
        // ------------------------------------------------------------------------
        //根据path进行移动:
            if (!stuck)
            {
                int Dir_x = pathx[1] - api.GridToCell((api.GetSelfInfo())->x);
                int Dir_y = pathy[1] - api.GridToCell((api.GetSelfInfo())->y);
                if (Dir_y > 0)
                    api.MoveRight(100);
                else if (Dir_y < 0)
                    api.MoveLeft(100);
                else if (Dir_y == 0)
                {
                    if (Dir_x > 0)
                        api.MoveDown(100);
                    else if (Dir_x < 0)
                        api.MoveUp(100);
                }
                pathx.clear();
                pathy.clear();
                if ((api.GetSelfInfo()->x) == beforex && (api.GetSelfInfo()->y) == beforey && (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1 || mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1))stuck = true;
                if (sqrt((api.GridToCell(api.GetSelfInfo()->x) - aimworkx) * (api.GridToCell(api.GetSelfInfo()->x) - aimworkx) + (api.GridToCell(api.GetSelfInfo()->y) - aimworky) * (api.GridToCell(api.GetSelfInfo()->y) - aimworky)) <= 1.5)
                {
                    got = true;
                }
            }
            else
            {
                if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//右下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) + 1] == 1)
                {
                    api.Move(100, atan2(-1, 1));//右上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(-1, -1));//左上有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y) - 1] == 1)
                {
                    api.Move(100, atan2(1, -1));//左下有墙
                    stuck = false;
                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) + 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正下有墙
                    stuck = false;

                }
                else if (mapp[api.GridToCell(api.GetSelfInfo()->x) - 1][api.GridToCell(api.GetSelfInfo()->y)] == 1)
                {
                    api.MoveUp(100);//正上有墙
                    stuck = false;

                }
                api.Print("stuck");
            }
        }
        if (got)
        {
            api.StartLearning();
            if (api.GetClassroomProgress(aimworkx, aimworky) == 10000000)
            {
                got = false;
                for (register int i = 0;i < 10;i++)
                {
                    if (aimworkx == Workx[i])
                    {
                        finished[i] = true;
                

                    }
                }

            }

        }
        beforex = (api.GetSelfInfo()->x);
        beforey = (api.GetSelfInfo()->y);

    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}

void AI::play(ITrickerAPI& api)//捣蛋鬼是刺客，初步定下是先在固定位置游走，能看到学生就直接开始追
{
    static double angle;
    if (api.GetStudents().size() > 0)
    {
        angle = atan2(api.GetStudents()[0]->y - api.GetSelfInfo()->y, api.GetStudents()[0]->x - api.GetSelfInfo()->x);
        if(sqrt((api.GetStudents()[0]->y - api.GetSelfInfo()->y)*(api.GetStudents()[0]->y - api.GetSelfInfo()->y)+(api.GetStudents()[0]->x - api.GetSelfInfo()->x)*(api.GetStudents()[0]->x - api.GetSelfInfo()->x)<=2200))
            api.Attack(angle);
        api.Move(100, angle);
    }
    else
    {
        api.MoveLeft(200);
        api.EndAllAction();
        api.MoveDown(200);
        api.EndAllAction();
        api.MoveRight(200);
        api.EndAllAction();
        api.MoveUp(200);
    }
}
