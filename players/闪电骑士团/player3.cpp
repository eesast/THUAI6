#include <vector>
#include <thread>
#include <array>
#include <algorithm>
#include "AI.h"
#include "constants.h"
using std::vector;
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;


// 可以在AI.cpp内部声明变量与函数

//全局变量
//1.整张图的地形信息
std::vector<std::vector<THUAI6::PlaceType>> FullMapInfo;

//2.存储格点坐标的结构体
//(1)存储格子坐标
struct cell {
    int x;
    int y;
};

//(2)存储绝对坐标
struct grid {
    int x;
    int y;
};

//两种坐标间的转换函数
inline grid CellToGrid(cell c)
{
    grid g = { IAPI::CellToGrid(c.x),IAPI::CellToGrid(c.y) };
    return g;
}
inline cell GridToCell(grid g)
{
    cell c = { IAPI::GridToCell(g.x),IAPI::GridToCell(g.y) };
    return c;
}

//3.各种重要地点信息
//箱子位置信息
std::vector<cell> ChestPlace;

//教学楼门位置信息
std::vector<cell> Door3Place, Door5Place, Door6Place;

//教室位置信息
std::vector<cell> ClassRoomPlace;

//大门位置信息
std::vector<cell> GatePlace;

//4.当两个地点之间最短路消耗时间不小于此值时，认为两个地点相互不可达
const double TIME_INF = 32768.0;

//step1: 给定玩家当前位置和目标位置，计算连接这两个位置的最短路径 (此处最短指时间最短)
//实现方式:
std::vector<cell> shortcut(cell start, cell end, IStudentAPI& api);
std::vector<cell> shortcut(cell start, cell end, ITrickerAPI& api);
//start和end分别是起点和终点的坐标(按地图格数计)，返回一个元素为cell的vector，其中按序排列从起点沿api对应玩家最短路径走到终点经过的格点坐标
//为了找出时间最短的路径，可能需要自己定义数组存储每个玩家的移动速度、翻窗速度、是否有钥匙、开锁门速度等
//再定义一个辅助函数，返回连接两个位置的最短路径消耗的时间(参数含义与shortcut()参数含义相同):
double timecost(cell start, cell end, IStudentAPI& api);
double timecost(cell start, cell end, ITrickerAPI& api);

//step2: 给定路径和玩家当前位置，计算出下一步移动的方向
//实现方式:
int Guide(grid& cur, std::vector<cell>& path, IStudentAPI& api);
int Guide(grid& cur, std::vector<cell>& path, ITrickerAPI& api);
//cur为玩家当前坐标，path为规划好的路径(其中按序排列从起点走到终点经过的格点坐标)，该函数调用api.Move()函数使玩家向合适的方向移动，返回值可以表示是否到达目的地等信息

//返回dest_v中距离start时间最近的地点
cell nearest_place(cell start, std::vector<cell>& dest_v, IStudentAPI& api);
cell nearest_place(cell start, std::vector<cell>& dest_v, ITrickerAPI& api);

//策略：0为开心果，1为老师，2为运动员，3为学霸
//0和3不断地寻找最近的教室并写作业，写完7份作业后到大门申请毕业
//1和2去开箱子，找到钥匙后，立即去开教学楼门

//之后的计划
//通信方案：第（20n）帧，所有学生向其他学生发送自己的坐标
//第（20n+1）帧，所有学生接收其他学生的坐标
//第（20n+2）帧，所有学生根据其他学生的坐标重新计算最短路

void AI::play(IStudentAPI& api)  // 每帧执行一次AI::play(IStudentAPI& api)或AI::play(ITrickerAPI& api)（除非执行该函数超过一帧50ms），获取的信息都是这一帧的开始的状态
{
    // 公共操作
    static bool started = false;

    //游戏开始，获取地形信息并提取重要地点坐标
    if (!started)
    {
        FullMapInfo = api.GetFullMap();
        int i, j;
        cell temp;
        for (i = 0; i < (int)FullMapInfo.size(); i++)
        {
            for (j = 0; j < (int)FullMapInfo[i].size(); j++)
            {
                temp.x = i;
                temp.y = j;
                switch (FullMapInfo[i][j])
                {
                case THUAI6::PlaceType::Chest:
                    ChestPlace.emplace_back(temp); break;
                case THUAI6::PlaceType::Door3:
                    Door3Place.emplace_back(temp); break;
                case THUAI6::PlaceType::Door5:
                    Door5Place.emplace_back(temp); break;
                case THUAI6::PlaceType::Door6:
                    Door6Place.emplace_back(temp); break;
                case THUAI6::PlaceType::ClassRoom:
                    ClassRoomPlace.emplace_back(temp); break;
                case THUAI6::PlaceType::Gate:
                    GatePlace.emplace_back(temp); break;
                }
            }
        }
        started = true;
    }
    //以下为所有玩家都需要用到的变量
    grid current_place = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };//玩家当前位置
    static cell nearest_dest = { 0,0 };//最近的目的地
    double nearest_dest_time;//到达最近的目的地消耗的时间
    static std::vector<cell> cur_path;//当前行走路径
    static bool tracking = false;//是否正在沿路径行走
    if (this->playerID == 0)
    {
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)//如果玩家可移动
        {
            if (api.GetGameInfo()->subjectFinished < 7)//如果没写完7份作业，则找最近的教室写作业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //更新一下ClassRoomPlace信息，如果有作业已经写完了，就将其坐标从ClassRoomPlace中剔除
                    auto itr = ClassRoomPlace.begin();
                    while (itr != ClassRoomPlace.end())
                    {
                        if (api.GetClassroomProgress(itr->x, itr->y) >= 10000000)
                            itr = ClassRoomPlace.erase(itr);
                        else itr++;
                    }

                    //计算最近的教室位置
                    nearest_dest = nearest_place(GridToCell(current_place), ClassRoomPlace, api);

                    //如果有教室可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.StartLearning();//开始学习
                    }
                }
            }
            else//如果写完了7份作业，则找最近的校门并毕业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //计算最近的大门位置
                    nearest_dest = nearest_place(GridToCell(current_place), GatePlace, api);

                    //如果有大门可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.Graduate();//申请毕业
                    }
                }
            }
        }
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
        // 初赛玩家3和玩家0的代码完全相同
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)//如果玩家可移动
        {
            if (api.GetGameInfo()->subjectFinished < 7)//如果没写完7份作业，则找最近的教室写作业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //更新一下ClassRoomPlace信息，如果有作业已经写完了，就将其坐标从ClassRoomPlace中剔除
                    auto itr = ClassRoomPlace.begin();
                    while (itr != ClassRoomPlace.end())
                    {
                        if (api.GetClassroomProgress(itr->x, itr->y) >= 10000000)
                            itr = ClassRoomPlace.erase(itr);
                        else itr++;
                    }

                    //计算最近的教室位置
                    nearest_dest = nearest_place(GridToCell(current_place), ClassRoomPlace, api);

                    //如果有教室可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        api.UseSkill(0);
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.StartLearning();//开始学习
                    }
                }
            }
            else//如果写完了7份作业，则找最近的校门并毕业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //计算最近的大门位置
                    nearest_dest = nearest_place(GridToCell(current_place), GatePlace, api);

                    //如果有大门可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.Graduate();//申请毕业
                    }
                }
            }
        }

    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        // 初赛玩家3和玩家0的代码完全相同
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)//如果玩家可移动
        {
            if (api.GetGameInfo()->subjectFinished < 7)//如果没写完7份作业，则找最近的教室写作业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //更新一下ClassRoomPlace信息，如果有作业已经写完了，就将其坐标从ClassRoomPlace中剔除
                    auto itr = ClassRoomPlace.begin();
                    while (itr != ClassRoomPlace.end())
                    {
                        if (api.GetClassroomProgress(itr->x, itr->y) >= 10000000)
                            itr = ClassRoomPlace.erase(itr);
                        else itr++;
                    }

                    //计算最近的教室位置
                    nearest_dest = nearest_place(GridToCell(current_place), ClassRoomPlace, api);

                    //如果有教室可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.UseSkill(0);
                        api.StartLearning();//开始学习
                    }
                }
            }
            else//如果写完了7份作业，则找最近的校门并毕业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //计算最近的大门位置
                    nearest_dest = nearest_place(GridToCell(current_place), GatePlace, api);

                    //如果有大门可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.Graduate();//申请毕业
                    }
                }
            }
        }

    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        // 初赛玩家3和玩家0的代码完全相同
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)//如果玩家可移动
        {
            if (api.GetGameInfo()->subjectFinished < 7)//如果没写完7份作业，则找最近的教室写作业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //更新一下ClassRoomPlace信息，如果有作业已经写完了，就将其坐标从ClassRoomPlace中剔除
                    auto itr = ClassRoomPlace.begin();
                    while (itr != ClassRoomPlace.end())
                    {
                        if (api.GetClassroomProgress(itr->x, itr->y) >= 10000000)
                            itr = ClassRoomPlace.erase(itr);
                        else itr++;
                    }

                    //计算最近的教室位置
                    nearest_dest = nearest_place(GridToCell(current_place), ClassRoomPlace, api);

                    //如果有教室可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.UseSkill(0);
                        api.StartLearning();//开始学习
                    }

                }
            }
            else//如果写完了7份作业，则找最近的校门并毕业
            {
                if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
                {
                    //计算最近的大门位置
                    nearest_dest = nearest_place(GridToCell(current_place), GatePlace, api);

                    //如果有大门可达，则生成最短路径并且开始行走
                    if (nearest_dest.x < 50)
                    {
                        cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
                        tracking = true;
                        Guide(current_place, cur_path, api);
                    }
                }
                else//如果玩家有目标
                {
                    if (Guide(current_place, cur_path, api))//如果到达了目的地
                    {
                        tracking = false;
                        api.Graduate();//申请毕业
                    }
                }
            }
        }
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
    api.PrintSelfInfo();
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    static bool started = false;
    //游戏开始，获取地形信息并提取重要地点坐标
    if (!started)
    {
        FullMapInfo = api.GetFullMap();
        int i, j;
        cell temp;
        for (i = 0; i < (int)FullMapInfo.size(); i++)
        {
            for (j = 0; j < (int)FullMapInfo[i].size(); j++)
            {
                temp.x = i;
                temp.y = j;
                switch (FullMapInfo[i][j])
                {
                case THUAI6::PlaceType::Chest:
                    ChestPlace.emplace_back(temp); break;
                case THUAI6::PlaceType::Door3:
                    Door3Place.emplace_back(temp); break;
                case THUAI6::PlaceType::Door5:
                    Door5Place.emplace_back(temp); break;
                case THUAI6::PlaceType::Door6:
                    Door6Place.emplace_back(temp); break;
                case THUAI6::PlaceType::ClassRoom:
                    ClassRoomPlace.emplace_back(temp); break;
                case THUAI6::PlaceType::Gate:
                    GatePlace.emplace_back(temp); break;
                }
            }
        }
        started = true;
    }
    //以下为所有玩家都需要用到的变量
    grid current_place = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };//玩家当前位置
    static cell nearest_dest = { 0,0 };//最近的目的地
    double nearest_dest_time;//到达最近的目的地消耗的时间
    static std::vector<cell> cur_path;//当前行走路径
    std::vector<std::shared_ptr<const THUAI6::Student>> stu_info;
    static bool tracking = false;//是否正在沿路径行走
    if (!tracking && nearest_dest.x < 50)//如果玩家无目标，并且未确认是否有可达的目标
    {
        //更新一下ClassRoomPlace信息，如果有作业已经写完了，就将其坐标从ClassRoomPlace中剔除
        auto itr = ClassRoomPlace.begin();
        while (itr != ClassRoomPlace.end())
        {
            if (api.GetClassroomProgress(itr->x, itr->y) >= 10000000)
                itr = ClassRoomPlace.erase(itr);
            else itr++;
        }

        //计算最近的教室位置
        nearest_dest = nearest_place(GridToCell(current_place), ClassRoomPlace, api);

        //如果有教室可达，则生成最短路径并且开始行走
        if (nearest_dest.x < 50)
        {
            cur_path = shortcut(GridToCell(current_place), nearest_dest, api);
            tracking = true;
            Guide(current_place, cur_path, api);
        }
    }
    else//如果玩家有目标
    {
        if (Guide(current_place, cur_path, api))//如果到达了目的地
        {
            tracking = false;
        }
        stu_info = api.GetStudents();//获取学生信息
        if (!stu_info.empty())//若可见范围内有学生
        {
            double angle = atan2((double)(stu_info.begin()->get()->y - current_place.y), (double)((stu_info.begin()->get()->x) - current_place.x));
            if (sqrt((double)(stu_info.begin()->get()->y - current_place.y) * (double)(stu_info.begin()->get()->y - current_place.y) + (double)((stu_info.begin()->get()->x) - current_place.x) * (double)((stu_info.begin()->get()->x) - current_place.x)) <= 78)
                api.UseSkill(1);
            api.UseSkill(0);
            api.Attack(angle);
        }

    }
}

//学生翻窗时间
extern double window_student_time[7] = { 0,0.0328,0.07874,0.047259,0,0,0.03937 };
//捣蛋鬼翻窗时间
extern double window_tricker_time = 0.03937;

//step1: 计算最短路径，以下为相关函数

//计算连接两个给定格点c1和c2之间的边的权重
double weight(cell c1, cell c2, IStudentAPI& api)
{
    double cost = TIME_INF;
    if ((c1.x == c2.x) && (c1.y == c2.y))
        cost = 0;//如果传入两点相同，权重为零
    else if (((c1.x == c2.x) && (abs(c1.y - c2.y) == 1)) || ((c1.y == c2.y) && (abs(c1.x - c2.x) == 1)))//判断传入两点是否相邻
    {

        if ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Land) ||
            (FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Grass) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door3) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door5) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door6) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)))
            //如果c1为陆地、草地或开着的门的话
        {
            switch (FullMapInfo[c2.x][c2.y])
                //判断c2点的情况
            {
            case THUAI6::PlaceType::Land: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为长度除速度
            case THUAI6::PlaceType::Grass: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为长度除速度
            case THUAI6::PlaceType::Door3:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Door5:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Door6:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break; //是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Window: cost = window_student_time[(int)(api.GetSelfInfo()->studentType)]; break;
                //若遇到窗户，权重的计算
            default: cost = TIME_INF;//其他情况，权重为无穷
            }
        }
        else if (FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Window)//如果c1点为窗户
        {
            switch (FullMapInfo[c2.x][c2.y])
                //判断c2点的情况            
            {
            case THUAI6::PlaceType::Land: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为翻窗时间
            case THUAI6::PlaceType::Grass: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为翻窗时间
            case THUAI6::PlaceType::Door3:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            case THUAI6::PlaceType::Door5:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            case THUAI6::PlaceType::Door6:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            default: cost = TIME_INF;//其他情况，权重为无穷
            }
        }
        else cost = TIME_INF;//如果c1点一开始就不是草地、开的门、陆地、窗户的情况，那么权重为无穷
    }
    else cost = TIME_INF;//若两点不相邻且不相同，权重为无穷
    return cost;
}

double weight(cell c1, cell c2, ITrickerAPI& api)
{
    double cost = TIME_INF;
    if ((c1.x == c2.x) && (c1.y == c2.y))
        cost = 0;//如果传入两点相同，权重为零
    else if (((c1.x == c2.x) && (abs(c1.y - c2.y) == 1)) || ((c1.y == c2.y) && (abs(c1.x - c2.x) == 1)))//判断传入两点是否相邻
    {

        if ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Land) ||
            (FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Grass) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door3) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door5) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)) ||
            ((FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Door6) && (api.GetDoorProgress(c1.x, c1.y) == 10000000)))
            //如果c1为陆地、草地或开着的门的话
        {
            switch (FullMapInfo[c2.x][c2.y])
                //判断c2点的情况
            {
            case THUAI6::PlaceType::Land: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为长度除速度
            case THUAI6::PlaceType::Grass: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为长度除速度
            case THUAI6::PlaceType::Door3:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Door5:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Door6:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break; //是否门为开，若开则正常计算，否则权值为无穷
            case THUAI6::PlaceType::Window: cost = window_tricker_time; break;
                //若遇到窗户，权重的计算
            default: cost = TIME_INF;//其他情况，权重为无穷
            }
        }
        else if (FullMapInfo[c1.x][c1.y] == THUAI6::PlaceType::Window)//如果c1点为窗户
        {
            switch (FullMapInfo[c2.x][c2.y])
                //判断c2点的情况            
            {
            case THUAI6::PlaceType::Land: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为翻窗时间
            case THUAI6::PlaceType::Grass: cost = 1000.0 / api.GetSelfInfo()->speed; break;//权重为翻窗时间
            case THUAI6::PlaceType::Door3:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            case THUAI6::PlaceType::Door5:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            case THUAI6::PlaceType::Door6:
                if (api.GetDoorProgress(c2.x, c2.y) == 10000000)
                    cost = 1000.0 / api.GetSelfInfo()->speed;
                else cost = TIME_INF; break;//若门开，权重为翻窗时间，否则权重为无穷
            default: cost = TIME_INF;//其他情况，权重为无穷
            }
        }
        else cost = TIME_INF;//如果c1点一开始就不是草地、开的门、陆地、窗户的情况，那么权重为无穷
    }
    else cost = TIME_INF;//若两点不相邻且不相同，权重为无穷
    return cost;
}

//以下的timecost()函数和shortcut()函数使用Bellman-Ford最短路算法

//计算两个给定格点start和end之间最短路消耗时间
double timecost(cell start, cell end, IStudentAPI& api)
{
    int i, k, x_min, x_max, y_min, y_max;
    cell j, m, n;
    int p, q;
    cell neighbour[4];
    double cost;
    double length[48][48];
    for (p = 0; p < 48; p++)
        for (q = 0; q < 48; q++)
        {
            j.x = p + 1; j.y = q + 1;
            length[p][q] = weight(start, j, api);
        }
    for (i = 0; i < 150; i++)
    {
        x_min = start.x - i - 2;
        if (x_min < 1)x_min = 1;
        x_max = start.x + i + 2;
        if (x_max > 48)x_max = 48;
        for (j.x = x_min; j.x <= x_max; j.x++)
        {
            y_min = start.y - (i + 2 - abs(start.x - j.x));
            if (y_min < 1)y_min = 1;
            y_max = start.y + (i + 2 - abs(start.x - j.x));
            if (y_max > 48)y_max = 48;
            for (j.y = y_min; j.y <= y_max; j.y++)
            {
                neighbour[0].x = j.x - 1;
                neighbour[0].y = j.y;
                neighbour[1].x = j.x + 1;
                neighbour[1].y = j.y;
                neighbour[2].x = j.x;
                neighbour[2].y = j.y - 1;
                neighbour[3].x = j.x;
                neighbour[3].y = j.y + 1;
                for (k = 0; k < 4; k++)
                {
                    m.x = j.x; m.y = j.y;
                    n.x = neighbour[k].x; n.y = neighbour[k].y;

                    //加下面这个if-else块的原因是，weight()函数在计算权重时认为箱子、教室、大门等方块是不可进入的，但终点处的这些方块需要认为是“可进入”的（这样才能使得起点和终点“可达”），所以需要单独讨论
                    if (!(m.x == end.x && m.y == end.y) && !(n.x == end.x && n.y == end.y))//如果m,n均不是终点
                        cost = weight(m, n, api);
                    else
                    {
                        //合理调整m,n的顺序使得n是终点
                        if (m.x == end.x && m.y == end.y)
                        {
                            cell temp = m;
                            m = n;
                            n = temp;
                        }
                        if (FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Land || FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Grass)cost = 1000.0 / api.GetSelfInfo()->speed;
                        else cost = TIME_INF;
                    }
                    if (cost < TIME_INF)
                    {
                        if (length[j.x - 1][j.y - 1] >= (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost))
                        {
                            length[j.x - 1][j.y - 1] = (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost);
                        }
                    }
                }
            }
        }
        if (length[end.x - 1][end.y - 1] < TIME_INF)break;
    }
    double ans = length[end.x - 1][end.y - 1];
    api.Print(fmt::format("Minimum time from ({},{}) to ({},{}): {}", start.x, start.y, end.x, end.y, ans));
    return ans;
}

double timecost(cell start, cell end, ITrickerAPI& api)
{
    int i, k, x_min, x_max, y_min, y_max;
    cell j, m, n;
    int p, q;
    cell neighbour[4];
    double cost;
    double length[48][48];
    for (p = 0; p < 48; p++)
        for (q = 0; q < 48; q++)
        {
            j.x = p + 1; j.y = q + 1;
            length[p][q] = weight(start, j, api);
        }
    for (i = 0; i < 150; i++)
    {
        x_min = start.x - i - 2;
        if (x_min < 1)x_min = 1;
        x_max = start.x + i + 2;
        if (x_max > 48)x_max = 48;
        for (j.x = x_min; j.x <= x_max; j.x++)
        {
            y_min = start.y - (i + 2 - abs(start.x - j.x));
            if (y_min < 1)y_min = 1;
            y_max = start.y + (i + 2 - abs(start.x - j.x));
            if (y_max > 48)y_max = 48;
            for (j.y = y_min; j.y <= y_max; j.y++)
            {
                neighbour[0].x = j.x - 1;
                neighbour[0].y = j.y;
                neighbour[1].x = j.x + 1;
                neighbour[1].y = j.y;
                neighbour[2].x = j.x;
                neighbour[2].y = j.y - 1;
                neighbour[3].x = j.x;
                neighbour[3].y = j.y + 1;
                for (k = 0; k < 4; k++)
                {
                    m.x = j.x; m.y = j.y;
                    n.x = neighbour[k].x; n.y = neighbour[k].y;

                    //加下面这个if-else块的原因是，weight()函数在计算权重时认为箱子、教室、大门等方块是不可进入的，但终点处的这些方块需要认为是“可进入”的（这样才能使得起点和终点“可达”），所以需要单独讨论
                    if (!(m.x == end.x && m.y == end.y) && !(n.x == end.x && n.y == end.y))//如果m,n均不是终点
                        cost = weight(m, n, api);
                    else
                    {
                        //合理调整m,n的顺序使得n是终点
                        if (m.x == end.x && m.y == end.y)
                        {
                            cell temp = m;
                            m = n;
                            n = temp;
                        }
                        if (FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Land || FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Grass)cost = 1000.0 / api.GetSelfInfo()->speed;
                        else cost = TIME_INF;
                    }
                    if (cost < TIME_INF)
                    {
                        if (length[j.x - 1][j.y - 1] >= (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost))
                        {
                            length[j.x - 1][j.y - 1] = (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost);
                        }
                    }
                }
            }
        }
        if (length[end.x - 1][end.y - 1] < TIME_INF)break;
    }
    double ans = length[end.x - 1][end.y - 1];
    api.Print(fmt::format("Minimum time from ({},{}) to ({},{}): {}", start.x, start.y, end.x, end.y, ans));
    return ans;
}

vector<cell> shortcut(cell start, cell end, IStudentAPI& api)
{
    int i, k, x_min, x_max, y_min, y_max;
    cell j, m, n;
    int p, q;
    cell neighbour[4];
    double cost;
    double length[48][48];
    //以下建立一个类似链表结构的二维数组，算法运行的目标是让每个Node的next成员指向从Node沿最短路走到start对应的Node，需要经过的下一个节点
    struct Node {
        cell c;
        Node* next;
    }graph[48][48];
    for (p = 0; p < 48; p++)
        for (q = 0; q < 48; q++)
        {
            j.x = p + 1; j.y = q + 1;
            length[p][q] = weight(start, j, api);
            graph[p][q].c = j;
            if (j.x == start.x && j.y == start.y)graph[p][q].next = NULL;
            else if (length[p][q] < TIME_INF)graph[p][q].next = &graph[start.x - 1][start.y - 1];
            else graph[p][q].next = NULL;
        }
    for (i = 0; i < 150; i++)
    {
        x_min = start.x - i - 2;
        if (x_min < 1)x_min = 1;
        x_max = start.x + i + 2;
        if (x_max > 48)x_max = 48;
        for (j.x = x_min; j.x <= x_max; j.x++)
        {
            y_min = start.y - (i + 2 - abs(start.x - j.x));
            if (y_min < 1)y_min = 1;
            y_max = start.y + (i + 2 - abs(start.x - j.x));
            if (y_max > 48)y_max = 48;
            for (j.y = y_min; j.y <= y_max; j.y++)
            {
                neighbour[0].x = j.x - 1;
                neighbour[0].y = j.y;
                neighbour[1].x = j.x + 1;
                neighbour[1].y = j.y;
                neighbour[2].x = j.x;
                neighbour[2].y = j.y - 1;
                neighbour[3].x = j.x;
                neighbour[3].y = j.y + 1;
                for (k = 0; k < 4; k++)
                {
                    m.x = j.x; m.y = j.y;
                    n.x = neighbour[k].x; n.y = neighbour[k].y;

                    //加下面这个if-else块的原因是，weight()函数在计算权重时认为箱子、教室、大门等方块是不可进入的，但终点处的这些方块需要认为是“可进入”的（这样才能使得起点和终点“可达”），所以需要单独讨论
                    if (!(m.x == end.x && m.y == end.y) && !(n.x == end.x && n.y == end.y))//如果m,n均不是终点
                        cost = weight(m, n, api);
                    else
                    {
                        //合理调整m,n的顺序使得n是终点
                        if (m.x == end.x && m.y == end.y)
                        {
                            cell temp = m;
                            m = n;
                            n = temp;
                        }
                        if (FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Land || FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Grass)cost = 1000.0 / api.GetSelfInfo()->speed;
                        else cost = TIME_INF;
                    }
                    if (cost < TIME_INF)
                    {
                        if (length[j.x - 1][j.y - 1] >= (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost))
                        {
                            length[j.x - 1][j.y - 1] = (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost);
                            graph[j.x - 1][j.y - 1].next = &graph[neighbour[k].x - 1][neighbour[k].y - 1];
                        }
                    }
                }
            }
        }
        if (length[end.x - 1][end.y - 1] < TIME_INF)break;
    }

    vector<cell> path;
    //让ptr从end对应的Node沿“链表”一直走到start，走过的路径即为从end到start的最短路。将过程中走到的节点坐标依次插入到path的开头，最终得到的即为符合要求的路径
    //补充：最终返回的路径中不包含end，这是因为end一般是箱子、作业、大门等不能进入的地方，只需要导航到这些方块的相邻格子即可与方块交互
    Node* ptr = &graph[end.x - 1][end.y - 1];
    ptr = ptr->next;
    path.insert(path.begin(), ptr->c);
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
        path.insert(path.begin(), ptr->c);
    }
    return path;
}

vector<cell> shortcut(cell start, cell end, ITrickerAPI& api)
{
    int i, k, x_min, x_max, y_min, y_max;
    cell j, m, n;
    int p, q;
    cell neighbour[4];
    double cost;
    double length[48][48];
    //以下建立一个类似链表结构的二维数组，算法运行的目标是让每个Node的next成员指向从Node沿最短路走到start对应的Node，需要经过的下一个节点
    struct Node {
        cell c;
        Node* next;
    }graph[48][48];
    for (p = 0; p < 48; p++)
        for (q = 0; q < 48; q++)
        {
            j.x = p + 1; j.y = q + 1;
            length[p][q] = weight(start, j, api);
            graph[p][q].c = j;
            if (j.x == start.x && j.y == start.y)graph[p][q].next = NULL;
            else if (length[p][q] < TIME_INF)graph[p][q].next = &graph[start.x - 1][start.y - 1];
            else graph[p][q].next = NULL;
        }
    for (i = 0; i < 150; i++)
    {
        x_min = start.x - i - 2;
        if (x_min < 1)x_min = 1;
        x_max = start.x + i + 2;
        if (x_max > 48)x_max = 48;
        for (j.x = x_min; j.x <= x_max; j.x++)
        {
            y_min = start.y - (i + 2 - abs(start.x - j.x));
            if (y_min < 1)y_min = 1;
            y_max = start.y + (i + 2 - abs(start.x - j.x));
            if (y_max > 48)y_max = 48;
            for (j.y = y_min; j.y <= y_max; j.y++)
            {
                neighbour[0].x = j.x - 1;
                neighbour[0].y = j.y;
                neighbour[1].x = j.x + 1;
                neighbour[1].y = j.y;
                neighbour[2].x = j.x;
                neighbour[2].y = j.y - 1;
                neighbour[3].x = j.x;
                neighbour[3].y = j.y + 1;
                for (k = 0; k < 4; k++)
                {
                    m.x = j.x; m.y = j.y;
                    n.x = neighbour[k].x; n.y = neighbour[k].y;

                    //加下面这个if-else块的原因是，weight()函数在计算权重时认为箱子、教室、大门等方块是不可进入的，但终点处的这些方块需要认为是“可进入”的（这样才能使得起点和终点“可达”），所以需要单独讨论
                    if (!(m.x == end.x && m.y == end.y) && !(n.x == end.x && n.y == end.y))//如果m,n均不是终点
                        cost = weight(m, n, api);
                    else
                    {
                        //合理调整m,n的顺序使得n是终点
                        if (m.x == end.x && m.y == end.y)
                        {
                            cell temp = m;
                            m = n;
                            n = temp;
                        }
                        if (FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Land || FullMapInfo[m.x][m.y] == THUAI6::PlaceType::Grass)cost = 1000.0 / api.GetSelfInfo()->speed;
                        else cost = TIME_INF;
                    }
                    if (cost < TIME_INF)
                    {
                        if (length[j.x - 1][j.y - 1] >= (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost))
                        {
                            length[j.x - 1][j.y - 1] = (length[neighbour[k].x - 1][neighbour[k].y - 1] + cost);
                            graph[j.x - 1][j.y - 1].next = &graph[neighbour[k].x - 1][neighbour[k].y - 1];
                        }
                    }
                }
            }
        }
        if (length[end.x - 1][end.y - 1] < TIME_INF)break;
    }

    vector<cell> path;
    //让ptr从end对应的Node沿“链表”一直走到start，走过的路径即为从end到start的最短路。将过程中走到的节点坐标依次插入到path的开头，最终得到的即为符合要求的路径
    //补充：最终返回的路径中不包含end，这是因为end一般是箱子、作业、大门等不能进入的地方，只需要导航到这些方块的相邻格子即可与方块交互
    Node* ptr = &graph[end.x - 1][end.y - 1];
    ptr = ptr->next;
    path.insert(path.begin(), ptr->c);
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
        path.insert(path.begin(), ptr->c);
    }
    return path;
}

//step2: 给定路径和玩家当前位置，计算出下一步移动的方向
int Guide(grid& cur, std::vector<cell>& path, IStudentAPI& api)
{
    //如果玩家与路径起点格的中心足够近，则可以删除起点
    if ((abs(cur.x - IAPI::CellToGrid(path.begin()->x)) < 200) && (abs(cur.y - IAPI::CellToGrid(path.begin()->y)) < 100))
        path.erase(path.begin());
    if (path.empty())return 1;//如果路径为空，说明已到达目的地，返回1

    //对起点处的地形分类讨论
    switch (FullMapInfo[path.begin()->x][path.begin()->y])
    {
    case THUAI6::PlaceType::Window://需要翻窗
        path.erase(path.begin());//删除在窗处的节点（即起点）
        api.SkipWindow();//翻窗
        break;
    case THUAI6::PlaceType::Door3://遇到3教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key3);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Door5://遇到5教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key5);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Door6://遇到6教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key6);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Land:
    case THUAI6::PlaceType::Grass:
        //首先计算该步的移动角度：angle=arctan((path.y-cur.y)/(path.x-cur.x)
        double angle = atan2((double)(IAPI::CellToGrid(path.begin()->y) - cur.y), (double)(IAPI::CellToGrid(path.begin()->x) - cur.x));
        api.Move(45, angle);
    }
    return 0;
}

int Guide(grid& cur, std::vector<cell>& path, ITrickerAPI& api)
{
    //如果玩家与路径起点格的中心足够近，则可以删除起点
    if ((abs(cur.x - IAPI::CellToGrid(path.begin()->x)) < 200) && (abs(cur.y - IAPI::CellToGrid(path.begin()->y)) < 100))
        path.erase(path.begin());
    if (path.empty())return 1;//如果路径为空，说明已到达目的地，返回1

    //对起点处的地形分类讨论
    switch (FullMapInfo[path.begin()->x][path.begin()->y])
    {
    case THUAI6::PlaceType::Window://需要翻窗
        path.erase(path.begin());//删除在窗处的节点（即起点）
        api.SkipWindow();//翻窗
        break;
    case THUAI6::PlaceType::Door3://遇到3教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key3);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Door5://遇到5教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key5);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Door6://遇到6教门
        if (!api.IsDoorOpen(path.begin()->x, path.begin()->y))//如果门没开，用钥匙开门
        {
            api.UseProp(THUAI6::PropType::Key6);
            break;
        }//如果门开着，直接进入，跳到case THUAI6::PlaceType::Land
    case THUAI6::PlaceType::Land:
    case THUAI6::PlaceType::Grass:
        //首先计算该步的移动角度：angle=arctan((path.y-cur.y)/(path.x-cur.x)
        double angle = atan2((double)(IAPI::CellToGrid(path.begin()->y) - cur.y), (double)(IAPI::CellToGrid(path.begin()->x) - cur.x));
        api.Move(45, angle);
    }
    return 0;
}

cell nearest_place(cell start, std::vector<cell>& dest_v, IStudentAPI& api)
{
    cell nearest_dest;
    double nearest_dest_time = TIME_INF, time_temp;
    for (auto itr = dest_v.begin(); itr != dest_v.end(); itr++)
    {
        time_temp = timecost(start, *itr, api);
        if (time_temp < nearest_dest_time)
        {
            nearest_dest_time = time_temp;
            nearest_dest = *itr;
        }
    }
    if (nearest_dest_time >= TIME_INF)//设置nearest_dest为{50,50}表示dest_v中所有地点均不可达
    {
        nearest_dest.x = 50;
        nearest_dest.y = 50;
    }
    return nearest_dest;
}
cell nearest_place(cell start, std::vector<cell>& dest_v, ITrickerAPI& api)
{
    cell nearest_dest;
    double nearest_dest_time = TIME_INF, time_temp;
    for (auto itr = dest_v.begin(); itr != dest_v.end(); itr++)
    {
        time_temp = timecost(start, *itr, api);
        if (time_temp < nearest_dest_time)
        {
            nearest_dest_time = time_temp;
            nearest_dest = *itr;
        }
    }
    if (nearest_dest_time >= TIME_INF)//设置nearest_dest为{50,50}表示dest_v中所有地点均不可达
    {
        nearest_dest.x = 50;
        nearest_dest.y = 50;
    }
    return nearest_dest;
}