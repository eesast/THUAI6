#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include<utility>
#include<queue>
#include<string>
#include <stdlib.h>
#include "AI.h"
#include "constants.h"
#include <atomic>
#include <mutex>
#include <condition_variable>

std::atomic<bool> running(true);
std::mutex mtx;
std::condition_variable cv;
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

// 可以在AI.cpp内部声明变量与函数

// 选手可以在这里定义自己的变量和函数
#define PI 3.1415926535897932384626433832795

double getDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double getRadian(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx == 0)
    {
        if (dy > 0)
            return PI / 2;
        else
            return PI * 3 / 2;
    }
    else if (dx > 0)
    {
        if (dy == 0)
            return 0;
        else if (dy > 0)
            return atan(dy / dx);
        else
            return atan(dy / dx) + PI * 2;
    }
    else
    {
        if (dy == 0)
            return PI;
        else
            return atan(dy / dx) + PI;
    }
}
double getGraident(int delta_Z_on_X, int delta_Z_on_Y)
{
    // 输入在X轴和Y轴上前进1格，Z轴上的变化量
    // 返回最大Z轴位移方向的弧度
    double delta_Z = sqrt(delta_Z_on_X * delta_Z_on_X + delta_Z_on_Y * delta_Z_on_Y);
    if (delta_Z == 0)
        return 0;
    double ans = acos(delta_Z_on_X / delta_Z);
    if (delta_Z_on_Y < 0)
        ans = 2 * PI - ans;
    return ans;
}
int BFS(int startX, int startY, std::vector<std::vector<THUAI6::PlaceType>>& mapinfo, THUAI6::PlaceType targetType)
{
    // return the first step to the target
    int visited[50][50];
    memset(visited, -1, sizeof(visited));//初始化为-1
    std::queue<std::tuple<int, int, int>> q;//队列q
    q.push(std::make_tuple(startX, startY, -1));
    visited[startX][startY] = 0;
    while (!q.empty())
    {
        std::tuple<int, int, int> now = q.front();
        q.pop();
        int x = std::get<0>(now);
        int y = std::get<1>(now);
        int direction = std::get<2>(now);
        if (mapinfo[x][y] == targetType)
            return direction;
        // 这四个方向可能需要改
        if (x > 0 && visited[x - 1][y] == -1 && (mapinfo[x - 1][y] == THUAI6::PlaceType::Land) || (mapinfo[x - 1][y] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x - 1, y, 2));
            else
                q.push(std::make_tuple(x - 1, y, direction));
            visited[x - 1][y] = visited[x][y] + 1;
        }
        if (x < 49 && visited[x + 1][y] == -1 && (mapinfo[x + 1][y] == THUAI6::PlaceType::Land) || (mapinfo[x + 1][y] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x + 1, y, 0));
            else
                q.push(std::make_tuple(x + 1, y, direction));
            visited[x + 1][y] = visited[x][y] + 1;
        }
        if (y > 0 && visited[x][y - 1] == -1 && (mapinfo[x][y - 1] == THUAI6::PlaceType::Land) || (mapinfo[x][y - 1] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x, y - 1, 3));
            else
                q.push(std::make_tuple(x, y - 1, direction));
            visited[x][y - 1] = visited[x][y] + 1;
        }
        if (y < 49 && visited[x][y + 1] == -1 && (mapinfo[x][y + 1] == THUAI6::PlaceType::Land) || (mapinfo[x][y + 1] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x, y + 1, 1));
            else
                q.push(std::make_tuple(x, y + 1, direction));
            visited[x][y + 1] = visited[x][y] + 1;
        }
    }
    return -1;
}
void func(IStudentAPI& api, short speed, short life, short hp, short shield, short rec, int* c, int* g, int* d3, int* d5, int* d6)
{
    bool skill;
    double direction, angle, movetime;
    std::vector<std::vector<THUAI6::PlaceType>> mapinfo = api.GetFullMap();
    std::shared_ptr<const THUAI6::Student> selfinfo = api.GetSelfInfo();
    std::vector<std::shared_ptr<const THUAI6::Tricker>> trickerinfo = api.GetTrickers();
    int dir[2], i = 0, flag = 0, flag1 = 0;
    while (api.HaveMessage() == true)
    {
        std::pair<int64_t, std::string> s = api.GetMessage();
        if (selfinfo->playerID == 0 && s.first == 2)
        {
            dir[i] = std::stoi(s.second);
        }
        else if (selfinfo->playerID == 1 && s.first == 3)
        {
            dir[i] = std::stoi(s.second);
        }
        i++;
        flag = 1;
    }
    if (dir[0] == 3)
    {
        if (dir[1] == 1)
        {
            while (1)
            {
            DOR31:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3)//这里写的是与门重叠，不知道是不是这样
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
            DOR3:
                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3)
                {
                    api.ThrowProp(THUAI6::PropType::Key3);
                    if (api.MoveRight(1 / selfinfo->speed).get() == true)
                    {
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                        api.MoveLeft(1 / selfinfo->speed);
                    }
                    else
                    {
                        if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveRight(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveUp(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveDown(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveUp(1 / selfinfo->speed);
                                }
                            }
                        }
                    }
                AGAIN11:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                    }
                    while (api.UseSkill(0).get() == false && (api.GetClassroomProgress(selfinfo->x + 1, selfinfo->y) != 1) && api.GetClassroomProgress(selfinfo->x - 1, selfinfo->y) != 1 && api.GetClassroomProgress(selfinfo->x, selfinfo->y + 1) != 1 && api.GetClassroomProgress(selfinfo->x, selfinfo->y - 1) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                    }
                    if (api.GetClassroomProgress(selfinfo->x - 1, selfinfo->y) == 1 || api.GetClassroomProgress(selfinfo->x + 1, selfinfo->y) == 1 || api.GetClassroomProgress(selfinfo->x, selfinfo->y + 1) == 1 || api.GetClassroomProgress(selfinfo->x, selfinfo->y - 1) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        goto AGAIN11;
                        flag1++;
                    }
                    if (flag1 == 3)
                    {
                        //逃跑函数2：出门
                        if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                        {
                            mapinfo[d5[2]][d5[3]] = THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                goto DOR5;
                            }
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                        }
                        if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                        {
                            mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y + 1] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                goto DOR5;
                            }
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                        }
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6)) {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6)) {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
        }
    }
    else
    {
        while (1)
        {
        DOR32:
            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))
            {
                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                angle = direction * PI / 2;
                movetime = 1000 / api.GetSelfInfo()->speed;
                api.Move(movetime, angle);
            }
            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3)) {
                api.ThrowProp(THUAI6::PropType::Key3);
                if (api.MoveRight(1 / selfinfo->speed).get() == true)
                {
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                    api.MoveLeft(1 / selfinfo->speed);
                }
                else
                {
                    if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                    {
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                        api.MoveRight(1 / selfinfo->speed);
                    }
                    else
                    {
                        if (api.MoveUp(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveDown(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveDown(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveUp(1 / selfinfo->speed);
                            }
                        }
                    }
                }


            AGAIN12:
                while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
                while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                {
                    api.StartLearning();
                    api.Wait();
                    api.EndAllAction();
                    //是否有bgm响4
                    //下接mz的逃跑函数
                }
                int flag3 = 0;
                if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                {
                    mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                    flag3++;
                    goto AGAIN12;
                }

                if (flag3 == 3)
                {
                    if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                    {
                        mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Wall;
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)) {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)) {
                            api.OpenDoor();
                            goto DOR5;
                        }
                        mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                    }
                    if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                    {
                        mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)) {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)) {
                            api.OpenDoor();
                            goto DOR5;
                        }
                        mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                    }
                    if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                    {
                        mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                        {
                            api.OpenDoor();
                            goto DOR6;
                        }
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                        {
                            api.OpenDoor();
                            goto DOR6;
                        }
                        mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                    }
                    if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                    {
                        mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                        {
                            api.OpenDoor();
                            goto DOR6;
                        }
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                        }
                        if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                        {
                            api.OpenDoor();
                            goto DOR6;
                        }
                        mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                    }
                }
            }
        }
    }
    if (dir[0] == 5)
    {
        if (dir[1] == 0)
        {
            while (1)
            {
            DOR51:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))//这里写的是与门重叠，不知道是不是这样
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }

                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5))

                {
                    api.ThrowProp(THUAI6::PropType::Key5);
                    //退开一点2
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                    //上边的逆动作3
                AGAIN13:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                    }
                    while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                    }
                    int flag4 = 0;
                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag4++;
                        goto AGAIN13;
                    }
                    if (flag4 == 3)
                    {
                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }

                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
        }
        else
        {
            while (1)
            {
            DOR52:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))//这里写的是与门重叠，不知道是不是这样
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
            DOR5:
                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5))

                {
                    api.ThrowProp(THUAI6::PropType::Key5);
                    if (api.MoveRight(1 / selfinfo->speed).get() == true)
                    {
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                        api.MoveLeft(1 / selfinfo->speed);
                    }
                    else
                    {
                        if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveRight(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveUp(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveDown(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveUp(1 / selfinfo->speed);
                                }
                            }
                        }
                    }


                AGAIN14:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                    }
                    while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                    }
                    int flag5 = 0;
                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag5++;
                        goto AGAIN14;
                    }
                    if (flag5 == 3)
                    {
                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }

                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
        }
    }
    if (dir[0] == 6)
    {
        if (dir[1] == 0)
        {
            while (1)
            {
            DOR61:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                {
                    api.ThrowProp(THUAI6::PropType::Key6);
                    //退开一点2
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                    //上边的逆动作3
                AGAIN15:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                    }
                    while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                    }
                    int flag6 = 0;
                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag6++;
                        goto AGAIN15;
                    }
                    if (flag6 == 3)
                    {
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }

                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Door3;
                        }
                    }

                }
            }
        }
        else
        {
            while (1)
            {
            DOR62:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
            DOR6:
                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                {
                    api.ThrowProp(THUAI6::PropType::Key6);
                    if (api.MoveRight(1 / selfinfo->speed).get() == true)
                    {
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                        api.MoveLeft(1 / selfinfo->speed);
                    }
                    else
                    {
                        if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveRight(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveUp(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveDown(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveUp(1 / selfinfo->speed);
                                }
                            }
                        }
                    }


                AGAIN16:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                    }
                    while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                    }
                    int flag7 = 0;
                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag7++;
                        goto AGAIN16;
                    }
                    if (flag7 == 3)
                    {
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                //这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                            {
                                api.OpenDoor();
                                goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }

                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                            }
                            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                            {
                                api.OpenDoor();
                                goto DOR3;
                            }
                            mapinfo[d6[0]][d6[1]] = THUAI6::PlaceType::Door3;
                        }
                    }
                }
            }
        }
    }
    if (trickerinfo[0]->trickerType == THUAI6::TrickerType::ANoisyPerson)
    {
        while (trickerinfo[0]->trickDesire > 0)
        {
            int firstAlert = trickerinfo[0]->trickDesire;
            // 试着向右走 得到dangerAlert的变化量
            int movetime = 100 / api.GetSelfInfo()->speed;
            api.Move(movetime, 0);
            int secondAlert = api.GetSelfInfo()->dangerAlert;
            // 试着向下走 得到dangerAlert的变化量
            api.Move(movetime, PI / 2);
            int thirdAlert = api.GetSelfInfo()->dangerAlert;
            // 计算出dangerAlert的梯度，作为降低最快的方向
            double gradient = getGraident(secondAlert - firstAlert, thirdAlert - secondAlert);
            // 向梯度方向走
            movetime *= 10;
            api.Move(movetime, gradient);
            if (api.GetSelfInfo()->dangerAlert == 0) break;
            if (speed == 1 && trickerinfo[0]->trickDesire != 0)
            {
                api.UseProp(THUAI6::PropType::AddSpeed);
            }
            if (life == 1 && selfinfo->determination < 1500000)
            {
                api.UseProp(THUAI6::PropType::AddLifeOrClairaudience);
            }
            if (hp == 1 && selfinfo->determination < 1500000)
            {
                api.UseProp(THUAI6::PropType::AddHpOrAp);
            }
            std::vector<std::shared_ptr<const THUAI6::Bullet>> danger = api.GetBullets();
            if (shield == 1 && getDistance(danger[0]->x, danger[0]->y, selfinfo->x, selfinfo->y) < 0.1)
            {
                api.UseProp(THUAI6::PropType::ShieldOrSpear);
            }
            if (rec == 1 && selfinfo->playerState == THUAI6::PlayerState::Stunned)
            {
                api.UseProp(THUAI6::PropType::RecoveryFromDizziness);
            }//daoju
        }
    }
    while (selfinfo->dangerAlert > 0)
    {
        int firstAlert = selfinfo->dangerAlert;
        // 试着向右走 得到dangerAlert的变化量
        int movetime = 100 / api.GetSelfInfo()->speed;
        api.Move(movetime, 0);
        int secondAlert = api.GetSelfInfo()->dangerAlert;
        // 试着向下走 得到dangerAlert的变化量
        api.Move(movetime, PI / 2);
        int thirdAlert = api.GetSelfInfo()->dangerAlert;
        // 计算出dangerAlert的梯度，作为降低最快的方向
        double gradient = getGraident(secondAlert - firstAlert, thirdAlert - secondAlert);
        // 向梯度方向走
        movetime *= 10;
        api.Move(movetime, gradient);
        if (api.GetSelfInfo()->dangerAlert == 0) break;
        if (speed == 1 && selfinfo->dangerAlert != 0)
        {
            api.UseProp(THUAI6::PropType::AddSpeed);
        }
        if (life == 1 && selfinfo->determination < 1500000)
        {
            api.UseProp(THUAI6::PropType::AddLifeOrClairaudience);
        }
        if (hp == 1 && selfinfo->determination < 1500000)
        {
            api.UseProp(THUAI6::PropType::AddHpOrAp);
        }
        std::vector<std::shared_ptr<const THUAI6::Bullet>> danger = api.GetBullets();
        if (shield == 1 && getDistance(danger[0]->x, danger[0]->y, selfinfo->x, selfinfo->y) < 0.1)
        {
            api.UseProp(THUAI6::PropType::ShieldOrSpear);
        }
        if (rec == 1 && selfinfo->playerState == THUAI6::PlayerState::Stunned)
        {
            api.UseProp(THUAI6::PropType::RecoveryFromDizziness);
        }//daoju
    }

    if (api.GetClassroomProgress(c[0], c[1]) + api.GetClassroomProgress(c[3], c[2]) + api.GetClassroomProgress(c[5], c[4]) + api.GetClassroomProgress(c[7], c[6]) + api.GetClassroomProgress(c[9], c[8]) + api.GetClassroomProgress(c[11], c[10]) + api.GetClassroomProgress(c[13], c[12]) + api.GetClassroomProgress(c[15], c[14]) + api.GetClassroomProgress(c[17], c[16]) + api.GetClassroomProgress(c[19], c[18]) >= 7)
    {
        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::Gate)
        {
            double direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Gate) != -1;
            double angle = direction * PI / 2;
            double movetime = 1000 / api.GetSelfInfo()->speed;
            api.Move(movetime, angle);
        }
        if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Gate)
            api.StartOpenGate();
        if (api.GetGateProgress(g[0], g[1]) == 1 || api.GetGateProgress(g[2], g[3]) == 1) api.Graduate();
    }//biye
    std::vector<std::shared_ptr<const THUAI6::Student>> stuinfo = api.GetStudents();
    if (selfinfo->playerID == 2 || selfinfo->playerID == 3)
    {
        if (stuinfo[0]->determination <= 1800000 || selfinfo->determination <= 1800000)
        {
            api.UseSkill(1);//如果学霸的血量低，使用勉励
            if (stuinfo[0]->determination <= 1800000 || selfinfo->determination <= 1800000)
            {
                if (stuinfo[0]->determination < selfinfo->determination)
                {
                    if (selfinfo->playerID == 2) api.StartEncourageMate(0);
                    else api.StartEncourageMate(1);
                }
            }
        }
        if (stuinfo[0]->determination == 0 || selfinfo->determination == 0)
        {
            api.UseSkill(0);
            if (stuinfo[0]->determination == 0)
            {
                if (selfinfo->playerID == 2) api.StartRouseMate(0);
                else api.StartRouseMate(1);
            }
        }
        if (selfinfo->dangerAlert > 0)
        {
            api.UseSkill(1);
        }
    }
    else
    {
        if (stuinfo[0]->determination <= 1800000 || selfinfo->determination <= 1800000)
        {
            if (stuinfo[0]->determination < selfinfo->determination)
            {
                if (selfinfo->playerID == 0) api.StartEncourageMate(2);
                else api.StartEncourageMate(3);
            }
        }
        if (stuinfo[0]->determination == 0 || selfinfo->determination == 0)
        {
            if (selfinfo->playerID == 0) api.StartRouseMate(2);
            else api.StartRouseMate(3);
        }
    }
}

void AI::play(IStudentAPI& api)
{

    std::vector<std::vector<THUAI6::PlaceType>> mapinfo = api.GetFullMap();
    std::shared_ptr<const THUAI6::Student> selfinfo = api.GetSelfInfo();
    short has_3 = 0, has_5 = 0, has_6 = 0, speed = 0, life = 0, hp = 0, shield = 0, rec = 0;//道具设置
    int chests[Constants::numOfChest * 2]; int* c = chests;
    int dor3[4], dor5[4], dor6[4], gate[4];
    int* d3 = dor3, * d5 = dor5, * d6 = dor6, * g = gate;
    double tran, d1, d2;
    int id = this->playerID;
    int flag = 0, goal = 0, flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0, flag5 = 0, flag6 = 0, flag7 = 0, flag8 = 0, flag9 = 0, flag10 = 0, flag11 = 0, flag12 = 0, flag13 = 0, flag14 = 0;
    std::string s1, s2, s3, s4, sd0, sd1, sd2, sd3;
    double direction, angle;
    int movetime;
    int i = 0, j = 0, NUM = 0;//循环变量
    if (this->playerID == 0 || this->playerID == 1)
    {
        for (i = 0; i < 50; i++)
        {
            for (j = 0; j < 50; j++)
            {
                if (mapinfo[i][j] == THUAI6::PlaceType::Chest)
                {
                    *c = i; c++; *c = j; c++;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door3)
                {
                    *d3 = i; d3++; *d3 = j;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door5)
                {
                    *d5 = i; d5++; *d5 = j;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door6)
                {
                    *d6 = i; d6++; *d6 = j;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Gate)
                {
                    *g = i; g++; *g = j;
                }
            }
        }
        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
        for (int i = 0; i < Constants::numOfChest * 2; i += 2)
        {
            for (int j = i + 2; j < Constants::numOfChest * 2; j += 2)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, c[i], c[i + 1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, c[j], c[j + 1]);
                if (d2 < d1)
                {
                    tran = c[j]; c[j] = c[i]; c[i] = tran;
                    tran = c[j + 1]; c[j + 1] = c[i + 1]; c[i + 1] = tran;
                }
                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

            }
        }

        while (1)
        {
            std::string sc1 = std::to_string(c[NUM]), sc2 = std::to_string(c[NUM + 1]);
            goal = NUM;
            if (NUM == Constants::numOfChest * 2) break;
            else NUM++;
            if (api.GetChestProgress(c[goal], c[goal + 1]) != 1) flag = 1;//探测其是否被开
            //看看队友找没找到钥匙,如果有，直接跳出跟随1->interrupt
            while (flag == 1 && api.GetChestProgress(c[goal], c[goal + 1]) != 1 && flag2 != 1)
            {
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Chest)
                {
                    flag2 = 1;//到达箱子
                }
                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Chest) != -1;
                angle = direction * PI / 2;
                movetime = 1000 / api.GetSelfInfo()->speed;
                api.Move(movetime, angle);
                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

            }//向箱子走
            if (flag2 == 1)
            {
                while (api.GetChestProgress(c[goal], c[goal + 1]) != 1) api.StartOpenChest();//开箱
                mapinfo[i][j] = THUAI6::PlaceType::Wall;
                flag = 0;
                if (api.PickProp(THUAI6::PropType::Key3).get()) has_3 = 1;
                if (api.PickProp(THUAI6::PropType::Key5).get()) has_5 = 1;
                if (api.PickProp(THUAI6::PropType::Key6).get()) has_6 = 1;
                if (api.PickProp(THUAI6::PropType::AddSpeed).get()) speed = 1;
                if (api.PickProp(THUAI6::PropType::AddLifeOrClairaudience).get()) life = 1;
                if (api.PickProp(THUAI6::PropType::AddHpOrAp).get()) hp = 1;
                if (api.PickProp(THUAI6::PropType::ShieldOrSpear).get()) shield = 1;
                if (api.PickProp(THUAI6::PropType::RecoveryFromDizziness).get()) rec = 1;
                if ((has_3 + has_5 + has_6 + speed + life + hp + shield + rec) == 3) break;//若道具箱满了
            }
            if (has_3 == 1)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, d3[0], d3[1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, d3[2], d3[3]);
                if (d1 >= d2)
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "2");
                        api.SendTextMessage(2, "3");
                    }
                    else
                    {
                        api.SendTextMessage(3, "2");
                        api.SendTextMessage(3, "3");
                    }
                DOR31:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3)//这里写的是与门重叠，不知道是不是这样
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                    }
                DOR3:
                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3 || mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3)
                    {
                        api.ThrowProp(THUAI6::PropType::Key3);
                        if (api.MoveRight(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveLeft(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveRight(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveUp(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveDown(1 / selfinfo->speed);
                                }
                                else
                                {
                                    if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                    {
                                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                        api.MoveUp(1 / selfinfo->speed);
                                    }
                                }
                            }
                        }
                    AGAIN11:
                        while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                        }
                        while (api.UseSkill(0).get() == false && (api.GetClassroomProgress(selfinfo->x + 1, selfinfo->y) != 1) && api.GetClassroomProgress(selfinfo->x - 1, selfinfo->y) != 1 && api.GetClassroomProgress(selfinfo->x, selfinfo->y + 1) != 1 && api.GetClassroomProgress(selfinfo->x, selfinfo->y - 1) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                        }

                        if (api.GetClassroomProgress(selfinfo->x - 1, selfinfo->y) == 1 || api.GetClassroomProgress(selfinfo->x + 1, selfinfo->y) == 1 || api.GetClassroomProgress(selfinfo->x, selfinfo->y + 1) == 1 || api.GetClassroomProgress(selfinfo->x, selfinfo->y - 1) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            goto AGAIN11;
                            flag1++;
                        }
                        if (flag1 == 3)
                        {
                            //逃跑函数2：出门
                            if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                            {
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                                }
                                if (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5)
                                {
                                    api.OpenDoor();
                                    goto DOR5;
                                }
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                            }
                            if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                            {
                                mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5 && mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                                }
                                if (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x + 1][selfinfo->y + 1] == THUAI6::PlaceType::Door5 || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)
                                {
                                    api.OpenDoor();
                                    goto DOR5;
                                }
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                            }
                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6)) {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6)) {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                        }
                    }
                }
                else
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "1");
                        api.SendTextMessage(2, "3");
                    }
                    else
                    {
                        api.SendTextMessage(3, "1");
                        api.SendTextMessage(3, "3");
                    }
                DOR32:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3)) {
                        api.ThrowProp(THUAI6::PropType::Key3);
                        if (api.MoveRight(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveLeft(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveRight(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveUp(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveDown(1 / selfinfo->speed);
                                }
                                else
                                {
                                    if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                    {
                                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                        api.MoveUp(1 / selfinfo->speed);
                                    }
                                }
                            }
                        }


                    AGAIN12:
                        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        int flag3 = 0;
                        if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            flag3++;
                            goto AGAIN12;
                        }

                        if (flag3 == 3)
                        {
                            if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                            {
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)) {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)) {
                                    api.OpenDoor();
                                    goto DOR5;
                                }
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                            }
                            if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                            {
                                mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5)) {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5)) {
                                    api.OpenDoor();
                                    goto DOR5;
                                }
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                            }
                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                        }
                    }
                }
            }
            if (has_5 == 1)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, d5[0], d5[1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, d5[2], d5[3]);
                if (d1 >= d2)
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "2");
                        api.SendTextMessage(2, "5");
                    }
                    else
                    {
                        api.SendTextMessage(3, "2");
                        api.SendTextMessage(3, "5");
                    }
                DOR51:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))//这里写的是与门重叠，不知道是不是这样
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }

                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5))

                    {
                        api.ThrowProp(THUAI6::PropType::Key5);
                        //退开一点2
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                        //上边的逆动作3
                    AGAIN13:
                        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        int flag4 = 0;
                        if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            flag4++;
                            goto AGAIN13;
                        }
                        if (flag4 == 3)
                        {
                            if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                            {
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                            }
                            if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                            {
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                            }

                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                            }
                        }
                    }
                }
                else
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "1");
                        api.SendTextMessage(2, "5");
                    }
                    else
                    {
                        api.SendTextMessage(3, "1");
                        api.SendTextMessage(3, "5");
                    }
                DOR52:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))//这里写的是与门重叠，不知道是不是这样
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                DOR5:
                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door5 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door5) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door5))

                    {
                        api.ThrowProp(THUAI6::PropType::Key5);
                        if (api.MoveRight(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveLeft(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveRight(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveUp(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveDown(1 / selfinfo->speed);
                                }
                                else
                                {
                                    if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                    {
                                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                        api.MoveUp(1 / selfinfo->speed);
                                    }
                                }
                            }
                        }


                    AGAIN14:
                        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }

                        if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            flag5++;
                            goto AGAIN14;
                        }
                        if (flag5 == 3)
                        {
                            if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                            {
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                            }
                            if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                            {
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                            }

                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                            }
                        }
                    }
                }
            }
            if (has_6 == 1)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, d6[0], d6[1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, d6[2], d6[3]);
                if (d1 >= d2)
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "2");
                        api.SendTextMessage(2, "6");
                    }
                    else
                    {
                        api.SendTextMessage(3, "2");
                        api.SendTextMessage(3, "6");
                    }
                DOR61:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                    {
                        api.ThrowProp(THUAI6::PropType::Key6);
                        //退开一点2
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                        //上边的逆动作3
                    AGAIN15:
                        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        int flag6 = 0;
                        if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            flag6++;
                            goto AGAIN15;
                        }
                        if (flag6 == 3)
                        {
                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                            }

                            if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                            {
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                            }
                            if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                            {
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Door3;
                            }
                        }

                    }
                }
                else
                {
                    if (id == 0)
                    {
                        api.SendTextMessage(2, "1");
                        api.SendTextMessage(2, "6");
                    }
                    else
                    {
                        api.SendTextMessage(3, "1");
                        api.SendTextMessage(3, "6");
                    }
                DOR62:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                DOR6:
                    if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                    {
                        api.ThrowProp(THUAI6::PropType::Key6);
                        if (api.MoveRight(1 / selfinfo->speed).get() == true)
                        {
                            while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                            api.MoveLeft(1 / selfinfo->speed);
                        }
                        else
                        {
                            if (api.MoveLeft(1 / selfinfo->speed).get() == true)
                            {
                                while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                api.MoveRight(1 / selfinfo->speed);
                            }
                            else
                            {
                                if (api.MoveUp(1 / selfinfo->speed).get() == true)
                                {
                                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                    api.MoveDown(1 / selfinfo->speed);
                                }
                                else
                                {
                                    if (api.MoveDown(1 / selfinfo->speed).get() == true)
                                    {
                                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y + 1) == false && api.IsDoorOpen(selfinfo->x, selfinfo->y - 1) == false && api.IsDoorOpen(selfinfo->x + 1, selfinfo->y) == false && api.IsDoorOpen(selfinfo->x - 1, selfinfo->y) == false);
                                        api.MoveUp(1 / selfinfo->speed);
                                    }
                                }
                            }
                        }


                    AGAIN16:
                        while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                        {
                            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                            angle = direction * PI / 2;
                            movetime = 1000 / api.GetSelfInfo()->speed;
                            api.Move(movetime, angle);
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        while (api.UseSkill(0).get() == false && api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                        {
                            api.StartLearning();
                            api.Wait();
                            api.EndAllAction();
                            //是否有bgm响4
                            //下接mz的逃跑函数
                            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                        }
                        int flag7 = 0;
                        if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                        {
                            mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                            flag7++;
                            goto AGAIN16;
                        }
                        if (flag7 == 3)
                        {
                            if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                            {
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                                    //这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                            }
                            if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                            {
                                mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door6 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door6) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door6))
                                {
                                    api.OpenDoor();
                                    goto DOR6;
                                }
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                            }

                            if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                            {
                                mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                            }
                            if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                            {
                                mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))//这里写的是与门重叠，不知道是不是这样
                                {
                                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                    angle = direction * PI / 2;
                                    movetime = 1000 / api.GetSelfInfo()->speed;
                                    api.Move(movetime, angle);
                                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                                }
                                if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Door3 || (mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Door3) || (mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Door3))
                                {
                                    api.OpenDoor();
                                    goto DOR3;
                                }
                                mapinfo[d6[0]][d6[1]] = THUAI6::PlaceType::Door3;
                            }
                        }
                    }
                }
            }
        }
        //退出开箱阶段，打游击战
    }
    if (this->playerID == 2 || this->playerID == 3)
    {

        for (i = 0; i < 50; i++)
        {
            for (j = 0; j < 50; j++)
            {
                if (mapinfo[i][j] == THUAI6::PlaceType::Chest)
                {
                    *c = i; c++; *c = j; c++;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door3)
                {
                    *d3 = i; d3++; *d3 = j;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door5)
                {
                    *d5 = i; d5++; *d5 = j;
                }
                if (mapinfo[i][j] == THUAI6::PlaceType::Door6)
                {
                    *d6 = i; d6++; *d6 = j;
                }
            }
        }
        for (int i = 0; i < Constants::numOfChest * 2; i += 2)
        {
            for (int j = i + 2; j < Constants::numOfChest * 2; j += 2)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, c[i], c[i + 1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, c[j], c[j + 1]);
                if (d2 < d1)
                {
                    tran = c[j]; c[j] = c[i]; c[i] = tran;
                    tran = c[j + 1]; c[j + 1] = c[i + 1]; c[i + 1] = tran;
                }
            }
        }

        while (1)
        {
            std::string sc1 = std::to_string(c[NUM]), sc2 = std::to_string(c[NUM + 1]);
            goal = NUM;
            if (NUM == (Constants::numOfChest * 2 + 1)) break;
            if ((NUM == goal) && (api.GetChestProgress(c[goal], c[goal + 1]) != 1)) flag = 1;//探测其是否被开
            func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
            //看看队友找没找到钥匙,如果有，直接跳出跟随1->interrupt
            while (flag == 1 && api.GetChestProgress(c[goal], c[goal + 1]) != 1 && flag2 != 1)
            {
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Chest)
                {
                    flag2 = 1;//到达箱子
                }
                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Chest) != -1;
                angle = direction * PI / 2;
                movetime = 1000 / api.GetSelfInfo()->speed;
                api.Move(movetime, angle);
                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
            }//向箱子走
            if (flag2 == 1)
            {
                while (api.GetChestProgress(c[goal], c[goal + 1]) != 1) api.StartOpenChest();//开箱
                mapinfo[c[goal]][c[goal + 1]] = THUAI6::PlaceType::Wall;
                flag = 0;
                if (api.PickProp(THUAI6::PropType::Key3).get()) has_3 = 1;
                if (api.PickProp(THUAI6::PropType::Key5).get()) has_5 = 1;
                if (api.PickProp(THUAI6::PropType::Key6).get()) has_6 = 1;
                if (api.PickProp(THUAI6::PropType::AddSpeed).get()) speed = 1;
                if (api.PickProp(THUAI6::PropType::AddLifeOrClairaudience).get()) life = 1;
                if (api.PickProp(THUAI6::PropType::AddHpOrAp).get()) hp = 1;
                if (api.PickProp(THUAI6::PropType::ShieldOrSpear).get()) shield = 1;
                if (api.PickProp(THUAI6::PropType::RecoveryFromDizziness).get()) rec = 1;
                if ((has_3 + has_5 + has_6 + speed + life + hp + shield + rec) == 3) break;//若找到钥匙/道具箱满了
            }
            if (has_3 == 1)
            {
                d1 = getDistance(selfinfo->x, selfinfo->y, d3[0], d3[1]);
                d2 = getDistance(selfinfo->x, selfinfo->y, d3[2], d3[3]);
                if (d1 >= d2)
                {
                    if (id == 2)
                    {
                        api.SendTextMessage(0, "2");
                        api.SendTextMessage(0, "3");
                    }
                    else
                    {
                        api.SendTextMessage(1, "2");
                        api.SendTextMessage(1, "3");
                    }
                    //   DOR31:
                    while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    //DOR3:
                    if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                        while (api.IsDoorOpen(selfinfo->x, selfinfo->y) == false) api.OpenDoor();

                AGAIN21:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag9++;
                        goto AGAIN21;
                    }
                    if (flag9 == 3)
                    {
                        if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                        {
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                                //goto DOR5;
                            }

                        }
                        if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                        {
                            mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                //goto DOR5;
                            }
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                        }
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
            else
            {
                if (id == 0)
                {
                    api.SendTextMessage(2, "1");
                    api.SendTextMessage(2, "3");
                }
                else
                {
                    api.SendTextMessage(3, "1");
                    api.SendTextMessage(3, "3");
                }
                //DOR32:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                }
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                {
                    api.OpenDoor();
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                AGAIN22:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag10++;
                        goto AGAIN22;
                    }
                    if (flag10 == 3)
                    {
                        if (api.GetDoorProgress(d5[0], d5[1]) == 1)
                        {
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                //goto DOR5;
                            }
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                        }
                        if (api.GetDoorProgress(d5[2], d5[3]) == 1)
                        {
                            mapinfo[d5[0]][d5[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                            {
                                api.OpenDoor();
                                //goto DOR5;
                            }
                            mapinfo[d5[2]][d5[3]] == THUAI6::PlaceType::Door5;
                        }
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
        }
        if (has_5 == 1)
        {
            d1 = getDistance(selfinfo->x, selfinfo->y, d5[0], d5[1]);
            d2 = getDistance(selfinfo->x, selfinfo->y, d5[2], d5[3]);
            if (d1 >= d2)
            {
                if (id == 0)
                {
                    api.SendTextMessage(2, "2");
                    api.SendTextMessage(2, "5");
                }
                else
                {
                    api.SendTextMessage(3, "2");
                    api.SendTextMessage(3, "5");
                }
                //DOR51:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))

                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                }
                //DOR5:
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                {
                    api.OpenDoor();
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                AGAIN23:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag11++;
                        goto AGAIN23;
                    }
                    if (flag11 == 3)
                    {
                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }

                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }

                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                        }
                    }

                }
            }
            else
            {
                if (id == 0)
                {
                    api.SendTextMessage(2, "1");
                    api.SendTextMessage(2, "5");
                }
                else
                {
                    api.SendTextMessage(3, "1");
                    api.SendTextMessage(3, "5");
                }
                //DOR52:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door5 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door5) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door5))
                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                }
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door5)
                {
                    api.OpenDoor();
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                AGAIN24:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag12++;
                        goto AGAIN24;
                    }
                    if (flag12 == 3)
                    {
                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))
                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();

                            }
                            //goto DOR3;
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door3;
                        }

                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);
                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Door6;
                        }
                    }
                }
            }
        }
        if (has_6 == 1)
        {
            d1 = getDistance(selfinfo->x, selfinfo->y, d6[0], d6[1]);
            d2 = getDistance(selfinfo->x, selfinfo->y, d6[2], d6[3]);
            sd0 = std::to_string(d6[0]);
            sd1 = std::to_string(d6[1]);
            sd2 = std::to_string(d6[2]);
            sd3 = std::to_string(d6[3]);
            if (d1 >= d2)
            {
                if (id == 0)
                {
                    api.SendTextMessage(2, "2");
                    api.SendTextMessage(2, "6");
                }
                else
                {
                    api.SendTextMessage(3, "2");
                    api.SendTextMessage(3, "6");
                }
                //DOR61:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                    func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                }
                //DOR6:
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                {
                    api.OpenDoor();
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                AGAIN25:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag13++;
                        goto AGAIN25;
                    }
                    if (flag13 == 3)
                    {
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))


                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }

                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Door3;
                        }
                    }
                }
            }
            else
            {
                if (id == 0)
                {
                    api.SendTextMessage(2, "1");
                    api.SendTextMessage(2, "6");
                }
                else
                {
                    api.SendTextMessage(3, "1");
                    api.SendTextMessage(3, "6");
                }
                //DOR62:
                while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                {
                    direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                    angle = direction * PI / 2;
                    movetime = 1000 / api.GetSelfInfo()->speed;
                    api.Move(movetime, angle);
                }
                if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                {
                    api.OpenDoor();
                    while (api.IsDoorOpen(selfinfo->x, selfinfo->y - 1000) == false);
                AGAIN26:
                    while (mapinfo[selfinfo->x][selfinfo->y] != THUAI6::PlaceType::ClassRoom)
                    {
                        direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::ClassRoom) != -1;
                        angle = direction * PI / 2;
                        movetime = 1000 / api.GetSelfInfo()->speed;
                        api.Move(movetime, angle);
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                    }
                    while (api.GetClassroomProgress(selfinfo->x, selfinfo->y) != 1)
                    {
                        api.StartLearning();
                        api.Wait();
                        api.EndAllAction();
                        //是否有bgm响4
                        //下接mz的逃跑函数
                        func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                    }

                    if (api.GetClassroomProgress(selfinfo->x, selfinfo->y) == 1)
                    {
                        mapinfo[selfinfo->x][selfinfo->y] = THUAI6::PlaceType::Wall;
                        flag14++;
                        goto AGAIN26;
                    }
                    if (flag14 == 3)
                    {
                        if (api.GetDoorProgress(d6[0], d6[1]) == 1)
                        {
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door5) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door6;
                        }
                        if (api.GetDoorProgress(d6[2], d6[3]) == 1)
                        {
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door6) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door6)
                            {
                                api.OpenDoor();
                                //goto DOR6;
                            }
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Door6;
                        }

                        if (api.GetDoorProgress(d3[0], d3[1]) == 1)
                        {
                            mapinfo[d3[2]][d3[3]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door6 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door6) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door6))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d6[2]][d6[3]] == THUAI6::PlaceType::Door3;
                        }
                        if (api.GetDoorProgress(d3[2], d3[3]) == 1)
                        {
                            mapinfo[d3[0]][d3[1]] == THUAI6::PlaceType::Wall;
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }

                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::Door3 && (mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::Door3) && (mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::Door3))

                            {
                                direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Door3) != -1;
                                angle = direction * PI / 2;
                                movetime = 1000 / api.GetSelfInfo()->speed;
                                api.Move(movetime, angle);
                                func(api, speed, life, hp, shield, rec, c, g, d3, d5, d6);

                            }
                            if (mapinfo[selfinfo->x][selfinfo->y] == THUAI6::PlaceType::Door3)
                            {
                                api.OpenDoor();
                                //goto DOR3;
                            }
                            mapinfo[d6[0]][d6[1]] == THUAI6::PlaceType::Door3;
                        }
                    }
                }
            }
        }
    }

}

//tricker
void func2(ITrickerAPI& api)
{
    std::vector<std::vector<THUAI6::PlaceType>> mapinfo = api.GetFullMap();
    std::vector<std::shared_ptr<const THUAI6::Student>> info = api.GetStudents();
    std::shared_ptr<const THUAI6::Tricker> selfinfo = api.GetSelfInfo();
    if (selfinfo->trickDesire >= (153 / 130))
    {
        api.UseSkill(0);
    }

}
int BFSx(int startX, int startY, std::vector<std::vector<THUAI6::PlaceType>>& mapinfo, int targetx, int targety)
{
    // return the first step to the target
    int visited[50][50];
    memset(visited, -1, sizeof(visited));//初始化为-1
    std::queue<std::tuple<int, int, int>> q;//队列q
    q.push(std::make_tuple(startX, startY, -1));
    visited[startX][startY] = 0;
    while (!q.empty())
    {
        std::tuple<int, int, int> now = q.front();
        q.pop();
        int x = std::get<0>(now);
        int y = std::get<1>(now);
        int direction = std::get<2>(now);
        if (x == targetx && y == targety)
            return direction;
        // 这四个方向可能需要改
        if (x > 0 && visited[x - 1][y] == -1 && (mapinfo[x - 1][y] == THUAI6::PlaceType::Land) || (mapinfo[x - 1][y] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x - 1, y, 2));
            else
                q.push(std::make_tuple(x - 1, y, direction));
            visited[x - 1][y] = visited[x][y] + 1;
        }
        if (x < 49 && visited[x + 1][y] == -1 && (mapinfo[x + 1][y] == THUAI6::PlaceType::Land) || (mapinfo[x + 1][y] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x + 1, y, 0));
            else
                q.push(std::make_tuple(x + 1, y, direction));
            visited[x + 1][y] = visited[x][y] + 1;
        }
        if (y > 0 && visited[x][y - 1] == -1 && (mapinfo[x][y - 1] == THUAI6::PlaceType::Land) || (mapinfo[x][y - 1] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x, y - 1, 3));
            else
                q.push(std::make_tuple(x, y - 1, direction));
            visited[x][y - 1] = visited[x][y] + 1;
        }
        if (y < 49 && visited[x][y + 1] == -1 && (mapinfo[x][y + 1] == THUAI6::PlaceType::Land) || (mapinfo[x][y + 1] == THUAI6::PlaceType::Grass))
        {
            if (direction == -1)
                q.push(std::make_tuple(x, y + 1, 1));
            else
                q.push(std::make_tuple(x, y + 1, direction));
            visited[x][y + 1] = visited[x][y] + 1;
        }
    }
    return -1;
}

void AI::play(ITrickerAPI& api)
{
    std::vector<std::vector<THUAI6::PlaceType>> mapinfo = api.GetFullMap();
    std::shared_ptr<const THUAI6::Tricker> selfinfo = api.GetSelfInfo();
    std::vector<std::vector<THUAI6::PlaceType>> mapinfo1 = api.GetFullMap();
    short has_3 = 0, has_5 = 0, has_6 = 0, speed = 0, life = 0, hp = 0, shield = 0, rec = 0;//道具设置
    int chests[Constants::numOfChest * 2]; int* c = chests;
    int classroomx[9], classroomy[9];
    double tran, d1, d2;
    int id = selfinfo->playerID;
    int flag = 0, goal = 0, flag2 = 0, sign = 0;
    std::string s1, s2, s3, s4, sd0, sd1, sd2, sd3;
    double direction, angle;
    int movetime;
    int i = 0, j = 0, NUM = 0;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            if (mapinfo[i][j] == THUAI6::PlaceType::ClassRoom)
            {
                classroomx[NUM] = i;
                classroomy[NUM] = j;
                NUM++;
            }
            if (mapinfo[i][j] == THUAI6::PlaceType::Chest)
            {
                *c = i; c++; *c = j; c++;
            }
            func2(api);
        }
    }
    for (int i = 0; i < Constants::numOfChest * 2; i += 2)
    {
        for (int j = i + 2; j < Constants::numOfChest * 2; j += 2)
        {
            d1 = getDistance(selfinfo->x, selfinfo->y, c[i], c[i + 1]);
            d2 = getDistance(selfinfo->x, selfinfo->y, c[j], c[j + 1]);
            if (d2 < d1)
            {
                tran = c[j]; c[j] = c[i]; c[i] = tran;
                tran = c[j + 1]; c[j + 1] = c[i + 1]; c[i + 1] = tran;
            }
            func2(api);
        }
    }
    NUM = 0;
    while (flag == 0)
    {
        std::string sc1 = std::to_string(c[NUM]), sc2 = std::to_string(c[NUM + 1]);
        goal = NUM;
        if (NUM == (Constants::numOfChest * 2 + 1)) break;
        if ((NUM == goal) && (api.GetChestProgress(c[goal], c[goal + 1]) != 1)) flag = 1;
        while (flag == 1 && api.GetChestProgress(c[goal], c[goal + 1]) != 1 && flag2 != 1)
        {
            if (mapinfo[selfinfo->x + 1][selfinfo->y] == THUAI6::PlaceType::Chest || mapinfo[selfinfo->x - 1][selfinfo->y] == THUAI6::PlaceType::Chest || mapinfo[selfinfo->x][selfinfo->y + 1] == THUAI6::PlaceType::Chest || mapinfo[selfinfo->x][selfinfo->y - 1] == THUAI6::PlaceType::Chest)
            {
                flag2 = 1;
            }
            direction = BFS(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, THUAI6::PlaceType::Chest) != -1;
            angle = direction * PI / 2;
            movetime = 1000 / api.GetSelfInfo()->speed;
            api.Move(movetime, angle);
            func2(api);
        }
        if (api.GetChestProgress(c[goal], c[goal + 1]) == 1)
        {
            flag = 0;
            mapinfo[c[goal]][c[goal + 1]] = THUAI6::PlaceType::Wall;
        }
        if (flag2 == 1)
        {
            while (api.GetChestProgress(c[goal], c[goal + 1]) != 1) api.StartOpenChest();//开箱
            mapinfo[c[goal]][c[goal + 1]] = THUAI6::PlaceType::Wall;
            flag = 0;
            if (api.PickProp(THUAI6::PropType::Key3).get()) has_3 = 1;
            if (api.PickProp(THUAI6::PropType::Key5).get()) has_5 = 1;
            if (api.PickProp(THUAI6::PropType::Key6).get()) has_6 = 1;
            func2(api);
        }
        while (flag != 0)
        {
            NUM = 0;
            while (sign == 0)
            {
                if (api.GetClassroomProgress(classroomx[NUM], classroomy[NUM]) > 0 && api.GetClassroomProgress(classroomx[NUM], classroomy[NUM]) < 1)sign = 1;
                else NUM++;
            }
            while (mapinfo[selfinfo->x + 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x - 1][selfinfo->y] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y + 1] != THUAI6::PlaceType::ClassRoom || mapinfo[selfinfo->x][selfinfo->y - 1] != THUAI6::PlaceType::ClassRoom)
            {
                direction = BFSx(selfinfo->x / 1000, selfinfo->y / 1000, mapinfo, classroomx[NUM], classroomy[NUM]);
                angle = direction * PI / 2;
                movetime = 1000 / api.GetSelfInfo()->speed;
                api.Move(movetime, angle);
                func2(api);
            }
        }
    }
}
