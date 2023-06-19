#include <vector>
#include <list>
#include <math.h>
#include <string>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"

constexpr int deltaTimems = 50;
constexpr int LastTrickerNum = 10;
#define deltaTime 0.05
#define pi 3.1415926

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
};
extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;

/*
* 全局常量：
*
* 全局变量：
*
* Debug相关
* 寻路相关
* 游戏相关
*
* 学生变量：
* 通信相关
* 状态相关
* 游戏相关
*
* StraightAStudent变量：
* 冥想相关
* Athlete变量：
*
* Teacher变量：
*
* Sunshine变量：
*
*
* 捣蛋鬼变量：
*
*
* Assas变量：
*
* 工具类
* A*类
*
*/
struct Node {
    int x, y;
    int F, G, H;
    Node* parent;
    Node(int _x, int _y):x(_x), y(_y), F(0), G(0), H(0), parent(NULL) {}
}*node[50][50];
const std::list<Node*> emptylist;
const int EscapeDist = 16000;
std::list<Node*> PathToGo, PathGone;
bool isFirstFrame = true;
int PlayerId;
int Gridx, Gridy;
int Cellx, Celly;
std::vector<std::vector<THUAI6::PlaceType>> Map;
int map[50][50];//0为无阻碍，1为半阻碍，2为完全阻碍, 3为临时避障
struct ClassRoomInfo {
    int x, y;
    bool finished;
    int occupied;//为4说明无学生占用，为0-3说明相应编号学生正在占用
}Classroom[100];
int ClassroomCount, ClassroomFinished;
struct ChestInfo {
    int x, y;
    bool opened;
    int occupied;
}Chest[80];
int ChestCount;
struct GateInfo {
    int x, y;
    bool opening;
    bool open;
}Gate[20];
int GateCount;
struct HiddenGateInfo {
    int x, y;
    bool refreshed;
    bool open;
}HiddenGate[30];
int HiddenGateCount;
struct GrassInfo {
    int x, y, occupied;
}Grass[200];
int GrassCount;
double faceDir = 0;
bool map1 = false, map2 = false;
std::pair<int, int> LastTarget;

std::pair<int, std::string> msgrcv;
std::string msg;//信息结构：'准备前往*号教室''1/0完成了作业''*号教室作业已完成''需要被营救'"捣蛋鬼x坐标""捣蛋鬼y坐标"
std::shared_ptr<const THUAI6::Student> studentinfo;
THUAI6::Tricker Trickers,LastTrickers[LastTrickerNum];
bool TrickerisInSight;
bool CanGetTrickerInfo;
enum class StudentState {
    MovingForClassroom, Learning, MovingForGrass, MovingToAttck, MovingForProp, MovingToRescue, MovingToGraduate, WaitToGraduate
}studentstate;
int CurrentClassroomToGo;
int CurrentGateToGo;
int CurrentStudentToRescue;
int CurrentGrassToGo;
bool ShouldChangeClassroomToGo;
float Courage = 1.0;
int TrickerStuckTime;
bool rousing = false;
int framecount;

int MeditationProgress;

double ChargeTimeLeft;

std::shared_ptr<const THUAI6::Tricker> trickerinfo;
THUAI6::Student Students[4];
bool StudentisInSight[4];
enum class TrickerState {
    MovingToPatrol, MovingToPursue, MovingToSearch, MovingForProp
}trickerstate;
struct _PatrolNode {
    int x, y;
};
std::vector<_PatrolNode> PatrolNode;
int CurrentPatrolNodeToGo;
int CurrentStudentToPursue;
int StuckTime;
int WaitTrickerTime;
bool bgmuseful;


class Tools {
public:
    double max(double a, double b) {
        return a > b ? a : b;
    }
    double min(double a, double b) {
        return a < b ? a : b;
    }
    double Dist(double x1, double y1, double x2, double y2) {
        return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }
    int ToInt(std::string s) {
        int x = 0;
        for(int i = 0;i < 5;i++, x *= 10)
            x += s[i] - '0';
        return x / 10;
    }
    std::string ToString(int x) {
        std::string s = "*****";
        for(int i = 0;i < 5;i++, x /= 10)
            s[4 - i] = x % 10 + '0';
        return s;
    }
    double StuDistValSet(double Dist) {
        if(Dist < 7500)
            return Dist;
        return (Dist - 7500) * 2 + 7500;
    }
    double TriDistValSet(double Dist) {
        int dist1 = 8500, dist2 = 10500;
        if(Dist < dist1)
            return Dist * 6;
        if(Dist >= dist1 && Dist <= dist2)
            return (Dist - dist1) * 3 + dist1 * 6;
        return (Dist - dist2) + dist1 * 4 + (dist2 - dist1) * 2;
    }
}T;
class A_Star {
public:
    void init() {
        openlist.clear();
        closelist.clear();
    }
    std::list<Node*> GetPath(std::vector<int> st, std::vector<int> ed, bool Reach) {
        if(st == ed) {
            std::list<Node*> simplelist;
            simplelist.push_back(new Node(st[0], st[1]));
            return simplelist;
        }
        openlist.push_back(new Node(st[0], st[1]));
        while(!openlist.empty()) {
            Node* CurrentUpdateNode = GetLeastF();
            closelist.push_back(CurrentUpdateNode);
            UpdateFValue(CurrentUpdateNode, ed);
            Node* CurPathNode;
            if(!Reach) {
                CurPathNode = GetDestSurroundNode(ed);
                if(CurPathNode) {
                    std::list<Node*> path;
                    while(CurPathNode != NULL) {
                        path.push_front(CurPathNode);
                        CurPathNode = CurPathNode->parent;
                    }
                    return path;
                }
            } else {
                CurPathNode = isInList(openlist, ed[0], ed[1]);
                if(CurPathNode != NULL) {
                    std::list<Node*> path;
                    while(CurPathNode != NULL) {
                        path.push_front(CurPathNode);
                        CurPathNode = CurPathNode->parent;
                    }
                    return path;
                }
            }
            openlist.remove(CurrentUpdateNode);
        }
        return emptylist;
    }
private:
    void UpdateFValue(Node* CurNode, std::vector<int> ed) {
        std::list<std::vector<int>> UpdateList = GetSurroundNode(CurNode);
        for(auto Curnum = UpdateList.begin();Curnum != UpdateList.end();Curnum++) {
            int i = (*Curnum)[0], j = (*Curnum)[1];
            if(isInList(openlist, i, j) == NULL) {
                Node* UpdateNode = new Node(i, j);
                UpdateNode->parent = CurNode;
                if(map[i][j] == 1) {
                    if(PlayerId <= 3 && studentType[PlayerId] == THUAI6::StudentType::StraightAStudent) {
                        if(MeditationProgress + CurNode->G / 28.8 * 40000 > 300000)
                            UpdateNode->G = CurNode->G + 1;
                        else
                            UpdateNode->G = CurNode->G + 100;
                    } else
                        UpdateNode->G = CurNode->G + 1 * (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                } else
                    UpdateNode->G = CurNode->G + (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                UpdateNode->H = (abs(ed[0] - i) + abs(ed[1] - j)) * 10;
                UpdateNode->F = UpdateNode->G + UpdateNode->H;
                openlist.push_back(UpdateNode);
            } else {
                Node* UpdateNode = isInList(openlist, i, j);
                int newG = (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                if(CurNode->G + newG <= UpdateNode->G) {
                    UpdateNode->parent = CurNode;
                    UpdateNode->G = CurNode->G + (int)sqrt(100 * ((i - CurNode->x) * (i - CurNode->x) + (j - CurNode->y) * (j - CurNode->y)));
                    UpdateNode->F = UpdateNode->G + UpdateNode->H;
                }
            }
        }
    }
    std::list<std::vector<int>> GetSurroundNode(Node* CurNode) {
        std::list<std::vector<int>> SurroundNodeList;
        bool up = false, down = false, left = false, right = false;
        if(CurNode->x > 0 && map[CurNode->x - 1][CurNode->y] < 2 && isInList(closelist, CurNode->x - 1, CurNode->y) == NULL)
            SurroundNodeList.push_back({CurNode->x - 1,CurNode->y}), up = true;
        if(CurNode->x < 49 && map[CurNode->x + 1][CurNode->y] < 2 && isInList(closelist, CurNode->x + 1, CurNode->y) == NULL)
            SurroundNodeList.push_back({CurNode->x + 1,CurNode->y}), down = true;
        if(CurNode->y > 0 && map[CurNode->x][CurNode->y - 1] < 2 && isInList(closelist, CurNode->x, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({CurNode->x,CurNode->y - 1}), left = true;
        if(CurNode->y < 49 && map[CurNode->x][CurNode->y + 1] < 2 && isInList(closelist, CurNode->x, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({CurNode->x,CurNode->y + 1}), right = true;
        if(up && left && map[CurNode->x - 1][CurNode->y - 1] < 2 && isInList(closelist, CurNode->x - 1, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({CurNode->x - 1,CurNode->y - 1});
        if(up && right && map[CurNode->x - 1][CurNode->y + 1] < 2 && isInList(closelist, CurNode->x - 1, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({CurNode->x - 1,CurNode->y + 1});
        if(down && left && map[CurNode->x + 1][CurNode->y - 1] < 2 && isInList(closelist, CurNode->x + 1, CurNode->y - 1) == NULL)
            SurroundNodeList.push_back({CurNode->x + 1,CurNode->y - 1});
        if(down && right && map[CurNode->x + 1][CurNode->y + 1] < 2 && isInList(closelist, CurNode->x + 1, CurNode->y + 1) == NULL)
            SurroundNodeList.push_back({CurNode->x + 1,CurNode->y + 1});
        return SurroundNodeList;
    }
    Node* isInList(std::list<Node*>& List, int x, int y) {
        for(auto i = List.begin();i != List.end();i++)
            if((*i)->x == x && (*i)->y == y)
                return (*i);
        return NULL;
    }
    Node* GetDestSurroundNode(std::vector<int> ed) {
        Node* retNode;
        retNode = isInList(openlist, ed[0] - 1, ed[1]);
        if(retNode) return retNode;
        retNode = isInList(openlist, ed[0] + 1, ed[1]);
        if(retNode) return retNode;
        retNode = isInList(openlist, ed[0], ed[1] - 1);
        if(retNode) return retNode;
        retNode = isInList(openlist, ed[0], ed[1] + 1);
        if(retNode) return retNode;
        return NULL;
    }
    Node* GetLeastF() {
        if(!openlist.empty()) {
            Node* resPoint = openlist.front();
            for(auto i = openlist.begin();i != openlist.end();i++)
                if(resPoint->F > (*i)->F)
                    resPoint = *i;
            return resPoint;
        }
        return NULL;
    }
private:
    std::list<Node*> openlist;
    std::list<Node*> closelist;
}AStar;
int SearchBestClassroom(IAPI& api) {
    double valmin = 10000000;
    int nearestId = -1;
    for(int i = 0;i < ClassroomCount;i++) {
        if(!Classroom[i].finished && Classroom[i].occupied == 4) {
            double val = T.Dist(api.CellToGrid(Classroom[i].x), api.CellToGrid(Classroom[i].y), Gridx, Gridy)
                - 0.5 * T.Dist(Trickers.x, Trickers.y, api.CellToGrid(Classroom[i].x), api.CellToGrid(Classroom[i].y));
            if(val < valmin) {
                valmin = val;
                nearestId = i;
            }
        }
    }
    return nearestId;
}
int SearchBestGate(IAPI& api) {
    double valmin = 10000000;
    int nearestId = -1;
    for(int i = 0;i < GateCount;i++) {
        double val = T.Dist(api.CellToGrid(Gate[i].x), api.CellToGrid(Gate[i].y), Gridx, Gridy)
            - 0.5 * T.Dist(Trickers.x, Trickers.y, api.CellToGrid(Gate[i].x), api.CellToGrid(Gate[i].y));
        if(val < valmin) {
            valmin = val;
            nearestId = i;
        }
    }
    return nearestId;
}
int StudentSearchBestGrass(IAPI& api) {
    double valmin = 10000000;
    int nearestId = -1;
    /*api.Print(fmt::format("Grid{},{}", Gridx, Gridy));
    api.Print(fmt::format("Tri{},{}", Trickers.x, Trickers.y));*/
    for(int i = 0;i < GrassCount;i++) {
        double val = T.StuDistValSet(T.Dist(api.CellToGrid(Grass[i].x), api.CellToGrid(Grass[i].y), Gridx, Gridy));
        /*api.Print(fmt::format("Grass{},{}:val{}", Grass[i].x, Grass[i].y, val));
        api.Print(fmt::format("Dist(stu,Grass)={}", T.Dist(api.CellToGrid(Grass[i].x), api.CellToGrid(Grass[i].y), Gridx, Gridy)));
        api.Print(fmt::format("Dist(tri,Grass)={}", T.Dist(Trickers.x, Trickers.y, api.CellToGrid(Grass[i].x), api.CellToGrid(Grass[i].y))));*/
        if(val < valmin && !Grass[i].occupied) {
            valmin = val;
            nearestId = i;
        }
    }
    return nearestId;
}
int TrickerSearchPossibleGrass(IAPI& api, int DistMax) {
    double distmin = 10000000;
    int nearestId = -1;
    for(int i = 0;i < GrassCount;i++) {
        double dist = T.Dist(api.CellToGrid(Grass[i].x), api.CellToGrid(Grass[i].y), Gridx, Gridy);
        if(dist < distmin) {
            distmin = dist;
            nearestId = i;
        }
    }
    return nearestId;
}
void FindPathTo(std::pair<int, int> Target, bool CanReach, bool ForceToFind = false) {
    if((PathToGo.empty() || (!PathToGo.empty() && LastTarget != Target)) || ForceToFind) {
        LastTarget = Target;
        AStar.init();
        PathToGo = AStar.GetPath({Cellx,Celly}, {Target.first,Target.second}, CanReach);
    }
}
inline void StudentStart(IAPI& api) {
    studentstate = StudentState::MovingForClassroom;
    CurrentClassroomToGo = SearchBestClassroom(api);
    FindPathTo({Classroom[CurrentClassroomToGo].x,Classroom[CurrentClassroomToGo].y}, false);
}
inline void TrickerStart(IAPI& api) {
    CurrentPatrolNodeToGo = 0;
    trickerstate = TrickerState::MovingToPatrol;
    FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
    CurrentStudentToPursue = -1;
}
inline void Start(IAPI& api) {
    isFirstFrame = false;
    Map = api.GetFullMap();
    for(int i = 0;i < 50;i++)
        for(int j = 0;j < 50;j++) {
            if(Map[i][j] == THUAI6::PlaceType::Window)
                map[i][j] = 1;
            if(Map[i][j] == THUAI6::PlaceType::Wall || Map[i][j] == THUAI6::PlaceType::Gate
               || Map[i][j] == THUAI6::PlaceType::Chest || Map[i][j] == THUAI6::PlaceType::ClassRoom
               || Map[i][j] == THUAI6::PlaceType::HiddenGate)
                map[i][j] = 2;
            if(Map[i][j] == THUAI6::PlaceType::ClassRoom)
                Classroom[ClassroomCount].x = i, Classroom[ClassroomCount++].y = j;
            if(Map[i][j] == THUAI6::PlaceType::Chest)
                Chest[ChestCount].x = i, Chest[ChestCount++].y = j;
            if(Map[i][j] == THUAI6::PlaceType::Gate)
                Gate[GateCount].x = i, Gate[GateCount++].y = j;
            if(Map[i][j] == THUAI6::PlaceType::HiddenGate)
                HiddenGate[HiddenGateCount].x = i, HiddenGate[HiddenGateCount++].y = j;
            if(Map[i][j] == THUAI6::PlaceType::Grass) {
                Grass[GrassCount].x = i;
                Grass[GrassCount++].y = j;
                //api.Print(fmt::format("Grass[{}]:{},{}", GrassCount - 1, Grass[GrassCount - 1].x, Grass[GrassCount - 1].y));
            }
        }
    if(Gate[0].y > 25) {
        PatrolNode = {{18,18},{31,31},{42,31},{42,18},{31,7},{31,18},{18,31},{18,42},{7,31},{7,18}};
        map2 = true;
    } else {
        PatrolNode = {{23,13},{18,7},{8,9},{8,33},{14,36},{23,29},{31,35},{42,41},{40,24}};
        map1 = true;
    }
    if(PlayerId <= 3) {
        StudentStart(api);
    } else
        TrickerStart(api);
}
void UpdateStudentInfo(IStudentAPI& api) {
    msg = "*0*0";
    if(studentinfo->x == Gridx && studentinfo->y == Gridy && studentinfo->playerState == THUAI6::PlayerState::Idle)
        StuckTime += 50;
    else
        StuckTime = 0;
    switch(studentType[PlayerId]) {
    case  THUAI6::StudentType::StraightAStudent:
        if((int)studentinfo->playerState >= 1 && (int)studentinfo->playerState <= 10 && (int)studentinfo->playerState != 2)
            MeditationProgress += 40 * deltaTimems;
        else
            MeditationProgress = 0;
        break;
    }
    Gridx = studentinfo->x;
    Gridy = studentinfo->y;
    Cellx = api.GridToCell(studentinfo->x);
    Celly = api.GridToCell(studentinfo->y);
    for(int i = 0;i < 4;i++)
        Students[i] = *(api.GetStudents()[i]);
    auto TrickersInSight = api.GetTrickers();
    TrickerisInSight = false;
    CanGetTrickerInfo = false;
    for(int i= LastTrickerNum - 1;i >= 1;i--)
        LastTrickers[i] = LastTrickers[i-1];
    LastTrickers[0] = Trickers;
    if(!TrickersInSight.empty()) {
        //api.Print("TrickerisInSight!");
        TrickerisInSight = true;
        CanGetTrickerInfo = true;
        Trickers = *TrickersInSight[0];
    } else {
        //api.Print("TrickerisnotInSight!");
        //if(Trickers.x != -1)
        //api.Print(fmt::format("tricker{},{}", Trickers.x, Trickers.y));
        if(Trickers.x != -1 && api.HaveView(api.GridToCell(Trickers.x), api.GridToCell(Trickers.y))) {
            Trickers.x = -1;
            //api.Print("TrickerisnotInSightremove!");
        }
    }
    //api.Print(fmt::format("TrickerStuckTime {}", TrickerStuckTime));
}
void UpdateTrickerInfo(ITrickerAPI& api) {
    if(trickerinfo->x == Gridx && trickerinfo->y == Gridy && trickerinfo->playerState == THUAI6::PlayerState::Idle)
        StuckTime += 50;
    else
        StuckTime = 0;
    Gridx = trickerinfo->x;
    Gridy = trickerinfo->y;
    Cellx = api.GridToCell(trickerinfo->x);
    Celly = api.GridToCell(trickerinfo->y);
    auto StudentsInSight = api.GetStudents();
    for(int i = 0;i < 4;i++)
        StudentisInSight[i] = false;
    for(int i = 0;i < StudentsInSight.size();i++) {
        StudentisInSight[StudentsInSight[i]->playerID] = true;
        Students[StudentsInSight[i]->playerID] = *StudentsInSight[i];
    }
}
void UpdateMapInfo(IAPI& api) {
    for(int i = 0;i < 50;i++)
        for(int j = 0;j < 50;j++) {
            if(map[i][j] == 3)
                map[i][j] = 0;
            if(map[i][j] % 5 == 3)
                map[i][j] -= 5;
        }
}
void MoveAlongPath(IAPI& api) {
    if(!PathToGo.empty() && ((!PathGone.empty() && PathToGo.front() != PathGone.front()) || PathGone.empty())) {
        bool Reach = false;
        if(PathToGo.size() == 1) {
            if(T.Dist(Gridx, Gridy, api.CellToGrid(PathToGo.front()->x), api.CellToGrid(PathToGo.front()->y)) < 100) {
                Reach = true;
            }
        } else {
            auto it = PathToGo.begin();it++;
            if(T.Dist(Gridx, Gridy, api.CellToGrid(PathToGo.front()->x), api.CellToGrid(PathToGo.front()->y))
               + T.Dist(Gridx, Gridy, api.CellToGrid((*it)->x), api.CellToGrid((*it)->y)) <
               T.Dist(api.CellToGrid(PathToGo.front()->x), api.CellToGrid(PathToGo.front()->y),
               api.CellToGrid((*it)->x), api.CellToGrid((*it)->y)) * 1.15) {
                Reach = true;
            }
        }
        if(Reach) {
            PathGone.push_front(PathToGo.front());
            PathToGo.pop_front();
        }
        if(!PathToGo.empty()) {
            if(Map[PathToGo.front()->x][PathToGo.front()->y] == THUAI6::PlaceType::Window) {
                if(api.SkipWindow().get()) {
                    PathGone.push_front(PathToGo.front());
                    PathToGo.pop_front();
                }
            } else {
                faceDir = atan2(api.CellToGrid(PathToGo.front()->y) - Gridy, api.CellToGrid(PathToGo.front()->x) - Gridx);
                api.Move(500, faceDir);
            }
        } else {
            if(PlayerId <= 3) {
                switch(studentstate) {
                case StudentState::MovingForClassroom:
                    if(studentType[PlayerId] == THUAI6::StudentType::StraightAStudent && studentinfo->timeUntilSkillAvailable[0] == 0)
                        api.UseSkill(0);
                    studentstate = StudentState::Learning;
                    break;
                case StudentState::MovingForGrass:
                    break;
                case StudentState::MovingToGraduate:
                    api.StartOpenGate();
                    studentstate = StudentState::WaitToGraduate;
                    break;
                case StudentState::WaitToGraduate:
                    break;
                }
            } else {
                int GrassId;
                switch(trickerstate) {
                case TrickerState::MovingToPatrol:
                    CurrentPatrolNodeToGo = (CurrentPatrolNodeToGo + 1) % PatrolNode.size();
                    FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
                    break;
                case TrickerState::MovingToPursue:
                    if(Students[CurrentStudentToPursue].determination <= 0) {
                        FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
                        trickerstate = TrickerState::MovingToPatrol;
                    } else {
                        trickerstate = TrickerState::MovingToSearch;
                        GrassId = TrickerSearchPossibleGrass(api, 2000);
                        if(GrassId != -1) {
                            FindPathTo({Grass[GrassId].x,Grass[GrassId].y}, true);
                        }
                    }
                    break;
                case TrickerState::MovingToSearch:
                    FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
                    trickerstate = TrickerState::MovingToPatrol;
                    break;
                }
            }
        }
    }
}
bool OnlyTeacherLeft(ITrickerAPI& api) {
    if(framecount > 6000)
        return true;
    for(int i = 0;i < 4;i++) {
        if(Students[i].studentType != THUAI6::StudentType::Teacher && Students[i].determination > 0)
            return false;
    }
    return true;
}
int FindStudentInSight(ITrickerAPI& api) {
    auto StudentsInSight = api.GetStudents();
    for(auto i = 0;i < StudentsInSight.size();i++) {
        if(OnlyTeacherLeft(api)) {
            if(Students[StudentsInSight[i]->playerID].determination > 0)
                return StudentsInSight[i]->playerID;
        } else {
            if(Students[StudentsInSight[i]->playerID].studentType != THUAI6::StudentType::Teacher && Students[StudentsInSight[i]->playerID].determination > 0)
                return StudentsInSight[i]->playerID;
        }
    }
    return -1;
}
void ReadMessage(IStudentAPI& api) {
    for(int i = 0;i < 10;i++)
        Classroom[i].occupied = 4;
    if(map1)
        Classroom[5].occupied = 5;
    ShouldChangeClassroomToGo = false;
    while(api.HaveMessage()) {
        msgrcv = api.GetMessage();
        Classroom[msgrcv.second[0] - '0'].occupied = msgrcv.first;
        if(msgrcv.second[0] - '0' == CurrentClassroomToGo) ShouldChangeClassroomToGo = true;
        if(msgrcv.second[1] == '1') Classroom[msgrcv.second[2] - '0'].finished = true, ClassroomFinished++;
        if(msgrcv.second[3] == '1' && studentstate != StudentState::MovingToRescue) {
            int dist = 1000000, id = 0;
            for(int i = 0;i < 4;i++) {
                //api.Print(fmt::format("stu[{}].x = {}   stu[{}].y = {}", i, Students[i].x, i, Students[i].y));
                if(msgrcv.first != i) {
                    if(dist > T.Dist(Students[i].x, Students[i].y, Students[msgrcv.first].x, Students[msgrcv.first].y)) {
                        dist = T.Dist(Students[i].x, Students[i].y, Students[msgrcv.first].x, Students[msgrcv.first].y);
                        id = i;
                    }
                }
            }
            //api.Print(fmt::format("id{},dist{}", id, dist));
            if(id == PlayerId) {
                studentstate = StudentState::MovingToRescue;
                CurrentStudentToRescue = msgrcv.first;
            }
        }
        if(msgrcv.second[4] != '*' && !TrickerisInSight) {
            Trickers.x = T.ToInt(msgrcv.second.substr(4, 5));
            Trickers.y = T.ToInt(msgrcv.second.substr(9, 5));
            CanGetTrickerInfo = true;
        }
    }
    if(ShouldChangeClassroomToGo) {
        CurrentClassroomToGo = SearchBestClassroom(api);
        if(studentstate == StudentState::MovingForClassroom)
            FindPathTo({Classroom[CurrentClassroomToGo].x,Classroom[CurrentClassroomToGo].y}, false);
    }
}
void SendMessage(IStudentAPI& api) {
    if(studentstate == StudentState::MovingForClassroom || studentstate == StudentState::Learning)
        msg[0] = CurrentClassroomToGo + '0';
    if(studentinfo->playerState == THUAI6::PlayerState::Addicted &&
       !((studentinfo->dangerAlert < 1000 && studentinfo->dangerAlert > 0.01 && 15000 / studentinfo->dangerAlert < EscapeDist / Courage)
       || (T.Dist(Gridx, Gridy, Trickers.x, Trickers.y) < EscapeDist / Courage)))
        msg[3] = '1';
    if(TrickerisInSight)
        msg = msg + T.ToString(Trickers.x) + T.ToString(Trickers.y);
    else
        msg = msg + "**********";
    for(int i = 0;i < 4;i++)
        if(PlayerId != i)
            api.SendTextMessage(i, msg);
}
bool CheckTrickerStuck(){
    int avex = 0,avey = 0;
    for(int i = 0;i < LastTrickerNum;i++){
        avex += LastTrickers[i].x;
        avey += LastTrickers[i].y;
    }
    avex /= LastTrickerNum;
    avey /= LastTrickerNum;
    return (T.Dist(avex,avey,Trickers.x,Trickers.y) < 350);
}
void AI::play(IStudentAPI& api) {
    studentinfo = api.GetSelfInfo();
            //api.Print(fmt::format("studentinfo->dangerAlert {}", studentinfo->dangerAlert));
    UpdateMapInfo(api);
    UpdateStudentInfo(api);
    if(studentstate != StudentState::Learning && !rousing) {
        api.EndAllAction();
        //api.Print("endallaction");
        //api.Print(fmt::format("state {}", (int)studentinfo->playerState));
    }
    if(isFirstFrame) {
        PlayerId = this->playerID;
        Start(api);
    }
    ReadMessage(api);
    if(CheckTrickerStuck() && CanGetTrickerInfo) {
        TrickerStuckTime += 50;
    } else
        TrickerStuckTime = 0;
    if(studentinfo->dangerAlert < 1000 && studentinfo->dangerAlert > 0.01 && abs(T.Dist(Gridx, Gridy, Trickers.x, Trickers.y) - 15000 / studentinfo->dangerAlert) > 100)
        Trickers.x = -1;
    if(!CanGetTrickerInfo)
        Trickers.x = -1;
    //api.Print(fmt::format("Trickers{},{}", Trickers.x, Trickers.y));
    //api.Print(fmt::format("tricker{},{}", Trickers.x, Trickers.y));
    if(StuckTime >= 200) {
        if(PathToGo.size() >= 2) {
            map[PathToGo.front()->x][PathToGo.front()->y] = 203;
            FindPathTo({PathToGo.back()->x, PathToGo.back()->y}, true, true);
        }
        StuckTime = 0;
    }
    if(studentstate != StudentState::MovingForGrass) {
        if((studentinfo->dangerAlert < 1000 && studentinfo->dangerAlert > 0.01 && 15000 / studentinfo->dangerAlert < EscapeDist / Courage)
           || (Trickers.x != -1 && (T.Dist(Gridx, Gridy, Trickers.x, Trickers.y) < EscapeDist / Courage))) {
            rousing = false;
            CurrentGrassToGo = -1;
            if(map1) {
                if(studentstate == StudentState::Learning) {
                    if(CurrentClassroomToGo == 0) CurrentGrassToGo = 15;
                    if(CurrentClassroomToGo == 1) CurrentGrassToGo = 30;
                    if(CurrentClassroomToGo == 3) CurrentGrassToGo = 37;
                    if(CurrentClassroomToGo == 4) CurrentGrassToGo = 65;
                    if(CurrentClassroomToGo == 7) CurrentGrassToGo = 90;
                }
            } else {
                if(studentstate == StudentState::Learning) {
                    if(CurrentClassroomToGo == 3) CurrentGrassToGo = 19;
                    if(CurrentClassroomToGo == 4) CurrentGrassToGo = 45;
                    if(CurrentClassroomToGo == 5) CurrentGrassToGo = 78;
                    if(CurrentClassroomToGo == 6) CurrentGrassToGo = 104;
                }
            }
            if(CurrentGrassToGo == -1)
                CurrentGrassToGo = StudentSearchBestGrass(api);
            FindPathTo({Grass[CurrentGrassToGo].x, Grass[CurrentGrassToGo].y}, true);
            Grass[CurrentGrassToGo].occupied = true;
            studentstate = StudentState::MovingForGrass;
            //api.Print(fmt::format("grassfind{},{}", Grass[TargetGrass].x, Grass[TargetGrass].y));
        }
        auto bullets = api.GetBullets();
        bool FlyingKnifeInSight = false;
        for(auto i = 0;i < bullets.size();i++)
            if(bullets[i]->bulletType == THUAI6::BulletType::FlyingKnife){
                FlyingKnifeInSight = true;
                //api.Print(fmt::format("FlyingKnifeInSight!!!"));
            }
        if(FlyingKnifeInSight){
            CurrentGrassToGo = -1;
            if(map1) {
                if(studentstate == StudentState::Learning) {
                    if(CurrentClassroomToGo == 0) CurrentGrassToGo = 15;
                    if(CurrentClassroomToGo == 1) CurrentGrassToGo = 30;
                    if(CurrentClassroomToGo == 3) CurrentGrassToGo = 37;
                    if(CurrentClassroomToGo == 4) CurrentGrassToGo = 65;
                    if(CurrentClassroomToGo == 7) CurrentGrassToGo = 90;
                }
            } else {
                if(studentstate == StudentState::Learning) {
                    if(CurrentClassroomToGo == 3) CurrentGrassToGo = 19;
                    if(CurrentClassroomToGo == 4) CurrentGrassToGo = 45;
                    if(CurrentClassroomToGo == 5) CurrentGrassToGo = 78;
                    if(CurrentClassroomToGo == 6) CurrentGrassToGo = 104;
                }
            }
            if(CurrentGrassToGo == -1)
                CurrentGrassToGo = StudentSearchBestGrass(api);
            Grass[CurrentGrassToGo].occupied = true;
            FindPathTo({Grass[CurrentGrassToGo].x, Grass[CurrentGrassToGo].y}, true);
            studentstate = StudentState::MovingForGrass;
        }
    }
    if(ClassroomFinished >= 7 && studentstate != StudentState::WaitToGraduate && studentstate != StudentState::MovingToGraduate && studentstate != StudentState::MovingForGrass) {
        studentstate = StudentState::MovingToGraduate;
        CurrentGateToGo = SearchBestGate(api);
        FindPathTo({Gate[CurrentGateToGo].x,Gate[CurrentGateToGo].y}, false);
    }
    //api.Print(fmt::format("Courage{}", Courage));
    //api.Print(fmt::format("EscapeDist / Courage{}", EscapeDist / Courage));
        if(studentType[PlayerId] == THUAI6::StudentType::Sunshine && studentinfo->timeUntilSkillAvailable[1] < 0.01 && studentinfo->determination < 3200000) {
            api.UseSkill(1);
        }
    switch(studentstate) {
    case StudentState::MovingForClassroom:
        //api.Print(fmt::format("state:MovingForClassroom"));
        //api.Print(fmt::format("classroom{}:{},{}", CurrentClassroomToGo, Classroom[CurrentClassroomToGo].x, Classroom[CurrentClassroomToGo].y));
        //CurrentClassroomToGo = SearchBestClassroom(api);
        FindPathTo({Classroom[CurrentClassroomToGo].x, Classroom[CurrentClassroomToGo].y}, false);
        break;
    case StudentState::Learning:
        //api.Print(fmt::format("state:Learning"));
        //api.Print(fmt::format("classroom[{}] process = {}", CurrentClassroomToGo, api.GetClassroomProgress(Classroom[CurrentClassroomToGo].x, Classroom[CurrentClassroomToGo].y)));

        if(api.GetClassroomProgress(Classroom[CurrentClassroomToGo].x, Classroom[CurrentClassroomToGo].y) == 10000000) {
            //api.Print("ClassroomFinished");
            studentstate = StudentState::MovingForClassroom;
            Classroom[CurrentClassroomToGo].finished = true;
            ClassroomFinished++;
            msg[1] = '1';
            msg[2] = (char)(CurrentClassroomToGo + '0');
            CurrentClassroomToGo = SearchBestClassroom(api);
            FindPathTo({Classroom[CurrentClassroomToGo].x,Classroom[CurrentClassroomToGo].y}, false);
        } else
            api.StartLearning();
        break;
    case StudentState::MovingForGrass:
        Courage += 0.002;
        //api.Print(fmt::format("state:MovingForGrass"));
        if(!((studentinfo->dangerAlert < 1000 && studentinfo->dangerAlert > 0.01 && 15000 / studentinfo->dangerAlert < EscapeDist / Courage)
           || (Trickers.x != -1 && (T.Dist(Gridx, Gridy, Trickers.x, Trickers.y) < EscapeDist / Courage)))) {
            WaitTrickerTime += 50;
            if(Trickers.trickerType == THUAI6::TrickerType::ANoisyPerson){
                if(WaitTrickerTime >= 10000) {
                    Grass[CurrentGrassToGo].occupied = false;
                    if(ClassroomFinished >= 7)
                        studentstate = StudentState::MovingToGraduate;
                    else
                        studentstate = StudentState::MovingForClassroom;
                    WaitTrickerTime = 0;
                }
            }else{
                if(WaitTrickerTime >= 2500) {
                    Grass[CurrentGrassToGo].occupied = false;
                    if(ClassroomFinished >= 7)
                        studentstate = StudentState::MovingToGraduate;
                    else
                        studentstate = StudentState::MovingForClassroom;
                    WaitTrickerTime = 0;
                }
            }
        } else {
            if(studentType[PlayerId] == THUAI6::StudentType::Sunshine && studentinfo->timeUntilSkillAvailable[2] < 0.01) {
                api.UseSkill(2);
            }
            if(PathToGo.empty()) {
                //api.Print("gototricker");
                if(Map[Cellx - 1][Celly] == THUAI6::PlaceType::Grass
                   && T.Dist(api.CellToGrid(Cellx - 1), api.CellToGrid(Celly), Trickers.x, Trickers.y) < T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly), Trickers.x, Trickers.y))
                    FindPathTo({Cellx - 1, Celly}, true);
                if(Map[Cellx + 1][Celly] == THUAI6::PlaceType::Grass
                   && T.Dist(api.CellToGrid(Cellx + 1), api.CellToGrid(Celly), Trickers.x, Trickers.y) < T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly), Trickers.x, Trickers.y))
                    FindPathTo({Cellx + 1, Celly}, true);
                if(Map[Cellx][Celly - 1] == THUAI6::PlaceType::Grass
                   && T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly - 1), Trickers.x, Trickers.y) < T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly), Trickers.x, Trickers.y))
                    FindPathTo({Cellx, Celly - 1}, true);
                if(Map[Cellx][Celly + 1] == THUAI6::PlaceType::Grass
                   && T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly + 1), Trickers.x, Trickers.y) < T.Dist(api.CellToGrid(Cellx), api.CellToGrid(Celly), Trickers.x, Trickers.y))
                    FindPathTo({Cellx, Celly + 1}, true);
            }
            /*int TargetGrass;
            TargetGrass = StudentSearchBestGrass(api);
            FindPathTo({Grass[TargetGrass].x, Grass[TargetGrass].y}, true);*/
        }
        break;
    case StudentState::MovingToRescue:
        //api.Print(fmt::format("state:MovingToRescue"));
        //api.Print(fmt::format("rescuestudent{}:{},{}", CurrentStudentToRescue, Students[CurrentStudentToRescue].x, Students[CurrentStudentToRescue].y));
        if(Students[CurrentStudentToRescue].playerState != THUAI6::PlayerState::Addicted && Students[CurrentStudentToRescue].playerState != THUAI6::PlayerState::Roused) {
            rousing = false;
            studentstate = StudentState::MovingForClassroom;
            FindPathTo({Classroom[CurrentClassroomToGo].x,Classroom[CurrentClassroomToGo].y}, false);
            break;
        }
        FindPathTo({api.GridToCell(Students[CurrentStudentToRescue].x),api.GridToCell(Students[CurrentStudentToRescue].y)}, false);
        if(T.Dist(Gridx, Gridy, Students[CurrentStudentToRescue].x, Students[CurrentStudentToRescue].y) < 1200) {
            api.StartRouseMate(CurrentStudentToRescue);
            PathToGo.clear();
            rousing = true;
            //api.Print("rousing!");
            //api.Print(fmt::format("state {}", (int)studentinfo->playerState));
            if(Students[CurrentStudentToRescue].playerState != THUAI6::PlayerState::Addicted && Students[CurrentStudentToRescue].playerState != THUAI6::PlayerState::Roused) {
                studentstate = StudentState::MovingForClassroom;
                FindPathTo({Classroom[CurrentClassroomToGo].x,Classroom[CurrentClassroomToGo].y}, false);
                //api.Print("rouseover!");
                rousing = false;
                if(studentType[PlayerId] == THUAI6::StudentType::Sunshine && studentinfo->timeUntilSkillAvailable[1] < 0.01) {
                    api.UseSkill(1);
                }
            }
        }
        break;
    case StudentState::MovingToGraduate:
        CurrentGateToGo = SearchBestGate(api);
        FindPathTo({Gate[CurrentGateToGo].x,Gate[CurrentGateToGo].y}, false);
        break;
    case StudentState::WaitToGraduate:
        api.StartOpenGate();
        if(api.GetGateProgress(Gate[CurrentGateToGo].x, Gate[CurrentGateToGo].y) == 18000) {
            api.Graduate();
        }
        break;
    default:
        break;
    }
    if(TrickerStuckTime >= 200) {
        Courage += 0.003;
    }else{
        if(CanGetTrickerInfo)
            Courage = 1;
    }
    if(Courage > 1)
        Courage -= 0.0015;
    if(Courage > 2.3)
        Courage = 2.3;
    if(Courage < 1)
        Courage = 1;
    //api.Print(fmt::format("Courage = {}", Courage));
    //api.Print(fmt::format("CriticalDist = {}", EscapeDist / Courage));
    //api.Print(fmt::format("BGM = {}", studentinfo->dangerAlert));
    //api.Print(fmt::format("BGM Dist = {}", 15000 / studentinfo->dangerAlert));
    //for(auto i = PathToGo.begin();i != PathToGo.end();i++)
    //    api.Print(fmt::format("{} {}", (*i)->x, (*i)->y));
    MoveAlongPath(api);
    SendMessage(api);
}
void AI::play(ITrickerAPI& api) {
    framecount++;
    if(trickerType == THUAI6::TrickerType::ANoisyPerson) {
        api.UseSkill(0);
        goto end;
    }
    api.EndAllAction();
    trickerinfo = api.GetSelfInfo();
    UpdateMapInfo(api);
    UpdateTrickerInfo(api);
    if(isFirstFrame) {
        PlayerId = this->playerID;
        Start(api);
    }
    int StudentsInSightId;
    if(StuckTime >= 200) {
        if(StuckTime >= 1000) {
            CurrentPatrolNodeToGo = (CurrentPatrolNodeToGo + 1) % PatrolNode.size();
            trickerstate = TrickerState::MovingToPatrol;
            CurrentStudentToPursue = -1;
        } else {
            if(trickerstate == TrickerState::MovingToSearch || trickerstate == TrickerState::MovingToPursue) {
                if(api.Attack(faceDir).get())
                    Students[CurrentStudentToPursue].determination -= 1500000;
                if(Students[CurrentStudentToPursue].determination <= 0) {
                    trickerstate = TrickerState::MovingToPatrol;
                    CurrentStudentToPursue = -1;
                }
            } else {
                if(!PathToGo.empty()) {
                    map[PathToGo.front()->x][PathToGo.front()->y] = 203;
                    FindPathTo({PathToGo.back()->x, PathToGo.back()->y}, true, true);
                }
            }
        }
    }
    switch(trickerstate) {
    case TrickerState::MovingToPatrol:
        StudentsInSightId = FindStudentInSight(api);
        if(StudentsInSightId != -1) {
            trickerstate = TrickerState::MovingToPursue;
            CurrentStudentToPursue = StudentsInSightId;
        } else {
            FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
        }
        break;
    case TrickerState::MovingToPursue:
        if(Students[CurrentStudentToPursue].playerState == THUAI6::PlayerState::Addicted) {
            trickerstate = TrickerState::MovingToPatrol;
            FindPathTo({PatrolNode[CurrentPatrolNodeToGo].x,PatrolNode[CurrentPatrolNodeToGo].y}, true);
            break;
        }
        FindPathTo({api.GridToCell(Students[CurrentStudentToPursue].x), api.GridToCell(Students[CurrentStudentToPursue].y)}, false);
        if(StudentisInSight[CurrentStudentToPursue]) {
            if(trickerinfo->timeUntilSkillAvailable[1] == 0) {
                if(trickerType == THUAI6::TrickerType::Klee && T.Dist(Gridx, Gridy, Students[CurrentStudentToPursue].x, Students[CurrentStudentToPursue].y) <= 2000) {
                    api.UseSkill(0);
                }
                if(api.HaveView(Students[CurrentStudentToPursue].y + sin(Students[CurrentStudentToPursue].facingDirection) * Students[CurrentStudentToPursue].speed * 0.01,
                   Students[CurrentStudentToPursue].x + cos(Students[CurrentStudentToPursue].facingDirection) * Students[CurrentStudentToPursue].speed * 0.01))
                    if(trickerType == THUAI6::TrickerType::Assassin)
                        api.UseSkill(1);
            }
            switch(trickerinfo->bulletType) {
            case THUAI6::BulletType::CommonAttackOfTricker:
                if(T.Dist(Gridx, Gridy, Students[CurrentStudentToPursue].x, Students[CurrentStudentToPursue].y) <= 2000) {
                    api.Attack(atan2(Students[CurrentStudentToPursue].y - Gridy, Students[CurrentStudentToPursue].x - Gridx));
                    Students[CurrentStudentToPursue].determination -= 1500000;
                }
                break;
            case THUAI6::BulletType::FlyingKnife:
                api.Attack(atan2(
                    Students[CurrentStudentToPursue].y - Gridy + sin(Students[CurrentStudentToPursue].facingDirection) * Students[CurrentStudentToPursue].speed * 0.01,
                    Students[CurrentStudentToPursue].x - Gridx + cos(Students[CurrentStudentToPursue].facingDirection) * Students[CurrentStudentToPursue].speed * 0.01));
                Students[CurrentStudentToPursue].determination -= 1200000;
                break;
            case THUAI6::BulletType::BombBomb:
                api.Attack(atan2(Students[CurrentStudentToPursue].y - Gridy, Students[CurrentStudentToPursue].x - Gridx));
                Students[CurrentStudentToPursue].determination -= 1200000;
                break;
            }
        }
        break;
    case TrickerState::MovingToSearch:
        break;
    }
    MoveAlongPath(api);
end:;
}