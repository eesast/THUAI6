#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;
static int flag = 0;
static int flagg = 0;
static int flaggg = 0;
static int flagggg = 0;
double angle = 0;
static int homework = 0;
int homework_19_6 = 0;
int homework_31_8 = 0;
int homework_23_19 = 0;
int homework_9_30 = 0;
int homework_11_39 = 0;
int homework_20_42 = 0;
int homework_29_27 = 0;
int homework_34_41 = 0;
int homework_41_13 = 0;
int homework_46_33 = 0;
// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

int ArrayLen(int* a)
{
    int i = 0;
    while (a[i] != 0)
        i++;
    return i;
}

int* UpLeft(int dx, int dy, int x, int y, IStudentAPI& api)       //会返回一串形如101000111的字符(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
{                                                                   //从左往右每个数字代表一步，0代表向上一步，1代表向左一步//
    int a[200] = { 0 }, *c, b[1];
    b[0] = 3;
    int flag = 0, i = 0;
    if (dx == 0)
    {
        for (int i = 1; i < dy; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a[++i]=2;
        }
        if (flag == 1)  return b;
        else return a;
    }
    flag = 0;
    if (dy == 0)
    {
        for (int i = 0; i < dx; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a[++i]=1;
        }
        if (flag == 1)  return b;
        else   return a;
    }
    if (*(UpLeft(dx, dy - 1, x, y - 1000, api)) != 3 && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Window)
    {
        c = UpLeft(dx, dy - 1, x, y - 1000, api);
        for (int i = 0; i < ArrayLen(c); i++)
            a[i] = c[i];
        a[ArrayLen(c)] = 2;
        return a;
    }
    if (*(UpLeft(dx - 1, dy, x - 1000, y, api)) != 3 && api.GetPlaceType(api.GridToCell(x) - 1, api.GridToCell(y)) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x) - 1, api.GridToCell(y)) != THUAI6::PlaceType::Window)
    {
        c = UpLeft(dx - 1, dy, x - 1000, y, api);
        for (int i = 0; i < ArrayLen(c); i++)
            a[i] = c[i];
        a[ArrayLen(c)] = 1;
        return a;
    }
    else return b;
}

/*
long long UpRight(int dx, int dy, int x, int y, IStudentAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
{                                                                   //从右往左每个数字代表一步，0代表向上一步，1代表向右一步//
    int a = 0, flag = 0;
    if (dx == 0)
    {
        for (int i = 1; i < dy; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + i) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + i) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 1;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    flag = 0;
    if (dy == 0)
    {
        for (int i = 0; i < dx; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 0;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    if (UpLeft(dx, dy - 1, x, y + 1000, api) != 2 && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + 1) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + 1) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx, dy - 1, x, y + 1000, api) + 1;
    }
    if (UpLeft(dx - 1, dy, x - 1000, y, api) != 2 && api.GetPlaceType(api.GridToCell(x) - 1, api.GridToCell(y)) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x) - 1, api.GridToCell(y)) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx - 1, dy, x - 1000, y, api);
    }
    else return 2;
}

long long DownLeft(int dx, int dy, int x, int y, IStudentAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
{                                                                   //从右往左每个数字代表一步，0代表向下一步，1代表向左一步//
    int a = 0, flag = 0;
    if (dx == 0)
    {
        for (int i = 1; i < dy; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 1;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    flag = 0;
    if (dy == 0)
    {
        for (int i = 0; i < dx; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x) + i, api.GridToCell(y)) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x) + i, api.GridToCell(y)) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 0;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    if (UpLeft(dx, dy - 1, x, y - 1000, api) != 2 && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx, dy - 1, x, y - 1000, api) + 1;
    }
    if (UpLeft(dx - 1, dy, x + 1000, y, api) != 2 && api.GetPlaceType(api.GridToCell(x) + 1, api.GridToCell(y)) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x) + 1, api.GridToCell(y)) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx - 1, dy, x + 1000, y, api);
    }
    else return 2;
}

long long DownRight(int dx, int dy, int x, int y, IStudentAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
{                                                                   //从右往左每个数字代表一步，0代表向下一步，1代表向右一步//
    int a = 0, flag = 0;
    if (dx == 0)
    {
        for (int i = 1; i < dy; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + i) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + i) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 1;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    flag = 0;
    if (dy == 0)
    {
        for (int i = 0; i < dx; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x) + i, api.GridToCell(y)) == THUAI6::PlaceType::Wall || api.GetPlaceType(api.GridToCell(x) + i, api.GridToCell(y)) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 0;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    if (UpLeft(dx, dy - 1, x, y + 1000, api) != 2 && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + 1) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) + 1) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx, dy - 1, x, y + 1000, api) + 1;
    }
    if (UpLeft(dx - 1, dy, x + 1000, y, api) != 2 && api.GetPlaceType(api.GridToCell(x) + 1, api.GridToCell(y)) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x) + 1, api.GridToCell(y)) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx - 1, dy, x + 1000, y, api);
    }
    else return 2;
}

//可以在AI.cpp内部声明变量与函数
*/

void MoveTo(int des_x, int des_y, IStudentAPI& api, double speed, int a)//走到cell坐标为（des_x，des_y)的函数,speed是api每秒移动的速度
{
    if (a == 0)
    {
        auto p = api.GetSelfInfo();
        int x_cell = api.GridToCell(p->x);
        int y_cell = api.GridToCell(p->y);
        int* step;//用于记录走到目的地所需要的步骤
        double time_ms = (1000.0 / (speed)) * 1000;
        int len = abs((x_cell - des_x)/2) + abs((y_cell - des_y)/2) - 2;
        if (x_cell > des_x && y_cell > des_y)
        {
           
            step = UpLeft(abs((x_cell - des_x) / 2), abs((y_cell - des_y) / 2), p->x, p->y, api);
            //std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms*20)));
           while(len>=0)
           {
                if (step[len] == 1)
                {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                    api.MoveUp(time_ms);
                    
                }
                if (step[len] == 2)
                {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                    api.MoveLeft(time_ms);
                  
                }
                len--;
            }

        }
        //MoveTo(des_x, des_y, api, 2880,1);
    }
    else 
    {
        auto p = api.GetSelfInfo();
        int x_cell = api.GridToCell(p->x);
        int y_cell = api.GridToCell(p->y);
        int* step;//用于记录走到目的地所需要的步骤
        double time_ms = (1000.0 / (speed)) * 1000;
        int len = abs(x_cell - des_x) + abs(y_cell - des_y) - 2;
        step = UpLeft(abs(x_cell - des_x), abs(y_cell - des_y), p->x, p->y, api);
        if (x_cell > des_x && y_cell > des_y)
        {
            while (len >= 0)
            {
                if (step[len] == 1)
                {
                    api.MoveUp(time_ms);
                    std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                }
                if (step[len] == 2)
                {
                    api.MoveLeft(time_ms);
                    std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                }
                len--;
            }
        }

    }
    //else if(x_cell > des_x && y_cell < des_y) step = UpRight(abs(x_cell - des_x), abs(y_cell - des_y), x_cell, y_cell,api);
    //else if (x_cell < des_x && y_cell > des_y) step = DownLeft(abs(x_cell - des_x), abs(y_cell - des_y), x_cell, y_cell, api);
    //else if (x_cell< des_x && y_cell < des_y) step = DownRight(abs(x_cell - des_x), abs(y_cell - des_y), x_cell, y_cell, api);
}
void DoHomework(int a, int b, IStudentAPI& api)//写作业函数，这部分可能会有问题         @@@（需要测试）
{
    auto p1 = api.GetSelfInfo();
    int x1 = api.GridToCell(p1->x);
    int y1 = api.GridToCell(p1->y);
    auto judge1 = api.GetPlaceType(x1 - 1, y1);
    auto judge2 = api.GetPlaceType(x1 + 1, y1);
    auto judge3 = api.GetPlaceType(x1, y1 - 1);
    auto judge4 = api.GetPlaceType(x1, y1 + 1);
    if (((judge1 == THUAI6::PlaceType::ClassRoom) || (judge2 == THUAI6::PlaceType::ClassRoom) || (judge3 == THUAI6::PlaceType::ClassRoom) || (judge4 == THUAI6::PlaceType::ClassRoom)) && (api.GetClassroomProgress(a, b) < 10000000))
    {
        api.StartLearning();
    }
    if (((judge1 == THUAI6::PlaceType::ClassRoom) || (judge2 == THUAI6::PlaceType::ClassRoom) || (judge3 == THUAI6::PlaceType::ClassRoom) || (judge4 == THUAI6::PlaceType::ClassRoom)) && (api.GetClassroomProgress(a, b) >= 10000000))
    {
        api.EndAllAction();
        api.Wait();
    }
}
void Opendoor(int a, int b, IStudentAPI& api)//开大门函数，这部分可能会有问题         @@@（需要测试）
{
    auto p1 = api.GetSelfInfo();
    int x1 = api.GridToCell(p1->x);
    int y1 = api.GridToCell(p1->y);
    auto judge1 = api.GetPlaceType(x1 - 1, y1);
    auto judge2 = api.GetPlaceType(x1 + 1, y1);
    auto judge3 = api.GetPlaceType(x1, y1 - 1);
    auto judge4 = api.GetPlaceType(x1, y1 + 1);
    if (((judge1 == THUAI6::PlaceType::Gate) || (judge2 == THUAI6::PlaceType::Gate) || (judge3 == THUAI6::PlaceType::Gate) || (judge4 == THUAI6::PlaceType::Gate)) && (api.GetGateProgress(a, b) < 18000))
    {
        api.StartOpenGate();
    }
    if (((judge1 == THUAI6::PlaceType::ClassRoom) || (judge2 == THUAI6::PlaceType::ClassRoom) || (judge3 == THUAI6::PlaceType::ClassRoom) || (judge4 == THUAI6::PlaceType::ClassRoom)) && (api.GetGateProgress(a, b) >= 18000))
    {
        api.EndAllAction();
        api.Wait();
        api.Graduate();
    }
}
void GetToTheNearestHomework(IStudentAPI& api)//找到离自己最近的未完成作业并且走到作业
{
    auto p1 = api.GetSelfInfo();
    int x1 = api.GridToCell(p1->x);
    int y1 = api.GridToCell(p1->y);
    int distance[10][3];
    distance[0][0] = { (1000 * homework_19_6 + abs(x1 - 19) + abs(y1 - 6)) }; distance[0][1] = 19; distance[0][2] = 6;
    distance[1][0] = { 1000 * homework_31_8 + abs(x1 - 31) + abs(y1 - 8) }; distance[1][1] = 31; distance[1][2] = 8;
    distance[2][0] = { 1000 * homework_23_19 + abs(x1 - 23) + abs(y1 - 19) }; distance[2][1] = 23; distance[2][2] = 19;
    distance[3][0] = { 1000 * homework_9_30 + abs(x1 - 9) + abs(y1 - 30) }; distance[3][1] = 9; distance[3][2] = 30;
    distance[4][0] = { 1000 * homework_11_39 + abs(x1 - 11) + abs(y1 - 39) }; distance[4][1] = 11; distance[4][2] = 39;
    distance[5][0] = { 1000 * homework_20_42 + abs(x1 - 20) + abs(y1 - 42) }; distance[5][1] = 20; distance[5][2] = 42;
    distance[6][0] = { 1000 * homework_29_27 + abs(x1 - 29) + abs(y1 - 27) }; distance[6][1] = 29; distance[6][2] = 27;
    distance[7][0] = { 1000 * homework_34_41 + abs(x1 - 34) + abs(y1 - 41) }; distance[7][1] = 34; distance[7][2] = 41;
    distance[8][0] = { 1000 * homework_41_13 + abs(x1 - 41) + abs(y1 - 13) }; distance[8][1] = 41; distance[8][2] = 13;
    distance[9][0] = { 1000 * homework_46_33 + abs(x1 - 46) + abs(y1 - 33) }; distance[9][1] = 46; distance[9][2] = 33;
    //这边需要个冒泡排序求出distance[i][0]的最大值         @@@
    //移动到(distance[i][1],distance[i][2])         @@@
}
void RunAway(IStudentAPI& api)//逃离监管者函数
{
    auto self = api.GetSelfInfo();
    int x1 = api.GridToCell(self->x); int y1 = api.GridToCell(self->y);
    auto Trick = api.GetTrickers();
    int x2 = 0, y2 = 0;
    if (!Trick.empty())
    {
        x2 = Trick[0]->x; y2 = Trick[0]->y;
    }
    else
    {
        x2 = x1; y2 = y1;
    }
    //这边写一个反向逃离Tricker的Move函数进去，前面x1,y1是自己的位置，x2,y2是Tricker的位置，但是极坐标这里我不太熟        @@@

}
void PlayerCarryOut(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    int x = api.GridToCell(self->x); int y = api.GridToCell(self->y);
    if (((x <= 17) && (y <= 5)) || ((x <= 10) && (y >= 5) && (y <= 12)))       //一号区，去（6，7）看门的情况
    {
        auto gameinfo = api.GetGameInfo();
        if (gameinfo->subjectFinished < 7)//门未达到开启条件
        {
            GetToTheNearestHomework(api);
        }
        else//门达到了开启条件
        {
            if (api.GetGateProgress(6, 7) < 18000)
            {
                //写一个走到（6，7）的函数         @@@
                Opendoor(6, 7, api);
            }
            else
            {
                //写一个走到（6，7）的函数         @@@
                api.EndAllAction();
                api.Graduate();
            }
        }
    }
    if ((x <= 6) && (y > 13) && (y <= 26))   //二号区，去开箱，此处还没完成捡道具；
    {
        //走到（5，16）         @@@
        if (api.GetChestProgress(5, 16) < 10000000)
        {
            api.StartOpenChest();
        }
        else
        {
            GetToTheNearestHomework(api);
        }

    }
    if ((x > 6) && (x <= 12) && (y > 13) && (y <= 26))//三号区，走到（12，21）翻入3教
    {
        //走到（12，21）         @@@
        if (api.GetPlaceType(x + 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if ((x <= 6) && (y >= 27) && (y <= 41))//四号区，翻墙进入五教
    {
        //走到(5,33)         @@@
        if (api.GetPlaceType(x + 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if (((x <= 6) && (y >= 42) && (y <= 50)) || ((x >= 7) && (x <= 9) && (y >= 45) && (y <= 50)))//五号区，开箱，问题同二号区
    {
        //走到（5，46）         @@@
        if (api.GetChestProgress(5, 46) < 10000000)
        {
            api.StartOpenChest();
        }
        else
        {
            GetToTheNearestHomework(api);
        }

    }
    if (((x >= 13) && (x <= 14) && (y >= 24) && (y <= 35)) || ((x >= 15) && (x <= 19) && (y >= 24) && (y <= 31)) || ((x >= 20) && (x <= 21) && (y >= 21) && (y <= 28)))
        //六号区，翻入五教
    {
        //走到（18，30）         @@@
        if (api.GetPlaceType(x, y + 1) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if (((x >= 22) && (x <= 23) && (y >= 22) && (y <= 29)) || ((x >= 24) && (x <= 27) && (y >= 22) && (y <= 40)))
        //七号区，翻到六教
    {
        //走到（26,35）         @@@
        if (api.GetPlaceType(x + 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if (((x >= 10) && (x <= 21) && (y >= 47) && (y <= 50)) || ((x >= 22) && (x <= 29) && (y >= 41) && (y <= 50)) || ((x >= 30) && (x <= 38) && (y >= 45) && (y <= 50)))
    {
        //八号区，走到（29，40）附近
        if (api.IsDoorOpen(29, 40) == true)
        {
            //走到（29，38）         @@@
        }
        else
        {
            //走到（39，43）         @@@
            if (api.GetPlaceType(x - 1, y) == THUAI6::PlaceType::Window)
            {
                api.SkipWindow();
            }
        }
    }
    if (((x > 39) && (x <= 43) && (y >= 33) && (y <= 39)) || ((x >= 39) && (x <= 50) && (y >= 40) && (y <= 50)))//第九区，看门（47，46）
    {
        auto gameinfo = api.GetGameInfo();
        if (gameinfo->subjectFinished < 7)//门未达到开启条件
        {
            GetToTheNearestHomework(api);
        }
        else//门达到了开启条件
        {
            if (api.GetGateProgress(47, 46) < 18000)//此处对于GetGateProgress的使用存疑
            {
                //写一个走到（47，46）的函数         @@@
                Opendoor(47, 46, api);
            }
            else
            {
                //写一个走到（47，46）的函数         @@@
                api.EndAllAction();
                api.Graduate();
            }
        }
    }
    if (((x >= 28) && (x <= 31) && (y >= 11) && (y <= 21)) || ((x >= 32) && (x <= 33) && (y >= 11) && (y <= 28)) || ((x >= 34) && (x <= 38) && (y >= 9) && (y <= 17)))
        //第十区，走到（27，11）翻墙进入三教
    {
        //走到（28，11）         @@@
        if (api.GetPlaceType(x - 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if ((x >= 33) && (x <= 39) && (y >= 0) && (y <= 8))//第十一区，走到（38，3）开箱
    {
        //走到（38，3）附近         @@@
        if (api.GetChestProgress(38, 3) < 10000000)
        {
            api.StartOpenChest();
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 40) && (x <= 43) && (y >= 0) && (y <= 8)) || ((x >= 44) && (x <= 50) && (y >= 0) && (y <= 18)))
        //第十二区，翻（42，18）进入三教
    {
        //走到（43，18）         @@@
        if (api.GetPlaceType(x - 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if ((x >= 48) && (x <= 50) && (y >= 21) && (y <= 34))//第十三区，翻（47，27）进入三教
    {
        //走到（48，27）         @@@
        if (api.GetPlaceType(x - 1, y) == THUAI6::PlaceType::Window)
        {
            api.SkipWindow();
        }
    }
    if (((x >= 12) && (x <= 17) && (y >= 5) && (y <= 12)) || ((x >= 18) && (x <= 21) && (y >= 0) && (y <= 12)))
        //作业一区，到（19，6）做作业
    {
        //走到（19，6）         @@@
        if (homework_19_6 == 0)
        {
            DoHomework(19, 6, api);
            if (api.GetClassroomProgress(19, 6) >= 10000000)
            {
                homework_19_6 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 22) && (x <= 32) && (y >= 4) && (y <= 10)) || ((x >= 22) && (x <= 27) && (y >= 10) && (y <= 12)))
        //作业二区，到（31，8）做作业
    {
        //走到（31,8）         @@@
        if (homework_31_8 == 0)
        {
            DoHomework(31, 8, api);
            if (api.GetClassroomProgress(31, 8) >= 10000000)
            {
                homework_31_8 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 13) && (x <= 27) && (y >= 13) && (y <= 21)))
        //作业三区，到（23，19）做作业
    {
        //走到（23,19）         @@@
        if (homework_23_19 == 0)
        {
            DoHomework(23, 19, api);
            if (api.GetClassroomProgress(23, 19) >= 10000000)
            {
                homework_23_19 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 7) && (x <= 11) && (y >= 27) && (y <= 35)))
        //作业四区，到（9，30）做作业
    {
        //走到（9,30）         @@@
        if (homework_9_30 == 0)
        {
            DoHomework(9, 30, api);
            if (api.GetClassroomProgress(9, 30) >= 10000000)
            {
                homework_9_30 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 7) && (x <= 15) && (y >= 36) && (y <= 43)) || ((x >= 11) && (x <= 13) && (y >= 44) && (y <= 48)))
        //作业五区，到（11，39）做作业
    {
        //走到（11,39）         @@@
        if (homework_11_39 == 0)
        {
            DoHomework(11, 39, api);
            if (api.GetClassroomProgress(11, 39) >= 10000000)
            {
                homework_11_39 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 16) && (x <= 21) && (y >= 31) && (y <= 44)))
        //作业六区，到（20，42）做作业
    {
        //走到（20,42）         @@@
        if (homework_20_42 == 0)
        {
            DoHomework(20, 42, api);
            if (api.GetClassroomProgress(20, 42) >= 10000000)
            {
                homework_20_42 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 27) && (x <= 31) && (y >= 25) && (y <= 36)))
        //作业七区，到（29，27）做作业
    {
        //走到（29,27）         @@@
        if (homework_29_27 == 0)
        {
            DoHomework(29, 27, api);
            if (api.GetClassroomProgress(29, 27) >= 10000000)
            {
                homework_29_27 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 28) && (x <= 31) && (y >= 37) && (y <= 40)) || ((x >= 32) && (x <= 37) && (y >= 28) && (y <= 44)))
        //作业八区，到（34，41）做作业
    {
        //走到（34,41）         @@@
        if (homework_34_41 == 0)
        {
            DoHomework(34, 41, api);
            if (api.GetClassroomProgress(34, 41) >= 10000000)
            {
                homework_34_41 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 38) && (x <= 42) && (y >= 12) && (y <= 17)) || ((x >= 32) && (x <= 42) && (y >= 18) && (y <= 27)) || ((x >= 43) && (x <= 47) && (y >= 25) && (y <= 27)))
        //作业九区，到（41，13）做作业
    {
        //走到（41,13）         @@@
        if (homework_41_13 == 0)
        {
            DoHomework(41, 13, api);
            if (api.GetClassroomProgress(41, 13) >= 10000000)
            {
                homework_41_13 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
    if (((x >= 39) && (x <= 43) && (y >= 28) && (y <= 33)) || ((x >= 44) && (x <= 49) && (y >= 28) && (y <= 39)))
        //作业十区，到（46，33）做作业
    {
        //走到（46,33）         @@@

        if ((homework_46_33 == 0))
        {
            DoHomework(46, 33, api);
            if (api.GetClassroomProgress(46, 33) >= 10000000)
            {
                homework_46_33 = 1;
            }
        }
        else
        {
            GetToTheNearestHomework(api);
        }
    }
}
int pi = 3.1415926;
void AI::play(IStudentAPI& api)
{
    /* 防撞墙1.0(仅作记录，插入新的寻路后可删除)
    auto p = api.GetSelfInfo();
      double x_grid = p->x;
      double y_grid = p->y;
      double pi_2 = 2 * 3.1415926;
      double changeangle = 3.1415926 / 2;
      int x_cell_later = x_grid / 1000 + cos(angle);
      int y_cell_later = y_grid / 1000 + sin(angle);
      if (api.GetPlaceType(x_cell_later, y_cell_later) != THUAI6::PlaceType::Wall)
      {
          api.Move(1000, angle);
          if (api.GetPlaceType(x_cell_later, y_cell_later) == THUAI6::PlaceType::Window)
          {
              api.SkipWindow();
          }
      }
      else angle = angle + changeangle;*/




      /*作业尝试1.0，仅记录，Dohomework函数完成后可删除
      auto p1 = api.GetSelfInfo();
       if (flag == 0) api.MoveRight(1000);
       int x1 = api.GridToCell(p1->x);
       int y1 = api.GridToCell(p1->y);
       auto judge = api.GetPlaceType(x1 - 1, y1);
       if ((judge == THUAI6::PlaceType::ClassRoom) && (api.GetClassroomProgress(23, 19) < 10000000))
       {
           flag = 1;
           api.StartLearning();
       }
       if ((judge == THUAI6::PlaceType::ClassRoom) && (api.GetClassroomProgress(23, 19) >= 10000000))
       {
           api.EndAllAction();
           api.Wait();
           api.MoveLeft(1000);
       }
       */

       // 公共操作
    if (this->playerID == 0)
    {
        if (flag == 0)
        {
            api.MoveRight(7 * 347.22222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(347 * 7));
            flag = 1;
        }
        if (flag == 1)
        {
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            api.MoveLeft(11 * 347.22222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(12*347));
            flag++;
        }
        if (flag == 2)
        {
            api.MoveDown(7 * 347.222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(7 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            flag++;
        }
        if (flag == 3)
        {
            api.MoveUp(11 * 347.222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(11 * 347));
            flag++;
        }
        if (flag == 4)
        {
            api.MoveLeft(4 * 347.222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(4 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            flag++;
        }
        if (flag == 5)
        {
            auto p = api.GetSelfInfo();
            double x_grid = p->x;
            double y_grid = p->y;
            double changeangle = 3.1415926 / 2;
            int x_cell_later = x_grid / 1000 + cos(angle);
            int y_cell_later = y_grid / 1000 + sin(angle);
            if (api.GetPlaceType(x_cell_later, y_cell_later) == THUAI6::PlaceType::Land )//判断下一个方块是不是墙或者是窗户
            {
                api.Move(1000, angle);
            }
            else angle = angle + changeangle;//转角90度
        }
       /* auto Trick = api.GetTrickers();
        if (!Trick.empty())//如果周围有Tricker，进行躲避
        {
            RunAway(api);
        }
        if (Trick.empty())//如果没有Tricker，调用原策略，分区进行
        {
            auto gameinfo1 = api.GetGameInfo();
            if (gameinfo1->subjectFinished < 7)
            {
                PlayerCarryOut(api);
            }
            else
            {
                //走到门（6，7）或（47，46）     @@@（选一个就好）
                PlayerCarryOut(api);
            }
        }*/
    }
    if (this->playerID == 1)
    {
        if (flaggg == 0)
        {
            api.MoveUp(10 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(3472*2));
            api.MoveLeft(4 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(4 * 347));
            flaggg++;
        }
        if (flaggg == 1)
        {

            api.SkipWindow();
            std::this_thread::sleep_for(std::chrono::milliseconds(3472*3));
            flaggg++;

        }
        if (flaggg == 2)
        {
            api.MoveDown(7 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(7 * 347));
            flaggg++;
        }
        if (flaggg == 3)
        {
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            api.MoveUp(9.5 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * 347));
            api.MoveLeft(4 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(4 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            flaggg++;
        }
        if (flaggg == 4)
        {
            api.MoveUp(2 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(3 * 347));
            api.MoveLeft(7 * 347);
            std::this_thread::sleep_for(std::chrono::milliseconds(8 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            flaggg++;
        }
        if (flagg == 5)
        {
            auto p = api.GetSelfInfo();
            double x_grid = p->x;
            double y_grid = p->y;
            double changeangle = 3.1415926 / 2;
            int x_cell_later = x_grid / 1000 + cos(angle);
            int y_cell_later = y_grid / 1000 + sin(angle);
            if (api.GetPlaceType(x_cell_later, y_cell_later) == THUAI6::PlaceType::Land)//判断下一个方块是不是墙或者是窗户
            {
                api.Move(1000, angle);
            }
            else angle = angle + changeangle;//转角90度
        }
        

    } // 玩家1执行操作
    else if (this->playerID == 2)
    {
        if (flagggg == 0)
        {
            api.MoveDown(20 * 347.222222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(25 * 347));
            api.MoveRight(8 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(8 * 347));
            flagggg++;
        }
        if (flagggg == 1)
        {
            api.SkipWindow();
            std::this_thread::sleep_for(std::chrono::milliseconds(3472 * 2));
            api.MoveDown(6 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(8 * 347));
            api.MoveRight(14 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(15 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            api.MoveLeft(5 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(6 * 347));
            api.MoveUp(5 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(6 * 347));
            api.MoveLeft(14 * 347.2222222222);
            std::this_thread::sleep_for(std::chrono::milliseconds(15 * 347));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            flagggg++;
        }
        if (flagggg == 2)
        {
            auto p = api.GetSelfInfo();
            double x_grid = p->x;
            double y_grid = p->y;
            double changeangle = 3.1415926 / 2;
            int x_cell_later = x_grid / 1000 + cos(angle);
            int y_cell_later = y_grid / 1000 + sin(angle);
            if (api.GetPlaceType(x_cell_later, y_cell_later) == THUAI6::PlaceType::Land)//判断下一个方块是不是墙或者是窗户
            {
                api.Move(1000, angle);
            }
            else angle = angle + changeangle;//转角90度
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        //MoveTo(5, 6, api, 2880, 0);
        /*
        double dx, dy, aa;
            //std::this_thread::sleep_for(std::chrono::milliseconds(810));
            dx = x-4000;
            dy = y-21000;
            ddx = abs(api.GridToCell(dx));
            ddy = abs(api.GridToCell(dy));
            c= UpLeft(ddx, ddy, x, y, api);
            ii = abs(ddx) + abs(ddy) - 2;
            while (ii>=0 && c[0] != 3)
            {
                if (c[ii]==1)
                {
                    api.MoveUp(347.222222);
                    std::this_thread::sleep_for(std::chrono::milliseconds(347));
                }
                if (c[ii] == 2)
                {
                    api.MoveLeft(347.222222);
                    std::this_thread::sleep_for(std::chrono::milliseconds(347));
                }
                ii--;
            }*/
           // flag1 = 1;

        }
        /*long long step;//用于记录走到目的地所需要的步骤
        step = UpLeft(x_cell - 10, y_cell - 21, x_cell, y_cell, api);;
        double time_ms = (1000.0 / (2880)) * 1000;
        int len = x_cell - 10 + y_cell - 21-1;
        if (len>0)
        {
            
                if (step % 10 == 0)
                {
                    api.MoveUp(time_ms);
                    std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                }
                else
                {
                    api.MoveLeft(time_ms);
                    std::this_thread::sleep_for(std::chrono::milliseconds(int(time_ms)));
                }
                step /= 10;
                len--;

        }*/// 玩家2执行操作

    else if (this->playerID == 3)
    {
        if (flagg == 0)
        {
            api.Move(3472 * 2, pi / 4.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(3472 * 2));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
            api.MoveUp(347 * 4.5);
            std::this_thread::sleep_for(std::chrono::milliseconds(347 * 5));
            flagg++;
        }
        if (flagg == 1)
        {
            api.MoveLeft(347 * 30);
            std::this_thread::sleep_for(std::chrono::milliseconds(3472 * 3));
            api.StartLearning();
            std::this_thread::sleep_for(std::chrono::milliseconds(75000));
        }
        
        if (flagg == 2)
        {
            auto p = api.GetSelfInfo();
            double x_grid = p->x;
            double y_grid = p->y;
            double changeangle = 3.1415926 / 2;
            int x_cell_later = x_grid / 1000 + cos(angle);
            int y_cell_later = y_grid / 1000 + sin(angle);
            if (api.GetPlaceType(x_cell_later, y_cell_later) == THUAI6::PlaceType::Land)//判断下一个方块是不是墙或者是窗户
            {
                api.Move(1000, angle);
            }
            else angle = angle + changeangle;//转角90度
        }

    } // 玩家3执行操作
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}


void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
}               
