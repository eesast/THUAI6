#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <logic.h>


// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数
double LandCount = 1.0, GrassCount = 0.8, WindowCount = 0.2;
int TempMap[50][50] = { 0 };//寻路过程中使用的临时地图，每次用后要清理一遍。若数值为零则该格子表示可以通行。
int GrassMap[50][50] = { 0 };//草地寻路过程中的地图，0代表非草地，1代表草地
double TempMapCount[50][50] = { 9999.0 };//TempMapCount标记每个可以通过的格子目前到出发点的距离权重
int STempPlaceX[5];//存储0~3号学生的X坐标，单位是Cell
int STempPlaceY[5];//存储0~3号学生的Y坐标，单位是Cell
int SAttemptX[5];//存储0~3学生要去格子的X坐标，单位是Cell
int SAttemptY[5];//存储0~3学生要去格子的Y坐标，单位是Cell
int STPX[4][5];//即每个个体两个定位点的X坐标，前为四角，后为玩家序号，单位是Cell
int STPXC[5];//与上一个相对是中心点X坐标，单位是Cell
int STPY[4][5];//即每个个体两个定位点的Y坐标，前为四角，后为玩家序号，单位是Cell
int STPYC[5];//与上一个相对是中心点Y坐标，单位是Cell
int SNextX[5];//存储0~3学生下一个落脚点的X坐标，单位是Cell
int SNextY[5];//存储0~3学生下一个落脚点的Y坐标，单位是Cell
double LessSteps, CurrentSteps = 0.0;//分别标记目前所有可行路径中步数最少的方案步数和当前实时的步数
int CurrentChoice[5] = { 1 };//（递归用）最少步数和当前步数
int BestChoice;//（递归用）当前选择
int NextPlaceType[5];//下一个前进方向格子种类,"0"代表陆地，“3”代表窗户
int NowAction = -1;//目前正在进行的是玩家几号的代码
int NowTrialDirect = 0;//目前的第一步，在firststep()函数中是每一步要换的，0、1、2、3分别代表上右下左
int MoveDuring = 40, AdjustmentDuring = 25;
int const HalfBall = 401;

int SMission[5] = { 0 };//学生当前任务，-1代表寻找教室（仅一瞬间），0代表前往目标点，1代表暂停在原地（学习，开门），2代表逃脱捣蛋鬼
int Escaping=0;

bool GameStartFlag = false;//是否已经进行每局游戏一次的Special check
int TotalDoneClass = 0;//已经完成教室总数
int ScanClass[10] = { 0 };//已经完成的教室记为1
int ScanClassX[10] = { 0 };
int ScanClassY[10] = { 0 };
int TargetClass[4] = { 0 };
int TargetGate[4] = { 0 };
int ScanGate[2] = { 0 };
int ScanGateX[2] = { 0 };
int ScanGateY[2] = { 0 };
int GateOpen[3] = { 0 };
int MissionStorage=237159;

int StartGrad[4] = { 0 };
//此区域定义Students所见Tricker信息
int TrickerType;
int TrickerLastWitness[2];
//监管者



//向前声明区
void trialstep(int f, int g);//向前声明每一步的尝试
void trialstepForRoom(int f, int g);
void trialstepForGrad(int f, int g);
void SCheckTricker(IStudentAPI& api);
void trialstepForGrass(int f, int g);
void trialstepForTRoom(int f, int g);


//逃生者函数区
int NextStep()//判断下一格格式
{
    if (CurrentChoice[NowAction] == 0)
    {
        SNextX[NowAction] = STempPlaceX[NowAction] - 1;
        SNextY[NowAction] = STempPlaceY[NowAction];
    }
    if (CurrentChoice[NowAction] == 1)
    {
        SNextX[NowAction] = STempPlaceX[NowAction];
        SNextY[NowAction] = STempPlaceY[NowAction] + 1;

    }
    if (CurrentChoice[NowAction] == 2)
    {
        SNextX[NowAction] = STempPlaceX[NowAction] + 1;
        SNextY[NowAction] = STempPlaceY[NowAction];
    }
    if (CurrentChoice[NowAction] == 3)
    {
        SNextX[NowAction] = STempPlaceX[NowAction];
        SNextY[NowAction] = STempPlaceY[NowAction] - 1;
    }
    NextPlaceType[NowAction] = TempMap[SNextX[NowAction]][SNextY[NowAction]];
    return 0;
}
int firststep(int f, int g)//寻路的第一步，单独分出来的原因是我们的目的就是下一步的位置
{
    if (f < 0 || f>49 || g < 0 || g>49) {
        return 0;
    }
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                TempMapCount[i][j] = 9999.0;
            }
        }
        NextPlaceType[NowAction] = 3;
        CurrentChoice[NowAction] = 5;//Test
        TempMap[f][g] = 1;
        CurrentSteps = 0.0;//初始化现在步数
        LessSteps = 9999.0;//初始化当前寻路过程的最少步数    
        NowTrialDirect = 0;//尝试向上的所有分支
        trialstep((f - 1), g);
        CurrentSteps = 0.0;
        NowTrialDirect = 1;//尝试向右的所有分支
        trialstep((f), (g + 1));
        CurrentSteps = 0.0;
        NowTrialDirect = 2;//尝试向下的所有分支
        trialstep((f + 1), g);
        TempMap[f][g] = 0;
        NowTrialDirect = 3;//尝试向左的所有分支
        trialstep(f, (g - 1));
        CurrentSteps = 0.0;
        NextStep();
        if (LessSteps < 9999.0) { return CurrentChoice[NowAction]; }
        else return 0;
    }
}
void trialstep(int f, int g)//用于一步步测试，该函数使用了递归
{

    if (f < 0 || f>49 || g < 0 || g>49) { return; }//如果越界，直接结束该分支
    else
    {
        if (TempMap[f][g] == 1 || TempMap[f][g] == 4) {}//如果临时地图TempMap上（f，g）不可移动：直接结束该分支
        else//说明这边至少是可以走的，进行下一步尝试
        {
            if ((CurrentSteps + LandCount) < TempMapCount[f][g])
            {
                CurrentSteps += LandCount;
                TempMapCount[f][g] = CurrentSteps;
                if (f == SAttemptX[NowAction] && g == SAttemptY[NowAction])//原checkstep函数改换至此
                {
                    if (CurrentSteps < LessSteps) { LessSteps = CurrentSteps; CurrentChoice[NowAction] = NowTrialDirect; }
                }
                else
                {
                    TempMap[f][g] ++;
                    trialstep((f - 1), g);
                    trialstep((f), (g + 1));
                    trialstep((f + 1), g);
                    trialstep(f, (g - 1));
                    TempMap[f][g] --;
                }
                CurrentSteps -= LandCount;
            }
        }
    }
    return;
}
void LandMove(IStudentAPI& api)
{
    if ((STempPlaceX[NowAction] != SAttemptX[NowAction]) || (STempPlaceY[NowAction] != SAttemptY[NowAction]))
    {
        if (CurrentChoice[NowAction] == 0) { api.Move(MoveDuring, 3.14); }
        if (CurrentChoice[NowAction] == 1) { api.Move(MoveDuring, 1.57); }
        if (CurrentChoice[NowAction] == 2) { api.Move(MoveDuring, 0); }
        if (CurrentChoice[NowAction] == 3) { api.Move(MoveDuring, 4.71); }
    }
}

void BallLocate(IStudentAPI& api)
{
    STPXC[NowAction] = api.GridToCell(api.GetSelfInfo()->x);
    STPYC[NowAction] = api.GridToCell(api.GetSelfInfo()->y);
    STPX[0][NowAction] = api.GridToCell(api.GetSelfInfo()->x - HalfBall);//0号左上
    STPY[0][NowAction] = api.GridToCell(api.GetSelfInfo()->y - HalfBall);
    STPX[1][NowAction] = api.GridToCell(api.GetSelfInfo()->x + HalfBall);//1号右下
    STPY[1][NowAction] = api.GridToCell(api.GetSelfInfo()->y + HalfBall);
    STPX[2][NowAction] = api.GridToCell(api.GetSelfInfo()->x - HalfBall);//2号右上
    STPY[2][NowAction] = api.GridToCell(api.GetSelfInfo()->y + HalfBall);
    STPX[3][NowAction] = api.GridToCell(api.GetSelfInfo()->x + HalfBall);//3号左下
    STPY[3][NowAction] = api.GridToCell(api.GetSelfInfo()->y - HalfBall);
}
void RoadDirectionMove(IStudentAPI& api)//寻路系统
{
    BallLocate(api);
    if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.785);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststep((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }

    //else {LandMove(api); }//For Debug
}//寻路

void GetStudentInSight(IStudentAPI& api)
{
    int NumID = 0;
    for (int i = 0; i < 4; i++)
    {
        NumID = api.GetStudents()[i]->playerID;
        int TempX = api.GetStudents()[i]->x;
        int TempY = api.GetStudents()[i]->y;
        STPXC[NumID] = api.GridToCell(TempX);
        STPYC[NumID] = api.GridToCell(TempY);
        STPX[0][NumID] = api.GridToCell(TempX - HalfBall);
        STPY[0][NumID] = api.GridToCell(TempY - HalfBall);
        STPX[1][NumID] = api.GridToCell(TempX + HalfBall);
        STPY[1][NumID] = api.GridToCell(TempY + HalfBall);
        STPX[2][NumID] = api.GridToCell(TempX - HalfBall);
        STPY[2][NumID] = api.GridToCell(TempY + HalfBall);
        STPX[3][NumID] = api.GridToCell(TempX + HalfBall);
        STPY[3][NumID] = api.GridToCell(TempY - HalfBall);
    }
}

int firststepForRoom(int f, int g)//寻路的第一步，单独分出来的原因是我们的目的就是下一步的位置
{
    if (f < 0 || f>49 || g < 0 || g>49) {
        return 0;
    }
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                TempMapCount[i][j] = 9999.0;
            }
        }
        NextPlaceType[NowAction] = 3;
        CurrentChoice[NowAction] = 5;//Test
        TempMap[f][g] = 1;
        CurrentSteps = 0.0;//初始化现在步数
        LessSteps = 9999.0;//初始化当前寻路过程的最少步数    
        NowTrialDirect = 0;//尝试向上的所有分支
        trialstepForRoom((f - 1), g);
        CurrentSteps = 0.0;
        NowTrialDirect = 1;//尝试向右的所有分支
        trialstepForRoom((f), (g + 1));
        CurrentSteps = 0.0;
        NowTrialDirect = 2;//尝试向下的所有分支
        trialstepForRoom((f + 1), g);
        TempMap[f][g] = 0;
        NowTrialDirect = 3;//尝试向左的所有分支
        trialstepForRoom(f, (g - 1));
        CurrentSteps = 0.0;
        NextStep();
        if (LessSteps < 9999.0) { return CurrentChoice[NowAction]; }
        else return 0;
    }
}
void trialstepForRoom(int f, int g)//用于一步步测试，该函数使用了递归
{
    int FinalFlag = 0;
    if (f < 0 || f>49 || g < 0 || g>49) { return; }//如果越界，直接结束该分支
    else
    {
        if (TempMap[f][g] == 1 || TempMap[f][g] == 4) {}//如果临时地图TempMap上（f，g）不可移动：直接结束该分支
        else//说明这边至少是可以走的，进行下一步尝试
        {
            if ((CurrentSteps + LandCount) < TempMapCount[f][g])
            {
                CurrentSteps += LandCount;
                TempMapCount[f][g] = CurrentSteps;
                for (int i = 0; i < 10; i++)
                {
                    if (f == ScanClassX[i] && g == ScanClassY[i] && ScanClass[i] == 0)//原checkstep函数改换至此
                    {
                        FinalFlag = 1;
                        if (CurrentSteps < LessSteps)
                        {
                            LessSteps = CurrentSteps;
                            CurrentChoice[NowAction] = NowTrialDirect;
                            TargetClass[NowAction] = i;//这暂时是必要的，因为后期要写这个教室被完成了
                            SAttemptX[NowAction] = f;
                            SAttemptX[NowAction] = g;
                        }
                    }
                }
                if (FinalFlag == 0)
                {
                    TempMap[f][g] ++;
                    trialstepForRoom((f - 1), g);
                    trialstepForRoom((f), (g + 1));
                    trialstepForRoom((f + 1), g);
                    trialstepForRoom(f, (g - 1));
                    TempMap[f][g] --;
                }
                FinalFlag = 0;
                CurrentSteps -= LandCount;
            }
        }
    }
    return;
}
void RoadDirectionMoveForRoom(IStudentAPI& api)//寻路系统
{
    BallLocate(api);
    //if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1 || TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 4)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.786);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1 || TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1 || TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 1 || TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststepForRoom((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }

    //else {LandMove(api); }//For Debug
}//寻路

int firststepForGrass(int f, int g)//寻路的第一步，单独分出来的原因是我们的目的就是下一步的位置
{
    if (f < 0 || f>49 || g < 0 || g>49) {
        return 0;
    }
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                TempMapCount[i][j] = 9999.0;
            }
        }
        NextPlaceType[NowAction] = 3;
        CurrentChoice[NowAction] = 5;//Test
        TempMap[f][g] = 1;
        CurrentSteps = 0.0;//初始化现在步数
        LessSteps = 9999.0;//初始化当前寻路过程的最少步数    
        NowTrialDirect = 0;//尝试向上的所有分支
        trialstepForGrass((f - 1), g);
        CurrentSteps = 0.0;
        NowTrialDirect = 1;//尝试向右的所有分支
        trialstepForGrass((f), (g + 1));
        CurrentSteps = 0.0;
        NowTrialDirect = 2;//尝试向下的所有分支
        trialstepForGrass((f + 1), g);
        TempMap[f][g] = 0;
        NowTrialDirect = 3;//尝试向左的所有分支
        trialstepForGrass(f, (g - 1));
        CurrentSteps = 0.0;
        NextStep();
        if (LessSteps < 9999.0) { return CurrentChoice[NowAction]; }
        else return 0;
    }
}
void trialstepForGrass(int f, int g)//用于一步步测试，该函数使用了递归
{
    int FinalFlag = 0;
    if (f < 0 || f>49 || g < 0 || g>49) { return; }//如果越界，直接结束该分支
    else
    {
        if (TempMap[f][g] == 1 || TempMap[f][g] == 4) {}//如果临时地图TempMap上（f，g）不可移动：直接结束该分支
        else//说明这边至少是可以走的，进行下一步尝试
        {
            if ((CurrentSteps + LandCount) < TempMapCount[f][g])
            {
                CurrentSteps += LandCount;
                TempMapCount[f][g] = CurrentSteps;
                for (int i = 0; i < 10; i++)
                {
                    if (GrassMap[f][g]==1)//原checkstep函数改换至此
                    {
                        FinalFlag = 1;
                        if (CurrentSteps < LessSteps)
                        {
                            LessSteps = CurrentSteps;
                            CurrentChoice[NowAction] = NowTrialDirect;
                            TargetClass[NowAction] = i;//这暂时是必要的，因为后期要写这个教室被完成了
                            SAttemptX[NowAction] = f;
                            SAttemptX[NowAction] = g;
                        }
                    }
                }
                if (FinalFlag == 0)
                {
                    TempMap[f][g] ++;
                    trialstepForGrass((f - 1), g);
                    trialstepForGrass((f), (g + 1));
                    trialstepForGrass((f + 1), g);
                    trialstepForGrass(f, (g - 1));
                    TempMap[f][g] --;
                }
                FinalFlag = 0;
                CurrentSteps -= LandCount;
            }
        }
    }
    return;
}
void RoadDirectionMoveForGrass(IStudentAPI& api)//寻路系统
{
    BallLocate(api);
    //if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1 || TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 4)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.786);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1 || TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1 || TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 1 || TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststepForGrass((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }

    //else {LandMove(api); }//For Debug
}//寻路

int firststepForGrad(int f, int g)//寻路的第一步，单独分出来的原因是我们的目的就是下一步的位置
{
    if (f < 0 || f>49 || g < 0 || g>49) {
        return 0;
    }
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                TempMapCount[i][j] = 9999.0;
            }
        }
        NextPlaceType[NowAction] = 3;
        CurrentChoice[NowAction] = 5;//Test
        TempMap[f][g] = 1;
        CurrentSteps = 0.0;//初始化现在步数
        LessSteps = 9999.0;//初始化当前寻路过程的最少步数    
        NowTrialDirect = 0;//尝试向上的所有分支
        trialstepForGrad((f - 1), g);
        CurrentSteps = 0.0;
        NowTrialDirect = 1;//尝试向右的所有分支
        trialstepForGrad((f), (g + 1));
        CurrentSteps = 0.0;
        NowTrialDirect = 2;//尝试向下的所有分支
        trialstepForGrad((f + 1), g);
        TempMap[f][g] = 0;
        NowTrialDirect = 3;//尝试向左的所有分支
        trialstepForGrad(f, (g - 1));
        CurrentSteps = 0.0;
        NextStep();
        if (LessSteps < 9999.0) { return CurrentChoice[NowAction]; }
        else return 0;
    }
}
void trialstepForGrad(int f, int g)//用于一步步测试，该函数使用了递归
{
    int FinalFlag = 0;
    if (f < 0 || f>49 || g < 0 || g>49) { return; }//如果越界，直接结束该分支
    else
    {
        if (TempMap[f][g] == 1 || TempMap[f][g] == 4) {}//如果临时地图TempMap上（f，g）不可移动：直接结束该分支
        else//说明这边至少是可以走的，进行下一步尝试
        {
            if ((CurrentSteps + LandCount) < TempMapCount[f][g])
            {
                CurrentSteps += LandCount;
                TempMapCount[f][g] = CurrentSteps;
                for (int i = 0; i < 10; i++)
                {
                    if (f == ScanGateX[i] && g == ScanGateY[i] && ScanGate[i] == 0)//原checkstep函数改换至此
                    {
                        FinalFlag = 1;
                        if (CurrentSteps < LessSteps)
                        {
                            LessSteps = CurrentSteps;
                            CurrentChoice[NowAction] = NowTrialDirect;
                            TargetGate[NowAction] = i;//这暂时是必要的，因为后期要写这个Gate被完成了
                            SAttemptX[NowAction] = f;
                            SAttemptX[NowAction] = g;
                        }
                    }
                }
                if (FinalFlag == 0)
                {
                    TempMap[f][g] ++;
                    trialstepForGrad((f - 1), g);
                    trialstepForGrad((f), (g + 1));
                    trialstepForGrad((f + 1), g);
                    trialstepForGrad(f, (g - 1));
                    TempMap[f][g] --;
                }
                FinalFlag = 0;
                CurrentSteps -= LandCount;
            }
        }
    }
    return;
}
void RoadDirectionMoveForGrad(IStudentAPI& api)//寻路系统
{
    BallLocate(api);
    //if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1 || TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 4)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.786);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1 || TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1 || TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 1 || TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststepForGrad((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }

    //else {LandMove(api); }//For Debug
}//寻路

void MapClear(IStudentAPI& api)
{
    GetStudentInSight(api);
    for (int i = 0; i < 50; i++)//设置临时地图
    {
        for (int j = 0; j < 50; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                if (k != NowAction)
                {
                    if (i == STempPlaceX[k] && j == STempPlaceY[k]) { TempMap[i][j] = 4; }
                    if (i == STPX[0][k] && j == STPY[0][k]) { TempMap[i][j] = 4; }
                    if (i == STPX[1][k] && j == STPY[1][k]) { TempMap[i][j] = 4; }
                    if (i == STPX[2][k] && j == STPY[2][k]) { TempMap[i][j] = 4; }
                    if (i == STPX[3][k] && j == STPY[3][k]) { TempMap[i][j] = 4; }
                }
            }
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Wall || api.GetPlaceType(i, j) == THUAI6::PlaceType::Chest || api.GetPlaceType(i, j) == THUAI6::PlaceType::ClassRoom) //（i，j不属于草地也不属于陆地）
            {
                TempMap[i][j] = 1;
            }
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Window) //（i，j）属于窗户
            {
                TempMap[i][j] = 3;
            }
        }
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            TempMapCount[i][j] = 9999.0;
        }
    }
    return;
}
void MapReset()
{
    for (int i = 0; i < 50; i++)//清空临时地图,每个格子都记成0（可以自由行动的）
    {
        for (int j = 0; j < 50; j++)
        {
            TempMap[i][j] = 0;
        }
    }
}

void SpecialMapCheck(IStudentAPI& api)
{
    int TotalNumber1 = 0;
    int TotalNumber2 = 0;
    if (GameStartFlag == true) {}
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::ClassRoom)
                {
                    ScanClassX[TotalNumber1] = i;
                    ScanClassY[TotalNumber1] = j;
                    TotalNumber1++;
                }
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Gate)
                {
                    ScanGateX[TotalNumber2] = i;
                    ScanGateY[TotalNumber2] = j;
                    TotalNumber2++;
                }
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass)
                {
                    GrassMap[i][j] = 1;
                }
            }
        }
    }
    return;
}
void GameStartSettings(IStudentAPI& api)
{
    SpecialMapCheck(api);
    SMission[0] = -1;
    SMission[1] = -1;
    SMission[2] = -1;
    SMission[3] = -1;
    GameStartFlag = true;
}

void MissionCheck(IStudentAPI& api)//随时检查当前任务状态
{
    SCheckTricker(api);
    if (Escaping == 1)
    {
        SMission[NowAction] = 4;
    }
    if(SMission[NowAction] == 4&&GrassMap[STPXC[NowAction]][STPYC[NowAction]]==1)
    {
        SMission[NowAction] = MissionStorage;
        MissionStorage = 237159;
        Escaping = 0;
        printf("Stop Escaping!");
    }
    if (SMission[NowAction] == -1)//正在寻找教室，此时要把教室在临时地图中设置为可以行走，否则寻路无法进行
    {
        for (int i = 0; i < 10; i++)
        {
            if (ScanClass[i] == 0)
            {
                TempMap[ScanClassX[i]][ScanClassY[i]] = 0;
            }
        }
        //if ((abs(api.GetSelfInfo()->x - api.CellToGrid(ScanClassX[TargetClass[NowAction]])) + abs(api.GetSelfInfo()->y - api.CellToGrid(ScanClassX[TargetClass[NowAction]])))<= 2000)
        if (((abs(STPXC[NowAction] - ScanClassX[TargetClass[NowAction]])) + abs(STPYC[NowAction] - ScanClassY[TargetClass[NowAction]])) <= 1)
        {
            SMission[NowAction] = 1;
        }
    }
    if (SMission[NowAction] == -2)//正在寻找Gate，此时要把Gate在临时地图中设置为可以行走，否则寻路无法进行
    {
        for (int i = 0; i < 10; i++)
        {
            if (ScanGate[i] == 0)
            {
                TempMap[ScanGateX[i]][ScanGateY[i]] = 0;
            }
        }
        if (((abs(STPXC[NowAction] - ScanGateX[TargetGate[NowAction]])) + abs(STPYC[NowAction] - ScanGateY[TargetGate[NowAction]])) <= 1)
        {
            SMission[NowAction] = 2;
        }
    }
    if (SMission[NowAction] == 1)//此状态时刻准备开始学习
    {
        if (api.GetSelfInfo()->playerState != THUAI6::PlayerState::Learning)
        {
            api.StartLearning();
        }
        if (api.GetClassroomProgress(ScanClassX[TargetClass[NowAction]], ScanClassY[TargetClass[NowAction]]) == Constants::maxClassroomProgress)
        {
            ScanClass[TargetClass[NowAction]] = 1;
            SMission[NowAction] = -1;
            std::string str1 = "ILoveTHU";
            str1[0] = 0;//代表“0”
            str1[1] = 1;//"1"
            str1[6] = TargetClass[NowAction];
            for (int i = 0; i < 4; i++)
            {
                api.SendTextMessage(i, str1);
            }
        }
    }
    if (SMission[NowAction] == 2)
    {
        if (api.GetSelfInfo()->playerState != THUAI6::PlayerState::OpeningAGate)
        {
            api.StartOpenGate();
        }
        if (api.GetGateProgress(ScanGateX[TargetGate[NowAction]], ScanGateY[TargetGate[NowAction]]) == Constants::maxGateProgress)
        {
            ScanGate[TargetGate[NowAction]] = 1;
            SMission[NowAction] = 3;
            std::string str1 = "ILoveTHU!";
            str1[0] = 0;//代表“0”
            str1[1] = 2;//"2"
            str1[3] = ScanGateX[TargetGate[NowAction]];
            str1[5] = ScanGateY[TargetGate[NowAction]];
            str1[6] = TargetGate[NowAction];
            for (int i = 0; i < 4; i++)
            {
                api.SendTextMessage(i, str1);

            }
        }
    }
    if (SMission[NowAction] == 3)
    {
        api.Graduate();
    }
    if (TotalDoneClass >= 7 && GateOpen[0] == 0)
    {
        SMission[NowAction] = -2;
        StartGrad[NowAction] = 1;
        if (((abs(STPXC[NowAction] - ScanGateX[TargetGate[NowAction]])) + abs(STPYC[NowAction] - ScanGateY[TargetGate[NowAction]])) <= 1)
        {
            SMission[NowAction] = 2;
            api.StartOpenGate();
            api.Graduate();
        }
    }
    if (GateOpen[0] == 1)
    {
        SMission[NowAction] = 3;
    }
}
int ClassroomCheck(IStudentAPI& api)
{
    int TotalNum = 0;
    for (int i = 0; i < 10; i++)
    {
        if (ScanClass[i] == 1)
        {
            TotalNum++;
        }
    }
    return TotalNum;
}

void SGetMessage(IStudentAPI& api)
{
    if (api.HaveMessage() == true)
    {
        std::string str1 = api.GetMessage().second;
        int ID = api.GetMessage().first;
        if (str1[0] == 0 && str1[1] == 1)
        {
            int ClassFixed = str1[6];
            ScanClass[ClassFixed] = 1;
            std::cout << "Get Message" << ClassFixed << "\n";
        }
        if (str1[0] == 0 && str1[1] == 2)
        {
            int GateFixed = str1[6];
            ScanGate[GateFixed] = 1;
            std::cout << "Get Message" << GateFixed << "\n";
        }
    }
}
void SAction(IStudentAPI& api)
{
    if (SMission[NowAction] == -1)
    {
        RoadDirectionMoveForRoom(api);
    }
    if (SMission[NowAction] == -2 || SMission[NowAction] == 3)
    {
        RoadDirectionMoveForGrad(api);
    }
    if (SMission[NowAction] == 4)
    {
        RoadDirectionMoveForGrass(api);
    }
}
//Student与检查Tricker踪迹相关的函数
void SCheckTricker(IStudentAPI& api)
{
    if (api.GetTrickers().empty()==false)
    {
        printf("GetTricker!\n");
        if (SMission[NowAction] != 4)
        {
            MissionStorage = SMission[NowAction];
        }
        SMission[NowAction] = 4;
        Escaping = 1;
        std::cout << "RUN!";
    }
}


//监管者函数区
void TMapClear(ITrickerAPI& api)
{
    for (int i = 0; i < 50; i++)//设置临时地图
    {
        for (int j = 0; j < 50; j++)
        {
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Wall || api.GetPlaceType(i, j) == THUAI6::PlaceType::Chest || api.GetPlaceType(i, j) == THUAI6::PlaceType::ClassRoom) //（i，j不属于草地也不属于陆地）
            {
                TempMap[i][j] = 1;
            }
            if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Window) //（i，j）属于窗户
            {
                TempMap[i][j] = 3;
            }
        }
    }
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            TempMapCount[i][j] = 9999.0;
        }
    }
    if (SMission[NowAction]==-1)
    {
        {
            for (int i = 0; i < 10; i++)
            {
                if (ScanClass[i] == 0)
                {
                    TempMap[ScanClassX[i]][ScanClassY[i]] = 0;
                }
            }
            //if ((abs(api.GetSelfInfo()->x - api.CellToGrid(ScanClassX[TargetClass[NowAction]])) + abs(api.GetSelfInfo()->y - api.CellToGrid(ScanClassX[TargetClass[NowAction]])))<= 2000)
            if (((abs(STPXC[NowAction] - ScanClassX[TargetClass[NowAction]])) + abs(STPYC[NowAction] - ScanClassY[TargetClass[NowAction]])) <= 1)
            {
                SMission[NowAction] = 1;
            }
        }
    }
    return;
}
void BallLocate(ITrickerAPI& api)
{
    STPXC[NowAction] = api.GridToCell(api.GetSelfInfo()->x);
    STPYC[NowAction] = api.GridToCell(api.GetSelfInfo()->y);
    STPX[0][NowAction] = api.GridToCell(api.GetSelfInfo()->x - HalfBall);//0号左上
    STPY[0][NowAction] = api.GridToCell(api.GetSelfInfo()->y - HalfBall);
    STPX[1][NowAction] = api.GridToCell(api.GetSelfInfo()->x + HalfBall);//1号右下
    STPY[1][NowAction] = api.GridToCell(api.GetSelfInfo()->y + HalfBall);
    STPX[2][NowAction] = api.GridToCell(api.GetSelfInfo()->x - HalfBall);//2号右上
    STPY[2][NowAction] = api.GridToCell(api.GetSelfInfo()->y + HalfBall);
    STPX[3][NowAction] = api.GridToCell(api.GetSelfInfo()->x + HalfBall);//3号左下
    STPY[3][NowAction] = api.GridToCell(api.GetSelfInfo()->y - HalfBall);
}
void LandMove(ITrickerAPI& api)
{
    if ((STempPlaceX[NowAction] != SAttemptX[NowAction]) || (STempPlaceY[NowAction] != SAttemptY[NowAction]))
    {
        if (CurrentChoice[NowAction] == 0) { api.Move(MoveDuring, 3.14); }
        if (CurrentChoice[NowAction] == 1) { api.Move(MoveDuring, 1.57); }
        if (CurrentChoice[NowAction] == 2) { api.Move(MoveDuring, 0); }
        if (CurrentChoice[NowAction] == 3) { api.Move(MoveDuring, 4.71); }
    }
}
void RoadDirectionMove(ITrickerAPI& api)//寻路系统
{
    BallLocate(api);
    if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.785);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststep((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }
}

int firststepForTRoom(int f, int g)//寻路的第一步，单独分出来的原因是我们的目的就是下一步的位置
{
    if (f < 0 || f>49 || g < 0 || g>49) {
        return 0;
    }
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                TempMapCount[i][j] = 9999.0;
            }
        }
        NextPlaceType[NowAction] = 3;
        CurrentChoice[NowAction] = 5;//Test
        TempMap[f][g] = 1;
        CurrentSteps = 0.0;//初始化现在步数
        LessSteps = 9999.0;//初始化当前寻路过程的最少步数    
        NowTrialDirect = 0;//尝试向上的所有分支
        trialstepForTRoom((f - 1), g);
        CurrentSteps = 0.0;
        NowTrialDirect = 1;//尝试向右的所有分支
        trialstepForTRoom((f), (g + 1));
        CurrentSteps = 0.0;
        NowTrialDirect = 2;//尝试向下的所有分支
        trialstepForTRoom((f + 1), g);
        TempMap[f][g] = 0;
        NowTrialDirect = 3;//尝试向左的所有分支
        trialstepForTRoom(f, (g - 1));
        CurrentSteps = 0.0;
        NextStep();
        if (LessSteps < 9999.0) { return CurrentChoice[NowAction]; }
        else return 0;
    }
}
void trialstepForTRoom(int f, int g)//用于一步步测试，该函数使用了递归
{
    int FinalFlag = 0;
    if (f < 0 || f>49 || g < 0 || g>49) { return; }//如果越界，直接结束该分支
    else
    {
        if (TempMap[f][g] == 1 || TempMap[f][g] == 4) {}//如果临时地图TempMap上（f，g）不可移动：直接结束该分支
        else//说明这边至少是可以走的，进行下一步尝试
        {
            if ((CurrentSteps + LandCount) < TempMapCount[f][g])
            {
                CurrentSteps += LandCount;
                TempMapCount[f][g] = CurrentSteps;
                for (int i = 0; i < 10; i++)
                {
                    if (f == SAttemptX[NowAction] && g == SAttemptY[NowAction])//原checkstep函数改换至此
                    {
                        FinalFlag = 1;
                        if (CurrentSteps < LessSteps)
                        {
                            LessSteps = CurrentSteps;
                            CurrentChoice[NowAction] = NowTrialDirect;
                            TargetClass[NowAction] = i;//这暂时是必要的，因为后期要写这个教室被完成了
                            SAttemptX[NowAction] = f;
                            SAttemptX[NowAction] = g;
                        }
                    }
                }
                if (FinalFlag == 0)
                {
                    TempMap[f][g] ++;
                    trialstepForTRoom((f - 1), g);
                    trialstepForTRoom((f), (g + 1));
                    trialstepForTRoom((f + 1), g);
                    trialstepForTRoom(f, (g - 1));
                    TempMap[f][g] --;
                }
                FinalFlag = 0;
                CurrentSteps -= LandCount;
            }
        }
    }
    return;
}
void RoadDirectionMoveForTRoom(ITrickerAPI& api)//寻路系统
{
    BallLocate(api);
    //if ((STPX[1][NowAction] == STPX[0][NowAction]) && (STPY[1][NowAction] == STPY[0][NowAction]))//确定整个在同一个格子内
    {
        STempPlaceX[NowAction] = STPXC[NowAction]; STempPlaceY[NowAction] = STPYC[NowAction];
    }
    if (TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 1 || TempMap[STPX[0][NowAction]][STPY[0][NowAction]] == 4)//若偏离，补救措施
    {
        api.Move(AdjustmentDuring, 0.786);
    }
    else if (TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 1 || TempMap[STPX[1][NowAction]][STPY[1][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 3.925);
    }
    else if (TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 1 || TempMap[STPX[2][NowAction]][STPY[2][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 5.495);
    }
    else if (TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 1 || TempMap[STPX[3][NowAction]][STPY[3][NowAction]] == 4)
    {
        api.Move(AdjustmentDuring, 2.395);
    }
    else
    {
        firststepForTRoom((STempPlaceX[NowAction]), (STempPlaceY[NowAction]));
        NextStep();
        if (NextPlaceType[NowAction] == 0) { LandMove(api); }
        else if (NextPlaceType[NowAction] == 3)
        {
            api.SkipWindow();
        }
    }

    //else {LandMove(api); }//For Debug
}//寻路

void SpecialMapCheck(ITrickerAPI& api)
{
    int TotalNumber1 = 0;
    int TotalNumber2 = 0;
    if (GameStartFlag == true) {}
    else
    {
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::ClassRoom)
                {
                    ScanClassX[TotalNumber1] = i;
                    ScanClassY[TotalNumber1] = j;
                    TotalNumber1++;
                }
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Gate)
                {
                    ScanGateX[TotalNumber2] = i;
                    ScanGateY[TotalNumber2] = j;
                    TotalNumber2++;
                }
                if (api.GetPlaceType(i, j) == THUAI6::PlaceType::Grass)
                {
                    GrassMap[i][j] = 1;
                }
            }
        }
    }
    return;
}
void GameStartSettings(ITrickerAPI& api)
{
    SpecialMapCheck(api);
    SMission[4] = -1;
    GameStartFlag = true;
}

void StuLocate(ITrickerAPI& api)
{
    int num = api.GetStudents().size();
    bool stu = api.GetStudents().empty();
    if (stu!=true)
    {
        for (int i = 0; i < num; i++)
        {
            if (api.GetStudents()[i]->studentType != THUAI6::StudentType::Teacher && api.GetStudents()[i]->studentType != THUAI6::StudentType::NullStudentType)
            {
                SMission[NowAction] = -1;
                int Xtarget = api.GetStudents()[i]->x;
                int Ytarget = api.GetStudents()[i]->y;
                double ShotAngle = (api.GetSelfInfo()->y - Ytarget) / (api.GetSelfInfo()->x - Xtarget);
                double distanceq = abs(api.GetSelfInfo()->y - Ytarget) * abs(api.GetSelfInfo()->y - Ytarget) + abs(api.GetSelfInfo()->x - Xtarget) * abs(api.GetSelfInfo()->x - Xtarget);
                if (distanceq < 9000000.0)
                {
                    if (api.GetSelfInfo()->x > Xtarget) { api.Attack(atan(ShotAngle) + 3.14); }
                    else api.Attack(atan(ShotAngle));
                }
                //SAttemptX[NowAction] = api.GridToCell(Xtarget);
                //SAttemptY[NowAction] = api.GridToCell(Ytarget);
                std::cout << "Get Student!";
            }
        }
    }
    else
    {
    }
}

void TMissionCheck()
{

}

void AI::play(IStudentAPI& api)
{
    GameStartSettings(api);
    TotalDoneClass = ClassroomCheck(api);
    // 公共操作
    if (this->playerID == 0)
    {
        // 玩家0执行操作
        NowAction = 0;
        SGetMessage(api);
        MapClear(api);
        BallLocate(api);
        MissionCheck(api);    
        SAction(api);
        MapReset();
        BallLocate(api);
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
        NowAction = 1;
        SGetMessage(api);
        MapClear(api);
        BallLocate(api);
        MissionCheck(api);
        SAction(api);
        MapReset();
        BallLocate(api);
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
        NowAction = 2;
        SGetMessage(api);
        MapClear(api);
        BallLocate(api);
        MissionCheck(api);
        SAction(api);
        MapReset();
        BallLocate(api);
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
        NowAction = 3;
        SGetMessage(api);
        MapClear(api);
        BallLocate(api);
        MissionCheck(api);
        SAction(api);
        MapReset();
        BallLocate(api);
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
    std::cout << "NUM" << TotalDoneClass;
    std::cout << "State" << SMission[NowAction];
    if (Escaping == 1) { printf("Now Escaping!\n"); }
    std:: cout<< GrassMap[STPXC[NowAction]][STPYC[NowAction]];
    api.UseSkill(0);
}
void AI::play(ITrickerAPI& api)
{
    TMapClear(api);
    GameStartSettings(api);
    NowAction = 4;
    StuLocate(api);
    if (SMission[NowAction] == -1)
    {
        RoadDirectionMoveForTRoom(api);
    }
    if (LessSteps == 9999.0)
    {
        std::cout << "Change";
        SMission[NowAction] = 1;
    }
    if (SAttemptX[NowAction]==STPXC[NowAction]&&SAttemptY[NowAction]==STPYC[NowAction])
    {
        std::cout << "Change";
        SMission[NowAction] = 1;
    }
    if (SMission[NowAction] == 1)
    {
        srand(time(0));
        int a = rand() % 10;
        SAttemptX[NowAction] = ScanClassX[a];
        SAttemptY[NowAction] = ScanClassY[a];
        RoadDirectionMoveForTRoom(api);
        SMission[4] = -1;
    }
    api.UseSkill(1);
    api.UseSkill(2);
    MapReset();
    std::cout << SAttemptX[NowAction] << SAttemptY[NowAction]<<SMission[NowAction];
}
