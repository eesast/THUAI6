#include <vector>
#include <thread>
#include <array>
#include<cmath>
#include "AI.h"
#include "constants.h"
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::Sunshine,
    THUAI6::StudentType::StraightAStudent};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

// 可以在AI.cpp内部声明变量与函数

int window3[5][2] = { {13,21},{15,5},{29,3},{27,11},{19,23} };
int window5[3][2] = { {18,31},{6,33},{16,44} };
int window6[6][2] = { {27,35},{33,18},{42,18},{47,27},{46,39},{38,43} };

int e[2] = { 0,0 };
int door3[2][2] = { {14,14},{28,15} };
int door5[2][2] = { {20,29},{8,38} };
int door6[3][2] = { {37,16},{29,39},{44,36} };

//当要进入某教学楼时查询最快通道
int* Insidebuilding(int x,int y,int i)
{
    if (i == 3)
    {
        if (1 <= x && x <= 16 && 1 <= y && y <= 13)return window3[1];
        if (1 <= x && x <= 15 && 14 <= y && y <= 44)return window3[0];
        if (1 <= x && x <= 9 && 45 <= y && y <= 49)return window3[0];
        if (23 <= x && x <= 49 && 1 <= y && y <= 7)return window3[2];
        if (26 <= x && x <= 49 && 8 <= y && y <= 49)return window3[3];
        return window3[4];
    }
    if (i == 5)
    {
        if (1 <= x && x <= 7)return window5[1];
        if (1 <= y && y <= 38)return window5[0];
        return window5[2];
    }
    if (i == 6)
    {
        if (43 <= x && x <= 49 && 22 <= y && y <= 33)return window6[3];
        if (43 <= x && x <= 49 && 40 <= y && y <= 49)return window6[4];
        if (38 <= x && x <= 49 && 1 <= y && y <= 21)return window6[2];
        if (1 <= x && x <= 37 && 1 <= y && y <= 22)return window6[1];
        if (32 <= x && x <= 42 && 34 <= y && y <= 49)return window6[5];
        return window6[0];
    }
    return e;
}

int* Buildingtobuildingtw(int i, int j)
{
    if (i == 3)
    {
        if (j == 5)return window3[4];
        if (j == 6)return window3[3];
    }
    if (i == 5)return window5[0];
    if (i == 6)
    {
        if (j == 3)return window6[1];
        if (j == 5)return window6[0];
    }
    return e;
}

//查询坐标在什么教学楼内
int Buildingnum(int x, int y)
{
    if (x == 7)
        if (28 <= y && y <= 33)return 5;
    if (8 <= x && x <= 10)
        if (28 <= y && y <= 43)return 5;
    if (11 <= x && x <= 13)
    {
        if (6 <= y && y <= 11)return 3;
        if (36 <= y && y <= 47)return 5;
    }
    if (14 <= x && x <= 15)
    {
        if (6 <= y && y <= 22)return 3;
        if (36 <= y && y <= 43)return 5;
    }
    if (16 <= x && x <= 17)
    {
        if (6 <= y && y <= 23)return 3;
        if (32 <= y && y <= 43)return 5;
    }
    if (x == 18)
    {
        if (1 <= y && y <= 23)return 3;
        if (32 <= y && y <= 44)return 5;
    }
    if (19 <= x && x <= 21)
    {
        if (1 <= y && y <= 20)return 3;
        if (29 <= y && y <= 45)return 5;
    }
    if (x == 22)
    {
        if (5 <= y && y <= 20)return 3;
        if (31 <= y && y <= 37)return 5;
    }
    if (23 <= x && x <= 26)
        if (5 <= y && y <= 21)return 3;
    if (x == 27)
        if (4 <= y && y <= 9)return 3;
    if (28 <= x && x <= 29)
    {
        if (4 <= y && y <= 9)return 3;
        if (25 <= y && y <= 39)return 6;
    }
    if (30 <= x && x <= 32)
    {
        if (4 <= y && y <= 10)return 3;
        if (y <= 43 && y >= 23)return 6;
    }
    if (33 <= x && x <= 35)
        if (19 <= y && y <= 34)return 6;
    if (36 <= x && x <= 37)
        if (18 <= y && y <= 34)return 6;
    if (38 <= x && x <= 39)
        if (13 <= y && y <= 28)return 6;
    if (40 <= x && x <= 41)
        if (10 <= y && y <= 28)return 6;
    if (42 <= x && x <= 43)
    {
        if (10 <= y && 14 >= y)return 6;
        if (20 <= y && y <= 21)return 6;
        if (25 <= y && y <= 32)return 6;
    }
    if (44 <= x && x <= 46)
        if (25 <= y && y <= 38)return 6;
    if (x == 47)
        if (30 <= y && y <= 38)return 6;
    if (x == 48 || x == 49)
        if (y <= 38 && y >= 35)return 6;
    return 0;
}

//设置目的地
void Setpoint(IStudentAPI& api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map)
{
    void Moveto(IStudentAPI & api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map);
    std::string dtype = THUAI6::placeTypeDict[map[xd][xd]];
    std::string ntype = THUAI6::placeTypeDict[map[xn][xn]];
    if (dtype != "Window" && ntype != "Window" && dtype != "Door3" && dtype != "Door5" && dtype != "Door6" && ntype != "Door3" && ntype != "Door5" && ntype != "Door6")
    {
        int a = Buildingnum(xd, yd);
        int b = Buildingnum(xn, yn);
        if (a == b)Moveto(api, xd, yd, xn, yn, map);
        else if (a == 0) {
            int* c = Insidebuilding(xd, yd, b);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
        else if (b == 0) {
            int* c = Insidebuilding(xn, yn, a);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
        else {
            int* c = Buildingtobuildingtw(a, b);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
    }
    else Moveto(api, xd, yd, xn, yn, map);
}

void Setpoint(ITrickerAPI& api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map)
{
    void Moveto(ITrickerAPI & api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map);
    std::string dtype = THUAI6::placeTypeDict[map[xd][xd]];
    std::string ntype = THUAI6::placeTypeDict[map[xn][xn]];
    if (dtype != "Window" && ntype != "Window" && dtype != "Door3" && dtype != "Door5" && dtype != "Door6" && ntype != "Door3" && ntype != "Door5" && ntype != "Door6")
    {
        int a = Buildingnum(xd, yd);
        int b = Buildingnum(xn, yn);
        if (a == b)Moveto(api, xd, yd, xn, yn, map);
        else if (a == 0) {
            int* c = Insidebuilding(xd, yd, b);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
        else if (b == 0) {
            int* c = Insidebuilding(xn, yn, a);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
        else {
            int* c = Buildingtobuildingtw(a, b);
            Moveto(api, c[0], c[1], xn, yn, map);
        }
    }
    else Moveto(api, xd, yd, xn, yn, map);
}

int xa=0, ya=0;

void Moveto(IStudentAPI& api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map)
{
    if (xd == xn && yn == yd) { xa = 0; ya = 0; }
    else {
        int xt = xn, yt = yn;
        int t = 0, l;
        if ((THUAI6::placeTypeDict[map[xd][yd]] == "Window") && (((xd == xn) && ((yd - yn == 1) || (yn - yd == 1))) || ((yd == yn) && ((xd - xn == 1) || (xn - xd == 1)))))
        {
            api.SkipWindow(); xa = 0; ya = 0;
        }
        else if (xd > xn)
        {
            xt++;
            std::string m = THUAI6::placeTypeDict[map[xn + 1][yn]];
            if ((m == "Land") || (m == "Grass"))api.MoveDown(50);
            else
            {
                while ((t != 2) && ((m != "Land") && (m != "Grass")))
                {
                    if (yd > yn)
                    {
                        if (t == 0)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveRight(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                    if (yd <= yn)
                    {
                        if (t == 0)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveLeft(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                }
                if (t == 2)
                {
                    if (yd > yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l++; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                    if (yd <= yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l--; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                }xa = xt, ya = yt;
            }
        }
        else if (xd < xn)
        {
            xt--;
            std::string m = THUAI6::placeTypeDict[map[xt][yn]];
            if ((m == "Land") || (m == "Grass"))api.MoveUp(50);
            else
            {
                while ((t != 2) && ((m != "Land") && (m != "Grass")))
                {
                    if (yd < yn)
                    {
                        if (t == 0)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveLeft(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                    if (yd >= yn)
                    {
                        if (t == 0)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveRight(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                }
                if (t == 2)
                {
                    if (yd < yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l--; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                   }
                    if (yn >= yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l++; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                }
                xa = xt, ya = yt;
            }
        }
        else if (xd == xn)
        {
            if (yd > yn)
            {
                {
                    yt++;
                    std::string m = THUAI6::placeTypeDict[map[xt][yt]];
                    if ((m == "Land") || (m == "Grass"))api.MoveRight(50);
                    else
                    {
                        while ((t != 2) && ((m != "Land") && (m != "Grass")))
                        {
                            if (t == 0)
                            {
                                xt++;
                                m = THUAI6::placeTypeDict[map[xt][yt - 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else { t = 1; l = xt; xt = xn; }
                            }
                            if (t == 1)
                            {
                                xt--;
                                m = THUAI6::placeTypeDict[map[xt][yt - 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else t = 2;
                            }
                        }
                        if (t == 2)
                        {
                            m = THUAI6::placeTypeDict[map[l][yt]];
                            while ((m != "Land") && (m != "Grass")) {
                                l++; m = THUAI6::placeTypeDict[map[l][yt]];
                            }
                            xt = l;
                        }
                        xa = xt, ya = yt;
                    }
                }
            }
            else if (yd < yn)
            {
                {
                    yt--;
                    std::string m = THUAI6::placeTypeDict[map[xt][yt]];
                    if ((m == "Land") || (m == "Grass"))api.MoveLeft(50);
                    else
                    {
                        while ((t != 2) && ((m != "Land") && (m != "Grass")))
                        {
                            if (t == 0)
                            {
                                xt--;
                                m = THUAI6::placeTypeDict[map[xt][yt + 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else { t = 1; l = xt; xt = xn; }
                            }
                            if (t == 1)
                            {
                                xt++;
                                m = THUAI6::placeTypeDict[map[xt][yt + 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else t = 2;
                            }
                        }
                        if (t == 2)
                        {
                            m = THUAI6::placeTypeDict[map[l][yt]];
                            while ((m != "Land") && (m != "Grass")) {
                                l--; m = THUAI6::placeTypeDict[map[l][yt]];
                            }
                            xt = l;
                        }
                        xa = xt, ya = yt;
                    }
                }
            }
        }
    }
}

void Moveto(ITrickerAPI& api, int xd, int yd, int xn, int yn, std::vector<std::vector<THUAI6::PlaceType>>map) {
    if (xd == xn && yn == yd) { xa = 0; ya = 0; }
    else {
        int xt = xn, yt = yn;
        int t = 0, l;
        if ((THUAI6::placeTypeDict[map[xd][yd]] == "Window") && (((xd == xn) && ((yd - yn == 1) || (yn - yd == 1))) || ((yd == yn) && ((xd - xn == 1) || (xn - xd == 1)))))
        {
            api.SkipWindow(); xa = 0; ya = 0;
        }
        else if (xd > xn)
        {
            xt++;
            std::string m = THUAI6::placeTypeDict[map[xn + 1][yn]];
            if ((m == "Land") || (m == "Grass"))api.MoveDown(50);
            else
            {
                while ((t != 2) && ((m != "Land") && (m != "Grass")))
                {
                    if (yd > yn)
                    {
                        if (t == 0)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveRight(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                    if (yd <= yn)
                    {
                        if (t == 0)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveLeft(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                }
                if (t == 2)
                {
                    if (yd > yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l++; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                    if (yd <= yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l--; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                }xa = xt, ya = yt;
            }
        }
        else if (xd < xn)
        {
            xt--;
            std::string m = THUAI6::placeTypeDict[map[xt][yn]];
            if ((m == "Land") || (m == "Grass"))api.MoveUp(50);
            else
            {
                while ((t != 2) && ((m != "Land") && (m != "Grass")))
                {
                    if (yd < yn)
                    {
                        if (t == 0)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveLeft(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt + 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                    if (yd >= yn)
                    {
                        if (t == 0)
                        {
                            yt++;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass")) {
                                m = THUAI6::placeTypeDict[map[xt][yt]]; api.MoveRight(50);
                            }
                            else { t = 1; l = yt; yt = yn; }
                        }
                        if (t == 1)
                        {
                            yt--;
                            m = THUAI6::placeTypeDict[map[xt - 1][yt]];
                            if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                            else t = 2;
                        }
                    }
                }
                if (t == 2)
                {
                    if (yd < yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l--; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                    if (yn >= yn)
                    {
                        m = THUAI6::placeTypeDict[map[xt][l]];
                        while ((m != "Land") && (m != "Grass")) {
                            l++; m = THUAI6::placeTypeDict[map[xt][l]];
                        }
                        yt = l;
                    }
                }
                xa = xt, ya = yt;
            }
        }
        else if (xd == xn)
        {
            if (yd > yn)
            {
                {
                    yt++;
                    std::string m = THUAI6::placeTypeDict[map[xt][yt]];
                    if ((m == "Land") || (m == "Grass"))api.MoveRight(50);
                    else
                    {
                        while ((t != 2) && ((m != "Land") && (m != "Grass")))
                        {
                            if (t == 0)
                            {
                                xt++;
                                m = THUAI6::placeTypeDict[map[xt][yt - 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else { t = 1; l = xt; xt = xn; }
                            }
                            if (t == 1)
                            {
                                xt--;
                                m = THUAI6::placeTypeDict[map[xt][yt - 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else t = 2;
                            }
                        }
                        if (t == 2)
                        {
                            m = THUAI6::placeTypeDict[map[l][yt]];
                            while ((m != "Land") && (m != "Grass")) {
                                l++; m = THUAI6::placeTypeDict[map[l][yt]];
                            }
                            xt = l;
                        }
                        xa = xt, ya = yt;
                    }
                }
            }
            else if (yd < yn)
            {
                {
                    yt--;
                    std::string m = THUAI6::placeTypeDict[map[xt][yt]];
                    if ((m == "Land") || (m == "Grass"))api.MoveLeft(50);
                    else
                    {
                        while ((t != 2) && ((m != "Land") && (m != "Grass")))
                        {
                            if (t == 0)
                            {
                                xt--;
                                m = THUAI6::placeTypeDict[map[xt][yt + 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else { t = 1; l = xt; xt = xn; }
                            }
                            if (t == 1)
                            {
                                xt++;
                                m = THUAI6::placeTypeDict[map[xt][yt + 1]];
                                if ((m == "Land") || (m == "Grass"))m = THUAI6::placeTypeDict[map[xt][yt]];
                                else t = 2;
                            }
                        }
                        if (t == 2)
                        {
                            m = THUAI6::placeTypeDict[map[l][yt]];
                            while ((m != "Land") && (m != "Grass")) {
                                l--; m = THUAI6::placeTypeDict[map[l][yt]];
                            }
                            xt = l;
                        }
                        xa = xt, ya = yt;
                    }
                }
            }
        }
    }
}

int Ale=0;
int Alt[3] = { 0,0,0 };
int Stu[3];
int Che[8][2] = { {5,16},{5,46},{14,32},{22,42},{30,21},{38,3},{49,21},{40,34} };
int gridChe[8][2];

void AI::play(IStudentAPI& api)
{
    // 公共操作
    int i;
    auto map = api.GetFullMap();
    auto self = api.GetSelfInfo();
    auto Tri = api.GetTrickers();
    int xn = api.GridToCell(self->x), yn = api.GridToCell(self->y);
    auto student = api.GetStudents();
    int classnum[10] = { api.GetClassroomProgress(19, 6) ,
        api.GetClassroomProgress(31, 8) ,
        api.GetClassroomProgress(23, 19),
        api.GetClassroomProgress(20, 42),
        api.GetClassroomProgress(11, 39),
        api.GetClassroomProgress(9, 32),
        api.GetClassroomProgress(45, 33),
        api.GetClassroomProgress(41, 13),
        api.GetClassroomProgress(34, 41),
        api.GetClassroomProgress(29, 32)
    };
    int dangerbuilding=-1;
    int roomnum = 0;
    for (i = 0; i <= 9; i++)if (classnum[i] == 100)roomnum++;
    if (this->playerID == 0)
    {
        // 玩家0执行操作
        if (xa * ya != 0)Setpoint(api, xa, ya, xn, yn, map);
        else {
            if ((!api.HaveView(Tri[0]->x, Tri[0]->y)) && (self->dangerAlert<=2.7))
                for (i = 0; i <= 3; i++)
                    if ((i!=0)&&(api.HaveView(student[i]->x, student[i]->y)))
                    {
                        if (THUAI6::playerStateDict[student[i]->playerState] == "Addicted")
                            api.StartRouseMate(i);
                        else if (((i == 1) && (student[i]->determination < 20000000)) || (student[i]->determination < 2400000))
                            api.StartEncourageMate(i);
                    }
            if (roomnum >= 7)
            {
                if (dangerbuilding != 6)
                    if (xn == 47 && yn == 45)
                    {
                        if (api.GetGateProgress(47, 46) == 100)api.Graduate();
                        else api.StartOpenGate();
                    }
                    else  Setpoint(api, 47, 45, xn, yn, map);
                else if (xn == 7 && yn == 7)
                {
                    if (api.GetGateProgress(6, 7) == 100)api.Graduate();
                    api.StartOpenGate();
                }
                else Setpoint(api, 7, 7, xn, yn, map);
            }
            while (api.HaveMessage())
            {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
                else  if (mess.second == "Secure") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    Setpoint(api, xd, yd, xn, yn, map);
                    break;
                }
                else if (mess.second == "Help")
                {
                    Setpoint(api, api.GridToCell(student[2]->x), api.GridToCell(student[2]->y), xn, yn, map);
                    if (THUAI6::playerStateDict[self->playerState] == "Addicted")
                        api.SendMessage(3, "Come");
                }
            }
            while (api.HaveMessage()) {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
            }
            if (THUAI6::playerStateDict[self->playerState] == "Addicted")
                api.SendMessage(2, fmt::format("Help"));
            else if (THUAI6::playerStateDict[self->playerState] == "Learning")api.UseSkill(0);
            else if (THUAI6::playerStateDict[self->playerState] == "Idel")
            {
                int a = Buildingnum(xn, yn);
                if (a != 0)
                {
                    if (a != dangerbuilding)
                    {
                        if (a == 3)
                        {
                            if (Stu[0] == 0) {
                                if (classnum[0] != 100)
                                {
                                    if ((xn == 20) && (yn == 6))api.StartLearning();
                                    else Setpoint(api, 20, 6, xn, yn, map);
                                }
                                else if (classnum[1] != 100)
                                {
                                    if ((xn == 31) && (yn == 7))api.StartLearning();
                                    else Setpoint(api, 31, 7, xn, yn, map);
                                }
                                else if (classnum[2] != 100)
                                {
                                    if ((xn == 23) && (yn == 18))api.StartLearning();
                                    else Setpoint(api, 23, 18, xn, yn, map);
                                }
                                else { Stu[0] = 1; }
                            }
                            else if (dangerbuilding != 5)
                            {
                                int* m = Buildingtobuildingtw(3, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(3, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 5)
                        {
                            if (Stu[1] == 0)
                            {
                                if (classnum[3] != 100) {
                                    if ((xn == 19) && (yn == 42))api.StartLearning();
                                    else Setpoint(api, 19, 42, xn, yn, map);
                                }
                                else if (classnum[4] != 100) {
                                    if ((xn == 11) && (yn == 38))api.StartLearning();
                                    else Setpoint(api, 11, 38, xn, yn, map);
                                }
                                else if (classnum[5] != 100) {
                                    if ((xn == 9) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 9, 33, xn, yn, map);
                                }
                                else { Stu[1] = 1; }
                            }
                            else if (dangerbuilding != 6)
                            {
                                int* m = Buildingtobuildingtw(5, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(5, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 6)
                        {
                            if (Stu[2] == 0)
                            {
                                if (classnum[6] != 100) {
                                    if ((xn == 45) && (yn == 34))api.StartLearning();
                                    else Setpoint(api, 45, 34, xn, yn, map);
                                }
                                else if (classnum[7] != 100) {
                                    if ((xn == 41) && (yn == 14))api.StartLearning();
                                    else Setpoint(api, 41, 14, xn, yn, map);
                                }
                                else if (classnum[8] != 100) {
                                    if ((xn == 34) && (yn == 40))api.StartLearning();
                                    else Setpoint(api, 34, 40, xn, yn, map);
                                }
                                else if (classnum[9] != 100) {
                                    if ((xn == 29) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 29, 33, xn, yn, map);
                                }
                                else { Stu[2] = 1; }
                            }
                            else if (dangerbuilding != 3)
                            {
                                int* m = Buildingtobuildingtw(6, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(6, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                    }
                    else if (a == dangerbuilding)
                    {
                        if (a == 3)
                        {
                            int* m = Buildingtobuildingtw(3, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 6)
                        {
                            int* m = Buildingtobuildingtw(6, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 5)
                        {
                            int* m = Buildingtobuildingtw(5, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
                else if (a == 0)
                {
                    if (Stu[2] == 0)
                        if (dangerbuilding == 6)
                        {
                            if (Stu[0] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else if (Stu[1] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    else {
                        if (dangerbuilding != 3) {
                            int* m = Insidebuilding(xn, yn, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 5);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
            }
            if ((self->dangerAlert>=2.7) || api.HaveView(Tri[0]->x, Tri[0]->y))
            {
                api.EndAllAction();
                for (i = 0; i <= 3; i++) api.SendMessage(i, fmt::format("Attention"));
            }
        }
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
        static THUAI6::PropType p[4];
        if (xa * ya != 0)Setpoint(api, xa, ya, xn, yn, map);
        else{
            if (((!api.HaveView(Tri[0]->x, Tri[0]->y)) && (self->dangerAlert<=1.5))||(self->dangerAlert==0))
                for (i = 0; i <= 3; i++)
                    if ((i!=1)&&(api.HaveView(student[i]->x, student[i]->y)))
                    {
                        if (THUAI6::playerStateDict[student[i]->playerState] == "Addicted")
                            api.StartRouseMate(i);
                        else if (student[i]->determination < 2400000)
                            api.StartEncourageMate(i);
                    }
            if ((api.HaveView(Tri[0]->x, Tri[0]->y)) || (self->dangerAlert>=1.5))
            {
                if (api.HaveView(Tri[0]->x, Tri[0]->y))
                    if ((THUAI6::playerStateDict[Tri[0]->playerState] == "Attacking") || (THUAI6::playerStateDict[Tri[0]->playerState] == "Swinging") || (THUAI6::playerStateDict[Tri[0]->playerState] == "Climbing") || (THUAI6::playerStateDict[Tri[0]->playerState] == "Locking"))
                        api.UseSkill(0);
                api.UseProp(p[0]);
                if (self->determination < 20000000)api.UseProp(p[1]);
                api.SendMessage(0, "Attention"); api.SendMessage(3, "Attention"); api.SendMessage(2, "Attention");
                int x1 = api.GridToCell(student[0]->x), y1 = api.GridToCell(student[0]->y), x2 = api.GridToCell(student[3]->x), y2 = api.GridToCell(student[3]->y);
                int a = Buildingnum(x1, y1), b = Buildingnum(x2, y2), c = Buildingnum(xn, yn);
                if ((a != 5) && (b != 5))
                {
                    api.SendMessage(0, fmt::format("B5")); api.SendMessage(3, fmt::format("B5"));
                    if (Alt[0] == 0)
                        if (c == 0)
                        {
                            int* d = Insidebuilding(xn, yn, 5); Setpoint(api, d[0], d[1], xn, yn, map);
                        }
                        else if (c == 5)
                        {
                            Setpoint(api, window5[2][0], window5[2][1], xn, yn, map);
                            if ((window5[2][0] == xn) && (window5[2][1] - yn == 1))
                            {
                                api.SkipWindow(); Alt[0]++;
                            }
                        }
                        else { int* d = Buildingtobuildingtw(c, 5); Setpoint(api, d[0], d[1], xn, yn, map); }
                    else if (Alt[0] % 2 == 1)
                    {
                        Setpoint(api, window5[1][0], window5[1][1], xn, yn, map);
                        if ((window5[1][1] == yn) && (window5[1][0] - xn == 1))
                        {
                            api.SkipWindow(); Alt[0]++;
                        }
                    }
                    else {
                        Setpoint(api, window5[2][0], window5[2][1], xn, yn, map);
                        if ((window5[2][0] == xn) && (window5[2][1] - yn == 1))
                        {
                            api.SkipWindow(); Alt[0]++;
                        }
                    }
                }
                else if ((a != 3) && (b != 3))
                {
                    api.SendMessage(0, fmt::format("B3")); api.SendMessage(3, fmt::format("B3"));
                    if (Alt[1] == 0)
                        if (c == 0)
                        {
                            int* d = Insidebuilding(xn, yn, 3); Setpoint(api, d[0], d[1], xn, yn, map);
                        }
                        else if (c == 3)
                        {
                            Setpoint(api, window3[0][0], window3[0][1], xn, yn, map);
                            if ((window3[0][1] == yn) && (window3[0][0] - xn == -1))
                            {
                                api.SkipWindow(); Alt[1]++;
                            }
                        }
                        else { int* d = Buildingtobuildingtw(c, 3); Setpoint(api, d[0], d[1], xn, yn, map); }
                    else if (Alt[1] % 2 == 1)
                    {
                        Setpoint(api, window3[4][0], window3[4][1], xn, yn, map);
                        if ((window3[4][1] == yn) && (window3[4][0] - xn == -1))
                        {
                            api.SkipWindow(); Alt[1]++;
                        }
                    }
                    else {
                        Setpoint(api, window3[0][0], window3[0][1], xn, yn, map);
                        if ((window3[0][1] == yn) && (window3[0][0] - xn == -1))
                        {
                            api.SkipWindow(); Alt[1]++;
                        }
                    }
                }
                else {
                    api.SendMessage(0, fmt::format("B6")); api.SendMessage(2, fmt::format("B6"));
                    if (Alt[2] == 0)
                        if (c == 0)
                        {
                            int* d = Insidebuilding(xn, yn, 6); Setpoint(api, d[0], d[1], xn, yn, map);
                        }
                        else if (c == 6)
                        {
                            Setpoint(api, window6[5][0], window6[5][1], xn, yn, map);
                            if ((window6[5][0] == xn) && (window6[5][1] - yn == 1))
                            {
                                api.SkipWindow(); Alt[2]++;
                            }
                        }
                        else { int* d = Buildingtobuildingtw(c, 6); Setpoint(api, d[0], d[1], xn, yn, map); }
                    else if (Alt[2] % 2 == 1)
                    {
                        Setpoint(api, window6[0][0], window6[0][1], xn, yn, map);
                        if ((window6[0][0] == xn) && (window6[0][1] - yn == 1))
                        {
                            api.SkipWindow(); Alt[2]++;
                        }
                    }
                    else {
                        Setpoint(api, window6[5][0], window6[5][1], xn, yn, map);
                        if ((window6[5][0] == xn) && (window6[5][1] - yn == 1))
                        {
                            api.SkipWindow(); Alt[2]++;
                        }
                    }
                }
            }
            if (THUAI6::playerStateDict[self->playerState] == "Addicted")
                api.SendMessage(2, fmt::format("Help"));
            if (THUAI6::playerStateDict[self->playerState] == "Stunned")
            {
                api.UseProp(p[3]);
            }
            if (THUAI6::playerStateDict[self->playerState] == "Idel")
            {
                if (THUAI6::propTypeDict[p[3]] == "Key3") {
                    Setpoint(api, door3[0][0], door3[0][1], xn, yn, map);
                    if (api.IsDoorOpen(13, 14))api.CloseDoor();
                    Setpoint(api, door3[1][0], door3[1][1], xn, yn, map);
                    if (api.IsDoorOpen(29, 15)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
                }
                else  if (THUAI6::propTypeDict[p[3]] == "Key5")
                {
                    Setpoint(api, door5[0][0], door5[0][1], xn, yn, map);
                    if (api.IsDoorOpen(7, 38))api.CloseDoor();
                    Setpoint(api, door5[1][0], door5[1][1], xn, yn, map);
                    if (api.IsDoorOpen(20, 28)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
                }
                else  if (THUAI6::propTypeDict[p[3]] == "Key5")
                {
                    Setpoint(api, door6[0][0], door6[0][1], xn, yn, map);
                    if (api.IsDoorOpen(37, 17))api.CloseDoor();
                    Setpoint(api, door6[1][0], door6[1][1], xn, yn, map);
                    if (api.IsDoorOpen(29, 40))api.CloseDoor();
                    Setpoint(api, door6[2][0], door6[2][1], xn, yn, map);
                    if (api.IsDoorOpen(43, 36)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
                }
                for (i = 0; i <= 7; i++)
                {
                    gridChe[i][0] = api.CellToGrid(Che[i][0]);
                    gridChe[i][1] = api.CellToGrid(Che[i][1]);
                    if (api.HaveView(gridChe[i][0], gridChe[i][1]))
                        if (api.GetChestProgress(Che[i][0], Che[i][1]) != 100)
                        {
                            if (((Che[i][0] == xn) && ((Che[i][1] - yn == 1) || (Che[i][1] - yn == -1))) || ((Che[i][1] == yn) && ((Che[i][0] - xn == 1) || (Che[i][0] - xn == -1))))
                                api.StartOpenChest();
                            else  Setpoint(api, Che[i][0], Che[i][1], xn, yn, map); break;
                        }
                }
                auto pro = api.GetProps();
                if (api.HaveView(pro[0]->x, pro[0]->y)) {
                    for (i = 0; i <= 1; i++)
                        api.PickProp(pro[i]->type);
                    for (i = 0; i <= 1; i++)
                        if (THUAI6::propTypeDict[pro[i]->type] == "Key3")p[2] = pro[i]->type;
                        else if (THUAI6::propTypeDict[pro[i]->type] == "Key5")p[2] = pro[i]->type;
                        else if (THUAI6::propTypeDict[pro[i]->type] == "Key6")p[2] = pro[i]->type;
                        else if (THUAI6::propTypeDict[pro[i]->type] == "AddSpeed")api.UseProp(pro[i]->type);
                        else if (THUAI6::propTypeDict[pro[i]->type] == "AddHpOrAp")p[1] = pro[i]->type;
                        else if (THUAI6::propTypeDict[pro[i]->type] == "RecoveryFromDizziness")p[3] = pro[i]->type;
                        else p[0] = pro[i]->type;
                }
            }
            i = 0;
            while (api.HaveMessage())
            {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
                if (mess.second == "Attention") {
                    api.SendMessage(mess.first, fmt::format("Secure"));
                    if (i == 0) {
                        int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                        Setpoint(api, xd, yd, xn, yn, map);
                    }
                    i++;
                }
            }
        }
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        if (xa * ya != 0)Setpoint(api, xa, ya, xn, yn, map);
        else {
            for (i = 0; i <= 3; i++)
                if ((i != 2) && (api.HaveView(student[i]->x, student[i]->y)))
                {
                    if (THUAI6::playerStateDict[student[i]->playerState] == "Addicted")
                    {
                        if (self->timeUntilSkillAvailable[0] == 0)api.UseSkill(0);
                        else api.StartRouseMate(i);
                    }
                    if (((i == 1) && (student[i]->determination < 20000000)) || (student[i]->determination < 2400000))
                    {
                        if (self->timeUntilSkillAvailable[1] == 0)api.UseSkill(1);
                        else api.StartEncourageMate(i);
                    }
                }
            i = 0;
            while (api.HaveMessage())
            {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
                if (mess.second == "Secure") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    Setpoint(api, xd, yd, xn, yn, map);
                    if (self->timeUntilSkillAvailable[3] == 0)api.UseSkill(3);
                    break;
                }
                if (mess.second == "Help") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    if (i == 0)Setpoint(api, xd, yd, xn, yn, map);
                    i++;
                }
            }
            while (api.HaveMessage())
            {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
            }
            if (THUAI6::playerStateDict[self->playerState] == "Addicted")
                for (i = 0; i <= 3; i++)api.SendMessage(i, fmt::format("Help"));
            else if (THUAI6::playerStateDict[self->playerState] == "Learning")api.UseSkill(0);
            else if (THUAI6::playerStateDict[self->playerState] == "Idel")
            {
                int a = Buildingnum(xn, yn);
                if (a != 0)
                {
                    if (a != dangerbuilding)
                    {
                        if (a == 3)
                        {
                            if (Stu[0] == 0) {
                                if (classnum[0] != 100)
                                {
                                    if ((xn == 20) && (yn == 6))api.StartLearning();
                                    else Setpoint(api, 20, 6, xn, yn, map);
                                }
                                else if (classnum[1] != 100)
                                {
                                    if ((xn == 31) && (yn == 7))api.StartLearning();
                                    else Setpoint(api, 31, 7, xn, yn, map);
                                }
                                else if (classnum[2] != 100)
                                {
                                    if ((xn == 23) && (yn == 18))api.StartLearning();
                                    else Setpoint(api, 23, 18, xn, yn, map);
                                }
                                else { Stu[0] = 1; }
                            }
                            else if (dangerbuilding != 5)
                            {
                                int* m = Buildingtobuildingtw(3, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(3, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 5)
                        {
                            if (Stu[1] == 0)
                            {
                                if (classnum[3] != 100) {
                                    if ((xn == 19) && (yn == 42))api.StartLearning();
                                    else Setpoint(api, 19, 42, xn, yn, map);
                                }
                                else if (classnum[4] != 100) {
                                    if ((xn == 11) && (yn == 38))api.StartLearning();
                                    else Setpoint(api, 11, 38, xn, yn, map);
                                }
                                else if (classnum[5] != 100) {
                                    if ((xn == 9) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 9, 33, xn, yn, map);
                                }
                                else { Stu[1] = 1; }
                            }
                            else if (dangerbuilding != 6)
                            {
                                int* m = Buildingtobuildingtw(5, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(5, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 6)
                        {
                            if (Stu[2] == 0)
                            {
                                if (classnum[6] != 100) {
                                    if ((xn == 45) && (yn == 34))api.StartLearning();
                                    else Setpoint(api, 45, 34, xn, yn, map);
                                }
                                else if (classnum[7] != 100) {
                                    if ((xn == 41) && (yn == 14))api.StartLearning();
                                    else Setpoint(api, 41, 14, xn, yn, map);
                                }
                                else if (classnum[8] != 100) {
                                    if ((xn == 34) && (yn == 40))api.StartLearning();
                                    else Setpoint(api, 34, 40, xn, yn, map);
                                }
                                else if (classnum[9] != 100) {
                                    if ((xn == 29) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 29, 33, xn, yn, map);
                                }
                                else { Stu[2] = 1; }
                            }
                            else if (dangerbuilding != 3)
                            {
                                int* m = Buildingtobuildingtw(6, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(6, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                    }
                    else if (a == dangerbuilding)
                    {
                        if (a == 3)
                        {
                            int* m = Buildingtobuildingtw(3, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 6)
                        {
                            int* m = Buildingtobuildingtw(6, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 5)
                        {
                            int* m = Buildingtobuildingtw(5, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
                else if (a == 0)
                {
                    if (Stu[2] == 0)
                        if (dangerbuilding == 6)
                        {
                            if (Stu[0] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else if (Stu[1] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    else {
                        if (dangerbuilding != 3) {
                            int* m = Insidebuilding(xn, yn, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 5);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
            }
            if ((self->dangerAlert>=3) || api.HaveView(Tri[0]->x, Tri[0]->y))
            {
                for (i = 0; i <= 3; i++) api.SendMessage(i, fmt::format("Attention"));
                api.EndAllAction();
            }
        }
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        if (xa * ya != 0)Setpoint(api, xa, ya, xn, yn, map);
        else {
            if ((!api.HaveView(Tri[0]->x, Tri[0]->y)) && (self->dangerAlert<=2.7))
                for (i = 0; i <= 3; i++)
                    if ((i != 4) && (api.HaveView(student[i]->x, student[i]->y)))
                    {
                        if (THUAI6::playerStateDict[student[i]->playerState] == "Addicted")
                            api.StartRouseMate(i);
                        else if (((i == 1) && (student[i]->determination < 20000000)) || (student[i]->determination < 2400000))
                            api.StartEncourageMate(i);
                    }
            if (roomnum >= 7)
            {
                if (dangerbuilding != 6)
                    if (xn == 47 && yn == 45)
                    {
                        if (api.GetGateProgress(47, 46) == 100)api.Graduate();
                        else api.StartOpenGate();
                    }
                    else  Setpoint(api, 47, 45, xn, yn, map);
                else if (xn == 7 && yn == 7)
                {
                    if (api.GetGateProgress(6, 7) == 100)api.Graduate();
                    api.StartOpenGate();
                }
                else Setpoint(api, 7, 7, xn, yn, map);
            }
            while (api.HaveMessage())
            {
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
                if (mess.second == "Secure") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    Setpoint(api, xd, yd, xn, yn, map); break;
                }
                if (mess.second == "Come") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    Setpoint(api, xd, yd, xn, yn, map);
                }
            }
            while(api.HaveMessage()){
                auto mess = api.GetMessage();
                if (mess.second == "B3")dangerbuilding = 3;
                else if (mess.second == "B5")dangerbuilding = 5;
                else if (mess.second == "B6")dangerbuilding = 6;
                if (mess.second == "Attention") {
                    int xd = api.GridToCell(student[mess.first]->x), yd = api.GridToCell(student[mess.first]->y);
                    dangerbuilding = Buildingnum(xd, yd);
                }
            }
            if (THUAI6::playerStateDict[self->playerState] == "Addicted")
                for (i = 0; i <= 3; i++)api.SendMessage(i, fmt::format("Help"));
            else if (THUAI6::playerStateDict[self->playerState] == "Learning")api.UseSkill(0);
            else if (THUAI6::playerStateDict[self->playerState] == "Idel")
            {
                int a = Buildingnum(xn, yn);
                if (a != 0)
                {
                    if (a != dangerbuilding)
                    {
                        if (a == 3)
                        {
                            if (Stu[0] == 0) {
                                if (classnum[0] != 100)
                                {
                                    if ((xn == 20) && (yn == 6))api.StartLearning();
                                    else Setpoint(api, 20, 6, xn, yn, map);
                                }
                                else if (classnum[1] != 100)
                                {
                                    if ((xn == 31) && (yn == 7))api.StartLearning();
                                    else Setpoint(api, 31, 7, xn, yn, map);
                                }
                                else if (classnum[2] != 100)
                                {
                                    if ((xn == 23) && (yn == 18))api.StartLearning();
                                    else Setpoint(api, 23, 18, xn, yn, map);
                                }
                                else { Stu[0] = 1; }
                            }
                            else if (dangerbuilding != 5)
                            {
                                int* m = Buildingtobuildingtw(3, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(3, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 5)
                        {
                            if (Stu[1] == 0)
                            {
                                if (classnum[3] != 100) {
                                    if ((xn == 19) && (yn == 42))api.StartLearning();
                                    else Setpoint(api, 19, 42, xn, yn, map);
                                }
                                else if (classnum[4] != 100) {
                                    if ((xn == 11) && (yn == 38))api.StartLearning();
                                    else Setpoint(api, 11, 38, xn, yn, map);
                                }
                                else if (classnum[5] != 100) {
                                    if ((xn == 9) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 9, 33, xn, yn, map);
                                }
                                else { Stu[1] = 1; }
                            }
                            else if (dangerbuilding != 6)
                            {
                                int* m = Buildingtobuildingtw(5, 6);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(5, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else if (a == 6)
                        {
                            if (Stu[2] == 0)
                            {
                                if (classnum[6] != 100) {
                                    if ((xn == 45) && (yn == 34))api.StartLearning();
                                    else Setpoint(api, 45, 34, xn, yn, map);
                                }
                                else if (classnum[7] != 100) {
                                    if ((xn == 41) && (yn == 14))api.StartLearning();
                                    else Setpoint(api, 41, 14, xn, yn, map);
                                }
                                else if (classnum[8] != 100) {
                                    if ((xn == 34) && (yn == 40))api.StartLearning();
                                    else Setpoint(api, 34, 40, xn, yn, map);
                                }
                                else if (classnum[9] != 100) {
                                    if ((xn == 29) && (yn == 33))api.StartLearning();
                                    else Setpoint(api, 29, 33, xn, yn, map);
                                }
                                else { Stu[2] = 1; }
                            }
                            else if (dangerbuilding != 3)
                            {
                                int* m = Buildingtobuildingtw(6, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else {
                                int* m = Buildingtobuildingtw(6, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                    }
                    else if (a == dangerbuilding)
                    {
                        if (a == 3)
                        {
                            int* m = Buildingtobuildingtw(3, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 6)
                        {
                            int* m = Buildingtobuildingtw(6, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        if (a == 5)
                        {
                            int* m = Buildingtobuildingtw(5, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
                else if (a == 0)
                {
                    if (Stu[2] == 0)
                        if (dangerbuilding == 6)
                        {
                            if (Stu[0] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 3);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                            else if (Stu[1] == 0)
                            {
                                int* m = Insidebuilding(xn, yn, 5);
                                Setpoint(api, m[0], m[1], xn, yn, map);
                            }
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 6);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    else {
                        if (dangerbuilding != 3) {
                            int* m = Insidebuilding(xn, yn, 3);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                        else {
                            int* m = Insidebuilding(xn, yn, 5);
                            Setpoint(api, m[0], m[1], xn, yn, map);
                        }
                    }
                }
            }
            if ((self->dangerAlert>=2.7) || api.HaveView(Tri[0]->x, Tri[0]->y))
            {
                for (i = 0; i <= 3; i++) api.SendMessage(i, fmt::format("Attention"));
                api.EndAllAction();
            }
        }
    }
    // 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    //  公共操作
}

double t1;
int timegrass;

void AI::play(ITrickerAPI& api)
{
    int i,x,y,time=0;
    auto map = api.GetFullMap();
    auto self = api.GetSelfInfo();
    int xn = api.GridToCell(self->x), yn = api.GridToCell(self->y);
    auto student = api.GetStudents();
    int classnum[10] = { api.GetClassroomProgress(19, 6) ,
        api.GetClassroomProgress(31, 8) ,
        api.GetClassroomProgress(23, 19),
        api.GetClassroomProgress(20, 42),
        api.GetClassroomProgress(11, 39),
        api.GetClassroomProgress(9, 32),
        api.GetClassroomProgress(45, 33),
        api.GetClassroomProgress(41, 13),
        api.GetClassroomProgress(34, 41),
        api.GetClassroomProgress(29, 32)
    };
    static THUAI6::PropType p[4];
    int a = Buildingnum(xn, yn);
    for (i = 0; i <= 2; i++)
        if (classnum[i] == 100)time++;
    if (time == 3)Stu[0] = 1;
    time = 0;
    for (i = 3; i <= 5; i++)
        if (classnum[i] == 100)time++;
    if (time == 3)Stu[1] = 1;
    time = 0;
    for (i = 6; i <= 9; i++)
        if (classnum[i] == 100)time++;
    if (time == 4)Stu[2] = 1;
    time = 0;
    if (THUAI6::playerStateDict[self->playerState] == "Stunned")
        if (p[3] != (enum THUAI6::PropType)0) {
            api.UseProp(p[3]); p[0] = (enum THUAI6::PropType)0;
        }
    for(i=0;i<=3;i++)
        if (api.HaveView(student[i]->x, student[i]->y)) {
            if (THUAI6::playerStateDict[student[i]->playerState] != "Addicted")
            {
                xa = 0; ya = 0;
                api.EndAllAction();
                if (p[0] != (enum THUAI6::PropType)0) {
                    api.UseProp(p[0]); p[0] = (enum THUAI6::PropType)0;
                }
                if (self->timeUntilSkillAvailable[0] == 0)api.UseSkill(0);
                if (Buildingnum(api.GridToCell(student[i]->x), api.GridToCell(student[i]->y)) != a)
                    Setpoint(api, api.GridToCell(student[i]->x), api.GridToCell(student[i]->y), xn, yn, map);
                else {
                    if (self->x < student[i]->x) {
                        api.Move(50, atan((student[i]->y - self->y) / (student[i]->x - self->x)));
                        if ((student[i]->y - self->y) * (student[i]->y - self->y) + (student[i]->x - self->x) * (student[i]->x - self->x) <= 1800 * 1800)
                            api.Attack(atan((student[i]->y - self->y) / (student[i]->x - self->x)));
                    }
                    else if (self->x > student[i]->x) {
                        api.Move(50, acos(-1) + atan((student[i]->y - self->y) / (student[i]->x - self->x)));
                        if ((student[i]->y - self->y) * (student[i]->y - self->y) + (student[i]->x - self->x) * (student[i]->x - self->x) <= 1800 * 1800)
                            api.Attack(acos(-1) + atan((student[i]->y - self->y) / (student[i]->x - self->x)));
                    }
                    else if (self->y > student[i]->y) {
                        api.Move(50, asin(-1));
                        if ((student[i]->y - self->y) * (student[i]->y - self->y) + (student[i]->x - self->x) * (student[i]->x - self->x) <= 1800 * 1800)
                            api.Attack(asin(-1));
                    }
                    else {
                        api.Move(50, asin(1));
                        if ((student[i]->y - self->y) * (student[i]->y - self->y) + (student[i]->x - self->x) * (student[i]->x - self->x) <= 1800 * 1800)
                            api.Attack(asin(1));
                    }break;
                }
            }
           else {
                if (THUAI6::placeTypeDict[map[xn][yn]] != "Grass")
                    for (i = 1; i <= 3; i++)
                    {
                        for (x = xn - i; x <= xn + i; x++)
                            if (x >= 2 && x <= 49)
                            {
                                if (yn - i >= 2)
                                    if (THUAI6::placeTypeDict[map[x][yn - i]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn - i)))Setpoint(api, x, yn - i, xn, yn, map);
                                if (yn + i <= 49)
                                    if (THUAI6::placeTypeDict[map[x][yn + i]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn + i)))Setpoint(api, x, yn + i, xn, yn, map);
                            }
                        for (y = yn - i + 1; y <= yn + i - 1; y++)
                            if (y >= 2 && y <= 49)
                            {
                                if (xn - i >= 2)
                                    if (THUAI6::placeTypeDict[map[xn - i][y]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn - i)))Setpoint(api, xn - i, y, xn, yn, map);
                                if (xn - i <= 49)
                                    if (THUAI6::placeTypeDict[map[xn + i][y]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn + i)))Setpoint(api, xn + i, y, xn, yn, map);
                            }
                    }
                else api.EndAllAction();
            }
        }
    if (xa * ya != 0)Setpoint(api, xa, ya, xn, yn, map);
    else {
        if (self->classVolume >= 2.7)
        {
            if ((a == 3) && (Stu[0] == 0))
            {
                if ((classnum[0] != 0) && (classnum[0] != 100))
                {
                    if (xn == 20 && yn == 6)api.Attack(acos(-1));
                    else Setpoint(api, 20, 6, xn, yn, map);
                }
                else if ((classnum[1] != 0) && (classnum[1] != 100))
                {
                    if (xn == 31 && yn == 7)api.Attack(asin(1));
                    else Setpoint(api, 31, 7, xn, yn, map);
                }
                else if ((classnum[2] != 0) && (classnum[2] != 100))
                {
                    if (xn == 23 && yn == 18)api.Attack(asin(1));
                    else Setpoint(api, 23, 18, xn, yn, map);
                }
                else if (Stu[1] == 0) {
                    int* m = Buildingtobuildingtw(3, 5);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(3, 6);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
            else if ((a == 5) && (Stu[1] == 0))
            {
                if ((classnum[3] != 0) && (classnum[3] != 100))
                {
                    if (xn == 19 && yn == 42)api.Attack(acos(1));
                    else Setpoint(api, 19, 42, xn, yn, map);
                }
                else if ((classnum[4] != 0) && (classnum[4] != 100))
                {
                    if (xn == 11 && yn == 38)api.Attack(asin(1));
                    else Setpoint(api, 11, 38, xn, yn, map);
                }
                else if ((classnum[5] != 0) && (classnum[5] != 100))
                {
                    if (xn == 9 && yn == 33)api.Attack(asin(-1));
                    else Setpoint(api, 9, 33, xn, yn, map);
                }
                else if (Stu[2] == 0) {
                    int* m = Buildingtobuildingtw(5, 6);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(5, 3);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
            else if ((a == 6) && (Stu[2] == 0))
            {
                if ((classnum[6] != 0) && (classnum[6] != 100))
                {
                    if (xn == 45 && yn == 34)api.Attack(asin(-1));
                    else Setpoint(api, 45, 34, xn, yn, map);
                }
                else if ((classnum[7] != 0) && (classnum[7] != 100))
                {
                    if (xn == 41 && yn == 14)api.Attack(asin(-1));
                    else Setpoint(api, 41, 14, xn, yn, map);
                }
                else if ((classnum[8] != 0) && (classnum[8] != 100))
                {
                    if (xn == 34 && yn == 40)api.Attack(asin(1));
                    else Setpoint(api, 34, 40, xn, yn, map);
                }
                else if ((classnum[9] != 0) && (classnum[9] != 100))
                {
                    if (xn == 29 && yn == 33)api.Attack(asin(-1));
                    else Setpoint(api, 29, 33, xn, yn, map);
                }
                else if (Stu[0] == 0) {
                    int* m = Buildingtobuildingtw(6, 3);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(6, 5);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
        }
        else if (self->trickDesire >= 2.7) {
            if (timegrass == 0)
            {
                if (THUAI6::placeTypeDict[map[xn][yn]] != "Grass")
                    for (i = 1; i <= 3; i++)
                    {
                        for (x = xn - i; x <= xn + i; x++)
                            if (x >= 2 && x <= 49)
                            {
                                if (yn - i >= 2)
                                    if (THUAI6::placeTypeDict[map[x][yn - i]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn - i)))Setpoint(api, x, yn - i, xn, yn, map);
                                if (yn + i <= 49)
                                    if (THUAI6::placeTypeDict[map[x][yn + i]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn + i)))Setpoint(api, x, yn + i, xn, yn, map);
                            }
                        for (y = yn - i + 1; y <= yn + i - 1; y++)
                            if (y >= 2 && y <= 49)
                            {
                                if (xn - i >= 2)
                                    if (THUAI6::placeTypeDict[map[xn - i][y]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn - i)))Setpoint(api, xn - i, y, xn, yn, map);
                                if (xn - i <= 49)
                                    if (THUAI6::placeTypeDict[map[xn + i][y]] == "Grass")
                                        if (api.HaveView(api.CellToGrid(x), api.CellToGrid(yn + i)))Setpoint(api, xn + i, y, xn, yn, map);
                            }
                    }
                else timegrass = 1;
            }
        }
        else timegrass = 0;
        if(THUAI6::playerStateDict[self->playerState]=="Idel")
        {
            if ((a == 3) && (Stu[0] == 0))
            {
                if ((classnum[0] != 0) && (classnum[0] != 100))
                {
                    if (xn == 20 && yn == 6)api.Attack(acos(-1));
                    else Setpoint(api, 20, 6, xn, yn, map);
                }
                else if ((classnum[1] != 0) && (classnum[1] != 100))
                {
                    if (xn == 31 && yn == 7)api.Attack(asin(1));
                    else Setpoint(api, 31, 7, xn, yn, map);
                }
                else if ((classnum[2] != 0) && (classnum[2] != 100))
                {
                    if (xn == 23 && yn == 18)api.Attack(asin(1));
                    else Setpoint(api, 23, 18, xn, yn, map);
                }
                else if (Stu[1] == 0) {
                    int* m = Buildingtobuildingtw(3, 5);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(3, 6);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
            else if ((a == 5) && (Stu[1] == 0))
            {
                if ((classnum[3] != 0) && (classnum[3] != 100))
                {
                    if (xn == 19 && yn == 42)api.Attack(acos(1));
                    else Setpoint(api, 19, 42, xn, yn, map);
                }
                else if ((classnum[4] != 0) && (classnum[4] != 100))
                {
                    if (xn == 11 && yn == 38)api.Attack(asin(1));
                    else Setpoint(api, 11, 38, xn, yn, map);
                }
                else if ((classnum[5] != 0) && (classnum[5] != 100))
                {
                    if (xn == 9 && yn == 33)api.Attack(asin(-1));
                    else Setpoint(api, 9, 33, xn, yn, map);
                }
                else if (Stu[2] == 0) {
                    int* m = Buildingtobuildingtw(5, 6);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(5, 3);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
            else if ((a == 6) && (Stu[2] == 0))
            {
                if ((classnum[6] != 0) && (classnum[6] != 100))
                {
                    if (xn == 45 && yn == 34)api.Attack(asin(-1));
                    else Setpoint(api, 45, 34, xn, yn, map);
                }
                else if ((classnum[7] != 0) && (classnum[7] != 100))
                {
                    if (xn == 41 && yn == 14)api.Attack(asin(-1));
                    else Setpoint(api, 41, 14, xn, yn, map);
                }
                else if ((classnum[8] != 0) && (classnum[8] != 100))
                {
                    if (xn == 34 && yn == 40)api.Attack(asin(1));
                    else Setpoint(api, 34, 40, xn, yn, map);
                }
                else if ((classnum[9] != 0) && (classnum[9] != 100))
                {
                    if (xn == 29 && yn == 33)api.Attack(asin(-1));
                    else Setpoint(api, 29, 33, xn, yn, map);
                }
                else if (Stu[0] == 0) {
                    int* m = Buildingtobuildingtw(6, 3);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
                else {
                    int* m = Buildingtobuildingtw(6, 5);
                    Setpoint(api, m[0], m[1], xn, yn, map);
                }
            }
            if (THUAI6::propTypeDict[p[3]] == "Key3") {
                Setpoint(api, door3[0][0], door3[0][1], xn, yn, map);
                if (api.IsDoorOpen(13, 14))api.CloseDoor();
                Setpoint(api, door3[1][0], door3[1][1], xn, yn, map);
                if (api.IsDoorOpen(29, 15)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
            }
            else  if (THUAI6::propTypeDict[p[3]] == "Key5")
            {
                Setpoint(api, door5[0][0], door5[0][1], xn, yn, map);
                if (api.IsDoorOpen(7, 38))api.CloseDoor();
                Setpoint(api, door5[1][0], door5[1][1], xn, yn, map);
                if (api.IsDoorOpen(20, 28)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
            }
            else  if (THUAI6::propTypeDict[p[3]] == "Key5")
            {
                Setpoint(api, door6[0][0], door6[0][1], xn, yn, map);
                if (api.IsDoorOpen(37, 17))api.CloseDoor();
                Setpoint(api, door6[1][0], door6[1][1], xn, yn, map);
                if (api.IsDoorOpen(29, 40))api.CloseDoor();
                Setpoint(api, door6[2][0], door6[2][1], xn, yn, map);
                if (api.IsDoorOpen(43, 36)) { api.CloseDoor(); p[3] = (enum THUAI6::PropType)0; }
            }
            for (i = 0; i <= 7; i++)
            {
                gridChe[i][0] = api.CellToGrid(Che[i][0]);
                gridChe[i][1] = api.CellToGrid(Che[i][1]);
                if (api.HaveView(gridChe[i][0], gridChe[i][1]))
                    if (api.GetChestProgress(Che[i][0], Che[i][1]) != 100)
                    {
                        if (((Che[i][0] == xn) && ((Che[i][1] - yn == 1) || (Che[i][1] - yn == -1))) || ((Che[i][1] == yn) && ((Che[i][0] - xn == 1) || (Che[i][0] - xn == -1))))
                            api.StartOpenChest();
                        else  Setpoint(api, Che[i][0], Che[i][1], xn, yn, map); break;
                    }
            }
            auto pro = api.GetProps();
            if (api.HaveView(pro[0]->x, pro[0]->y)) {
                for (i = 0; i <= 1; i++)
                    api.PickProp(pro[i]->type);
                for (i = 0; i <= 1; i++)
                    if (THUAI6::propTypeDict[pro[i]->type] == "Key3")p[2] = pro[i]->type;
                    else if (THUAI6::propTypeDict[pro[i]->type] == "Key5")p[2] = pro[i]->type;
                    else if (THUAI6::propTypeDict[pro[i]->type] == "Key6")p[2] = pro[i]->type;
                    else if (THUAI6::propTypeDict[pro[i]->type] == "AddSpeed")p[0]=pro[i]->type;
                    else if (THUAI6::propTypeDict[pro[i]->type] == "AddLifeOrClairaudience")
                    {
                        api.UseProp(pro[i]->type);
                        student = api.GetStudents();
                        int min = 100000;
                        for (i = 0; i <= 3; i++)
                            if (student[i]->y - self->y + student[i]->x - self->y < min)
                            {
                                min = student[i]->y - self->y + student[i]->x - self->y;
                                xa = api.GridToCell(student[i]->x);
                                ya = api.GridToCell(student[i]->y);
                            }
                    }
                    else if (THUAI6::propTypeDict[pro[i]->type] == "RecoveryFromDizziness")p[3] = pro[i]->type;
                    else p[0] = pro[i]->type;
            }
        }

    }
}
