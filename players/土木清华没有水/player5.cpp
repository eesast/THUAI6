#include <vector>
#include <thread>
#include <array>
#include <chrono>
#include <cmath>
#include "AI.h"
#include "constants.h"

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}

int PatrolMode = 0;

bool Passable(THUAI6::PlaceType a)
{
    if (a == THUAI6::PlaceType::Land)return true;
    else if (a == THUAI6::PlaceType::Grass)return true;
    else return false;
}
void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    
    auto AllMap = api.GetFullMap();
    
    
        int NumberOfAims = 0;
        int x = api.GridToCell(self->x);
        int y = api.GridToCell(self->y);
        bool CenterLeft = self->y % 1000 <= 500;
        bool CenterUp = self->x % 1000 <= 500;
        
        auto Aims = api.GetStudents();
        auto Aims_location = Aims.begin();
        if(Aims.size() != 0)
            for(;Aims_location!=Aims.end();++Aims_location)
            {
                if ((*Aims_location)->playerState != THUAI6::PlayerState::Addicted)NumberOfAims++;

            }
        Aims_location = Aims.begin();
        if (NumberOfAims!=0)
        {
            
            int Aims_x = api.GridToCell((*Aims_location)->x);
            int Aims_y = api.GridToCell((*Aims_location)->y);
            double relative_x = (*Aims_location)->x - self->x;
            double relative_y = (*Aims_location)->y - self->y;
            double Aims_angle = atan2(relative_y,relative_x);
            api.Move(20, Aims_angle);
            if (relative_x*relative_x+relative_y*relative_y<=4000000&&(*Aims_location)->playerState!=THUAI6::PlayerState::Addicted)
            {
                api.Attack(Aims_angle);
            }
            //else if ((*Aims_location)->playerState == THUAI6::PlayerState::Addicted && Aims_location != Aims.end())
           // {
          //      Aims_location++;
           // }
            //else if((*Aims_location)->playerState == THUAI6::PlayerState::Addicted)
        }
        else
        {
            
            switch (PatrolMode)
            {
            case 0:
                if (Passable(api.GetPlaceType(x + 1, y)) && Passable(api.GetPlaceType(x + 1, y + 1 - CenterLeft * 2)))
                {
                    api.MoveDown(20);
                }
                else
                {
                    PatrolMode++;
                }
                break;
            case 1:
                if (Passable(api.GetPlaceType(x, y + 1)) && Passable(api.GetPlaceType(x + 1 - CenterUp * 2, y + 1 )) )
                {
                    api.MoveRight(20);
                }
                else
                {
                    PatrolMode++;
                }
                break;
            case 2:
                if (Passable(api.GetPlaceType(x-1, y )) && Passable(api.GetPlaceType(x - 1, y + 1 - CenterLeft * 2)) )
                {
                    api.MoveUp(20);
                }
                else
                {
                    PatrolMode++;
                }
                break;
            case 3:
                if (Passable(api.GetPlaceType(x, y - 1)) && Passable(api.GetPlaceType(x + 1 - CenterUp * 2, y - 1)))
                {
                    api.MoveLeft(20);
                }
                else
                {
                    PatrolMode++;
                }
                break;
            default:
                PatrolMode = 0;
                break;
            }
            if (PatrolMode == 4)PatrolMode =0;

            
            
            
            
            
        }
        
        /*NullPlaceType = 0,
        Land = 1,
        Wall = 2,
        Grass = 3,
        ClassRoom = 4,
        Gate = 5,
        HiddenGate = 6,
        Window = 7,
        Door3 = 8,
        Door5 = 9,
        Door6 = 10,
        Chest = 11,*/
   
}
