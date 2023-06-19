#include <vector>
#include <thread>
#include <array>
#include <queue>
#include <stack>
#include <cmath>
#include <sstream>
#include "AI.h"
#include "constants.h"
#define PI 3.14159

//edit
// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent
};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

struct CellPlace {
    int cellx;
    int celly;
};

struct Path {
    int lastIndex;
    CellPlace place;
};

// 可以在AI.cpp内部声明变量与函数
bool _isMapGetted = false;
bool tryToGraduate = false;
int Finished = 0;
bool startLearning = false;
bool escaping = false;
int trembleing = 0;
std::vector<std::vector<THUAI6::PlaceType>> fullMap;
std::vector<CellPlace> ignorePlace;
std::stack<CellPlace> walkPath;
std::stack<CellPlace> graduatePath;
THUAI6::PlaceType nextType;
std::vector<CellPlace> classrooms;
std::vector<CellPlace> gates;
std::vector<CellPlace> finishedClassrooms;
std::vector<CellPlace> grasses;
std::stack<CellPlace> escapePath;
std::vector<CellPlace> spotted;
int lastHp;
int selfreportCountdown;
std::vector<std::shared_ptr<const THUAI6::Tricker>> tricker;

std::stack<CellPlace> GetPath(THUAI6::PlaceType type, CellPlace begin, IStudentAPI& api);
std::stack<CellPlace> TraceBack(std::vector<Path>& allPoint);
bool TypeHandler(THUAI6::PlaceType type, CellPlace place, IStudentAPI& api);
bool AutoMove(CellPlace target, std::shared_ptr<const THUAI6::Student> info, IStudentAPI& api);


std::stringstream formatter;
std::string messager;
std::string analyser;
void broadcast(std::string message, IStudentAPI& api)
{
    api.SendTextMessage(0, message);
    api.SendTextMessage(1, message);
    api.SendTextMessage(2, message);
    api.SendTextMessage(3, message);
}
bool received;

class RankedCell
    ///<summary>
    /// ranked cell for priority queue, search cells looks nearer first
    /// rank = real distance from begin to this cell + straightForward distance to end
{
public:
    CellPlace loc;
    int rank;
    int dist;
    friend bool operator > (const RankedCell& a, const RankedCell& b)
    {
        return a.rank > b.rank || (a.rank == b.rank && a.dist < b.dist);
    }
    friend bool operator < (const RankedCell& a, const RankedCell& b)
    {
        return a.rank < b.rank || (a.rank == b.rank && a.dist > b.dist);
    }
    CellPlace Cell() { return loc; }
    RankedCell(CellPlace newLoc, int newRank, int newDist) { loc = newLoc, rank = newRank, dist = newDist; }
    RankedCell() { loc = { 0,0 }, rank = 2147483647, dist = 0; }
    RankedCell next(char direction)
    {
        RankedCell ans = *this;
        switch (direction)
        {
        case 1:
            ans.loc.cellx += 1;
            ans.dist += 10;
            break;
        case 2:
            ans.loc.cellx += 1;
            ans.loc.celly += 1;
            ans.dist += 14;
            break;
        case 3:
            ans.loc.celly += 1;
            ans.dist += 10;
            break;
        case 4:
            ans.loc.celly += 1;
            ans.loc.cellx -= 1;
            ans.dist += 14;
            break;
        case 5:
            ans.loc.cellx -= 1;
            ans.dist += 10;
            break;
        case 6:
            ans.loc.cellx -= 1;
            ans.loc.celly -= 1;
            ans.dist += 14;
            break;
        case 7:
            ans.loc.celly -= 1;
            ans.dist += 10;
            break;
        case 8:
            ans.loc.celly -= 1;
            ans.loc.cellx += 1;
            ans.dist += 14;
            break;
        }
        return ans;
    }
    bool in(std::vector<CellPlace> group)
    {
        for (auto iter = group.begin(); iter < group.end(); iter++)
        {
            if (this->loc.cellx == (*iter).cellx && this->loc.celly == (*iter).celly)
                return true;
        }
        return false;
    }
    friend std::stack<CellPlace> AStarSearch(CellPlace, std::vector<CellPlace>, const std::vector<std::vector<THUAI6::PlaceType>>&, IStudentAPI&);
};

int DiagDist (CellPlace from, CellPlace to)
{
    int dx = from.cellx - to.cellx;
    if(dx < 0) dx = -dx;
    int dy = from.celly - to.celly;
    if(dy < 0) dy = -dy;
    int min = dx > dy ? dy : dx;
    return 10 * (dx + dy) - 6 * min;
}

int DiagDist(CellPlace from, std::vector<CellPlace> to)
{
    int mindist = 100000;
    for (auto iter = to.begin(); iter < to.end(); iter++)
    {
        if (DiagDist(from, (*iter)) < mindist)
        {
            mindist = DiagDist(from, (*iter));
        }
    }
    return mindist;
}

bool SelfDoorOpen(int x, int y)
{
    return false;
}

bool Walkable(THUAI6::PlaceType loc, int cellx, int celly, IStudentAPI& api)
{
    if (!tricker.empty())
    {
        if (cellx == api.GridToCell(tricker[0].get()->x) && celly == api.GridToCell(tricker[0].get()->y))
        {
            return false;
        }
    }
    switch(loc)
    {
        case THUAI6::PlaceType::Land:
        return true;
        case THUAI6::PlaceType::Grass:
        return true;
        case THUAI6::PlaceType::Window:
        return true;
        case THUAI6::PlaceType::Door3:
        if(SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
        case THUAI6::PlaceType::Door5:
        if(SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
        case THUAI6::PlaceType::Door6:
        if(SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
        default:
        return false;
    }
}

std::stack<CellPlace> AStarSearch(CellPlace begin, std::vector<CellPlace> end, const std::vector<std::vector<THUAI6::PlaceType>>& map, IStudentAPI& api)
{
    std::priority_queue<RankedCell, std::vector<RankedCell>, std::greater<RankedCell>> todo;
    RankedCell pushBuf(begin, DiagDist(begin, end), 0);
    RankedCell searchCenter;
    CellPlace endPoint;
    todo.push(pushBuf);
    char available;
    char visited[50][50] = { 0 };
    short distance[50][50];
    for (int i = 0; i < 2500; i++)
    {
        distance[i / 50][i % 50] = 30000;
    }
    //start: visited = 16;
    //from left: visited = 1;
    //from downleft: visited = 2;
    //counter clockwise
    
    while(!todo.empty())
    {
        available = 0;
        searchCenter = todo.top();
        todo.pop();
        if (visited[searchCenter.loc.cellx][searchCenter.loc.celly] == 0)
        {
            visited[searchCenter.loc.cellx][searchCenter.loc.celly] = 16;
            distance[searchCenter.loc.cellx][searchCenter.loc.celly] = 0;
        }

        if (Walkable(map[searchCenter.loc.cellx+1][searchCenter.loc.celly], 
        searchCenter.loc.cellx+1, searchCenter.loc.celly, api))
            available |= 1;
        if (searchCenter.loc.cellx != 49 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10 
            < distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly])
        {
            if(searchCenter.next(1).in(end))
            {
                todo.push(searchCenter.next(1));
                visited[searchCenter.loc.cellx + 1][searchCenter.loc.celly] = 1;
                endPoint = searchCenter.next(1).loc;
                break;
            }
            else if(available & 1)
            {
                visited[searchCenter.loc.cellx + 1][searchCenter.loc.celly] = 1;
                distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(1);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }

        if (Walkable(map[searchCenter.loc.cellx][searchCenter.loc.celly + 1], 
        searchCenter.loc.cellx, searchCenter.loc.celly + 1, api))
            available |= 2;
        if (searchCenter.loc.celly != 49 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx][searchCenter.loc.celly + 1])
        {
            if(searchCenter.next(3).in(end))
            {
                todo.push(searchCenter.next(3));
                visited[searchCenter.loc.cellx][searchCenter.loc.celly + 1] = 3;
                endPoint = searchCenter.next(3).loc;
                break;
            }
            else if(available & 2)
            {
                visited[searchCenter.loc.cellx][searchCenter.loc.celly + 1] = 3;
                distance[searchCenter.loc.cellx][searchCenter.loc.celly + 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(3);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }

        if(Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly], 
        searchCenter.loc.cellx - 1, searchCenter.loc.celly, api))
            available |= 4;
        if (searchCenter.loc.cellx != 0 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly])
        {
            if(searchCenter.next(5).in(end))
            {
                todo.push(searchCenter.next(5));
                visited[searchCenter.loc.cellx - 1][searchCenter.loc.celly] = 5;
                endPoint = searchCenter.next(5).loc;
                break;
            }
            else if(available & 4)
            {
                visited[searchCenter.loc.cellx - 1][searchCenter.loc.celly] = 5;
                distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(5);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }

        if(Walkable(map[searchCenter.loc.cellx][searchCenter.loc.celly - 1], 
        searchCenter.loc.cellx, searchCenter.loc.celly - 1, api))
            available |= 8;
        if (searchCenter.loc.celly != 0 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx][searchCenter.loc.celly - 1])
        {
            if(searchCenter.next(7).in(end))
            {
                todo.push(searchCenter.next(7));
                visited[searchCenter.loc.cellx][searchCenter.loc.celly - 1] = 7;
                endPoint = searchCenter.next(7).loc;
                break;
            }
            else if(available & 8)
            {
                visited[searchCenter.loc.cellx][searchCenter.loc.celly - 1] = 7;
                distance[searchCenter.loc.cellx][searchCenter.loc.celly - 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(7);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }


        if ((available & 3) == 3 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14
            < distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly + 1])
        {
            if(Walkable(map[searchCenter.loc.cellx + 1][searchCenter.loc.celly + 1], 
            searchCenter.loc.cellx + 1, searchCenter.loc.celly + 1, api))
            {
                pushBuf = searchCenter.next(2);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
                visited[pushBuf.loc.cellx][pushBuf.loc.celly] = 2;
                distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly + 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14;
            }
        }

        if ((available & 6) == 6 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14
            < distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly + 1])
        {
            if(Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly + 1], 
            searchCenter.loc.cellx - 1, searchCenter.loc.celly + 1, api))
            {
                pushBuf = searchCenter.next(4);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
                visited[pushBuf.loc.cellx][pushBuf.loc.celly] = 4;
                distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly + 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14;
            }
        }

        if ((available & 12) == 12 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14
            < distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly - 1])
        {
            if(Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly - 1], 
            searchCenter.loc.cellx - 1, searchCenter.loc.celly - 1, api))
            {
                pushBuf = searchCenter.next(6);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
                visited[pushBuf.loc.cellx][pushBuf.loc.celly] = 6;
                distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly - 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14;
            }
        }
        
        if((available & 9) == 9 
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14
            < distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly - 1])
        {
            if(Walkable(map[searchCenter.loc.cellx + 1][searchCenter.loc.celly - 1], 
            searchCenter.loc.cellx + 1, searchCenter.loc.celly - 1, api))
            {
                pushBuf = searchCenter.next(8);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
                visited[pushBuf.loc.cellx][pushBuf.loc.celly] = 8;
                distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly - 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14;
            }
        }
    }
    if(todo.empty())
        throw "unreachable";
    while (!todo.empty())
        todo.pop();
    bool finished = false;
    CellPlace point = endPoint;
    std::stack<CellPlace> ans;
    while(!finished)
    {
        ans.push(point);
        switch (visited[point.cellx][point.celly])
        {
            case 1:
            point.cellx = point.cellx - 1;
            break;
            case 2:
            point.cellx = point.cellx - 1;
            point.celly = point.celly - 1;
            break;
            case 3:
            point.celly = point.celly - 1;
            break;
            case 4:
            point.celly = point.celly - 1;
            point.cellx = point.cellx + 1;
            break;
            case 5:
            point.cellx = point.cellx + 1;
            break;
            case 6:
            point.cellx = point.cellx + 1;
            point.celly = point.celly + 1;
            break;
            case 7:
            point.celly = point.celly + 1;
            break;
            case 8:
            point.celly = point.celly + 1;
            point.cellx = point.cellx - 1;
            break;
            case 16:
            ans.pop();
            finished = true;
            break;
            default:
            throw "functionFailed";
        }
    }
    return ans;
}

void AI::play(IStudentAPI& api)
{   
    auto info = api.GetSelfInfo();
    tricker = api.GetTrickers();
    CellPlace selfPlace;
    selfPlace.cellx = api.GridToCell(info.get()->x);
    selfPlace.celly = api.GridToCell(info.get()->y);
    received = api.HaveMessage();
    if (received)
    {
        int temp;
        CellPlace targ;
        messager = api.GetMessage().second;
        formatter << messager;
        formatter >> analyser;
        if (analyser.compare("LearnStarted") == 0)
        {
            formatter >> temp;
            targ.cellx = temp;
            formatter >> temp;
            targ.celly = temp;
            if (!classrooms.empty())
            {
                for (auto iter = classrooms.begin(); iter < classrooms.end(); iter++)
                {
                    if ((*iter).cellx == targ.cellx && (*iter).celly == targ.celly)
                    {
                        classrooms.erase(iter);
                        break;
                    }
                }
                if (walkPath.size() > 2)
                {
                    while (!walkPath.empty())
                        walkPath.pop();
                    walkPath = AStarSearch(selfPlace, classrooms, fullMap, api);
                }
            }
        }
        else if (analyser.compare("LearnAbandoned") == 0)
        {
            formatter >> temp;
            targ.cellx = temp;
            formatter >> temp;
            targ.celly = temp;
            classrooms.push_back(targ);
        }
        else if (analyser.compare("LearnFinished") == 0)
        {
            formatter >> temp;
            targ.cellx = temp;
            formatter >> temp;
            targ.celly = temp;
            if (finishedClassrooms.empty())
            {
                finishedClassrooms.push_back(targ);
                Finished++;
            }
            else
            {
                bool reported = false;
                for (auto iter = finishedClassrooms.begin(); iter < finishedClassrooms.end(); iter++)
                {
                    if ((*iter).cellx == targ.cellx && (*iter).celly == targ.celly)
                    {
                        reported = true;
                        break;
                    }
                }
                if (!reported)
                {
                    Finished++;
                    finishedClassrooms.push_back(targ);
                }
            }
        }
        formatter.clear();
        formatter.str("");
        formatter.clear();
    }
    if (!_isMapGetted) {
        _isMapGetted = true;
        fullMap = api.GetFullMap();
        for(int iter1 = 0; iter1 < 50; iter1++)
        {
            for(int iter2 = 0; iter2 < 50; iter2++)
            {
                if(fullMap[iter1][iter2] == THUAI6::PlaceType::ClassRoom)
                {
                    classrooms.push_back({ iter1, iter2 });
                }
                if (fullMap[iter1][iter2] == THUAI6::PlaceType::Grass)
                {
                    grasses.push_back({ iter1, iter2 });
                }
            }
        }
        selfreportCountdown = 20;
    }
    if (trembleing > 0)
    {
        if (fullMap[selfPlace.cellx][selfPlace.celly] == THUAI6::PlaceType::Grass)
        {
            trembleing--;
            if (trembleing == 0)
            {
                for (auto iter = spotted.begin(); iter < spotted.end(); iter++)
                {
                    grasses.push_back(*iter);
                }
                spotted.clear();
                if (Finished <= 7)
                {
                    walkPath = AStarSearch(selfPlace, classrooms, fullMap, api);
                }
                else
                {
                    graduatePath = AStarSearch(selfPlace, classrooms, fullMap, api);
                }
            }
            if (selfreportCountdown == 0)
            {
                messager = "NeedHelp ";
                formatter << selfPlace.cellx;
                formatter >> analyser;
                formatter.clear();
                messager += analyser;
                formatter << selfPlace.celly;
                formatter >> analyser;
                messager += " ";
                messager += analyser;
                selfreportCountdown += 20;
                formatter.clear();
            }
            else
            {
                selfreportCountdown--;
            }
            if (info.get()->determination < lastHp)
            {
                bool abandonGrassCompleted = false;
                while (!abandonGrassCompleted)
                {
                    abandonGrassCompleted = true;
                    for (auto iter = grasses.begin(); iter < grasses.end(); iter++)
                    {
                        if (DiagDist(*iter, selfPlace) < 500)
                        {
                            spotted.push_back(*iter);
                            grasses.erase(iter);
                            abandonGrassCompleted = false;
                            break;
                        }
                    }
                }
                trembleing = 0;
                escapePath = AStarSearch(selfPlace, grasses, fullMap, api);
            }
            else if(lastHp < 2000000)
            {
                messager = "NeedEncourage ";
                formatter << info.get()->determination;
                formatter >> analyser;
                messager += analyser;
                messager += " ";
                formatter.clear();
                formatter << selfPlace.cellx;
                formatter >> analyser;
                messager += analyser;
                messager += " ";
                formatter.clear();
                formatter << selfPlace.celly;
                formatter >> analyser;
                messager += analyser;
                selfreportCountdown += 20;
                formatter.clear();
            }
        }
        else
        {
            trembleing = 0;
            escapePath = AStarSearch(selfPlace, grasses, fullMap, api);
        }
    }
    else if (!escapePath.empty())
    {
        CellPlace heading = escapePath.top();
        AutoMove(heading, info, api);
        escapePath = AStarSearch(selfPlace, grasses, fullMap, api);
    }
    else if (!walkPath.empty()) {
        CellPlace nextPoint = walkPath.top();
        bool res;
        res = AutoMove(nextPoint, info, api);
        if (res) {
            if (walkPath.size() >= 2)
            {
                walkPath.pop();
            }
            if (walkPath.size() == 1) {
                CellPlace working = walkPath.top();
                walkPath.pop();
                if (nextType == THUAI6::PlaceType::ClassRoom) {
                    api.UseSkill(0);
                    api.StartLearning();
                    startLearning = true;
                    messager = "LearnStarted ";
                    formatter << working.cellx;
                    formatter >> analyser;
                    messager += analyser;
                    messager += " ";
                    formatter.clear();
                    formatter << working.celly;
                    formatter >> analyser;
                    messager += analyser;
                    formatter.str("");
                    formatter.clear();
                    broadcast(messager, api);
                }
                else if (nextType == THUAI6::PlaceType::Gate) {
                    api.Graduate();
                    api.StartOpenGate();
                    tryToGraduate = true;
                }
            }
        }
    }
    else if (!graduatePath.empty())
    {
        CellPlace heading = graduatePath.top();
        bool res;
        res = AutoMove(heading, info, api);
        if (res) {
            if (graduatePath.size() >= 2)
            {
                graduatePath.pop();
            }
            if (graduatePath.size() == 1) {
                CellPlace working = graduatePath.top();
                graduatePath.pop();
                api.Graduate();
                api.StartOpenGate();
                tryToGraduate = true;
            }
        }
    }
    else {
        int mindist = 10000;
        CellPlace nearestClassroom;
        if (!classrooms.empty())
        {
            for (auto iter = classrooms.begin(); iter < classrooms.end(); iter++)
            {
                if (DiagDist(selfPlace, *iter) < mindist)
                {
                    mindist = DiagDist(selfPlace, *iter);
                    nearestClassroom = *iter;
                }
            }
        }
        else
        {
            nearestClassroom = { 0,0 };
        }
        if ((info.get()->dangerAlert > 2.5 && info.get()->dangerAlert < 10) || (!tricker.empty() && DiagDist({api.GridToCell(tricker[0].get()->x),api.GridToCell(tricker[0].get()->y)}, selfPlace) < 20))
        {
            if (api.GetClassroomProgress(nearestClassroom.cellx, nearestClassroom.celly) == 10000000)
            {
                startLearning = false;
                Finished += 1;
                for (auto iter1 = classrooms.begin(); iter1 < classrooms.end(); iter1++)
                {
                    if ((*iter1).cellx == nearestClassroom.cellx && (*iter1).celly == nearestClassroom.celly)
                    {
                        classrooms.erase(iter1);
                        messager = "LearnFinished ";
                        formatter << nearestClassroom.cellx;
                        formatter >> analyser;
                        messager += analyser;
                        messager += " ";
                        formatter.clear();
                        formatter << nearestClassroom.celly;
                        formatter >> analyser;
                        messager += analyser;
                        formatter.str("");
                        formatter.clear();
                        broadcast(messager, api);
                        bool reported = false;
                        for (auto iter2 = finishedClassrooms.begin(); iter2 < finishedClassrooms.end(); iter2++)
                        {
                            if ((*iter2).cellx == nearestClassroom.cellx && (*iter2).celly == nearestClassroom.celly)
                            {
                                reported = true;
                                break;
                            }
                        }
                        if (!reported)
                        {
                            Finished++;
                            finishedClassrooms.push_back(nearestClassroom);
                        }
                        break;
                    }
                }
            }
            else
            {
                messager = "LearnAbandoned ";
                formatter << nearestClassroom.cellx;
                formatter >> analyser;
                messager += analyser;
                messager += " ";
                formatter.clear();
                formatter << nearestClassroom.celly;
                formatter >> analyser;
                messager += analyser;
                formatter.str("");
                formatter.clear();
                broadcast(messager, api);
            }
            while (!walkPath.empty())
                walkPath.pop();
            while (!graduatePath.empty())
                graduatePath.pop();
            escapePath = AStarSearch(selfPlace, grasses, fullMap, api);
            tryToGraduate = false;
            startLearning = false;
        }
        else if (Finished >= 8 && !tryToGraduate)
        {
            api.Print("Finished Detected");
            CellPlace Gate;
            startLearning = false;
            for (int i = 0; i < 2500; i++)
            {
                if (fullMap[i / 50][i % 50] == THUAI6::PlaceType::Gate)
                {
                    Gate.cellx = i / 50;
                    Gate.celly = i % 50;
                    gates.push_back(Gate);
                }
            }
            if (!gates.empty())
            {
                if (gates.size() > 2)
                    api.Print("Gate detection error");
                while (!walkPath.empty())
                    walkPath.pop();
                graduatePath = AStarSearch(selfPlace, gates, fullMap, api);
                nextType = THUAI6::PlaceType::Gate;
            }
        }
        else if (tryToGraduate)
        {   
            bool test = false;
            test = test || fullMap[selfPlace.cellx + 1][selfPlace.celly] == THUAI6::PlaceType::Gate;
            test = test || fullMap[selfPlace.cellx][selfPlace.celly + 1] == THUAI6::PlaceType::Gate;
            test = test || fullMap[selfPlace.cellx - 1][selfPlace.celly] == THUAI6::PlaceType::Gate;
            test = test || fullMap[selfPlace.cellx][selfPlace.celly - 1] == THUAI6::PlaceType::Gate;
            if (test)
            {
                api.Graduate();
                api.StartOpenGate();
            }
            else
            {
                gates.clear();
                tryToGraduate = false;
            }
        }
        else if (startLearning)
        {
            if (api.GetClassroomProgress(nearestClassroom.cellx, nearestClassroom.celly) == 10000000)
            {
                startLearning = false;
                Finished += 1;
                for (auto iter = classrooms.begin(); iter < classrooms.end(); iter++)
                {
                    if ((*iter).cellx == nearestClassroom.cellx && (*iter).celly == nearestClassroom.celly)
                    {
                        classrooms.erase(iter);
                        messager = "LearnFinished ";
                        formatter << nearestClassroom.cellx;
                        formatter >> analyser;
                        messager += analyser;
                        messager += " ";
                        formatter.clear();
                        formatter << nearestClassroom.celly;
                        formatter >> analyser;
                        messager += analyser;
                        formatter.str("");
                        formatter.clear();
                        broadcast(messager, api);
                        bool reported = false;
                        for (auto iter = finishedClassrooms.begin(); iter < finishedClassrooms.end(); iter++)
                        {
                            if ((*iter).cellx == nearestClassroom.cellx && (*iter).celly == nearestClassroom.celly)
                            {
                                reported = true;
                                break;
                            }
                        }
                        if (!reported)
                        {
                            Finished++;
                            finishedClassrooms.push_back(nearestClassroom);
                        }
                        break;
                    }
                }
            }
            else
                api.StartLearning();
        }
        else
        {
            if (!classrooms.empty())
            {
                walkPath = AStarSearch(selfPlace, classrooms, fullMap, api);
                nextType = THUAI6::PlaceType::ClassRoom;
            }
            else
            {
                Finished = 7;
            }
        }
        lastHp = info.get()->determination;
    }
    //// 公共操作
    //if (this->playerID == 0)
    //{
    //    // 玩家0执行操作
    //    
    //}
    //else if (this->playerID == 1)
    //{
    //    // 玩家1执行操作
    //}
    //else if (this->playerID == 2)
    //{
    //    // 玩家2执行操作
    //}
    //else if (this->playerID == 3)
    //{
    //    // 玩家3执行操作
    //}
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

void AI::play(ITrickerAPI& api)
{
    if (!_isMapGetted) {
        _isMapGetted = true;
        fullMap = api.GetFullMap();
    }
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    api.GetTrickers();
}

std::stack<CellPlace> GetPath(THUAI6::PlaceType type, CellPlace begin, IStudentAPI& api) {
    int i = -1;
    bool visited[50][50] = { false };
    std::queue<Path> search;
    std::vector<Path> path;
    Path fstPath;
    fstPath.place = begin;
    fstPath.lastIndex = i;
    search.push(fstPath);
    visited[begin.cellx][begin.celly] = true;
    while(!search.empty()) {
        Path thispath = search.front();
        CellPlace top = thispath.place;
        i++;
        path.push_back(thispath);
        //visited[top.cellx][top.celly] = true;
        search.pop();
        if (top.cellx >= 0 && top.celly - 1 >= 0 && !visited[top.cellx][top.celly - 1]) {
            THUAI6::PlaceType linkType = fullMap[top.cellx][top.celly - 1];
            if (linkType == THUAI6::PlaceType::Wall) {
                continue;
            }
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window ||linkType == THUAI6::PlaceType::Grass) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx;
                p.place.celly = top.celly - 1;
                visited[top.cellx][top.celly - 1] = true;
                search.push(p);
            }
            else if (linkType == type) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx;
                p.place.celly = top.celly - 1;
                if (TypeHandler(type, p.place, api)) {
                    path.push_back(p);
                    return TraceBack(path);
                }
                else {
                    continue;
                }
            }
        }
        if (top.cellx - 1 >= 0 && top.celly >= 0 && !visited[top.cellx - 1][top.celly]) {
            THUAI6::PlaceType linkType = fullMap[top.cellx - 1][top.celly];
            if (linkType == THUAI6::PlaceType::Wall) {
                continue;
            }
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window || linkType == THUAI6::PlaceType::Grass) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx - 1;
                p.place.celly = top.celly;
                visited[top.cellx - 1][top.celly] = true;
                search.push(p);
            }
            else if (linkType == type) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx - 1;
                p.place.celly = top.celly;
                if (TypeHandler(type, p.place, api)) {
                    path.push_back(p);
                    return TraceBack(path);
                }
                else {
                    continue;
                }
            }
        }
        if (top.cellx + 1 < 50 && top.celly >= 0 && !visited[top.cellx + 1][top.celly]) {
            THUAI6::PlaceType linkType = fullMap[top.cellx + 1][top.celly];
            if (linkType == THUAI6::PlaceType::Wall) {
                continue;
            }
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window || linkType == THUAI6::PlaceType::Grass) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx + 1;
                p.place.celly = top.celly;
                visited[top.cellx + 1][top.celly] = true;
                search.push(p);
            }
            else if (linkType == type) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx + 1;
                p.place.celly = top.celly;
                if (TypeHandler(type, p.place, api)) {
                    path.push_back(p);
                    return TraceBack(path);
                }
                else {
                    continue;
                }
            }
        }
        if (top.cellx >= 0 && top.celly + 1 < 50 && !visited[top.cellx][top.celly + 1]) {
            THUAI6::PlaceType linkType = fullMap[top.cellx][top.celly + 1];
            if (linkType == THUAI6::PlaceType::Wall) {
                continue;
            }
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window || linkType == THUAI6::PlaceType::Grass) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx;
                p.place.celly = top.celly + 1;
                visited[top.cellx][top.celly + 1] = true;
                search.push(p);
            }
            else if (linkType == type) {
                Path p;
                p.lastIndex = i;
                p.place.cellx = top.cellx;
                p.place.celly = top.celly + 1;
                if (TypeHandler(type, p.place, api)) {
                    path.push_back(p);
                    return TraceBack(path);
                }
                else {
                    continue;
                }
            }
        }
    }
}

std::stack<CellPlace> TraceBack(std::vector<Path>& allPoint) {
    std::stack<CellPlace> res;
    for (int i = allPoint.size() - 1; ;) {
        res.push(allPoint[i].place);
        i = allPoint[i].lastIndex;
        if (i == -1) {
            return res;
        }
    }
    return res;
}

bool AutoMove(CellPlace target, std::shared_ptr<const THUAI6::Student> info, IStudentAPI& api) {
    int tar_x = api.CellToGrid(target.cellx);
    int tar_y = api.CellToGrid(target.celly);
    int self_x = info.get()->x;
    int self_y = info.get()->y;
    if (fullMap[target.cellx][target.celly] == THUAI6::PlaceType::Window) {
        api.SkipWindow();
        return true;
    }
    if (fullMap[target.cellx - 1][target.celly] == THUAI6::PlaceType::Wall)
        tar_x += 80;
    if (fullMap[target.cellx - 1][target.celly - 1] == THUAI6::PlaceType::Wall)
    {
        tar_x += 80;
        tar_y += 80;
    }
    if (fullMap[target.cellx][target.celly - 1] == THUAI6::PlaceType::Wall)
        tar_y += 80;
    if (fullMap[target.cellx + 1][target.celly - 1] == THUAI6::PlaceType::Wall)
    {
        tar_x -= 80;
        tar_y += 80;
    }
    if (fullMap[target.cellx + 1][target.celly] == THUAI6::PlaceType::Wall)
        tar_x -= 80;
    if (fullMap[target.cellx + 1][target.celly + 1] == THUAI6::PlaceType::Wall)
    {
        tar_x -= 80;
        tar_y -= 80;
    }
    if (fullMap[target.cellx][target.celly + 1] == THUAI6::PlaceType::Wall)
        tar_y -= 80;
    if (fullMap[target.cellx - 1][target.celly + 1] == THUAI6::PlaceType::Wall)
    {
        tar_x += 80;
        tar_y -= 80;
    }
    int dx = abs(self_x - tar_x);
    int dy = abs(self_y - tar_y);
    if (abs(dx - dy) < (dx > dy ? dx : dy) / 2)
    {
        if(dx > 60)
        {
            if(self_x < tar_x && self_y < tar_y)
                api.Move(20,PI/4);
            else if(self_x > tar_x && self_y < tar_y)
                api.Move(20,3*PI/4);
            else if(self_x > tar_x && self_y > tar_y)
                api.Move(20, 5*PI/4);
            else
                api.Move(20,7*PI/4);
            return false;
        }
        else
            return true;
    }
    else
    {
        if (dx >= 80) {
            //移动x
            if (self_x < tar_x) {
                api.MoveDown(20);
            }
            else {
                api.MoveUp(20);
            }
            return false;
        }
        else if (dy >= 80) {
            if (self_y < tar_y) {
                api.MoveRight(20);
            }
            else {
                api.MoveLeft(20);
            }
            return false;
        }
        else {
            return true;
        }
    }
}

bool TypeHandler(THUAI6::PlaceType type, CellPlace place, IStudentAPI& api) {
    for (int i = 0; i < ignorePlace.size(); i++) {
        if (ignorePlace[i].cellx == place.cellx && ignorePlace[i].celly == place.celly) {
            return false;
        }
    }
    if (type == THUAI6::PlaceType::NullPlaceType) {
        return false;
    }
    else if (type == THUAI6::PlaceType::ClassRoom) {
        auto progress = api.GetClassroomProgress(place.cellx, place.celly);
        if (progress == 10000000) {
            ignorePlace.push_back(place);
            Finished++;
            startLearning = false;
            return false;
        }
        else {
            return true;
        }
    }
    else if (type == THUAI6::PlaceType::Window) {
        return true;
    }
    else if (type == THUAI6::PlaceType::Gate) {
        return true;
    }
}