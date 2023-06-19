#include <vector>
#include <thread>
#include <array>
#include<math.h>
#include "AI.h"
#include "constants.h"
int flag1 = 0, flag2 = 0, flag3 = 0;
long long c = 0;
int ii = -1, jj = 0, iii = -1;
int count = 0;
// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Athlete };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;
// 可以在AI.cpp内部声明变量与函数


long long UpLeft(int dx, int dy, int x, int y, ITrickerAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
{                                                                   //从右往左每个数字代表一步，0代表向上一步，1代表向左一步//
    int a = 0, flag = 0;
    if (dx == 0)
    {
        for (int i = 1; i < dy; i++)
        {
            if (api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Wall|| api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - i) == THUAI6::PlaceType::Window)
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
            if (api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Wall|| api.GetPlaceType(api.GridToCell(x) - i, api.GridToCell(y)) == THUAI6::PlaceType::Window)
            {
                flag = 1; break;
            }
            else a = a * 10 + 0;
        }
        if (flag == 1)  return 2;
        else return a;
    }
    if (UpLeft(dx, dy - 1, x, y - 1000, api) != 2 && api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Wall&& api.GetPlaceType(api.GridToCell(x), api.GridToCell(y) - 1) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx, dy-1, x , y-1000, api) + 1;
    }
    if (UpLeft(dx - 1, dy, x - 1000, y, api) != 2 && api.GetPlaceType(api.GridToCell(x)-1, api.GridToCell(y)) != THUAI6::PlaceType::Wall && api.GetPlaceType(api.GridToCell(x)-1, api.GridToCell(y)) != THUAI6::PlaceType::Window)
    {
        return 10 * UpLeft(dx - 1, dy, x - 1000, y, api);
    }
    else return 2;
}

long long UpRight(int dx, int dy, int x, int y, ITrickerAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
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

long long DownLeft(int dx, int dy, int x, int y, ITrickerAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
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

long long DownRight(int dx, int dy, int x, int y, ITrickerAPI& api)       //会输出一串形如101000111的数字(数字应有dx+dy-1位，若不足，则在高位自动补足0)//
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
void DownRight(long long c, ITrickerAPI& api)
{
    if (c % 10 == 0)
    {
        api.MoveDown(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    if (c % 10 == 1)
    {
        api.MoveRight(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    return;
}
void DownLeft(long long c, ITrickerAPI& api)
{
    if (c % 10 == 0)
    {
        api.MoveDown(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    if (c % 10 == 1)
    {
        api.MoveLeft(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    return;
}
void UpRight(long long c, ITrickerAPI& api)
{
    if (c % 10 == 0)
    {
        api.MoveUp(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    if (c % 10 == 1)
    {
        api.MoveRight(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    return;
}
void UpLeft(long long c, ITrickerAPI& api)
{
    if (c % 10 == 0)
    {
        api.MoveUp(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    if (c % 10 == 1)
    {
        api.MoveLeft(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    return;
}


void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 2)
    {
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

void AI::play(ITrickerAPI& api)
{
    double dx, dy, aa=0;
    int ddx, ddy;
    auto self = api.GetSelfInfo();
    auto st = api.GetStudents();
    if (!st.empty())
    {
        api.UseSkill(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(810));
        if (flag1 == 0)
        {
            dx = st[0]->x - self->x;
            dy = st[0]->y - self->y;
            ddx = abs(api.GridToCell(dx));
            ddy = abs(api.GridToCell(dy));
            if (dx >= 0&&dy >= 0) { c = DownRight(ddx, ddy, self->x, self->y, api); jj = 1; }
            if (dx >= 0&&dy < 0) { c = DownLeft(ddx, ddy, self->x, self->y, api); jj = 2; }
            if (dx < 0&&dy >= 0) { c = UpRight(ddx, ddy, self->x, self->y, api); jj = 3;  }
            if (dx < 0&&dy < 0) { c = UpLeft(ddx, ddy, self->x, self->y, api); jj = 4; }
            ii = ddx + ddy - 1;
            flag1 = 1;
        }
        if (ii > 0 && c != 2 && jj == 1)
        {
            DownRight(c, api);
            c = c / 10;
            ii--;
        }
        if (ii > 0 && c != 2 && jj == 2)
        {
            DownLeft(c, api);
            c = c / 10;
            ii--;
        }
        if (ii > 0 && c != 2 && jj == 3)
        {

            UpRight(c, api);
            c = c / 10;
            ii--;
        }
        if (ii > 0 && c != 2 && jj == 4)
        {
            UpLeft(c, api);
            c = c / 10;
            ii--;
        }
        if (ii == 0)
        {
            auto self = api.GetSelfInfo();
            auto st = api.GetStudents();
            if (!st.empty())
            {
                if (api.GridToCell(st[0]->x) == api.GridToCell(self->x) && st[0]->y > self->y) aa = 1.5707963;
                if (api.GridToCell(st[0]->x) == api.GridToCell(self->x) && st[0]->y < self->y) aa = 4.7123889;
                if (api.GridToCell(st[0]->y) == api.GridToCell(self->y) && st[0]->x > self->x) aa = 0;
                if (api.GridToCell(st[0]->y) == api.GridToCell(self->y) && st[0]->x < self->x) aa = 3.1415926;
                api.Attack(aa);
            }
        }
    }
    if(st.empty())
    {
         if (count==0)
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            api.MoveUp(2 * 259.6053998);
            std::this_thread::sleep_for(std::chrono::milliseconds(540));
         }
         if (count < 13)
         {
             api.MoveLeft(259.6053998);
             std::this_thread::sleep_for(std::chrono::milliseconds(270));
             count++;
         }
         if (count == 13)
         {
             api.MoveDown(2 * 259.6053998);
             std::this_thread::sleep_for(std::chrono::milliseconds(530));
             count++;
         }
     }
    if (count == 14)
    {
        auto self = api.GetSelfInfo();
        c = DownRight(9, 5, self->x, self->y, api);
        iii = 13; count++; ii = -1;
    }
    if (count == 15)
    {
        if (iii > 0 && c != 2)
        {
            DownRight(c, api);
            c = c / 10;
            iii--;
        }
        if (iii == 1)
        {
            api.MoveDown(259.6053998);
            std::this_thread::sleep_for(std::chrono::milliseconds(280));
            count++; flag1 = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    if (st.empty() && api.GridToCell(self->x) == 23 && api.GridToCell(self->y) == 12 && count == 16)
    {
        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        api.MoveRight(259.6053998 * 2);
        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        api.MoveDown(259.6053998 * 13);
        std::this_thread::sleep_for(std::chrono::milliseconds(3500));
        ii = -1;
    }
    if (count < 26 && count >= 17)
    {
        api.MoveRight(259.6053998);
        std::this_thread::sleep_for(std::chrono::milliseconds(280));
        count++;
    }
    if (count == 26)
    {
        ii = 0;
    }
}