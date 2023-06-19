//四学霸策略

#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include "API.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType =
{
    THUAI6::StudentType::StraightAStudent ,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent
};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数

//全局变量
int32_t HomeworkLocations[10][2]{ {8,31},{18,5},{10,38},{19,41},{22,18},{28,26},{30,7},{33,40},{40,12},{44,32}};
int32_t GrassLocations[13][2]{ {8,2},{8,16},{5,25},{4,29},{10,46},{18,46},{20,36},{15,29},{20,1},{30,25},{40,24},{39,35},{46,8} };

bool IfResetDestination = true;

std::vector<bool> Condition1 = { false, true, false, true, false, false, false, false, true ,true ,true, false };//门都可以，草地，land可以



namespace MoveFunc
{
    int32_t Distance(int32_t gridX0, int32_t gridY0, int32_t gridX1, int32_t gridY1)
    {
        return int32_t(sqrt((gridX0 - gridX1) * (gridX0 - gridX1) + (gridY0 - gridY1) * (gridY0 - gridY1)));
    }

    //角度为0指向1
    double Angle(int32_t gridX0, int32_t gridY0, int32_t gridX1, int32_t gridY1)
    {
        return atan2(gridY1 - gridY0, gridX1 - gridX0);
    }

    namespace FindWay
    {

        int IfAchievable(THUAI6::PlaceType ThisPlace, std::vector<bool> Condition)
        {
            //是否可以通过的条件可以进一步完善
            if ((ThisPlace == THUAI6::PlaceType::NullPlaceType && Condition[0])
                || (ThisPlace == THUAI6::PlaceType::Land && Condition[1])
                || (ThisPlace == THUAI6::PlaceType::Wall && Condition[2])
                || (ThisPlace == THUAI6::PlaceType::Grass && Condition[3])
                || (ThisPlace == THUAI6::PlaceType::ClassRoom && Condition[4])
                || (ThisPlace == THUAI6::PlaceType::Gate && Condition[5])
                || (ThisPlace == THUAI6::PlaceType::HiddenGate && Condition[6])
                || (ThisPlace == THUAI6::PlaceType::Window && Condition[7])
                || (ThisPlace == THUAI6::PlaceType::Door3 && Condition[8])
                || (ThisPlace == THUAI6::PlaceType::Door5 && Condition[9])
                || (ThisPlace == THUAI6::PlaceType::Door6 && Condition[10])
                || (ThisPlace == THUAI6::PlaceType::Chest && Condition[11]))
                return 0;
            else
                return 1;//1不可达
        }

        const int kCost1 = 10; //直移一格消耗
        const int kCost2 = 14; //斜移一格消耗

        struct Point
        {
            int x, y; //点坐标，这里为了方便按照C++的数组来计算，x代表横排，y代表竖列
            int F, G, H; //F=G+H
            Point* parent; //parent的坐标，这里没有用指针，从而简化代码
            Point(int _x, int _y) :x(_x), y(_y), F(0), G(0), H(0), parent(NULL)  //变量初始化
            {
            }
        };

        class Astar
        {
        public:
            void InitAstar(const std::vector<std::vector<THUAI6::PlaceType>>& _maze, std::vector<bool> Condition, IStudentAPI& api);
            std::list<Point*> GetPath(Point& startPoint, Point& endPoint, bool isIgnoreCorner);
            std::vector<bool> ThisCondition;
            Astar(IStudentAPI& _api) :
                maze({ {} }), api(_api), openList({}), closeList({}) {}

        private:
            Point* findPath(Point& startPoint, Point& endPoint, bool isIgnoreCorner);
            std::vector<Point*> getSurroundPoints(const Point* point, bool isIgnoreCorner) const;
            bool isCanreach(const Point* point, const Point* target, bool isIgnoreCorner) const; //判断某点是否可以用于下一步判断
            Point* isInList(const std::list<Point*>& list, const Point* point) const; //判断开启/关闭列表中是否包含某点
            Point* getLeastFpoint(); //从开启列表中返回F值最小的节点
            //计算FGH值
            int calcG(Point* temp_start, Point* point);
            int calcH(Point* point, Point* end);
            int calcF(Point* point);
        private:
            std::vector<std::vector<THUAI6::PlaceType>> maze;
            std::list<Point*> openList;  //开启列表
            std::list<Point*> closeList; //关闭列表
            IStudentAPI& api;
        };




        void Astar::InitAstar(const std::vector<std::vector<THUAI6::PlaceType>>& _maze, std::vector<bool> Condition, IStudentAPI& _api)
        {
            maze = _maze;
            ThisCondition = Condition;
            api = _api;
        }

        int Astar::calcG(Point* temp_start, Point* point)
        {
            int extraG = (abs(point->x - temp_start->x) + abs(point->y - temp_start->y)) == 1 ? kCost1 : kCost2;
            int parentG = point->parent == NULL ? 0 : point->parent->G; //如果是初始节点，则其父节点是空
            return parentG + extraG;
        }

        int Astar::calcH(Point* point, Point* end)
        {
            //用简单的欧几里得距离计算H，这个H的计算是关键，还有很多算法，没深入研究^_^
            return sqrt((double)(end->x - point->x) * (double)(end->x - point->x) + (double)(end->y - point->y) * (double)(end->y - point->y)) * kCost1;
        }

        int Astar::calcF(Point* point)
        {
            return point->G + point->H;
        }

        Point* Astar::getLeastFpoint()
        {
            if (!openList.empty())
            {
                auto resPoint = openList.front();
                for (auto& point : openList)
                    if (point->F < resPoint->F)
                        resPoint = point;
                return resPoint;
            }
            return NULL;
        }

        Point* Astar::findPath(Point& startPoint, Point& endPoint, bool isIgnoreCorner)
        {
            openList.push_back(new Point(startPoint.x, startPoint.y)); //置入起点,拷贝开辟一个节点，内外隔离
            while (!openList.empty())
            {
                auto curPoint = getLeastFpoint(); //找到F值最小的点
                openList.remove(curPoint); //从开启列表中删除
                closeList.push_back(curPoint); //放到关闭列表
                //1,找到当前周围八个格中可以通过的格子
                auto surroundPoints = getSurroundPoints(curPoint, isIgnoreCorner);
                for (auto& target : surroundPoints)
                {
                    //2,对某一个格子，如果它不在开启列表中，加入到开启列表，设置当前格为其父节点，计算F G H
                    if (!isInList(openList, target))
                    {
                        target->parent = curPoint;

                        target->G = calcG(curPoint, target);
                        target->H = calcH(target, &endPoint);
                        target->F = calcF(target);

                        openList.push_back(target);
                    }
                    //3，对某一个格子，它在开启列表中，计算G值, 如果比原来的大, 就什么都不做, 否则设置它的父节点为当前点,并更新G和F
                    else
                    {
                        int tempG = calcG(curPoint, target);
                        if (tempG < target->G)
                        {
                            target->parent = curPoint;

                            target->G = tempG;
                            target->F = calcF(target);
                        }
                    }
                    Point* resPoint = isInList(openList, &endPoint);
                    if (resPoint)
                        return resPoint; //返回列表里的节点指针，不要用原来传入的endpoint指针，因为发生了深拷贝
                }
            }

            return NULL;
        }

        std::list<Point*> Astar::GetPath(Point& startPoint, Point& endPoint, bool isIgnoreCorner)
        {
            Point* result = findPath(startPoint, endPoint, isIgnoreCorner);
            std::list<Point*> path;
            //返回路径，如果没找到路径，返回空链表
            while (result)
            {
                path.push_front(result);
                result = result->parent;
            }

            // 清空临时开闭列表，防止重复执行GetPath导致结果异常
            openList.clear();
            closeList.clear();

            return path;
        }

        Point* Astar::isInList(const std::list<Point*>& list, const Point* point) const
        {
            //判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标
            for (auto p : list)
                if (p->x == point->x && p->y == point->y)
                    return p;
            return NULL;
        }

        bool Astar::isCanreach(const Point* point, const Point* target, bool isIgnoreCorner) const
        {
            if ((target->x==api.GridToCell(api.GetSelfInfo()->x)&& target->y == api.GridToCell(api.GetSelfInfo()->y))
                ||target->x<0 || target->x>maze.size() - 1
                || target->y<0 || target->y>maze[0].size() - 1
                || IfAchievable(maze[target->x][target->y], ThisCondition) == 1
                || target->x == point->x && target->y == point->y
                || isInList(closeList, target)) //如果点与当前节点重合、超出地图、是障碍物、或者在关闭列表中，返回false
                return false;
            else
            {
                if (abs(point->x - target->x) + abs(point->y - target->y) == 1) //非斜角可以
                    return true;
                else
                {
                    //斜对角要判断是否绊住
                    if (IfAchievable(maze[point->x][target->y], ThisCondition) == 0 && IfAchievable(maze[target->x][point->y], ThisCondition) == 0)
                        return true;
                    else
                        return isIgnoreCorner;
                }
            }
        }

        std::vector<Point*> Astar::getSurroundPoints(const Point* point, bool isIgnoreCorner) const
        {
            std::vector<Point*> surroundPoints;

            for (int x = point->x - 1; x <= point->x + 1; x++)
                for (int y = point->y - 1; y <= point->y + 1; y++)
                    if (isCanreach(point, new Point(x, y), isIgnoreCorner))
                        surroundPoints.push_back(new Point(x, y));

            return surroundPoints;
        }


        bool InPath(const int& row, const int& col, const std::list<Point*>& path) {
            for (const auto& p : path) {
                if (row == p->x && col == p->y) {
                    return true;
                }
            }
            return false;
        }

        std::vector<std::vector<int32_t>> findPath(const std::vector<std::vector<THUAI6::PlaceType>>& map, int32_t cellX0, int32_t cellY0, int32_t cellX1, int32_t cellY1, std::vector<bool> Condition, IStudentAPI& api)
        {
            Astar astar(api);
            astar.InitAstar(map, Condition,api);
            Point start(cellX0, cellY0);
            Point end(cellX1, cellY1);
            std::list<Point*> path = astar.GetPath(start, end, false);
            std::vector<std::vector<int>> result{};
            for (auto P : path)
            {
                std::vector<int> Loc{ P->x,P->y };
                result.push_back(Loc);
            }
            return result;
        }

    }

    namespace GoToSomePlace
    {
        std::vector<bool> MoveToNextBlock(IStudentAPI& api, int32_t cellX, int32_t cellY)
        {
            auto self = api.GetSelfInfo();
            if (Distance(self->x, self->y, api.CellToGrid(cellX), api.CellToGrid(cellY)) < 50)//可以调整
                return { false,true };
            auto p = api.Move(50, Angle(self->x, self->y, api.CellToGrid(cellX), api.CellToGrid(cellY)));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            api.EndAllAction();
            if (!p.get())
                return { false,false };
            return { true,true };
        }//可以修改移动时间
        //不检查是否可达，谨慎使用

        class GoToAPlace
        {
        protected:
            bool Going = false;//是否正在走
            int32_t DestinationX = -1;//cell
            int32_t DestinationY = -1;
            std::vector<std::vector<int32_t>> Way{ {} };//没吃到
            int32_t StepNumber = 0;
            int32_t ThisStep = 0;
            IStudentAPI& api;
            std::vector<bool> ThisCondition;
            int32_t LastGridX=-1;
            int32_t LastGridY=-1;

        public:
            GoToAPlace(IStudentAPI& _api) :
                Going(false), DestinationX(-1), DestinationY(-1), LastGridX(-1), LastGridY(-1), api(_api), StepNumber(0), ThisStep(0), Way({}), ThisCondition{ {} } {}

            GoToAPlace& operator =(GoToAPlace P)
            {
                bool Going = false;//是否正在走
                DestinationX = -1;//cell
                DestinationY = -1;
                Way={ {} };//没吃到
                StepNumber = 0;
                ThisStep = 0;
                api=P.api;
                ThisCondition=P.ThisCondition;
                LastGridX = -1;
                LastGridY = -1;
                return (*this);
            }

            bool SetDestination(int32_t cellX, int32_t cellY, std::vector<bool> Condition)
            {
                if (api.GridToCell(api.GetSelfInfo()->x) == cellX && api.GridToCell(api.GetSelfInfo()->y) == cellY)
                    Way = { {} };
                else
                    Way = FindWay::findPath(api.GetFullMap(), api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y), cellX, cellY, Condition, api);
                if (Way.empty())
                {
                    Unset();
                    return false;
                }
                Going = true;
                DestinationX = cellX;
                DestinationY = cellY;
                StepNumber = Way.size();//size-1是最大可访问下表
                ThisStep = 0;//从0到size-1
                ThisCondition = Condition;
                LastGridX = -1;
                LastGridY = -1;
                return true;
            }

            void Unset()
            {
                Going = false;
                DestinationX = -1;
                DestinationY = -1;
                Way = { {} };
                StepNumber = 0;
                ThisStep = 0;
                LastGridX = -1;
                LastGridY = -1;
            }

            void Update()
            {
                if (!Going)
                    return;
                if (ThisStep >= Way.size() - 1)
                {
                    Unset();
                    return;
                }
                return;
            }

            bool MoveAStep()
            {
                Update();
                if (!Going||Way.empty())
                {
                    Unset();
                    return false;
                }
                else
                {
                    if ((LastGridX != -1 && Distance(LastGridX, LastGridY, api.GetSelfInfo()->x, api.GetSelfInfo()->y) < 20)
                        || abs(Way[ThisStep + 1][0] - api.GridToCell(api.GetSelfInfo()->x)) + abs(Way[ThisStep + 1][1] - api.GridToCell(api.GetSelfInfo()->y)) > 5)
                    {
                        SetDestination(DestinationX, DestinationY, Condition1);
                        return false;
                    }
                    auto p = MoveToNextBlock(api, Way[ThisStep + 1][0], Way[ThisStep + 1][1]);
                    if (!p[0])
                        ThisStep++;
                    LastGridX = api.GetSelfInfo()->x;
                    LastGridY = api.GetSelfInfo()->y;
                    Update();
                    return true;
                }
            }

            bool IfDone()
            {
                Update();
                return !Going;
            }
        };
    }

}

namespace Homework
{
    struct Homework
    {

        int32_t x = -1;
        int32_t y = -1;//cell
        int32_t Progress = 0;
        bool IfBeingDone = false;//待实现
        Homework() :x(-1), y(-1), Progress(0), IfBeingDone(false) {}
    };

    enum class Direction
    {
        Left = 0,
        Right = 1,
        Up = 2,
        Down = 3,
        LeftUp = 4,
        LeftDown = 5,
        RightUp = 6,
        RightDown = 7
    };

    bool DoHomework(IStudentAPI& api, Homework ThisHomework);

    class GoToNearestHomework :public MoveFunc::GoToSomePlace::GoToAPlace
    {
    private:
        Homework ThisHomework;
    public:
        GoToNearestHomework(IStudentAPI& _api) :
            GoToAPlace(_api), ThisHomework() {}

        GoToNearestHomework& operator=(GoToNearestHomework P)
        {
            bool Going = false;//是否正在走
            DestinationX = -1;//cell
            DestinationY = -1;
            Way = { {} };//没吃到
            StepNumber = 0;
            ThisStep = 0;
            api = P.api;
            ThisCondition = P.ThisCondition;
            LastGridX = -1;
            LastGridY = -1;
            ThisHomework = Homework();
            return (*this);
        }

        Homework FindTheNearestHomework()//考虑实现对于多人走向同一份作业的规避，目前未实现
        {
            Homework NearestHomework;
            int32_t DistanceOfTheNearestHomework = -1;
            int32_t NumberOfTheNearestHomework = -1;
            //int32_t i = 0;//遍历次数

            for (int32_t i = 0; i < 10; i++)
            {
                int32_t D = MoveFunc::FindWay::findPath
                (api.GetFullMap(), api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y), HomeworkLocations[i][0], HomeworkLocations[i][1], { false,true,false,true,true,false,false,false,true,true,true,false }, api).size();
                if (i == 0)
                {
                    DistanceOfTheNearestHomework = D;
                    NumberOfTheNearestHomework = i;
                }
                else if (D < DistanceOfTheNearestHomework  && api.GetClassroomProgress(HomeworkLocations[i][0], HomeworkLocations[i][1]) < 10000000)//待实现无人正在写
                {
                    DistanceOfTheNearestHomework = D;
                    NumberOfTheNearestHomework = i;
                }
            }
            if (NumberOfTheNearestHomework != -1)
            {
                NearestHomework.x = HomeworkLocations[NumberOfTheNearestHomework][0];
                NearestHomework.y = HomeworkLocations[NumberOfTheNearestHomework][1];
                NearestHomework.Progress = api.GetClassroomProgress(NearestHomework.x, NearestHomework.y);
            }
            return NearestHomework;
        }

        std::vector<bool> MoveAStepToNearestHomework(Direction ThisDirection)//第一个位置返回是否有最近作业，第二个位置返回是否成功
        {
            if (ThisHomework.x == -1)
                return { false,false };
            switch (ThisDirection)
            {
            case Direction::Left:
                SetDestination(ThisHomework.x, ThisHomework.y - 1, Condition1);
                break;
            case Direction::Right:
                SetDestination(ThisHomework.x, ThisHomework.y + 1, Condition1);
                break;
            case Direction::Up:
                SetDestination(ThisHomework.x - 1, ThisHomework.y, Condition1);
                break;
            case Direction::Down:
                SetDestination(ThisHomework.x + 1, ThisHomework.y, Condition1);
                break;
            case Direction::LeftUp:
                SetDestination(ThisHomework.x - 1, ThisHomework.y - 1, Condition1);
                break;
            case Direction::LeftDown:
                SetDestination(ThisHomework.x + 1, ThisHomework.y - 1, Condition1);
                break;
            case Direction::RightUp:
                SetDestination(ThisHomework.x - 1, ThisHomework.y + 1, Condition1);
                break;
            case Direction::RightDown:
                SetDestination(ThisHomework.x + 1, ThisHomework.y + 1, Condition1);
                break;
            }
            auto a = MoveAStep();
            return { true,  a };
        }

        void UnsetHomework()
        {
            ThisHomework = Homework();
        }

        std::vector<bool> FindAndDo(Direction ThisDirection = Direction::Right)//是否找到，是否赶到，是否完成
        {
            if (api.GetClassroomProgress(ThisHomework.x, ThisHomework.y) >= 10000000)
            {
                UnsetHomework();
                Unset();
            }
            if (ThisHomework.x == -1)
            {
                ThisHomework = FindTheNearestHomework();
                if (ThisHomework.x == -1)
                    return { false,false,false };
                switch (ThisDirection)
                {
                case Direction::Left:
                    SetDestination(ThisHomework.x, ThisHomework.y - 1, Condition1);
                    break;
                case Direction::Right:
                    SetDestination(ThisHomework.x, ThisHomework.y + 1, Condition1);
                    break;
                case Direction::Up:
                    SetDestination(ThisHomework.x - 1, ThisHomework.y, Condition1);
                    break;
                case Direction::Down:
                    SetDestination(ThisHomework.x + 1, ThisHomework.y, Condition1);
                    break;
                case Direction::LeftUp:
                    SetDestination(ThisHomework.x - 1, ThisHomework.y - 1, Condition1);
                    break;
                case Direction::LeftDown:
                    SetDestination(ThisHomework.x + 1, ThisHomework.y - 1, Condition1);
                    break;
                case Direction::RightUp:
                    SetDestination(ThisHomework.x - 1, ThisHomework.y + 1, Condition1);
                    break;
                case Direction::RightDown:
                    SetDestination(ThisHomework.x + 1, ThisHomework.y + 1, Condition1);
                    break;
                }
            }
            /*if (ThisHomework.x == -1)
            {
                ThisHomework = FindTheNearestHomework();
                SetDestination(ThisHomework.x, ThisHomework.y + 1);
            }*/
            //MoveAStep();
            if (!IfDone())
            {
                MoveAStep();
                return { true,false,false };
            }
            else
            {
                Unset();
                auto a = DoHomework(api, ThisHomework);
                return{ true,true,!a };
            }
        }
    };

    bool DoHomework(IStudentAPI& api, Homework ThisHomework)
    {
        auto self = api.GetSelfInfo();
        if (abs(api.GridToCell(self->x) - ThisHomework.x) > 1 || abs(api.GridToCell(self->y) - ThisHomework.y) > 1 || api.GetClassroomProgress(ThisHomework.x, ThisHomework.y) >= 10000000)
            return false;
        auto State = api.StartLearning();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        api.EndAllAction();
        return true;
    }
}


namespace Grass
{
    class MoveToGrass : public MoveFunc::GoToSomePlace::GoToAPlace
    {
    private:
        bool Escaping = false;
    public:
        MoveToGrass(IStudentAPI& _api) :
            GoToAPlace(_api), Escaping(false) {}

        MoveToGrass& operator=(MoveToGrass P)
        {
            bool Going = false;//是否正在走
            DestinationX = -1;//cell
            DestinationY = -1;
            Way = { {} };//没吃到
            StepNumber = 0;
            ThisStep = 0;
            api = P.api;
            ThisCondition = P.ThisCondition;
            LastGridX = -1;
            LastGridY = -1;
            Escaping = false;
            return (*this);
        }

        std::vector<int32_t> FindTheNearestGrass()
        {
            int32_t DistanceOfTheNearestGrass = -1;
            int32_t NumberOfTheNearestGrass = -1;
            //int32_t i = 0;//遍历次数

            for (int32_t i = 0; i < 13; i++)
            {
                int32_t D = MoveFunc::FindWay::findPath
                (api.GetFullMap(), api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y), GrassLocations[i][0], GrassLocations[i][1], Condition1, api).size();
                if (i == 0)
                {
                    DistanceOfTheNearestGrass = D;
                    NumberOfTheNearestGrass = i;
                }
                else if (D < DistanceOfTheNearestGrass)//待实现无人正在写
                {
                    DistanceOfTheNearestGrass = D;
                    NumberOfTheNearestGrass = i;
                }
            }
            return { GrassLocations[NumberOfTheNearestGrass][0],GrassLocations[NumberOfTheNearestGrass][1] };
        }

        void FindAndGo()
        {
            if (!Escaping)
            {
                auto p = FindTheNearestGrass();
                SetDestination(p[0], p[1], Condition1);
                Escaping = true;
            }
            else
                if (!IfDone())
                {
                    MoveAStep();
                    return;
                }
                else
                {
                    return;
                }
        }

        void StopEscape()
        {
            Escaping = false;
        }
    };
}


void AI::play(IStudentAPI& api)
{
    // 公共操作
    static std::vector<std::vector<int>> a{ {24,6},{15,6},{15,17},{24,17} };

    if (this->playerID == 0)//三教
    {
        static MoveFunc::GoToSomePlace::GoToAPlace Test(api);
        static Homework::GoToNearestHomework HomeworkTest(api);
        static Grass::MoveToGrass GrassTest(api);
        static int i = 0;
        if (api.GetTrickers().empty())
        {
            GrassTest.StopEscape();
            HomeworkTest.FindAndDo();
        }
        else
        {
            GrassTest.FindAndGo();
        }
        // 玩家0执行操作
    }
    else if (this->playerID == 1)//外边
    {
        static MoveFunc::GoToSomePlace::GoToAPlace Test(api);
        static Homework::GoToNearestHomework HomeworkTest(api);
        static Grass::MoveToGrass GrassTest(api);
        static int i = 0;
        if (api.GetTrickers().empty())
        {
            GrassTest.StopEscape();
            HomeworkTest.FindAndDo();
        }
        else
        {
            GrassTest.FindAndGo();
        }
        //玩家1执行操作
    }
    else if (this->playerID == 2)//外边
    {
        static MoveFunc::GoToSomePlace::GoToAPlace Test(api);
        static Homework::GoToNearestHomework HomeworkTest(api);
        static Grass::MoveToGrass GrassTest(api);
        static int i = 0;
        if (api.GetTrickers().empty())
        {
            GrassTest.StopEscape();
            HomeworkTest.FindAndDo();
        }
        else
        {
            GrassTest.FindAndGo();
        }
        // 玩家2执行操作
    }
    else if (this->playerID == 3)//六教
    {
        static MoveFunc::GoToSomePlace::GoToAPlace Test(api);
        static Homework::GoToNearestHomework HomeworkTest(api);
        static Grass::MoveToGrass GrassTest(api);
        static int i = 0;
        if (api.GetTrickers().empty())
        {
            GrassTest.StopEscape();
            HomeworkTest.FindAndDo();
        }
        else
        {
            GrassTest.FindAndGo();
        }
        // 玩家3执行操作
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
}
