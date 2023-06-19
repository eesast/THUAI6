#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include<math.h>
#include<queue>
#include<list>
#include<algorithm>
#include<chrono>
#include <iostream>
#define P2I 0.785398163397425

//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！
//一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！一切恐惧源于性能不足！！！


// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;

struct mapnode {
	int x, y;
};
static std::vector<mapnode> path;
static int scurstate = 3;
static bool initialized;
static std::vector< std::vector<int>> wallmap(50, std::vector<int>(50));
static std::vector<mapnode> homeworkmap;
//卷图，标记作业进度
static int juanmap[10] = { 0 };
static std::vector<mapnode> xiaomenmap;
static std::vector<mapnode> grassmap;
static int juaned[10] = { 0 };
//static std::vector<mapnode> routemap = { {22,15},{25,7},{40,20},{41,31},{30,35},{15,40},{8,35} };
static std::vector<mapnode> routemap = { {6,25},{4,31},{14,39},{21,44},{23,40},{29,34},{29,25},{40,24},{25,6},{15,15} };
static std::vector<mapnode> routemap2 = { {31,4},{41,4},{45,8},{45,20},{45,29},{30,31},{21,31},{20,45},{8,45},{4,40}, {4,29} , {4,20}, {19,18}, {28,18} };
//mapflag 0为1图，1为2图
static bool mapflag;
void Move(mapnode n1, mapnode n2);
struct info {
	mapnode nownode;
	mapnode nowgrid;
	double CD[3];
	//std::string message;
	double dangeralert;
	std::vector<std::shared_ptr<const THUAI6::Tricker>> trickerinfo;
};
static int direction = 0;
static bool captain = false;
//Astar的锁
static bool unlockAstar = 1;
//发送集合消息的锁
static bool unlockpos = 1;
//是否开始写作业的锁
const int d8x[8] = { 1,1,0,-1,-1,-1,0,1 };
const int d8y[8] = { 0,1,1,1,0,-1,-1,-1 };
//走路用的迭代器
static int counter = 0;
//状态表
static int curstate = 4;
//上一次tonode
static mapnode lastpos = { -1,-1 };
//上一次位置
static mapnode lastgrid = { -1,-1 };
//卡人角度
static int d30x[4] = { 700,400,-700,-400 };
static int d30y[4] = { -400,700,400,-700 };
//作业是否写完
static bool juanedmap[10] = { 0 };
//辅助计算函数

//1.计算距离
double distant(mapnode A, mapnode B) {
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

//2.计算最短路径
//以下是我网上抄的一个A星算法，实例化以后（如 Astar A），直接用A.savetopath（格点 A，格点 B，路径 path），路会自动存到path里，每次存之前会自动清空path，不要使用其他接口，不要改变其中的变量
//***********************************************************************************************************************
const int kCost1 = 10;
const int kCost2 = 14;
struct Point

{

	int x, y; //点坐标，这里为了方便按照C++的数组来计算，x代表横排，y代表竖列

	int F, G, H; //F=G+H

	Point* parent; //parent的坐标，这里没有用指针，从而简化代码

	Point(int _x, int _y)//变量初始化

	{

		x = _x;

		y = _y;

		F = 0;

		G = 0;

		H = 0;

		parent = NULL;

	}

};
class Astar

{

public:

	void InitAstar(std::vector<std::vector<int>>& _maze);

	std::list<Point*> GetPath(Point& startPoint, Point& endPoint, bool isIgnoreCorner);

	void savetopath(mapnode A, mapnode B, std::vector<mapnode>& path);


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

	std::vector<std::vector<int>> maze;

	std::list<Point*> openList; //开启列表

	std::list<Point*> closeList; //关闭列表

};
void Astar::savetopath(mapnode A, mapnode B, std::vector<mapnode>& path) {
	path.resize(0);
	Point start(A.x, A.y);
	Point end(B.x, B.y);
	auto road = GetPath(start, end, false);
	for (auto& point : road) {
		path.push_back({ point->x,point->y });
	}
}
void Astar::InitAstar(std::vector<std::vector<int>>& _maze)

{

	maze = _maze;

}
int Astar::calcG(Point* temp_start, Point* point)

{

	int extraG = (abs(point->x - temp_start->x) + abs(point->y - temp_start->y)) == 1 ? kCost1 : kCost2;//条件表达式，第一个真就是b，不然是c

	int parentG = point->parent == NULL ? 0 : point->parent->G; //如果是初始节点，则其父节点是空

	return parentG + extraG;

}
int Astar::calcH(Point* point, Point* end)

{

	return ((end->x - point->x) + end->y - point->y) * kCost1;//欧几里得距离

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

	if (target->x<0 || target->x>maze.size() - 1

		|| target->y<0 || target->y>maze[0].size() - 1

		|| maze[target->x][target->y] == 0//0代表障碍物

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

			if (maze[point->x][target->y] != 0 && maze[target->x][point->y] != 0 && maze[target->x][target->y] != 2)

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
static int tcurstate = 1;

void pathmerging(mapnode A, mapnode B, std::vector<mapnode>& path, IStudentAPI& api) {
	//api.Print("1");
	path.resize(0);
	//api.Print("2");
	counter = 0;
	Astar Astar;
	//api.Print("3");
	Astar.InitAstar(wallmap);
	//api.Print("4");
	Astar.savetopath(A, B, path);
	//api.Print("5");
}
void pathmerging(mapnode A, mapnode B, std::vector<mapnode>& path, ITrickerAPI& api) {
	//api.Print("1");
	path.resize(0);
	//api.Print("2");
	counter = 0;
	Astar Astar;
	//api.Print("3");
	Astar.InitAstar(wallmap);
	//api.Print("4");
	Astar.savetopath(A, B, path);
	//api.Print("5");
}
//************************************************************************************************************************

//3.初始化地图,初始化了草地、作业、正常校门的位置数组，初始化了墙图（0不可以走,1是草地和空地，2是窗户）

void initialmap(IAPI& api) {
	/*api.Print("initialmap called");*/
	auto map = api.GetFullMap();
	if (api.GetPlaceType(5, 6) == THUAI6::PlaceType::Gate)
		mapflag = 0;
	else
		mapflag = 1;
	if (1) {
		int i, j;
		for (i = 0; i < 50; i++)
			for (j = 0; j < 50; j++)
			{
				if (map[i][j] == THUAI6::PlaceType::Land || map[i][j] == THUAI6::PlaceType::Grass || map[i][j] == THUAI6::PlaceType::Door3 || map[i][j] == THUAI6::PlaceType::Door6 || map[i][j] == THUAI6::PlaceType::Door5) {
					wallmap[i][j] = 1;

				}
				else if (map[i][j] == THUAI6::PlaceType::Window) {
					wallmap[i][j] = 2;
				}
				else {
					wallmap[i][j] = 0;
				}
				if (map[i][j] == THUAI6::PlaceType::ClassRoom) {

					mapnode temp = { api.CellToGrid(i),api.CellToGrid(j) };
					homeworkmap.emplace_back(temp);
					/*for (int k = 0; k < 8; k++)
						if (map[i + d8x[k]][j + d8y[k]] == THUAI6::PlaceType::Grass || map[i + d8x[k]][j + d8y[k]] == THUAI6::PlaceType::Land) {
							mapnode temp1 = { i + d8x[k],j + d8y[k] };
							routemap.emplace_back(temp1);
							break;*/
							/*}*/
				}
				else if (map[i][j] == THUAI6::PlaceType::Grass) {
					mapnode temp = { api.CellToGrid(i),api.CellToGrid(j) };
					grassmap.emplace_back(temp);
				}
				else if (map[i][j] == THUAI6::PlaceType::Gate) {
					mapnode temp = { api.CellToGrid(i),api.CellToGrid(j) };
					xiaomenmap.emplace_back(temp);
					//for (int k = 0; k < 8; k++)
					//	if (map[i + d8x[k]][j + d8y[k]] == THUAI6::PlaceType::Grass || map[i + d8x[k]][j + d8y[k]] == THUAI6::PlaceType::Land) {
					//		mapnode temp1 = { i + d8x[k],j + d8y[k] };
					//		routemap.emplace_back(temp1);
					//		break;
					//	}
				}
			}
		/*initialized = 1;*/
	}
	/*api.Print("initialmap eneded");*/
}

mapnode gridtonode(mapnode grid);

//4.计算应该走的方向
void Move(mapnode n1, mapnode n2)
{
	if (n1.x > n2.x && n1.y == n2.y)direction = 1;
	if (n1.x > n2.x && n1.y > n2.y)direction = 2;
	if (n1.x == n2.x && n1.y > n2.y)direction = 3;
	if (n1.x < n2.x && n1.y > n2.y)direction = 4;
	if (n1.x < n2.x && n1.y == n2.y)direction = 5;
	if (n1.x < n2.x && n1.y < n2.y)direction = 6;
	if (n1.x == n2.x && n1.y < n2.y)direction = 7;
	if (n1.x > n2.x && n1.y < n2.y)direction = 8;
}

//5.计算哪个东西距离自己最近
mapnode mindisgrid(mapnode nowgrid, std::vector<mapnode>& candidate) {
	int d = 2147483647, k = 0;
	for (int i = 0; i < candidate.size(); i++) {
		if (distant(nowgrid, candidate[i]) < d) {
			d = distant(nowgrid, candidate[i]);
			k = i;
		}
	}
	mapnode temp = candidate[k];
	return temp;
}
//作业专用
mapnode mindisgrid(mapnode nowgrid, std::vector<mapnode>& candidate, IStudentAPI& api) {
	int d = 2147483647, k = 0;
	for (int i = 0; i < 10; i++) {
		//api.Print(fmt::format("{}", juanedmap[i]));
		if ((distant(nowgrid, candidate[i]) < d)&&!juanedmap[i]) {
			d = distant(nowgrid, candidate[i]);
			k = i;
		}
	}
	mapnode temp = candidate[k];
	/*api.Print("sssend");*/
	return temp;
}

//6.坐标转换
mapnode gridtonode(mapnode grid) {
	mapnode temp;
	temp.x = grid.x / 1000;
	temp.y = grid.y / 1000;
	return temp;
}
mapnode nodetogrid(mapnode node) {
	mapnode temp;
	temp.x = node.x * 1000 + 500;
	temp.y = node.y * 1000 + 500;
	return temp;
}

//7.站位计算
static int dx[4] = { 1,0,-1,0 };
static int dy[4] = { 0,-1,0,1 };
mapnode poscal(IAPI& api, mapnode node) {
	std::vector<mapnode> temp;
	for (int i = 0; i < 4; i++) {
		if (wallmap[node.x + dx[i]][node.y + dy[i]]!=0) {
			mapnode node1;
			node1 = { (node.x + dx[i]),(node.y + dy[i]) };
			temp.emplace_back(node1);
			break;
		}
	}
	return temp[0];
}

//8.查找点是否在其中
bool findin(std::vector<mapnode>path, mapnode node) {
	for (int i = 0; i < path.size(); i++) {
		if (path[i].x == node.x && path[i].y == node.y)
			return 1;
	}
	return 0;
}
//*************************************************************************************************************************




//基本状态，封装了走路函数
class basicstate
{
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	info nowinfo;
	basicstate(IStudentAPI& api);
	virtual ~basicstate();
	void walkpath(IStudentAPI& api);
	int isarrive(IStudentAPI& api);
	bool isskipwindow();
	void skipwindow(IStudentAPI& api);
	bool isarrivend(IStudentAPI& api);
	void walkcomp(IStudentAPI& api, mapnode grid);
	virtual void print(IStudentAPI& api) { 
		//api.Print(":aa");
	}
	mapnode tonode;
	bool stuck;
};
//每次play更新信息
basicstate::basicstate(IStudentAPI& api) {
	//api.Print("initself called");
	auto self = api.GetSelfInfo();
	nowinfo.nownode.x = api.GridToCell(self->x);
	nowinfo.nownode.y = api.GridToCell(self->y);
	nowinfo.nowgrid.x = self->x;
	nowinfo.nowgrid.y = self->y;
	if (distant(nowinfo.nowgrid, lastgrid) < 0.1)
		stuck = 1;
	else
		stuck = 0;
	lastgrid.x = self->x;
	lastgrid.y = self->y;
	for (int i = 0; i < 3; i++) {
		nowinfo.CD[i] = self->timeUntilSkillAvailable[i];
	}
	nowinfo.dangeralert = self->dangerAlert;
	nowinfo.trickerinfo = api.GetTrickers();
	auto belf = api.GetStudents();
	//api.Print(fmt::format("{}", belf.size()));
	int j=0;
	if (self->studentType == THUAI6::StudentType::StraightAStudent) {
		for (int i = 0; i < 4; i++) {
			if (belf[i]->playerID != self->playerID) {
				mapnode tt = gridtonode({ belf[i]->x, belf[i]->y });
				wallmap[tt.x][tt.y] = 0;
				if (belf[i]->studentType == THUAI6::StudentType::Teacher)
					for (int i = 0; i < 8; i++) {
						mapnode temp = { tt.x + d8x[i],tt.y + d8y[i] };
						if (temp.x >= 0 || temp.x < 50 || temp.y >= 0 || temp.y < 50)
							wallmap[temp.x][temp.y] = 0;
					}
			}
			if (!nowinfo.trickerinfo.empty()) {
				mapnode tt = gridtonode({ nowinfo.trickerinfo[0]->x, nowinfo.trickerinfo[0]->y });
				wallmap[tt.x][tt.y] = 0;
			}
		}
		if (api.HaveMessage()) {
			auto message=api.GetMessage().second;
			std::string nodex;
			std::string nodey;
			int i = 0;
			auto f = message.find(',');
			nodex = message.substr(0, f);
			int xx = atoi(nodex.c_str());
			auto l = message.size();
			nodey = message.substr(f + 1, l - f - 1);
			int yy = atoi(nodey.c_str());
			xx = api.CellToGrid(xx);
			yy = api.CellToGrid(yy);
			int s;
			for ( s = 0; s < 10; s++)
				if (homeworkmap[s].x == xx && homeworkmap[s].y == yy)
					break;
			juanedmap[s] = 1;
		}
		if (!api.GetTrickers().empty()) {
			mapnode node = { api.GridToCell(api.GetTrickers()[0]->x),api.GridToCell(api.GetTrickers()[0]->y) };
			api.SendTextMessage(0, fmt::format("{},{}", node.x, node.y));
			api.SendTextMessage(2, fmt::format("{},{}", node.x, node.y));
		}
	}
	//for (int m = 0; m < 10; m++)
	//	api.Print(fmt::format("{}", juanedmap[m]));
}
//学生好像需要记忆
basicstate::~basicstate() {
	lastpos = tonode;
}

//沿路径走
//判断是否到达目标并且改变方向和迭代器
int basicstate::isarrive(IStudentAPI& api) {
	//api.Print(fmt::format("{},{}", path[counter].x, path[counter].y));
	if (path.empty())
		return 0;
	else if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) <= 100) {
		/*double angle = atan2((path[counter].y - nowinfo.nowgrid.y), (path[counter].x - nowinfo.nowgrid.x));
		int time = distant(nowinfo.nowgrid, nodetogrid(path[counter])) * 1000 / Constants::Sunshine::moveSpeed;
		api.Move(time,angle).get();*/
		//api.Print("arrivedpoint");
		Move(path[counter], path[counter + 1]);
		counter++;
		//api.Print(fmt::format("{}", counter));
		return 1;
	}
	//else if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) >= 2000) {
	//	/*api.Print("pianli called");
	//	unlockAstar = 1;
	//	unlockpos = 1;
	//	return 2;*/
	//}
	else return 0;
}
//还是得位置补偿。。。
void basicstate::walkcomp(IStudentAPI& api, mapnode grid) {
	//api.Print("walkcomp called");
	double angle = atan2((grid.y - nowinfo.nowgrid.y), (grid.x - nowinfo.nowgrid.x));
	int time;
	if (api.GetSelfInfo()->studentType == THUAI6::StudentType::Teacher) {
		time = distant(grid, nowinfo.nowgrid) * 1000 / 2700;
	}
	else {
		time = distant(grid, nowinfo.nowgrid) * 1000 / 2880;
	}
	//api.Print(fmt::format("{},{}", nowinfo.nowgrid.y, nowinfo.nowgrid.x));
	//api.Print(fmt::format("time{}", time));
	/* api.Move(time, angle);
	 if (time < 200) {
		 std::this_thread::sleep_for(std::chrono::milliseconds(time));
	 }
	 else {
		 std::this_thread::sleep_for(std::chrono::milliseconds(200));
		 api.EndAllAction();
	 }*/
}
//判断是否到达终点
bool basicstate::isarrivend(IStudentAPI& api) {
	if (distant(nodetogrid(tonode), nowinfo.nowgrid) <= 100) {
		//api.Print("arrived");
		walkcomp(api, nodetogrid(tonode));
		counter = 0;
		return 1;
	}
	else if (path.empty()) {
		return 0;
	}
	else {
		if (distant(nowinfo.nowgrid, { api.CellToGrid(path[counter].x),api.CellToGrid(path[counter].y) }) <= 100 && counter == (path.size() - 1)) {
			//api.Print("arrived pathend");
			walkcomp(api, nodetogrid(path.back()));
			counter = 0;
			return 1;
		}
		else return 0;
	}
}
//判断是否需要进行翻窗
bool basicstate::isskipwindow()
{
	if (wallmap[path[counter].x][path[counter].y] == 2)
		return true;
	else
		return 0;
}
//进行翻窗并改变迭代器
void basicstate::skipwindow(IStudentAPI& api) {
	if (api.SkipWindow().get()) {
		counter++;
	}
}
//根据方向执行走路命令
void basicstate::walkpath(IStudentAPI& api) {
	if (api.GetSelfInfo()->studentType == THUAI6::StudentType::Teacher) {
		if (direction == 1) {
			//api.EndAllAction(); 
			 api.MoveUp(92);
			std::this_thread::sleep_for(std::chrono::milliseconds(102));
		}
		if (direction == 2) {
			//api.EndAllAction(); 
			 api.Move(131, P2I * 5);
			std::this_thread::sleep_for(std::chrono::milliseconds(141));
		}
		if (direction == 3) {
			//api.EndAllAction(); 
			 api.MoveLeft(92);
			 std::this_thread::sleep_for(std::chrono::milliseconds(102));
		}
		if (direction == 4) {
			/*api.EndAllAction(); */
			 api.Move(131, P2I * 7);
			 std::this_thread::sleep_for(std::chrono::milliseconds(141));
		}
		if (direction == 5) {
			/*	api.EndAllAction(); */
			 api.MoveDown(92);
			 std::this_thread::sleep_for(std::chrono::milliseconds(102));
		}
		if (direction == 6) {
			//api.EndAllAction(); 
			 api.Move(131, P2I);
			 std::this_thread::sleep_for(std::chrono::milliseconds(141));
		}
		if (direction == 7) {
			//api.EndAllAction(); 
			 api.MoveRight(92);
			 std::this_thread::sleep_for(std::chrono::milliseconds(102));
		}
		if (direction == 8) {
			//api.EndAllAction(); 
			 api.Move(131, P2I * 3);
			 std::this_thread::sleep_for(std::chrono::milliseconds(141));
		}
	}
	//api.Print(fmt::format("{}", direction));
	//api.Print(fmt::format("{}", counter));
	//api.Print("walkpath used!");
	else {
		if (direction == 1) {
			//api.EndAllAction(); 
			 api.MoveUp(87);
			std::this_thread::sleep_for(std::chrono::milliseconds(97));
		}
		if (direction == 2) {
			//api.EndAllAction(); 
			 api.Move(123, P2I * 5);
			std::this_thread::sleep_for(std::chrono::milliseconds(133));
		}
		if (direction == 3) {
			//api.EndAllAction(); 
			 api.MoveLeft(87);
			 std::this_thread::sleep_for(std::chrono::milliseconds(97));
		}
		if (direction == 4) {
			/*api.EndAllAction(); */
			 api.Move(123, P2I * 7);
			std::this_thread::sleep_for(std::chrono::milliseconds(133));
		}
		if (direction == 5) {
			/*	api.EndAllAction(); */
			 api.MoveDown(87);
			 std::this_thread::sleep_for(std::chrono::milliseconds(97));
		}
		if (direction == 6) {
			//api.EndAllAction(); 
			 api.Move(123, P2I);
			std::this_thread::sleep_for(std::chrono::milliseconds(133));
		}
		if (direction == 7) {
			//api.EndAllAction(); 
			 api.MoveRight(87);
			 std::this_thread::sleep_for(std::chrono::milliseconds(97));
		}
		if (direction == 8) {
			//api.EndAllAction(); 
			 api.Move(123, P2I * 3);
			std::this_thread::sleep_for(std::chrono::milliseconds(133));
		}
	}
}
//用于测试走路代码的，没啥用
static bool test = 1;
void basicstate::statemove(IStudentAPI& api) {
	/*if (unlockAstar) {
		pathmerging(nowinfo.nownode, { 19,15 }, path, api);
		for (int i = 0; i < path.size(); i++)
			api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (isarrivend(api)) { std::this_thread::sleep_for(std::chrono::seconds(100)); }
	else {
		isarrive(api);
		if (!isskipwindow()) {
			walkpath(api);
		}
		else {
			skipwindow(api);
		}
	}*/
}
void basicstate::statetransfer(IStudentAPI& api) {
	;
}
//*************************************************************************************************************************
static std::vector < mapnode> initnode1 = { { 21,25 }, {15,40},{40,30},{25,10} };
static std::vector < mapnode> initnode2 = { {20,20}, {30,20} ,{30,35},{20,35} };
static mapnode meg;
static int routecounte = 0;
//状态四，初始行动
class initmove :public basicstate {
public:
	virtual ~initmove() { ; }
	initmove(IStudentAPI& api) :basicstate(api) { ; }
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	virtual void print(IStudentAPI& api) {
		//api.Print("sssaa");
	}
	bool getmes(IStudentAPI& api);
};
bool initmove::getmes(IStudentAPI& api) {
	if (api.HaveMessage()) {
		auto a = api.GetMessage();
		a.second;
		std::string nodex;
		std::string nodey;
		int i = 0;
		auto f = a.second.find(',');
		nodex = a.second.substr(0, f);
		//api.Print(nodex);
		int xx = atoi(nodex.c_str());
		//api.Print(fmt::format("{}", xx));
		auto l = a.second.size();
		nodey = a.second.substr(f + 1, l - f - 1);
		//api.Print(nodey);
		int yy = atoi(nodey.c_str());
		meg = { xx,yy };
		return true;
	}
	else
		return false;
}

void initmove::statemove(IStudentAPI& api) {
	//api.Print("aaaaaaa");
	if(mapflag==0)
		tonode = initnode1[routecounte];
	else
		tonode= initnode2[routecounte];
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{}\n", path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api)) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}
			//else if (flag == 2) {
			//	;
			{
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			unlockAstar = 1;
			routecounte++;
			if (mapflag == 0)
				routecounte %= initnode1.size();
			else
				routecounte %= initnode2.size();
		}
	}
}

void initmove::statetransfer(IStudentAPI& api) {
	//api.Print(fmt::format("{}", api.GetTrickers().empty()));
	if (stuck) {
		curstate = 4;
	}
	else if (getmes(api)) {
		unlockAstar = 1;
		curstate = 7;
	}
	else if (!api.GetTrickers().empty()) {
		unlockAstar = 1;
		curstate = 5;
	}
}

//*************************************************************************************************************************
int zone(mapnode self, mapnode target);
//状态五，卡人,初步接近
class lesson :public basicstate {
public:
	virtual ~lesson() { ; }
	lesson(IStudentAPI& api) :basicstate(api) { ; }
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	void infosend(IStudentAPI& api);
	void roughpos(IStudentAPI& api);
};

void lesson::infosend(IStudentAPI& api) {
	if (!nowinfo.trickerinfo.empty()) {
		auto a = (zone(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y }) + 2) % 4;
		//api.Print(fmt::format("{},{}", a, zone(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y })));
		int x = nowinfo.trickerinfo[0]->x / 1000;
		int y = nowinfo.trickerinfo[0]->y / 1000;
		/*switch (a) {
		case(1): {
			x = x - 1;
			y = y - 1;
			break;
		}
		case(2): {
			x += 1;
			y -= 1;
			break;
		}
		case(3): {
			x += 1;
			y += 1;
			break;
		}
		case(4): {
			x -= 1;
			y += 1;
			break;
		}
		}*/
		if (api.GetSelfInfo()->playerID == 2)
			api.SendBinaryMessage(0, fmt::format("{},{}", x, y));
		else if (api.GetSelfInfo()->playerID == 0)
			api.SendBinaryMessage(2, fmt::format("{},{}", x, y));
	}
}

void lesson::roughpos(IStudentAPI& api) {
	if (!nowinfo.trickerinfo.empty()) {
		int d = 100000000;
		int j;
		mapnode g;
		for (int i = 1; i < 8; i = i + 2) {
			auto place = api.GetPlaceType(api.GridToCell(nowinfo.trickerinfo[0]->x), api.GridToCell(nowinfo.trickerinfo[0]->y));

			if (place == THUAI6::PlaceType::Grass || place == THUAI6::PlaceType::Land) {
				mapnode aaa = gridtonode({ nowinfo.trickerinfo[0]->x, nowinfo.trickerinfo[0]->y });
				aaa.x += d8x[i];
				aaa.y += d8y[i];
				int temp = distant(nowinfo.nowgrid, nodetogrid(aaa));
				if (temp < d)
				{
					d = temp;
					g.x = aaa.x;
					g.y = aaa.y;
				}
			}

		}

		if (g.x != lastpos.x && g.y != lastpos.y) {
			tonode = g;
			unlockAstar = 1;
		}
	}
}

void lesson::statemove(IStudentAPI& api) {
	//roughpos(api);
	mapnode temp = gridtonode({ nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y });
	if (lastpos.x != temp.x && lastpos.y != temp.y) {
		tonode = temp;
		unlockAstar = 1;
	}
	infosend(api);
	//api.Print("bnbbbb");
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		if (!path.empty()) {
			//api.Print(fmt::format("{}\n", path.size()));
			//for (int i = 0; i < path.size(); i++)
			//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		}
		unlockAstar = 0;
	}
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api)) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}
			//else if (flag == 2) {
			//	;
			{
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			unlockAstar = 1;
		}
	}
}

void lesson::statetransfer(IStudentAPI& api) {
	if (stuck)
		curstate = 9;
	else if (nowinfo.trickerinfo.empty()) {
		curstate = 4;
	}
	else if ((distant(nowinfo.nowgrid, {nowinfo.trickerinfo[0]->x, nowinfo.trickerinfo[0]->y}))<2100) {
		curstate = 6;
	}
}


//*************************************************************************************************************************

int zone(mapnode self, mapnode target) {
	//一象限，以此类推
	if (self.x >= target.x && self.y > target.y)
		return 1;
	else if (self.x < target.x && self.y >= target.y)
		return 2;
	else if (self.x <= target.x && self.y < target.y)
		return 3;
	else if (self.x > target.x && self.y <= target.y)
		return 4;
	else
		return 5;
}
//状态六，卡人,接近微操
class teach :public basicstate {
public:
	virtual ~teach() { ; }
	teach(IStudentAPI& api) :basicstate(api) { ; }
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	void sendinfo(IStudentAPI& api);
	bool poscheck(IStudentAPI& api);
	mapnode actpos();
};

bool teach::poscheck(IStudentAPI& api) {
	bool flag=1;
	for (int i = 0; i < 8; i++) {
		mapnode temp = { api.GridToCell(nowinfo.trickerinfo[0]->x)+d8x[i],api.GridToCell(nowinfo.trickerinfo[0]->y)+d8y[i]};
		if (api.GetPlaceType(temp.x, temp.y) == THUAI6::PlaceType::Land || api.GetPlaceType(temp.x, temp.y) == THUAI6::PlaceType::Grass)
			flag = flag;
		else
			flag = 0;
	}
	return flag;
}

mapnode teach::actpos() {
	auto zo = zone(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x, nowinfo.trickerinfo[0]->y });
	mapnode temp = { -1,-1 };
	switch (zo) {
	case(1): {
		temp = { nowinfo.trickerinfo[0]->x + 400,nowinfo.trickerinfo[0]->y + 700 };
		return temp;
		break;
	}
	case(2): {
		temp = { nowinfo.trickerinfo[0]->x -700,nowinfo.trickerinfo[0]->y + 400 };
		return temp;
		break;
	}
	case(3): {
		temp = { nowinfo.trickerinfo[0]->x -400,nowinfo.trickerinfo[0]->y -700 };
		return temp;
		break;
	}
	case(4): {
		temp = { nowinfo.trickerinfo[0]->x + 700,nowinfo.trickerinfo[0]->y -400 };
		return temp;
		break;
	}
	}
	return temp;
}

void teach::sendinfo(IStudentAPI& api) {
	if (!nowinfo.trickerinfo.empty()) {
		auto a = (zone(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y })) % 4;
		//api.Print(fmt::format("{},{}", a, zone(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y })));
		int x = nowinfo.trickerinfo[0]->x / 1000;
		int y = nowinfo.trickerinfo[0]->y / 1000;
		/*switch (a) {
		case(1): {
			x = x- 1;
			y = y - 1;
			break;
		}
		case(2): {
			x+=1;
			y-=1;
			break;
		}
		case(3): {
			x+=1;
			y+=1;
			break;
		}
		case(0): {
			x-=1;
			y+=1;
			break;
		}
		}*/
			if (api.GetSelfInfo()->playerID == 2)
				api.SendBinaryMessage(0, fmt::format("{},{}", x, y));
			else if (api.GetSelfInfo()->playerID == 0)
				api.SendBinaryMessage(2, fmt::format("{},{}", x, y));
		//}
	}
}

void teach::statemove(IStudentAPI& api) {
	if (api.GetSelfInfo()->playerID == 2) {
		tonode = actpos();
		sendinfo(api);
		if (distant(nowinfo.nowgrid, tonode) > 1) {
			double angle1 = -atan2((api.GetTrickers()[0]->y - api.GetSelfInfo()->y), (api.GetTrickers()[0]->x - api.GetSelfInfo()->x));
			int time1 = 20;
			api.EndAllAction();
			auto b = api.Move(time1, angle1).get();
			api.Wait();
			double angle = atan2((tonode.y - api.GetSelfInfo()->y), (tonode.x - api.GetSelfInfo()->x));
			int time = distant(tonode, { api.GetSelfInfo()->x ,api.GetSelfInfo()->y }) * 1000 / 2700;

			api.EndAllAction();
			auto a = api.Move(time, angle).get();
		}
		if (nowinfo.CD[0] < 0.1)
			if (nowinfo.trickerinfo[0]->playerState == THUAI6::PlayerState::Attacking || nowinfo.trickerinfo[0]->playerState == THUAI6::PlayerState::Swinging)
				api.UseSkill(0).get();
	}
	else {
		std::shared_ptr<const THUAI6::Student> mate;
		for (int i = 0; i < 4; i++) {
			if (api.GetStudents()[i]->playerID == 2)
				mate = api.GetStudents()[i];
		}
		sendinfo(api);
		auto b=zone({ mate->x,mate->y }, { nowinfo.trickerinfo[0]->x, nowinfo.trickerinfo[0]->y });
		int a = (b + 2) % 4;
		switch (a) {
		case(1): {
			tonode = { nowinfo.trickerinfo[0]->x + 400,nowinfo.trickerinfo[0]->y + 700 };
			break;
		}
		case(2): {
			tonode = { nowinfo.trickerinfo[0]->x - 700,nowinfo.trickerinfo[0]->y + 400 };
			break;
		}
		case(3): {
			tonode = { nowinfo.trickerinfo[0]->x - 400,nowinfo.trickerinfo[0]->y - 700 };
			break;
		}
		case(0): {
			tonode = { nowinfo.trickerinfo[0]->x + 700,nowinfo.trickerinfo[0]->y - 400 };
			break;
		}
		}
		/*if(!(api.GetPlaceType(tonode.x/1000, tonode.y / 1000)==THUAI6::PlaceType::Grass|| api.GetPlaceType(tonode.x / 1000, tonode.y / 1000) == THUAI6::PlaceType::Land))
		for (int i = 0; i < 4; i++) {
			if (b != i) {
				tonode = { nowinfo.trickerinfo[0]->x + d30x[i],nowinfo.trickerinfo[0]->y + d30y[i] };
				if (api.GetPlaceType(tonode.x / 1000, tonode.y / 1000) == THUAI6::PlaceType::Grass || api.GetPlaceType(tonode.x / 1000, tonode.y / 1000) == THUAI6::PlaceType::Land)
					break;
			}
		}*/
		if (distant(nowinfo.nowgrid, tonode) > 20) {
		double angle1 = -atan2((api.GetTrickers()[0]->y - api.GetSelfInfo()->y), (api.GetTrickers()[0]->x - api.GetSelfInfo()->x));
		int time1 = 20;
		auto b = api.Move(time1, angle1);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		double angle = atan2((tonode.y - api.GetSelfInfo()->y), (tonode.x - api.GetSelfInfo()->x));
		int time = distant(tonode, { api.GetSelfInfo()->x ,api.GetSelfInfo()->y }) * 1000 / 2700;
		auto a = api.Move(time, angle);
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
		}
		if (nowinfo.CD[0] < 0.1)
			if (nowinfo.trickerinfo[0]->playerState == THUAI6::PlayerState::Attacking || nowinfo.trickerinfo[0]->playerState == THUAI6::PlayerState::Swinging)
				api.UseSkill(0).get();
	}

}

void teach::statetransfer(IStudentAPI& api) {
	if (api.GetTrickers().empty())
		curstate = 4;
	/*else if(poscheck(api))
		curstate = 4;*/
	else if (distant(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y }) > 3000)
		curstate = 5;
}

//*************************************************************************************************************************
//状态七，收信息行动
class wego:public basicstate {
public:
	virtual ~wego() { ; }
	wego(IStudentAPI& api) :basicstate(api) { ; }
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	bool getmes(IStudentAPI& api);
};

bool wego::getmes(IStudentAPI& api) {
	if (api.HaveMessage()) {
		auto a = api.GetMessage();
		a.second;
		std::string nodex;
		std::string nodey;
		int i = 0;
		auto f = a.second.find(',');
		nodex = a.second.substr(0, f);
		//api.Print(nodex);
		int xx = atoi(nodex.c_str());
		//api.Print(fmt::format("{}", xx));
		auto l = a.second.size();
		nodey = a.second.substr(f + 1, l - f - 1);
		//api.Print(nodey);
		int yy = atoi(nodey.c_str());
		if (meg.x != xx || meg.y != yy) {
			meg.x = xx;
			meg.y = yy;
			unlockAstar = 1;
		}
		return true;
	}
	else
		return false;
}

void wego::statemove(IStudentAPI& api) {
	getmes(api);
	tonode = meg;
	//if (stuck)
	//{
	//	api.Print("struck");
	//	unlockAstar = 1;
	//	walkcomp(api, nodetogrid(nowinfo.nownode));
	//	
	//}
	//api.Print(fmt::format("{},{},{}", tonode.x, tonode.y, nowinfo.trickerinfo.empty()));
	//if (!api.GetTrickers().empty()) {
	//	int x = api.GridToCell(nowinfo.trickerinfo[0]->x);
	//	int y = api.GridToCell(nowinfo.trickerinfo[0]->y);
	//	wallmap [x][y] = 0;
	//	unlockAstar == 1;
	//}
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
			//api.Print(fmt::format("{}\n", path.size()));
			//for (int i = 0; i < path.size(); i++)
			//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	/*api.Print(fmt::format("{},{}", path[counter].x, path[counter].y));*/
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api)) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}
			{
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			unlockAstar = 1;
		}
	}
}

void wego::statetransfer(IStudentAPI& api) {
	if (stuck)
		curstate = 9;
	/*else if (!nowinfo.trickerinfo.empty())
		curstate = 5;*/
	else if (!nowinfo.trickerinfo.empty()) {
		if (distant(nowinfo.nowgrid, { nowinfo.trickerinfo[0]->x,nowinfo.trickerinfo[0]->y }) < 2100) {
			/*api.Print(fmt::format("{},{},{},{}", nowinfo.nowgrid.x, nowinfo.nowgrid.y, nodetogrid(tonode).x, nodetogrid(tonode).y));*/
			curstate = 6;
		}
	}
}
//*************************************************************************************************************************
static bool insave = 0;
//状态九卡住重动
class retry :public basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	retry(IStudentAPI& api) :basicstate(api) { ; }
	virtual ~retry() { ; }
};
void retry::statemove(IStudentAPI& api) {
	//mapnode temp;
	//unlockAstar = 1;
	//for(int i=0;i<8;i++)
	//	if (wallmap[nowinfo.nownode.x + d8x[i]][nowinfo.nownode.y + d8y[i]]!=0){
	//		temp = { nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i] };
	//	}
	unlockAstar = 1;
	api.Wait();
	mapnode temp = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
	mapnode to = nodetogrid(gridtonode(temp));
	double angle = atan2(to.y - temp.y, to.x - temp.x);
	int time;
	if (api.GetSelfInfo()->studentType == THUAI6::StudentType::StraightAStudent)
		time = distant(temp, to) * 1000 / 2880;
	else
		time = distant(temp, to) * 1000 / 2700;
	api.Move(time, angle).get();
}
void retry::statetransfer(IStudentAPI& api) {
	/*if (!stuck||distant(nowinfo.nowgrid,nodetogrid(nowinfo.nownode))<10)*/
	if (api.GetSelfInfo()->studentType == THUAI6::StudentType::StraightAStudent) {
		bool flag = 1;
		for (int i = 0; i < 10; i++) {
			if (juanedmap[i]==false) {
				flag = 0;
				break;
			}
		}
		if(insave)
			scurstate = 10;
		else if(flag)
			scurstate = 2;
		else
			scurstate = 3;
	}
	else
		curstate = 4;
}
//*************************************************************************************************************************
//teacher完全体
class teacherall {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	teacherall(IStudentAPI& api);
	virtual ~teacherall();
	basicstate* nowstate;
};
teacherall::~teacherall() {
	delete (nowstate);
}

teacherall ::teacherall(IStudentAPI& api) {
	switch (curstate) {
	case 4: {nowstate = new initmove(api); 
		//api.Print("now  initmove");
		break; }
	case 5: {nowstate = new lesson(api);
		/*api.Print("now  lesson");*/
		break; }
	case 6: {nowstate = new teach(api);
	/*	api.Print("now  teach");*/
		break;}
	case 7: {nowstate = new wego(api);
		/*api.Print("now wego");*/
		break; }
	case 9: {nowstate = new retry(api);
		/*api.Print("now retry");*/
		break; }
	}
}
void teacherall::statemove(IStudentAPI& api) {
	//api.Print(fmt::format("now  statemove"));
	//nowstate->print(api);
	nowstate->statemove(api);
}
void teacherall::statetransfer(IStudentAPI& api) {
	auto a = curstate;
	nowstate->statetransfer(api);
	if (a == curstate) {
		;
	}
	else {
		delete(nowstate);
		switch (curstate) {
		case 4: {nowstate = new initmove(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
		/*	api.Print("now transto initmove"); */
			break; }
		case 5: {nowstate = new lesson(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
	/*		api.Print("now transto lesson"); */
			break; }
		case 6: {nowstate = new teach(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
	/*		api.Print("now transto teach");*/
			break; }
		case 7: {nowstate = new wego(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			/*api.Print("now transto wego");*/
			break;}
		case 9: {nowstate = new retry(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
		/*	api.Print("now transto retry");*/
			break; }
		}
	}
}


//*************************************************************************************************************************

//目前卷
static mapnode nowjuan;
// 状态一，写作业
class juan :virtual public basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	juan(IStudentAPI& api) :basicstate(api) { ; }
	virtual ~juan() { ; }
	void startjuan(IStudentAPI& api);
	bool juaned(IStudentAPI& api);
	void initialnowjuan(IStudentAPI& api);
	void useskill(IStudentAPI& api);
	void findin();
	bool tograduate(IStudentAPI& api);
	void sendjuaned(IStudentAPI& api);
};
void juan::findin() {
	auto temp = nodetogrid(nowjuan);
	for (int i = 0; i < homeworkmap.size(); i++) {
		if (nowjuan.x == homeworkmap[i].x && nowjuan.y == homeworkmap[i].y)
		{
			juanmap[i] = 1;
			break;
		}
	}
}
void juan::useskill(IStudentAPI& api) {
	if (nowinfo.CD[0] < 0.1)
		api.UseSkill(0).get();
}
void juan::initialnowjuan(IStudentAPI& api) {
	for (int i = 0; i < 8; i++) {
		if (api.GetPlaceType(nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i]) == THUAI6::PlaceType::ClassRoom)
		{
			nowjuan = { nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i] };
			break;
		}
	}
}
void juan::sendjuaned(IStudentAPI& api) {
	if (api.GetSelfInfo()->playerID == 1)
		api.SendTextMessage(3, fmt::format("{},{}", nowjuan.x, nowjuan.y));
	if (api.GetSelfInfo()->playerID == 3)
		api.SendTextMessage(1, fmt::format("{},{}", nowjuan.x, nowjuan.y));
}
bool juan::juaned(IStudentAPI& api) {
	auto temp = api.GetClassroomProgress(nowjuan.x, nowjuan.y);
	//api.Print(fmt::format("{}", abs(temp - 10000000)));
	if (abs(temp - 10000000) < 1) {
		int s;
		for (s = 0; s < 10; s++)
			if (homeworkmap[s].x ==api.CellToGrid(nowjuan.x) && homeworkmap[s].y == api.CellToGrid(nowjuan.y))
				break;
		juanedmap[s] = 1;
		sendjuaned(api);
		return 1;
	}
	else
		return 0;
}
void juan::startjuan(IStudentAPI& api) {
	api.StartLearning().get();
}
bool juan::tograduate(IStudentAPI& api) {
	bool flag = 1;
	for (int i = 0; i < 10; i++) {
		if (juanedmap[i]==false) {
			flag = 0;
			break;
		}
	}
	return flag;
}
void juan::statemove(IStudentAPI& api) {
	initialnowjuan(api);
	useskill(api);
	if (api.GetSelfInfo()->playerState != THUAI6::PlayerState::Learning) {
		startjuan(api);
	}
}
void juan::statetransfer(IStudentAPI& api) {
	if (tograduate(api)) {
		scurstate = 2;
	}
	if (juaned(api)) {
		/*findin();*/
		scurstate = 3;
	}
}
//*************************************************************************************************************************
static mapnode togradu = { -1,-1 };
//状态二，准备毕业
class readygraduate :virtual public basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	readygraduate(IStudentAPI& api) :basicstate(api) { ; }
	virtual ~readygraduate() { ; }
};
void readygraduate::statemove(IStudentAPI& api) {
	if (unlockAstar) {
		auto temp = gridtonode(mindisgrid(nowinfo.nowgrid, xiaomenmap));
		tonode = poscal(api, temp);
		togradu = tonode;
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{},{},{},{},{}", nowinfo.nownode.x, nowinfo.nownode.y, tonode.x, tonode.y, path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api)) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}
			{
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			unlockAstar = 1;
		}
	}
}
void readygraduate::statetransfer(IStudentAPI& api) {
	if (stuck) {
		scurstate = 9;
	}
	if (distant(nowinfo.nowgrid, nodetogrid(togradu)) < 100) {
		scurstate = 8;
	}
}
//*************************************************************************************************************************
static mapnode  tojuan = { -1,-1 };
//状态三，找作业
class gather :public virtual  basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	void sendpos(IStudentAPI& api);
	bool getinstru(IStudentAPI& api);
	void getpos();
	bool arrivejuan(IStudentAPI& api);
	gather(IStudentAPI& api) :basicstate(api) { tonode = { -1,-1 }; }
	bool tograduate(IStudentAPI& api);
};
//
bool gather::tograduate(IStudentAPI& api) {
	bool flag = 1;
	for (int i = 0; i < 10; i++) {
		if (juanedmap[i]==false) {
			flag = 0;
			break;
		}
	}
	return flag;
}
//判断是否到达卷点
bool gather::arrivejuan(IStudentAPI& api) {
	for (int i = 0; i < 8; i++) {
		if (api.GetPlaceType(nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i]) == THUAI6::PlaceType::ClassRoom)
			return 1;
	}
	return 0;
}
//计算作业点
void gather::getpos() {
	tonode = gridtonode(mindisgrid(nowinfo.nowgrid, homeworkmap));
}
//发送作业点
void gather::sendpos(IStudentAPI& api) {
	std::string a;
	a.append((char*)&tonode.x);
	a.append((char*)&tonode.y);
	for (int i = 0; i < 4; i++) {
		if (i != api.GetSelfInfo()->playerID)
			api.SendTextMessage(i, a);
	}
}
//接受作业点
bool gather::getinstru(IStudentAPI& api) {
	if (api.HaveMessage()) {
		auto temp = api.GetMessage().second;
		tonode.x = (int)temp[0];
		tonode.y = (int)temp[1];
		return true;
	}
	else
		return false;
}
//找作业接口
void gather::statemove(IStudentAPI& api) {
		//api.Print("1111");
		//api.Print(fmt::format("{}",homeworkmap.size()));
		//api.Print("2111");
		//api.Print("3111");
		auto temp = gridtonode(mindisgrid(nowinfo.nowgrid, homeworkmap, api));
		auto temp2 = nodetogrid(temp);
		tonode = poscal(api, temp);
		tojuan = tonode;
		if (lastpos.x != tonode.x || lastpos.y != tonode.y)
			unlockAstar = 1;
		if (unlockAstar) {
			/*walkcomp(api, nodetogrid(nowinfo.nownode));*/
			pathmerging(nowinfo.nownode, tonode, path, api);
			//api.Print(fmt::format("{},{},{},{},{}", nowinfo.nownode.x, nowinfo.nownode.y, tonode.x, tonode.y, path.size()));
			//for (int i = 0; i < path.size(); i++)
			//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
			unlockAstar = 0;
		}
		if (!path.empty()) {
			if (!isarrivend(api)) {
				if (isarrive(api)) {
					/*walkcomp(api, nodetogrid(path[counter - 1]));*/
					;
				}
				{
					if (!isskipwindow()) {
						walkpath(api);
					}
					else {
						skipwindow(api);
					}
				}
			}
			else {
				walkcomp(api, nodetogrid(tonode));
				unlockAstar = 1;
			}
		}
}
//找作业转换
void gather::statetransfer(IStudentAPI& api) {
	/*api.Print(fmt::format("{},{},{},{},{}", distant(nowinfo.nowgrid, nodetogrid(tonode)), nowinfo.nowgrid.x, nowinfo.nowgrid.y, nodetogrid(tonode).x, nodetogrid(tonode).y));*/
	if (stuck) {
		scurstate = 9;
	}
	 if (tograduate(api)) {
		scurstate = 2;
	}
	else if (!path.empty()) {
		if ((distant(nowinfo.nowgrid, nodetogrid(path[counter])) >= 2000))
			scurstate = 9;
		else if (distant(nowinfo.nowgrid, nodetogrid(tojuan)) < 100) {
			scurstate = 1;
		}
	}
	else if (distant(nowinfo.nowgrid,nodetogrid(tojuan))<100) {
		scurstate = 1;
	}

}
//*************************************************************************************************************************
// 状态八，开门毕业
class run :public basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api) { ; }
	run(IStudentAPI& api) :basicstate(api) { ; }
	virtual ~run() { ; }
};
void run::statemove(IStudentAPI& api) {
	mapnode temp;
	for (int i = 0; i < 8; i++) {
		if (api.GetPlaceType(nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i]) == THUAI6::PlaceType::Gate)
		{
			temp = { nowinfo.nownode.x + d8x[i], nowinfo.nownode.y + d8y[i] };
			break;
		}
	}
	if (api.GetSelfInfo()->playerState != THUAI6::PlayerState::OpeningAGate)
		api.StartOpenGate().get();
		api.Graduate();
	
}
//*************************************************************************************************************************
//状态10，救人
class save:public basicstate {
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	save(IStudentAPI& api) :basicstate(api) {
		auto id = api.GetSelfInfo()->playerID;
		for (int k = 0; k < 4; k++) {
			if ((id - api.GetStudents()[k]->playerID) == 1)
				tosave = api.GetStudents()[k];
			else if (api.GetStudents()[k]->studentType == THUAI6::StudentType::Teacher)
				another = api.GetStudents()[k];
		}; 
		mapnode tosavenode = gridtonode({ tosave->x, tosave->y });
		mapnode anothernode = gridtonode({ another->x, another->y });
	}
	virtual ~save() { ; }
	std::shared_ptr<const THUAI6::Student> tosave;
	std::shared_ptr<const THUAI6::Student> another;
	mapnode tosavenode;
	mapnode anothernode;
	void upmap();
	bool inge(IStudentAPI& api);
};
bool save::inge(IStudentAPI& api) {
	for (int i = 0; i < 4; i++) {
		if (nowinfo.nownode.x == tosavenode.x + dx[i] && nowinfo.nownode.y == tosavenode.y + dy[i])
			return 1;
	}
	return 0;
}
void save::upmap() {
	int xstep = (tosavenode.x - anothernode.x) / abs(tosavenode.x - anothernode.x);
	int ystep = (tosavenode.y - anothernode.y) / abs(tosavenode.y - anothernode.y);
	for (int s = 0; s != (tosavenode.x - anothernode.x) + xstep; s += xstep) {
		wallmap[tosavenode.x + s][tosavenode.y] = 0;
		wallmap[tosavenode.x + s][anothernode.y] = 0;
	}
	for (int s = 0; s != (tosavenode.y - anothernode.y) + ystep; s += ystep) {
		wallmap[tosavenode.x][tosavenode.y + s] = 0;
		wallmap[anothernode.x][tosavenode.y + s] = 0;
	}
}
void save::statemove(IStudentAPI& api) {
	insave = 1;
	initialmap(api);
	upmap();
	tonode=poscal(api,tosavenode);
	if (unlockAstar) {
		/*walkcomp(api, nodetogrid(nowinfo.nownode));*/
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{},{},{},{},{}", nowinfo.nownode.x, nowinfo.nownode.y, tonode.x, tonode.y, path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (!inge(api)) {
		if (!path.empty()) {
			if (!isarrivend(api)) {
				if (isarrive(api)) {
					/*walkcomp(api, nodetogrid(path[counter - 1]));*/
					;
				}
				{
					if (!isskipwindow()) {
						walkpath(api);
					}
					else {
						skipwindow(api);
					}
				}
			}
			else {
				walkcomp(api, nodetogrid(tonode));
				unlockAstar = 1;
			}
		}
	}
	else {
		api.StartRouseMate(tosave->playerID);
	}
}
void save::statetransfer(IStudentAPI& api) {
	if (tosave->playerState != THUAI6::PlayerState::Addicted) {
		unlockAstar = 1;
		insave = 0;
		scurstate = 2;
	}
	else if (!inge(api) && stuck) {
		scurstate = 9;
	}

}
//*************************************************************************************************************************
//完全体
class straight{
public:
	virtual void statemove(IStudentAPI& api);
	virtual void statetransfer(IStudentAPI& api);
	straight(IStudentAPI& api);
	virtual ~straight();
	basicstate* nowstate;
};
straight::~straight() {
	delete(nowstate);
} 
straight::straight(IStudentAPI& api) {
	switch (scurstate) {
	case 3: {nowstate = new gather(api); 
		//api.Print("now gather"); 
		break; }
	case 1: {nowstate = new juan(api); 
		//api.Print("now juan");
		break; }
	case 2: {nowstate = new readygraduate(api); 
		//api.Print("now readygraduate("); 
		break; }
	case 8: {nowstate = new run(api);
		//api.Print("now run"); 
		break; }
	case 9: {nowstate = new retry(api); 
	/*	api.Print("now retry"); */
		break; }
	case 10: {nowstate = new save(api); 
		//api.Print("now save");
		break; }
	}
}
//完全体接口:状态转换
void straight::statetransfer(IStudentAPI& api) {
	auto a = scurstate;
	nowstate->statetransfer(api);
	if (a == scurstate) {
		;
	}
	else {
		delete(nowstate);
		switch (scurstate) {
		case 3: {nowstate = new gather(api); 
			///*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto gather"); 
			break; }
		case 1: {nowstate = new juan(api); 
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto juan"); 
			break; }
		case 2: {nowstate = new readygraduate(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto readygraduate");
			break;}
		case 8: {nowstate = new run(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto run");
			break;}
		case 9: {nowstate = new retry(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto retry"); 
			break; }
		case 10: {nowstate = new save(api);
			/*nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));*/
			//api.Print("now transto save");
			break; }
		}
	}
}
//完全体接口:状态行动
void straight::statemove(IStudentAPI& api) {
	nowstate->statemove(api);
}

//*************************************************************************************************************************
//*************************************************************************************************************************
//*************************************************************************************************************************
//*************************************************************************************************************************
//*************************************************************************************************************************
//*************************************************************************************************************************
//*************************************************************************************************************************



int first;
static bool timego = 0;
static mapnode pregrid;
static double prevoice;
static bool inCD=0;

struct info2 {
	mapnode nownode;
	mapnode nowgrid;
	double CD[3];
	double dangeralert;
	std::vector<std::shared_ptr<const THUAI6::Student>> Studentinfo;
};

//基本状态，封装了走路函数
class basicstate2 {
public:
	virtual void statemove(ITrickerAPI& api);
	virtual void statetransfer(ITrickerAPI& api) { ; }
	info2 nowinfo;
	basicstate2(ITrickerAPI& api);
	virtual ~basicstate2();
	void walkpath(ITrickerAPI& api);
	int isarrive(ITrickerAPI& api);
	bool isskipwindow();
	void skipwindow(ITrickerAPI& api);
	bool isarrivend(ITrickerAPI& api);
	bool walkcomp(ITrickerAPI& api, mapnode grid);
	void wallmapupdate(ITrickerAPI& api);
	//void wallmaprefresh(ITrickerAPI& api);
	mapnode tonode;
	bool stuck;
};

basicstate2::~basicstate2() {
	lastpos=tonode;
	prevoice = nowinfo.dangeralert;
	pregrid = nowinfo.nowgrid;
}

basicstate2::basicstate2(ITrickerAPI& api) {
	if (timego) {
		first = api.GetFrameCount();
		timego = 0;
	}
	if (inCD) {
		auto b = api.GetFrameCount();
		int s = b-first;
		if (s > 800)
			inCD = 0;
	}
	auto self = api.GetSelfInfo();
	nowinfo.nownode.x = api.GridToCell(self->x);
	nowinfo.nownode.y = api.GridToCell(self->y);
	nowinfo.nowgrid.x = self->x;
	nowinfo.nowgrid.y = self->y;
	for (int i = 0; i < 3; i++) {
		nowinfo.CD[i] = self->timeUntilSkillAvailable[i];
	}
	nowinfo.dangeralert = self->trickDesire;
	nowinfo.Studentinfo = api.GetStudents();
	if (!nowinfo.Studentinfo.empty()) {
		for (int i = 0; i < nowinfo.Studentinfo.size(); i++) {
			if (nowinfo.Studentinfo[i]->playerState == THUAI6::PlayerState::Addicted) {
				auto x = api.GridToCell(nowinfo.Studentinfo[i]->x);
				auto y = api.GridToCell(nowinfo.Studentinfo[i]->y);
				wallmap[x][y] = 0;
			}
		}
	}
	if (distant(nowinfo.nowgrid, lastgrid) < 0.1)
		stuck = 1;
	else
		stuck = 0;
	lastgrid.x = self->x;
	lastgrid.y = self->y;
}

void basicstate2::statemove(ITrickerAPI& api) {
	api.PrintSelfInfo();
	tonode = { 6,5 };
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{}\n", path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (!isarrivend(api)) {
		if (isarrive(api)) {
			walkcomp(api, nodetogrid(path[counter - 1]));
		}
		//else if (flag == 2) {
		//	;
		{
			if (!isskipwindow()) {
				walkpath(api);
			}
			else {
				skipwindow(api);
			}
		}
	}
}

int basicstate2::isarrive(ITrickerAPI& api) {
	if (path.empty()||(counter==path.size()-1))
		return 2;
	else if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) <= 100) {
		/*double angle = atan2((path[counter].y - nowinfo.nowgrid.y), (path[counter].x - nowinfo.nowgrid.x));
		int time = distant(nowinfo.nowgrid, nodetogrid(path[counter])) * 1000 / Constants::Sunshine::moveSpeed;
		api.Move(time,angle).get();*/
		//api.Print("arrivedpoint");
		Move(path[counter], path[counter + 1]);
		counter++;
		//api.Print(fmt::format("{}", counter));
		return 1;
	}
	//else if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) >= 2000) {
	//	/*api.Print("pianli called");
	//	unlockAstar = 1;
	//	unlockpos = 1;
	//	return 2;*/
	//}
	else return 0;
}

bool basicstate2::isarrivend(ITrickerAPI& api) {
	if (distant(nowinfo.nowgrid, nodetogrid(tonode)) < 100)
		return 1;
	else
		return 0;
}

void basicstate2::walkpath(ITrickerAPI& api) {
	if (direction == 1) {
		/*api.EndAllAction(); */
		api.MoveUp(70);
		std::this_thread::sleep_for(std::chrono::milliseconds(80));

	}
	if (direction == 2) {
		/*api.EndAllAction(); */
		api.Move(98, P2I * 5).get();
		std::this_thread::sleep_for(std::chrono::milliseconds(108));
	}
	if (direction == 3) {
		/*api.EndAllAction(); */
		api.MoveLeft(70);
		std::this_thread::sleep_for(std::chrono::milliseconds(80));
	}
	if (direction == 4) {
		/*api.EndAllAction(); */
		api.Move(98, P2I * 7);
		std::this_thread::sleep_for(std::chrono::milliseconds(108));
	}
	if (direction == 5) {
		/*api.EndAllAction(); */
		api.MoveDown(70);
		std::this_thread::sleep_for(std::chrono::milliseconds(80));
	}
	if (direction == 6) {
		/*api.EndAllAction(); */
		api.Move(98, P2I);
		std::this_thread::sleep_for(std::chrono::milliseconds(108));
	}
	if (direction == 7) {
		/*api.EndAllAction(); */
		api.MoveRight(70);
		std::this_thread::sleep_for(std::chrono::milliseconds(80));
	}
	if (direction == 8) {
		/*api.EndAllAction(); */
		api.Move(98, P2I * 3);
		std::this_thread::sleep_for(std::chrono::milliseconds(108));
	}
}

bool basicstate2::isskipwindow()
{
	if (wallmap[path[counter].x][path[counter].y] == 2)
		return true;
	else
		return 0;
}

void basicstate2::skipwindow(ITrickerAPI& api) {
	if (api.SkipWindow().get()) {
		counter++;
	}
}

bool basicstate2::walkcomp(ITrickerAPI& api, mapnode grid) {
	//api.Print("walkcomp called");
	double angle = atan2((grid.y - nowinfo.nowgrid.y), (grid.x - nowinfo.nowgrid.x));
	int time = distant(grid, nowinfo.nowgrid) / 3;
	api.EndAllAction();
	api.Move(time, angle);
	if (time > 100) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		api.EndAllAction();
	}
	else
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
	return 1;
}

static mapnode ooo = { -1,-1 };

void basicstate2::wallmapupdate(ITrickerAPI& api) {
	if (!nowinfo.Studentinfo.empty()) {
		for (int i = 0; i < nowinfo.Studentinfo.size(); i++) {
			int x = nowinfo.Studentinfo[i]->x / 1000, y = nowinfo.Studentinfo[i]->y / 1000;
			//api.Print(fmt::format("{},{},update", x, y));
			if (nowinfo.Studentinfo[i]->playerState == THUAI6::PlayerState::Addicted && ooo.x != x && ooo.y != y) {
				wallmap[x][y] = 0;
				ooo = { x,y };
				//api.Print(fmt::format("{},{},update", x, y));
				unlockAstar = 1;
			}
			if (nowinfo.Studentinfo[i]->playerState == THUAI6::PlayerState::Quit) {
				wallmap[x][y] = 1;
			}
		}
	}
}

//****************************************************************************************************

static int routecount = 0;
static bool unlockroutecount = 1;
//全挂了返回假
bool checkaddict(std::vector<std::shared_ptr<const THUAI6::Student>> student) {
	bool flag = 0;
	for (int i = 0; i < student.size(); i++) {
		if (student[i]->playerState == THUAI6::PlayerState::Addicted) {
			flag = flag;
		}
		else {
			flag = 1;
		}
	}
	return flag;
}


//状态1 巡逻

class route :public basicstate2 {
public:
	virtual void statemove(ITrickerAPI& api);
	virtual void statetransfer(ITrickerAPI& api);
	route(ITrickerAPI& api) :basicstate2(api) { ; }
	virtual ~route() { ; }
	void handletonode();
};

void route::handletonode() {
	if (mapflag == 0) {
		int d = 2147483647, k = 0;
		for (int i = 0; i < routemap.size(); i++) {
			if (distant(nowinfo.nowgrid, routemap[i]) < d) {
				d = distant(nowinfo.nowgrid, routemap[i]);
				k = i;
			}
		}
		routecount = k;
		tonode = routemap[routecount];
	}
	else {
		int d = 2147483647, k = 0;
		for (int i = 0; i < routemap.size(); i++) {
			if (distant(nowinfo.nowgrid, routemap2[i]) < d) {
				d = distant(nowinfo.nowgrid, routemap2[i]);
				k = i;
			}
		}
		routecount = k;
		tonode = routemap2[routecount];
	}
}

void route::statemove(ITrickerAPI& api) {
	if (unlockroutecount) {
		handletonode();
		unlockroutecount = 0;
	}
	else {
		if(mapflag==0)
			tonode = routemap[routecount];
		else
			tonode = routemap2[routecount];
	}
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{}\n", path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	if (path.empty()) {
		unlockAstar = 1;
		routecount++;
		if (mapflag == 0)
			routecount %= routemap.size();
		else
			routecount %= routemap2.size();
	}
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api)==1) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}
	
			 {
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			routecount++;
			if(mapflag==0)
				routecount %= routemap.size();
			else
				routecount %= routemap2.size();
			unlockAstar = 1;
		}
	}
}

void route::statetransfer(ITrickerAPI& api) {
	if (stuck) {
		unlockAstar = 1;
		tcurstate = 6;
	}
	 else if (!nowinfo.Studentinfo.empty() && checkaddict(nowinfo.Studentinfo)) {
		unlockAstar = 1;
		unlockroutecount = 1;
		tcurstate = 3;
	}
	 else if (nowinfo.dangeralert >1.7) {
		if (!inCD) {
			unlockAstar = 1;
			unlockroutecount = 1;
			tcurstate = 2;
			timego = 1;
			inCD = 1;
		}
	}
	 else if (!path.empty()) {
		if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) > 2000) {
			unlockAstar = 1;
			tcurstate = 6;
		}
	}
}
//****************************************************************************************************


//状态2 运动获取搜索信息

static mapnode assumepos = {-1,-1};

class search :public basicstate2 {
public:
	search(ITrickerAPI& api):basicstate2(api) { ; }
	virtual ~search() { ; }
	virtual void statetransfer(ITrickerAPI& api);
	virtual void statemove(ITrickerAPI& api);
	mapnode soundtrace(ITrickerAPI& api);
};

mapnode search::soundtrace(ITrickerAPI& api) {
	api.Wait();
	mapnode now = { api.GetSelfInfo()->x, api.GetSelfInfo()->y };
	double r1 = Constants::Klee::alertnessRadius / api.GetSelfInfo()->trickDesire;
	auto d=api.MoveDown(200).get();
	api.Wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	api.Wait();
	mapnode now1 = { api.GetSelfInfo()->x, api.GetSelfInfo()->y };
	double d1 = distant(now, now1);
	double r2 = Constants::Klee::alertnessRadius / api.GetSelfInfo()->trickDesire;
	double cosa = (d1 * d1 + r2 * r2 - r1 * r1) / (2.0 * d1 * r2);
	double sina = sqrt(1 - cosa * cosa);
	mapnode assume = { now1.x - (int)r2 * cosa,now1.y + (int)r2 * sina };
	auto a=api.MoveRight(200).get();
	api.Wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	api.Wait();
	mapnode now2 = { api.GetSelfInfo()->x, api.GetSelfInfo()->y };
	double r3 = Constants::Klee::alertnessRadius / api.GetSelfInfo()->trickDesire;
	double tryd = distant(assume, now2);
	mapnode result;
	if (abs(tryd - r3) < 1000)
		result = gridtonode(assume);
	else {
		assume.y = now1.y - (int)r2 * sina;
		result = gridtonode(assume);
	}
	auto b=api.MoveLeft(200).get();
	api.Wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	api.Wait();
	auto c=api.MoveUp(200).get();
	api.Wait();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	api.Wait();
	return result;
}

void search::statemove(ITrickerAPI& api) {
	assumepos = soundtrace(api);
	//api.Print(fmt::format("{},{}", assumepos.x, assumepos.y));
	//std::this_thread::sleep_for(std::chrono::seconds(200));
}

void search::statetransfer(ITrickerAPI& api) {
	//if (nowinfo.dangeralert >= 8.5) {
	//	unlockAstar = 1;
	//	tcurstate = 4;
	//}
	 {
		unlockAstar = 1;
		tcurstate = 7;
	}
}

//*************************************************************************************************************************
static bool insearch = 0;
//状态7 得信息搜索
class searchinfo :public basicstate2 {
public:
	searchinfo(ITrickerAPI& api) :basicstate2(api) { ; }
	virtual ~searchinfo() { ; }
	virtual void statetransfer(ITrickerAPI& api);
	virtual void statemove(ITrickerAPI& api);
	int checkalid();
};

int searchinfo::checkalid() {
	if (assumepos.x > 49 || assumepos.x < 0 || assumepos.y>49 || assumepos.y < 0)
		return 2;
	else if (wallmap[assumepos.x][assumepos.y])
		return 1;
	else
		return 0;
}

void searchinfo::statemove(ITrickerAPI& api) {
	insearch = 1;
	if (checkalid() == 0) {
		tonode = assumepos;
	}
	else if (checkalid() == 1) {
		tonode = poscal(api,assumepos);
	}
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		unlockAstar = 0;
	}
	if (!path.empty()) {
		if (!isarrivend(api)) {
			if (isarrive(api) == 1) {
				walkcomp(api, nodetogrid(path[counter - 1]));
			}

			{
				if (!isskipwindow()) {
					walkpath(api);
				}
				else {
					skipwindow(api);
				}
			}
		}
		else {
			walkcomp(api, nodetogrid(tonode));
			unlockAstar = 1;
		}
	}
}

void searchinfo::statetransfer(ITrickerAPI& api) {
	if (!nowinfo.Studentinfo.empty()) {
		if (checkaddict(nowinfo.Studentinfo)) {
			unlockAstar = 1;
			insearch = 0;
			tcurstate = 3;
		}
		else  {
			unlockAstar = 1;
			insearch = 0;
			tcurstate = 1;
		}
	}
	//else if (nowinfo.dangeralert > 8.5&& checkaddict(nowinfo.Studentinfo)) {
	//	insearch = 0;
	//	tcurstate = 4;
	//}
	else if (distant(nowinfo.nowgrid, nodetogrid(tonode)) < 200||checkalid()==2) {
		unlockroutecount = 1;
		unlockAstar = 1;
		insearch = 0;
		tcurstate = 1;
	}
	else if (stuck) {
		unlockAstar = 1;
		tcurstate = 6;
	}
	else if (!path.empty()) {
		if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) > 2000) {
			unlockAstar = 1;
			tcurstate = 6;
		}
	}
}

//*************************************************************************************************************************
//状态3 有视野追杀

class kill :public basicstate2 {
public:
	kill(ITrickerAPI& api) :basicstate2(api) {/* api.Print(fmt::format("aaa"));*/; }
	virtual ~kill() { ; }
	virtual void statemove(ITrickerAPI& api);
	virtual void statetransfer(ITrickerAPI& api);
	int valuecount();
	void skilluse(ITrickerAPI& api);
	//void wallmapupdate();
	//void wallmaprefresh();
	bool inrange();
	void attack(ITrickerAPI& api);
	int p;

};

void kill::statetransfer(ITrickerAPI& api) {
	//if (nowinfo.Studentinfo.empty()) {
	//	walkcomp(api, nodetogrid(nowinfo.nownode));
	//	tcurstate = 2;
	//}
	p = valuecount();
	 if (!checkaddict(nowinfo.Studentinfo)) {
		//walkcomp(api, nodetogrid(nowinfo.nownode));
		tcurstate = 1;
	}
	 else if (!inrange() && stuck) {
		 tcurstate = 6;
	 }
	 else if (!path.empty()) {
		 if (distant(nowinfo.nowgrid, nodetogrid(path[counter])) > 2000) {
			 unlockAstar = 1;
			 tcurstate = 6;
		 }
	 }
}

void kill::skilluse(ITrickerAPI& api) {
	if (distant(nowinfo.nowgrid, { nowinfo.Studentinfo[p]->x,nowinfo.Studentinfo[p]->y }) < 2000 && nowinfo.CD[0] < 0.1)
		api.UseSkill(0).get();
}

bool kill::inrange() {
	if (nowinfo.Studentinfo.size() > 0)
		if (nowinfo.CD[0] < 1) {
			if (distant(nowinfo.nowgrid, { nowinfo.Studentinfo[p]->x,nowinfo.Studentinfo[p]->y }) < 1100)
				return 1;
			else
				return 0;
		}
		else {
			if (distant(nowinfo.nowgrid, { nowinfo.Studentinfo[p]->x,nowinfo.Studentinfo[p]->y }) < 1100)
				return 1;
			else
				return 0;
		}
	else
		return 0;

}

void kill::attack(ITrickerAPI& api) {
	double angle;
	angle = atan2(nowinfo.Studentinfo[p]->y - nowinfo.nowgrid.y, nowinfo.Studentinfo[p]->x - nowinfo.nowgrid.x);
	api.Attack(angle).get();
}

int kill::valuecount() {
	if (nowinfo.Studentinfo.size() == 1)
		return 0;
	else {
		int d = 1000000000, p = 0;
		for (int i = 0; i < nowinfo.Studentinfo.size(); i++) {
			mapnode temp = { nowinfo.Studentinfo[i]->x,nowinfo.Studentinfo[i]->y };
			if (distant(nowinfo.nowgrid, temp) < d && nowinfo.Studentinfo[i]->playerState != THUAI6::PlayerState::Addicted)
			{
				d = distant(nowinfo.nowgrid, temp);
				p = i;
			}
		}
		return p;
	}
}

void kill::statemove(ITrickerAPI& api) {
	p = valuecount();
	//api.Print(fmt::format("1111,{},{},{}", unlockAstar, p, nowinfo.Studentinfo.size()));
	if (nowinfo.Studentinfo.size() > 0) {
		tonode = gridtonode({ nowinfo.Studentinfo[p]->x,nowinfo.Studentinfo[p]->y });
	}
	//api.Print(fmt::format("1111,{},{},{}", unlockAstar, p, nowinfo.Studentinfo.size()));
	if (unlockAstar == 1) {
		pathmerging(nowinfo.nownode, tonode, path, api);
		//api.Print(fmt::format("{}\n", path.size()));
		//for (int i = 0; i < path.size(); i++)
		//	api.Print(fmt::format("{},{}\n", path[i].x, path[i].y));
		unlockAstar = 0;
	}
	//api.Print(fmt::format("1111,{},{},{}", unlockAstar, p, nowinfo.Studentinfo.size()));
	if (nowinfo.Studentinfo.size() > 0) {
		skilluse(api);
	}

		if (!inrange()) {
			//api.Print(fmt::format("1111,{},{},{}", unlockAstar, p,counter));
			if (!path.empty()) {
				if (isarrive(api) == 1) {
					/*			api.Print(fmt::format("1111,{},{},{}", unlockAstar, p, path.size()));*/
					if (counter >= 1)
						walkcomp(api, nodetogrid(path[counter - 1]));
				}
				{
					if (!isskipwindow()) {
						walkpath(api);
					}
					else {
						skipwindow(api);
					}
				}
			}
		}
		else {
			attack(api);
			unlockAstar = 1;
		}

}


//*************************************************************************************************************************
static int score;
static bool scoreflag=1;
//状态4 无视野乱杀
class randkill :public basicstate2 {
public:
	randkill(ITrickerAPI& api) :basicstate2(api) { ; }
	virtual ~randkill() { ; }
	virtual void statetransfer(ITrickerAPI& api);
	virtual void statemove(ITrickerAPI& api);
	void skilluse(ITrickerAPI& api);
	void attack(mapnode togrid, ITrickerAPI& api);
	bool givein(ITrickerAPI& api);
};
void randkill::skilluse(ITrickerAPI& api) {
	if (nowinfo.CD[0] < 0.1)
		api.UseSkill(0).get();
}

void randkill::attack(mapnode togrid, ITrickerAPI& api) {
	double angle = atan2(togrid.y - nowinfo.nowgrid.y, togrid.x - nowinfo.nowgrid.x);
	api.Attack(angle).get();
}
void randkill::statemove(ITrickerAPI& api) {
	skilluse(api);
	attack(lastpos,api);
}

bool randkill::givein(ITrickerAPI& api) {
	return 1;
}

void randkill::statetransfer(ITrickerAPI& api) {
	if (scoreflag) {
		auto a = api.GetGameInfo();
		score = a->trickerScore;
		scoreflag = 0;
	}
	if (givein(api))
		tcurstate = 1;
}




//*************************************************************************************************************************


static bool hoflag=1;
//状态5 打作业
class homeworkkill:public kill {
public:
	homeworkkill(ITrickerAPI& api) :kill(api) { ; }
	virtual ~homeworkkill() { ; }
	virtual void statetransfer(ITrickerAPI& api);
	virtual void statemove(ITrickerAPI& api);
};




//*************************************************************************************************************************
//状态6 鉴定为：重开
class redo:public basicstate2 {
public:
	redo(ITrickerAPI& api) :basicstate2(api) { ; }
	virtual ~redo() { ; }
	virtual void statetransfer(ITrickerAPI& api);
	virtual void statemove(ITrickerAPI& api);
};

void redo::statemove(ITrickerAPI& api) {
	unlockAstar = 1;
	api.Wait();
	mapnode temp = { api.GetSelfInfo()->x,api.GetSelfInfo()->y };
	mapnode to = nodetogrid(gridtonode(temp));
	double angle = atan2(to.y - temp.y, to.x - temp.x);
	int time;
	time = distant(temp, to) * 1000 / 3600;
	api.EndAllAction();
	api.Move(time, angle).get();
}

void redo::statetransfer(ITrickerAPI& api) {
	if (insearch)
		tcurstate = 7;
	else {
		if (!nowinfo.Studentinfo.empty() && checkaddict(nowinfo.Studentinfo))
			tcurstate = 3;
		else {
			tcurstate = 1;
		}
	}
}

//*************************************************************************************************************************
//完全体
class klee {
public:
	virtual void statemove(ITrickerAPI& api);
	virtual void statetransfer(ITrickerAPI& api);
	klee(ITrickerAPI& api);
	virtual ~klee();
	basicstate2* nowstate;
};
klee::~klee() {
	delete(nowstate);
}
klee::klee(ITrickerAPI& api) {
	switch (tcurstate) {
	case 1: {nowstate = new route(api);
		/*api.Print("now gather"); */
		//api.Print("route");
		break; }
	case 3: {
		nowstate = new kill(api);
		//api.Print("kill");
		break;
	}
	case 6: {
		nowstate = new redo(api);
		//api.Print("redo");
		break;
	}
	case 2: {
		nowstate = new search(api);
		//api.Print("search");
		break;
	}
	case 4: {
		nowstate = new randkill(api);
		//api.Print("randkill");
		break;
	}
	case 7: {
		nowstate = new searchinfo(api);
		//api.Print("searchinfo");
		break;
	}
	}
}
//完全体接口:状态转换
void klee::statetransfer(ITrickerAPI& api) {
	auto a = tcurstate;
	nowstate->statetransfer(api);
	if (a == tcurstate) {
		;
	}
	else {
		delete(nowstate);
		switch (tcurstate) {
		case 1: {
			nowstate = new route(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
			/*api.Print("now transto gather");*/
			//api.Print("toroute");
			break; }
		case 3: {
			//api.Print(fmt::format("{},aaa", tcurstate));
			nowstate = new kill(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
			//api.Print(fmt::format("{}", tcurstate));
			//api.Print("tokill");
			break;
		}
		case 6: {
			//api.Print(fmt::format("{},aaa", tcurstate));
			nowstate = new redo(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
			//api.Print("toredo");
			//api.Print(fmt::format("{}", tcurstate));
			break;
		}
		case 4: {
			//api.Print(fmt::format("{},aaa", tcurstate));
			nowstate = new randkill(api);
			//nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
	/*		api.Print("torandkill");*/
			//api.Print(fmt::format("{}", tcurstate));
			break;
		}
		case 2: {
			//api.Print(fmt::format("{},aaa", tcurstate));
			nowstate = new search(api);
			////nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
			//api.Print("tosearch");
			//api.Print(fmt::format("{}", tcurstate));
			break;
		}
		case 7: {
			//api.Print(fmt::format("{},aaa", tcurstate));
			nowstate = new searchinfo(api);
			////nowstate->walkcomp(api, nodetogrid(nowstate->nowinfo.nownode));
			//api.Print("tosearchinfo");
			//api.Print(fmt::format("{}", tcurstate));
			break;
		}
		}
	}
}
//完全体接口:状态行动
void klee::statemove(ITrickerAPI& api) {
	nowstate->statemove(api);
}



//static bool flagaa = 1;
void AI::play(IStudentAPI& api)
{
	if (this->playerID == 0)
	{
		/*api.PrintSelfInfo();*/
		initialmap(api);
		teacherall a(api);
		a.statetransfer(api);
		a.statemove(api);
		//initialmap(api);
		//straight a(api);
		//a.statetransfer(api);
		//a.statemove(api);
	}
	else if (this->playerID == 1)
	{
		/*api.PrintSelfInfo();*/
		initialmap(api);
		straight a(api);
		a.statetransfer(api);
		a.statemove(api);
	}
	else if (this->playerID == 2)
	{
		/*api.PrintSelfInfo();*/
		initialmap(api);
		teacherall a(api);
		a.statetransfer(api);
		a.statemove(api);
		//initialmap(api);
		//straight a(api);
		//a.statetransfer(api);
		//a.statemove(api);
	}
	else if (this->playerID == 3)
	{
		/*api.PrintSelfInfo();*/
		initialmap(api);
		straight a(api);
		a.statetransfer(api);
		a.statemove(api);
	}
	// 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
	//  公共操作
}



void AI::play(ITrickerAPI& api)
{
	initialmap(api);
	klee a(api);
	a.statetransfer(api);
	a.statemove(api);
}
