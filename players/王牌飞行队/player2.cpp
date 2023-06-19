#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include"structures.h"
#include<math.h>

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::StraightAStudent};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数
bool move0 = false;//移动
bool move3 = false;
int homeworkx0, homeworky0;
int homeworkx1, homeworky1;
int homeworkx3, homeworky3;
bool scan_map0 = false;//扫图
bool scan_map3 = false;
bool find0 = false;//是否搜寻作业信息
bool find3 = false;
int code0 = 0;
int code3 = 0;
struct location
{
    int x;
    int y;
};
THUAI6::PlaceType place0[50][50];
location classroom0[10];
bool  workfinished0[10] = { false };
THUAI6::PlaceType place3[50][50];
location classroom3[10];
bool  workfinished3[10] = { false };
double distance(double a, double b, double c,double d)
{
    return sqrt(pow(a-c,2)+pow(b-d,2));
}
bool one_reach_low_x(IStudentAPI& api, int a, int b, int c, int d)//当x大于目标点
{
    auto mapinfo = api.GetFullMap();
    if (b < d - 1)
    {
        for (int i = a; i >= c - 1; i--)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j <= d - 1; j++)
            {
                if (mapinfo[c-1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d-1)
                    return true;
            }
        }
        for (int i = a; i >= c +1; i--)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j <= d-1; j++)
            {
                if (mapinfo[c+1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d- 1)
                    return true;
            }
        }
    }
    if (b > d + 1)
    {
        for (int i = a; i >= c + 1; i--)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j >= d+ 1; j--)
            {
                if (mapinfo[c+1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d+ 1)
                    return true;
            }
        }
        for (int i = a; i >= c - 1; i--)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j >= d+ 1; j--)
            {
                if (mapinfo[c-1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d + 1)
                    return true;
            }
        }
    }
    return false;
}
bool one_reach_high_x(IStudentAPI& api, int a, int b, int c, int d)
{
    auto mapinfo = api.GetFullMap();
    if (b < d - 1)
    {
        for (int i = a; i <= c - 1; i++)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j <= d - 1; j++)
            {
                if (mapinfo[c - 1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d - 1)
                    return true;
            }
        }
        for (int i = a; i <= c + 1; i++)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j <= d - 1; j++)
            {
                if (mapinfo[c + 1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d - 1)
                    return true;
            }
        }
    }
    if (b > d + 1)
    {
        for (int i = a; i <= c + 1; i++)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j >= d + 1; j--)
            {
                if (mapinfo[c + 1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d + 1)
                    return true;
            }
        }
        for (int i = a; i <= c - 1; i++)
        {
            if (mapinfo[i][b] == THUAI6::PlaceType::Wall)
                break;
            for (int j = b; j >= d + 1; j--)
            {
                if (mapinfo[c - 1][j] == THUAI6::PlaceType::Wall)
                    break;
                if (j == d + 1)
                    return true;
            }
        }
    }
    return false;
}
bool one_reach_high_y(IStudentAPI& api, int a, int b, int c, int d)//当小于目标点
{
    auto mapinfo = api.GetFullMap();
    if (b < d - 1)
    {
        for (int i = b; i <= d - 1; i++)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j <= c - 1; j++)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c - 1)
                    return true;
            }
        }
        for (int i = b; i <= d + 1; i++)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j <= c - 1; j++)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c - 1)
                    return true;
            }
        }
    }
    if (b > d + 1)
    {
        for (int i = b; i >= d + 1; i--)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j <= c - 1; j++)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c - 1)
                    return true;
            }
        }
        for (int i = b; i >= d - 1; i--)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j <= c - 1; j++)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c -1)
                    return true;
            }
        }
    }
    return false;
}
bool one_reach_low_y(IStudentAPI& api, int a, int b, int c, int d)
{
    auto mapinfo = api.GetFullMap();
    if (b < d - 1)
    {
        for (int i = b; i <= d - 1; i++)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j >= c + 1; j--)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c + 1)
                    return true;
            }
        }
        for (int i = b; i <= d + 1; i++)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j >= c + 1; j--)
            {
                if (mapinfo[j][d - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c + 1)
                    return true;
            }
        }
    }
    if (b > d + 1)
    {
        for (int i = b; i >= d + 1; i--)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j >= c + 1; j--)
            {
                if (mapinfo[j][b - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c + 1)
                    return true;
            }
        }
        for (int i = b; i >= d - 1; i--)
        {
            if (mapinfo[a][i] == THUAI6::PlaceType::Wall)
                break;
            for (int j = a; j >= c + 1; j--)
            {
                if (mapinfo[j][b - 1] == THUAI6::PlaceType::Wall)
                    break;
                if (j == c + 1)
                    return true;
            }
        }
    }
    return false;
}
bool xfirst = false;
bool xfirst1 = false;
void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        if (!scan_map0)
        {
            int k = 0;
            auto mapinfo = api.GetFullMap();
            for (int i = 0; i < 50; i++)
                for (int j = 0; j < 50; j++)
                {
                    place0[i][j] = mapinfo[i][j];
                    if (place0[i][j] == THUAI6::PlaceType::ClassRoom)
                    {
                        classroom0[k].x = i;
                        classroom0[k].y = j;
                        k++;
                    }
                }
            scan_map0 = true;
        }
        auto info = api.GetSelfInfo();
        auto currentx = api.GridToCell(info->x);
        auto currenty = api.GridToCell(info->y);
        if (find0 == false)
        {
            auto min = 20000;
            for (int k = 0; k < 7; k++)//寻找作业的函数
            {
                if (one_reach_low_y(api, currentx, currenty, classroom0[k].x, classroom0[k].y) || one_reach_high_y(api, currentx, currenty, classroom0[k].x, classroom0[k].y)|| one_reach_low_x(api, currentx, currenty, classroom0[k].x, classroom0[k].y)|| one_reach_high_x(api, currentx, currenty, classroom0[k].x, classroom0[k].y))
                {
                    if (distance(currentx, currenty, classroom0[k].x, classroom0[k].y) <= min && workfinished0[k] == false)
                    {
                        homeworkx0 = classroom0[k].x;
                        homeworky0 = classroom0[k].y;
                        min = distance(currentx, currenty, classroom0[k].x, classroom0[k].y);
                        code0 = k;
                        find0 = true;
                    }
                }
            }
            if (one_reach_low_x(api, currentx, currenty, classroom0[code0].x, classroom0[code0].y) || one_reach_high_x(api, currentx, currenty, classroom0[code0].x, classroom0[code0].y))
                xfirst = true;
            else
                xfirst = false;
        }
         if (distance(currentx,currenty,homeworkx0,homeworky0)<=sqrt(2)+0.1 && workfinished0[code0] == false)//这个句子是报错根源
          {
                 api.UseSkill(0);
                api.StartLearning();
                move0 = true;
                if (api.GetClassroomProgress(homeworkx0, homeworky0) == 10000000)
                {
                    workfinished0[code0] = true;
                    move0 = false;
                    find0 = false;
                }
          }
         if (move0 == false)
         {
             if (xfirst == false)
             {
                 if (homeworky0 - currenty <= -1)
                 {
                     api.MoveLeft(20);
                     if (api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::Wall)
                         api.MoveUp(20);
                 }
                 else if (homeworky0 - currenty >= 1)
                 {
                     if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                         api.MoveUp(20);
                     api.MoveRight(20);
                 }
                 else if (homeworky0 - currenty >= -1 && homeworky0 - currenty <= 1)
                 {
                     if (homeworkx0 - currentx >= 1)
                     {
                         api.MoveDown(20);
                         if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                             api.MoveRight(20);
                     }
                     else if (homeworkx0 - currentx <= -1)
                     {
                         api.MoveUp(20);
                         if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                             api.MoveRight(20);
                     }
                 }
             }
             else if (xfirst == true)
             {
                 if (homeworkx0 - currentx <= -1)
                 {
                     if (api.GetPlaceType(currentx+1, currenty ) == THUAI6::PlaceType::Wall)
                         api.MoveRight(20);
                     api.MoveUp(20);
                 }
                 else if (homeworkx0 - currentx >= 1)
                 {
                     if (api.GetPlaceType(currentx-1, currenty ) == THUAI6::PlaceType::Wall)
                         api.MoveRight(20);
                     api.MoveDown(20);
                 }
                 else
                 {
                     if (homeworky0 - currenty >= 1)
                     {
                         if (api.GetPlaceType(currentx, currenty +1) == THUAI6::PlaceType::Wall)
                             api.MoveUp(20);
                         api.MoveRight(20);
                     }
                     else if (homeworky0 - currenty <= -1)
                     {
                         if (api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::Wall)
                             api.MoveUp(20);
                         api.MoveLeft(20);
                     }
                 }
             }
         }
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
        auto info = api.GetSelfInfo();
        auto currentx = info->x / 1000.0;
        auto currenty = info->y / 1000.0;
        /*  if (api.GetPlaceType(currentx - 1, currenty + 1) != THUAI6::PlaceType::Wall)
              api.MoveUp(50);
          if (api.GetPlaceType(currentx - 1, currenty + 1) == THUAI6::PlaceType::Wall)
              api.MoveLeft(50);*/
        auto info2 = api.GetFullMap();
        /*   for(int i=0;i<10;i++)
               for(int j=0;j<10;j++)*/
               /* api.Print(fmt::format("x=i,y=j,placetype:{}", THUAI6::placeTypeDict[info2[i][j]]));*/
        auto x1 = 0;
        auto y1 = 0;
        auto min = 20000;

        for (int i = 0; i < 50; i++)
            for (int j = 0; j < 50; j++)
            {
                if (info2[i][j] == THUAI6::PlaceType::ClassRoom)
                {
                    if (distance(i, j, x1, y1) <= min)
                    {
                        homeworkx1 = i;
                        homeworky1 = j;
                        min = distance(i, j, x1, y1);
                    }
                }
            }
        if (api.GetPlaceType(currentx - 1, currenty) == THUAI6::PlaceType::ClassRoom || api.GetPlaceType(currentx + 1, currenty) == THUAI6::PlaceType::ClassRoom || api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::ClassRoom || api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::ClassRoom)
        {
            api.StartLearning();
            move0 = true;
        }
        if (move0 == false)
        {
            if (homeworky1 < currenty)
            {
                api.MoveLeft(10);
                if (api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::Wall)
                    api.MoveUp(10);
            }
            else if (homeworky1> currenty)
            {
                if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                    api.MoveUp(10);
                api.MoveRight(10);
            }
            else if (homeworky1 == currenty)
            {
                if (homeworkx1 > currentx)
                {
                    api.MoveDown(10);
                    if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                        api.MoveRight(10);
                }
                else if (homeworkx1 < currentx)
                {
                    api.MoveUp(10);
                    if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                        api.MoveRight(10);
                }
            }
        }
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        auto a = api.GetTrickers();
        auto info = api.GetSelfInfo();
        auto currentx = api.GridToCell(info->x);
        auto currenty = api.GridToCell(info->y);
        if (a.size()==0 || (a.size()!=0 && (abs(api.GridToCell(a[0]->x) - currentx) >= 5 || abs(api.GridToCell(a[0]->y) - currenty) >= 5))) {
            api.MoveDown(25);
        }
        else {
            ;
            /*api.MoveDown(0);*/
        }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
     }
    else if (this->playerID == 3)
    {
    if (!scan_map3)
    {
        int k = 0;
        auto mapinfo = api.GetFullMap();
        for (int i = 0; i < 50; i++)
            for (int j = 0; j < 50; j++)
            {
                place3[i][j] = mapinfo[i][j];
                if (place3[i][j] == THUAI6::PlaceType::ClassRoom)
                {
                    classroom3[k].x = i;
                    classroom3[k].y = j;
                    k++;
                }
            }
        scan_map3 = true;
    }
    auto info = api.GetSelfInfo();
    auto currentx = api.GridToCell(info->x);
    auto currenty = api.GridToCell(info->y);
    if (find3 == false)
    {
        auto min = 20000;
        for (int k = 0; k < 7; k++)//寻找作业的函数
        {
            if (one_reach_low_y(api, currentx, currenty, classroom3[k].x, classroom3[k].y) || one_reach_high_y(api, currentx, currenty, classroom3[k].x, classroom3[k].y) || one_reach_low_x(api, currentx, currenty, classroom3[k].x, classroom3[k].y) || one_reach_high_x(api, currentx, currenty, classroom3[k].x, classroom3[k].y))
            {
                if (distance(currentx, currenty, classroom3[k].x, classroom3[k].y) <= min && workfinished3[k] == false)
                {
                    homeworkx3 = classroom3[k].x;
                    homeworky3 = classroom3[k].y;
                    min = distance(currentx, currenty, classroom3[k].x, classroom3[k].y);
                    code3 = k;
                    find3= true;
                }
            }
        }
        if (one_reach_low_x(api, currentx, currenty, classroom3[code3].x, classroom3[code3].y) || one_reach_high_x(api, currentx, currenty, classroom3[code3].x, classroom3[code3].y))
            xfirst1 = true;
        else
            xfirst1 = false;
    }
    if (distance(currentx, currenty, homeworkx3, homeworky3) <= sqrt(2) + 0.1 && workfinished3[code3] == false)//这个句子是报错根源
    {
        api.UseSkill(0);
        api.StartLearning();
        move3 = true;
        if (api.GetClassroomProgress(homeworkx3, homeworky3) == 10000000)
        {
            workfinished3[code3] = true;
            move3 = false;
            find3 = false;
        }
    }
    if (move3 == false)
    {
        if (xfirst1 == false)
        {
            if (homeworky3 - currenty <= -1)
            {
                api.MoveLeft(20);
                if (api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::Wall)
                    api.MoveUp(20);
            }
            else if (homeworky3 - currenty >= 1)
            {
                if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                    api.MoveUp(20);
                api.MoveRight(20);
            }
            else if (homeworky3 - currenty >= -1 && homeworky3 - currenty <= 1)
            {
                if (homeworkx3 - currentx >= 1)
                {
                    api.MoveDown(20);
                    if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                        api.MoveRight(20);
                }
                else if (homeworkx3 - currentx <= -1)
                {
                    api.MoveUp(20);
                    if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                        api.MoveRight(20);
                }
            }
        }
        else if (xfirst1 == true)
        {
            if (homeworkx3 - currentx <= -1)
            {
                if (api.GetPlaceType(currentx + 1, currenty) == THUAI6::PlaceType::Wall)
                    api.MoveRight(20);
                api.MoveUp(20);
            }
            else if (homeworkx3 - currentx >= 1)
            {
                if (api.GetPlaceType(currentx - 1, currenty) == THUAI6::PlaceType::Wall)
                    api.MoveRight(20);
                api.MoveDown(20);
            }
            else
            {
                if (homeworky3 - currenty >= 1)
                {
                    if (api.GetPlaceType(currentx, currenty + 1) == THUAI6::PlaceType::Wall)
                        api.MoveUp(20);
                    api.MoveRight(20);
                }
                else if (homeworky3 - currenty <= -1)
                {
                    if (api.GetPlaceType(currentx, currenty - 1) == THUAI6::PlaceType::Wall)
                        api.MoveUp(20);
                    api.MoveLeft(20);
                }
            }
        }
    }
 }
}

void AI::play(ITrickerAPI& api)
{
    double distance(double x, double y, double x0, double y0);
    double maxdis(double a, double b, double c, double d);
    double mindis(double a, double b, double c, double d);
    int maxdisorder(double a, double b, double c, double d);

    int flag[4] = { 0 };//用于判断是否有学生数据返回，1表示有

    auto self = api.GetSelfInfo();
    auto selfx = self->x;
    auto selfy = self->y;

    double dis[4] = { 10000 };
    double the[4] = { 0 };

    auto stu = api.GetStudents();

    if (stu.size()!=0)
    {
        flag[0] = 1;
        auto x0 = stu[0]->x;
        auto y0 = stu[0]->y;
        dis[0] = (x0, y0, selfx, selfy);//与学生0的距离
        the[0] = atan((x0 - selfx) / ((double)y0 - selfy));//与学生0的夹角
    }
    /*if (!stu.empty() && stu[1]->x<50000 && stu[1]->x>0);
    {
        flag[1] = 1;
        auto x1 = stu[1]->x;
        api.Print(fmt::format("ptr {}", x1));
        auto y1 = stu[1]->y;
        dis[1] = (x1, y1, selfx, selfy);//与学生1的距离
        the[1] = atan((x1 - selfx) / ((double)y1 - selfy));//与学生1的夹角
    }
    /*if (!stu.empty())
    {
        flag[2] = 1;
        auto x2 = stu[2]->x;
        auto y2 = stu[2]->y;
        dis[2] = (x2, y2, selfx, selfy);//与学生2的距离
        the[2] = atan((x2 - selfx) / ((double)y2 - selfy));//与学生2的夹角
    }
    if (!stu.empty())
    {
        flag[3] = 1;
        auto x3 = stu[3]->x;
        auto y3 = stu[3]->y;
        dis[3] = (x3, y3, selfx, selfy);//与学生3的距离
        the[3] = atan((x3 - selfx) / ((double)y3 - selfy));//与学生3的夹角
    }*/

    auto map = api.GetFullMap();



    double dismax = 0, dismin = 0;
    dismax = maxdis(dis[1], dis[2], dis[3], dis[0]);
    dismin = mindis(dis[0], dis[1], dis[2], dis[3]);
    int minnum = -1;

    /*for (int i = 0; i <= 3; i++)
    {
        if (flag[i] == 1&&((dismin - dis[i]) > -0.2 && (dismin - dis[i]) < 0.2))
        {
            minnum = i;
            break;
        }
    }*/

    if (distance(stu[0]->x, stu[0]->y, selfx, selfy) > 2000)
    {
        if (selfx < stu[0]->x)
        {
            api.MoveDown(20);
        }
        else if (selfx > stu[0]->x)
        {
            api.MoveUp(20);
        }
        if (selfy < stu[0]->y)
        {
            api.MoveRight(20);
        }
        else
        {
            api.MoveLeft(20);
        }
    }
    else
    {
        api.Attack(the[0]);
    }
    /*else {
        api.Attack(the[0]);
    }*/

    //api.UseSkill(1);
    /*if (dismin > 2000)
    {
        api.Move(40, the[minnum]);
    }
    else
    {
        api.Attack(the[minnum]);
    }*/
    api.Print(fmt::format("Vec {} {}", stu[0]->x, stu[0]->y));
    //api.PrintStudent();

    //api.PrintSelfInfo();
}


double maxdis(double a, double b, double c, double d)
{
    double arr[4] = { 0 };
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    arr[3] = d;
    double temp = 0;
    int i = 0, j = 0;
    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 2 - i; j++)
        {
            if (arr[j] <= arr[j + 1])
            {
                temp = arr[j + 1];
                arr[j + 1] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return arr[0];
}

double mindis(double a, double b, double c, double d)
{
    double arr[4] = { 0 };
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    arr[3] = d;
    double temp = 0;
    int i = 0, j = 0;
    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 2 - i; j++)
        {
            if (arr[j] <= arr[j + 1])
            {
                temp = arr[j + 1];
                arr[j + 1] = arr[j];
                arr[j] = temp;
            }
        }
    }
    return arr[3];
}
