#include <vector>
#include <thread>
#include <array>
#include <math.h>
#include "AI.h"
#include "constants.h"
#define PI 3.14159265358979323846

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义
extern const std::array<THUAI6::StudentType,4> studentType ={
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Sunshine};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

// 可以在AI.cpp内部声明变量与函数

// 学生阵营
// 前进方向
double directionS[4] ={0, 0, 0, 0};
// 上次经过的门
int LastDoorS[4][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}};
// 上次经过门后的帧数
int LastDoorFrame[4] ={0, 0, 0, 0};
// 上次经过的校门
int LastGateS[4][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}};
// 上一帧所在坐标
int32_t LastAddress[4][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}};
// 在相同坐标不动的帧数
int StayFrame[4] ={0, 0, 0, 0};
// 经过门的方向，-1表示沿坐标减小方向，1表示沿坐标增大方向
int CrossDoorDirection[4] ={0, 0, 0, 0};
//进入九宫格后的帧数
int NineFrame[4] ={0,0,0,0};

// 捣蛋鬼阵营
//  前进方向
double directionTricker = 0;

// 捣蛋鬼上一帧参数
int beforeTrickerAddress[2];

// 返回目的地与现在位置间的角度，目的地为格数，现在位置为坐标
double angleangleInRadian(int32_t add[2],int add0[2])
{
	int addx = 1000 * add0[0] + 500;
	int addy = 1000 * add0[1] + 500;
	double a;
	if((addx - add[0]) == 0)
	{
		a = (addy >= add[1]) ? PI / 2 : 3 * PI / 2;
	}
	else
	{
		double tan = (double)(addy - add[1]) / (double)(addx - add[0]);
		a = (addx > add[0]) ? atan(tan) : (atan(tan) + PI);
	}
	return a;
}

// 返回目的地与现在位置间的角度（Grid坐标）
double angleInRadianGrid(int add[2],int add0[2])
{
	double a;
	if((add0[0] - add[0]) == 0)
		a = (add0[1] >= add[1]) ? PI / 2 : 3 * PI / 2;
	else
	{
		double tan = (double)(add0[1] - add[1]) / (double)(add0[0] - add[0]);
		a = (add0[0] > add[0]) ? atan(tan) : (atan(tan) + PI);
	}
	return a;
}

// 判断目的地与现在位置间的距离，目的地为格数，现在位置为坐标
int32_t Distance(int32_t add[2],int joint[2])
{
	int jointx = 1000 * joint[0] + 500;
	int jointy = 1000 * joint[1] + 500;
	return sqrt(pow(jointx - add[0],2) + pow(jointy - add[1],2));
}

// 计算绝对坐标距离
int32_t distanceGrid(int self[2],int target[2])
{
	return sqrt(pow(self[1] - target[1],2) + pow(self[0] - target[0],2));
}

// 判断目标点是否可视 (for student)
bool CheckVisible(IStudentAPI& api,int32_t add[2],int joint[2])
{
	double angle = angleangleInRadian(add,joint);
	double dis;
	bool flag = true;
	int32_t check[2];
	check[0] = add[0];
	check[1] = add[1];
	dis = Distance(add,joint);
	while(Distance(check,joint) > 750)
	{
		if(api.GetPlaceType(api.GridToCell(check[0]),api.GridToCell(check[1])) != (enum THUAI6::PlaceType)1)
		{
			flag = false;
			break;
		}
		check[0] += 250 * cos(angle);
		check[1] += 250 * sin(angle);
	}
	return flag;
}

// 判断目标点是否可视 (for tricker)
bool CheckVisible(ITrickerAPI& api,int32_t add[2],int joint[2])
{
	double angle = angleangleInRadian(add,joint);
	double dis;
	bool flag = true;
	int32_t check[2];
	check[0] = add[0];
	check[1] = add[1];
	dis = Distance(add,joint);
	while(Distance(check,joint) > 750)
	{
		if(api.GetPlaceType(api.GridToCell(check[0]),api.GridToCell(check[1])) != (enum THUAI6::PlaceType)1)
		{
			flag = false;
			break;
		}
		check[0] += 250 * cos(angle);
		check[1] += 250 * sin(angle);
	}
	return flag;
}

// 判断目标点是否可到达 (for student)
bool CheckApproachable(IStudentAPI& api,int32_t add[2],int joint[2])
{
	int edge[2];
	bool flag = true;
	double dir = angleangleInRadian(add,joint);
	for(int i = 0; i <= 1; i++)
	{
		edge[0] = add[0] + 500 * cos(dir + PI / 2 - i * PI);
		edge[1] = add[1] + 500 * sin(dir + PI / 2 - i * PI);
		flag = flag && CheckVisible(api,edge,joint);
	}
	return flag;
}

// 判断目标点是否可到达（绝对坐标） (for student)
bool CheckApproachableGrid(IStudentAPI& api,int32_t add[2],int32_t jointAdd[2])
{
	int joint[2] ={jointAdd[0] / 1000, jointAdd[1] / 1000};
	return CheckApproachable(api,add,joint);
}

// 判断目标点是否可到达 (for tricker)
bool CheckApproachable(ITrickerAPI& api,int32_t add[2],int joint[2])
{
	int edge[2];
	bool flag = true;
	double dir = angleangleInRadian(add,joint);
	for(int i = 0; i <= 1; i++)
	{
		edge[0] = add[0] + 500 * cos(dir + PI / 2 - i * PI);
		edge[1] = add[1] + 500 * sin(dir + PI / 2 - i * PI);
		flag = flag && CheckVisible(api,edge,joint);
	}
	return flag;
}

// 判断前进的方向
double Direction(int32_t add[2],int joint[2])
{
	double ang = angleangleInRadian(add,joint);
	double dir = 0;
	for(int i = 1; i <= 29; i += 2)
	{
		if(ang > (double)i * PI / 16 && ang <= (double)(i + 2) * PI / 16)
			dir = (double)(i + 1) * PI / 16;
	}
	return dir;
}

// 检查前进方向是否有阻碍 (for students)
bool CheckBlocekd(IStudentAPI& api,int32_t add[2],double dir)
{
	int edge[2];
	bool flag = false;
	for(int i = 0; i <= 2; i++)
	{
		edge[0] = add[0] + 500 * cos(dir + PI / 2 - i * PI / 2);
		edge[1] = add[1] + 500 * sin(dir + PI / 2 - i * PI / 2);
		for(int32_t dis = 250; dis <= 1000; dis += 250)
		{
			int front[2] ={(edge[0] + dis * cos(dir)) / 1000, (edge[1] + dis * sin(dir)) / 1000};
			if(api.GetPlaceType(front[0],front[1]) != (enum THUAI6::PlaceType)1 && api.GetPlaceType(front[0],front[1]) != (enum THUAI6::PlaceType)3)
			{
				flag = true;
				break;
			}
		}
	}
	return flag;
}

// 检查前进方向是否有阻碍 (for tricker)
bool CheckBlocekd(ITrickerAPI& api,int32_t add[2],double dir)
{
	int edge[2];
	bool flag = false;
	for(int i = 0; i <= 2; i++)
	{
		edge[0] = add[0] + 500 * cos(dir + PI / 2 - i * PI / 2);
		edge[1] = add[1] + 500 * sin(dir + PI / 2 - i * PI / 2);
		for(int32_t dis = 250; dis <= 1000; dis += 250)
		{
			int front[2] ={(edge[0] + dis * cos(dir)) / 1000, (edge[1] + dis * sin(dir)) / 1000};
			if(api.GetPlaceType(front[0],front[1]) != (enum THUAI6::PlaceType)1 && api.GetPlaceType(front[0],front[1]) != (enum THUAI6::PlaceType)3)
			{
				flag = true;
				break;
			}
		}
	}
	return flag;
}

// 检查被卡住时是否与学生相撞，返回值为相撞的学生序号，4代表未相撞 (for students)
int StudentStrike(int32_t add[2],int32_t stuAdd[4][2],int id)
{
	int strikeID = 4;
	for(int i = 0; i < 4; i++)
	{
		if(i == id)
			continue;
		if(!stuAdd[i][0] && distanceGrid(add,stuAdd[i]) < 1200)
		{
			strikeID = i;
			break;
		}
	}
	return strikeID;
}

// 判断被阻碍时应向左转还是向右转,逆时针为正 (for students)
double Turn(IStudentAPI& api,int32_t add[2],double dir)
{
	double turn = 0;
	int d[9];
	bool di[9];
	for(int i = 0; i < 9; i++)
	{
		d[i] = 0;
		di[i] = true;
	}
	bool flag = true;
	int max = 0;
	for(int32_t dis = 500; flag; dis += 500)
	{
		for(int i = 0; i < 9; i++)
		{
			if(i == 4)
				continue;
			if(di[i])
			{
				int x = (add[0] + dis * cos(dir + PI / 2 - i * PI / 8)) / 1000;
				int y = (add[1] + dis * sin(dir + PI / 2 - i * PI / 8)) / 1000;
				if(api.GetPlaceType(x,y) == (enum THUAI6::PlaceType)1 && api.GetPlaceType(x,y) != (enum THUAI6::PlaceType)3)
					d[i]++;
				else
					di[i] = false;
			}
			flag = flag || di[i];
		}
		flag = flag && (dis < 12000);
	}
	for(int i = 0; i < 9; i++)
	{
		if(i == 4)
			continue;
		if(d[i] > max)
		{
			turn = (4 - i) * PI / 8;
			max = d[i];
		}
	}
	return turn;
}

// 判断被阻碍时应向左转还是向右转,逆时针为正 (for tricker)
double Turn(ITrickerAPI& api,int32_t add[2],double dir)
{
	double turn = 0;
	int d[9];
	bool di[9];
	for(int i = 0; i < 9; i++)
	{
		d[i] = 0;
		di[i] = true;
	}
	bool flag = true;
	int max = 0;
	for(int32_t dis = 500; flag; dis += 500)
	{
		for(int i = 0; i < 9; i++)
		{
			if(i == 4)
				continue;
			if(di[i])
			{
				int x = (add[0] + dis * cos(dir + PI / 2 - i * PI / 8)) / 1000;
				int y = (add[1] + dis * sin(dir + PI / 2 - i * PI / 8)) / 1000;
				if(api.GetPlaceType(x,y) == (enum THUAI6::PlaceType)1)
					d[i]++;
				else
					di[i] = false;
			}
			flag = flag || di[i];
		}
		flag = flag && (dis < 12000);
	}
	for(int i = 0; i < 9; i++)
	{
		if(i == 4)
			continue;
		if(d[i] > max)
		{
			turn = (4 - i) * PI / 8;
			max = d[i];
		}
	}
	return turn;
}

// 判断被卡住时应该往哪个方向走 (for students)
double ChuckDirection(IStudentAPI& api,int32_t add[2])
{
	double dir = PI;
	for(int i = 0; i < 4; i++)
	{
		int check[2] ={api.GridToCell(add[0]), api.GridToCell(add[1])};
		switch(i)
		{
		case 0:
			check[0]--;
			check[1]--;
			break;
		case 1:
			check[0]++;
			check[1]--;
			break;
		case 2:
			check[0]++;
			check[1]++;
			break;
		case 3:
			check[0]--;
			check[1]++;
			break;
		default:
			break;
		}
		if(api.GetPlaceType(check[0],check[1]) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(check[0],check[1]) == (enum THUAI6::PlaceType)4)
			dir = i * PI / 2 + PI / 4;
	}
	if(dir == PI)
	{
		for(int j = 0; j < 4; j++)
		{
			int check[2] ={api.GridToCell(add[0]), api.GridToCell(add[1])};
			switch(j)
			{
			case 0:
				check[0]--;
				break;
			case 1:
				check[1]--;
				break;
			case 2:
				check[0]++;
				break;
			case 3:
				check[1]++;
				break;
			default:
				break;
			}
			if(api.GetPlaceType(check[0],check[1]) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(check[0],check[1]) == (enum THUAI6::PlaceType)4)
				dir = j * PI / 2;
		}
	}
	return dir;
}

// 判断目标点与现在位置是否在同一个九宫格内
bool CheckNine(int32_t add[2],int joint[2])
{
	int jointGrid[2] ={joint[0] * 1000 + 500, joint[1] * 1000 + 500};
	bool flag = false;
	if((abs(jointGrid[0] - add[0]) < 1200) && (abs(jointGrid[1] - add[1]) < 1200))
		flag = true;
	return flag;
}

// 判断门开的方向，竖直开（水平两边是墙）返回0，水平开返回1
int DoorDirection(IStudentAPI& api,int joint[2])
{
	int dir = 0;
	if(api.GetPlaceType(joint[0] + 1,joint[1]) == (enum THUAI6::PlaceType)2)
		dir = 1;
	return dir;
}

// 一个数组，返回最小项索引
int findMinIndex(double arr[],int size)
{
	int minIndex = 0; // 用于保存当前最小项的索引
	for(int i = 1; i < size; i++)
	{
		if(arr[i] < arr[minIndex]) // 如果当前项小于最小项
		{
			minIndex = i; // 更新最小项的索引
		}
	}
	return minIndex; // 返回最小项的索引
}

// 判断是否被卡住
bool isStuck(int past[2],int now[2])
{
	int deltaMove;
	deltaMove = distanceGrid(past,now);
	if(deltaMove < 10)
		return true;
	else
		return false;
}

// 看草函数
bool findGrass(ITrickerAPI& api)
{
	bool thereIsGrass = false;
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)};
	for(int i = addressCell[0] - 2; i <= addressCell[0] + 2; i++)
	{
		for(int j = addressCell[1] - 2; j <= addressCell[1] + 2; j++)
		{
			if(api.GetPlaceType(addressCell[0] - 2,addressCell[1] - 2) == (enum THUAI6::PlaceType)2)
				thereIsGrass = true;
		}
	}
	return thereIsGrass;
}

// 进草函数
void trickerInGrass(ITrickerAPI& api)
{
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)};
	if(api.GetPlaceType(addressCell[0] - 1,addressCell[1] - 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,5 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] - 2,addressCell[1] - 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,5 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] - 1,addressCell[1]) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI);
	}
	else if(api.GetPlaceType(addressCell[0] - 2,addressCell[1]) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI);
	}
	else if(api.GetPlaceType(addressCell[0] - 1,addressCell[1] + 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,3 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] - 2,addressCell[1] + 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,3 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0],addressCell[1] - 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,3 * PI / 2);
	}
	else if(api.GetPlaceType(addressCell[0],addressCell[1] - 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,3 * PI / 2);
	}
	else if(api.GetPlaceType(addressCell[0],addressCell[1] + 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI / 2);
	}
	else if(api.GetPlaceType(addressCell[0],addressCell[1] + 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI / 2);
	}
	else if(api.GetPlaceType(addressCell[0] + 1,addressCell[1] - 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,7 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] + 2,addressCell[1] - 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,7 * PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] + 1,addressCell[1]) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,0);
	}
	else if(api.GetPlaceType(addressCell[0] + 2,addressCell[1]) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,0);
	}
	else if(api.GetPlaceType(addressCell[0] + 1,addressCell[1] + 1) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI / 4);
	}
	else if(api.GetPlaceType(addressCell[0] + 2,addressCell[1] + 2) == (enum THUAI6::PlaceType)2)
	{
		api.Move(200,PI / 4);
	}
}

// 临窗函数
bool findWindow(ITrickerAPI& api)
{
	bool thereIsWindow = false;
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)};
	for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
	{
		for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
		{
			if(api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)7)
				thereIsWindow = true;
		}
	}
	return thereIsWindow;
}

// 找窗函数
bool bool_findWindowIn25(ITrickerAPI& api)
{
	bool thereIsWindow = false;
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)};
	for(int i = addressCell[0] - 2; i <= addressCell[0] + 2; i++)
	{
		for(int j = addressCell[1] - 2; j <= addressCell[1] + 2; j++)
		{
			if(api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)7)
				thereIsWindow = true;
		}
	}
	return thereIsWindow;
}

// 周围移动方向
double aroundDir(int i,int j)
{
	if(i == -1 && j == -1)
		return 1.25 * PI;
	else if(i == -1 && j == 0)
		return PI;
	else if(i == -1 && j == 1)
		return PI / 4;
	else if(i == 0 && j == -1)
		return 3 * PI / 2;
	else if(i == 0 && j == 1)
		return PI / 2;
	else if(i == 1 && j == -1)
		return 7 * PI / 4;
	else if(i == 1 && j == 0)
		return 0.0;
	else if(i == 1 && j == 1)
		return PI / 4;
	else return 0;
}

// 学生操作
void AI::play(IStudentAPI& api)
{
	// 获取自身ID
	int id = this->playerID;
	// 获取自身坐标
	int32_t address[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
	// 获取自身所在格数
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)};
	// 获取视野内捣蛋鬼信息
	std::vector<std::shared_ptr<const THUAI6::Tricker>> trickerInfo = api.GetTrickers();
	// 获取视野内学生信息
	std::vector<std::shared_ptr<const THUAI6::Student>> studentInfo = api.GetStudents();
	// 捣蛋鬼绝对坐标
	int32_t trickerAddress[2] ={0, 0};
	// 学生绝对坐标
	int32_t studentAddress[4][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}};
	// 可见的教室格子坐标,第三位表示是否找到
	int ClassroomVisible[3] ={0, 0, 0};
	// 可见的门坐标，第三位表示门开的方向，0为竖直开，1为水平开；第四位表示是否找到
	int DoorVisible[4] ={0, 0, 0, 0};
	// 可见的校门或隐藏校门的坐标，第三位表示是否开启，第四位表示是否找到
	int GateVisible[4] ={0, 0, 0};
	// 是否逃跑
	bool IsEscape = ((int)trickerInfo.size() > 0);
	// 是否学习
	bool IsLearn = false;
	// 视野内学生数量
	int numStudent = (int)studentInfo.size();
	// 获取视野内捣蛋鬼坐标
	if(IsEscape)
	{
		trickerAddress[0] = (*trickerInfo[0]).x;
		trickerAddress[1] = (*trickerInfo[0]).y;
	}
	// 获取视野内学生坐标
	for(int i = 0; i < numStudent; i++)
	{
		int studentID = (*studentInfo[i]).playerID;
		studentAddress[id][0] = (*studentInfo[i]).x;
		studentAddress[id][1] = (*studentInfo[i]).y;
	}
	// 判断是否与相对上一帧静止
	if(distanceGrid(address,LastAddress[id]) < 10)
		StayFrame[id]++;
	else
		StayFrame[id] = 0;
	// 判断是否卡住
	bool Chucked = api.GetSelfInfo()->playerState == (enum THUAI6::PlayerState)1 && StayFrame[id] > 10;
	// 如果1000帧未过门，则清空上一次过门数据
	if(LastDoorFrame[id] > 1000)
	{
		LastDoorS[id][0] = 0;
		LastDoorS[id][1] = 0;
	}
	// 若没有卡住，则扫描周围24方内的地点
	if(!Chucked)
	{
		for(int i = (addressCell[0] >= 12) ? addressCell[0] - 12 : 0; i <= addressCell[0] + 12 && i < 50; i++)
		{
			for(int j = (addressCell[1] >= 12) ? addressCell[1] - 12 : 0; j <= addressCell[1] + 12 && j < 50; j++)
			{
				int joint[2] ={i, j};
				// 判断是否为门
				if((api.GetPlaceType(i,j) == ((enum THUAI6::PlaceType)8) || api.GetPlaceType(i,j) == ((enum THUAI6::PlaceType)9)
					|| api.GetPlaceType(i,j) == ((enum THUAI6::PlaceType)10)) && CheckApproachable(api,address,joint) && (DoorVisible[3] == 0)
					&& i != LastDoorS[id][0] && j != LastDoorS[id][1])
				{
					DoorVisible[0] = i;
					DoorVisible[1] = j;
					DoorVisible[2] = DoorDirection(api,joint);
					DoorVisible[3] = 1;
				}
				// 若不用逃跑，则继续扫描教室和校门
				if(!IsEscape)
				{
					// 判断是否为教室
					if((api.GetPlaceType(i,j) == ((enum THUAI6::PlaceType)4)) && (ClassroomVisible[2] == 0)
						&& CheckApproachable(api,address,joint) && (api.GetClassroomProgress(i,j) < 10000000))
					{
						ClassroomVisible[0] = i;
						ClassroomVisible[1] = j;
						ClassroomVisible[2] = 1;
					}
					// 判断是否为校门
					if((api.GetGameInfo())->subjectFinished >= 7 && (api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)5
						|| (api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)6 && api.GetHiddenGateState(i,j) == (enum THUAI6::HiddenGateState)1))
						&& (GateVisible[3] == 0) && CheckApproachable(api,address,joint) && i != LastGateS[id][0] && j != LastGateS[id][1])
					{
						GateVisible[0] = i;
						GateVisible[1] = j;
						GateVisible[2] = (api.GetGateProgress(i,j) == 18000) ? 1 : 0;
						GateVisible[3] = 1;
					}
					if(api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)6 && api.GetHiddenGateState(i,j) != (enum THUAI6::HiddenGateState)0)
						api.Print(fmt::format("HiddenGate: {}",(int)api.GetHiddenGateState(i,j)));
				}
			}
		}
	}
	api.Print(fmt::format("Subject: {}",(api.GetGameInfo())->subjectFinished));
	// 开心果使用技能
	if(id == 3)
	{
		// 若视野中存在捣蛋鬼，则使用鼓舞技能
		if(IsEscape && (api.GetSelfInfo())->timeUntilSkillAvailable[2] == 0)
			api.UseSkill(2);
		else
		{
			int determination[4] ={3300000, 3300000, 3200000, 30000000};
			for(int i = 0; i < numStudent; i++)
			{
				// 若该学生在视野内
				if(api.HaveView(studentAddress[i][0],studentAddress[i][1]))
				{
					// 若视野内学生沉迷则使用唤醒技能
					if((*studentInfo[i]).playerState == (enum THUAI6::PlayerState)3 && (api.GetSelfInfo())->timeUntilSkillAvailable[0] == 0)
						api.UseSkill(0);
					// 若视野内学生毅力值不满则使用勉励技能
					else if(((*studentInfo[i]).determination < determination[i] - 750000)
						&& (api.GetSelfInfo())->timeUntilSkillAvailable[1] == 0)
						api.UseSkill(1);
				}
			}
		}
	}
	// 若视野内有可见教室，向其移动，到达时开始学习
	if(ClassroomVisible[2] == 1)
	{
		if(CheckNine(address,ClassroomVisible))
		{
			if((id == 1 || id == 0) && api.GetSelfInfo()->playerState == (enum THUAI6::PlayerState)1)
				api.UseSkill(0);
			api.StartLearning();
		}
		else
		{
			directionS[id] = angleangleInRadian(address,ClassroomVisible);
			api.EndAllAction();
			api.Move(50,directionS[id]);
		}
	}
	// 若不是老师且视野内有可到达校门，向其移动，并开启
	else if(GateVisible[3] == 1 && id != 2)
	{
		api.Print(fmt::format("GateProgress: {}",api.GetGateProgress(GateVisible[0],GateVisible[1])));
		directionS[id] = angleangleInRadian(address,GateVisible);
		api.EndAllAction();
		api.Move(50,directionS[id]);
		// 若到达距离则开始开门
		if(CheckNine(address,GateVisible))
		{
			if(!GateVisible[2])
				api.StartOpenGate();
			else
				api.Graduate();
		}
	}
	// 若卡住的补救措施
	else if(Chucked)
	{
		// 若卡住的地方为门
		if(api.GetPlaceType(addressCell[0],addressCell[1]) == (enum THUAI6::PlaceType)8
			|| api.GetPlaceType(addressCell[0],addressCell[1]) == (enum THUAI6::PlaceType)9 || api.GetPlaceType(addressCell[0],addressCell[1]) == (enum THUAI6::PlaceType)10)
		{
			if(DoorDirection(api,addressCell) == 0)
			{
				if(CrossDoorDirection[id] == 1)
					directionS[id] = 0;
				else
					directionS[id] = PI;
			}
			else if(DoorDirection(api,addressCell) == 1)
			{
				if(CrossDoorDirection[id] == 1)
					directionS[id] = PI / 2;
				else
					directionS[id] = 3 * PI / 2;
			}
		}
		// 若为学生相撞而卡住
		else if(StudentStrike(address,studentAddress,id) != 4)
			directionS[id] =
			angleInRadianGrid(studentAddress[StudentStrike(address,studentAddress,id)],address);
		// 若卡住的地方不为门
		else
			directionS[id] = ChuckDirection(api,address);
		api.EndAllAction();
		api.Move(50,directionS[id]);
	}
	else
	{
		// 若视野中存在捣蛋鬼
		if(IsEscape)
		{
			// 若为老师且捣蛋鬼可被惩罚时
			if(id == 2 && ((*trickerInfo[0]).playerState == (enum THUAI6::PlayerState)11 || (*trickerInfo[0]).playerState == (enum THUAI6::PlayerState)12
				|| (*trickerInfo[0]).playerState == (enum THUAI6::PlayerState)13 || (*trickerInfo[0]).playerState == (enum THUAI6::PlayerState)15)
				&& (api.GetSelfInfo())->timeUntilSkillAvailable[0] == 0)
				api.UseSkill(0);
			// 逃跑的情况
			else
			{
				directionS[id] = angleangleInRadian(trickerAddress,addressCell);
				if((api.GetSelfInfo())->timeUntilSkillAvailable[1] == 0 && distanceGrid(address,trickerAddress) < 3000)
					api.UseSkill(1,1000 * directionS[id]);
				else
				{
					if(CheckBlocekd(api,address,directionS[id]))
						directionS[id] += Turn(api,address,directionS[id]);
					api.EndAllAction();
					api.Move(50,directionS[id]);
				}
			}
		}
		// 若视野中不存在捣蛋鬼
		else
		{
			// 若视野内无教室但有门
			if(DoorVisible[3] == 1)
			{
				// 再到达门时将其记为上一次经过的门
				if(DoorVisible[0] == addressCell[0] && DoorVisible[1] == addressCell[1])
				{
					LastDoorS[id][0] = DoorVisible[0];
					LastDoorS[id][1] = DoorVisible[1];
					LastDoorFrame[id] = 0;
				}
				// 未到达门前
				else
				{
					CrossDoorDirection[id] = (addressCell[DoorVisible[2]] >= DoorVisible[DoorVisible[2]]);
					CrossDoorDirection[id] = (CrossDoorDirection[id] == 0) ? -1 : 1;
					int joint[2] ={DoorVisible[0], DoorVisible[1]};
					joint[DoorVisible[2]] += CrossDoorDirection[id];
					bool flag;
					if(DoorVisible[2] == 0)
						flag = (joint[1] == (address[1] + 450) / 1000) && (joint[1] == (address[1] - 450) / 1000);
					else
						flag = (joint[0] == (address[0] + 450) / 1000) && (joint[0] == (address[0] - 450) / 1000);
					if(flag)
					{
						if(DoorVisible[2] == 0)
						{
							if(CrossDoorDirection[id] == 1)
								directionS[id] = PI;
							else
								directionS[id] = 0;
						}
						else
						{
							if(CrossDoorDirection[id] == 1)
								directionS[id] = 3 * PI / 2;
							else
								directionS[id] = PI / 2;
						}
					}
					else if(CheckApproachable(api,address,joint))
						directionS[id] = angleangleInRadian(address,joint);
					else
					{
						// 若前方有阻碍
						if(CheckBlocekd(api,address,directionS[id]))
							directionS[id] += Turn(api,address,directionS[id]);
					}
					api.EndAllAction();
					api.Move(50,directionS[id]);
					// 若过门时卡住需要使用该数据
					CrossDoorDirection[id] = -CrossDoorDirection[id];
				}
			}
			// 视野中什么也没有时
			else
			{
				// 若前方有阻碍
				if(CheckBlocekd(api,address,directionS[id]))
					directionS[id] += Turn(api,address,directionS[id]);
				api.EndAllAction();
				api.Move(50,directionS[id]);
			}
		}
	}
	// 将方向化为0至2PI之间
	directionS[id] = directionS[id] - (int)(directionS[id] / (2 * PI)) * 2 * PI;
	// 一些善后操作
	LastAddress[id][0] = address[0];
	LastAddress[id][1] = address[1];
	LastDoorFrame[id]++;
}

// 捣蛋鬼操作
void AI::play(ITrickerAPI& api)
{
	auto self = api.GetSelfInfo();
	api.PrintSelfInfo();
	int address[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};									   // 获取自身坐标
	int addressCell[2] ={api.GridToCell(api.GetSelfInfo()->x), api.GridToCell(api.GetSelfInfo()->y)}; // 获取自身所在格数
	int stuTarget;																					   // 最小距离学生索引
	int numTarget;																					   // 可以攻击学生数量
	int waitWindow=0;
	bool isStuckTricker = false;
	bool attackOK = false;																 // 判断是否进行攻击
	int didnotMove=0;
	int studentInView[5][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};					 // 视野内有无学生，若有，返回格子坐标；第一个指标为学生序号，第二个指标为坐标
	int studentInViewCell[5][2] ={{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};				 // 视野内有无学生(格子版)
	std::vector<std::shared_ptr<const THUAI6::Student>> studentInfo = api.GetStudents(); // 获取视野可见学生信息
	std::vector<std::vector<THUAI6::PlaceType>> mapTricker = api.GetFullMap();

	waitWindow+=1;
	// 判断进入攻击模式还是巡航模式
	numTarget = (int)studentInfo.size();
	for(int i = 0; i < (int)studentInfo.size(); i++)
	{
		/*if((*studentInfo[i]).studentType==THUAI6::StudentType::Teacher)
			numTarget--;*/
		if((*studentInfo[i]).determination == 0)
			numTarget--;
	}

	// 周围可攻击作业数目

	// 判断是否被卡
	isStuckTricker = (isStuck(beforeTrickerAddress,address)) && (numTarget == 0);

	// 判断是否使用技能
	if(api.GetSelfInfo()->trickDesire > 1.177 && api.GetSelfInfo()->timeUntilSkillAvailable[0] == 0)
	{
		api.UseSkill(0);
	}

	// 如果被卡，判断哪个方向被卡，并向反方向移动
	if(isStuckTricker == true)
	{
		api.Print("Tricker is stuck\n");
		// 判断哪个方向被卡，并向附近没墙的地方移动
		if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y-297)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y-297)) == (enum THUAI6::PlaceType)4 ||api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y-297))== (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] - 1,addressCell[1] - 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck zuoshang\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(-i-j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)4 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] - 1,addressCell[1]) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck shang\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(-i<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,0);
			}
		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x-297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] - 1,addressCell[1] + 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck yuoshang\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(-i+j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,7*PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y-410)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y-410)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y-410)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] ,addressCell[1] - 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck zuo\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(-j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,2*PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y+410)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y+410)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x),api.GridToCell(api.GetSelfInfo()->y+410)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] ,addressCell[1]+ 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck you\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,6*PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y-297)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y-297)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y-297)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] + 1,addressCell[1] - 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck zuoxia\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(i-j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,3*PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+410),api.GridToCell(api.GetSelfInfo()->y)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] + 1,addressCell[1] ) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck xia\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(i<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,4*PI/4);
			}

		}
		else if(api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)2 || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)4  || api.GetPlaceType(api.GridToCell(api.GetSelfInfo()->x+297),api.GridToCell(api.GetSelfInfo()->y+297)) == (enum THUAI6::PlaceType)5 || api.GetPlaceType(addressCell[0] + 1,addressCell[1] + 1) == (enum THUAI6::PlaceType)11)
		{
			api.Print("Tricker is stuck youxia\n");
			for(int i = addressCell[0] - 1; i <= addressCell[0] + 1; i++)
			{
				int alreadyFind;
				for(int j = addressCell[1] - 1; j <= addressCell[1] + 1; j++)
				{
					alreadyFind = 0;
					if((api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)1)&&(i+j<=0))
					{
						api.Move(50,aroundDir(i - addressCell[0],j - addressCell[0]));
						api.Print("already move");
						alreadyFind = 1;
						break;
					}
				}
				if(alreadyFind == 1)
					break;
			}

			// 补救措施
			int addressThen[2] ={api.GetSelfInfo()->x, api.GetSelfInfo()->y};
			if(isStuck(beforeTrickerAddress,addressThen))
			{
				api.Move(50,5*PI/4);
			}

		}
		else
		{
			didnotMove=1;
			api.MoveRight(50);
			api.Print("Stuck, but no wall");
		}
	}

	// 如果视野内有学生，进入跟随模式
	else if(numTarget > 0)
	{
		api.Print("numberTarget>0");
		for(int i = 0; i < (int)studentInfo.size(); i++)
		{
			studentInView[i][0] = (*studentInfo[i]).x;
			studentInView[i][1] = (*studentInfo[i]).y;
			studentInViewCell[i][0] = api.GridToCell((*studentInfo[i]).x);
			studentInViewCell[i][1] = api.GridToCell((*studentInfo[i]).y);
		}

		/*******寻找目标*******/
		double dist[5];
		for(int i = 0; i < (int)studentInfo.size(); i++)
		{
			dist[i] = sqrt(((*studentInfo[i]).x - api.GetSelfInfo()->x) * ((*studentInfo[i]).x - api.GetSelfInfo()->x) + ((*studentInfo[i]).y - api.GetSelfInfo()->y) * ((*studentInfo[i]).y - api.GetSelfInfo()->y));
			// 特殊情况，不攻击的对象
			/*if((*studentInfo[i]).studentType == THUAI6::StudentType::Teacher)
				dist[i] = 20000;*/
			if((*studentInfo[i]).determination == 0)
				dist[i] = 20000;
		}
		stuTarget = findMinIndex(dist,(int)studentInfo.size());
		attackOK = true;
		/**********************/

		if(distanceGrid(address,studentInView[stuTarget]) > 1000 && attackOK == true) // 追赶
		{
			api.Move(200,angleangleInRadian(address,studentInViewCell[stuTarget]));
		}

		if(distanceGrid(address,studentInView[stuTarget]) < 1500 && attackOK == true) // 进行攻击
		{
			api.Attack(angleangleInRadian(address,studentInViewCell[stuTarget]));
			api.Print("using common attack\n");
		}
	}

	// 如果有作业，也可以攻击
	/*else if()
	{

	}*/
	
	// 指导思想：见窗翻窗，见门过门，见草进草
	else if(findGrass(api) == true)
	{
		trickerInGrass(api);
	}

	// 翻窗
	else if((findWindow(api) == true)&&(waitWindow>1000))
	{
		api.SkipWindow();
		waitWindow=0;
		api.Print("Skip Window\n");
	}

	else if(bool_findWindowIn25(api)==true)
	{
		for(int i = addressCell[0] - 2; i <= addressCell[0] + 2; i++)
		{
			for(int j = addressCell[1] - 2; j <= addressCell[1] + 2; j++)
			{
				if(api.GetPlaceType(i,j) == (enum THUAI6::PlaceType)7)
				{
					int aWindow[2]={i,j};
					api.Move(100,angleangleInRadian(address,aWindow));
					api.Print("Move to Window\n");
				}
			}
		}
	}
	// 最后的选择，随机游走
	else
	{
		if(CheckBlocekd(api,address,directionTricker) == false)
			api.Move(200,directionTricker);
		else
		{
			directionTricker += Turn(api,address,directionTricker);
			api.Move(200,directionTricker);
		}
	}

	beforeTrickerAddress[0] = api.GetSelfInfo()->x; // 为了记录上一帧坐标，写在最后
	beforeTrickerAddress[1] = api.GetSelfInfo()->y;
}