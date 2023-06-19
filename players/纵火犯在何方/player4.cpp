#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include"structures.h"
#include<iostream>

/*  现用库   */
#include<list>
#include<math.h>
#include<stdio.h>
#define min(a,b) (a>b?b:a)
#define max(a,b) (a>b?a:b)
#define PI 3.14159265358979323846
/*  现用库   */

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义
extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

/*Map Define*/
int FullMap[50][50];
int map[50][50];
int map_copy[50][50];

/*状态集*/
int Situation = 0;
int LastSituation = 0;
/*
0:学习
1:毕业
2;偷学
3;逃跑
*/

/*  AsStar  */
int mymap_flag = 0;
int m = 50, n = 50;
std::vector<int>Tricker_Node = { 0,0 };
struct Node {
    int x, y;
    int F, G, H;
    Node* parent;
    int parentDir;
    Node(int _x, int _y) :x(_x), y(_y), F(0), G(0), H(0), parent(NULL) {}
};
void OutPut(std::list<Node*>List)
{
    while (!List.empty())
    {
        printf("(%d,%d) parentDir=%d\n", List.front()->x, List.front()->y, List.front()->parentDir);
        List.pop_front();
    }
    printf("\n");
}
class A_Star {
public:
    void Clear()
    {
        openlist.clear();
        closelist.clear();
    }
    void update()
    {
        openlist.clear();
        closelist.clear();
    }
    void Reshape()
    {
        for (int i = 0; i < 50; i++)
            for (int j = 0; j < 50; j++)
                map[i][j] = map_copy[i][j];
    }
    std::list<Node*> GetPath(std::vector<int> st, std::vector<int> ed) {
        openlist.push_back(new Node(st[0], st[1]));
        while (!openlist.empty()) {
            Node* CurrentUpdateNode = GetLeastF();
            closelist.push_back(CurrentUpdateNode);
            //printf("Node(%d,%d)\n",CurrentUpdateNode->x,CurrentUpdateNode->y);
            openlist.remove(CurrentUpdateNode);
            UpdateFValue(CurrentUpdateNode, ed);
            Node* CurPathNode = GetDestSurroundNode(ed);
            if (CurPathNode) {
                std::list<Node*> path;
                while (CurPathNode != NULL) {
                    path.push_front(CurPathNode);
                    CurPathNode = CurPathNode->parent;
                }
                return path;
            }
        }
        std::list<Node*> emptylist;
        return emptylist;
    }
private:
    void UpdateFValue(Node* CurNode, std::vector<int> ed) {
        std::list<std::vector<int>> UpdateList = GetSurroundNode(CurNode);
        for (auto Curnum = UpdateList.begin(); Curnum != UpdateList.end(); Curnum++) {
            int i = (*Curnum)[0], j = (*Curnum)[1];
            if (isInList(openlist, i, j) == NULL) {
                Node* UpdateNode = new Node(i, j);
                UpdateNode->parent = CurNode;
                UpdateNode->parentDir = (*Curnum)[2];
                if (map[i][j] == 1)
                    UpdateNode->G = CurNode->G + 1 * (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                else
                    UpdateNode->G = CurNode->G + (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                UpdateNode->H = (abs(ed[0] - i) + abs(ed[1] - j)) * 10;
                UpdateNode->F = UpdateNode->G + UpdateNode->H;
                openlist.push_back(UpdateNode);
            }
            else {
                Node* UpdateNode = isInList(openlist, i, j);
                if (CurNode->G + (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y))) <= UpdateNode->G) {
                    UpdateNode->parent = CurNode;
                    UpdateNode->parentDir = (*Curnum)[2];
                    UpdateNode->G = CurNode->G + (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                    UpdateNode->F = UpdateNode->G + UpdateNode->H;
                }
            }
        }
    }
    std::list<std::vector<int>> GetSurroundNode(Node* CurNode) {
        std::list<std::vector<int>> SurroundNodeList;
        bool up = false, down = false, left = false, right = false;
        if (CurNode->x > 0 && map[CurNode->x - 1][CurNode->y] != 2 && isInList(closelist, CurNode->x - 1, CurNode->y) == NULL)
            SurroundNodeList.push_back({ CurNode->x - 1,CurNode->y,4 }), up = true;
        if (CurNode->x < m - 1 && map[CurNode->x + 1][CurNode->y] != 2 && isInList(closelist, CurNode->x + 1, CurNode->y) == NULL)
            SurroundNodeList.push_back({ CurNode->x + 1,CurNode->y,0 }), down = true;
        if (CurNode->y > 0 && map[CurNode->x][CurNode->y - 1] != 2 && isInList(closelist, CurNode->x, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x,CurNode->y - 1,6 }), left = true;
        if (CurNode->y < n - 1 && map[CurNode->x][CurNode->y + 1] != 2 && isInList(closelist, CurNode->x, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x,CurNode->y + 1,2 }), right = true;
        if (up && left && map[CurNode->x - 1][CurNode->y - 1] != 2 && isInList(closelist, CurNode->x - 1, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x - 1,CurNode->y - 1,5 });
        if (up && right && map[CurNode->x - 1][CurNode->y + 1] != 2 && isInList(closelist, CurNode->x - 1, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x - 1,CurNode->y + 1,3 });
        if (down && left && map[CurNode->x + 1][CurNode->y - 1] != 2 && isInList(closelist, CurNode->x + 1, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x + 1,CurNode->y - 1,7 });
        if (down && right && map[CurNode->x + 1][CurNode->y + 1] != 2 && isInList(closelist, CurNode->x + 1, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({ CurNode->x + 1,CurNode->y + 1,1 });
        return SurroundNodeList;
    }
    Node* isInList(std::list<Node*>& List, int x, int y) {
        for (auto i = List.begin(); i != List.end(); i++)
            if ((*i)->x == x && (*i)->y == y)
                return (*i);
        return NULL;
    }
    Node* GetDestSurroundNode(std::vector<int> ed) {
        Node* retNode;
        retNode = isInList(openlist, ed[0] - 1, ed[1]);
        if (retNode) return retNode;
        retNode = isInList(openlist, ed[0] + 1, ed[1]);
        if (retNode) return retNode;
        retNode = isInList(openlist, ed[0], ed[1] - 1);
        if (retNode) return retNode;
        retNode = isInList(openlist, ed[0], ed[1] + 1);
        if (retNode) return retNode;
        return NULL;
    }
    Node* GetLeastF() {
        if (!openlist.empty()) {
            Node* resPoint = openlist.front();
            for (auto i = openlist.begin(); i != openlist.end(); i++)
                if (resPoint->F > (*i)->F)
                    resPoint = *i;
            return resPoint;
        }
        return NULL;
    }
private:
    std::list<Node*> openlist;
    std::list<Node*> closelist;
}AStar, AStar1;
/*  AStar   */

/*  Gate & Classroom & Grass  */
class Gate
{
public:
    int OpenRate;
    std::vector<int> GetNode()
    {
        return std::vector<int> {X, Y};
    }
    std::vector<int> GetPlace()
    {
        return std::vector<int>{X * 1000 + 500, Y * 1000 + 500};
    }
public:
    int X, Y;
};
class Grass
{
public:
    std::vector<int> GetNode()
    {
        return std::vector<int> {X, Y};
    }
    std::vector<int> GetPlace()
    {
        return std::vector<int>{X * 1000 + 500, Y * 1000 + 500};
    }
public:
    int X, Y;
};
class Classroom
{
public:
    int StudyRate;
    std::vector<int> GetNode()
    {
        return std::vector<int> {X, Y};
    }
    std::vector<int> GetPlace()
    {
        return std::vector<int>{X * 1000 + 500, Y * 1000 + 500};
    }
public:
    int X, Y;
};
Classroom classroom[15];
Gate gate[5];
Grass grass[200];
/*  Gate & Classroom & Grass  */

/*  FirstFrame & Map  */
int tar_G;
int tar_E;
int tar_L = 0;
int classroom_count = 0;
int gate_count = 0;
int grass_count = 0;
bool StartFrame = true;
bool IsMap1 = false;
void Start(IStudentAPI& api)
{
    auto mapinfo = api.GetFullMap();

    for (int i = 0; i < 50; i++)
        for (int j = 0; j < 50; j++)
        {
            FullMap[i][j] = (int)mapinfo[i][j];
            if ((int)api.GetSelfInfo()->studentType == (int)THUAI6::StudentType::StraightAStudent)
            {
                map[i][j] = ((int)mapinfo[i][j] == 2 || (int)mapinfo[i][j] == 4 || (int)mapinfo[i][j] == 7 || (int)mapinfo[i][j] == 5|| (int)mapinfo[i][j] ==11 ) ? 2 : 0;
            }
            else
            {
                map[i][j] = ((int)mapinfo[i][j] == 2 || (int)mapinfo[i][j] == 4 || (int)mapinfo[i][j] == 5||(int)mapinfo[i][j] == 11) ? 2 : 0;
            }
            map_copy[i][j] = map[i][j];
            if ((int)mapinfo[i][j] == 4)
            {
                classroom[classroom_count].X = i;
                classroom[classroom_count].Y = j;
                classroom[classroom_count].StudyRate = 0;
                //printf("%d,%d\n", i, j);
                classroom_count++;
            }
            if ((int)mapinfo[i][j] == 5)
            {
                gate[gate_count].X = i;
                gate[gate_count].Y = j;
                gate[gate_count].OpenRate = 0;
                gate_count++;
            }
            if ((int)mapinfo[i][j] == 3)
            {
                grass[grass_count].X = i;
                grass[grass_count].Y = j;
                grass_count++;
            }
        }
    if (FullMap[5][6] == 5)
    {
        IsMap1 = true;
    }
    StartFrame = false;
}
/*  FirstFrame & Map  */

/*  通信处理   */
std::string ClassroomMessage(int a)
{
    switch (a)
    {
    case 0:return "C0"; break;
    case 1:return "C1"; break;
    case 2:return "C2"; break;
    case 3:return "C3"; break;
    case 4:return "C4"; break;
    case 5:return "C5"; break;
    case 6:return "C6"; break;
    case 7:return "C7"; break;
    case 8:return "C8"; break;
    case 9:return "C9"; break;
    default:break;
    }
}
int DClassroomMessage(std::string c)
{
    if (!c.empty() && c[0] == 'C')
    {
        switch (c[1])
        {
        case '0':return 0; break;
        case '1':return 1; break;
        case '2':return 2; break;
        case '3':return 3; break;
        case '4':return 4; break;
        case '5':return 5; break;
        case '6':return 6; break;
        case '7':return 7; break;
        case '8':return 8; break;
        case '9':return 9; break;
        default:break;
        }
    }
    return -1;
}
/*  通信处理   */

/*  指向移动   */
bool Move_towards_node(std::vector<int>& target_node, IStudentAPI& api)
{
    //printf("Current target node is (%d,%d)", target_node[0], target_node[1]);
    std::vector<int> Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> target_point = { target_node[0] * 1000 + 500,target_node[1] * 1000 + 500 };
    if (api.GetSelfInfo()->speed <= 3150)
    {
        if (abs(Current_point[0] - target_point[0]) < 100 && abs(Current_point[1] - target_point[1]) < 100)
        {
            return true;
        }
        if (abs(target_point[0] - Current_point[0]) < 100)
        {
            if (target_point[1] > Current_point[1])
                api.MoveRight(50);
            else
                api.MoveLeft(50);
        }
        else if (abs(target_point[1] - Current_point[1]) < 100)
        {
            if (target_point[0] > Current_point[0])
                api.MoveDown(50);
            else
                api.MoveUp(50);
        }
        else if (target_point[0] > Current_point[0] && target_point[1] > Current_point[1])
            api.Move(50, PI / (double)4);
        else if (target_point[0] < Current_point[0] && target_point[1] > Current_point[1])
            api.Move(50, PI * 3 / (double)4);
        else if (target_point[0] < Current_point[0] && target_point[1] < Current_point[1])
            api.Move(50, PI * 5 / (double)4);
        else if (target_point[0] > Current_point[0] && target_point[1] < Current_point[1])
            api.Move(50, PI * 7 / (double)4);
    }
    else
    {
        if (abs(Current_point[0] - target_point[0]) < 160 && abs(Current_point[1] - target_point[1]) < 160)
        {
            return true;
        }
        if (abs(target_point[0] - Current_point[0]) < 100)
        {
            if (target_point[1] > Current_point[1])
                api.MoveRight(50);
            else
                api.MoveLeft(50);
        }
        else if (abs(target_point[1] - Current_point[1]) < 100)
        {
            if (target_point[0] > Current_point[0])
                api.MoveDown(50);
            else
                api.MoveUp(50);
        }
        else if (target_point[0] > Current_point[0] && target_point[1] > Current_point[1])
            api.Move(50, PI / (double)4);
        else if (target_point[0] < Current_point[0] && target_point[1] > Current_point[1])
            api.Move(50, PI * 3 / (double)4);
        else if (target_point[0] < Current_point[0] && target_point[1] < Current_point[1])
            api.Move(50, PI * 5 / (double)4);
        else if (target_point[0] > Current_point[0] && target_point[1] < Current_point[1])
            api.Move(50, PI * 7 / (double)4);
    }
    return false;
}
/*  指向移动   */

/*  寻路移动   */
std::list<Node*>path;
std::list<Node*>::iterator next_node;
std::vector<int> Past_point = { 0,0 };
bool AStarRun = false;
int Lagtime = 0;
bool Move_to(std::vector<int>Target, IStudentAPI& api)
{
    std::vector<int> Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000,Current_point[1] / 1000 };
    if (!AStarRun)
    {
        path.clear();
        path = AStar.GetPath(Current_node, Target);
        AStarRun = true;
        //OutPut(path);
        if (!path.empty())
        {
            next_node = path.begin();
            //next_node++;
        }
    }
    std::vector<int>Current_target = { (*next_node)->x,(*next_node)->y };
    if (path.empty())
    {
        path.clear();
        AStar.Clear();
        AStar.Reshape();
        AStarRun = false;
    }
    if (!path.empty() && next_node != path.end())
    {
        if (FullMap[Current_target[0]][Current_target[1]] == 7)
        {
            api.SkipWindow();
        }
        if (Move_towards_node(Current_target, api))
        {
            ++next_node;
        }
        if (Past_point[0] == Current_point[0] && Past_point[1] == Current_point[1])
        {
            Lagtime++;
            //printf("Lag! %d (%d,%d) %d\n", Lagtime,Current_target[0],Current_target[1], map[Current_target[0]][Current_target[1]]);
            if (Lagtime == 10)
            {
                path.clear();
                AStar.Clear();
                AStar.Reshape();
                map[Current_target[0]][Current_target[1]] = 2;
                AStarRun = false;
            }
        }
        else
        {
            Lagtime = 0;
        }
    }
    if (!path.empty() && next_node == path.end())
    {
        path.clear();
        AStar.Clear();
        AStar.Reshape();
        AStarRun = false;
        return true;
    }
    Past_point[0] = Current_point[0];
    Past_point[1] = Current_point[1];
    return false;
}
/*  寻路移动   */

/*  快速地图搜索  */
int Fast_Search_classroom(int last_tar_L, std::vector<int>Current_node)
{
    int Search_classroom(std::vector<int>Current_node);
    int finished_count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (classroom[i].StudyRate == 10000000)
        {
            finished_count++;
        }
    }
    if (finished_count >= 8)
        return -1;
    if (abs(Current_node[0] - classroom[last_tar_L].X) <= 1 && abs(Current_node[1] - classroom[last_tar_L].Y) <= 1)
    {
        if (IsMap1)
        {
            switch (last_tar_L)
            {
            case -1:return -1;
            case 0:
            {
                if (classroom[1].StudyRate < 10000000)
                    return 1;
                else if (classroom[3].StudyRate < 10000000)
                    return 3;
                else if (classroom[2].StudyRate < 10000000)
                    return 2;
                else if (classroom[7].StudyRate < 10000000)
                    return 7;
                else
                    return Search_classroom(Current_node);
            }
            case 1:
            {
                if (classroom[0].StudyRate < 10000000)
                    return 0;
                else if (classroom[2].StudyRate < 10000000)
                    return 2;
                else
                    return Search_classroom(Current_node);
            }
            case 2:
            {
                if (classroom[6].StudyRate < 10000000)
                    return 6;
                else if (classroom[4].StudyRate < 10000000)
                    return 4;
                else
                    return Search_classroom(Current_node);
            }
            case 3:
            {
                if (classroom[1].StudyRate < 10000000)
                    return 1;
                else if (classroom[0].StudyRate < 10000000)
                    return 0;
                else if (classroom[8].StudyRate < 10000000)
                    return 8;
                else
                    return Search_classroom(Current_node);
            }
            case 4:
            {
                if (classroom[2].StudyRate < 10000000)
                    return 2;
                else if (classroom[6].StudyRate < 10000000)
                    return 6;
                else
                    return Search_classroom(Current_node);
            }
            case 5:
            {
                if (classroom[8].StudyRate < 10000000)
                    return 8;
                else if (classroom[7].StudyRate < 10000000)
                    return 7;
                else if (classroom[9].StudyRate < 10000000)
                    return 9;
                else
                    return Search_classroom(Current_node);
            }
            case 6:
            {
                if (classroom[2].StudyRate < 10000000)
                    return 2;
                else if (classroom[4].StudyRate < 10000000)
                    return 4;
                else
                    return Search_classroom(Current_node);
            }
            case 7:
            {
                if (classroom[5].StudyRate < 10000000)
                    return 5;
                else if (classroom[9].StudyRate < 10000000)
                    return 9;
                else if (classroom[8].StudyRate < 10000000)
                    return 8;
                else
                    return Search_classroom(Current_node);
            }
            case 8:
            {
                if (classroom[5].StudyRate < 10000000)
                    return 5;
                else if (classroom[9].StudyRate < 10000000)
                    return 9;
                else if (classroom[7].StudyRate < 10000000)
                    return 7;
                else
                    return Search_classroom(Current_node);
            }
            case 9:
            {
                if (classroom[8].StudyRate < 10000000)
                    return 8;
                else if (classroom[5].StudyRate < 10000000)
                    return 5;
                else if (classroom[7].StudyRate < 10000000)
                    return 7;
                else
                    return Search_classroom(Current_node);
            }
            }
        }
        else
        {
            switch (last_tar_L)
            {
            case -1:return -1;
            case 0:
            {
                if (classroom[3].StudyRate < 10000000)
                    return 3;
                else if (classroom[1].StudyRate < 10000000)
                    return 1;
                else
                    return Search_classroom(Current_node);
            }
            case 1:
            {
                if (classroom[2].StudyRate < 10000000)
                    return 2;
                else if (classroom[3].StudyRate < 10000000)
                    return 3;
                else
                    return Search_classroom(Current_node);
            }
            case 2:
            {
                if (classroom[1].StudyRate < 10000000)
                    return 1;
                else
                    return Search_classroom(Current_node);
            }
            case 3:
            {
                if (classroom[5].StudyRate < 10000000)
                    return 5;
                else if (classroom[4].StudyRate < 10000000)
                    return 4;
                else if (classroom[1].StudyRate < 10000000)
                    return 1;
                else
                    return Search_classroom(Current_node);
            }
            case 4:
            {
                if (classroom[6].StudyRate < 10000000)
                    return 6;
                else if (classroom[3].StudyRate < 10000000)
                    return 3;
                else
                    return Search_classroom(Current_node);
            }
            case 5:
            {
                if (classroom[6].StudyRate < 10000000)
                    return 6;
                else if (classroom[3].StudyRate < 10000000)
                    return 3;
                else
                    return Search_classroom(Current_node);
            }
            case 6:
            {
                if (classroom[5].StudyRate < 10000000)
                    return 5;
                else if (classroom[4].StudyRate < 10000000)
                    return 4;
                else
                    return Search_classroom(Current_node);
            }
            case 7:
            {
                if (classroom[8].StudyRate < 10000000)
                    return 8;
                else if (classroom[5].StudyRate < 10000000)
                    return 5;
                else
                    return Search_classroom(Current_node);
            }
            case 8:
            {
                if (classroom[9].StudyRate < 10000000)
                    return 9;
                else if (classroom[7].StudyRate < 10000000)
                    return 7;
                else
                    return Search_classroom(Current_node);
            }
            case 9:
            {
                if (classroom[6].StudyRate < 10000000)
                    return 6;
                else if (classroom[8].StudyRate < 10000000)
                    return 8;
                else
                    return Search_classroom(Current_node);
            }
            }
        }
    }
    else
    {
        return Search_classroom(Current_node);
    }
}
/*  快速地图搜索  */

/*  位置搜索   */
int Search_gate(std::vector<int>Current_node)
{
    int length0 = abs(Current_node[0] - gate[0].X) + abs(Current_node[1] - gate[0].Y);
    int length1 = abs(Current_node[0] - gate[1].X) + abs(Current_node[1] - gate[1].Y);
    return length0 <= length1 ? 0 : 1;
}
int Search_grass(std::vector<int>Current_node, IStudentAPI& api)
{
    int min = 0;
    int length[150];
    auto tricker_info = api.GetTrickers();
    if (!tricker_info.empty())
    {
        for (int i = 0; i < grass_count; i++)
        {
            length[i] = 2 * abs(Current_node[0] - grass[i].X) + 2 * abs(Current_node[1] - grass[i].Y) - abs(tricker_info[0]->x - grass[i].X) - abs(tricker_info[0]->y - grass[i].Y);
            if (Current_node[0] >= grass[i].X && Current_node[1] >= grass[i].Y && Current_node[0] >= tricker_info[0]->x && Current_node[1] >= tricker_info[0]->y || Current_node[0] < grass[i].X && Current_node[1] >= grass[i].Y && Current_node[0] < tricker_info[0]->x && Current_node[1] >= tricker_info[0]->y && Current_node[0] >= grass[i].X && Current_node[1] < grass[i].Y && Current_node[0] >= tricker_info[0]->x && Current_node[1] < tricker_info[0]->y || Current_node[0] < grass[i].X && Current_node[1] < grass[i].Y && Current_node[0] < tricker_info[0]->x && Current_node[1] < tricker_info[0]->y)
            {
                length[i] = length[i] * 3;
            }
        }
        for (int i = 0; i < grass_count; i++)
        {
            if (length[i] < length[min])
                min = i;
        }
        return min;
    }
    else
    {
        for (int i = 0; i < grass_count; i++)
        {
            length[i] = abs(Current_node[0] - grass[i].X) + abs(Current_node[1] - grass[i].Y);
        }
        for (int i = 0; i < grass_count; i++)
        {
            if (length[i] < length[min])
                min = i;
        }
        return min;
    }
}
int Search_classroom(std::vector<int>Current_node)
{
    int min = 0;
    int length[10];
    int Finished_count = 0;
    for (int i = 0; i < classroom_count; i++)
    {
        length[i] = abs(Current_node[0] - classroom[i].X) + abs(Current_node[1] - classroom[i].Y);
    }
    for (int k = 0; k < classroom_count; k++)
    {
        if (classroom[k].StudyRate == 10000000)
        {
            if (min == k)
            {
                min++;
            }
            Finished_count++;
        }
    }
    for (int i = 0; i < 10; i++)
    {
        if (classroom[i].StudyRate < 10000000)
        {
            if (length[i] < length[min])
                min = i;
        }
    }
    return Finished_count < 7 ? min : -1;
}
int Search_classroom_for_stoling(std::vector<int>Current_node)
{
    int min = 0;
    int length[10];
    int Finished_count = 0;
    for (int i = 0; i < classroom_count; i++)
    {
        length[i] = abs(Current_node[0] - classroom[i].X) + abs(Current_node[1] - classroom[i].Y);
    }
    for (int k = 0; k < classroom_count; k++)
    {
        if (classroom[k].StudyRate == 10000000)
        {
            if (min == k)
            {
                min++;
            }
            Finished_count++;
        }
    }
    for (int i = 0; i < 10; i++)
    {
        if (classroom[i].StudyRate < 10000000)
        {
            if (length[i] < length[min])
                min = i;
        }
    }
    return Finished_count < 7 ? min : -1;
}
int Search_Grass_for_stoling(int tar_L)
{
    int length[150];
    int min = 0;
    for (int i = 0; i < grass_count; i++)
    {
        length[i] = abs(grass[i].X - classroom[tar_L].X) + abs(grass[i].Y - classroom[tar_L].Y);
    }
    for (int i = 0; i < grass_count; i++)
    {
        if (length[i] < length[min])
        {
            min = i;
        }
    }
    return min;
}
/*  位置搜索   */

/*  自主学习   */
bool ClassroomSearching = false;
bool ClassArrive = false;
bool GoStudying(IStudentAPI& api)
{
    std::vector<int> Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000,Current_point[1] / 1000 };
    if (!ClassroomSearching)
    {
        tar_L = Fast_Search_classroom(tar_L, Current_node);
        if (tar_L == -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != api.GetSelfInfo()->playerID)
                {
                    api.SendTextMessage(i, "F");
                }
            }
            return true;
        }
        ClassroomSearching = true;
    }
    if (!ClassArrive)
    {
        if (Move_to(classroom[tar_L].GetNode(), api))
        {
            ClassArrive = true;
        }
    }
    if (ClassArrive && abs(Current_node[0] - classroom[tar_L].X) <= 1 && abs(Current_node[1] - classroom[tar_L].Y) <= 1)
    {
        if (api.GetSelfInfo()->studentType == THUAI6::StudentType::StraightAStudent)
        {
            if (!api.GetSelfInfo()->timeUntilSkillAvailable[0])
            {
                api.UseSkill(0);
            }
        }
        api.StartLearning();
        classroom[tar_L].StudyRate = api.GetClassroomProgress(classroom[tar_L].X, classroom[tar_L].Y);
    }
    if (classroom[tar_L].StudyRate == 10000000)
    {
        for (int i = 0; i < 4; i++)
        {
            if (i != api.GetSelfInfo()->playerID)
            {
                api.SendTextMessage(i, ClassroomMessage(tar_L));
            }
        }
    }
    if (classroom[tar_L].StudyRate == 10000000)
    {
        ClassroomSearching = false;
        AStar.Clear();
        AStarRun = false;
        ClassArrive = false;
    }
    return false;
}
/*  自主学习   */

/*  强行毕业   */
bool GateArrive = false;
void GoGraduate(IStudentAPI& api)
{
    std::vector<int>Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000, Current_point[1] / 1000 };
    tar_G = Search_gate(Current_node);
    if (!GateArrive)
    {
        if (Move_to(gate[tar_G].GetNode(), api))
        {
            GateArrive = true;
        }
    }
    else if (GateArrive && abs(Current_node[0] - gate[tar_G].X) <= 1 && abs(Current_node[1] - gate[tar_G].Y) <= 1)
    {
        gate[tar_G].OpenRate = api.GetGateProgress(gate[tar_G].X, gate[tar_G].Y);
        api.StartOpenGate();
        if (gate[tar_G].OpenRate == 18000)
        {
            api.Graduate();
        }
    }
}
/*  强行毕业   */

/*  偷偷学习   */
int Section1 = 0;
bool StolingSearch = false;
int tar_S;
bool ArriveGrass_for_stoling = false;
bool ArriveClass_for_stoling = false;
int waiting_TIME = 0;
std::vector<int>Class_Target = { 0,0 };
std::vector<int>Grass_Target = { 0,0 };
bool GrassPerArrive = false;
bool tick;
bool GoStoling(IStudentAPI& api)
{
    std::vector<int>Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000, Current_point[1] / 1000 };
    /*if (!StolingSearch)
    {
        tar_L = Search_classroom_for_stoling(Current_node);
        if (tar_L == -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (i != api.GetSelfInfo()->playerID)
                {
                    api.SendTextMessage(i, "F");
                }
            }
            return true;
        }
        switch (tar_L)
        {
        case 1:Grass_Target[0] = 10 ; Grass_Target[1] = 45; break;
        case 2:Grass_Target[0] = 20 ; Grass_Target[1] = 2 ; break;
        case 3:Grass_Target[0] = 20 ; Grass_Target[1] = 37 ; break;
        case 6:Grass_Target[0] = 30; Grass_Target[1] = 3; break;
        case 8:Grass_Target[0] = 41; Grass_Target[1] = 10; break;
        case 9:Grass_Target[0] = 40; Grass_Target[1] = 25; break;
        }
        ClassroomSearching = true;
    }*/
    if (IsMap1)
    {
        Class_Target[0] = classroom[tar_L].X;
        Class_Target[1] = classroom[tar_L].Y;
        switch (tar_L)
        {
        case 0:Grass_Target[0] = 5; Grass_Target[1] = 34; break;
        case 1:Grass_Target[0] = 5; Grass_Target[1] = 34; break;
        case 2:Grass_Target[0] = 20; Grass_Target[1] = 2; break;
        case 3:Grass_Target[0] = 20; Grass_Target[1] = 37; break;
        case 5:Grass_Target[0] = 40; Grass_Target[1] = 24; break;
        case 6:Grass_Target[0] = 30; Grass_Target[1] = 3; break;
        case 7:Grass_Target[0] = 25; Grass_Target[1] = 44; break;
        case 8:Grass_Target[0] = 41; Grass_Target[1] = 10; break;
        case 9:Grass_Target[0] = 39; Grass_Target[1] = 35; break;
        }
    }
    else
    {
        Class_Target[0] = classroom[tar_L].X;
        Class_Target[1] = classroom[tar_L].Y;
        switch (tar_L)
        {
        case 0:Grass_Target[0] = 4; Grass_Target[1] = 13; break;
        case 1:Grass_Target[0] = 4; Grass_Target[1] = 36; break;
        case 2:Grass_Target[0] = 13; Grass_Target[1] = 45; break;
        case 3:Grass_Target[0] = 13; Grass_Target[1] = 16; break;
        case 6:Grass_Target[0] = 36; Grass_Target[1] = 33; break;
        case 7:Grass_Target[0] = 36; Grass_Target[1] = 4; break;
        case 8:Grass_Target[0] = 45; Grass_Target[1] = 13; break;
        case 9:Grass_Target[0] = 45; Grass_Target[1] = 36; break;
        }
    }
    if (!ArriveGrass_for_stoling)
    {
        if (!GrassPerArrive)
        {
            if (Move_to(Grass_Target, api))
            {
                GrassPerArrive = true;
            }
        }
        if (GrassPerArrive)
        {
            if (Move_towards_node(Grass_Target, api))
            {
                ArriveGrass_for_stoling = true;
                ArriveClass_for_stoling = false;
                GrassPerArrive = false;
            }
        }
    }
    else if (ArriveGrass_for_stoling)
    {
        waiting_TIME++;
    }
    //printf("%d", waiting_TIME);
    if (waiting_TIME >= 4700)
    {
        if (api.GetTrickers().empty())
        {
            tick = true;
        }
        if (tick)
        {
            if (!ArriveClass_for_stoling)
            {
                if (Move_to(Class_Target, api))
                {
                    ArriveClass_for_stoling = true;
                }
            }
            if (ArriveClass_for_stoling && abs(Current_node[0] - classroom[tar_L].X) <= 1 && abs(Current_node[1] - classroom[tar_L].Y) <= 1)
            {
                api.UseSkill(0);
                api.StartLearning();
                classroom[tar_L].StudyRate = api.GetClassroomProgress(classroom[tar_L].X, classroom[tar_L].Y);
            }
            if (classroom[tar_L].StudyRate == 10000000)
            {
                tick = false;
                ArriveGrass_for_stoling = false;
                waiting_TIME = 0;
                Section1++;
                for (int i = 0; i < 4; i++)
                {
                    if (i != api.GetSelfInfo()->playerID)
                    {
                        api.SendTextMessage(i, ClassroomMessage(tar_L));
                    }
                }
                if (IsMap1)
                {
                    if (Section1 <= 1)
                    {
                        switch ((int)api.GetSelfInfo()->playerID)
                        {
                        case 0:tar_L = 2; break;
                        case 1:tar_L = 7; break;
                        case 2:tar_L = 1; break;
                        case 3:tar_L = 5; break;
                        }
                    }
                    else
                    {
                        Situation = 0;
                        LastSituation = 0;
                    }
                }
                else
                {
                    if (Section1 <= 1)
                    {
                        switch ((int)api.GetSelfInfo()->playerID)
                        {
                        case 0:tar_L = 3; break;
                        case 1:tar_L = 1; break;
                        case 2:tar_L = 8; break;
                        case 3:tar_L = 6; break;
                        }
                    }
                    else
                    {
                        Situation = 0;
                        LastSituation = 0;
                    }
                }
            }
        }
    }
    return false;
}
/*  偷偷学习   */

/*  躲草  */
bool SearchingGrass = false;
bool GrassArrive = false;
std::vector<int>HiddenGrass = { 0,0 };
bool Gohidden(IStudentAPI& api)
{
    std::vector<int>Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000, Current_point[1] / 1000 };
    if (!SearchingGrass)
    {
        tar_G = Search_grass(Current_node, api);
        HiddenGrass[0] = grass[tar_G].X;
        HiddenGrass[1] = grass[tar_G].Y;
        SearchingGrass = true;
    }
    if (!GrassArrive)
    {
        if (Move_to(HiddenGrass, api))
        {
            GrassArrive = true;
        }
    }
    if (GrassArrive)
    {
        if (Move_towards_node(HiddenGrass, api))
        {
            return true;
        }
    }
    return false;
}
/*  躲草  */

/* 消息处理 */
void ReadMessage(IStudentAPI& api)
{
    while (api.HaveMessage())
        for (int i = 1; i <= 3; i++)
        {
            std::pair<int, std::string> message;
            message = api.GetMessage();
            //printf("Get Message from %d\nmessage is %s\n", message.first, message.second.c_str());
            if (message.second[0] == 'C')
            {
                classroom[DClassroomMessage(message.second.c_str())].StudyRate = 10000000;
            }
            if (message.second[0] == 'F')
            {
                Situation = 1;
            }
        }
}
/* 消息处理 */

/*Safe map*/
void SafeMap()
{
    for (int i = 0; i < 50; i++)
        for (int j = 0; j < 50; j++)
            if (FullMap[i][j] == 4)
            {
                map[i][j] = 0;
                map_copy[i][j] = 0;
            }
}
void StudyMap()
{
    for (int i = 0; i < 50; i++)
        for (int j = 0; j < 50; j++)
            if (FullMap[i][j] == 4)
            {
                map[i][j] = 2;
                map_copy[i][j] = 2;
            }
}
/*Safe map*/

/*  SafeJudge  */
bool FirstAlert = false;
bool SafeSituation = true;
int SafeTime = 0;
int AlarmTime = 0;
double LastAlarm = 0;
bool SafeJudge(IStudentAPI& api)
{
    if (api.GetSelfInfo()->dangerAlert)
    {
        if (api.GetSelfInfo()->dangerAlert > LastAlarm)
        {
            AlarmTime++;
        }
    }
    else
    {
        AlarmTime = 0;
    }
    if (!api.GetTrickers().empty() || AlarmTime >= 20)
    {
        SafeSituation = false;
        SafeTime = 0;
        if (!api.GetTrickers().empty())
        {
            auto Trickersinfo = api.GetTrickers()[0];
            Tricker_Node[0] = Trickersinfo->x;
            Tricker_Node[1] = Trickersinfo->y;
        }
        return false;
    }
    else if (!SafeSituation)
    {
        SafeTime++;
        if (SafeTime >= 100)
        {
            SafeSituation = true;
            SafeTime = 0;
            AStarRun = false;
            AStar.Clear();
            AStar.Reshape();
            return true;
        }
        return false;
    }
    return true;
}
/*  SafeJudge  */

bool StartGraduate = true;
bool IsStudyMap = true;
bool GrassPerArriveH = false;
int DangerTimes = 0;
void AI::play(IStudentAPI& api)
{
    if (StartFrame)
    {
        Start(api);
        Situation = 2;
        LastSituation = 2;
        StolingSearch = true;
        if (IsMap1)
        {
            switch ((int)api.GetSelfInfo()->playerID)
            {
            case 0:tar_L = 6; break;
            case 1:tar_L = 9; break;
            case 2:tar_L = 0; break;
            case 3:tar_L = 8; break;
            }
        }
        else
        {
            switch ((int)api.GetSelfInfo()->playerID)
            {
            case 0:tar_L = 0; break;
            case 1:tar_L = 2; break;
            case 2:tar_L = 7; break;
            case 3:tar_L = 9; break;
            }
        }
    }
    /*if (SafeJudge(api))
    {
        Situation = LastSituation;
        bool GrassPerArriveH = false;
        if (!IsStudyMap)
        {
            StudyMap();
            IsStudyMap = true;
        }
        if (!FirstAlert)
        {
            FirstAlert = true;
        }
    }
    else
    {
        Situation = 3;
        if (FirstAlert)
        {
            AStarRun = false;
            AStar.Clear();
            AStar.Reshape();
            FirstAlert = false;
        }
    }*/
    std::vector<int>Current_point = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
    std::vector<int> Current_node = { Current_point[0] / 1000, Current_point[1] / 1000 };
    //printf("Current_node (%d,%d)", Current_node[0], Current_node[1]);
    //printf("Situation:%d    tar_L:%d", Situation, tar_L);
    if (Situation == 2)
    {
        GoStoling(api);
    }
    else if (Situation == 0)
    {
        if (GoStudying(api))
        {
            Situation = 1;
            LastSituation = 1;
        }
        //printf("Try studying");
    }
    else if (Situation == 1)
    {
        if (!StartGraduate)
        {
            AStar.Clear();
            AStar.Reshape();
            AStarRun = false;
            SafeMap();
            StartGraduate = true;
        }
        GoGraduate(api);
        //printf("Try graduating");
    }
    /*else if (Situation == 3)
    {
        if (LastSituation == 2)
        {
            if (IsMap1)
            {
                switch (tar_L)
                {
                case 0:Grass_Target[0] = 5; Grass_Target[1] = 34; break;
                case 1:Grass_Target[0] = 5; Grass_Target[1] = 34; break;
                case 2:Grass_Target[0] = 20; Grass_Target[1] = 2; break;
                case 3:Grass_Target[0] = 20; Grass_Target[1] = 37; break;
                case 5:Grass_Target[0] = 40; Grass_Target[1] = 24; break;
                case 6:Grass_Target[0] = 30; Grass_Target[1] = 3; break;
                case 7:Grass_Target[0] = 25; Grass_Target[1] = 44; break;
                case 8:Grass_Target[0] = 41; Grass_Target[1] = 10; break;
                case 9:Grass_Target[0] = 39; Grass_Target[1] = 35; break;
                }
            }
            else
            {
                Class_Target[0] = classroom[tar_L].X;
                Class_Target[1] = classroom[tar_L].Y;
                switch (tar_L)
                {
                case 0:Grass_Target[0] = 4; Grass_Target[1] = 13; break;
                case 1:Grass_Target[0] = 4; Grass_Target[1] = 36; break;
                case 2:Grass_Target[0] = 13; Grass_Target[1] = 45; break;
                case 3:Grass_Target[0] = 13; Grass_Target[1] = 16; break;
                case 6:Grass_Target[0] = 36; Grass_Target[1] = 33; break;
                case 7:Grass_Target[0] = 36; Grass_Target[1] = 4; break;
                case 8:Grass_Target[0] = 45; Grass_Target[1] = 13; break;
                case 9:Grass_Target[0] = 45; Grass_Target[1] = 36; break;
                }
            }
            if (!GrassPerArriveH)
            {
                if (Move_to(Grass_Target, api))
                {
                    GrassPerArriveH = true;
                }
            }
            if (GrassPerArriveH)
            {
                Move_towards_node(Grass_Target, api);
            }
        }
        else if (FullMap[Current_node[0]][Current_node[1]] == 3 && abs(Current_point[0] - 1000 * Current_node[0] - 500) < 100 && abs(Current_point[1] - 1000 * Current_node[1] - 500))
        {
            api.EndAllAction();
        }
        else
        {
            ClassroomSearching = false;
            ClassArrive = false;
            if (IsStudyMap)
            {
                SafeMap();
                IsStudyMap = false;
            }
            Gohidden(api);
        }
    }*/
    ReadMessage(api);
    if (this->playerID == 0)
    {

    }
    else if (this->playerID == 1)
    {

    }
    else if (this->playerID == 2)
    {

    }
    else if (this->playerID == 3)
    {

    }
}

///////////////////////////////////////////////////////////////////////////////////////Trickerrrrrrrrrrrrrrrrrrrrrr
struct grass
{
    int x;
    int y;
    bool flag;
} mygrass[150];
int grass_number = 0;
bool tricker_firstframe;
bool tricker_ID;
bool tricker_ID1;
std::list<Node*> tricker_path;
int tricker_aim_xy = 0;
int tri_last_x = 0;
int tri_last_y = 0;
int time_stop = 0;
int time_stop_1 = 0;
struct tricker_classroom
{
    int x;
    int y;
    bool cla_flag;
};
tricker_classroom cla_tricker[15];
void Start_Tricker(ITrickerAPI& api)
{
    tri_last_x = api.GridToCell(api.GetSelfInfo()->x);
    tri_last_y = api.GridToCell(api.GetSelfInfo()->y);
    time_stop = 0;
    api.MoveUp(50);
    int i, j;
    int cla_number = 0;
    int gate_number = 0;
    auto mymap = api.GetFullMap();
    tricker_firstframe = true;
    cla_tricker[14].x = 1000;
    cla_tricker[14].y = 1000;
    for (i = 0; i < 50; i++)
        for (j = 0; j < 50; j++)
        {
            if (mymap[i][j] == THUAI6::PlaceType::Wall)
                map[i][j] = 2;
            if (mymap[i][j] == THUAI6::PlaceType::Grass)
            {
                map[i][j] = 3;
                mygrass[grass_number].x = i;
                mygrass[grass_number].y = j;
                mygrass[grass_number].flag = true;
                grass_number++;
            }
            if (mymap[i][j] == THUAI6::PlaceType::Window)
                map[i][j] = 7;
            if (mymap[i][j] == THUAI6::PlaceType::ClassRoom)
            {
                map[i][j] = 2;
                cla_tricker[cla_number].x = i;
                cla_tricker[cla_number].y = j;
                cla_tricker[cla_number].cla_flag = true;
                cla_number++;
            }
            if (mymap[i][j] == THUAI6::PlaceType::Gate)
            {
                map[i][j] = 2;
            }



        }
    for (i = cla_number; i < 15; i++)
    {
        cla_tricker[cla_number].cla_flag = false;
    }
}
void tricker_move_path(std::list<Node*>& tricker_path, ITrickerAPI& api)
{
    //OutPut(tricker_path);
    if (!tricker_path.empty())
    {
        if (map[tricker_path.front()->x][tricker_path.front()->y] == 7)
        {
            api.SkipWindow();
            tricker_path.pop_front();

        }
        //printf("moving!!!\n");
        if (api.GetSelfInfo()->x > (api.CellToGrid(tricker_path.front()->x) + 100))
        {
            if (api.GetSelfInfo()->y > (api.CellToGrid(tricker_path.front()->y) + 100))
            {
                //printf("左上");
                api.Move(50, -1.57079632 - atan2(abs(api.GetSelfInfo()->x - api.CellToGrid(tricker_path.front()->x)), abs(api.GetSelfInfo()->y - api.CellToGrid(tricker_path.front()->y))));
            }
            if (api.GetSelfInfo()->y < (api.CellToGrid(tricker_path.front()->y) - 100))
            {
                //printf("右上");
                api.Move(50, 1.57079632 + atan2(abs(api.GetSelfInfo()->x - api.CellToGrid(tricker_path.front()->x)), abs(api.GetSelfInfo()->y - api.CellToGrid(tricker_path.front()->y))));
            }
            if ((api.GetSelfInfo()->y >= (api.CellToGrid(tricker_path.front()->y) - 100)) && (api.GetSelfInfo()->y <= (api.CellToGrid(tricker_path.front()->y) + 100)))
            {
                // printf("上");
                api.MoveUp(50);
            }
        }
        else if (api.GetSelfInfo()->x < (api.CellToGrid(tricker_path.front()->x) - 100))
        {
            if (api.GetSelfInfo()->y > (api.CellToGrid(tricker_path.front()->y) + 100))
            {
                // printf("左下");
                api.Move(50, -1.5707963 + atan2(abs(api.GetSelfInfo()->x - api.CellToGrid(tricker_path.front()->x)), abs(api.GetSelfInfo()->y - api.CellToGrid(tricker_path.front()->y))));
            }
            if (api.GetSelfInfo()->y < (api.CellToGrid(tricker_path.front()->y) - 100))
            {
                // printf("右下");
                api.Move(50, 1.5707963 - atan2(abs(api.GetSelfInfo()->x - api.CellToGrid(tricker_path.front()->x)), abs(api.GetSelfInfo()->y - api.CellToGrid(tricker_path.front()->y))));
            }
            if ((api.GetSelfInfo()->y >= (api.CellToGrid(tricker_path.front()->y) - 100)) && (api.GetSelfInfo()->y <= (api.CellToGrid(tricker_path.front()->y) + 100)))
            {
                //printf("下");
                api.MoveDown(50);
            }
        }
        else if ((api.GetSelfInfo()->x >= (api.CellToGrid(tricker_path.front()->x) - 100)) && (api.GetSelfInfo()->x <= (api.CellToGrid(tricker_path.front()->x) + 100)))
        {
            if (api.GetSelfInfo()->y > (api.CellToGrid(tricker_path.front()->y) + 100))
            {
                //printf("左");
                api.MoveLeft(50);
            }
            if (api.GetSelfInfo()->y < (api.CellToGrid(tricker_path.front()->y) - 100))
            {
                //printf("右");
                api.MoveRight(50);
            }
        }
        if ((abs(api.GetSelfInfo()->y - api.CellToGrid(tricker_path.front()->y)) <= 100) && (abs(api.GetSelfInfo()->x - api.CellToGrid(tricker_path.front()->x)) <= 100))
        {

            tricker_path.pop_front();
        }
    }
}
void tricker_moveto_xy(int x, int y, ITrickerAPI& api)
{
    std::vector<int>st;
    std::vector<int>ed;
    if (!tricker_ID)
    {
        tricker_ID = true;
        std::vector<int>st = { api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y) };
        std::vector<int>ed = { x,y };
        //OutPut(AStar.GetPath(st, ed));
        tricker_path = AStar.GetPath(st, ed);
    }
    if (tricker_ID)
    {
        tricker_move_path(tricker_path, api);

    }
}


void tricker_move_to_xy_in(int x, int y, ITrickerAPI& api)
{
    std::vector<int>st;
    std::vector<int>ed;
    if (!tricker_ID)
    {
        tricker_ID = true;
        std::vector<int>st = { api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y) };
        std::vector<int>ed = { x,y };
        //OutPut(AStar.GetPath(st, ed));
        tricker_path = AStar.GetPath(st, ed);
        tricker_path.push_back(new Node(ed[0], ed[1]));
    }
    if (tricker_ID)
    {
        tricker_move_path(tricker_path, api);
        int tri_current_x = api.GridToCell(api.GetSelfInfo()->x);
        int tri_current_y = api.GridToCell(api.GetSelfInfo()->y);
        if ((tri_last_x == tri_current_x) && (tri_current_y == tri_last_y))
        {
            time_stop++;
            time_stop_1++;
        }
        else
        {
            time_stop = 0;
        }
        tri_last_x = tri_current_x;
        tri_last_y = tri_current_y;

        if (time_stop > 20)
        {
            int a = tricker_path.front()->x;
            int b = tricker_path.front()->y;
            int map_type_stop = map[a][b];
            map[a][b] = 2;
            tricker_path.clear();
            AStar.update();
            std::vector<int>st = { api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y) };
            std::vector<int>ed = { x,y };
            tricker_path = AStar.GetPath(st, ed);
            tricker_path.push_back(new Node(ed[0], ed[1]));
            if (time_stop_1 >= 60)
            {

                if (tricker_path.size() > 1)
                    tricker_path.pop_front();
                time_stop_1 = 0;
            }
            map[a][b] = map_type_stop;
            time_stop = 0;
            //printf("\nlast:%d %d\ncurrent:%d %d\ntime:%d\ntime1:%d\n", tri_last_x, tri_last_y, tri_current_x, tri_current_y, time_stop,time_stop_1);
        }

    }
}
void tricker_moveto_xy1(int x, int y, ITrickerAPI& api)
{
    std::vector<int>st;
    std::vector<int>ed;
    if (!tricker_ID1)
    {
        tricker_ID1 = true;
        std::vector<int>st = { api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y) };
        std::vector<int>ed = { x,y };
        //OutPut(AStar1.GetPath(st, ed));
        tricker_path = AStar1.GetPath(st, ed);
    }
    if (tricker_ID1)
    {
        tricker_move_path(tricker_path, api);

    }
}
int flag = 0;
int student_x;
int student_y;
bool finish;
int flag_skill = 0;
double student_length;
bool last_see_student;
///////////////////////////////////////////////////////////////////////////////////////Trickerrrrrrrrrrrrrrrrrrrrrr
void AI::play(ITrickerAPI& api)
{
    //for (int i = 0; i < 150; i++)
    //{
      //  if(mygrass[i].flag==true)
        //printf("\n%d %d\n", mygrass[i].x, mygrass[i].y);
    //}
    api.EndAllAction();
    //student_length = 15300 / api.GetSelfInfo()->trickDesire;
    //printf("\n%lf\n", student_length);

    if (!tricker_firstframe)
        Start_Tricker(api);
    //tricker_move_to_xy_in(1,48, api);
    //printf("\n%d %d\n", api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y));
    int a[8] = { 15,7,16,28,41,39,24,7 };
    int b[8] = { 17,33,38,38,27,17,7,7 };
    int a1[10] = { 17,6,6,17,20,29,43,43,31,29 };
    int b1[10] = { 20,17,32,43,32,32,32,17,6,16 };
    if ((api.GetStudents().empty() == false) && (api.GetStudents()[0]->playerState == THUAI6::PlayerState::Addicted))
    {
        map[api.GridToCell(api.GetStudents()[0]->x)][api.GridToCell(api.GetStudents()[0]->y)] = 2;
    }
    if ((api.GetSelfInfo()->timeUntilSkillAvailable[0] == 0) && (api.GetStudents().empty() == false) && (api.GetStudents()[0]->playerState != THUAI6::PlayerState::Addicted))
    {
        api.UseSkill(0);
    }
    if ((api.GetStudents().empty() == false) && (api.GetStudents()[0]->playerState != THUAI6::PlayerState::Addicted))
    {
        last_see_student = true;
        int student_x_grid = api.GetStudents()[0]->x;
        int student_y_grid = api.GetStudents()[0]->y;
        int xxx = abs(-api.GetSelfInfo()->x + student_x_grid);
        int yyy = abs(-api.GetSelfInfo()->y + student_y_grid);

        if ((abs(xxx) < 1800) && (abs(yyy) < 1800) && (api.HaveView(student_x_grid - 142, student_y_grid - 142) == true) && (api.HaveView(student_x_grid - 142, student_y_grid + 142) == true) && (api.HaveView(student_x_grid + 142, student_y_grid - 142) == true) && (api.HaveView(student_x_grid + 142, student_y_grid + 142) == true))
        {
            //api.GetStudents()[0]->
            double attack_angle = atan2(xxx, yyy);
            if (api.GetSelfInfo()->x > student_x_grid)
            {
                if (api.GetSelfInfo()->y > student_y_grid)
                    api.Attack(-1.5707963 - attack_angle);
                if (api.GetSelfInfo()->y < student_y_grid)
                    api.Attack(1.5707963 + attack_angle);
                if (api.GetSelfInfo()->y == student_y_grid)
                    api.Attack(-3.1415926);
            }
            if (api.GetSelfInfo()->x < student_x_grid)
            {
                if (api.GetSelfInfo()->y > student_y_grid)
                    api.Attack(-1.5707963 + attack_angle);
                if (api.GetSelfInfo()->y < student_y_grid)
                    api.Attack(1.5707963 - attack_angle);
                if (api.GetSelfInfo()->y == student_y_grid)
                    api.Attack(0);
            }
            if (api.GetSelfInfo()->x == student_x_grid)
            {
                if (api.GetSelfInfo()->y > student_y_grid)
                    api.Attack(-1.5707963);
                if (api.GetSelfInfo()->y < student_y_grid)
                    api.Attack(1.5707963);
            }
            //printf("attack\n");
        }

        flag = 1;
        if (!finish)
        {
            student_x = api.GridToCell(api.GetStudents()[0]->x);
            student_y = api.GridToCell(api.GetStudents()[0]->y);
            finish = true;
            tricker_ID = false;
            AStar.update();
            tricker_path.clear();
        }
        //printf("falsefalsefalse\n");
    }
    if ((flag == 0) && (map[2][7] != 2))
    {
        //printf("truetruetrue\n%d\n", tricker_aim_xy);
        tricker_move_to_xy_in(a[tricker_aim_xy], b[tricker_aim_xy], api);
        if ((abs(api.GetSelfInfo()->x - api.CellToGrid(a[tricker_aim_xy])) <= 100) && (abs(api.GetSelfInfo()->y - api.CellToGrid(b[tricker_aim_xy]))) <= 100)
        {
            tricker_aim_xy++;
            if (tricker_aim_xy == 8)
                tricker_aim_xy = 0;
            tricker_ID = false;
            AStar.update();
            tricker_path.clear();
        }
    }
    if ((flag == 0) && (map[2][7] == 2))
    {
        //printf("truetruetrue\n%d\n", tricker_aim_xy);
        tricker_move_to_xy_in(a1[tricker_aim_xy], b1[tricker_aim_xy], api);
        if ((abs(api.GetSelfInfo()->x - api.CellToGrid(a1[tricker_aim_xy])) <= 100) && (abs(api.GetSelfInfo()->y - api.CellToGrid(b1[tricker_aim_xy]))) <= 100)
        {
            tricker_aim_xy++;
            if (tricker_aim_xy == 10)
                tricker_aim_xy = 0;
            tricker_ID = false;
            AStar.update();
            tricker_path.clear();
        }
    }

    if (flag == 1)
    {
        //printf("%d %d", student_x, student_y);
        tricker_move_to_xy_in(student_x, student_y, api);
        if ((abs(api.GetSelfInfo()->x - api.CellToGrid(student_x)) < 1100) && (abs(api.GetSelfInfo()->y - api.CellToGrid(student_y))) < 1100)
        {
            // printf("finish\n");
            tricker_ID = false;
            AStar.update();
            tricker_path.clear();
            flag = 0;
            finish = false;
        }
    }






}




