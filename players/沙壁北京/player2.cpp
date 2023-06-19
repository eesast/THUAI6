#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include<math.h>
#define PI 3.14159265358979323846
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数

static std::vector<std::vector<int>> path;
static int step[100] = { 0 };
const int kCost1 = 1;	//直移一格的消耗
const int kCost2 = 2;	//斜移一格的消耗


typedef struct _Point {
    int x, y;	//点坐标，x - 横排  y - 竖排
    int F, G, H;	//F=G+H
    struct _Point* parent;
}Point;


std::list<Point*> GetPath(Point* startPoint, Point* endPoint, IStudentAPI& api);
void ClearAstarMaze();


static std::list<Point*>openList;	// 开放列表
static std::list<Point*>closeList;	// 关闭列表

//函数声明
Point* AllocPoint(int x, int y);
/*static*/ std::vector<Point*>getSurroundPoints(const Point* point, IStudentAPI& api);
/*static*/ Point* isInList(std::list<Point*>& list, const Point* point);
/******************************
* 功能：查找openList中F值最小节点
* 输入：
*		无
* 返回：
*		F值最小节点
******************************/
/*static*/ Point* getLeastFpoint() {

    if (!openList.empty()) {
        Point* resPoint = openList.front();
        std::list<Point*>::const_iterator itor;
        for (itor = openList.begin(); itor != openList.end(); ++itor) {
            Point* p = *itor;	// itor是list<Point *>类型迭代器，所以*itor是Poit *类型
            if (p->F < resPoint->F) {
                resPoint = *itor;
            }
        }
        return resPoint;
    }

    //是空值情况
    return NULL;
}

/******************************
* 功能：计算指定结点到父节点的G值
* 输入：
*		parent - 父节点
*		point - 指定结点
* 返回：
*		指定结点到父节点的G值
******************************/
/*static*/ int calcG(Point* parrent, Point* point) {
    //如果可以直走额外消耗量为kCost1，如果需要斜走额外消耗量为kCost2
    //int extraG=abs(parrent->x - point->x) + abs(parrent->y - point->y) == 1 ? kCost1 : kCost2;
    int extraG = 2;	// 规定只能直着走
    int parentG = (point->parent == NULL ? NULL : point->parent->G);
    return parentG + extraG;
}

/******************************
* 功能：计算指定结点到终点结点的H值
* 输入：
*		point - 指定结点
*		endPoint - 终点结点
* 返回：
*		指定结点到终点结点的H值
******************************/
/*static*/ int calcH(Point* point, Point* end) {
    //考虑斜移
    //return (int)sqrt((double)(end->x - point->x) * (double)(end->x - point->x) + (double)(end->y - point->y) + (double)(end->y - point->y));
    //规定只能直走
    return sqrt((end->x - point->x) * (end->x - point->x) + (end->y - point->y) * (end->y - point->y));
}
/******************************
* 功能：计算指定结点的F值
* 输入：
*		point - 指定结点
* 返回：
*		指定结点的F值
******************************/
/*static*/ int calcF(Point* point) {
    return point->G + point->H;
}


/******************************
* 功能：A*算法实现
* 输入：
*		startPoint - 起始位置
*		endPoint - 终点位置
* 返回：
*		终点位置的结点
******************************/
/*static*/ Point* findPath(Point* startPoint, Point* endPoint, IStudentAPI& api) {
    //置入起点，拷贝开辟一格节点，内外隔离
    openList.push_back(AllocPoint(startPoint->x, startPoint->y));

    //openList不为空时，不停的取openList中L的最小值
    while (!openList.empty()) {
        //查找openList中F最小值节点
        Point* curPoint = getLeastFpoint();

        //把最小值节点放到关闭列表中
        openList.remove(curPoint);	// 从openList中移除
        closeList.push_back(curPoint);	// 放入closeList中

        //找到当前节点周围可达节点处理
        std::vector<Point*>surroundPoints = getSurroundPoints(curPoint, api);
        std::vector<Point*>::const_iterator iter;
        for (iter = surroundPoints.begin(); iter != surroundPoints.end(); ++iter) {
            Point* target = *iter;
            //对某一结点，如果不在openList中，则加入到openList中，并设置其父节点；如果在openList中，则重新计算F值，选取min(F)的结点
            Point* exist = isInList(openList, target);
            //不在openList中
            if (!exist) {
                target->parent = curPoint;
                target->G = calcG(curPoint, target);
                target->H = calcH(target, endPoint);
                target->F = calcF(target);
                openList.push_back(target);
            }// 在开放列表中，重新计算G值，保留min(F)
            else {
                int tempG = calcG(curPoint, target);
                if (tempG < target->G) {
                    exist->parent = curPoint;
                    exist->G = tempG;
                    exist->F = calcF(target);
                }
                delete target;
            }
        }	// end for
        surroundPoints.clear();

        //判断终点是否在openList中
        Point* resPoint = isInList(openList, endPoint);
        if (resPoint) {
            return resPoint;
        }
    }
    //如果while执行完毕则表明没有找到
    return NULL;
}

/******************************
* 功能：判断指定节点是否在指定链表中
* 输入：
*		list - 指定链表
*		point - 指定节点
* 返回：
*		Point* - 判断节点在链表中，返回该在链表中地址
*		NULL - 不在节点中
******************************/
/*static*/ Point* isInList(std::list<Point*>& list, const Point* point) {
    std::list<Point*>::const_iterator itor;
    for (itor = list.begin(); itor != list.end(); ++itor) {
        if ((*itor)->x == point->x && (*itor)->y == point->y) {
            return *itor;
        }
    }
    return NULL;
}

/******************************
* 功能：判断某点是否可达指定点
* 输入：
*		point - 指定节点
*		target - 待判断节点
* 返回：
*		true - 可达
*		false - 不可达
******************************/
/*static*/ bool isCanreach(const Point* point, const Point* target, IStudentAPI& api) {

    int x = target->x;
    int y = target->y;
    //待测节点超过二维数组，或待测节点是障碍物，或者待测节点与指定节点重合，或待测节点在closeList链表中。则不可达
    if (x < 0 || x >= 49 || y < 0 || y >= 49
        || (int)api.GetFullMap()[point->x][point->y] == 2
        || (int)api.GetFullMap()[point->x][point->y] == 4
        || (int)api.GetFullMap()[point->x][point->y] == 5
        || (int)api.GetFullMap()[point->x][point->y] == 11
        || (x == point->x && y == point->y)
        || isInList(closeList, target)) {

        return false;
    }
    if (abs(point->x - target->x) + abs(point->y - target->y) == 1) {
        //待测点与指定点相邻
        return true;
    }
    else {
        return false;
    }
}

/******************************
* 功能：获得当前节点的周围可达节点（这里只考虑直着走）
* 输入：
*		point - 指定节点
* 返回：
*		周围可达节点vector数组
******************************/
/*static*/ std::vector<Point*>getSurroundPoints(const Point* point, IStudentAPI& api) {
    //定义存储可达节点的数组
    std::vector<Point*>surroundPoints;

    for (int x = point->x - 1; x <= point->x + 1; ++x) {
        for (int y = point->y - 1; y <= point->y + 1; ++y) {
            Point* temp = AllocPoint(x, y);
            if (isCanreach(point, temp, api)) {
                surroundPoints.push_back(temp);
            }
            else {
                //不可达则释放资源
                delete temp;
            }
        }
    }
    return surroundPoints;
}



/******************************
* 功能：分配一格结点
* 输入：
*		x - 二维数组行
*		y - 二维数组列
* 返回：
*		已初始化的结点的指针
******************************/
Point* AllocPoint(int x, int y) {
    Point* temp = new Point;
    memset(temp, 0, sizeof(Point));	//初始值清零
    temp->x = x;
    temp->y = y;
    return temp;
}

/******************************
* 功能：调用A*算法，寻找路径
* 输入：
*		startPoint - 起始位置
*		endPoint - 终点位置
* 返回：
*		返回路径链表的头节点
******************************/
/*static*/ std::list<Point*> GetPath(Point* startPoint, Point* endPoint, IStudentAPI& api) {
    Point* result = findPath(startPoint, endPoint, api);
    std::list<Point*>path;
    //返回路径，如果没有找到路径，返回空链表
    while (result) {
        path.push_front(result);
        result = result->parent;
    }
    //path.reverse();
    return path;
}


/******************************
* 功能：资源清理
* 输入：
*		无
* 返回：
*		无
******************************/
void ClearAstarMaze() {
    std::list<Point*>::iterator itor;
    for (itor = openList.begin(); itor != openList.end();) {
        delete* itor;
        itor = openList.erase(itor);	//从链表中删除某个结点，并返回下一个结点

    }
    for (itor = closeList.begin(); itor != closeList.end();) {
        delete* itor;
        itor = closeList.erase(itor);	//从链表中删除某个结点，并返回下一个结点

    }
}

//void move_to_place(IStudentAPI& api, int target_x, int target_y)
//{
//    int time_1 = 1000000 / (api.GetSelfInfo()->speed);
//    if ((abs(api.GetSelfInfo()->x - target_x) >= 100) || (abs(api.GetSelfInfo()->y - target_y) >= 100))
//    {
//        if ((abs(api.GetSelfInfo()->y - target_y) >= 100))
//        {
//            if (api.GetSelfInfo()->y - target_y <= 100)  api.MoveRight(1000000 / double(api.GetSelfInfo()->speed));
//            else if (api.GetSelfInfo()->y - target_y >= 100) api.MoveLeft(1000000 / double(api.GetSelfInfo()->speed));
//            api.EndAllAction();
//        }
//        else if (abs(api.GetSelfInfo()->y - target_y) <= 100)
//        {
//            if (api.GetSelfInfo()->x - target_x <= 100)  api.MoveDown(1000000 / double(api.GetSelfInfo()->speed));
//            else if (api.GetSelfInfo()->x - target_x >= 100) api.MoveUp(1000000 / double(api.GetSelfInfo()->speed));
//            api.EndAllAction();
//        }
//
//        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000000 / double(api.GetSelfInfo()->speed) + 50)));
//    }
//}

void autoMove(int target_x, int target_y, IStudentAPI& api) {
    int man_x = api.GridToCell(api.GetSelfInfo()->x);
    int man_y = api.GridToCell(api.GetSelfInfo()->y);
    Point* start = AllocPoint(man_x, man_y);
    Point* end = AllocPoint(target_x, target_y);
    std::list<Point*>path1 = GetPath(start, end, api);
    std::list<Point*>::const_iterator iter;
    for (iter = path1.begin(); iter != path1.end(); ++iter) {
        Point* test = *iter;
        std::vector<int> a{ test->x ,test->y };
        path.push_back(a);
        std::cout << "x: " << test->x << std::endl;
        std::cout << "y: " << test->y << std::endl;
    }
    //for (iter = path1.begin(); iter != path1.end(); ++iter) {
    //    Point* cur = *iter;
    //    api.PrintSelfInfo();
    //    move:cellMove( cur->x , cur->y,api );
    //    std::this_thread::sleep_for(std::chrono::milliseconds(int(1000000 / double(api.GetSelfInfo()->speed) + 80)));
    //    if ((abs(api.GridToCell(api.GetSelfInfo()->x) - cur->x) <= 10) && (abs(api.GridToCell(api.GetSelfInfo()->y) - cur->y) <= 10))
    //        continue;
    //    else goto move;
    //    /*while((abs(api.GridToCell(api.GetSelfInfo()->x) - cur->x) >= 5) || (abs(api.GridToCell(api.GetSelfInfo()->y) - cur->y) >= 5))
    //        cellMove(cur->x, cur->y, api);*/
    //    /*std::cout << "x: " << man_x << std::endl;
    //    std::cout << "y: " << man_y << std::endl;
    //    man_x = cur->x;
    //    man_y = cur->y;*/
    //    std::this_thread::sleep_for(std::chrono::milliseconds(int(1000000 / double(api.GetSelfInfo()->speed) + 80)));
    //}
    //if ((abs(api.GetSelfInfo()->x - target_x) <= 100) && (abs(api.GetSelfInfo()->y - target_y) <= 100))
       ClearAstarMaze();
}

//


void cellMove(uint32_t X, uint32_t Y, IStudentAPI& api)
{
    auto x = api.GetSelfInfo()->x;
    auto y = api.GetSelfInfo()->y;
    auto x_cell = api.GridToCell(x);
    auto y_cell = api.GridToCell(y);
    auto X_grid = api.CellToGrid(X);
    auto Y_grid = api.CellToGrid(Y);
    auto time_1 = 1000000 / double(api.GetSelfInfo()->speed);
    if ((Y_grid - y) > 10)
        if ((Y_grid - y) > 500)
            api.MoveRight(100);
        else
            api.MoveRight((Y_grid - y) * 1000 / api.GetSelfInfo()->speed);
    else
    {
        if ((y - Y_grid) >= 10)
            if ((y - Y_grid) > 500)
                api.MoveLeft(100);
            else
                api.MoveLeft((y - Y_grid) * 1000 / api.GetSelfInfo()->speed);
        else
        {
            if ((X_grid - x) > 10)
                if ((X_grid - x) > 500)
                    api.MoveDown(100);
                else
                    api.MoveDown((X_grid - x) * 1000 / api.GetSelfInfo()->speed);

            else
                if ((x - X_grid) >= 10)
                    if ((x - X_grid) > 500)
                        api.MoveUp(100);
                    else
                        api.MoveUp((x - X_grid) * 1000 / api.GetSelfInfo()->speed);
        }
    }
}
void Move_tour(std::vector<std::vector<int>>& v, IStudentAPI& api)
{
    if (v.size() > 0)
    {
        cellMove(v[0][0], v[0][1], api);
        int i = 0;
        for (i = 0; i < v.size() - 1; i++)
        {
            if (int(api.GetFullMap()[v[i + 1][0]][v[i + 1][1]]) == 7)
            {
                api.SkipWindow();
                std::this_thread::sleep_for(std::chrono::milliseconds(5000 / 3));
                i++;
            }
            else
            {
                if (v[i + 1][1] == v[i][1])
                {
                    if (v[i + 1][0] > v[i][0])
                    {
                        api.MoveDown(1000000 / double(api.GetSelfInfo()->speed));
                    }
                    else
                    {
                        api.MoveUp(1000000 / double(api.GetSelfInfo()->speed));
                    }
                }
                else
                {
                    if (v[i + 1][1] > v[i][1])
                    {
                        api.MoveRight(1000000 / double(api.GetSelfInfo()->speed));
                    }
                    else
                    {
                        api.MoveLeft(1000000 / double(api.GetSelfInfo()->speed));
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000000 / double(api.GetSelfInfo()->speed) + 50)));
        }
    }

}
void setele(std::vector<std::vector<int>>& v, int x, int y, int num)
{
    std::vector<std::vector<int>>::iterator itr = v.begin();
    std::vector<int> cell{ x,y };
    v[num] = cell;
}
void set(std::vector<std::vector<int>>& v, int x, int y, int z)
{
    std::vector<int> cell{ x,y,z };
    v.push_back(cell);
}
int judge(std::vector<std::vector<int>> v, std::vector<int> v2)
{
    int i = 0;
    for (i = 0; i < v.size(); i++)
    {
        if (v[i][0] == v2[0] && v[i][1] == v2[1])
            return 0;
    }
    return 1;
}
int autoMove(uint32_t X, uint32_t Y, IStudentAPI& api, int n)
{
    auto x = api.GetSelfInfo()->x;
    auto y = api.GetSelfInfo()->y;
    auto x_c = api.GridToCell(x);
    auto y_c = api.GridToCell(y);
    int i = 0;
    if (step[n] != 2)
    {
        if (step[n] == 0)
        {
            autoMove(X, Y, api);
        }
        if (!(path.empty()))
        {
            if (abs(x - api.CellToGrid(path[0][0])) < 10 && abs(y - api.CellToGrid(path[0][1])) < 10)
            {
                path.erase(path.begin());
            }
            if (!(path.empty()))
            {
                if (int(api.GetFullMap()[path[0][0]][path[0][1]]) == 7)
                {
                    api.SkipWindow();
                    path.erase(path.begin());
                    std::this_thread::sleep_for(std::chrono::milliseconds(5000 / 3));
                }
                else
                    cellMove(path[0][0], path[0][1], api);
            }

        }
        else
        {
            step[n] = 2;
            return 1;
        }
        step[n] = 1;
        return 0;
    }
    else
    return 1;
}

void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        /*api.PrintSelfInfo();
        cellMove(23, 12, api);*/
        if (autoMove(29, 7, api, 0))
        {
            api.UseSkill(0);
            api.StartLearning();
        }
        if (api.GetClassroomProgress(30, 7) == 10000000)
            if (autoMove(18, 6, api, 1))
            {
                api.UseSkill(0);
                api.StartLearning();
            }
        if (api.GetClassroomProgress(18, 5) == 10000000)
            if (autoMove(21, 18, api, 2))
            {
                api.UseSkill(0);
                api.StartLearning();
            }
        if (api.GetClassroomProgress(22, 18) == 10000000)
            if (autoMove(6, 6, api, 3))
                api.StartOpenGate();
        if(api.GetGateProgress(5,6)==18000)
                api.Graduate();
        api.Wait();
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        if (autoMove(18, 40, api, 0))
        {
            api.UseSkill(0);
            api.StartLearning();
        }
        if (api.GetClassroomProgress(19, 41) == 10000000)
            if (autoMove(10, 37, api, 1))
            {
                api.UseSkill(0);
                api.StartLearning();
            }
        if (api.GetClassroomProgress(10, 38) == 10000000)
        {
            if (autoMove(8, 32, api, 2))
            {
                api.UseSkill(0);
                api.StartLearning();
            }
        }
        if (api.GetClassroomProgress(8, 31) == 10000000)
        {
            if (autoMove(5, 5, api, 3))
            {
                api.StartOpenGate();
                api.Graduate();
            }
        }
        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        if (autoMove(7, 23, api, 0))
            api.UseSkill(0);
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        if (autoMove(44, 31, api, 0))
        {
            api.StartLearning();
        }
        if (api.GetClassroomProgress(44, 32) == 10000000)
        {
            api.UseSkill(2);
            if (autoMove(40, 13, api, 1))
                api.StartLearning();
        }
        if (api.GetClassroomProgress(40, 12) == 10000000)
        {
            api.UseSkill(2);
            if (autoMove(34, 40, api, 2))
                api.StartLearning();
        }
        if (api.GetClassroomProgress(33, 40) == 10000000)
        {
            api.UseSkill(2);
            if (autoMove(46, 46, api, 3))
                api.StartOpenGate();
            api.Graduate();
        }
        // 玩家3执行操作
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

std::list<Point*> GetPath(Point* startPoint, Point* endPoint, ITrickerAPI& api);
std::vector<Point*>getSurroundPoints(const Point* point, ITrickerAPI& api);
Point* findPath(Point* startPoint, Point* endPoint, ITrickerAPI& api) {
    //置入起点，拷贝开辟一格节点，内外隔离
    openList.push_back(AllocPoint(startPoint->x, startPoint->y));

    //openList不为空时，不停的取openList中L的最小值
    while (!openList.empty()) {
        //查找openList中F最小值节点
        Point* curPoint = getLeastFpoint();

        //把最小值节点放到关闭列表中
        openList.remove(curPoint);	// 从openList中移除
        closeList.push_back(curPoint);	// 放入closeList中

        //找到当前节点周围可达节点处理
        std::vector<Point*>surroundPoints = getSurroundPoints(curPoint, api);
        std::vector<Point*>::const_iterator iter;
        for (iter = surroundPoints.begin(); iter != surroundPoints.end(); ++iter) {
            Point* target = *iter;
            //对某一结点，如果不在openList中，则加入到openList中，并设置其父节点；如果在openList中，则重新计算F值，选取min(F)的结点
            Point* exist = isInList(openList, target);
            //不在openList中
            if (!exist) {
                target->parent = curPoint;
                target->G = calcG(curPoint, target);
                target->H = calcH(target, endPoint);
                target->F = calcF(target);
                openList.push_back(target);
            }// 在开放列表中，重新计算G值，保留min(F)
            else {
                int tempG = calcG(curPoint, target);
                if (tempG < target->G) {
                    exist->parent = curPoint;
                    exist->G = tempG;
                    exist->F = calcF(target);
                }
                delete target;
            }
        }	// end for
        surroundPoints.clear();
        //判断终点是否在openList中
        Point* resPoint = isInList(openList, endPoint);
        if (resPoint) {
            return resPoint;
        }
    }
    //如果while执行完毕则表明没有找到
    return NULL;
}
bool isCanreach(const Point* point, const Point* target, ITrickerAPI& api) {

    int x = target->x;
    int y = target->y;
    //待测节点超过二维数组，或待测节点是障碍物，或者待测节点与指定节点重合，或待测节点在closeList链表中。则不可达
    if (x < 0 || x >= 49 || y < 0 || y >= 49
        || (int)api.GetFullMap()[point->x][point->y] == 2
        || (int)api.GetFullMap()[point->x][point->y] == 4
        || (int)api.GetFullMap()[point->x][point->y] == 5
        || (int)api.GetFullMap()[point->x][point->y] == 11
        || (x == point->x && y == point->y)
        || isInList(closeList, target)) {

        return false;
    }
    if (abs(point->x - target->x) + abs(point->y - target->y) == 1) {
        //待测点与指定点相邻
        return true;
    }
    else {
        return false;
    }
}
std::vector<Point*>getSurroundPoints(const Point* point, ITrickerAPI& api) {
    //定义存储可达节点的数组
    std::vector<Point*>surroundPoints;

    for (int x = point->x - 1; x <= point->x + 1; ++x) {
        for (int y = point->y - 1; y <= point->y + 1; ++y) {
            Point* temp = AllocPoint(x, y);
            if (isCanreach(point, temp, api)) {
                surroundPoints.push_back(temp);
            }
            else {
                //不可达则释放资源
                delete temp;
            }
        }
    }
    return surroundPoints;
}
std::list<Point*> GetPath(Point* startPoint, Point* endPoint, ITrickerAPI& api) {
    Point* result = findPath(startPoint, endPoint, api);
    std::list<Point*>path;
    //返回路径，如果没有找到路径，返回空链表
    while (result) {
        path.push_front(result);
        result = result->parent;
    }
    //path.reverse();
    return path;
}
void autoMove(int target_x, int target_y, ITrickerAPI& api) {
    int man_x = api.GridToCell(api.GetSelfInfo()->x);
    int man_y = api.GridToCell(api.GetSelfInfo()->y);
    Point* start = AllocPoint(man_x, man_y);
    Point* end = AllocPoint(target_x, target_y);
    std::list<Point*>path1 = GetPath(start, end, api);
    std::list<Point*>::const_iterator iter;
    for (iter = path1.begin(); iter != path1.end(); ++iter) {
        Point* test = *iter;
        std::vector<int> a{ test->x ,test->y };
        path.push_back(a);
        std::cout << "x: " << test->x << std::endl;
        std::cout << "y: " << test->y << std::endl;
    }
    ClearAstarMaze();
}
void cellMove(uint32_t X, uint32_t Y, ITrickerAPI& api)
{
    auto x = api.GetSelfInfo()->x;
    auto y = api.GetSelfInfo()->y;
    auto x_cell = api.GridToCell(x);
    auto y_cell = api.GridToCell(y);
    auto X_grid = api.CellToGrid(X);
    auto Y_grid = api.CellToGrid(Y);
    auto time_1 = 1000000 / double(api.GetSelfInfo()->speed);
    if ((Y_grid - y) > 10)
        if ((Y_grid - y) > 500)
            api.MoveRight(200);
        else
            api.MoveRight((Y_grid - y) * 1000 / api.GetSelfInfo()->speed);
    else
    {
        if ((y - Y_grid) >= 10)
            if ((y - Y_grid) > 500)
                api.MoveLeft(200);
            else
                api.MoveLeft((y - Y_grid) * 1000 / api.GetSelfInfo()->speed);
        else
        {
            if ((X_grid - x) > 10)
                if ((X_grid - x) > 500)
                    api.MoveDown(200);
                else
                    api.MoveDown((X_grid - x) * 1000 / api.GetSelfInfo()->speed);

            else
                if ((x - X_grid) >= 10)
                    if ((x - X_grid) > 500)
                        api.MoveUp(200);
                    else
                        api.MoveUp((x - X_grid) * 1000 / api.GetSelfInfo()->speed);
        }
    }
}
void Move_tour(std::vector<std::vector<int>>& v, ITrickerAPI& api)
{
    if (v.size() > 0)
    {
        cellMove(v[0][0], v[0][1], api);
        int i = 0;
        for (i = 0; i < v.size() - 1; i++)
        {
            if (int(api.GetFullMap()[v[i + 1][0]][v[i + 1][1]]) == 7)
            {
                api.SkipWindow();
                std::this_thread::sleep_for(std::chrono::milliseconds(5000 / 3));
                i++;
            }
            else
            {
                if (v[i + 1][1] == v[i][1])
                {
                    if (v[i + 1][0] > v[i][0])
                    {
                        api.MoveDown(1000000 / double(api.GetSelfInfo()->speed));
                    }
                    else
                    {
                        api.MoveUp(1000000 / double(api.GetSelfInfo()->speed));
                    }
                }
                else
                {
                    if (v[i + 1][1] > v[i][1])
                    {
                        api.MoveRight(1000000 / double(api.GetSelfInfo()->speed));
                    }
                    else
                    {
                        api.MoveLeft(1000000 / double(api.GetSelfInfo()->speed));
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000000 / double(api.GetSelfInfo()->speed) + 50)));
        }
    }

}
int autoMove(uint32_t X, uint32_t Y, ITrickerAPI& api, int n)
{
    auto x = api.GetSelfInfo()->x;
    auto y = api.GetSelfInfo()->y;
    auto x_c = api.GridToCell(x);
    auto y_c = api.GridToCell(y);
    int i = 0;
    if (step[n] != 2)
    {
        if (step[n] == 0)
        {
            autoMove(X, Y, api);
        }
        if (!(path.empty()))
        {
            if (abs(x - api.CellToGrid(path[0][0])) < 10 && abs(y - api.CellToGrid(path[0][1])) < 10)
            {
                path.erase(path.begin());
            }
            if (!(path.empty()))
            {
                if (int(api.GetFullMap()[path[0][0]][path[0][1]]) == 7)
                {
                    api.SkipWindow();
                    path.erase(path.begin());
                    std::this_thread::sleep_for(std::chrono::milliseconds(5000 / 3));
                }

                else
                    cellMove(path[0][0], path[0][1], api);
            }

        }
        else
        {
            step[n] = 2;
            return 1;
        }
        step[n] = 1;
        return 0;
    }
    else
        return 1;
}

static int fra1 = 0, fra2;

void AI::play(ITrickerAPI& api)
{
    if (autoMove(20, 17, api, 50));
    if (api.GetStudents().size() != 0)

        if (api.GetStudents()[0]->playerID == 0 || api.GetStudents()[0]->playerID == 1 || api.GetStudents()[0]->playerID == 2 || api.GetStudents()[0]->playerID == 3)
        {
            std::vector< std::shared_ptr<const THUAI6::Student>> a1 = api.GetStudents();
            if (a1.size() != 0)
            {

                uint32_t length;
                double angel;
                std::shared_ptr<const THUAI6::Student> a = api.GetStudents()[0];

                std::shared_ptr<const THUAI6::Tricker> b = api.GetSelfInfo();
                std::vector<double> hhh = b->timeUntilSkillAvailable;
                if (hhh.size() >= 2)
                {
                    if (a->determination != 0)
                    {
                        if (b->timeUntilSkillAvailable[0] == 0.0)
                            api.UseSkill(0);
                        if (b->timeUntilSkillAvailable[1] == 0.0)
                            api.UseSkill(1);
                    }
                    length = (int)pow(pow((a->x - b->x), 2) + pow((a->y - b->y), 2), 0.5);
                    if ((api.GridToCell(a->y) - api.GridToCell(b->y)) == 0)
                    {
                        if ((a->x - b->x) > 0)
                            angel = 0;
                        else
                            angel = PI;
                    }
                    else if ((api.GridToCell(a->x) - api.GridToCell(b->x)) == 0)
                    {
                        if ((a->y - b->y) > 0)
                            angel = 0.5 * PI;
                        if ((a->y - b->y) < 0)
                            angel = 1.5 * PI;
                    }
                    else if ((a->x - b->x) < 0 && (a->y - b->y) > 0)
                        angel = atan((a->y - b->y) / (a->x - b->x)) + PI;
                    else if ((a->x - b->x) < 0 && (a->y - b->y) < 0)
                        angel = atan((a->y - b->y) / (a->x - b->x)) + PI;
                    else if ((a->x - b->x) > 0 && (a->y - b->y) < 0)
                        angel = atan((a->y - b->y) / (a->x - b->x)) + 2 * PI;
                    else if ((a->x - b->x) > 0 && (a->y - b->y) > 0)
                        angel = atan((a->y - b->y) / (a->x - b->x));

                    if (a->studentType == THUAI6::StudentType::Teacher);
                    else
                    {
                        if (a->determination == 0)
                            api.EndAllAction();
                        else {
                            fra2 = api.GetFrameCount();
                            if (fra2 - fra1 >= 10)
                            {
                                fra1 = fra2;
                                step[0] = 0;
                            }
                            if (autoMove(api.GridToCell(a->x), api.GridToCell(a->y), api, 0))
                                step[0] = 0;

                            if (a->determination != 0)
                            {
                                if (b->timeUntilSkillAvailable.size() != 0)
                                {
                                    if (b->timeUntilSkillAvailable[1] > 29000)
                                        api.Attack(angel);

                                    else if (length <= 1500)
                                        api.Attack(angel);
                                }
                            }

                        }
                    }
                }
            }
        }

        else
        {



            int x, y;
            int32_t a[10], c[10], b[20] = { 18,5, 30,7, 22,18, 40,12,8,31,10,38, 19,41, 28,26, 33,40, 44,32 };

            for (int k = 0; k < 10; k++)
            {
                x = b[2 * k];
                y = b[2 * k + 1];

                if (autoMove(x, y, api, 2))
                {
                    if (api.GetClassroomProgress(x, y) >= 3000000)
                    {
                        std::shared_ptr<const THUAI6::Tricker> b;
                        double angle;
                        if ((api.GridToCell(y) - api.GridToCell(b->y)) == 0)
                        {
                            if ((x - b->x) > 0)
                                angle = 0;
                            else
                                angle = PI;
                        }
                        else if ((api.GridToCell(x) - api.GridToCell(b->x)) == 0)
                        {
                            if ((y - b->y) > 0)
                                angle = 0.5 * PI;
                            if ((y - b->y) < 0)
                                angle = 1.5 * PI;
                        }
                        else if ((x - b->x) < 0 && (y - b->y) > 0)
                            angle = atan((y - b->y) / (x - b->x)) + PI;
                        else if ((x - b->x) < 0 && (y - b->y) < 0)
                            angle = atan((y - b->y) / (x - b->x)) + PI;
                        else
                            angle = atan((y - b->y) / (x - b->x));
                        api.Attack(angle);
                    }
                    step[2] = 0;
                }
            }
        }
}
