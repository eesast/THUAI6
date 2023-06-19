#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include<map>
#include<queue>
#include<chrono>
#include<thread>
#define MAXPRO 10000000
#define PI 3.14
#define MOVETIME 100
#define _CellX api.GridToCell(api.GetSelfInfo()->x)
#define _CellY api.GridToCell(api.GetSelfInfo()->y)
#define _X api.GetSelfInfo()->x
#define _Y api.GetSelfInfo()->y
#define AVOANDIS 405
#define CLIWINDIS 403
#define ARMDIS 2150//武器距离
#define ALERTRAN 15300//警戒范围
#define AVOIDRAN 1200//躲避老师的半径
#define DODGETIM 30//被墙黏住时的移动时间
#define GRASSGAIN 2.3//草地相对增益
#define MAXGATEPRO 18000
#define MAXHIDTIM 2300

extern const bool asynchronous = false;

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

std::vector<std::vector<std::pair<std::pair<int, int>, int>>>THUClassroom =
{
    {   
        std::make_pair(std::make_pair(22,18),1),
        std::make_pair(std::make_pair(30,7),1),
        std::make_pair(std::make_pair(18,5),1),
        std::make_pair(std::make_pair(40,12),1),
        std::make_pair(std::make_pair(44,32),1),
        std::make_pair(std::make_pair(28,26),1),
        std::make_pair(std::make_pair(33,40),1),
        std::make_pair(std::make_pair(19,41),1),
        std::make_pair(std::make_pair(10,38),1),
        std::make_pair(std::make_pair(8,31),1),  
    },
    {
        std::make_pair(std::make_pair(16,16),1),
        std::make_pair(std::make_pair(6,13),1),
        std::make_pair(std::make_pair(6,36),1),
        std::make_pair(std::make_pair(13,43),1),
        std::make_pair(std::make_pair(33,33),1),
        std::make_pair(std::make_pair(16,33),1),
        std::make_pair(std::make_pair(43,36),1),
        std::make_pair(std::make_pair(43,13),1),
        std::make_pair(std::make_pair(36,6),1),
        std::make_pair(std::make_pair(33,16),1)
    }
};
std::vector<std::vector<std::pair<int, int>>>THUGate =
{
    {
        std::make_pair(5,6),
        std::make_pair(46,45)   },
    {
        std::make_pair(2,43),
        std::make_pair(47,6)   }
};
std::vector<std::vector<std::pair<std::pair<int, int>, int>>>THUChest =
{
    {
        std::make_pair(std::make_pair(29,21),1),
        std::make_pair(std::make_pair(13,31),1),
        std::make_pair(std::make_pair(4,15),1),
        std::make_pair(std::make_pair(4,45),1),
        std::make_pair(std::make_pair(21,41),1),
        std::make_pair(std::make_pair(39,33),1),
        std::make_pair(std::make_pair(48,20),1),
        std::make_pair(std::make_pair(37,2),1),  
    },
    {
        std::make_pair(std::make_pair(9,15),1),
        std::make_pair(std::make_pair(15,9),1),
        std::make_pair(std::make_pair(34,9),1),
        std::make_pair(std::make_pair(40,15),1),
        std::make_pair(std::make_pair(40,34),1),
        std::make_pair(std::make_pair(34,40),1),
        std::make_pair(std::make_pair(15,40),1),
        std::make_pair(std::make_pair(9,34),1)  
    }
};

int THUMapIndex = -1;
std::pair<int, int>THUAttackTarget = std::make_pair(-1, -1);//攻击瞄准坐标
std::vector<std::pair<int, int>>THUPosition;//自身位置
std::vector<THUAI6::PlayerState>THUState;//自身处境
double THUDirection = PI;//移动方向
int THUClassroomUnfinished = 10;//剩余作业
int THUTargetClassroom = 0;//目标教室索引
int THUTargetGate = 0;//目标校门索引
int THUTargetChest = 0;
int THUAttackTime = 0;//草地攻击次数
int THUJumpAttack = 0;
int THUJumpTime = 30;//反弹帧数
int THUClassroomListened = -1;//被监听的教室
bool THUShouldClimb = false;
long long THUShouldUseSkill = -1;//是否使用技能及控制发动技能时间
std::vector<std::pair<int, int>>THUTeacherPos = { std::make_pair(-2, -2),std::make_pair(-2, -2),std::make_pair(-2, -2),std::make_pair(-2, -2) };
long long THUSpecialTime = 0;
std::vector<std::pair<int, int>>THUSpecialStudents;
int THUTargetSpecialStu = 0;
int THUSpecialAttackTime = 0;
int THUForTeaIndex[4] = { 0,0,0,0 };
int THUForTeaNum = 0;
std::map<std::pair<int, int>, int>THUDoorKeys;
long long THUSpecialPropTime = -1;

class FrontierCompare
{
public:
    bool operator()(std::pair<int, std::pair<int, int>>f1, std::pair<int, std::pair<int, int>>f2)
    {
        return f1.first > f2.first;
    }
};
int THUHeuristic(const std::pair<int, int>& p1, const std::pair<int, int>& p2);
double THUStraightDistance(int x1, int y1, int x2, int y2);
bool THULeaving(ITrickerAPI& api);
bool THUHumbling(ITrickerAPI& api);
bool THUClimbing(ITrickerAPI& api);
bool THUDodging(ITrickerAPI& api);
std::pair<int, int> THUNeighbor4(ITrickerAPI& api, THUAI6::PlaceType placetype);
std::pair<int, int> THUCloseNeighbor4(ITrickerAPI& api, THUAI6::PlaceType placetype);
std::vector<std::pair<int, int>> THUNeighbor40(std::pair<int, int>current, std::pair<int, int>goal, ITrickerAPI& api);
std::vector<std::pair<int, int>> THUNeighbor41(std::pair<int, int>current, ITrickerAPI& api);
double THUAStar(const std::pair<int, int>& start, const std::pair<int, int>& goal, ITrickerAPI& api);
double THUBFSPlus(const std::pair<int, int>& start, THUAI6::PlaceType targetType, ITrickerAPI& api);
bool THUAvoidAll(double viewRange, double radius, int x, int y, int newX, int newY, ITrickerAPI& api);
bool THUAvoidWall(double viewRange, double radius, int x, int y, int newX, int newY, ITrickerAPI& api);
bool THUAttacking(ITrickerAPI& api);
bool THUChasing(ITrickerAPI& api);
bool THUAttending(ITrickerAPI& api);
bool THULearning(ITrickerAPI& api);
bool THUJumping(ITrickerAPI& api);
bool THUListening(ITrickerAPI& api);
void THURefreshState(ITrickerAPI& api);
void THUSkillForANoisyPerson(ITrickerAPI& api);
bool THUSpecialTask(ITrickerAPI& api);

int MyHeuristic(const std::pair<int, int>& p1, const std::pair<int, int>& p2);
double MyStraightDistance(int x1, int y1, int x2, int y2);
bool MyDodging(IStudentAPI& api);
bool MyClimbing(IStudentAPI& api);
std::pair<int, int> MyNeighbor4(IStudentAPI& api, THUAI6::PlaceType placetype);
std::pair<int, int> MyNeighborDoors(IStudentAPI& api, int cellx, int celly);
std::pair<int, int> MyNeighbor8(IStudentAPI& api, THUAI6::PlaceType placetype);
std::pair<int, int> MyCloseNeighbor4(IStudentAPI& api, THUAI6::PlaceType placetype);
std::vector<std::pair<int, int>> MyNeighbor40(std::pair<int, int>current, std::pair<int, int>goal, IStudentAPI& api);
std::vector<std::pair<int, int>> MyNeighbor41(std::pair<int, int>current, IStudentAPI& api);
double MyAStar(const std::pair<int, int>& start, const std::pair<int, int>& goal, IStudentAPI& api);
double MyAStarFortea(const std::pair<int, int>& start, const std::pair<int, int>& goal, IStudentAPI& api);
double MyBFSPlus(const std::pair<int, int>& start, THUAI6::PlaceType targetType, IStudentAPI& api);
double MyBFSPlusForDoors(const std::pair<int, int>& start, IStudentAPI& api);
bool MyGraduating(IStudentAPI& api);
bool MyLeaving(IStudentAPI& api);
bool MyJumping(IStudentAPI& api);
bool MyHaveView(double viewRange, int x, int y, int newX, int newY, IStudentAPI& api);
bool MyHaveViewForWin(int x, int y, int newX, int newY, IStudentAPI& api);
void MyCommunicating(IStudentAPI& api);
bool MyLearning(IStudentAPI& api);
bool MyHiding(IStudentAPI& api);
bool MyHidingForTea(IStudentAPI& api);
bool MyAttending(IStudentAPI& api);
bool MyRescuing(IStudentAPI& api);
bool MyTask0(IStudentAPI& api);
void MyInitializing(IStudentAPI& api);
void MyDoors(IStudentAPI& api);
bool MyLearningForTea(IStudentAPI& api);
bool MySkillForTea(IStudentAPI& api);
bool MyChasingForTea(IStudentAPI& api);

std::vector<std::vector<std::pair<std::pair<int, int>, int>>>MyClassroom =
{
    {
        std::make_pair(std::make_pair(30,7),1),
        std::make_pair(std::make_pair(18,5),1),
        std::make_pair(std::make_pair(44,32),1),
        std::make_pair(std::make_pair(33,40),1),
        std::make_pair(std::make_pair(10,38),1),
        std::make_pair(std::make_pair(8,31),1),
        std::make_pair(std::make_pair(19,41),1),
    },
    {
        std::make_pair(std::make_pair(16,16),1),
        std::make_pair(std::make_pair(33,33),1),
        std::make_pair(std::make_pair(16,33),1),
        std::make_pair(std::make_pair(33,16),1),
        std::make_pair(std::make_pair(6,13),1),
        std::make_pair(std::make_pair(6,36),1),
        std::make_pair(std::make_pair(13,43),1),
        std::make_pair(std::make_pair(43,36),1),
        std::make_pair(std::make_pair(43,13),1),
        std::make_pair(std::make_pair(36,6),1),
    }
};
std::vector<std::vector<std::pair<int, int>>>MyGate =
{
    {
        std::make_pair(5,6),
        std::make_pair(46,45)   },
    {
        std::make_pair(2,43),
        std::make_pair(47,6)   }
};
int MyMapIndex = -1;
int MyClassroomUnfinished = 10;//剩余作业
int MyTargetClassroom = -1;//目标教室索引
int MyTargetGate = -1;//目标校门索引
int MyJumpTime = 30;//反弹帧数
bool MyShouldClimb = false;
std::pair<int, int>MyTriPos = std::make_pair(-2, -2);
std::map<std::pair<int, int>, int>MyDoorKeys;
std::vector<std::pair<int, int>>MyPosition;//自身位置
std::vector<THUAI6::PlayerState>MyState;//自身处境
double MyDirection = PI;//移动方向
bool MyShouldHide = false;
int MyHideTime = 0;
int MyRescueID = -1;
int MyCallID = -1;
int MyHidePro = 0;
int MyTaskPro = 0;
std::pair<int, int>MyLandPos = std::make_pair(-1, -1);
int MyMaxHidTime = 2300;

void AI::play(ITrickerAPI& api)
{
    if (::THUMapIndex == -1)
    {
        if (api.GetPlaceType(6, 13) == THUAI6::PlaceType::ClassRoom)
            ::THUMapIndex = 1;
        else
            ::THUMapIndex = 0;
        ::THUSpecialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    if (::THUForTeaNum == 0)
    {
        auto stu = api.GetStudents();
        for (int i = 0; i<int(stu.size()); ++i)
        {
            if (stu[i]->playerID >= 4)continue;
            if (stu[i]->studentType == THUAI6::StudentType::Teacher)
                ::THUForTeaIndex[stu[i]->playerID] = 10;
            else
                ::THUForTeaIndex[stu[i]->playerID] = 1;
        }
        if (::THUForTeaIndex[0] + ::THUForTeaIndex[1] + ::THUForTeaIndex[2] + ::THUForTeaIndex[3] >= 30)
            ::THUForTeaNum = -1;
        else if (::THUForTeaIndex[0] * ::THUForTeaIndex[1] * ::THUForTeaIndex[2] * ::THUForTeaIndex[3] > 0)
            ::THUForTeaNum = 1;
    }

    THUSkillForANoisyPerson(api);
    if (THUClimbing(api))return;
    if (THUDodging(api))return;
    //if (THUHumbling(api))return;
    if (THUJumping(api))return;
    if (::THUSpecialPropTime == -1 || std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - ::THUSpecialPropTime > 10000)
    {
        if (THUAttacking(api))return;
        if (THUChasing(api))return;
        if (THUListening(api))return;
    }
    if (THUSpecialTask(api))return;
    if (THULeaving(api))return;
    if (THULearning(api))return;
    if (THUAttending(api))return;
}


void AI::play(IStudentAPI& api)
{
    if (api.GetSelfInfo()->playerID == -10)
    {
        MyInitializing(api);
        MyCommunicating(api);
        MySkillForTea(api);
        if (MyClimbing(api))return;
        if (MyDodging(api))return;
        if (MyHidingForTea(api))return;
        if (MyRescuing(api))return;
        //if (MyChasingForTea(api))return;
        //if (MyLearningForTea(api))return;
    }
    else
    {
        MyInitializing(api);
        ::MyClassroomUnfinished = 0;
        for (int i = 0; i < int(::MyClassroom[::MyMapIndex].size()); ++i)
            ::MyClassroomUnfinished += ::MyClassroom[::MyMapIndex][i].second;
        MyCommunicating(api);
        auto tri = api.GetTrickers();
        if (!tri.empty() && (tri[0]->trickerType != THUAI6::TrickerType::ANoisyPerson || tri[0]->timeUntilSkillAvailable[0] > 0))
        {
            if (api.GetPlaceType(_CellX, _CellY) != THUAI6::PlaceType::Grass)
                api.UseSkill(2);
        }
        if (api.GetSelfInfo()->determination <= 2450000)
            api.UseSkill(1);
        MyDoors(api);
        if (MyClimbing(api))return;
        if (MyDodging(api))return;
        if (MyTask0(api))return;
        if (MyGraduating(api))return;
        if (MyJumping(api))return;
        if (MyHiding(api))return;
        //if (MyRescuing(api))return;
        if (MyLeaving(api))return;
        if (MyLearning(api))return;
        if (MyAttending(api))return;
    }
}


bool THUSpecialTask(ITrickerAPI& api)
{
    if (::THUSpecialTime != -1)
        return false;
    api.UseSkill(0);
    if (::THUSpecialStudents.empty())
    {
        if (api.PickProp(THUAI6::PropType::AddLifeOrClairaudience).get())
            ::THUSpecialPropTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        api.UseProp(THUAI6::PropType::AddLifeOrClairaudience);
        api.PickProp(THUAI6::PropType::AddHpOrAp);
        api.UseProp(THUAI6::PropType::AddHpOrAp);
        api.PickProp(THUAI6::PropType::CraftingBench);
        api.UseProp(THUAI6::PropType::CraftingBench);
        api.PickProp(THUAI6::PropType::ShieldOrSpear);
        api.UseProp(THUAI6::PropType::ShieldOrSpear);
        api.PickProp(THUAI6::PropType::RecoveryFromDizziness);
        api.UseProp(THUAI6::PropType::RecoveryFromDizziness);
        if (!api.GetStudents().empty())
        {
            auto stu = api.GetStudents();
            for (int i = 0; i < int(stu.size()); ++i)
            {
                if (stu[i]->studentType == THUAI6::StudentType::Teacher)
                    continue;
                ::THUSpecialStudents.push_back(std::make_pair(stu[i]->x, stu[i]->y));
            }
            if (!::THUSpecialStudents.empty())
                return true;
        }

        {
            if (::THUTargetChest >= 8)
            {
                ::THUSpecialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                ::THUSpecialStudents.clear();
                ::THUTargetSpecialStu = 0;
                ::THUSpecialAttackTime = 0;
                return true;
            }
            if (THUNeighbor4(api, THUAI6::PlaceType::Chest) == ::THUChest[::THUMapIndex][::THUTargetChest].first)
                api.StartOpenChest();
            else
            {
                double angle = THUAStar(std::make_pair(_CellX, _CellY), ::THUChest[::THUMapIndex][::THUTargetChest].first, api);
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                ::THUDirection = angle;
            }
            if (api.GetChestProgress(::THUChest[::THUMapIndex][::THUTargetChest].first.first, ::THUChest[::THUMapIndex][::THUTargetChest].first.second) == MAXPRO)
                ++::THUTargetChest;
        }
    }
    else
    {
        if (THUAvoidAll(1000, 210, _X, _Y, ::THUSpecialStudents[::THUTargetSpecialStu].first, ::THUSpecialStudents[::THUTargetSpecialStu].second, api))
        {
            if (::THUSpecialAttackTime <= 3)
            {
                api.UseSkill(0);
                if (api.Attack(atan2(::THUSpecialStudents[::THUTargetSpecialStu].second - _Y, ::THUSpecialStudents[::THUTargetSpecialStu].first - _X)).get())
                {
                    ++::THUSpecialAttackTime;
                }
            }
            else
            {
                ::THUSpecialAttackTime = 0;
                ++::THUTargetSpecialStu;
                if (::THUTargetSpecialStu >= int(::THUSpecialStudents.size()))
                {
                    ::THUSpecialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    ::THUSpecialStudents.clear();
                    ::THUTargetSpecialStu = 0;
                    ::THUSpecialAttackTime = 0;
                }
            }
        }
        else
        {
            double angle = THUAStar(std::make_pair(_CellX, _CellY), std::make_pair(api.GridToCell(::THUSpecialStudents[::THUTargetSpecialStu].first), api.GridToCell(::THUSpecialStudents[::THUTargetSpecialStu].second)), api);
            if (angle == -4)
            {
                ++::THUTargetSpecialStu;
                if (::THUTargetSpecialStu >= int(::THUSpecialStudents.size()))
                {
                    ::THUSpecialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    ::THUSpecialStudents.clear();
                    ::THUTargetSpecialStu = 0;
                    ::THUSpecialAttackTime = 0;
                }
            }
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                ::THUDirection = angle;;
            }
        }
    }
    return true;
}

//ANoisyPerson的技能
void THUSkillForANoisyPerson(ITrickerAPI& api)
{
    if (::THUForTeaNum != -1 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - ::THUSpecialTime >= 180000 && ::THUSpecialTime > 0)
    {
        ::THUSpecialTime = -1;
        ::THUForTeaNum = -1;
        return;
    }
    if (api.GetSelfInfo()->timeUntilSkillAvailable[0] > 0)
        return;
    if (::THUShouldUseSkill != -1 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - ::THUShouldUseSkill > 1000)
    {
        api.UseSkill(0);
        ::THUShouldUseSkill = -1;
        return;
    }
    auto stu = api.GetStudents();
    if (stu.empty())
        return;
    for (int i = 0; i<int(stu.size()); ++i)
    {
        if (stu[i]->studentType == THUAI6::StudentType::Robot || ::THUForTeaNum != -1 && stu[i]->studentType == THUAI6::StudentType::Teacher)
            continue;
        if (stu[i]->playerState == THUAI6::PlayerState::UsingSpecialSkill)
        {
            api.UseSkill(0);
            ::THUShouldUseSkill = -1;
            return;
        }
        if (stu[i]->playerState != THUAI6::PlayerState::Addicted && ::THUShouldUseSkill == -1)
        {
            ::THUShouldUseSkill = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    }
    return;
}

//更新老师坐标
void THURefreshState(ITrickerAPI& api)
{
    auto stu = api.GetStudents();
    for (int i = 0; i<int(sizeof(stu)); ++i)
    {
        if (stu[i]->studentType == THUAI6::StudentType::Teacher)
        {
            if (::THUForTeaNum == -1)
            {
                if (stu[i]->playerState != THUAI6::PlayerState::Addicted)
                    ::THUTeacherPos[stu[i]->playerID] = std::make_pair(stu[i]->x, stu[i]->y);
                else
                    ::THUTeacherPos[stu[i]->playerID] = std::make_pair(-2, -2);
            }
            else
            {
                ::THUTeacherPos[stu[i]->playerID] = std::make_pair(stu[i]->x, stu[i]->y);
            }
        }
    }
    return;
}

//返回两点垂直距离
int THUHeuristic(const std::pair<int, int>& p1, const std::pair<int, int>& p2)
{
    return std::abs(p1.first - p2.first) + std::abs(p1.second - p2.second);
}

//返回两点直线距离
double THUStraightDistance(int x1, int y1, int x2, int y2)
{
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

//通过BGM定位学生坐标
bool THUListening(ITrickerAPI& api)
{
    if (::THUClassroomListened == -1 && api.GetSelfInfo()->classVolume != 0 && api.GetSelfInfo()->trickDesire != 0)
    {
        double dis = ALERTRAN / api.GetSelfInfo()->trickDesire;
        for (int i = 0; i < 10; ++i)
        {
            if (::THUClassroom[::THUMapIndex][i].second == 0)
                continue;
            double disClass = THUStraightDistance(_X, _Y, ::THUClassroom[::THUMapIndex][i].first.first, ::THUClassroom[::THUMapIndex][i].first.second);
            if (abs(disClass - dis) <= 2121)
            {
                ::THUClassroomListened = i;
                double angle = THUAStar(std::make_pair(_CellX, _CellY), ::THUClassroom[::THUMapIndex][::THUClassroomListened].first, api);
                if (angle == -4)
                {
                    ::THUClassroomListened = -1;
                    continue;
                }
                if (angle == 4)
                    return true;
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                ::THUDirection = angle;
                return true;
            }
        }
    }
    else if (::THUClassroomListened != -1)
    {
        if (THUAvoidWall(api.GetSelfInfo()->viewRange - 4000, 210, _X, _Y, api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first), api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second), api))
        {
            ::THUClassroomListened = -1;
            return false;
        }
        double angle = THUAStar(std::make_pair(_CellX, _CellY), ::THUClassroom[::THUMapIndex][::THUClassroomListened].first, api);
        if (angle == -4)
        {
            ::THUClassroomListened = -1;
            return false;
        }
        if (angle == 4)
            return true;
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        ::THUDirection = angle;
        return true;
    }
    return false;
}

//防止被不可视学生堵住
bool THUJumping(ITrickerAPI& api)
{
    ::THUPosition.push_back(std::make_pair(_X, _Y));
    ::THUState.push_back(api.GetSelfInfo()->playerState);
    if (::THUPosition.size() > 20)
    {
        ::THUPosition.erase(::THUPosition.begin());
        ::THUState.erase(::THUState.begin());
    }
    else
        return false;
    if (::THUJumpAttack == 0)
    {
        for (int i = 0; i < ::THUPosition.size() - 1; ++i)
        {
            if (::THUPosition[i] != ::THUPosition[i + 1] || ::THUState[i] != THUAI6::PlayerState::Idle)
                return false;
        }
    }
    if (::THUJumpTime > 0 && ::THUJumpAttack < 3)
    {
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
            api.EndAllAction();
        if (api.Attack(::THUDirection).get())
            ++::THUJumpAttack;
        ::THUAttackTarget.first = -1;
        return true;
    }
    if (::THUJumpTime > 15)
    {
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Swinging)
            return true;
        --::THUJumpTime;
        api.EndAllAction();
        api.Move(MOVETIME, ::THUDirection - PI);
        ::THUAttackTarget.first = -1;
        return true;
    }
    else if (::THUJumpTime > 0)
    {
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Swinging)
            return true;
        --::THUJumpTime;
        api.EndAllAction();
        api.Move(MOVETIME, ::THUDirection - PI / 2);
        ::THUAttackTarget.first = -1;
        return true;
    }
    ::THUJumpTime = 30;
    ::THUJumpAttack = 0;
    return true;
}

//试图攻击教室
bool THULearning(ITrickerAPI& api)
{
    if (THUAvoidWall(api.GetSelfInfo()->viewRange - 4000, 210, _X, _Y, api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first), api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second), api))
    {
        if (api.GetClassroomProgress(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first, ::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second) < MAXPRO / 10)
        {
            ::THUTargetClassroom = (::THUTargetClassroom + 1) % 10;
            while (::THUClassroom[::THUMapIndex][::THUTargetClassroom].second == 0)
            {
                ::THUTargetClassroom = (::THUTargetClassroom + 1) % 10;
            }
            return false;
        }
        else if (api.GetClassroomProgress(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first, ::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second) == MAXPRO)
        {
            ::THUClassroom[::THUMapIndex][::THUTargetClassroom].second = 0;
            ::THUTargetClassroom = (::THUTargetClassroom + 1) % 10;
            while (::THUClassroom[::THUMapIndex][::THUTargetClassroom].second == 0)
            {
                ::THUTargetClassroom = (::THUTargetClassroom + 1) % 10;
            }
            --::THUClassroomUnfinished;
            return false;
        }
        else if (THUStraightDistance(_X, _Y, api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first), api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second)) <= ARMDIS)
        {
            if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
                api.EndAllAction();
            api.Attack(std::atan2(api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.second) - _Y, api.CellToGrid(::THUClassroom[::THUMapIndex][::THUTargetClassroom].first.first) - _X));
            return true;
        }
    }
    return false;
}

//寻找作业
bool THUAttending(ITrickerAPI& api)
{
    double angle = THUAStar(std::make_pair(_CellX, _CellY), ::THUClassroom[::THUMapIndex][::THUTargetClassroom].first, api);
    if (angle == -4)
        return false;
    if (angle == 4)
        return true;
    api.EndAllAction();
    api.Move(MOVETIME, angle);
    ::THUDirection = angle;
    return true;
}

//射程内：attack
//视野内，射程外：chase并更新target
//视野外：若地点与target不同：chase，不更新target；若地点与target相同：若target周围有草丛，朝target射击3次，target置零，反之放弃，target置零

//试图追击
bool THUChasing(ITrickerAPI& api)
{
    auto vect = api.GetStudents();
    for (int i = 0; i<int(vect.size()); ++i)
    {
        //if (vect[i]->playerState == THUAI6::PlayerState::Quit || (vect[i]->studentType == THUAI6::StudentType::Teacher && (::THUForTeaNum != -1 || vect[i]->playerState == THUAI6::PlayerState::Addicted)))
        if (vect[i]->playerState == THUAI6::PlayerState::Addicted || vect[i]->playerState == THUAI6::PlayerState::Quit || (vect[i]->studentType == THUAI6::StudentType::Teacher && ::THUForTeaNum != -1))
            continue;
        ::THUAttackTarget = std::make_pair(vect[0]->x, vect[0]->y);
        double angle = THUAStar(std::make_pair(_CellX, _CellY), std::make_pair(api.GridToCell(::THUAttackTarget.first), api.GridToCell(::THUAttackTarget.second)), api);
        if (angle == -4)
            continue;
        if (angle == 4)
            return true;
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        ::THUDirection = angle;
        return true;
    }
    if (::THUAttackTarget.first == -1)
        return false;
    if (::THUAttackTarget.first == 0)
        return true;
    if (_CellX != api.GridToCell(::THUAttackTarget.first) || _CellY != api.GridToCell(::THUAttackTarget.second))
    {
        double angle = THUAStar(std::make_pair(_CellX, _CellY), std::make_pair(api.GridToCell(::THUAttackTarget.first), api.GridToCell(::THUAttackTarget.second)), api);
        if (angle == -4)
        {
            ::THUAttackTarget.first = -1;
            return false;
        }
        if (angle == 4)
            return true;
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        ::THUDirection = angle;
        return true;
    }
    if (THUNeighbor4(api, THUAI6::PlaceType::Grass).first == -1)
    {
        ::THUAttackTarget.first = -1;
        return false;
    }
    if (::THUAttackTime < 3)
    {
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
            api.EndAllAction();
        if (api.Attack(std::atan2(::THUAttackTarget.second - _Y, ::THUAttackTarget.first - _X)).get())
            ++::THUAttackTime;
        return true;
    }
    ::THUAttackTime = 0;
    ::THUAttackTarget.first = -1;
    return false;
}

//试图攻击学生
bool THUAttacking(ITrickerAPI& api)
{
    auto vect = api.GetStudents();
    for (int i = 0; i < int(vect.size()); ++i)
    {
        //if (vect[i]->playerState == THUAI6::PlayerState::Quit || (vect[i]->studentType == THUAI6::StudentType::Teacher && (::THUForTeaNum != -1 || vect[i]->playerState == THUAI6::PlayerState::Addicted)))
        if (vect[i]->playerState == THUAI6::PlayerState::Addicted || vect[i]->playerState == THUAI6::PlayerState::Quit || (vect[i]->studentType == THUAI6::StudentType::Teacher && ::THUForTeaNum != -1))
        {
            ::THUAttackTarget = std::make_pair(-1, -1);
            continue;
        }
        if (::THUSpecialTime != -1 && vect[i]->studentType != THUAI6::StudentType::Robot)
            ::THUSpecialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (!THUAvoidAll(ARMDIS, 210, _X, _Y, vect[i]->x, vect[i]->y, api))
            continue;
        ::THUAttackTarget = std::make_pair(vect[i]->x, vect[i]->y);
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
            api.EndAllAction();
        api.Attack(std::atan2(vect[i]->y - _Y, vect[i]->x - _X));
        return true;
    }
    return false;
}

bool THUAvoidAll(double viewRange, double radius, int x, int y, int newX, int newY, ITrickerAPI& api)
{
    double deltaX = newX - x;
    double deltaY = newY - y;
    double distance = deltaX * deltaX + deltaY * deltaY;
    if (distance >= viewRange * viewRange)
        return false;
    int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
    if (divide == 0)
        return true;
    double dx = deltaX / divide;
    double dy = deltaY / divide;
    if (radius >= 499)
    {
        double myX = double(x);
        double myY = double(y);
        for (int i = 0; i < divide; i++)
        {
            myX += dx;
            myY += dy;
            auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
            if (type != THUAI6::PlaceType::Land && type != THUAI6::PlaceType::Grass)
                return false;
        }
    }
    int x1 = int(x + radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    int y1 = int(y - deltaX / deltaY * radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    double myX = double(x1);
    double myY = double(y1);
    for (int i = 0; i < divide; i++)
    {
        myX += dx;
        myY += dy;
        auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
        if (type != THUAI6::PlaceType::Land && type != THUAI6::PlaceType::Grass)
            return false;
    }
    int x2 = int(x - radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    int y2 = int(y + deltaX / deltaY * radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    myX = double(x2);
    myY = double(y2);
    for (int i = 0; i < divide; i++)
    {
        myX += dx;
        myY += dy;
        auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
        if (type != THUAI6::PlaceType::Land && type != THUAI6::PlaceType::Grass)
            return false;
    }
    return true;
}

bool THUAvoidWall(double viewRange, double radius, int x, int y, int newX, int newY, ITrickerAPI& api)
{
    double deltaX = newX - x;
    double deltaY = newY - y;
    double distance = deltaX * deltaX + deltaY * deltaY;
    if (distance >= viewRange * viewRange)
        return false;
    int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
    if (divide == 0)
        return true;
    double dx = deltaX / divide;
    double dy = deltaY / divide;
    if (radius >= 499)
    {
        double myX = double(x);
        double myY = double(y);
        for (int i = 0; i < divide; i++)
        {
            myX += dx;
            myY += dy;
            auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
            if (type == THUAI6::PlaceType::Wall)
                return false;
        }
    }    int x1 = int(x + radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    int y1 = int(y - deltaX / deltaY * radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    double myX = double(x1);
    double myY = double(y1);
    for (int i = 0; i < divide; i++)
    {
        myX += dx;
        myY += dy;
        auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
        if (type == THUAI6::PlaceType::Wall)
            return false;
    }
    int x2 = int(x - radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    int y2 = int(y + deltaX / deltaY * radius / sqrt(1 + (deltaX * deltaX) / (deltaY * deltaY)));
    myX = double(x2);
    myY = double(y2);
    for (int i = 0; i < divide; i++)
    {
        myX += dx;
        myY += dy;
        auto type = api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY));
        if (type == THUAI6::PlaceType::Wall)
            return false;
    }
    return true;
}

//寻找校门
bool THULeaving(ITrickerAPI& api)
{
    if (::THUClassroomUnfinished > 3)
        return false;
    if (THUNeighbor4(api, THUAI6::PlaceType::Gate).first != -1)
    {
        if (api.GetSelfInfo()->trickDesire == 0)
        {
            ::THUTargetGate = (::THUTargetGate + 1) % 2;
        }
        else
        {
            if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
            {
                api.EndAllAction();
            }
            return true;
        }
    }
    double angle = THUAStar(std::make_pair(_CellX, _CellY), ::THUGate[::THUMapIndex][::THUTargetGate], api);
    if (angle == -4)
        return false;
    if (angle == 4)
        return true;
    else
    {
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        ::THUDirection = angle;
        return true;
    }
}

//躲避老师的贴贴
bool THUHumbling(ITrickerAPI& api)
{
    auto vect = api.GetStudents();
    int selfx = _X;
    int selfy = _Y;
    int theirx, theiry;
    for (int i = 0; i < int(vect.size()); ++i)
    {
        if (vect[i]->studentType != THUAI6::StudentType::Teacher || ::THUForTeaNum == -1 && vect[i]->playerState != THUAI6::PlayerState::Addicted)
            continue;
        theirx = vect[i]->x;
        theiry = vect[i]->y;
        if (THUStraightDistance(theirx, theiry, selfx, selfy) <= 801)
        {
            api.EndAllAction();
            api.Move(MOVETIME, std::atan2(selfy - theiry, selfx - theirx));
            return true;
        }
    }
    return false;
}

//爬窗
bool THUClimbing(ITrickerAPI& api)
{
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Climbing)
    {
        if (api.GetPlaceType(api.GridToCell(::THUAttackTarget.first), api.GridToCell(::THUAttackTarget.second)) == THUAI6::PlaceType::Window)
            ::THUAttackTarget.first = 0;
        return true;
    }
    if (::THUAttackTarget.first == 0)
    {
        ::THUAttackTarget = std::make_pair(_X, _Y);
    }
    if (THUCloseNeighbor4(api, THUAI6::PlaceType::Window).first != -1)
    {
        if (::THUShouldClimb)
        {
            if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
                api.EndAllAction();
            api.SkipWindow();
            ::THUShouldClimb = false;
            return true;
        }
        else
            return false;
    }
    else if (THUNeighbor4(api, THUAI6::PlaceType::Window).first != -1)
        ::THUShouldClimb = true;
    return false;
}

//防止卡角
bool THUDodging(ITrickerAPI& api)
{
    int x = _X;
    int y = _Y;
    THUAI6::PlaceType placetype[4];
    bool judge[4] = { 0 };
    placetype[0] = api.GetPlaceType(api.GridToCell(x + AVOANDIS), api.GridToCell(y + AVOANDIS));
    placetype[1] = api.GetPlaceType(api.GridToCell(x - AVOANDIS), api.GridToCell(y + AVOANDIS));
    placetype[2] = api.GetPlaceType(api.GridToCell(x - AVOANDIS), api.GridToCell(y - AVOANDIS));
    placetype[3] = api.GetPlaceType(api.GridToCell(x + AVOANDIS), api.GridToCell(y - AVOANDIS));
    for (int i = 0; i < 4; ++i)
        if (placetype[i] == THUAI6::PlaceType::Wall || (placetype[i] != THUAI6::PlaceType::Land && placetype[i] != THUAI6::PlaceType::Grass && placetype[i] != THUAI6::PlaceType::Door3 && placetype[i] != THUAI6::PlaceType::Door5 && placetype[i] != THUAI6::PlaceType::Door6))
            judge[i] = true;
        else
            judge[i] = false;
    for (int i = 0; i < 4; ++i)
        if (judge[i] && judge[(i + 1) % 4] && judge[(i + 3) % 4])
        {
            api.EndAllAction();
            api.Move(MOVETIME, -0.75 * PI + 0.5 * PI * i);
            return true;
        }
    for (int i = 0; i < 4; ++i)
        if (judge[i] && judge[(i + 1) % 4])
        {
            api.EndAllAction();
            api.Move(DODGETIM, -0.5 * PI + 0.5 * PI * i);
            return true;
        }
    for (int i = 0; i < 4; ++i)
        if (judge[i])
        {
            api.EndAllAction();
            api.Move(MOVETIME, -0.75 * PI + 0.5 * PI * i);
            return true;
        }
    return false;
}

//判断上下左右四位邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> THUNeighbor4(ITrickerAPI& api, THUAI6::PlaceType placetype)
{
    int cellx = _CellX;
    int celly = _CellY;
    if (api.GetPlaceType(cellx, celly + 1) == placetype)    return std::make_pair(cellx, celly + 1);
    if (api.GetPlaceType(cellx, celly - 1) == placetype)    return std::make_pair(cellx, celly - 1);
    if (api.GetPlaceType(cellx + 1, celly) == placetype)    return std::make_pair(cellx + 1, celly);
    if (api.GetPlaceType(cellx - 1, celly) == placetype)    return std::make_pair(cellx - 1, celly);
    return std::make_pair(-1, -1);
}

//判断上下左右四位紧密相邻的邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> THUCloseNeighbor4(ITrickerAPI& api, THUAI6::PlaceType placetype)
{
    int x = _X;
    int y = _Y;
    if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y + CLIWINDIS)) == placetype)    return std::make_pair(api.GridToCell(x), api.GridToCell(y + CLIWINDIS));
    if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y - CLIWINDIS)) == placetype)    return std::make_pair(api.GridToCell(x), api.GridToCell(y - CLIWINDIS));
    if (api.GetPlaceType(api.GridToCell(x + CLIWINDIS), api.GridToCell(y)) == placetype)    return std::make_pair(api.GridToCell(x + CLIWINDIS), api.GridToCell(y));
    if (api.GetPlaceType(api.GridToCell(x - CLIWINDIS), api.GridToCell(y)) == placetype)    return std::make_pair(api.GridToCell(x - CLIWINDIS), api.GridToCell(y));
    return std::make_pair(-1, -1);
}

//返回上下左右所有是可通过地形或目标的邻居的坐标
std::vector<std::pair<int, int>> THUNeighbor40(std::pair<int, int>current, std::pair<int, int>goal, ITrickerAPI& api)
{
    std::vector<std::pair<int, int>>neighbors;
    bool isGoodType[4];
    auto nextType = api.GetPlaceType(current.first, current.second + 1);
    isGoodType[0] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first == goal.first && current.second + 1 == goal.second);
    nextType = api.GetPlaceType(current.first + 1, current.second);
    isGoodType[1] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first + 1 == goal.first && current.second == goal.second);
    nextType = api.GetPlaceType(current.first, current.second - 1);
    isGoodType[2] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first == goal.first && current.second - 1 == goal.second);
    nextType = api.GetPlaceType(current.first - 1, current.second);
    isGoodType[3] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first - 1 == goal.first && current.second == goal.second);
    if (isGoodType[0])
        neighbors.push_back(std::make_pair(current.first, current.second + 1));
    if (isGoodType[1])
        neighbors.push_back(std::make_pair(current.first + 1, current.second));
    if (isGoodType[2])
        neighbors.push_back(std::make_pair(current.first, current.second - 1));
    if (isGoodType[3])
        neighbors.push_back(std::make_pair(current.first - 1, current.second));
    return neighbors;
}

//返回四个角所有地形平坦的邻居的坐标
std::vector<std::pair<int, int>> THUNeighbor41(std::pair<int, int>current, ITrickerAPI& api)
{
    std::vector<std::pair<int, int>>neighbors;
    bool isGoodType[8];
    auto nextType = api.GetPlaceType(current.first, current.second + 1);
    isGoodType[0] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second);
    isGoodType[1] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first, current.second - 1);
    isGoodType[2] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second);
    isGoodType[3] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second + 1);
    isGoodType[4] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second - 1);
    isGoodType[5] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second - 1);
    isGoodType[6] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second + 1);
    isGoodType[7] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    if (isGoodType[4] && isGoodType[0] && isGoodType[1])
        neighbors.push_back(std::make_pair(current.first + 1, current.second + 1));
    if (isGoodType[5] && isGoodType[1] && isGoodType[2])
        neighbors.push_back(std::make_pair(current.first + 1, current.second - 1));
    if (isGoodType[6] && isGoodType[2] && isGoodType[3])
        neighbors.push_back(std::make_pair(current.first - 1, current.second - 1));
    if (isGoodType[7] && isGoodType[3] && isGoodType[0])
        neighbors.push_back(std::make_pair(current.first - 1, current.second + 1));
    return neighbors;
}

//A*
double THUAStar(const std::pair<int, int>& start, const std::pair<int, int>& goal, ITrickerAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost, priority;
    THUAI6::PlaceType nextType;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        if (current == goal)
            break;
        frontier.pop();
        auto neighbors = THUNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.414 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1.414;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                //if (::THUForTeaNum != -1)
                {
                    for (int i = 0; i < 4; ++i)
                        if (THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                //else
                {
                    auto stu = api.GetStudents();
                    for (int i = 0; i < stu.size(); ++i)
                        // if (stu[i]->studentType == THUAI6::StudentType::Teacher && stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                        if (stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + THUHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
        neighbors = THUNeighbor40(current, goal, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.0 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                nextType = api.GetPlaceType(next->first, next->second);
                if (nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6)
                {
                    if (::THUDoorKeys.find(*next) != ::THUDoorKeys.end())
                    {
                        if (!api.IsDoorOpen(next->first, next->second))
                            continue;
                    }
                    else
                    {
                        if (api.HaveView(api.CellToGrid((*next).first), api.CellToGrid((*next).second)))
                            ::THUDoorKeys[*next] = 0;
                    }
                }
                bool tag = false;
                //if (::THUForTeaNum != -1)
                {
                    for (int i = 0; i < 4; ++i)
                        if (THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                //else
                {
                    auto stu = api.GetStudents();
                    for (int i = 0; i < stu.size(); ++i)
                        // if (stu[i]->studentType == THUAI6::StudentType::Teacher && stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                        if (stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + THUHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}

//BFS
double THUBFSPlus(const std::pair<int, int>& start, THUAI6::PlaceType targetType, ITrickerAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost;
    THUAI6::PlaceType nextType;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        if (api.GetPlaceType(current.first, current.second) == targetType)
            break;
        frontier.pop();
        auto neighbors = THUNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.414 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1.414;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                //if (::THUForTeaNum != -1)
                {
                    for (int i = 0; i < 4; ++i)
                        if (THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                //else
                {
                    auto stu = api.GetStudents();
                    for (int i = 0; i < stu.size(); ++i)
                        // if (stu[i]->studentType == THUAI6::StudentType::Teacher && stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                        if (stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
        neighbors = THUNeighbor40(current, std::make_pair(-1, -1), api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.0 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                nextType = api.GetPlaceType(next->first, next->second);
                if (nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6)
                {
                    if (::THUDoorKeys.find(*next) != ::THUDoorKeys.end())
                    {
                        if (!api.IsDoorOpen(next->first, next->second))
                            continue;
                    }
                    else
                    {
                        if (api.HaveView(api.CellToGrid((*next).first), api.CellToGrid((*next).second)))
                            ::THUDoorKeys[*next] = 0;
                    }
                }
                bool tag = false;
                //if (::THUForTeaNum != -1)
                {
                    for (int i = 0; i < 4; ++i)
                        if (THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                //else
                {
                    auto stu = api.GetStudents();
                    for (int i = 0; i < stu.size(); ++i)
                        // if (stu[i]->studentType == THUAI6::StudentType::Teacher && stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                        if (stu[i]->playerState == THUAI6::PlayerState::Addicted && THUStraightDistance(::THUTeacherPos[i].first, ::THUTeacherPos[i].second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN)
                            tag = true;
                }
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}


















































//返回两点垂直距离
int MyHeuristic(const std::pair<int, int>& p1, const std::pair<int, int>& p2)
{
    return std::abs(p1.first - p2.first) + std::abs(p1.second - p2.second);
}

//返回两点直线距离
double MyStraightDistance(int x1, int y1, int x2, int y2)
{
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

//防止卡角
bool MyDodging(IStudentAPI& api)
{
    int x = _X;
    int y = _Y;
    THUAI6::PlaceType placetype[4];
    bool judge[4] = { 0 };
    placetype[0] = api.GetPlaceType(api.GridToCell(x + AVOANDIS), api.GridToCell(y + AVOANDIS));
    placetype[1] = api.GetPlaceType(api.GridToCell(x - AVOANDIS), api.GridToCell(y + AVOANDIS));
    placetype[2] = api.GetPlaceType(api.GridToCell(x - AVOANDIS), api.GridToCell(y - AVOANDIS));
    placetype[3] = api.GetPlaceType(api.GridToCell(x + AVOANDIS), api.GridToCell(y - AVOANDIS));
    for (int i = 0; i < 4; ++i)
        if (placetype[i] == THUAI6::PlaceType::Wall || (placetype[i] != THUAI6::PlaceType::Land && placetype[i] != THUAI6::PlaceType::Grass && placetype[i] != THUAI6::PlaceType::Door3 && placetype[i] != THUAI6::PlaceType::Door5 && placetype[i] != THUAI6::PlaceType::Door6))
            judge[i] = true;
        else
            judge[i] = false;
    for (int i = 0; i < 4; ++i)
        if (judge[i] && judge[(i + 1) % 4] && judge[(i + 3) % 4])
        {
            api.EndAllAction();
            api.Move(MOVETIME, -0.75 * PI + 0.5 * PI * i);
            return true;
        }
    for (int i = 0; i < 4; ++i)
        if (judge[i] && judge[(i + 1) % 4])
        {
            api.EndAllAction();
            api.Move(DODGETIM, -0.5 * PI + 0.5 * PI * i);
            return true;
        }
    for (int i = 0; i < 4; ++i)
        if (judge[i])
        {
            api.EndAllAction();
            api.Move(MOVETIME, -0.75 * PI + 0.5 * PI * i);
            return true;
        }
    return false;
}

bool MyClimbing(IStudentAPI& api)
{
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Climbing)
    {
        return true;
    }
    if (MyCloseNeighbor4(api, THUAI6::PlaceType::Window).first != -1)
    {
        if (::MyShouldClimb)
        {
            if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
                api.EndAllAction();
            api.SkipWindow();
            ::MyShouldClimb = false;
            return true;
        }
        else
            return false;
    }
    else if (MyNeighbor4(api, THUAI6::PlaceType::Window).first != -1)
        ::MyShouldClimb = true;
    return false;
}

void MyDoors(IStudentAPI& api)
{
    if (!::MyShouldHide)
        return;
    if (::MyHidePro == 1)
    {
        if (std::make_pair(_CellX, _CellY) == ::MyLandPos)
        {
            ::MyHidePro = 2;
        }
    }
    return;
}

//判断上下左右四位邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> MyNeighbor4(IStudentAPI& api, THUAI6::PlaceType placetype)
{
    int cellx = _CellX;
    int celly = _CellY;
    if (api.GetPlaceType(cellx, celly + 1) == placetype)    return std::make_pair(cellx, celly + 1);
    if (api.GetPlaceType(cellx, celly - 1) == placetype)    return std::make_pair(cellx, celly - 1);
    if (api.GetPlaceType(cellx + 1, celly) == placetype)    return std::make_pair(cellx + 1, celly);
    if (api.GetPlaceType(cellx - 1, celly) == placetype)    return std::make_pair(cellx - 1, celly);
    return std::make_pair(-1, -1);
}

//判断上下左右四位邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> MyNeighborDoors(IStudentAPI& api, int cellx, int celly)
{
    auto type = api.GetPlaceType(cellx, celly);
    if (type == THUAI6::PlaceType::Window || type == THUAI6::PlaceType::Door3 || type == THUAI6::PlaceType::Door5 || type == THUAI6::PlaceType::Door6)
        return std::make_pair(cellx, celly);
    type = api.GetPlaceType(cellx, celly + 1);
    if (type == THUAI6::PlaceType::Window || type == THUAI6::PlaceType::Door3 || type == THUAI6::PlaceType::Door5 || type == THUAI6::PlaceType::Door6)
        return std::make_pair(cellx, celly + 1);
    type = api.GetPlaceType(cellx, celly - 1);
    if (type == THUAI6::PlaceType::Window || type == THUAI6::PlaceType::Door3 || type == THUAI6::PlaceType::Door5 || type == THUAI6::PlaceType::Door6)
        return std::make_pair(cellx, celly - 1);
    type = api.GetPlaceType(cellx + 1, celly);
    if (type == THUAI6::PlaceType::Window || type == THUAI6::PlaceType::Door3 || type == THUAI6::PlaceType::Door5 || type == THUAI6::PlaceType::Door6)
        return std::make_pair(cellx + 1, celly);
    type = api.GetPlaceType(cellx - 1, celly);
    if (type == THUAI6::PlaceType::Window || type == THUAI6::PlaceType::Door3 || type == THUAI6::PlaceType::Door5 || type == THUAI6::PlaceType::Door6)
        return std::make_pair(cellx - 1, celly);
    return std::make_pair(-1, -1);
}

//判断八位邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> MyNeighbor8(IStudentAPI& api, THUAI6::PlaceType placetype)
{
    int cellx = _CellX;
    int celly = _CellY;
    if (api.GetPlaceType(cellx, celly + 1) == placetype)    return std::make_pair(cellx, celly + 1);
    if (api.GetPlaceType(cellx, celly - 1) == placetype)    return std::make_pair(cellx, celly - 1);
    if (api.GetPlaceType(cellx + 1, celly) == placetype)    return std::make_pair(cellx + 1, celly);
    if (api.GetPlaceType(cellx - 1, celly) == placetype)    return std::make_pair(cellx - 1, celly);
    if (api.GetPlaceType(cellx - 1, celly + 1) == placetype)    return std::make_pair(cellx - 1, celly + 1);
    if (api.GetPlaceType(cellx + 1, celly + 1) == placetype)    return std::make_pair(cellx + 1, celly + 1);
    if (api.GetPlaceType(cellx + 1, celly - 1) == placetype)    return std::make_pair(cellx + 1, celly - 1);
    if (api.GetPlaceType(cellx - 1, celly - 1) == placetype)    return std::make_pair(cellx - 1, celly - 1);
    return std::make_pair(-1, -1);
}

//判断上下左右四位紧密相邻的邻居是否存在给定地形，返回符合条件的邻居的坐标
std::pair<int, int> MyCloseNeighbor4(IStudentAPI& api, THUAI6::PlaceType placetype)
{
    int x = _X;
    int y = _Y;
    if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y + CLIWINDIS)) == placetype)    return std::make_pair(api.GridToCell(x), api.GridToCell(y + CLIWINDIS));
    if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y - CLIWINDIS)) == placetype)    return std::make_pair(api.GridToCell(x), api.GridToCell(y - CLIWINDIS));
    if (api.GetPlaceType(api.GridToCell(x + CLIWINDIS), api.GridToCell(y)) == placetype)    return std::make_pair(api.GridToCell(x + CLIWINDIS), api.GridToCell(y));
    if (api.GetPlaceType(api.GridToCell(x - CLIWINDIS), api.GridToCell(y)) == placetype)    return std::make_pair(api.GridToCell(x - CLIWINDIS), api.GridToCell(y));
    return std::make_pair(-1, -1);
}

//返回上下左右所有是可通过地形或目标的邻居的坐标
std::vector<std::pair<int, int>> MyNeighbor40(std::pair<int, int>current, std::pair<int, int>goal, IStudentAPI& api)
{
    std::vector<std::pair<int, int>>neighbors;
    bool isGoodType[4];
    auto nextType = api.GetPlaceType(current.first, current.second + 1);
    isGoodType[0] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first == goal.first && current.second + 1 == goal.second);
    nextType = api.GetPlaceType(current.first + 1, current.second);
    isGoodType[1] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first + 1 == goal.first && current.second == goal.second);
    nextType = api.GetPlaceType(current.first, current.second - 1);
    isGoodType[2] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first == goal.first && current.second - 1 == goal.second);
    nextType = api.GetPlaceType(current.first - 1, current.second);
    isGoodType[3] = nextType != THUAI6::PlaceType::Wall && (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass || nextType == THUAI6::PlaceType::Window || nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6 || current.first - 1 == goal.first && current.second == goal.second);
    if (isGoodType[0])
        neighbors.push_back(std::make_pair(current.first, current.second + 1));
    if (isGoodType[1])
        neighbors.push_back(std::make_pair(current.first + 1, current.second));
    if (isGoodType[2])
        neighbors.push_back(std::make_pair(current.first, current.second - 1));
    if (isGoodType[3])
        neighbors.push_back(std::make_pair(current.first - 1, current.second));
    return neighbors;
}

//返回四个角所有地形平坦的邻居的坐标
std::vector<std::pair<int, int>> MyNeighbor41(std::pair<int, int>current, IStudentAPI& api)
{
    std::vector<std::pair<int, int>>neighbors;
    bool isGoodType[8];
    auto nextType = api.GetPlaceType(current.first, current.second + 1);
    isGoodType[0] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second);
    isGoodType[1] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first, current.second - 1);
    isGoodType[2] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second);
    isGoodType[3] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second + 1);
    isGoodType[4] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first + 1, current.second - 1);
    isGoodType[5] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second - 1);
    isGoodType[6] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    nextType = api.GetPlaceType(current.first - 1, current.second + 1);
    isGoodType[7] = (nextType == THUAI6::PlaceType::Land || nextType == THUAI6::PlaceType::Grass);
    if (isGoodType[4] && isGoodType[0] && isGoodType[1])
        neighbors.push_back(std::make_pair(current.first + 1, current.second + 1));
    if (isGoodType[5] && isGoodType[1] && isGoodType[2])
        neighbors.push_back(std::make_pair(current.first + 1, current.second - 1));
    if (isGoodType[6] && isGoodType[2] && isGoodType[3])
        neighbors.push_back(std::make_pair(current.first - 1, current.second - 1));
    if (isGoodType[7] && isGoodType[3] && isGoodType[0])
        neighbors.push_back(std::make_pair(current.first - 1, current.second + 1));
    return neighbors;
}


//A*
double MyAStar(const std::pair<int, int>& start, const std::pair<int, int>& goal, IStudentAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost, priority;
    THUAI6::PlaceType nextType;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        if (current == goal)
            break;
        frontier.pop();
        auto neighbors = MyNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.414 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1.414;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && stu[i]->playerID != ::MyRescueID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + MyHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
        neighbors = MyNeighbor40(current, goal, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.0 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                nextType = api.GetPlaceType(next->first, next->second);
                if (nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6)
                {
                    if (::MyDoorKeys.find(*next) != ::MyDoorKeys.end())
                    {
                        if (!api.IsDoorOpen(next->first, next->second))
                            continue;
                    }
                    else
                    {
                        if (api.HaveView(api.CellToGrid((*next).first), api.CellToGrid((*next).second)))
                            ::MyDoorKeys[*next] = 0;
                    }
                }
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && stu[i]->playerID != ::MyRescueID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + MyHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}

//A*
double MyAStarFortea(const std::pair<int, int>& start, const std::pair<int, int>& goal, IStudentAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost, priority;
    THUAI6::PlaceType nextType;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        if (current == goal)
            break;
        frontier.pop();
        auto neighbors = MyNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.414 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1.414;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && stu[i]->playerID != ::MyRescueID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + MyHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
        neighbors = MyNeighbor40(current, goal, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            if (api.GetPlaceType(next->first, next->second) == THUAI6::PlaceType::Grass)
                new_cost = cost_so_far[current] + 1.0 / GRASSGAIN;
            else
                new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                nextType = api.GetPlaceType(next->first, next->second);
                if (nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6)
                {
                    if (::MyDoorKeys.find(*next) != ::MyDoorKeys.end())
                    {
                        if (!api.IsDoorOpen(next->first, next->second))
                            continue;
                    }
                    else
                    {
                        if (api.HaveView(api.CellToGrid((*next).first), api.CellToGrid((*next).second)))
                            ::MyDoorKeys[*next] = 0;
                    }
                }
                bool tag = false;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && stu[i]->playerID != ::MyRescueID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                priority = new_cost + MyHeuristic(goal, *next);
                frontier.push(std::make_pair(priority, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}

//BFS
double MyBFSPlus(const std::pair<int, int>& start, THUAI6::PlaceType targetType, IStudentAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        if (api.GetPlaceType(current.first, current.second) == targetType)
            break;
        frontier.pop();
        auto neighbors = MyNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            new_cost = cost_so_far[current] + 1.414;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
        neighbors = MyNeighbor40(current, std::make_pair(-1, -1), api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                auto type = int(api.GetPlaceType(next->first, next->second));
                if (type >= 7 && type <= 10)
                    continue;
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}

//BFS
double MyBFSPlusForDoors(const std::pair<int, int>& start, IStudentAPI& api)
{
    std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, FrontierCompare>frontier;
    frontier.push(std::make_pair(0, start));
    std::map<std::pair<int, int>, double> cost_so_far;
    cost_so_far[start] = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    came_from[start] = start;
    std::pair<int, int> current;
    double new_cost;
    THUAI6::PlaceType nextType;
    while (!frontier.empty())
    {
        current = frontier.top().second;
        auto type = int(api.GetPlaceType(current.first, current.second));
        if (type >= 7 && type <= 10)
            break;
        frontier.pop();
        auto neighbors = MyNeighbor41(current, api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            double xxx;
            if (::MyTriPos.first != -2)
                xxx = ((::MyTriPos.first - api.CellToGrid(start.first)) * (api.CellToGrid(next->first) - api.CellToGrid(start.first)) + (::MyTriPos.second - api.CellToGrid(start.second)) * (api.CellToGrid(next->second) - api.CellToGrid(start.second))) / (sqrt((::MyTriPos.first - api.CellToGrid(start.first)) * (::MyTriPos.first - api.CellToGrid(start.first)) + (::MyTriPos.second - api.CellToGrid(start.second)) * (::MyTriPos.second - api.CellToGrid(start.second))) * sqrt((api.CellToGrid(next->first) - api.CellToGrid(start.first)) * (api.CellToGrid(next->first) - api.CellToGrid(start.first)) + (api.CellToGrid(next->second) - api.CellToGrid(start.second)) * (api.CellToGrid(next->second) - api.CellToGrid(start.second))));
            else
                xxx = 0;
            new_cost = cost_so_far[current] + 1.414 * (xxx + 1);
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
        neighbors = MyNeighbor40(current, std::make_pair(-1, -1), api);
        for (auto next = neighbors.begin(); next != neighbors.end(); ++next)
        {
            double xxx;
            if (::MyTriPos.first != -2)
                xxx = ((::MyTriPos.first - api.CellToGrid(start.first)) * (api.CellToGrid(next->first) - api.CellToGrid(start.first)) + (::MyTriPos.second - api.CellToGrid(start.second)) * (api.CellToGrid(next->second) - api.CellToGrid(start.second))) / (sqrt((::MyTriPos.first - api.CellToGrid(start.first)) * (::MyTriPos.first - api.CellToGrid(start.first)) + (::MyTriPos.second - api.CellToGrid(start.second)) * (::MyTriPos.second - api.CellToGrid(start.second))) * sqrt((api.CellToGrid(next->first) - api.CellToGrid(start.first)) * (api.CellToGrid(next->first) - api.CellToGrid(start.first)) + (api.CellToGrid(next->second) - api.CellToGrid(start.second)) * (api.CellToGrid(next->second) - api.CellToGrid(start.second))));
            else
                xxx = 0;
            new_cost = cost_so_far[current] + 1.0 * (xxx + 1);
            if (cost_so_far.find(*next) == cost_so_far.end() || new_cost < cost_so_far[*next])
            {
                nextType = api.GetPlaceType(next->first, next->second);
                if (nextType == THUAI6::PlaceType::Door3 || nextType == THUAI6::PlaceType::Door5 || nextType == THUAI6::PlaceType::Door6)
                {
                    if (::MyDoorKeys.find(*next) != ::MyDoorKeys.end())
                    {
                        if (!api.IsDoorOpen(next->first, next->second))
                            continue;
                    }
                    else
                    {
                        if (api.HaveView(api.CellToGrid((*next).first), api.CellToGrid((*next).second)))
                            ::MyDoorKeys[*next] = 0;
                    }
                }
                bool tag = false;
                for (int i = 0; i < 4; ++i)
                    if (MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < AVOIDRAN && MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, api.CellToGrid(next->first), api.CellToGrid(next->second)) < MyStraightDistance(::MyTriPos.first, ::MyTriPos.second, _X, _Y))
                        tag = true;
                auto stu = api.GetStudents();
                for (int i = 0; i < stu.size(); ++i)
                    if (stu[i]->playerID != api.GetSelfInfo()->playerID && MyStraightDistance(stu[i]->x, stu[i]->y, api.CellToGrid(next->first), api.CellToGrid(next->second)) < 500 && MyNeighborDoors(api, next->first, next->second).first == -1)
                        tag = true;
                if (tag)
                    continue;
                cost_so_far[*next] = new_cost;
                frontier.push(std::make_pair(new_cost, *next));
                came_from[*next] = current;
            }
        }
    }
    if (frontier.empty())
        return -4;
    else if (current == start)
        return 4;
    else
    {
        auto x = came_from[current];
        ::MyLandPos = std::make_pair(2 * current.first - x.first, 2 * current.second - x.second);
        while (came_from[current] != start)
            current = came_from[current];
        return std::atan2(current.second - start.second, current.first - start.first);
    }
}

//试图毕业
bool MyGraduating(IStudentAPI& api)
{
    if (::MyClassroomUnfinished > int(::MyClassroom[::MyMapIndex].size()) - 7)
        return false;
    auto xy = MyNeighbor4(api, THUAI6::PlaceType::Gate);
    if (xy.first == -1)
        return false;
    if (api.GetGateProgress(xy.first, xy.second) == MAXGATEPRO)
    {
        api.Graduate();
        return true;
    }
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
        api.EndAllAction();
    api.StartOpenGate();
    return true;
}

//寻找校门
bool MyLeaving(IStudentAPI& api)
{
    if (::MyClassroomUnfinished > int(::MyClassroom[::MyMapIndex].size()) - 7)
        return false;
    if (::MyTargetGate == -1)
    {
        auto dis1 = MyHeuristic(std::make_pair(_CellX, _CellY), ::MyGate[::MyMapIndex][0]);
        auto dis2 = MyHeuristic(std::make_pair(_CellX, _CellY), ::MyGate[::MyMapIndex][1]);
        ::MyTargetGate = dis1 < dis2 ? 0 : 1;
    }
    double angle = MyAStar(std::make_pair(_CellX, _CellY), ::MyGate[::MyMapIndex][::MyTargetGate], api);
    if (angle == -4)
        return false;
    if (angle == 4)
        return true;
    else
    {
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        return true;
    }
}

//防止被不可视学生堵住
bool MyJumping(IStudentAPI& api)
{
    if (::MyJumpTime == 30)
        ::MyDirection = api.GetSelfInfo()->facingDirection;
    if (::MyShouldHide && api.GetPlaceType(_CellX, _CellY) == THUAI6::PlaceType::Grass)
        return false;
    ::MyPosition.push_back(std::make_pair(_X, _Y));
    ::MyState.push_back(api.GetSelfInfo()->playerState);
    if (::MyPosition.size() > 20)
    {
        ::MyPosition.erase(::MyPosition.begin());
        ::MyState.erase(::MyState.begin());
    }
    else
        return false;
    if (::MyJumpTime == 30)
    {
        for (int i = 0; i < ::MyPosition.size() - 1; ++i)
        {
            if (::MyPosition[i] != ::MyPosition[i + 1] || ::MyState[i] != THUAI6::PlayerState::Idle)
                return false;
        }
    }
    if (::MyJumpTime > 15)
    {
        --::MyJumpTime;
        api.EndAllAction();
        api.Move(MOVETIME, ::MyDirection - PI);
        return true;
    }
    else if (::MyJumpTime > 0)
    {
        --::MyJumpTime;
        api.EndAllAction();
        api.Move(MOVETIME, ::MyDirection - PI / 2);
        return true;
    }
    ::MyJumpTime = 30;
    return false;
}


//信息处理
void MyCommunicating(IStudentAPI& api)
{
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Addicted)
    {
        ::MyShouldHide = false;
        ::MyHidePro = 0;
        ::MyLandPos = std::make_pair(-1, -1);
        auto stu = api.GetStudents();
        for (int i = 0; i < int(stu.size()); ++i)
        {
            if (stu[i]->playerID == api.GetSelfInfo()->playerID || stu[i]->timeUntilSkillAvailable[0] > 0)
                continue;
            if (MyHaveView(stu[i]->viewRange, stu[i]->x, stu[i]->y, api.GetSelfInfo()->x, api.GetSelfInfo()->y, api))
            {
                api.SendTextMessage(stu[i]->playerID, fmt::format("r"));
                break;
            }
        }
    }
    auto tri = api.GetTrickers();
    if (!tri.empty())
    {
        ::MyTriPos = std::make_pair(tri[0]->x, tri[0]->y);
        for (int i = 0; i < 4; ++i)
        {
            if (i == api.GetSelfInfo()->playerID)
                continue;
            api.SendTextMessage(i, fmt::format("p{:05d}{:05d}", ::MyTriPos.first, ::MyTriPos.second));
        }
    }
    else
    {
        ::MyTriPos = std::make_pair(-2, -2);
        if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
            ::MyCallID = -1;
        auto stu = api.GetStudents();
        for (int i = 0; i < int(stu.size()); ++i)
        {
            if (stu[i]->playerID == ::MyCallID && stu[i]->playerState == THUAI6::PlayerState::Addicted)
            {
                ::MyCallID = -1;
                break;
            }
        }
        //if (::MyCallID == -1 && api.GetSelfInfo()->playerState == THUAI6::PlayerState::Addicted)
        //{
        //    std::map<double, int>cls;
        //    auto stu = api.GetStudents();
        //    for (int i = 0; i < int(stu.size()); ++i)
        //    {
        //        if (stu[i]->playerState != THUAI6::PlayerState::Addicted)
        //            cls[MyStraightDistance(_CellX, _CellY, api.GridToCell(stu[i]->x), api.GridToCell(stu[i]->y))] = stu[i]->playerID;
        //    }
        //    if (!cls.empty())
        //    {
        //        ::MyCallID = cls.begin()->second;
        //        api.SendTextMessage(::MyCallID, fmt::format("c{}",api.GetSelfInfo()->playerID));
        //    }
        //    api.SendTextMessage(::MyCallID, fmt::format("c{}", api.GetSelfInfo()->playerID));
        //}
    }



    if (::MyCallID == -1 && api.GetSelfInfo()->playerState == THUAI6::PlayerState::Addicted)
    {
        ::MyCallID = 2;
        api.SendTextMessage(::MyCallID, fmt::format("c{}", api.GetSelfInfo()->playerID));
    }
    while (api.HaveMessage())
    {
        std::string m = api.GetMessage().second;
        if (m[0] == 'r')
        {
            api.UseSkill(0);
            api.UseSkill(1);
        }
        else if (m[0] == 'h')
        {
            ::MyClassroom[::MyMapIndex][m[1] - '0'].second = 0;
        }
        else if (m[0] == 'p' && ::MyTriPos.first == -2)
        {
            ::MyTriPos = std::make_pair((m[1] - '0') * 10000 + (m[2] - '0') * 1000 + (m[3] - '0') * 100 + (m[4] - '0') * 10 + (m[5] - '0'), (m[6] - '0') * 10000 + (m[7] - '0') * 1000 + (m[8] - '0') * 100 + (m[9] - '0') * 10 + (m[10] - '0'));
        }
        else if (m[0] == 'c' && ::MyRescueID == -1)
        {
            ::MyRescueID = m[1] - '0';
        }
    }
    return;
}

bool MyHaveView(double viewRange, int x, int y, int newX, int newY, IStudentAPI& api)
{
    int32_t deltaX = newX - x;
    int32_t deltaY = newY - y;
    double distance = deltaX * deltaX + deltaY * deltaY;
    if (distance >= viewRange * viewRange)
        return false;
    THUAI6::PlaceType myPlace = api.GetPlaceType(api.GridToCell(x), api.GridToCell(y));
    THUAI6::PlaceType newPlace = api.GetPlaceType(api.GridToCell(newX), api.GridToCell(newY));
    if (newPlace == THUAI6::PlaceType::Grass && myPlace != THUAI6::PlaceType::Grass)
        return false;
    if (distance < viewRange * viewRange)
    {
        int32_t divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
        if (divide == 0)
            return true;
        double dx = deltaX / divide;
        double dy = deltaY / divide;
        double myX = double(x);
        double myY = double(y);
        if (newPlace == THUAI6::PlaceType::Grass && myPlace == THUAI6::PlaceType::Grass)  // 都在草丛内，要另作判断
            for (int32_t i = 0; i < divide; i++)
            {
                myX += dx;
                myY += dy;
                if (api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY)) != THUAI6::PlaceType::Grass)
                    return false;
            }
        else
            for (int32_t i = 0; i < divide; i++)
            {
                myX += dx;
                myY += dy;
                if (api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY)) == THUAI6::PlaceType::Wall)
                    return false;
            }
        return true;
    }
    else
        return false;
}

bool MyHaveViewForWin(int x, int y, int newX, int newY, IStudentAPI& api)
{
    int32_t deltaX = newX - x;
    int32_t deltaY = newY - y;
    double distance = deltaX * deltaX + deltaY * deltaY;
    int32_t divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
    if (divide == 0)
        return true;
    double dx = deltaX / divide;
    double dy = deltaY / divide;
    double myX = double(x);
    double myY = double(y);
    for (int32_t i = 0; i < divide; i++)
    {
        myX += dx;
        myY += dy;
        auto type = int(api.GetPlaceType(api.GridToCell(myX), api.GridToCell(myY)));
        if (type >= 7 && type <= 10)
            return false;
    }
    return true;
}

//试图学习
bool MyLearning(IStudentAPI& api)
{
    auto xy = MyNeighbor4(api, THUAI6::PlaceType::ClassRoom);
    if (xy != ::MyClassroom[::MyMapIndex][::MyTargetClassroom].first)
        return false;
    if (api.GetClassroomProgress(xy.first, xy.second) == MAXPRO)
    {
        ::MyClassroom[::MyMapIndex][::MyTargetClassroom].second = 0;
        for (int i = 0; i < 4; ++i)
            api.SendTextMessage(i, fmt::format("h{}", ::MyTargetClassroom));
        std::map<double, int>cls;
        for (int i = 0; i < int(::MyClassroom[::MyMapIndex].size()); ++i)
        {
            if (::MyClassroom[::MyMapIndex][i].second == 1)
                cls[MyStraightDistance(_CellX, _CellY, ::MyClassroom[::MyMapIndex][i].first.first, ::MyClassroom[::MyMapIndex][i].first.second)] = i;
            else
                cls[999] = i;
        }
        ::MyTargetClassroom = cls.begin()->second;
        return false;
    }
    if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
        api.EndAllAction();
    api.StartLearning();
    return true;
}

//察觉或遇到危险躲进草丛
bool MyHiding(IStudentAPI& api)
{
    auto tri = api.GetTrickers();
    if (!tri.empty() && ::MyShouldHide == false)
    {
        //if (MyHaveViewForWin(tri[0]->x, tri[0]->y, _X, _Y, api) && (::MyMapIndex == 0 || (_CellX > 8 && _CellX < 41 && _CellY > 8 && _CellY < 41)))
        //{
        //    ::MyShouldHide = true;
        //    ::MyHideTime = 0;
        //    ::MyHidePro = 0;
        //}
        //else
        //{
        //    ::MyShouldHide = true;
        //    ::MyHideTime = 0;
        //    ::MyHidePro = 10;
        //}
        ::MyShouldHide = true;
        ::MyHideTime = 0;
        ::MyHidePro = 0;
    }
    if (api.GetSelfInfo()->addiction > 40000)
    {
        ::MyShouldHide = true;
        ::MyHideTime = 0;
        ::MyHidePro = 10;
    }
    if (::MyShouldHide && ::MyHidePro != 10)
    {
        if (::MyHidePro == 0)
        {
            double angle = MyBFSPlusForDoors(std::make_pair(_CellX, _CellY), api);
            if (angle == -4)
                return false;
            else if (angle == 4)
                return true;
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                ::MyHidePro = 1;
                return true;
            }
        }
        else if (::MyHidePro == 1)
        {
            api.EndAllAction();
            double angle = MyAStar(std::make_pair(_CellX, _CellY), ::MyLandPos, api);
            if (angle == -4)
                return false;
            else if (angle == 4)
                return true;
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                return true;
            }
        }
        else if (api.GetPlaceType(_CellX, _CellY) == THUAI6::PlaceType::Grass)
        {
            api.EndAllAction();
            ++::MyHideTime;
            if (::MyHideTime >= ::MyMaxHidTime)
            {
                if (api.GetSelfInfo()->addiction > 40000)
                {
                    ::MyShouldHide = true;
                    ::MyHidePro = 10;
                    ::MyLandPos = std::make_pair(-1, -1);
                    return true;
                }
                ::MyShouldHide = false;
                ::MyHidePro = 0;
                ::MyLandPos = std::make_pair(-1, -1);
                std::map<double, int>cls;
                for (int i = 0; i < int(::MyClassroom[::MyMapIndex].size()); ++i)
                {
                    if (::MyClassroom[::MyMapIndex][i].second == 1)
                        cls[MyStraightDistance(_CellX, _CellY, ::MyClassroom[::MyMapIndex][i].first.first, ::MyClassroom[::MyMapIndex][i].first.second)] = i;
                    else
                        cls[999] = i;
                }
                ::MyTargetClassroom = cls.begin()->second;
                return false;
            }
            return true;
        }
        else if (::MyHidePro == 2)
        {
            double angle = MyBFSPlus(std::make_pair(_CellX, _CellY), THUAI6::PlaceType::Grass, api);
            if (angle == -4)
                return false;
            else if (angle == 4)
                return true;
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                return true;
            }
        }
    }
    else if (::MyShouldHide && ::MyHidePro == 10)
    {
        if (api.GetPlaceType(_CellX, _CellY) == THUAI6::PlaceType::Grass)
        {
            api.EndAllAction();
            ++::MyHideTime;
            if (::MyHideTime >= ::MyMaxHidTime)
            {
                ::MyShouldHide = false;
                ::MyHidePro = 0;
                ::MyLandPos = std::make_pair(-1, -1);
                return false;
            }
            return true;
        }
        else
        {
            double angle = MyBFSPlus(std::make_pair(_CellX, _CellY), THUAI6::PlaceType::Grass, api);
            if (angle == -4)
                return false;
            else if (angle == 4)
                return true;
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                return true;
            }
        }

    }
    return false;
}

//察觉或遇到危险躲进草丛
bool MyHidingForTea(IStudentAPI& api)
{
    if (!api.GetTrickers().empty()|| ::MyRescueID == -1)
    {
        ::MyShouldHide = true;
        ::MyHideTime = 0;
    }
    if (::MyShouldHide)
    {
        if (api.GetPlaceType(_CellX, _CellY) == THUAI6::PlaceType::Grass)
        {
            api.EndAllAction();
            ++::MyHideTime;
            if (::MyHideTime >= 150)
            {
                ::MyShouldHide = false;
                ::MyHidePro = 0;
                ::MyLandPos = std::make_pair(-1, -1);
                return false;
            }
            return true;
        }
        else
        {
            double angle = MyBFSPlus(std::make_pair(_CellX, _CellY), THUAI6::PlaceType::Grass, api);
            if (angle == -4)
                return false;
            else if (angle == 4)
                return true;
            else
            {
                api.EndAllAction();
                api.Move(MOVETIME, angle);
                return true;
            }
        }
    }
    return false;
}

//寻找作业
bool MyAttending(IStudentAPI& api)
{
    if (::MyTargetClassroom == -1)
    {
        std::map<double, int>cls;
        for (int i = 0; i < int(::MyClassroom[::MyMapIndex].size()); ++i)
        {
            if (::MyClassroom[::MyMapIndex][i].second == 1)
                cls[MyStraightDistance(_CellX, _CellY, ::MyClassroom[::MyMapIndex][i].first.first, ::MyClassroom[::MyMapIndex][i].first.second)] = i;
            else
                cls[999] = i;
        }
        ::MyTargetClassroom = cls.begin()->second;
    }
    double angle = MyAStar(std::make_pair(_CellX, _CellY), ::MyClassroom[::MyMapIndex][::MyTargetClassroom].first, api);
    if (angle == -4)
        return false;
    if (angle == 4)
        return true;
    api.EndAllAction();
    api.Move(MOVETIME, angle);
    return true;
}

bool MyRescuing(IStudentAPI& api)
{
    if (::MyRescueID == -1)
        return false;
    auto stu = api.GetStudents();
    for (int i = 0; i<int(stu.size()); ++i)
    {
        if (stu[i]->playerID == ::MyRescueID)
        {
            if (stu[i]->playerState != THUAI6::PlayerState::Addicted && stu[i]->playerState != THUAI6::PlayerState::Roused || stu[i]->playerState == THUAI6::PlayerState::Quit)
            {
                ::MyRescueID = -1;
                std::map<double, int>cls;
                for (int i = 0; i < int(::MyClassroom[::MyMapIndex].size()); ++i)
                {
                    if (::MyClassroom[::MyMapIndex][i].second == 1)
                        cls[MyStraightDistance(_CellX, _CellY, ::MyClassroom[::MyMapIndex][i].first.first, ::MyClassroom[::MyMapIndex][i].first.second)] = i;
                    else
                        cls[999] = i;
                }
                ::MyTargetClassroom = cls.begin()->second;
                return false;
            }
            else if (abs(api.GridToCell(stu[i]->x) - _CellX) <= 1 && abs(api.GridToCell(stu[i]->y) - _CellY) <= 1)
            {
                if (api.GetSelfInfo()->playerState == THUAI6::PlayerState::Idle)
                    api.EndAllAction();
                api.StartRouseMate(::MyRescueID);
                return true;
            }
            else
            {
                double angle = MyAStar(std::make_pair(_CellX, _CellY), std::make_pair(api.GridToCell(stu[i]->x), api.GridToCell(stu[i]->y)), api);
                if (angle == 4)
                    return true;
                else if (angle == -4)
                    return false;
                else
                {
                    api.EndAllAction();
                    api.Move(MOVETIME, angle);
                    return true;
                }
            }
        }
    }
    return false;
}

bool MyTask0(IStudentAPI& api)
{
    if (::MyTaskPro != 0)
        return false;
    auto xy = MyNeighbor8(api, THUAI6::PlaceType::ClassRoom);
    if (xy == ::MyClassroom[::MyMapIndex][::MyTargetClassroom].first)
    {
        ::MyTaskPro = 1;
        return false;
    }
    double angle = MyAStar(std::make_pair(_CellX, _CellY), ::MyClassroom[::MyMapIndex][::MyTargetClassroom].first, api);
    if (angle == 4)
        return true;
    else if (angle == -4)
        return false;
    else
    {
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        return true;
    }
}

void MyInitializing(IStudentAPI& api)
{
    if (::MyMapIndex == -1)
    {
        if (api.GetPlaceType(6, 13) == THUAI6::PlaceType::ClassRoom)
            ::MyMapIndex = 1;
        else
            ::MyMapIndex = 0;
        if (::MyMapIndex == 0)
        {
            if (api.GetSelfInfo()->playerID == 0)
                ::MyTargetClassroom = 1;
            else if (api.GetSelfInfo()->playerID == 1)
                ::MyTargetClassroom = 3;
            else if (api.GetSelfInfo()->playerID == 2)
                ::MyTargetClassroom = 5;
            else if (api.GetSelfInfo()->playerID == 3)
                ::MyTargetClassroom = 2;
        }
        else
        {
            if (api.GetSelfInfo()->playerID == 0)
                ::MyTargetClassroom = 0;
            else if (api.GetSelfInfo()->playerID == 1)
                ::MyTargetClassroom = 2;
            else if (api.GetSelfInfo()->playerID == 2)
                ::MyTargetClassroom = 3;
            else if (api.GetSelfInfo()->playerID == 3)
                ::MyTargetClassroom = 1;
        }
    }
}

//试图攻击教室
bool MyLearningForTea(IStudentAPI& api)
{
    if (api.HaveView(api.CellToGrid(::THUClassroom[::MyMapIndex][::THUTargetClassroom].first.first), api.CellToGrid(::THUClassroom[::MyMapIndex][::THUTargetClassroom].first.second)))
    {
        ::THUTargetClassroom = (::THUTargetClassroom + 1) % 10;
        return false;
    }
    else
    {
        double angle = MyAStar(std::make_pair(_CellX, _CellY), ::THUClassroom[::MyMapIndex][::THUTargetClassroom].first, api);
        if (angle == -4)
            return false;
        if (angle == 4)
            return true;
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        return true;
    }
    return false;
}

bool MySkillForTea(IStudentAPI& api)
{
    if (api.GetSelfInfo()->timeUntilSkillAvailable[0] > 0)
        return false;
    auto tri = api.GetTrickers();
    if (!tri.empty())
    {
        if (tri[0]->playerState == THUAI6::PlayerState::Swinging || tri[0]->playerState == THUAI6::PlayerState::UsingSpecialSkill || tri[0]->playerState == THUAI6::PlayerState::Climbing || tri[0]->playerState == THUAI6::PlayerState::Locking)
        {
            if (MyStraightDistance(_X, _Y, tri[0]->x, tri[0]->y) <= 3000)
                api.UseSkill(0);
            else if (MyStraightDistance(_X, _Y, tri[0]->x, tri[0]->y) <= 6000 && api.GetSelfInfo()->timeUntilSkillAvailable[1] == 0)
            {
                api.UseSkill(1, 1000 * atan2(tri[0]->y - _Y, tri[0]->x - _X));
                api.UseSkill(0);
            }
        }
    }
    else
        return false;
}

bool MyChasingForTea(IStudentAPI& api)
{
    if (api.GetSelfInfo()->timeUntilSkillAvailable[0] > 0)
        return false;
    auto tri = api.GetTrickers();
    if (tri.empty())
        return false;
    double angle = MyAStarFortea(std::make_pair(_CellX, _CellY), std::make_pair(api.GridToCell(tri[0]->x), api.GridToCell(tri[0]->y)), api);
    if (angle == 4)
        return true;
    else if (angle == -4)
        return false;
    else
    {
        api.EndAllAction();
        api.Move(MOVETIME, angle);
        return true;
    }
}