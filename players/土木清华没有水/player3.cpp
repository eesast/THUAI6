#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数
bool move = false;
int place[2] = { 2,2 };
//全局变量，坐标
long long abs_X = 0;
long long abs_Y = 0;
//卡位微调参量
int Turn_little = 0;
bool Turn_can = 0;
bool Turn_flag = 1;
int Y_change = 0;
void GetTo(int32_t X, int32_t Y, IStudentAPI& api)//X,Y为格子坐标，api直接放api即可
{
    //初始化参量
    bool Y_can_go = 1, X_can_go = 1;
    bool Down_Player;//下移参量，判断是否下移
    bool Up_Player;//上移参量，判断是否上移
    bool Left_Player;//左移参量
    bool Right_Player;//右移参量
    bool Go_Go_X = 1;//卡位参量 0卡位，1不卡位
    bool Go_Go_Y = 1;//卡位参量 0卡位，1不卡位
    auto Selfinfo = api.GetSelfInfo();//获取人物信息
    auto X_player = api.GridToCell(Selfinfo->x);//得到人物坐标X(格子)
    auto Y_player = api.GridToCell(Selfinfo->y);//得到人物坐标Y(格子)
    //获得人物周边信息
    auto Placetype_LU = api.GetPlaceType(X_player - 1, Y_player - 1);
    auto Placetype_L0 = api.GetPlaceType(X_player, Y_player - 1);
    auto Placetype_LD = api.GetPlaceType(X_player + 1, Y_player - 1);
    auto Placetype_0U = api.GetPlaceType(X_player - 1, Y_player);
    auto Placetype_00 = api.GetPlaceType(X_player, Y_player);
    auto Placetype_0D = api.GetPlaceType(X_player + 1, Y_player);
    auto Placetype_RU = api.GetPlaceType(X_player - 1, Y_player + 1);
    auto Placetype_R0 = api.GetPlaceType(X_player, Y_player + 1);
    auto Placetype_RD = api.GetPlaceType(X_player + 1, Y_player + 1);
    //判断上下左右是否可走
    int Left_can_go = (Placetype_L0 == THUAI6::PlaceType::Land || Placetype_L0 == THUAI6::PlaceType::Grass);
    int Right_can_go = (Placetype_R0 == THUAI6::PlaceType::Land || Placetype_R0 == THUAI6::PlaceType::Grass);
    int Up_can_go = (Placetype_0U == THUAI6::PlaceType::Land || Placetype_0U == THUAI6::PlaceType::Grass);
    int Down_can_go = (Placetype_0D == THUAI6::PlaceType::Land || Placetype_0D == THUAI6::PlaceType::Grass);
    //判断是否卡位
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }
    //判断朝向
    if (X > X_player)//Down
    {
        Down_Player = true;
    }
    else Down_Player = false;
    if (X < X_player)//Up
    {
        Up_Player = true;
    }
    else Up_Player = false;
    if (Y < Y_player)//Left
    {
        Left_Player = true;
    }
    else Left_Player = false;
    if (Y < Y_player)//Right
    {
        Right_Player = true;
    }
    else Right_Player = false;
    //先保证X对齐
    //可走则走
    if (Down_Player == true && Down_can_go && Go_Go_X)
    {
        api.MoveDown(10);
    }
    if (Up_Player == true && Up_can_go && Go_Go_X)
    {
        api.MoveUp(10);
    }
    //判断是否卡位
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }
    //可走但是卡位，则微调，需要每次检查是否仍卡位
    if (Down_Player == true && Down_can_go)
    {
        if ((Go_Go_X == 0) && Turn_can == 0 && Turn_flag == 1)
        {
            api.MoveLeft(3);
            Turn_little += 1;
        }
        else;
        if (Turn_little >= 15) Turn_flag = 0;
        else;
        if ((Go_Go_X == 0) && Turn_flag == 1 && Turn_can == 0)
        {
            api.MoveRight(3);
        }
        else;
    }
    else;
    if (Up_Player == true && Up_can_go)
    {
        if ((Go_Go_X == 0) && Turn_can == 0 && Turn_flag == 1)
        {
            api.MoveLeft(3);
            Turn_little += 1;
        }
        else;
        if (Turn_little >= 5) Turn_flag = 1;
        else;
        if ((Go_Go_X == 0) && Turn_flag == 0 && Turn_can == 0)
        {
            api.MoveRight(3);
        }
        else;
    }
    else;
    if (Go_Go_X == 1)
    {
        Turn_little = 0;
        Turn_can = 0;
        Turn_flag = 1;
    }
    //判断是否卡位
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }
    //若X对齐时竖直受阻，则调整Y参量，直到可以继续对齐
    //若是不可走，则水平调整，直到可走，先朝向Y调整，直到可走或者卡位，可走则继续，卡位则反向调整
    if (Down_Player == 1 && !Down_can_go && Go_Go_X)
    {
        if (Right_Player && Right_can_go)
        {
            api.MoveRight(20);
        }
        else if (Left_Player && Left_can_go)
        {
            api.MoveLeft(20);
        }
        else if (Left_can_go)
        {
            api.MoveLeft(20);
        }
        else if (Right_can_go)
        {
            api.MoveRight(20);
        }
        else Y_can_go = 0;
    }
    //若反向调整仍不行，则向原X方向反向移动，再进行上述相同的调整，直到可以继续对齐
    if (!Y_can_go)
    {
        api.MoveUp(100);
    }
    else;
    //
    //Y方向在X对齐后仍采用相同的方法，但是调整后成功Y对齐后需要进行再一次X对齐，反复循环直到对齐
    // 
    //
    //
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }

    //可走则走
    if (Right_Player == true && Right_can_go && Go_Go_Y)
    {
        api.MoveRight(10);
    }
    if (Left_Player == true && Left_can_go && Go_Go_Y)
    {
        api.MoveLeft(10);
    }
    //判断是否卡位
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }
    //可走但是卡位，则微调，需要每次检查是否仍卡位
    if (Right_Player == true && Right_can_go)
    {
        if ((Go_Go_Y == 0) && Turn_can == 0 && Turn_flag == 1)
        {
            api.MoveUp(3);
            Turn_little += 1;
        }
        else;
        if (Turn_little >= 15) Turn_flag = 0;
        else;
        if ((Go_Go_X == 0) && Turn_flag == 1 && Turn_can == 0)
        {
            api.MoveDown(3);
        }
        else;
    }
    else;
    if (Left_Player == true && Left_can_go)
    {
        if ((Go_Go_Y == 0) && Turn_can == 0 && Turn_flag == 1)
        {
            api.MoveUp(3);
            Turn_little += 1;
        }
        else;
        if (Turn_little >= 5) Turn_flag = 1;
        else;
        if ((Go_Go_Y == 0) && Turn_flag == 0 && Turn_can == 0)
        {
            api.MoveDown(3);
        }
        else;
    }
    else;
    if (Go_Go_Y == 1)
    {
        Turn_little = 0;
        Turn_can = 0;
        Turn_flag = 1;
    }
    //判断是否卡位
    Selfinfo = api.GetSelfInfo();
    if ((abs_X == Selfinfo->x) && (abs_Y == Selfinfo->y))
    {
        Go_Go_X = 0;
        Go_Go_Y = 0;//卡位
    }
    else
    {
        Go_Go_X = 1;
        Go_Go_Y = 1;//未卡
    }
    //若X对齐时竖直受阻，则调整Y参量，直到可以继续对齐
    //若是不可走，则水平调整，直到可走，先朝向Y调整，直到可走或者卡位，可走则继续，卡位则反向调整
    if (Right_Player == 1 && !Right_can_go && Go_Go_Y)
    {
        if (Down_Player && Down_can_go)
        {
            api.MoveDown(20);
        }
        else if (Up_Player && Up_can_go)
        {
            api.MoveUp(20);
        }
        else if (Up_can_go)
        {
            api.MoveUp(20);
        }
        else if (Down_can_go)
        {
            api.MoveDown(20);
        }
        else X_can_go = 0;
    }
    //若反向调整仍不行，则向原X方向反向移动，再进行上述相同的调整，直到可以继续对齐
    if (!X_can_go)
    {
        api.MoveUp(100);
    }
    else;







    //保留最后坐标
    auto Selfinfo_Last = api.GetSelfInfo();
    abs_X = Selfinfo_Last->x;
    abs_Y = Selfinfo_Last->y;

}


void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        GetTo(30, 30, api);




        
        
        
        
        
        
        
        
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        GetTo(10, 10, api);
        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        auto mapInfo = api.GetFullMap();
        auto SelfInfo = api.GetSelfInfo();
        auto cellx = api.GridToCell(SelfInfo->x);
        auto celly = api.GridToCell(SelfInfo->y);
        int flash = 0;
        int i = cellx, j = celly;
        if (place[0] == 2 )
        {
            for (flash = 0; i < (cellx + 10); i = i + 1)
            {

                for (; j < (celly + 10); j = j + 1)
                {
                    if (mapInfo[i][j] == THUAI6::PlaceType::Chest)
                    {
                        flash = 1;
                        break;
                    }
                }
                if (flash == 1) break;
            }
            if (flash == 1)
            {
                place[0] = i; place[1] = j;
            }
        }
        else if(1)
        {double angle = atan2(place[1] - celly, place[0] - cellx);
        api.Move(50, angle); 
        }
        else 
    {
        if (api.GetChestProgress(place[0], place[1]) < 10000000)
        {
            api.StartOpenChest();
        }
        else
        {
            auto PropInfo = api.GetProps();
            api.PickProp(PropInfo[0]->type);
            api.PickProp(PropInfo[1]->type);
        }
    }



        
        
        
        
        
        
        
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        GetTo(20, 20, api);
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

