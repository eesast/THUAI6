#include <vector>
#include <thread>
#include <array>
#include <queue>
#include <stack>
#include <cmath>
#include "AI.h"
#include "constants.h"


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
const double PI = 3.14159265358979;
bool _isMapGetted = false;
int Finished = 0;
int Arrived = 0;
std::vector<std::vector<THUAI6::PlaceType>> fullMap;
std::vector<CellPlace> ignorePlace;
std::vector<CellPlace> ignorePlace1;
std::stack<CellPlace> walkPath;
THUAI6::PlaceType nextType;
std::vector<CellPlace> classrooms;

std::stack<CellPlace> GetPath(THUAI6::PlaceType type, CellPlace begin, ITrickerAPI& api);
std::stack<CellPlace> TraceBack(std::vector<Path>& allPoint);
bool TypeHandler(THUAI6::PlaceType type, CellPlace place, ITrickerAPI& api);
bool AutoMove(CellPlace target, std::shared_ptr<const THUAI6::Tricker> info, ITrickerAPI& api);

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
    friend std::stack<CellPlace> AStarSearch(CellPlace, std::vector<CellPlace>, const std::vector<std::vector<THUAI6::PlaceType>>&);
};

int DiagDist(CellPlace from, CellPlace to)
{
    int dx = from.cellx - to.cellx;
    if (dx < 0) dx = -dx;
    int dy = from.celly - to.celly;
    if (dy < 0) dy = -dy;
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

bool Walkable(THUAI6::PlaceType loc, int cellx, int celly)
{
    switch (loc)
    {
    case THUAI6::PlaceType::Land:
        return true;
    case THUAI6::PlaceType::Grass:
        return true;
    case THUAI6::PlaceType::Window:
        return true;
    case THUAI6::PlaceType::Door3:
        if (SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
    case THUAI6::PlaceType::Door5:
        if (SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
    case THUAI6::PlaceType::Door6:
        if (SelfDoorOpen(cellx, celly))
        {
            return true;
        }
        return false;
    default:
        return false;
    }
}

std::stack<CellPlace> AStarSearch(CellPlace begin, std::vector<CellPlace> end, const std::vector<std::vector<THUAI6::PlaceType>>& map)
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

    while (!todo.empty())
    {
        available = 0;
        searchCenter = todo.top();
        todo.pop();
        if (visited[searchCenter.loc.cellx][searchCenter.loc.celly] == 0)
        {
            visited[searchCenter.loc.cellx][searchCenter.loc.celly] = 16;
            distance[searchCenter.loc.cellx][searchCenter.loc.celly] = 0;
        }

        if (Walkable(map[searchCenter.loc.cellx + 1][searchCenter.loc.celly],
            searchCenter.loc.cellx + 1, searchCenter.loc.celly))
            available |= 1;
        if (searchCenter.loc.cellx != 49
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly])
        {
            if (searchCenter.next(1).in(end))
            {
                todo.push(searchCenter.next(1));
                visited[searchCenter.loc.cellx + 1][searchCenter.loc.celly] = 1;
                endPoint = searchCenter.next(1).loc;
                break;
            }
            else if (available & 1)
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
            searchCenter.loc.cellx, searchCenter.loc.celly + 1))
            available |= 2;
        if (searchCenter.loc.celly != 49
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx][searchCenter.loc.celly + 1])
        {
            if (searchCenter.next(3).in(end))
            {
                todo.push(searchCenter.next(3));
                visited[searchCenter.loc.cellx][searchCenter.loc.celly + 1] = 3;
                endPoint = searchCenter.next(3).loc;
                break;
            }
            else if (available & 2)
            {
                visited[searchCenter.loc.cellx][searchCenter.loc.celly + 1] = 3;
                distance[searchCenter.loc.cellx][searchCenter.loc.celly + 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(3);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }

        if (Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly],
            searchCenter.loc.cellx - 1, searchCenter.loc.celly))
            available |= 4;
        if (searchCenter.loc.cellx != 0
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly])
        {
            if (searchCenter.next(5).in(end))
            {
                todo.push(searchCenter.next(5));
                visited[searchCenter.loc.cellx - 1][searchCenter.loc.celly] = 5;
                endPoint = searchCenter.next(5).loc;
                break;
            }
            else if (available & 4)
            {
                visited[searchCenter.loc.cellx - 1][searchCenter.loc.celly] = 5;
                distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10;
                pushBuf = searchCenter.next(5);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
            }
        }

        if (Walkable(map[searchCenter.loc.cellx][searchCenter.loc.celly - 1],
            searchCenter.loc.cellx, searchCenter.loc.celly - 1))
            available |= 8;
        if (searchCenter.loc.celly != 0
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 10
            < distance[searchCenter.loc.cellx][searchCenter.loc.celly - 1])
        {
            if (searchCenter.next(7).in(end))
            {
                todo.push(searchCenter.next(7));
                visited[searchCenter.loc.cellx][searchCenter.loc.celly - 1] = 7;
                endPoint = searchCenter.next(7).loc;
                break;
            }
            else if (available & 8)
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
            if (Walkable(map[searchCenter.loc.cellx + 1][searchCenter.loc.celly + 1],
                searchCenter.loc.cellx + 1, searchCenter.loc.celly + 1))
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
            if (Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly + 1],
                searchCenter.loc.cellx - 1, searchCenter.loc.celly + 1))
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
            if (Walkable(map[searchCenter.loc.cellx - 1][searchCenter.loc.celly - 1],
                searchCenter.loc.cellx - 1, searchCenter.loc.celly - 1))
            {
                pushBuf = searchCenter.next(6);
                pushBuf.rank = pushBuf.dist + DiagDist(pushBuf.loc, end);
                todo.push(pushBuf);
                visited[pushBuf.loc.cellx][pushBuf.loc.celly] = 6;
                distance[searchCenter.loc.cellx - 1][searchCenter.loc.celly - 1]
                    = distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14;
            }
        }

        if ((available & 9) == 9
            && distance[searchCenter.loc.cellx][searchCenter.loc.celly] + 14
            < distance[searchCenter.loc.cellx + 1][searchCenter.loc.celly - 1])
        {
            if (Walkable(map[searchCenter.loc.cellx + 1][searchCenter.loc.celly - 1],
                searchCenter.loc.cellx + 1, searchCenter.loc.celly - 1))
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
    if (todo.empty())
        throw "unreachable";
    while (!todo.empty())
        todo.pop();
    bool finished = false;
    CellPlace point = endPoint;
    std::stack<CellPlace> ans;
    while (!finished)
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
    if (!_isMapGetted) {
        _isMapGetted = true;
        fullMap = api.GetFullMap();
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
        for (int iter1 = 0; iter1 < 50; iter1++)
        {
            for (int iter2 = 0; iter2 < 50; iter2++)
            {
                if (fullMap[iter1][iter2] == THUAI6::PlaceType::ClassRoom)
                {
                    classrooms.push_back({ iter1, iter2 });
                }
            }
        }
    }
    auto info = api.GetSelfInfo();
    api.PrintSelfInfo();

    if (api.GetStudents().size() > 0) {
        api.UseSkill(0);
        CellPlace TarPlace;
        TarPlace.cellx = api.GetStudents()[0].get()->x;
        TarPlace.celly = api.GetStudents()[0].get()->y;
        CellPlace selfPlace;
        selfPlace.cellx = api.GridToCell(info.get()->x);
        selfPlace.celly = api.GridToCell(info.get()->y);
        CellPlace Delta;
        Delta.cellx = TarPlace.cellx - selfPlace.cellx;
        Delta.celly = TarPlace.celly - selfPlace.celly;
        double Sigma = (double)Delta.celly / (double)Delta.cellx;
        if (Sigma < 0) Sigma *= -1;
        Sigma = atan(Sigma);
        if ((Delta.cellx > 0) && (Delta.celly > 0)) Sigma = PI / 2*3 + Sigma;
        else if ((Delta.cellx < 0) && (Delta.celly > 0)) Sigma = Sigma;
        else if ((Delta.cellx < 0) && (Delta.celly < 0)) Sigma = PI / 2 + Sigma;
        else if ((Delta.cellx > 0) && (Delta.celly < 0)) Sigma = PI +  Sigma;
        api.Attack(Sigma);
    }

    if (!walkPath.empty()) {
        CellPlace nextPoint = walkPath.top();
        bool res = AutoMove(nextPoint, info, api);
        if (res) {
            walkPath.pop();
            if (walkPath.size() == 1) {
                Arrived++;
                ignorePlace1.push_back(walkPath.top());
                walkPath.pop();
                if (Arrived > 9) ignorePlace1.erase(ignorePlace1.begin() + 0, ignorePlace1.begin() + 1);
            }
        }
    }
    else {
        CellPlace selfPlace;
        selfPlace.cellx = api.GridToCell(info.get()->x);
        selfPlace.celly = api.GridToCell(info.get()->y);
        walkPath = AStarSearch(selfPlace, classrooms, fullMap);
        nextType = THUAI6::PlaceType::ClassRoom;
    }

    if (Finished >= 7) {
        //去校门
        while (!walkPath.empty()) {
            walkPath.pop();
        }
        CellPlace selfPlace;
        selfPlace.cellx = api.GridToCell(info.get()->x);
        selfPlace.celly = api.GridToCell(info.get()->y);
        walkPath = GetPath(THUAI6::PlaceType::Gate, selfPlace, api);
        nextType = THUAI6::PlaceType::Gate;
    }
    api.MoveRight(40);
}

std::stack<CellPlace> GetPath(THUAI6::PlaceType type, CellPlace begin, ITrickerAPI& api) {
    int i = -1;
    bool visited[50][50] = { false };
    std::queue<Path> search;
    std::vector<Path> path;
    Path fstPath;
    fstPath.place = begin;
    fstPath.lastIndex = i;
    search.push(fstPath);
    while (!search.empty()) {
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
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window) {
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
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window) {
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
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window) {
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
            else if (linkType == THUAI6::PlaceType::Land || linkType == THUAI6::PlaceType::Window) {
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

bool AutoMove(CellPlace target, std::shared_ptr<const THUAI6::Tricker> info, ITrickerAPI& api) {
    int tar_x = api.CellToGrid(target.cellx);
    int tar_y = api.CellToGrid(target.celly);
    int self_x = info.get()->x;
    int self_y = info.get()->y;
    if (fullMap[target.cellx][target.celly] == THUAI6::PlaceType::Window) {
        api.SkipWindow();
    }
    int dx = abs(self_x - tar_x);
    int dy = abs(self_y - tar_y);
    if (abs(dx - dy) < (dx > dy ? dx : dy) / 2)
    {
        if (dx > 60)
        {
            if (self_x < tar_x && self_y < tar_y)
                api.Move(20, PI / 4);
            else if (self_x > tar_x && self_y < tar_y)
                api.Move(20, 3 * PI / 4);
            else if (self_x > tar_x && self_y > tar_y)
                api.Move(20, 5 * PI / 4);
            else
                api.Move(20, 7 * PI / 4);
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

bool TypeHandler(THUAI6::PlaceType type, CellPlace place, ITrickerAPI& api) {
    for (int i = 0; i < ignorePlace.size(); i++) {
        if (ignorePlace[i].cellx == place.cellx && ignorePlace[i].celly == place.celly) {
            return false;
        }
    }
    for (int i = 0; i < ignorePlace1.size(); i++) {
        if (ignorePlace1[i].cellx == place.cellx && ignorePlace1[i].celly == place.celly) {
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