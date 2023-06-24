#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <queue>
#include <list>
// 注意不要使用conio.h，Windows.h等非标准库

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::Athlete,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Sunshine };

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数

auto Land = THUAI6::PlaceType::Land;
auto Wall = THUAI6::PlaceType::Wall;
auto Grass = THUAI6::PlaceType::Grass;
auto ClassRoom = THUAI6::PlaceType::ClassRoom;
auto Gate = THUAI6::PlaceType::Gate;
auto HiddenGate = THUAI6::PlaceType::HiddenGate;
auto Window = THUAI6::PlaceType::Window;
auto Door3 = THUAI6::PlaceType::Door3;
auto Door5 = THUAI6::PlaceType::Door5;
auto Door6 = THUAI6::PlaceType::Door6;
auto Chest = THUAI6::PlaceType::Chest;
auto NullPlaceType = THUAI6::PlaceType::NullPlaceType;
constexpr auto pi = 3.14159265358979;

//点类
class Node
{
public:
	//有参构造函数
	Node(const IAPI& api, int xx, int yy, int HH, int GG) :api(api), x(xx), y(yy), H(HH), G(GG)
	{
		Placetype = api.GetPlaceType(x, y);
		father = this;
		fatherplace = 5;
	}
	//拷贝构造函数
	Node(const Node& a) :api(a.api), x(a.x), y(a.y), H(a.H), G(a.G), Placetype(a.Placetype)
	{
		if (a.father == &a)
			this->father = this, this->fatherplace = 5;
		else
			this->father = a.father, this->fatherplace = a.fatherplace;
	}
	//计算H的值
	int GetH(const Node& a)
	{
		return 10 * (abs(x - a.x) + abs(y - a.y));
	}
	//返回该点周围九宫格内的各个点
	Node Up()
	{
		if (x != 0)
			return Node(api, x - 1, y, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node Down()
	{
		if (x != 50)
			return Node(api, x + 1, y, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node Left()
	{
		if (y != 0)
			return Node(api, x, y - 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node Right()
	{
		if (y != 50)
			return Node(api, x, y + 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node UpLeft()
	{
		if (x != 0 && y != 0)
			return Node(api, x - 1, y - 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node UpRight()
	{
		if (x != 0 && y != 50)
			return Node(api, x - 1, y + 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node DownLeft()
	{
		if (x != 50 && y != 0)
			return Node(api, x + 1, y - 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	Node DownRight()
	{
		if (x != 50 && y != 50)
			return Node(api, x + 1, y + 1, 0, 0);
		else
			return Node(api, 0, 0, 0, 0);
	}
	//重载Point类运算符 =
	Node operator = (const Node& a)
	{
		x = a.x, y = a.y, H = a.H, G = a.G, Placetype = a.Placetype;
		father = a.father, fatherplace = a.fatherplace;
		return *this;
	}
	//重载Point类运算符 == 和 !=
	friend int operator == (const Node& a, const Node& b)
	{
		if (b.x == (a.x) && b.y == a.y && b.Placetype == a.Placetype)
			return 1;
		return 0;
	}
	friend int operator != (const Node& a, const Node& b)
	{
		if (b.x == (a.x) && b.y == a.y && b.Placetype == a.Placetype)
			return 0;
		return 1;
	}
	//判断是否卡墙
	int ifcanmove()
	{
		if (Placetype == Wall)
			return 0;
		return 1;
	}
	//析构函数
	~Node() {}
public:
	const IAPI& api;
	int x;//该点的横坐标
	int y;//该点的纵坐标
	int H;//该点到终点的预计耗费
	int G;//起点到该点的耗费
	THUAI6::PlaceType Placetype;//该点的地形
	Node* father;//该点的父点
	int fatherplace;
};

//返回点2到点1的角度
double GetAngle(double x1, double y1, double x2, double y2)
{
	double x = x1 - x2;
	double y = y1 - y2;
	//点1在点2右上方
	if (x <= 0 && y > 0)
		return atan((-x) / y) + 0.5 * pi;
	//点1在点2右下方
	else if (x > 0 && y >= 0)
		return atan(y / x);
	//点1在点2左上方
	else if (x < 0 && y <= 0)
		return atan(y / x) + pi;
	//点1在点2左下方
	else if (x >= 0 && y < 0)
		return atan(x / (-y)) + 1.5 * pi;
	return 0;
}

//寻路函数
int* GetWalk(int* distance, int* Step, Node& start, Node & final, Node* map)
{
	int breaks = 0;//调试
	int* walk1 = new int[1];
	if (final.ifcanmove() == 0)
		return NULL;
	Node* open = (Node*)operator new(sizeof(Node) * 110000);
	*open = Node(start);
	if (open == NULL)
		return NULL;
	open->G = 0;
	open->H = open->GetH(final);
	open->father = open;//设置开启列表的第一个点
	int s1 = 1;//s1为开启列表的元素个数
	Node* close = (Node*)operator new(sizeof(Node) * 110000);
	if (close == NULL)
		return NULL;
	int s2 = 0;//s2为关闭列表的元素个数
	int i = 0;
	while (i == 0)
	{
		breaks++;
		if (s1 == 0)
			return NULL;//返回错误值
		int k, k1 = 0;
		Node C(*open);
		for (k = 0; k < s1; k++)//找开启列表中离终点最近的点
		{
			if (*(open + k) == final)
			{
				int step = 0;
				*(distance) = (open + k)->G;
				Node C(*(open + k));
				while (*(C.father) != C)
				{
					step += 1;
					C = *(C.father);
				}
				if (step != 0)
				{
					*(Step) = step;
					delete[]walk1;
					walk1 = new int[step];;//重置walk数组
					if (walk1 == NULL)
						return NULL;
					int o;
					Node C1(*(open + k));
					for (o = (step - 1); o >= 0; o--)
					{
						*(walk1 + o) = 10 - C1.fatherplace;
						C1 = *(C1.father);
					}
				}
				else
				{
					*(Step) = 0;
					*(walk1) = 5;
				}
				delete[]open;
				delete[]close;
				return walk1;
			}
			if ((C.G + C.H) > ((open + k)->G + (open + k)->H))
			{
				C = *(open + k), k1 = k;//k1记录C在开启列表中的位置
			}
		}
		int u = 0, d = 0, l = 0, r = 0, ul = 0, ur = 0, dl = 0, dr = 0;
		for (k = 0; k < s2; k++)//检测C点的周围是否在关闭列表内
		{
			if (C.Up() == *(close + k) || C.Up() == *(map))
				u = 1;
			if (C.Down() == *(close + k) || C.Down() == *(map))
				d = 1;
			if (C.Left() == *(close + k) || C.Left() == *(map))
				l = 1;
			if (C.Right() == *(close + k) || C.Right() == *(map))
				r = 1;
			if (C.UpLeft() == *(close + k) || C.UpLeft() == *(map))
				ul = 1;
			if (C.UpRight() == *(close + k) || C.UpRight() == *(map))
				ur = 1;
			if (C.DownLeft() == *(close + k) || C.DownLeft() == *(map))
				dl = 1;
			if (C.DownRight() == *(close + k) || C.DownRight() == *(map))
				dr = 1;
		}
		*(open + k1) = *(open + s1 - 1);
		s1 -= 1;//将C移除开启列表
		new(close + s2) Node(C);
		s2 += 1;//将C放入关闭列表
		if (u == 0 && C.Up().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.Up())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.Up());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 10;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 2;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 10)
			{
				(open + g2)->H = C.Up().GetH(final);
				(open + g2)->G = C.G + 10;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 2;
			}
		}
		if (d == 0 && C.Down().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.Down())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.Down());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 10;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 8;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 10)
			{
				(open + g2)->H = C.Down().GetH(final);
				(open + g2)->G = C.G + 10;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 8;
			}
		}
		if (l == 0 && C.Left().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.Left())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.Left());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 10;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 6;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 10)
			{
				(open + g2)->H = C.Left().GetH(final);
				(open + g2)->G = C.G + 10;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 6;
			}
		}
		if (r == 0 && C.Right().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.Right())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.Right());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 10;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 4;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 10)
			{
				(open + g2)->H = C.Right().GetH(final);
				(open + g2)->G = C.G + 10;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 4;
			}
		}
		if (u == 0 && C.UpLeft().ifcanmove() && C.Up().ifcanmove() && C.Left().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.UpLeft())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.UpLeft());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 14;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 3;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 14)
			{
				(open + g2)->H = C.UpLeft().GetH(final);
				(open + g2)->G = C.G + 14;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 3;
			}
		}
		if (u == 0 && C.UpRight().ifcanmove() && C.Up().ifcanmove() && C.Right().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.UpRight())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.UpRight());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 14;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 1;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 14)
			{
				(open + g2)->H = C.UpRight().GetH(final);
				(open + g2)->G = C.G + 14;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 1;
			}
		}
		if (u == 0 && C.DownLeft().ifcanmove() && C.Down().ifcanmove() && C.Left().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.DownLeft())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.DownLeft());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 14;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 9;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 14)
			{
				(open + g2)->H = C.DownLeft().GetH(final);
				(open + g2)->G = C.G + 14;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 9;
			}
		}
		if (u == 0 && C.DownRight().ifcanmove() && C.Down().ifcanmove() && C.Right().ifcanmove())
		{
			int g1 = 0;
			int g2 = -1;
			for (g1 = 0; g1 < s1; g1++)
			{
				if (*(open + g1) == C.DownRight())
					g2 = g1;
			}
			if (g2 == -1)
			{
				new(open + s1) Node(C.DownRight());
				(open + s1)->H = (open + s1)->GetH(final);
				(open + s1)->G = C.G + 14;
				(open + s1)->father = (close + s2 - 1);
				(open + s1)->fatherplace = 7;
				s1 += 1;
			}
			else if ((open + g2)->G > C.G + 14)
			{
				(open + g2)->H = C.DownRight().GetH(final);
				(open + g2)->G = C.G + 14;
				(open + g2)->father = (close + s2 - 1);
				(open + g2)->fatherplace = 7;
			}
		}
		//将C的上下左右放入开启列表中
	}
}

//移动函数
void move(int* Walk, int step, IAPI& api)
{
	int i;
	int k = 0;
	for (i = 0; i < step; i++)
	{
		if (*(Walk + i) >= 1 && *(Walk + i) <= 9)
			k++;
		else//退出循环
			i = step;
	}
	for (i = 0; i <= k; i++)
	{
		switch (*Walk)
		{
		case 1: api.Move(50, 1.75 * pi); break;
		case 2: api.Move(50, 0); break;
		case 3: api.Move(50, 0.25 * pi); break;
		case 4:	api.Move(50, 1.5 * pi); break;
		case 6: api.Move(50, 0.5 * pi); break;
		case 7: api.Move(50, 1.25 * pi); break;
		case 8: api.Move(50, pi); break;
		case 9: api.Move(50, 0.75 * pi); break;
		default: break;
		}
	}
}

//判断两个点是否连通
bool Connect(double startx, double starty, double finalx, double finaly, Node* map, IAPI& api)
{
	double A, B, C, C1, C2;     //直线方程为Ax+By+C=0
	int x1 = api.GridToCell(startx);
	int y1 = api.GridToCell(starty);
	int x2 = api.GridToCell(finalx);
	int y2 = api.GridToCell(finaly);
	Node final(*(map + x2 * 50 + y2));//final是终点
	Node start(*(map + x1 * 50 + y1));//start是起点
	double x = finalx - startx;
	double y = finaly - starty;
	if (x == 0.0 && y == 0.0) return false;
	if (x == 0.0)
	{
		A = 1;
		B = 0;
		C = (0 - finalx);
		C1 = C - 500 * 1.4143;
		C2 = C + 500 * 1.4143;
	}
	else
	{
		B = 1;
		A = (0 - y / x);
		C = (0 - A * finalx - B * finaly);
		double C3 = 500 * 1.4143 * sqrt(A * A + B * B);
		C1 = C - C3;
		C2 = C + C3;
	}
	if (B == 0)  //如果是x=a形式
	{
		for (double i = std::min(starty, finaly); i <= std::max(starty, finaly); i += 10.0)
		{
			int x31 = api.GridToCell(finalx), x32 = api.GridToCell(finalx - 500 * 1.4143 - 1), x33 = api.GridToCell(finalx + 500 * 1.4143 + 1);
			int y3 = api.GridToCell(i);
			Node D1(*(map + x31 * 50 + y3)), D2(*(map + x32 * 50 + y3)), D3(*(map + x33 * 50 + y3));
			if (D1.Placetype == Wall)
				return false;
			if (D2.Placetype == Wall)
				return false;
			if (D3.Placetype == Wall)
				return false;
		}
	}
	else
	{
		for (double i = std::min(startx, finalx); i <= std::max(startx, finalx); i += 10.0)
		{
			int x3 = api.GridToCell(i);
			int y31 = api.GridToCell(0 - A * i / B - C / B), y32 = api.GridToCell(0 - A * i / B - C1 / B), y33 = api.GridToCell(0 - A * i / B - C2 / B);
			Node D1(*(map + x3 * 50 + y31)), D2(*(map + x3 * 50 + y32)), D3(*(map + x3 * 50 + y33));
			if (D1.Placetype == Wall)
				return false;
			if (D2.Placetype == Wall)
				return false;
			if (D3.Placetype == Wall)
				return false;
		}
	}
	return true;
}

//点与点之间的距离
double Node_to_Node(int x1, int y1, int x2, int y2)
{
	double x = abs(x1 - x2);
	double y = abs(y1 - y2);
	return sqrt(x * x + y * y);
}

void AI::play(IStudentAPI& api)
{
	static Node* map = (Node*) operator new(sizeof(Node) * 2500);//用来扫描地图
	static int InitMap = 0;//用来判定第一次扫描地图
	//第一次扫描地图
	if (InitMap == 0)
	{
		for (int i1 = 0; i1 < 50; i1++)
			for (int i2 = 0; i2 < 50; i2++)
			{
				new(map + i1 * 50 + i2) Node(api, i1, i2, 0, 0);
			}
		InitMap = 1;
	}
	auto self = api.GetSelfInfo();//用来获取自身信息
	auto guid = api.GetPlayerGUIDs();//用来获取guid信息
	int x = api.GridToCell(self->x), y = api.GridToCell(self->y);//用来获取自身位置（0-50）
	Node Me(api, x, y, 0, 0);//记录自身的点
	static int target_x = 0, target_y = 0;//记录目标点的坐标
	static int target[50][50];
	float distance;
	int* path = new int;
	int* DISTANCE = new int;
	int* STEP = new int;
	THUAI6::PlaceType U = api.GetPlaceType(x - 1, y);
	THUAI6::PlaceType D = api.GetPlaceType(x + 1, y);
	THUAI6::PlaceType L = api.GetPlaceType(x, y - 1);
	THUAI6::PlaceType R = api.GetPlaceType(x, y + 1);
	THUAI6::PlaceType UL = api.GetPlaceType(x - 1, y - 1);
	THUAI6::PlaceType UR = api.GetPlaceType(x - 1, y + 1);
	THUAI6::PlaceType DL = api.GetPlaceType(x + 1, y - 1);
	THUAI6::PlaceType DR = api.GetPlaceType(x + 1, y + 1);
	auto near_class = false;
	//判断是否在教室附近
	if (U == ClassRoom || D == ClassRoom || L == ClassRoom || R == ClassRoom || UL == ClassRoom || UR == ClassRoom || DL == ClassRoom || DR == ClassRoom)
	{
		near_class = true;
	}
	//未在教室附近时，寻找最近的教室
	/*if (!near_class)
	{
		distance = 10000000;
		int distance_i;
		for (int i1 = 0; i1 < 50; i1++)
		{
			for (int i2 = 0; i2 < 50; i2++)
			{
				if (api.GetPlaceType(i1, i2) == ClassRoom)
				{
					distance_i = Node_to_Node(x, y, api.CellToGrid(i1), api.CellToGrid(i2));
					if (distance_i < distance)
					{
						target_x = i1;
						target_y = i2;
						distance = distance_i;
					}
				}
			}
		}
		Node TARGET(api, target_x, target_y, 0, 0);
		path = GetWalk(DISTANCE, STEP, Me, TARGET, map);
		move(path, 1, api);
	}*/
	/*if (api.GetPlaceType(api.GridToCell(self->x + 600), api.GridToCell(self->y + 600)) == Wall)
		api.MoveLeft(50);
	else if (api.GetPlaceType(api.GridToCell(self->x + 600), api.GridToCell(self->y - 600)) == Wall)
		api.MoveRight(50);
	else if (api.GetPlaceType(api.GridToCell(self->x - 600), api.GridToCell(self->y - 600)) == Wall)
		api.MoveRight(50);
	else if (api.GetPlaceType(api.GridToCell(self->x - 600), api.GridToCell(self->y + 600)) == Wall)
		api.MoveLeft(50);
	else if (api.GetPlaceType(api.GridToCell(self->x + 600), api.GridToCell(self->y)) == Wall)
		api.MoveLeft(50);
	else if (api.GetPlaceType(api.GridToCell(self->x - 600), api.GridToCell(self->y)) == Wall)
		api.MoveRight(50);
	else if (api.GetPlaceType(api.GridToCell(self->x), api.GridToCell(self->y + 600)) == Wall)
		api.MoveUp(50);
	else if (api.GetPlaceType(api.GridToCell(self->x), api.GridToCell(self->y - 600)) == Wall)
		api.MoveDown(50);
	else api.Move(50, 0.9 * pi + (rand() % 2) * 0.2 * pi);*/
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
		api.UseSkill(1);
		// 玩家3执行操作
	}
	// 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
	//  公共操作
	delete path;
	delete DISTANCE;
	delete STEP;
}

void AI::play(ITrickerAPI& api)
{
	auto self = api.GetSelfInfo();
	api.PrintSelfInfo();
}

