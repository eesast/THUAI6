#include <array>
#include <cmath>
#include <queue>
#include <stack>
#include <chrono>
#include <cfloat>
#include <string>
#include <thread>
#include <vector>
#include <cstdarg>
#include <exception>
#include <algorithm>
#include <functional>
#include "AI.h"
#include "API.h"
#include "constants.h"

#define USE_NEW_ASTAR 0
const double PI = acos(-1);

//*****************************************************
// #Basic geometry
//*****************************************************

THUAI6::TrickerType trickerT=THUAI6::TrickerType::NullTrickerType;

class Cell; // 格子 0~49
class Grid; // 像素 0~49999
#if USE_NEW_ASTAR
class Geop; // 实坐标 R
#endif
class Cell
{
public:
	int x, y;
	Cell();
	Cell(int x_, int y_);
	Grid ToGrid();
#if USE_NEW_ASTAR
	Geop ToGeop();
#endif
};
class Grid
{
public:
	int x, y;
	Grid();
	Grid(int x_, int y_);
	Cell ToCell();
#if USE_NEW_ASTAR
	Geop ToGeop();
#endif
};
#if USE_NEW_ASTAR
class Geop
{
public:
	double x, y;
	Geop();
	Geop(double x_, double y_);
	Cell ToCell();
	Grid ToGrid();
};
#endif
Cell::Cell() : x(0), y(0) {}
Cell::Cell(int x_, int y_) : x(x_), y(y_) {}
Grid Cell::ToGrid() { return Grid(x * 1000 + 500, y * 1000 + 500); }
Grid::Grid() : x(0), y(0) {}
Grid::Grid(int x_, int y_) : x(x_), y(y_) {}
Cell Grid::ToCell() { return Cell(x / 1000, y / 1000); }

#if USE_NEW_ASTAR
Geop::Geop() : x(0), y(0) {}
Geop::Geop(double x_, double y_) : x(x_), y(y_) {}
Geop Cell::ToGeop() { return Geop(x * 1000 + 500, y * 1000 + 500); }
Geop Grid::ToGeop() { return Geop(x, y); }
Cell Geop::ToCell() { return Cell((int)x / 1000, (int)y / 1000); }
Grid Geop::ToGrid() { return Grid((int)x, (int)y); }
double Distance(Geop A, Geop B)
{
	return sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}
// with direction S->T (can be considered as a vector), right side of the segment represents inside
class Geos
{
public:
	Geop S, T;
	Geos(const Geop& PS, const Geop& PT) : S(PS), T(PT) {}
	double GetTheta(Geop P);
};
Geop Project(Geos S, Geop P)
{
	double lambda = ((P.x - S.S.x) * (S.T.x - S.S.x) + (P.y - S.S.y) * (S.T.y - S.S.y)) / pow(Distance(S.S, S.T), 2);
	//	std::cerr << lambda << std::endl;
	if (lambda < 0 || lambda > 1)
		return S.S; // simple
	else
		return Geop(S.S.x + lambda * (S.T.x - S.S.x), S.S.y + lambda * (S.T.y - S.S.y));
}
double Geos::GetTheta(Geop P)
{
	double CrossDot = (S.x - P.x) * (T.y - P.y) - (S.y - P.y) * (T.x - P.x);
	double InnerDot = (S.x - P.x) * (T.x - P.x) + (S.y - P.y) * (T.y - P.y);
	double theta = acos(InnerDot / Distance(S, P) / Distance(T, P));
	//	if (fabs(CrossDot) < 1e-4)
	//	{
	//		std::cerr << "[Common Line Warning!]" << std::endl;
	//		return 0;
	//	}
	return CrossDot > 0 ? theta : -theta;
}
bool Intersect(Geos A, Geos B)
{
	//	std::cerr << "CheckIntersect" << std::endl;
	//	std::cerr << '[' << A.S.x / 1000 << ' ' << A.S.y / 1000 << ']' << std::endl;
	//	std::cerr << '[' << A.T.x / 1000 << ' ' << A.T.y / 1000 << ']' << std::endl;
	//	std::cerr << '[' << B.S.x / 1000 << ' ' << B.S.y / 1000 << ']' << std::endl;
	//	std::cerr << '[' << B.T.x / 1000 << ' ' << B.T.y / 1000 << ']' << std::endl;
	double CDAS = (B.S.x - A.S.x) * (B.T.y - A.S.y) - (B.S.y - A.S.y) * (B.T.x - A.S.x);
	double CDAT = (B.S.x - A.T.x) * (B.T.y - A.T.y) - (B.S.y - A.T.y) * (B.T.x - A.T.x);
	double CDBS = (A.S.x - B.S.x) * (A.T.y - B.S.y) - (A.S.y - B.S.y) * (A.T.x - B.S.x);
	double CDBT = (A.S.x - B.T.x) * (A.T.y - B.T.y) - (A.S.y - B.T.y) * (A.T.x - B.T.x);
	double C = (A.T.x - A.S.x) * (B.T.y - B.S.y) - (A.T.y - A.S.y) * (B.T.x - B.S.x);
	//	std::cerr << "C = " << C << std::endl;
	if (CDAS * CDAT <= 0 && CDBS * CDBT <= 0 && C <= 0)
		return true;
	return false;
}
#endif

//*****************************************************
// #Information
//*****************************************************

class noway_exception : public std::exception {};

struct MapUpdateInfo
{
	THUAI6::PlaceType type;
	int x, y, val;
};
typedef std::shared_ptr<const THUAI6::Student> NeedHelpInfo;
//typedef THUAI6::Tricker TrickerInfo_t;
class TrickerInfo_t
{
public:
	int64_t playerID;
	int32_t x;
	int32_t y;
	THUAI6::PlayerState playerState;
	THUAI6::TrickerType trickerType;
	TrickerInfo_t() {}
	TrickerInfo_t(const THUAI6::Tricker& t) : playerID(t.playerID), x(t.x), y(t.y), playerState(t.playerState), trickerType(t.trickerType) {}
};

class Doors : public Cell
{
public:
	Doors(int x_ = 0, int y_ = 0, bool ds_ = true, THUAI6::PlaceType dt_ = THUAI6::PlaceType::Door3)
		: Cell(x_, y_), DoorStatus(ds_), DoorType(dt_) {};
	Doors(Cell p_, bool ds_ = true, THUAI6::PlaceType dt_ = THUAI6::PlaceType::Door3)
		: Cell(p_), DoorStatus(ds_), DoorType(dt_) {};
	bool DoorStatus;
	THUAI6::PlaceType DoorType;
};

#if !USE_NEW_ASTAR
class Node : public Cell
{
public:
	Node(int x_ = 0, int y_ = 0, int px_ = -1, int py_ = -1,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		: Cell(x_, y_), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	Node(Cell p_, int px_ = -1, int py_ = -1,
		float fc_ = FLT_MAX, float gc_ = FLT_MAX, float hc_ = FLT_MAX)
		: Cell(p_), parentX(px_), parentY(py_), fCost(fc_), gCost(gc_), hCost(hc_) {};
	int parentX;
	int parentY;
	float fCost;
	float gCost;
	float hCost;
};
#endif

//*****************************************************
// #Definition & Interface
//*****************************************************

template <typename IFooAPI>
class Friends;
template <typename IFooAPI>
class Geographer;
template <typename IFooAPI>
class Pigeon;
template <typename IFooAPI>
class Predictor;
template <typename IFooAPI>
class CommandPost;

template <typename IFooAPI>
class CommandPost
{
	// 这是你和伙伴们所在的指挥所
protected:
	Cell TEMP;
	int LastAutoUpdateFrame;
	IFooAPI& API;
	void InitMap(IFooAPI& api);
	const int UpdateInterval = 1;

public:
	// 这里有你们共享的信息
	std::vector < std::vector <THUAI6::PlaceType> > Map;
	unsigned char Access[50][50];
	//	unsigned char Enemy[50][50];
	std::vector<Cell> Classroom;
	std::vector<Cell> Gate;
	std::vector<Cell> HiddenGate;
	std::vector<Cell> Chest;
	std::vector<Cell> Grass;
	std::vector<Doors> Door;
	int InfoMem[50][50];
	int LastUpdateFrame[50][50];
	bool IsStuck;
	double LastStuckAngle;
	Cell LastStuckDestinationGrid;

	Geographer<IFooAPI> Alice;
	Predictor<IFooAPI> Bob;
	Pigeon<IFooAPI> Gugu;

	// 指挥所应当能够直接解决一些基本的问题而不劳烦几位专职人员，比如当前是否在箱子旁边，以及最近的作业位置等等

	std::vector<THUAI6::PropType> Inventory;
	static std::vector<unsigned char> PickPropPriority;
	static std::vector<unsigned char> UsePropPriority;

	CommandPost(IFooAPI& api);

	void Update(MapUpdateInfo upinfo, int t_);						   // 更新地图信息，比如门和隐藏校门，需要约定info的格式
	void Update(TrickerInfo_t tri, int t_);                            // 更新Tricker信息
	std::vector<THUAI6::PropType> GetInventory() { return Inventory; } // 查看背包
	void OrganizeInventory(std::vector<unsigned char> Priority);	   // 整理背包

	//	bool MoveToAccurate(Grid Dest, bool WithWindows = true);
	bool MoveTo(Cell Dest, bool WithWindows);	   // 往目的地动一动
	bool MoveToNearestClassroom(bool WithWindows); // 往最近的作业的方向动一动
	bool MoveToNearestGate(bool WithWindows);	   // 往最近的关闭的校门旁边动一动
	bool MoveToNearestOpenGate(bool WithWindows);  // 往最近的开启的校门旁边动一动
	bool MoveToNearestChest(bool WithWindows);	   // 往最近的箱子的方向动一动

	bool NearCell(Cell P, int level = 1);	// level=0判断当前是否在该格子上，1判断是否在格子上或周围4格，2判断是否在格子上或周围8格
	bool NearClassroom(bool checkProgress); // 已经在作业旁边了吗？
	bool NearGate();						// 已经在关闭的校门旁边了吗？
	bool NearOpenGate();					// 已经在开启的校门旁边了吗？
	bool NearChest();						// 已经在箱子旁边了吗？
	bool NearWindow();						// 已经在窗户旁边了吗？
	bool InGrass();							// 已经在草丛里了吗？

	void DirectLearning(bool WithWindows);																			  // 前往最近的作业并学习
	void DirectOpeningChest(bool WithWindows);																		  // 前往最近的箱子并开箱
	void DirectOpeningGate(bool WithWindows, bool CanDirectGraduate);												  // 前往最近的关闭的校门并开门
	void DirectGraduate(bool WithWindows);																			  // 前往最近的开启的校门并毕业
	void DirectGrass(bool WithWindows);																				  // 前往最近的草丛并躲避
	void DirectHide(Cell TrickerLocation, int TrickerViewRange, bool WithWindows);									  // 前往最适合的草丛并躲避
	void DirectProp(std::vector<unsigned char> Priority, int DistanceInfluence, int PropInfluence, bool WithWindows); // 前往已知价值最高的道具并捡道具
	void DirectUseProp(std::vector<unsigned char> Priority);

	int CountFinishedClassroom() const;
	int CountNonemptyChest() const;
	int CountHiddenGate() const;
	int CountClosedGate() const;
	int CountOpenGate() const;

	int GetChestProgress(int cellx, int celly);
	int GetGateProgress(int cellx, int celly);
	int GetClassroomProgress(int cellx, int celly);
	int GetDoorProgress(int cellx, int celly);
	int GetChestProgress(Cell cell) const;
	int GetGateProgress(Cell cell) const;
	int GetClassroomProgress(Cell cell) const;
	int GetDoorProgress(Cell cell) const;

	bool IsAccessible(int x, int y, bool WithWindows);
};

template <typename IFooAPI>
class Friends
{
	// Student&Tricker的伙伴们
protected:
	IFooAPI& API;				  // 方便起见，每个人都有对api的直接引用，其实可以直接用Center.API
	CommandPost<IFooAPI>& Center; // 你和伙伴们都可以访问自己所在的指挥所，并和其他人交流
	Friends(IFooAPI& api, CommandPost<IFooAPI>& Center_) : API(api), Center(Center_) {}
};

template <typename IFooAPI>
class Geographer : public Friends<IFooAPI>
{
	// 这是一位Geographer，负责告诉要怎么走
protected:
	//	bool IsAccessible(THUAI6::PlaceType pt);

#if USE_NEW_ASTAR
	std::vector<Geos> StableMap;
	std::vector<Geop> StableCheckPoint;
	std::vector<Geos> VariableMap;
	std::vector<Geop> VariableCheckPoint;

	bool DirectReachable(Geop A, Geop B, bool IsDest = false);
	Geop Escape(Geop P);

	void InitStableMap();
	const double CheckPointRadius;
	double CheckPointCompensate;
	const double SegmentRadius;
	double SegmentCompensate;

	bool InsideObstacle(Geop P);
	void ResetVariableMap();
	void AddPlayer();
	void AddWindow();
	void AddLockedDoor();
#endif

public:
	Geographer(IFooAPI& api, CommandPost<IFooAPI>& Center_);
#if !USE_NEW_ASTAR
	bool IsDestination(int x, int y, Node dest);
	double CalculateH(int x, int y, Node dest);
	std::vector<Node> MakePath(const std::array<std::array<Node, 50>, 50>& map, Node dest);
	std::vector<Node> AStar(Node src, Node dest, bool WithWindows);
	int EstimateTime(Cell Dest); // 去目的地的预估时间
#else
	std::vector<Geop> FindPath(Geop From_, Geop Dest_);
#endif
	void BackwardExpand(Cell Source, int H[50][50]);
	bool IsViewable(Cell Src, Cell Dest, int ViewRange); // 判断两个位置是否可视
	std::vector<Cell> GetViewableCells(Cell src);

	Cell GetNearestGate();
	Cell GetNearestClassroom(); // 仅在没写完的作业中找
	Cell GetNearestOpenGate();
};

template<typename IFooAPI>
std::vector<Cell> Geographer<IFooAPI>::GetViewableCells(Cell src)
{
	int vrange = this->API.GetSelfInfo()->viewRange / 2;
	bool chk[50][50];
	memset(chk, 0, sizeof(chk));
	std::queue<Cell> Q;
	Q.push(src);
	chk[src.x][src.y] = true;
	std::vector<Cell> ans;
	while (!Q.empty())
	{
		Cell now = Q.front();
		Q.pop();
		ans.push_back(now);
		for (int ix = -1; ix <= 1; ix++)
			for (int iy = -1; iy <= 1; iy++)
				if (std::abs(ix) + std::abs(iy) == 1 &&
					now.x + ix >= 0 && now.x + ix < 50 && now.y + iy >= 0 && now.y + iy < 50 &&
					!chk[now.x + ix][now.y + iy])
				{
					Cell nxt(now.x + ix, now.y + iy);
					chk[now.x + ix][now.y + iy] = true;
					if (IsViewable(src, nxt, vrange)) Q.push(nxt);
				}
	}
	return ans;
}

class Encoder
{
private:
	static const int MaxLength = 255;
	char msg[MaxLength];
	int Celler;

public:
	Encoder();
	void SetHeader(char header);
	template <typename T>
	void PushInfo(T info);
	std::string ToString();
};

class Decoder
{
private:
	std::string msg;
	int Celler;

public:
	Decoder(std::string code);
	template <typename T>
	T ReadInfo();
};

template <typename IFooAPI>
class Pigeon : public Friends<IFooAPI>
{
	// 这是一只Pigeon，负责传递信息
private:
	void sendInfo(int64_t dest, std::string info);
	std::string buf;

public:
	Pigeon(IFooAPI& api, CommandPost<IFooAPI>& Center_) : Friends<IFooAPI>(api, Center_) {}
	void sendMapUpdate(int64_t dest, MapUpdateInfo muinfo);
	void sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val);
	void sendTrickerInfo(int64_t dest, TrickerInfo_t tricker);
	void sendNeedHelp(int64_t dest, NeedHelpInfo self);
	void sendRescue(int64_t dest, bool rescue);

	int receiveMessage(); // ���ؽ��յ�����Ϣ����
	std::pair<int, MapUpdateInfo> receiveMapUpdate();
	TrickerInfo_t receiveTrickerInfo();
	std::pair<int, int> receiveNeedHelp();
	bool receiveRescue();
};

template <typename IFooAPI>
class Predictor : public Friends<IFooAPI>
{
	// 这是一位Predictor，负责告诉其他人可能在哪里
protected:
	std::vector<double> DangerAlertLog;
	std::vector<double> TrickDesireLog;
	std::vector<double> ClassVolumeLog;

	double MagicMap[5][50][50];
	const int TotalValue;
	int PlayerStatus[5], CantMoveStart[5], CantMoveDuration[5];
	// 0表示不追踪该玩家，适用于本人或退学/毕业的情况；1表示可以移动并正常追踪；2表示最后一次看到时是不能运动的状态，但不是沉迷；3表示最后一次看到时是沉迷状态；2~3会用到上述两个CantMove数组。
	void NormalizeMagicMap(); // 正则化
	void DeduceMagicMap(); // 进行一次推算

public:
	Predictor(IFooAPI& api, CommandPost<IFooAPI>& Center_);

	void FindEnemy();
	void SaveDangerAlertLog(int maxNum);
	void SaveTrickDesireLog(int maxNum);
	void SaveClassVolumeLog(int maxNum);

	void AutoUpdate();
	void Update(const TrickerInfo_t& tri);
	std::pair<Cell, double> Recommend(int PlayerID);
	// Always return position with highest probility, even though the player was addicted (In this circumstance, chasing it results in a repetition of finding it addicted.) To chase it or not should be decided in stragety.
	// TODO: CommandPost should save info of addiction & quit etc., should it be the responsibility of Predictor? Probably yes. Ask Predictor for info of players' status. This only works for Tricker.
	void _display(int PlayerID);
	Cell SmartRecommend();
};

class CommandPostStudent : public CommandPost<IStudentAPI>
{
public:
	CommandPostStudent(IStudentAPI& api) : CommandPost(api) {}
	void AutoUpdate();

	void TeacherPunish();
	double TeacherPunishCD();

	void StraightAStudentWriteAnswers();
	double StraightAStudentWriteAnswersCD();

	void AtheleteCanBeginToCharge();
	double AtheleteCanBeginToChargeCD();

	void SunshineRouse();
	void SunshineEncourage();
	void SunshineInspire();
	double SunshineRouseCD();
	double SunshineEncourageCD();
	double SunshineInspireCD();
};

class CommandPostTricker : public CommandPost<ITrickerAPI>
{
public:
	CommandPostTricker(ITrickerAPI& api) : CommandPost(api) {}
	void AutoUpdate();

	void KleeDefaultAttack(int stux, int stuy); // 刺客普通攻击，传入学生坐标(stux,stuy)
	bool KleeDefaultAttackOver(int rank);		// 判断能否稳定命中，传入目前能观察到的学生列表的第几个，从0开始计数
	void KleeJumpyBomb();
	double KleeJumpyBombCD();
	void KleeSparksNSplash(int PlayerID);
	double KleeSparksNSplashCD();
};

//*****************************************************
// #Implementation
//*****************************************************

//--------------------
// #Predictor
//--------------------

template<typename IFooAPI>
Predictor<IFooAPI>::Predictor(IFooAPI& api, CommandPost<IFooAPI>& Center_) : Friends<IFooAPI>(api, Center_), TotalValue(2500)
{
	for (int i = 0; i < 5; i++) PlayerStatus[i] = 1;
	PlayerStatus[api.GetSelfInfo()->playerID] = 0;
}

template <typename IFooAPI>
void Predictor<IFooAPI>::NormalizeMagicMap()
{
	for (int id = 0; id < 5; id++)
		if (PlayerStatus[id] != 0)
		{
			double sum = 0;
			for (int i = 0; i < 50; i++)
				for (int j = 0; j < 50; j++)
					sum += MagicMap[id][i][j];
			if (sum == 0)
			{
				for (int i = 0; i < 50; i++)
					for (int j = 0; j < 50; j++)
						if (this->Center.Access[i][j]) MagicMap[id][i][j] = 1, sum += 1;
				std::cerr << "sum = " << sum;
			}
			for (int i = 0; i < 50; i++)
				for (int j = 0; j < 50; j++)
					MagicMap[id][i][j] *= TotalValue / sum;
		}
}

template <typename IFooAPI>
void Predictor<IFooAPI>::DeduceMagicMap()
{
	auto selfinfo = this->API.GetSelfInfo();
	bool deal[5];
	double NextStatus[50][50];
	for (int i = 0; i < 5; i++) deal[i] = (PlayerStatus[i] != 0);
	auto vision = this->Center.Alice.GetViewableCells(Grid(selfinfo->x, selfinfo->y).ToCell());
	for (int id = 0; id < 5; id++)
		if (deal[id])
		{
			memset(NextStatus, 0, sizeof(double) * 50 * 50);
			for (int i = 0; i < 50; i++)
				for (int j = 0; j < 50; j++)
				{
					if (this->Center.Access[i][j])
					{
						double ratio1 = (PlayerStatus[id] == 3 ? 0.01 : 0.15);
						int cnt = 0;
						for (int ix = -1; ix <= 1; ix++)
							for (int jx = -1; jx <= 1; jx++)
								if (((ix == 0) ^ (jx == 0)) &&
									i + ix >= 0 && i + ix < 50 && j + jx >= 0 && j + jx < 50 &&
									this->Center.Access[i + ix][j + jx]) cnt++;
						NextStatus[i][j] += MagicMap[id][i][j] * (1 - ratio1);
						if (cnt == 0) continue;
						for (int ix = -1; ix <= 1; ix++)
							for (int jx = -1; jx <= 1; jx++)
								if (((ix == 0) ^ (jx == 0)) &&
									i + ix >= 0 && i + ix < 50 && j + jx >= 0 && j + jx < 50 &&
									this->Center.Access[i + ix][j + jx])
								{
									NextStatus[i + ix][j + jx] += MagicMap[id][i][j] * ratio1 / cnt;
								}
					}
				}
			memcpy(MagicMap[id], NextStatus, sizeof(double) * 50 * 50);
			for (auto c : vision) MagicMap[id][c.x][c.y] = 0;
		}
	NormalizeMagicMap();
}

template<typename IFooAPI>
void Predictor<IFooAPI>::AutoUpdate()
{
	DeduceMagicMap();
	auto stuinfo = this->API.GetStudents();
	auto triinfo = this->API.GetTrickers();
	for (auto s : stuinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
	for (auto s : triinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
}

/* AutoUpdate with bgm
void Predictor<IStudentAPI>::AutoUpdate()
{
	DeduceMagicMap();
	auto stuinfo = this->API.GetStudents();
	auto triinfo = this->API.GetTrickers();
	for (auto s : stuinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
	for (auto s : triinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
}

void Predictor<ITrickerAPI>::AutoUpdate()
{
	DeduceMagicMap();
	auto stuinfo = this->API.GetStudents();
	auto triinfo = this->API.GetTrickers();
	for (auto s : stuinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
	for (auto s : triinfo)
		if (PlayerStatus[s->playerID])
		{
			Cell pos = Grid(s->x, s->y).ToCell();
			memset(MagicMap[s->playerID], 0, sizeof(double) * 50 * 50);
			MagicMap[s->playerID][pos.x][pos.y] = TotalValue;
			if (s->playerState == THUAI6::PlayerState::Addicted) PlayerStatus[s->playerID] = 3;
			else PlayerStatus[s->playerID] = 1;
		}
	if (this->API.GetSelfInfo()->trickDesire)
	{
		double cent = this->API.GetSelfInfo()->radius / this->API.GetSelfInfo()->trickDesire;
		std::cerr << "cent = " << cent << std::endl;
		int sx = this->API.GetSelfInfo()->x;
		int sy = this->API.GetSelfInfo()->y;
		for (int i = 0; i < 50; i++)
			for (int j = 0; j < 50; j++)
			{
				if (this->Center.IsAccessible(i, j, true))
				{
					double dx = i * 1000 + 500 - sx;
					double dy = j * 1000 + 500 - sy;
					double dist = sqrt(dx * dx + dy * dy) / cent;
//					std::cerr << "dist = " << dist << std::endl;;
					double distrib = exp(-(dist - 1) * (dist - 1) / 0.18) * .1 + 1;
					for (int k = 0; k < 4; k++)
						if (PlayerStatus[k]) MagicMap[k][i][j] *= distrib;
				}
			}
		for (int k = 0; k < 4; k++) if (PlayerStatus[k]) NormalizeMagicMap();
	}
}
*/

template<typename IFooAPI>
void Predictor<IFooAPI>::Update(const TrickerInfo_t& tri)
{
	if (PlayerStatus[tri.playerID])
	{
		Cell pos = Grid(tri.x, tri.y).ToCell();
		memset(MagicMap[tri.playerID], 0, sizeof(double) * 50 * 50);
		MagicMap[tri.playerID][pos.x][pos.y] = TotalValue;
		if (tri.playerState == THUAI6::PlayerState::Addicted) PlayerStatus[tri.playerID] = 3;
		else PlayerStatus[tri.playerID] = 1;
	}
}

template<typename IFooAPI>
void Predictor<IFooAPI>::_display(int PlayerID)
{
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++)
			//			std::cerr << MagicMap[PlayerID][i][j];
			std::cerr.put((MagicMap[PlayerID][i][j] > 1) ? '*' : '.');
		std::cerr << std::endl;
	}
}

template<typename IFooAPI>
std::pair<Cell, double> Predictor<IFooAPI>::Recommend(int PlayerID)
{
	int dist[50][50];
	Cell pos = Grid(this->API.GetSelfInfo()->x, this->API.GetSelfInfo()->y).ToCell();
	this->Center.Alice.BackwardExpand(pos, dist);
	Cell Maxc;
	double prob = 0;
	//	for (int i = 0; i < 50; i++) {
	//		for (int j = 0; j < 50; j++)
	//			std::cerr << (dist[i][j] == 10000000 ? -1 : dist[i][j]) << ' ';
	//		std::cerr << std::endl;
	//	}
	for (int i = 0; i < 50; i++)
		for (int j = 0; j < 50; j++)
			if (MagicMap[PlayerID][i][j] > prob)
			{
				Maxc = Cell(i, j);
				//				std::cerr << i << ' ' << j << std::endl;
				//				assert(dist[i][j] > 999 && MagicMap[PlayerID][i][j] == 0 || dist[i][j] <= 999);
				prob = MagicMap[PlayerID][i][j] / pow(dist[i][j] + 1, 0) * (1 + .5 * sin((i + j) / 10. + this->API.GetFrameCount() / 200. * 3.14));;
			}
	return std::make_pair(Maxc, prob);
}

template <typename IFooAPI>
void Predictor<IFooAPI>::SaveDangerAlertLog(int maxNum)
{
	if (DangerAlertLog.size() < maxNum)
	{
		DangerAlertLog.push_back(this->API.GetSelfInfo()->dangerAlert);
	}
	else
	{
		DangerAlertLog.erase(DangerAlertLog.begin());
		DangerAlertLog.push_back(this->API.GetSelfInfo()->dangerAlert);
	}
}

template <typename IFooAPI>
void Predictor<IFooAPI>::SaveTrickDesireLog(int maxNum)
{
	if (TrickDesireLog.size() < maxNum)
	{
		TrickDesireLog.push_back(this->API.GetSelfInfo()->trickDesire);
	}
	else
	{
		TrickDesireLog.erase(TrickDesireLog.begin());
		TrickDesireLog.push_back(this->API.GetSelfInfo()->trickDesire);
	}
}

template <typename IFooAPI>
void Predictor<IFooAPI>::SaveClassVolumeLog(int maxNum)
{
	if (ClassVolumeLog.size() < maxNum)
	{
		ClassVolumeLog.push_back(this->API.GetSelfInfo()->classVolume);
	}
	else
	{
		ClassVolumeLog.erase(ClassVolumeLog.begin());
		ClassVolumeLog.push_back(this->API.GetSelfInfo()->classVolume);
	}
}

template <typename IFooAPI>
Cell Predictor<IFooAPI>::SmartRecommend()
{
	int x = this->API.GetSelfInfo()->x / 1000;
	int y = this->API.GetSelfInfo()->y / 1000;
	int dist[50][50];
	Cell pos;
	double value = -1;
	this->Center.Alice.BackwardExpand(Cell(x, y), dist);

	int i_ = 0, j_ = 0, k_ = 0;
	for (int i = 0; i < 4; i++)
	{
		if (this->PlayerStatus[i])
		{
			for (int j = 0; j < 50; j++)
				for (int k = 0; k < 50; k++)
				{
					double eval = 0;
					if (PlayerStatus[i] == 3) eval = 1;
					else eval = 10000;
					eval = eval * MagicMap[i][j][k] / log(dist[j][k] + 2) * (1 + .5 * sin((i + j) / 10. + this->API.GetFrameCount() / 200. * 3.14));
					if (eval > value)
					{
						i_ = i;
						j_ = j;
						k_ = k;
						pos = Cell(j, k);
						value = eval;
					}
				}
		}
	}
	std::cerr << "recommend " << j_ << ' ' << k_ << ' ' << MagicMap[i_][j_][k_] << ' ' << dist[j_][k_] << std::endl;
	return pos;
}

//--------------------
// #Pigeon
//--------------------

/* ��ϢЭ��
��Ϣͷ��info[0]��
MapUpdate ��ͼ����
TrickerInfo ��������Ϣ
NeedHelp ����֧Ԯ
	info[1-2]������
WantProp �����ȡ���ߣ�������Ҫ��������Ȼ̫Զ����ȥ�����㣩
*/

#define NoMessage 0x00
#define MapUpdate 0x01
#define TrickerInfo 0x02
#define NeedHelp 0x03
#define WantProp 0x04
#define Rescue 0x05

Encoder::Encoder() : Celler(0)
{
	memset(msg, 0, sizeof(msg));
}
#define NEW_ENDEC 0
template <typename T>
void Encoder::PushInfo(T info)
{
	size_t t = sizeof(T);
	void* ptr = &info;
#if !NEW_ENDEC
	for (size_t i = 0; i < t; i++)
	{
		msg[Celler] = ((*((unsigned char*)ptr + i)) >> 4) + 'a';
		Celler++;
		msg[Celler] = ((*((unsigned char*)ptr + i)) & 0x0f) + 'a';
		Celler++;
	}
#else
	memcpy(msg + Celler, ptr, t);
	Celler += t;
#endif
}
void Encoder::SetHeader(char header)
{
	PushInfo(header);
}
std::string Encoder::ToString()
{
	return std::string(msg, msg + MaxLength);
}

Decoder::Decoder(std::string code) : msg(code), Celler(0) {}
template <typename T>
T Decoder::ReadInfo()
{
	std::cerr << msg << std::endl;
	T obj;
	void* ptr = &obj;
	size_t t = sizeof(T);
	std::cerr << "sizeofT: " << t << std::endl;
#if !NEW_ENDEC
	for (size_t i = 0; i < t; i++)
	{
		*((unsigned char*)ptr + i) = ((((unsigned char)*(msg.c_str() + Celler) - 'a') << 4)) | (((unsigned char)*(msg.c_str() + Celler + 1)) - 'a');
		Celler += 2;
	}
#else
	memcpy(ptr, msg.c_str() + Celler, t);
	Celler += t;
#endif
	return obj;
}

template <typename IFooAPI>
void Pigeon<IFooAPI>::sendInfo(int64_t dest, std::string info)
{
	if (dest != this->API.GetSelfInfo()->guid)
		this->API.SendBinaryMessage(dest, info);
}

template <typename IFooAPI>
void Pigeon<IFooAPI>::sendMapUpdate(int64_t dest, MapUpdateInfo muinfo)
{
	Encoder enc;
	enc.SetHeader(MapUpdate);
	enc.PushInfo(this->API.GetFrameCount());
	enc.PushInfo(muinfo);
	sendInfo(dest, enc.ToString());
}
template <typename IFooAPI>
void Pigeon<IFooAPI>::sendMapUpdate(int64_t dest, THUAI6::PlaceType type, int x, int y, int val)
{
	MapUpdateInfo muinfo = { type, x, y, val };
	sendMapUpdate(dest, muinfo);
}

template <typename IFooAPI>
std::pair<int, MapUpdateInfo> Pigeon<IFooAPI>::receiveMapUpdate()
{
	Decoder dec(buf);
	char header = dec.ReadInfo<char>();
	assert(header == MapUpdate);
	int frm = dec.ReadInfo<int>();
	MapUpdateInfo muinfo = dec.ReadInfo<MapUpdateInfo>();
	return std::make_pair<int, MapUpdateInfo>(static_cast<int&&>(frm), static_cast<MapUpdateInfo&&>(muinfo));
}

template <typename IFooAPI>
int Pigeon<IFooAPI>::receiveMessage()
{
	if (this->API.HaveMessage())
	{
		buf = this->API.GetMessage().second; // 是谁发来的好像不太重要，只取信息内容
		Decoder dec(buf);
		return dec.ReadInfo<char>();
	}
	else
		return NoMessage;
}

template <typename IFooAPI>
void Pigeon<IFooAPI>::sendTrickerInfo(int64_t dest, TrickerInfo_t tricker)
{
	Encoder enc;
	enc.SetHeader(TrickerInfo);
	//	enc.PushInfo(this->API.GetFrameCount());
	enc.PushInfo<TrickerInfo_t>(tricker);
	sendInfo(dest, enc.ToString());
}

template <typename IFooAPI>
TrickerInfo_t Pigeon<IFooAPI>::receiveTrickerInfo()
{
	std::cerr << "(receiveTrickerInfo)\n";
	Decoder dec(buf);
	std::cerr << "(receiveTrickerInfo)\n";
	char header = dec.ReadInfo<char>();
	std::cerr << "(receiveTrickerInfo)\n";
	assert(header == TrickerInfo);
	std::cerr << "(receiveTrickerInfo)\n";
	return dec.ReadInfo<TrickerInfo_t>();
}
// 捣蛋鬼信息的编码和解码函数
std::string sendOneselfMessage(std::shared_ptr<const THUAI6::Student> self)
{
	Encoder enc;
	enc.SetHeader(NeedHelp);
	enc.PushInfo<std::shared_ptr<const THUAI6::Student>>(self);
	std::string info = enc.ToString();
	return info;
}

std::shared_ptr<const THUAI6::Student> receiveOneselfMessage(std::string info)
{
	Decoder dec(info);
	char header = dec.ReadInfo<char>();
	std::shared_ptr<const THUAI6::Student> p1 = dec.ReadInfo<std::shared_ptr<const THUAI6::Student>>();
	return p1;
}

// 自己信息的编码和解码函数
template <typename IFooAPI>
void Pigeon<IFooAPI>::sendNeedHelp(int64_t dest, NeedHelpInfo self)
{
	Encoder enc;
	enc.SetHeader(NeedHelp);
	enc.PushInfo(this->API.GetFrameCount());
	int64_t id = self->playerID;
	enc.PushInfo(id);
	sendInfo(dest, enc.ToString());
}

template <typename IFooAPI>
std::pair<int, int> Pigeon<IFooAPI>::receiveNeedHelp()
{
	Decoder dec(buf);
	char header = dec.ReadInfo<char>();
	assert(header == NeedHelp);
	return std::make_pair<int, int>(dec.ReadInfo<int>(), dec.ReadInfo<int>());
}

template <typename IFooAPI>
void Pigeon<IFooAPI>::sendRescue(int64_t dest, bool rescue)
{
	Encoder enc;
	enc.SetHeader(Rescue);
	enc.PushInfo(rescue);
	sendInfo(dest, enc.ToString());
}

template <typename IFooAPI>
bool Pigeon<IFooAPI>::receiveRescue()
{
	Decoder dec(buf);
	char header = dec.ReadInfo<char>();
	assert(header == Rescue);
	return dec.ReadInfo<bool>();
}
std::string sendPropsMessage(std::vector<std::shared_ptr<const THUAI6::Prop>> prop)
{
	Encoder enc;
	enc.SetHeader(WantProp);
	enc.PushInfo<std::vector<std::shared_ptr<const THUAI6::Prop>>>(prop);
	std::string info = enc.ToString();
	return info;
}

std::vector<std::shared_ptr<const THUAI6::Prop>> receivePropsMessage(std::string info)
{
	Decoder dec(info);
	char header = dec.ReadInfo<char>();
	std::vector<std::shared_ptr<const THUAI6::Prop>> p1 = dec.ReadInfo<std::vector<std::shared_ptr<const THUAI6::Prop>>>();
	return p1;
}


// 道具信息的编码和解码函数
// void send_Door(IStudentAPI& api1, int64_t playerID)
//{
//	int i, j, n = 0;
//	int a[6], b[6];
//	char c[6];
//	for (i = 0; i < 50; i++)
//	{
//		for (j = 0; j < 50; j++)
//		{
//			if (Access[i][j] = 2u)
//			{
//				a[n] = i;
//				b[n] = j;
//				n++;
//				c[n] = char(api1.IsDoorOpen(i, j));
//			}
//		}
//	}
//	std::string info_Door = sendDoorMessage(a, b, n, c);
//
//	api1.SendBinaryMessage(playerID, info_Door);
//
// }
// 发送门信息的函数
// void send_Tricker(IStudentAPI& api1, int64_t playerID)
//{
//	std::string info_Tricker = sendTrickerMessage(api1.GetTrickers());
//	api1.SendBinaryMessage(playerID, info_Tricker);
// }
// 发送捣蛋鬼信息的函数
void send_Oneself(IStudentAPI& api1, int64_t playerID)
{
	std::string info_Oneself = sendOneselfMessage(api1.GetSelfInfo());
	api1.SendBinaryMessage(playerID, info_Oneself);
}
// 发送自己信息的函数
void send_Prop(IStudentAPI& api1, int64_t playerID)
{
	std::string info_Prop = sendPropsMessage(api1.GetProps());
	api1.SendBinaryMessage(playerID, info_Prop);
}
// 发送道具信息的函数
// std::pair<char, std::vector<std::pair<std::pair<int, int>, char>>> receive_Door(IStudentAPI& api2)
//{
//	std::string info_Door = api2.GetMessage().second;
//	std::pair<char, std::vector<std::pair<std::pair<int, int>, char>>> p1 = receiveDoorMessage(info_Door);
//	return p1;
// }
// 接收门信息的函数
// std::vector<std::shared_ptr<const THUAI6::Tricker>> receive_Tricker(IStudentAPI& api2)
//{
//	std::string info_Tricker = api2.GetMessage().second;
//	std::vector<std::shared_ptr<const THUAI6::Tricker>> p1 = receiveTrickerMessage(info_Tricker);
//	return p1;
// }
// 接收捣蛋鬼信息的函数
std::shared_ptr<const THUAI6::Student> receive_Oneself(IStudentAPI& api2)
{
	std::string info_Oneself = api2.GetMessage().second;
	std::shared_ptr<const THUAI6::Student> p1 = receiveOneselfMessage(info_Oneself);
	return p1;
}
// 接受发送者自身信息的函数
std::vector<std::shared_ptr<const THUAI6::Prop>> receive_Prop(IStudentAPI& api2)
{
	std::string info_Prop = api2.GetMessage().second;
	std::vector<std::shared_ptr<const THUAI6::Prop>> p1 = receivePropsMessage(info_Prop);
	return p1;
}
// 接受道具信息的函数

//--------------------
// #CommandPost
//--------------------

template <typename IFooAPI>
void CommandPost<IFooAPI>::InitMap(IFooAPI& api)
{
	int i, j;
	Map = api.GetFullMap();
	for (i = 0; i < 50; i++)
	{
		for (j = 0; j < 50; j++)
		{
			switch (Map[i][j])
			{
			case THUAI6::PlaceType::Wall: // Wall
				Access[i][j] = 0U;
				break;
			case THUAI6::PlaceType::Grass: // Grass
				Access[i][j] = 3U;
				Grass.emplace_back(Cell(i, j));
				break;
			case THUAI6::PlaceType::Window: // Window
				Access[i][j] = 1U;
				break;
			case THUAI6::PlaceType::Door3:  // Door3
			case THUAI6::PlaceType::Door5:  // Door5
			case THUAI6::PlaceType::Door6: // Door6
				Access[i][j] = 2U;
				Door.emplace_back(Cell(i, j), true, api.GetPlaceType(i, j));
				break;
			case THUAI6::PlaceType::ClassRoom: // Classroom
				Access[i][j] = 0U;
				Classroom.emplace_back(Cell(i, j));
				break;
			case THUAI6::PlaceType::Gate: // Gate
				Access[i][j] = 0U;
				Gate.emplace_back(Cell(i, j));
				break;
			case THUAI6::PlaceType::HiddenGate: // HiddenGate
				Access[i][j] = 0U;
				HiddenGate.emplace_back(Cell(i, j));
				break;
			case THUAI6::PlaceType::Chest: // Chest
				Access[i][j] = 0U;
				Chest.emplace_back(Cell(i, j));
				break;
			default:
				Access[i][j] = 2U;
				break;
			}
		}
	}
}

template <typename IFooAPI>
CommandPost<IFooAPI>::CommandPost(IFooAPI& api) : API(api), LastAutoUpdateFrame(0), Alice(api, *this), Bob(api, *this), Gugu(api, *this)
{
	srand(time(NULL));
	InitMap(api);
}

#define ACCESS_TEMP 1

#if !USE_NEW_ASTAR
template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveTo(Cell Dest, bool WithWindows)
{
	std::cerr << "Destination is (" << Dest.x << ',' << Dest.y << ')' << std::endl;
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	IsStuck = (sx == TEMP.x && sy == TEMP.y);
	int AccessBak[50][50];
	std::vector<std::shared_ptr<const THUAI6::Student>> TempS = API.GetStudents();
	std::vector<std::shared_ptr<const THUAI6::Tricker>> TempT = API.GetTrickers();
	//	std::vector<unsigned char> AccessTempS;
	//	std::vector<unsigned char> AccessTempT;
#if ACCESS_TEMP
	memcpy(AccessBak, Access, sizeof(Access));
	for (int i = 0; i < TempS.size(); i++)
	{
		if (TempS[i]->playerID == API.GetSelfInfo()->playerID) continue;
		if ((TempS[i]->x / 1000 != sx / 1000) || (TempS[i]->y / 1000 != sy / 1000))
			Access[(TempS[i]->x) / 1000][(TempS[i]->y) / 1000] = 0U;
	}
	for (int i = 0; i < TempT.size(); i++)
	{
		if (TempT[i]->playerID == API.GetSelfInfo()->playerID) continue;
		if ((TempT[i]->x / 1000 != sx / 1000) || (TempT[i]->y / 1000 != sy / 1000))
			Access[(TempT[i]->x) / 1000][(TempT[i]->y) / 1000] = 0U;
	}
#endif
	std::vector<Node> UsablePath;
	try
	{
		UsablePath = Alice.AStar(Node(sx / 1000, sy / 1000), Dest, WithWindows);
	}
	catch (const noway_exception& e)
	{
		std::cerr << "[noway_exception](MoveTo)Noway." << std::endl;
#if ACCESS_TEMP
		memcpy(Access, AccessBak, sizeof(Access));
#endif
		return false;
	}
	for (auto i : UsablePath)
	{
		std::cerr << '(' << i.x << ',' << i.y << ')' << ';';
	}
	if (UsablePath.size() < 2)
	{
#if ACCESS_TEMP
		memcpy(Access, AccessBak, sizeof(Access));
#endif
		return false;
	}
	else
	{
		int tx, ty;
		if (Map[UsablePath[1].x][UsablePath[1].y] == THUAI6::PlaceType::Window)
		{
			API.SkipWindow();
		}
		else
		{
			if (UsablePath.size() >= 3 && IsAccessible(sx / 1000, sy / 1000, WithWindows) && IsAccessible(UsablePath[1].x, UsablePath[1].y, WithWindows) && IsAccessible(UsablePath[2].x, UsablePath[2].y, false) && IsAccessible(sx / 1000, UsablePath[2].y, WithWindows) && IsAccessible(UsablePath[2].x, sy / 1000, WithWindows))
			{
				tx = UsablePath[2].x * 1000 + 500;
				ty = UsablePath[2].y * 1000 + 500;
			}
			else
			{
				tx = UsablePath[1].x * 1000 + 500;
				ty = UsablePath[1].y * 1000 + 500;
			}
			int dx = tx - sx;
			int dy = ty - sy;
			if (Map[tx / 1000][ty / 1000] != THUAI6::PlaceType::Window)
			{
				if (!IsStuck)
				{
					API.Move(1000 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed, atan2(dy, dx));
				}
				else
				{
					LastStuckAngle = atan2(dy, dx);
					LastStuckDestinationGrid.x = tx;
					LastStuckDestinationGrid.y = ty;
					API.Move(200 * sqrt(dx * dx + dy * dy) / API.GetSelfInfo()->speed, atan2(dy, dx) + rand());
				}
			}
			TEMP.x = sx;
			TEMP.y = sy;
		}
#if ACCESS_TEMP
		memcpy(Access, AccessBak, sizeof(Access));
#endif
		return true;
	}
}

#else
template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveTo(Cell Dest, bool WithWindows)
{
	auto self = API.GetSelfInfo();
	Geop GFrom(API.GetSelfInfo()->x, API.GetSelfInfo()->y), GDest(Dest.x * 1000 + 500, Dest.y * 1000 + 500);
	auto Path = Alice.FindPath(GFrom, GDest);
	for (auto p : Path)
	{
		std::cerr << '(' << p.x << ',' << p.y << ')' << "->";
	}
	int ptr = 0;
	if (Path.size() != 1)
	{
		while (ptr < Path.size() && Distance(GFrom, Path[ptr]) < 10)
			ptr++;
	}
	if (ptr == Path.size())
		return true;
	API.Move((int)std::max<double>((std::min<double>(150, Distance(Path[ptr], GFrom) / API.GetSelfInfo()->speed * 1000)), 10), atan2(Path[ptr].y - GFrom.y, Path[ptr].x - GFrom.x));
	if (WithWindows)
	{
		if (self->x == TEMP.x && self->y == TEMP.y && NearWindow())
			API.SkipWindow();
		TEMP = Cell(self->x, self->y);
	}
	std::cerr << "move angle = " << atan2(Path[ptr].y - GFrom.y, Path[ptr].x - GFrom.x) / acos(-1) * 180;
}

#endif

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearCell(Cell P, int level)
{
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	switch (level)
	{
	case 0:
		return (P.x == Self.x && P.y == Self.y) ? true : false;
	case 1:
		return (abs(P.x - Self.x) + abs(P.y - Self.y) <= 1) ? true : false;
	case 2:
		return (abs(P.x - Self.x) <= 1 && abs(P.y - Self.y) <= 1) ? true : false;
	case 3: // Hide Tricker
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 9) ? true : false;
	case 4: // Hide Tricker
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 25) ? true : false;
	case 5:
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 36) ? true : false;
	case 6:
		return ((P.x - Self.x) * (P.x - Self.x) + (P.y - Self.y) * (P.y - Self.y) <= 5) ? true : false;
	}
}

#if !USE_NEW_ASTAR

template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveToNearestClassroom(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Classroom.empty())
	{
		for (int i = 0; i < Classroom.size(); i++)
		{
			//			if (API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
			if (GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
			{
				try
				{
					Distance = Alice.AStar(Self, Classroom[i], WithWindows).size();
					if (Distance < minDistance && Distance != 0)
					{
						minDistance = Distance;
						minNum = i;
					}
				}
				catch (const noway_exception& e)
				{
					std::cerr << "[noway_exception](MoveToNearestClassroom)Noway." << std::endl;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Classroom[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

#endif

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearClassroom(bool checkProgress)
{
	for (int i = 0; i < Classroom.size(); i++)
	{
		if (NearCell(Classroom[i], 2) && (!checkProgress || (checkProgress && GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)))
			return true;
	}
	return false;
}

#if !USE_NEW_ASTAR

template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveToNearestGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			//			if (API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
			if (GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
			{
				try
				{
					Distance = Alice.AStar(Self, Gate[i], WithWindows).size();
					if (Distance < minDistance && Distance != 0)
					{
						minDistance = Distance;
						minNum = i;
					}
				}
				catch (const noway_exception& e)
				{
					std::cerr << "[noway_exception](MoveToNearestGate)Noway." << std::endl;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Gate[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveToNearestOpenGate(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	if (!Gate.empty())
	{
		for (int i = 0; i < Gate.size(); i++)
		{
			//			if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			if (GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			{
				try
				{
					Distance = Alice.AStar(Self, Gate[i], WithWindows).size();
					if (Distance < minDistance && Distance != 0)
					{
						minDistance = Distance;
						minNum = i;
					}
				}
				catch (const noway_exception& e)
				{
					std::cerr << "[noway_exception](MoveToNearestOpenGate)Noway." << std::endl;
				}
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Gate[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

#endif

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearCell(Gate[i], 2) && GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
			return true;
	}
	return false;
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearOpenGate()
{
	for (int i = 0; i < Gate.size(); i++)
	{
		if (NearCell(Gate[i], 2) && GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
			return true;
	}
	return false;
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::MoveToNearestChest(bool WithWindows)
{
	int minDistance = INT_MAX;
	int minNum = -1;
	int Distance = INT_MAX;
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	for (int i = 0; i < Chest.size(); i++)
	{
		if (GetChestProgress(Chest[i].x, Chest[i].y) < 10000000)
		{
			Distance = WithWindows ? Alice.AStarWithWindows(Self, Chest[i]).size() : Alice.AStarWithoutWindows(Self, Chest[i]).size();
			if (Distance < minDistance && Distance != 0)
			{
				minDistance = Distance;
				minNum = i;
			}
		}
	}
	if (minNum >= 0)
	{
		MoveTo(Chest[minNum], WithWindows);
		return true;
	}
	else
	{
		return false;
	}
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearChest()
{
	for (int i = 0; i < Chest.size(); i++)
	{
		if (NearCell(Chest[i], 2) && API.GetChestProgress(Chest[i].x, Chest[i].y) < 10000000)
			return true;
	}
	return false;
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::NearWindow()
{
	int X = API.GetSelfInfo()->x / 1000, Y = API.GetSelfInfo()->y / 1000;
	for (int i = X - 1; i <= X + 1; i++)
		for (int j = Y - 1; j <= Y + 1; j++)
			if (abs(i - X) + abs(j - Y) == 1 && API.GetPlaceType(i, j) == THUAI6::PlaceType::Window)
				return true;
	return false;
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::InGrass()
{
	if (Map[API.GetSelfInfo()->x / 1000][API.GetSelfInfo()->y / 1000] == THUAI6::PlaceType::Grass)
	{
		return true;
	}
	else
		return false;
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectLearning(bool WithWindows)
{
	if (!NearClassroom(true))
	{
		MoveToNearestClassroom(WithWindows);
	}
	else
	{
		API.StartLearning();
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectOpeningChest(bool WithWindows)
{
	if (!NearChest())
	{
		MoveToNearestChest(WithWindows);
	}
	else
	{
		API.StartOpenChest();
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectOpeningGate(bool WithWindows, bool CanDirectGraduate)
{
	if (!NearGate())
	{
		MoveToNearestGate(WithWindows);
	}
	else
	{
		if (!NearOpenGate())
		{
			API.StartOpenGate();
		}
		else
		{
			if (CanDirectGraduate)
			{
				DirectGraduate(WithWindows);
			}
			else
			{
				MoveToNearestGate(WithWindows);
			}
		}
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectGraduate(bool WithWindows)
{
	if (!NearOpenGate())
	{
		MoveToNearestOpenGate(WithWindows);
	}
	else
	{
		API.Graduate();
	}
}

#if !USE_NEW_ASTAR
template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectGrass(bool WithWindows)
{
	if (!InGrass())
	{
		int minDistance = INT_MAX;
		int minNum = -1;
		int Distance = INT_MAX;
		Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
		if (!Grass.empty())
		{
			for (int i = 0; i < Grass.size(); i++)
			{
				try
				{
					Distance = Alice.AStar(Self, Grass[i], WithWindows).size();
					if (Distance < minDistance && Distance != 0)
					{
						minDistance = Distance;
						minNum = i;
					}
				}
				catch (const noway_exception& e)
				{
					std::cerr << "[noway_exception](DirectGrass)Noway." << std::endl;
				}
			}
		}
		if (minNum >= 0)
		{
			MoveTo(Grass[minNum], WithWindows);
		}
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectHide(Cell TrickerLocation, int TrickerViewRange, bool WithWindows)
{
	std::cerr << "DirectHide\n";
	Cell Self(API.GetSelfInfo()->x / 1000, API.GetSelfInfo()->y / 1000);
	bool t = Alice.IsViewable(TrickerLocation, Self, TrickerViewRange);
	std::cerr << "ask " << TrickerLocation.x << ' ' << TrickerLocation.y << ' ' << Self.x << ' ' << Self.y << ' ' << TrickerViewRange << ' ' << Alice.IsViewable(TrickerLocation, Self, TrickerViewRange) << std::endl;
	std::cerr << "ingrass " << InGrass() << std::endl;
	if (!InGrass() && Alice.IsViewable(TrickerLocation, Self, TrickerViewRange))
	{
		std::cerr << "OK to hide." << std::endl;
		int minDistance = INT_MAX;
		int minNum = -1;
		int Distance = INT_MAX;
		int dist[50][50];
		Alice.BackwardExpand(Self, dist);
		if (!Grass.empty())
		{
			for (int i = 0; i < Grass.size(); i++)
			{
				if ((TrickerLocation.x - Grass[i].x) * (TrickerLocation.x - Grass[i].x) + (TrickerLocation.y - Grass[i].y) * (TrickerLocation.y - Grass[i].y) > 25)// && !Alice.IsViewable(TrickerLocation, Grass[i], TrickerViewRange))
				{
					Distance = dist[Grass[i].x][Grass[i].y];
					if (Distance < minDistance && Distance != 0)
					{
						minDistance = Distance;
						minNum = i;
					}
				}
			}
		}
		if (minNum >= 0)
		{
			MoveTo(Grass[minNum], WithWindows);
		}
	}
}
#endif

template <typename IFooAPI>
int CommandPost<IFooAPI>::CountFinishedClassroom() const
{
	int cnt = 0;
	for (auto i : Classroom)
	{
		if (InfoMem[i.x][i.y] >= 10000000)
			cnt++;
	}
	return cnt;
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::CountNonemptyChest() const
{
	int cnt = 0;
	for (auto i : Chest)
	{
		if (InfoMem[i.x][i.y] < 10000000)
			cnt++;
	}
	return cnt;
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::CountHiddenGate() const
{
	return HiddenGate.size();
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::CountClosedGate() const
{
	int ret = 0;
	for (int i = 0; i < Gate.size(); i++)
	{
		if (API.GetGateProgress(Gate[i].x, Gate[i].y) < 18000)
		{
			ret++;
		}
	}
	return ret;
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::CountOpenGate() const
{
	int ret = 0;
	for (int i = 0; i < Gate.size(); i++)
	{
		if (API.GetGateProgress(Gate[i].x, Gate[i].y) >= 18000)
		{
			ret++;
		}
	}
	return ret;
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::OrganizeInventory(std::vector<unsigned char> Priority)
{
	if (Inventory.size() > 1)
	{
		for (int i = 0; i < Inventory.size(); i++)
		{
			for (int j = i + 1; j < Inventory.size(); j++)
			{
				if (Priority[(int)Inventory[i]] < Priority[(int)Inventory[j]])
				{
					THUAI6::PropType temp = Inventory[i];
					Inventory[i] = Inventory[j];
					Inventory[j] = temp;
				}
			}
		}
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectProp(std::vector<unsigned char> Priority, int DistanceInfluence, int PropInfluence, bool WithWindows)
{
	if (Inventory.size() < 3)
	{
		int MaxValue = 0, MaxNum = 0;
		std::vector<std::shared_ptr<const THUAI6::Prop>> ViewableProps = API.GetProps();
		int PropValue = 0;
		for (int i = 0; i < ViewableProps.size(); i++)
		{
			int dx = (API.GetProps()[i]->x - API.GetSelfInfo()->x) / 1000;
			int dy = (API.GetProps()[i]->y - API.GetSelfInfo()->y) / 1000;
			int Distance = sqrt(dx * dx + dy * dy);
			PropValue = (Distance * DistanceInfluence + Priority[(int)(API.GetProps()[i]->type)] * PropInfluence);
			if (PropValue >= MaxValue)
			{
				MaxValue = PropValue;
				MaxNum = i;
			}
		}
		if (!NearCell(Cell(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), 0))
		{
			MoveTo(Cell(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), WithWindows);
		}
		else
		{
			API.PickProp(ViewableProps[MaxNum]->type);
			Inventory.emplace_back(ViewableProps[MaxNum]->type);
			OrganizeInventory(Priority);
		}
	}
	else
	{
		int MaxValue = Priority[(int)Inventory[2]], MaxNum = 0;
		std::vector<std::shared_ptr<const THUAI6::Prop>> ViewableProps = API.GetProps();
		int PropValue = 0;
		for (int i = 0; i < ViewableProps.size(); i++)
		{
			int dx = (API.GetProps()[i]->x - API.GetSelfInfo()->x) / 1000;
			int dy = (API.GetProps()[i]->y - API.GetSelfInfo()->y) / 1000;
			int Distance = sqrt(dx * dx + dy * dy);
			PropValue = (Distance * DistanceInfluence + Priority[(int)(API.GetProps()[i]->type)] * PropInfluence);
			if (PropValue >= MaxValue)
			{
				MaxValue = PropValue;
				MaxNum = i;
			}
		}
		if (MaxValue > Priority[(int)Inventory[2]])
		{
			if (!NearCell(Cell(ViewableProps[MaxNum]->x / 1000, ViewableProps[MaxNum]->y / 1000), 0))
			{
				MoveTo(Grid(ViewableProps[MaxNum]->x, ViewableProps[MaxNum]->y).ToCell(), WithWindows);
			}
			else
			{
				API.ThrowProp(Inventory[2]);
				API.PickProp(ViewableProps[MaxNum]->type);
				Inventory.emplace_back(ViewableProps[MaxNum]->type);
				OrganizeInventory(Priority);
			}
		}
	}
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::DirectUseProp(std::vector<unsigned char> Priority)
{
	if (!Inventory.empty())
	{
		OrganizeInventory(Priority);
		API.UseProp(Inventory[0]);
	}
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetChestProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange))
		return API.GetChestProgress(cellx, celly);
	else
		return InfoMem[cellx][celly];
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetGateProgress(int cellx, int celly)
{
	if (Alice.IsViewable((Grid(API.GetSelfInfo()->x, API.GetSelfInfo()->y)).ToCell(), Cell(cellx, celly), API.GetSelfInfo()->viewRange))
		return API.GetGateProgress(cellx, celly);
	else
		return InfoMem[cellx][celly];
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetClassroomProgress(int cellx, int celly)
{
	if (Alice.IsViewable((Grid(API.GetSelfInfo()->x, API.GetSelfInfo()->y)).ToCell(), Cell(cellx, celly), API.GetSelfInfo()->viewRange))
		return API.GetClassroomProgress(cellx, celly);
	else
		return InfoMem[cellx][celly];
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetDoorProgress(int cellx, int celly)
{
	if (IsViewable(cellx, celly, API.GetSelfInfo()->viewRange))
		return API.GetDoorProgress(cellx, celly);
	else
		return InfoMem[cellx][celly];
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetChestProgress(Cell cell) const
{
	return GetChestProgress(cell.x, cell.y);
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetGateProgress(Cell cell) const
{
	return GetGateProgress(cell.x, cell.y);
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetClassroomProgress(Cell cell) const
{
	return GetClassroomProgress(cell.x, cell.y);
}

template <typename IFooAPI>
int CommandPost<IFooAPI>::GetDoorProgress(Cell cell) const
{
	return GetDoorProgress(cell.x, cell.y);
}

template <typename IFooAPI>
void CommandPost<IFooAPI>::Update(MapUpdateInfo upinfo, int t_)
{
	if (t_ < LastUpdateFrame[upinfo.x][upinfo.y])
		return;
	LastUpdateFrame[upinfo.x][upinfo.y] = t_;
	if (upinfo.type == THUAI6::PlaceType::Chest || upinfo.type == THUAI6::PlaceType::ClassRoom || upinfo.type == THUAI6::PlaceType::Gate || upinfo.type == THUAI6::PlaceType::HiddenGate)
	{
		InfoMem[upinfo.x][upinfo.y] = upinfo.val;
	}
	else if (upinfo.type == THUAI6::PlaceType::Door3 || upinfo.type == THUAI6::PlaceType::Door5 || upinfo.type == THUAI6::PlaceType::Door6)
	{
		Access[upinfo.x][upinfo.y] = upinfo.val;
	}
}

template<typename IFooAPI>
void CommandPost<IFooAPI>::Update(TrickerInfo_t tri, int t_)
{
	std::cerr << "Update Tricker Info.\n";
	trickerT = tri.trickerType;
	Bob.Update(tri);
}

template <typename IFooAPI>
bool CommandPost<IFooAPI>::IsAccessible(int x, int y, bool WithWindows)
{
	return WithWindows ? (Access[x][y] >= 1) : (Access[x][y] >= 2);
}

//--------------------
// #CommandPostStudent
//--------------------

void CommandPostStudent::AutoUpdate()
{
	int cntframe = API.GetFrameCount();
	if (cntframe - LastAutoUpdateFrame < UpdateInterval)
		return;
	std::shared_ptr<const THUAI6::Student> selfinfo = API.GetSelfInfo();
	LastAutoUpdateFrame = cntframe;
	for (auto it : Door)
	{
		if (Alice.IsViewable(Grid(selfinfo->x, selfinfo->y).ToCell(), it, selfinfo->viewRange))
		{
			bool newDoor = false, checkopen = API.IsDoorOpen(it.x, it.y);
			//			assert(checkopen);
			if (checkopen && Access[it.x][it.y] == 0U)
			{
				newDoor = true;
				Access[it.x][it.y] = 2U;
				it.DoorStatus = true;
			}
			else if (!checkopen && Access[it.x][it.y] == 2U)
			{
				newDoor = true;
				Access[it.x][it.y] = 0U;
				it.DoorStatus = false;
			}
			if (newDoor)
			{
				Gugu.sendMapUpdate(0, it.DoorType, it.x, it.y, checkopen ? 2U : 0U); // 这里没有区分Door3, Door5, Door6的区别，可能要改
				Gugu.sendMapUpdate(1, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				Gugu.sendMapUpdate(2, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				Gugu.sendMapUpdate(3, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
			}
		}
	}
	for (auto it : Classroom)
	{
		if (Alice.IsViewable(Cell(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetClassroomProgress(it.x, it.y) >= 10000000 && InfoMem[it.x][it.y] < 10000000)
			{
				InfoMem[it.x][it.y] = 10000000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Chest)
	{
		if (Alice.IsViewable(Cell(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetChestProgress(it.x, it.y) >= 10000000 && InfoMem[it.x][it.y] < 10000000)
			{
				InfoMem[it.x][it.y] = 10000000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Gate)
	{
		if (Alice.IsViewable(Cell(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetGateProgress(it.x, it.y) >= 18000 && InfoMem[it.x][it.y] < 18000)
			{
				InfoMem[it.x][it.y] = 18000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
			}
		}
	}

	Bob.AutoUpdate();

	auto tris = this->API.GetTrickers();
	for (auto t : tris)
	{
		Gugu.sendTrickerInfo(0, *t);
		Gugu.sendTrickerInfo(1, *t);
		Gugu.sendTrickerInfo(2, *t);
		Gugu.sendTrickerInfo(3, *t);
	}
}

void CommandPostStudent::AtheleteCanBeginToCharge()
{
	API.UseSkill(0);
}

void CommandPostStudent::TeacherPunish()
{
	API.UseSkill(0);
}

void CommandPostStudent::StraightAStudentWriteAnswers()
{
	API.UseSkill(0);
}

void CommandPostStudent::SunshineRouse()
{
	API.UseSkill(0);
}

void CommandPostStudent::SunshineEncourage()
{
	API.UseSkill(1);
}

void CommandPostStudent::SunshineInspire()
{
	API.UseSkill(2);
}

double CommandPostStudent::AtheleteCanBeginToChargeCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double CommandPostStudent::TeacherPunishCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double CommandPostStudent::StraightAStudentWriteAnswersCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double CommandPostStudent::SunshineRouseCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

double CommandPostStudent::SunshineEncourageCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[1];
}

double CommandPostStudent::SunshineInspireCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[2];
}

//--------------------
// #CommandPostTricker
//--------------------

void CommandPostTricker::AutoUpdate()
{
	std::cerr << "AutoUpdateBegin" << std::endl;
	int cntframe = API.GetFrameCount();
	if (cntframe - LastAutoUpdateFrame < UpdateInterval)
		return;
	auto selfinfo = API.GetSelfInfo();
	LastAutoUpdateFrame = cntframe;
	for (auto it : Door)
	{
		if (Alice.IsViewable(Grid(selfinfo->x, selfinfo->y).ToCell(), it, selfinfo->viewRange))
		{
			bool newDoor = false, checkopen = API.IsDoorOpen(it.x, it.y);
			if (checkopen && Access[it.x][it.y] == 0U)
			{
				newDoor = true;
				Access[it.x][it.y] = 2U;
				it.DoorStatus = true;
			}
			else if (!checkopen && Access[it.x][it.y] == 2U)
			{
				newDoor = true;
				Access[it.x][it.y] = 0U;
				it.DoorStatus = false;
			}
			if (newDoor)
			{
				Gugu.sendMapUpdate(0, it.DoorType, it.x, it.y, checkopen ? 2U : 0U); // 这里没有区分Door3, Door5, Door6的区别，可能要改
				Gugu.sendMapUpdate(1, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				Gugu.sendMapUpdate(2, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
				Gugu.sendMapUpdate(3, it.DoorType, it.x, it.y, checkopen ? 2U : 0U);
			}
		}
	}
	for (auto it : Classroom)
	{
		if (Alice.IsViewable(Grid(selfinfo->x, selfinfo->y).ToCell(), it, selfinfo->viewRange))
		{
			if (API.GetClassroomProgress(it.x, it.y) >= 10000000 && InfoMem[it.x][it.y] < 10000000)
			{
				InfoMem[it.x][it.y] = 10000000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::ClassRoom, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Chest)
	{
		if (Alice.IsViewable(Grid(selfinfo->x, selfinfo->y).ToCell(), it, selfinfo->viewRange))
		{
			if (API.GetChestProgress(it.x, it.y) >= 10000000 && InfoMem[it.x][it.y] < 10000000)
			{
				InfoMem[it.x][it.y] = 10000000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::Chest, it.x, it.y, 10000000);
			}
		}
	}
	for (auto it : Gate)
	{
		if (Alice.IsViewable(Cell(selfinfo->x / 1000, selfinfo->y / 1000), it, selfinfo->viewRange))
		{
			if (API.GetGateProgress(it.x, it.y) >= 18000 && InfoMem[it.x][it.y] < 18000)
			{
				InfoMem[it.x][it.y] = 18000;
				Gugu.sendMapUpdate(0, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(1, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(2, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
				Gugu.sendMapUpdate(3, THUAI6::PlaceType::Gate, it.x, it.y, 18000);
			}
		}
	}

	Bob.AutoUpdate();
	std::cerr << "AutoUpdateEnd" << std::endl;
}

void CommandPostTricker::KleeDefaultAttack(int stux, int stuy) // 传入学生坐标
{
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	API.Attack(atan2(stuy - sy, stux - sx));
}

bool CommandPostTricker::KleeDefaultAttackOver(int rank)
{
	int stux = API.GetStudents()[rank]->x;
	int stuy = API.GetStudents()[rank]->y;
	int sx = API.GetSelfInfo()->x;
	int sy = API.GetSelfInfo()->y;
	double Distance = sqrt((stux - sx) * (stux - sx) + (stuy - sy) * (stuy - sy));
	switch (API.GetStudents()[rank]->studentType)
	{
	case THUAI6::StudentType::Athlete:							 // 运动员
		if (Distance < double(7400 * 400 / 3150 - 0.297 * 3150)) // = 4.13 ??? @dgf
			return true;
	case THUAI6::StudentType::Teacher:							 // 老师
		if (Distance < double(7400 * 400 / 2700 - 0.297 * 2700)) // = 294
			return true;
	case THUAI6::StudentType::StraightAStudent: // 学霸
		if (Distance < double(7400 * 400 / 2880 - 0.297 * 2880))
			return true;
	case THUAI6::StudentType::Sunshine: // 奶妈
		if (Distance < double(7400 * 400 / 3000 - 0.297 * 3000))
			return true;
	}
	return false;
}

void CommandPostTricker::KleeJumpyBomb()
{
	API.UseSkill(0);
}

double CommandPostTricker::KleeJumpyBombCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[0];
}

void CommandPostTricker::KleeSparksNSplash(int StudentID)
{
	API.UseSkill(1, StudentID);

}

double CommandPostTricker::KleeSparksNSplashCD()
{
	return API.GetSelfInfo()->timeUntilSkillAvailable[1];
}

//--------------------
// #Geographer
//--------------------

#if !USE_NEW_ASTAR

template <typename IFooAPI>
int Geographer<IFooAPI>::EstimateTime(Cell Dest)
{
	Cell Self(this->API.GetSelfInfo()->x / 1000, this->API.GetSelfInfo()->y / 1000);
	try
	{
		int Distance = AStar(Self, Dest, true).size();
		int Speed = this->API.GetSelfInfo()->speed;
		int Time = Distance * 1000 / Speed;
		return Time;
	}
	catch (const noway_exception& e)
	{
		std::cerr << "[noway_exception](EstimateTime)Noway." << std::endl;
		return 10000000;
	}
}

#endif

template <typename IFooAPI>
bool Geographer<IFooAPI>::IsViewable(Cell Src, Cell Dest, int ViewRange)
{
	//	std::cerr << "asking " << Src.x << ' ' << Src.y << ' ' << Dest.x << ' ' << Dest.y << std::endl;
	int deltaX = (Dest.x - Src.x) * 1000;
	int deltaY = (Dest.y - Src.y) * 1000;
	int Distance = deltaX * deltaX + deltaY * deltaY;
	auto SrcType = this->Center.Map[Src.x][Src.y];
	auto DestType = this->Center.Map[Dest.x][Dest.y];
	if (DestType == THUAI6::PlaceType::Grass && SrcType != THUAI6::PlaceType::Grass) // 草丛外必不可能看到草丛内
		return false;
	if (Distance < ViewRange * ViewRange)
	{
		double divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100.;
		if (divide == 0)
			return true;
		double dx = deltaX / divide;
		double dy = deltaY / divide;
		double myX = double(Src.x * 1000 + 500);
		double myY = double(Src.y * 1000 + 500);
		if (DestType == THUAI6::PlaceType::Grass && SrcType == THUAI6::PlaceType::Grass) // 都在草丛内，要另作判断
			for (int i = 0; i < divide; i++)
			{
				myX += dx;
				myY += dy;
				if (this->Center.Map[(int)myX / 1000][(int)myY / 1000] != THUAI6::PlaceType::Grass)
					return false;
			}
		else // 不在草丛内，只需要没有墙即可
			for (int i = 0; i < divide; i++)
			{
				myX += dx;
				myY += dy;
				if (this->Center.Map[(int)myX / 1000][(int)myY / 1000] == THUAI6::PlaceType::Wall)
					return false;
			}
		return true;
	}
	else
		return false;
}

/*
#include "utils.hpp"

template <typename IFooAPI>
bool Geographer<IFooAPI>::IsViewable(Cell Src, Cell Dest, int ViewRange)
{
	auto x = this->API.GetFullMap();
	return AssistFunction::HaveView(ViewRange, Src.ToGrid().x, Src.ToGrid().y, Dest.ToGrid().x, Dest.ToGrid().y, x);
}
*/

#if !USE_NEW_ASTAR

template <typename IFooAPI>
bool Geographer<IFooAPI>::IsDestination(int x, int y, Node dest)
{
	if (x == dest.x && y == dest.y)
		return true;
	else
		return false;
}

template <typename IFooAPI>
double Geographer<IFooAPI>::CalculateH(int x, int y, Node dest)
{
	double H = (sqrt((x - dest.x) * (x - dest.x) * 100 + (y - dest.y) * (y - dest.y) * 100));
	return H;
}

template <typename IFooAPI>
std::vector<Node> Geographer<IFooAPI>::MakePath(const std::array<std::array<Node, 50>, 50>& map, Node dest)
{
	int x = dest.x;
	int y = dest.y;
	std::vector<Node> UsablePath;
	while (!(x == -1 || y == -1))
	{
		//		std::cerr << x << ' ' << y << std::endl;
		UsablePath.push_back(map[x][y]);
		int tempX = x;
		int tempY = y;
		x = map[tempX][tempY].parentX;
		y = map[tempX][tempY].parentY;
	}
	std::reverse(UsablePath.begin(), UsablePath.end());
	return UsablePath;
}

template <typename IFooAPI>
std::vector<Node> Geographer<IFooAPI>::AStar(Node src, Node dest, bool WithWindows)
{
	//	std::cerr << "Start AStar " << src.x << ' ' << src.y << ' ' << dest.x << ' ' << dest.y << std::endl;
	std::vector<Node> empty;
	// if (IsValidWithWindows(dest.x, dest.y) == false)
	//{
	//	return empty;
	// }
	if (IsDestination(src.x, src.y, dest))
	{
		return empty;
	}
	bool ClosedList[50][50];
	std::array<std::array<Node, 50>, 50> AStarMap;
	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			AStarMap[x][y].fCost = FLT_MAX;
			AStarMap[x][y].gCost = FLT_MAX;
			AStarMap[x][y].hCost = FLT_MAX;
			AStarMap[x][y].parentX = -1;
			AStarMap[x][y].parentY = -1;
			AStarMap[x][y].x = x;
			AStarMap[x][y].y = y;
			ClosedList[x][y] = false;
		}
	}
	int x = src.x;
	int y = src.y;
	AStarMap[x][y].fCost = 0.0;
	AStarMap[x][y].gCost = 0.0;
	AStarMap[x][y].hCost = 0.0;
	AStarMap[x][y].parentX = -1;
	AStarMap[x][y].parentY = -1;
	std::vector<Node> OpenList;
	OpenList.emplace_back(AStarMap[x][y]);
	bool FoundDest = false;
	while (!OpenList.empty())// && OpenList.size() < 50 * 50)
	{
		Node node;
		{
			float temp = FLT_MAX;
			std::vector<Node>::iterator itNode;
			for (std::vector<Node>::iterator it = OpenList.begin();
				it != OpenList.end(); it = next(it))
			{
				Node n = *it;
				if (n.fCost < temp)
				{
					temp = n.fCost;
					itNode = it;
				}
			}
			node = *itNode;
			OpenList.erase(itNode);
		}
		if (this->Center.IsAccessible(node.x, node.y, WithWindows) == false) continue;
		x = node.x;
		y = node.y;
		//		std::cerr << x << ' ' << y << std::endl;
		ClosedList[x][y] = true;
		for (int newX = -1; newX <= 1; newX++)
		{
			for (int newY = -1; newY <= 1; newY++)
			{
				if (newX != 0 && newY != 0) continue;
				if (newX == 0 && newY == 0) continue;
				if (IsDestination(x + newX, y + newY, dest))
				{
					AStarMap[x + newX][y + newY].parentX = x;
					AStarMap[x + newX][y + newY].parentY = y;
					FoundDest = true;
					return MakePath(AStarMap, dest);
				}
				//				if (x + newX == 19 && y + newY == 27) std::cerr << "ALARM" << this->Center.IsAccessible(x+newX,y+newY,WithWindows) << std::endl;
				if (!(this->Center.IsAccessible(x + newX, y + newY, WithWindows))) continue;
				double gNew, hNew, fNew;
				if (ClosedList[x + newX][y + newY] == false)
				{
					gNew = node.gCost + 1.0;
					hNew = CalculateH(x + newX, y + newY, dest);
					fNew = gNew + hNew;
					if (AStarMap[x + newX][y + newY].fCost == FLT_MAX ||
						AStarMap[x + newX][y + newY].fCost > fNew)
					{
						AStarMap[x + newX][y + newY].fCost = fNew;
						AStarMap[x + newX][y + newY].gCost = gNew;
						AStarMap[x + newX][y + newY].hCost = hNew;
						AStarMap[x + newX][y + newY].parentX = x;
						AStarMap[x + newX][y + newY].parentY = y;
						OpenList.emplace_back(AStarMap[x + newX][y + newY]);
					}
				}
			}
		}
	}
	//	std::cerr << "End AStar" << std::endl;
	if (FoundDest == false)
	{
		for (int i = 0; i < 50; i++) {
			for (int j = 0; j < 50; j++)
				std::cerr << (this->Center.IsAccessible(i, j, true) ? 'X' : '.');
			std::cerr << std::endl;
		}
		//		Sleep(1000000);
		throw noway_exception();
	}
}

template <typename IFooAPI>
Geographer<IFooAPI>::Geographer(IFooAPI& api_, CommandPost<IFooAPI>& Center_) : Friends<IFooAPI>(api_, Center_) { }
#else
template <typename IFooAPI>
Geographer<IFooAPI>::Geographer(IFooAPI& api_, CommandPost<IFooAPI>& Center_) : Friends<IFooAPI>(api_, Center_), SegmentRadius(405), CheckPointRadius(410)
{
	SegmentCompensate = SegmentRadius * tan(PI / 8);
	CheckPointCompensate = CheckPointRadius * tan(PI / 8);
	//	std::cerr << SegmentRadius << ' '<< SegmentCompensate << std::endl;
	InitStableMap();
}

template <typename IFooAPI>
Geop Geographer<IFooAPI>::Escape(Geop P)
{
	Geop Q(1500, 1500);
	double d = 500;
	for (auto s : StableMap)
	{
		Geop R(Project(s, P));
		if (Distance(R, P) < d)
		{
			Q = R;
			d = Distance(R, P);
		}
	}
	for (auto s : VariableMap)
	{
		Geop R(Project(s, P));
		if (Distance(R, P) < d)
		{
			Q = R;
			d = Distance(R, P);
		}
	}
	return Q;
}

#endif

//template <typename IFooAPI>
//bool Geographer<IFooAPI>::IsAccessible(THUAI6::PlaceType pt)
//{
//	return pt == THUAI6::PlaceType::Land || pt == THUAI6::PlaceType::Door3 || pt == THUAI6::PlaceType::Door5 || pt == THUAI6::PlaceType::Door6 || pt == THUAI6::PlaceType::Grass || pt == THUAI6::PlaceType::Window;
//}

template <typename IFooAPI>
void Geographer<IFooAPI>::BackwardExpand(Cell Source, int H[50][50])
{
	for (int i = 0; i < 50; i++)
		for (int j = 0; j < 50; j++)
			H[i][j] = 10000000;
	H[Source.x][Source.y] = 0;
	std::queue<Cell> bfs;
	bfs.push(Source);
	while (!bfs.empty())
	{
		Cell now(bfs.front());
		bfs.pop();
		//		std::cerr << "bfs " << now.x << ' ' << now.y << std::endl;
		for (int i = now.x - 1; i <= now.x + 1; i++)
			for (int j = now.y - 1; j <= now.y + 1; j++)
				if (abs(i - now.x) + abs(j - now.y) == 1 && i >= 0 && i < 50 && j >= 0 && j < 50 && this->Center.IsAccessible(i, j, true))
				{
					if (H[now.x][now.y] + 1 < H[i][j])
					{
						H[i][j] = H[now.x][now.y] + 1;
						bfs.push(Cell(i, j));
						//						std::cerr << "bfs next" << i << ' ' << j << std::endl;
					}
				}
	}
	//	for (int i = 0; i < 50; i++)
	//	{
	//		for (int j = 0; j < 50; j++)
	//			std::cerr << (H[i][j] == 10000000 ? -1 : H[i][j]) << ' ';
	//		std::cerr << std::endl;
	//	}
}

// static int statistic;

#if USE_NEW_ASTAR
template <typename IFooAPI>
bool Geographer<IFooAPI>::DirectReachable(Geop A, Geop B, bool IsDest) // if IsDest, allow 1 through
{
	Geos AB(A, B);
	int icnt = 0;
	for (auto s : StableMap)
	{
		if (Intersect(AB, s))
			icnt++;
		if (!IsDest && icnt == 1 || IsDest && icnt == 2)
			return false;
		//		statistic++;
		//		if (statistic % 10000000 == 0)
		//		{
		//			std::cerr << "statistic count " << statistic << std::endl;
		//		}
	}
	for (auto s : VariableMap)
	{
		if (Intersect(AB, s))
			icnt++;
		if (!IsDest && icnt == 1 || IsDest && icnt == 2)
			return false;
		//		statistic++;
		//		if (statistic % 10000000 == 0)
		//		{
		//			std::cerr << "statistic count " << statistic << std::endl;
		//		}
	}
	return true;
}

template <typename IFooAPI>
void Geographer<IFooAPI>::InitStableMap()
{
	auto map = this->API.GetFullMap();
	bool visited[50][50];
	memset(visited, 0, sizeof(visited));

	for (int i = 1; i < 50; i++)
	{
		int leftP = 0;
		while (leftP < 50)
		{
			if (IsAccessible(map[i - 1][leftP]) == IsAccessible(map[i][leftP]))
				leftP++;
			else
			{
				int rightP = leftP;
				bool upWall = IsAccessible(map[i][leftP]);
				while (IsAccessible(map[i - 1][rightP + 1]) == IsAccessible(map[i - 1][leftP]) && IsAccessible(map[i][rightP + 1]) == IsAccessible(map[i][leftP]))
					rightP++;
				if (upWall)
				{
					Geop LP, RP;
					if (IsAccessible(map[i][leftP - 1]))
						LP = Geop(1000 * i + SegmentRadius, 1000 * leftP - SegmentCompensate);
					else
						LP = Geop(1000 * i + SegmentRadius, 1000 * leftP + SegmentRadius);
					if (IsAccessible(map[i][rightP + 1]))
						RP = Geop(1000 * i + SegmentRadius, 1000 * (rightP + 1) + SegmentCompensate);
					else
						RP = Geop(1000 * i + SegmentRadius, 1000 * (rightP + 1) - SegmentRadius);
					StableMap.push_back(Geos(RP, LP));
					//					std::cerr << "AddSegment (" << RP.x/1000 << ',' << RP.y/1000 << ") -> (" << LP.x/1000 << ',' << LP.y/1000 << ')' << std::endl;
				}
				else
				{
					Geop LP, RP;
					if (IsAccessible(map[i - 1][leftP - 1]))
						LP = Geop(1000 * i - SegmentRadius, 1000 * leftP - SegmentCompensate);
					else
						LP = Geop(1000 * i - SegmentRadius, 1000 * leftP + SegmentRadius);
					if (IsAccessible(map[i - 1][rightP + 1]))
						RP = Geop(1000 * i - SegmentRadius, 1000 * (rightP + 1) + SegmentCompensate);
					else
						RP = Geop(1000 * i - SegmentRadius, 1000 * (rightP + 1) - SegmentRadius);
					StableMap.push_back(Geos(LP, RP));
					//					std::cerr << "AddSegment (" << LP.x/1000 << ',' << LP.y/1000 << ") -> (" << RP.x/1000 << ',' << RP.y/1000 << ')' << std::endl;
				}
				leftP = rightP + 1;
			}
		}
	}

	for (int i = 1; i < 50; i++)
	{
		int upP = 0;
		while (upP < 50)
		{
			if (IsAccessible(map[upP][i - 1]) == IsAccessible(map[upP][i]))
				upP++;
			else
			{
				int downP = upP;
				bool leftWall = IsAccessible(map[upP][i]);
				while (IsAccessible(map[downP + 1][i - 1]) == IsAccessible(map[upP][i - 1]) && IsAccessible(map[downP + 1][i]) == IsAccessible(map[upP][i]))
					downP++;
				if (leftWall)
				{
					Geop UP, DP;
					if (IsAccessible(map[upP - 1][i]))
						UP = Geop(1000 * upP - SegmentCompensate, 1000 * i + SegmentRadius);
					else
						UP = Geop(1000 * upP + SegmentRadius, 1000 * i + SegmentRadius);
					if (IsAccessible(map[downP + 1][i]))
						DP = Geop(1000 * (downP + 1) + SegmentCompensate, 1000 * i + SegmentRadius);
					else
						DP = Geop(1000 * (downP + 1) - SegmentRadius, 1000 * i + SegmentRadius);
					StableMap.push_back(Geos(UP, DP));
					//					std::cerr << "AddSegment (" << UP.x/1000 << ',' << UP.y/1000 << ") -> (" << DP.x/1000 << ',' << DP.y/1000 << ')' << std::endl;
				}
				else
				{
					Geop UP, DP;
					if (IsAccessible(map[upP - 1][i - 1]))
						UP = Geop(1000 * upP - SegmentCompensate, 1000 * i - SegmentRadius);
					else
						UP = Geop(1000 * upP + SegmentRadius, 1000 * i - SegmentRadius);
					if (IsAccessible(map[downP + 1][i - 1]))
						DP = Geop(1000 * (downP + 1) + SegmentCompensate, 1000 * i - SegmentRadius);
					else
						DP = Geop(1000 * (downP + 1) - SegmentRadius, 1000 * i - SegmentRadius);
					StableMap.push_back(Geos(DP, UP));
					//					std::cerr << "AddSegment (" << DP.x/1000 << ',' << DP.y/1000 << ") -> (" << UP.x/1000 << ',' << UP.y/1000 << ')' << std::endl;
				}
				upP = downP + 1;
			}
		}
	}

	for (int i = 1; i < 50; i++)
		for (int j = 1; j < 50; j++)
		{
			int cntWall = (int)!IsAccessible(map[i - 1][j - 1]) + (int)!IsAccessible(map[i - 1][j]) + (int)!IsAccessible(map[i][j - 1]) + (int)!IsAccessible(map[i][j]);
			if (cntWall == 1)
			{
				Geop AP, BP, CAP, CBP;
				if (!IsAccessible(map[i - 1][j - 1]))
				{
					AP = Geop(1000 * i + SegmentCompensate, 1000 * j + SegmentRadius);
					BP = Geop(1000 * i + SegmentRadius, 1000 * j + SegmentCompensate);
				}
				else if (!IsAccessible(map[i - 1][j]))
				{
					AP = Geop(1000 * i + SegmentRadius, 1000 * j - SegmentCompensate);
					BP = Geop(1000 * i + SegmentCompensate, 1000 * j - SegmentRadius);
				}
				else if (!IsAccessible(map[i][j - 1]))
				{
					AP = Geop(1000 * i - SegmentRadius, 1000 * j + SegmentCompensate);
					BP = Geop(1000 * i - SegmentCompensate, 1000 * j + SegmentRadius);
				}
				else if (!IsAccessible(map[i][j]))
				{
					AP = Geop(1000 * i - SegmentCompensate, 1000 * j - SegmentRadius);
					BP = Geop(1000 * i - SegmentRadius, 1000 * j - SegmentCompensate);
				}

				if (!IsAccessible(map[i - 1][j - 1]))
				{
					CAP = Geop(1000 * i + CheckPointCompensate, 1000 * j + CheckPointRadius);
					CBP = Geop(1000 * i + CheckPointRadius, 1000 * j + CheckPointCompensate);
				}
				else if (!IsAccessible(map[i - 1][j]))
				{
					CAP = Geop(1000 * i + CheckPointRadius, 1000 * j - CheckPointCompensate);
					CBP = Geop(1000 * i + CheckPointCompensate, 1000 * j - CheckPointRadius);
				}
				else if (!IsAccessible(map[i][j - 1]))
				{
					CAP = Geop(1000 * i - CheckPointRadius, 1000 * j + CheckPointCompensate);
					CBP = Geop(1000 * i - CheckPointCompensate, 1000 * j + CheckPointRadius);
				}
				else if (!IsAccessible(map[i][j]))
				{
					CAP = Geop(1000 * i - CheckPointCompensate, 1000 * j - CheckPointRadius);
					CBP = Geop(1000 * i - CheckPointRadius, 1000 * j - CheckPointCompensate);
				}
				StableMap.push_back(Geos(AP, BP));
				//				std::cerr << "AddSegment (" << AP.x/1000 << ',' << AP.y/1000 << ") -> (" << BP.x/1000 << ',' << BP.y/1000 << ')' << std::endl;
				StableCheckPoint.push_back(CAP);
				StableCheckPoint.push_back(CBP);
			}
		}
	std::cerr << StableCheckPoint.size() << std::endl;
}

template <typename IFooAPI>
bool Geographer<IFooAPI>::InsideObstacle(Geop P)
{
	double rot = 0;
	for (auto s : StableMap)
		rot += s.GetTheta(P);
	for (auto s : VariableMap)
		rot += s.GetTheta(P);
	std::cerr << "Rot = " << rot / (2 * PI) << " (cycle)" << std::endl;
	if (fabs(rot - 2 * PI) < PI * 0.8)
		return false;
	else
		return true;
}

template <typename IFooAPI>
void Geographer<IFooAPI>::AddPlayer()
{
	//	std::cerr << "Add Player" << std::endl;
	auto stuinfo = this->API.GetStudents();
	double rads = (400 + SegmentRadius) / cos(PI / 8);
	for (auto s : stuinfo)
	{
		if (s->playerID == this->API.GetSelfInfo()->playerID)
			continue;
		Geop P[8], CP[8];
		for (int i = 0; i < 8; i++)
		{
			double theta = i * PI / 4 + PI / 8;
			P[i] = Geop(s->x + rads * cos(theta), s->y + rads * sin(theta));
			CP[i] = Geop(s->x + rads * cos(theta), s->y + rads * sin(theta));
		}
		for (int i = 1; i < 8; i++)
		{
			VariableMap.push_back(Geos(P[i], P[i - 1]));
			//			std::cerr << "AddSegment (" << P[i].x/1000 << ',' << P[i].y/1000 << ") -> (" << P[i-1].x/1000 << ',' << P[i-1].y/1000 << ')' << std::endl;
			VariableCheckPoint.push_back(CP[i]);
		}
		VariableMap.push_back(Geos(P[0], P[7]));
		VariableCheckPoint.push_back(CP[0]);
	}

	auto trkinfo = this->API.GetTrickers();
	for (auto s : trkinfo)
	{
		if (s->playerID == this->API.GetSelfInfo()->playerID)
			continue;
		Geop P[8], CP[8];
		for (int i = 0; i < 8; i++)
		{
			double theta = i * PI / 4 + PI / 8;
			P[i] = Geop(s->x + rads * cos(theta), s->y + rads * sin(theta));
			CP[i] = Geop(s->x + rads * cos(theta), s->y + rads * sin(theta));
		}
		for (int i = 1; i < 8; i++)
		{
			VariableMap.push_back(Geos(P[i], P[i - 1]));
			//			std::cerr << "AddSegment (" << P[i].x/1000 << ',' << P[i].y/1000 << ") -> (" << P[i-1].x/1000 << ',' << P[i-1].y/1000 << ')' << std::endl;
			VariableCheckPoint.push_back(CP[i]);
		}
		VariableMap.push_back(Geos(P[0], P[7]));
		VariableCheckPoint.push_back(CP[0]);
	}
}

class GeographerNode
{
public:
	double valueQ, valueH;
	bool parenttype, type;
	int parentid, id;
	double value() const { return valueQ + valueH; }
	GeographerNode(double valueQ_, double valueH_, bool type_, int id_, bool parenttype_, int parentid_)
		: valueQ(valueQ_), valueH(valueH_), type(type_), id(id_), parenttype(parenttype_), parentid(parentid_) {}

	bool operator<(const GeographerNode& n) const { return value() > n.value(); }
};

template <typename IFooAPI>
std::vector<Geop> Geographer<IFooAPI>::FindPath(Geop From_, Geop Dest_)
{
	std::cerr << "Destination is (" << Dest_.x << ' ' << Dest_.y << ')' << std::endl;
	AddPlayer();
	std::vector<Geop> Path;
	bool x = InsideObstacle(From_);
	if (x)
	{
		Path.push_back(Escape(From_));
		VariableCheckPoint.clear();
		VariableMap.clear();
		return Path;
	}
	int preH[50][50];
	BackwardExpand(Cell(Dest_.x / 1000, Dest_.y / 1000), preH);
	std::vector<double> StableF(StableCheckPoint.size(), 1e10);
	VariableCheckPoint.push_back(From_);
	VariableCheckPoint.push_back(Dest_);
	std::vector<double> VariableF(VariableCheckPoint.size(), 1e10);

	std::vector<std::pair<double, int>> StableParent(StableCheckPoint.size());
	std::vector<std::pair<double, int>> VariableParent(VariableCheckPoint.size());
	VariableParent[VariableCheckPoint.size() - 1].second = -1;
	std::priority_queue<GeographerNode> Q;
	Q.push(GeographerNode(0, preH[(int)From_.x / 1000][(int)From_.y / 1000], 1, VariableCheckPoint.size() - 2, 1, VariableCheckPoint.size() - 2));
	int cnt1 = 0, cnt2 = 0;
	while (true)
	{
		GeographerNode A(Q.top());
		Q.pop();
		Geop ACell = (A.type ? VariableCheckPoint : StableCheckPoint)[A.id];
		if (A.value() > (A.type ? VariableF : StableF)[A.id])
			continue;
		//		if (A.type || true)
		//		{
		//			std::cerr << "AStaring Cell: " << ACell.x << ' ' << ACell.y << ' ' << A.value() << std::endl;
		//			std::cerr << "Last Cell: " << A.parenttype << ' ' << A.parentid << std::endl;
		//		}
		(A.type ? VariableF : StableF)[A.id] = A.value();
		(A.type ? VariableParent : StableParent)[A.id] = std::make_pair(A.parenttype, A.parentid);
		//		std::cerr << "Fine." << std::endl;
		if (A.type && A.id == VariableCheckPoint.size() - 1)
		{
			break;
		}
		for (int i = 0; i < StableCheckPoint.size(); i++)
		{
			cnt1++;
			Geop p = StableCheckPoint[i];
			if (DirectReachable(ACell, p))
			{
				cnt2++;
				//				std::cerr << "AStaring Next Cell: " << p.x << ' ' << p.y << std::endl;
				Q.push(GeographerNode(A.valueQ + Distance(ACell, p) + 1, preH[(int)p.x / 1000][(int)p.y / 1000], 0, i, A.type, A.id));
				//				Sleep(1000);
			}
		}
		//		std::cerr << "Fine.2" << std::endl;
		//		Sleep(1000);
		for (int i = 0; i < VariableCheckPoint.size(); i++)
		{
			cnt1++;
			Geop p = VariableCheckPoint[i];
			if (DirectReachable(ACell, p))
			{
				cnt2++;
				//				std::cerr << "AStaring Next Cell: " << p.x << ' ' << p.y << std::endl;
				Q.push(GeographerNode(A.valueQ + Distance(ACell, p) + 1, preH[(int)p.x / 1000][(int)p.y / 1000], 1, i, A.type, A.id));
				//				Sleep(1000);
			}
		}
		//		std::cerr << "Fine.6" << std::endl;
		//		Sleep(1000);
	}
	if (VariableParent[VariableCheckPoint.size() - 1].second != -1)
	{
		Path.push_back(Dest_);
		bool tp = true;
		int id = VariableCheckPoint.size() - 1;
		while (!(tp && id == VariableCheckPoint.size() - 2))
		{
			auto pr = (tp ? VariableParent : StableParent)[id];
			assert(!(tp == pr.first && id == pr.second));
			tp = pr.first;
			id = pr.second;
			Geop last = (tp ? VariableCheckPoint : StableCheckPoint)[id];
			//			std::cerr << "AStaring Traceback Cell: " << last.x << ' ' << last.y << std::endl;
			Path.push_back(last);
		}
	}
	std::cerr << "cnt1 = " << cnt1 << ';' << "cnt2 = " << cnt2 << std::endl;
	std::reverse(Path.begin(), Path.end());
	VariableCheckPoint.clear();
	VariableMap.clear();
	return Path;
}
#endif

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

#define HAPPY_HAPPY_HA_PPY 0

#if HAPPY_HAPPY_HA_PPY
extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Teacher };
#else
extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Sunshine };
#endif


extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Klee;
// 可以在AI.cpp内部声明变量与函数

/* 人物状态
sCantMove 动弹不得（沉迷/前后摇/毕业……）
sDefault 默认
sDoClassroom 去写作业/在写作业
sOpenGate 去开校门/在开校门
sOpenChest 去开箱子/在开箱子
sDanger 应对危险
sRousing 唤醒某人
sEncouraging 勉励某人
sPicking 去捡道具
以上是所有角色普遍拥有的状态，其他状态需要自行定义，建议从0x80开始
比如运动员可以定义一个状态叫正面硬刚，老师可以定义一个状态叫巡逻（试图在视野内跟着捣蛋鬼），等等
基本逻辑就是每次AI::Play都会根据信息来决定维持当前状态还是跳到另一个状态（类似于图灵机模拟思维）
需要一些空间储存必要的信息，可能会用到通信
或许可以给各种状态分个类或者优先级，方便写代码
*/
#define sCantMove 0x00
#define sDefault 0x10

#define sDoClassroom 0x11
#define sOpenGate 0x12
#define sOpenChest 0x13
#define sDanger 0x14
#define sRousing 0x15
#define sEncouraging 0x16
#define sPicking 0x17
#define sInspiring 0x18
#define sFleeing 0x19

#define sFindPlayer 0x20
#define sAttackPlayer 0x21
#define sChasePlayer 0x22
#define sRescuePlayer 0x23
#define sRunnerPlayer 0x24


#if HAPPY_HAPPY_HA_PPY
void AI::play(IStudentAPI& api)
{
	api.PrintSelfInfo();
	static std::vector<unsigned char> Priority = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	static CommandPostStudent Center(api);
	static int CurrentState = sDefault;


	Center.AutoUpdate();

	int x[4] = { 48500, 48500, 4500, 46379 };
	int y[4] = { 16500, 22500, 27500, 21621 };
	int ang[4] = { 1571, -1571, 0, -785 };

	int id = api.GetSelfInfo()->playerID;
	int tick = 600;

	if (api.GetFrameCount() < tick)
	{
		if (!Center.NearCell(Cell(x[id] / 1000, y[id] / 1000), 2)) Center.MoveTo(Cell(x[id] / 1000, y[id] / 1000), true);
		else
		{
			int dx = api.GetSelfInfo()->x - x[id];
			int dy = api.GetSelfInfo()->y - y[id];
			if (std::abs(dx) >= 100 || std::abs(dy) >= 100)
			{
				int t = sqrt(dx * dx + dy * dy) * 1000 / api.GetSelfInfo()->speed;
				std::cerr << "t = " << t << std::endl;
				api.Move(t, atan2(-dy, -dx));
			}
		}
	}
	else if (api.GetFrameCount() == tick)
	{
		api.EndAllAction();
		api.UseSkill(1, ang[id]);
	}
	else if (api.GetFrameCount() <= 2 * tick) api.StartOpenChest();
	else
	{
		if (!api.GetProps().empty())
		{
			auto pr = api.GetProps().front()->type;
			api.PickProp(pr);
			api.UseProp(pr);
		}
	}
}
#else
void AI::play(IStudentAPI& api)
{
	api.PrintSelfInfo();
	static std::vector<unsigned char> Priority = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	static CommandPostStudent Center(api);
	static int CurrentState = sDefault;


	Center.AutoUpdate();
	//	Center.Bob._display(4);

	int MessageType;
	bool IsRescue = false;
	while ((MessageType = Center.Gugu.receiveMessage()) != NoMessage)
	{
		//		std::cerr << "MessageType = " << MessageType << std::endl;
		switch (MessageType)
		{
		case MapUpdate:
		{
			auto ms = Center.Gugu.receiveMapUpdate();
			//			std::cerr << "[custom]" << ms.second.x << ' ' << ms.second.y << std::endl;
			//			api.Print(std::to_string(ms.second.x) + " " + std::to_string(ms.second.y));
			Center.Update(ms.second, ms.first);
		}
		break;
		case Rescue:
		{
			IsRescue = Center.Gugu.receiveRescue();
		}
		break;
		case TrickerInfo:
		{
			std::cerr << "receive Tricker Info.\n";
			auto triinfo = Center.Gugu.receiveTrickerInfo();
			Center.Update(triinfo, 0);
		}
		break;
		}
	}
	//	std::cerr << "[FinishedClassroom]" << Center.CountFinishedClassroom() << std::endl;
	//	std::cerr << "[OpenGate]" << Center.CountOpenGate() << std::endl;
	//	for (int i = 0; i < Center.Gate.size(); i++)
	//	{
	//		std::cerr << "[Gate" << i << "]" << api.GetGateProgress(Center.Gate[i].x, Center.Gate[i].y) << std::endl;
	//	}
		/*
		for (int i = 0; i < 50; i++)
		{
			for (int j = 0; j < 50; j++)
				std::cerr << (int)Center.Access[i][j];
			std::cerr << std::endl;
		}
		*/
		//	for (auto d : Center.Door) std::cerr << "ALARM " << d.x << ' ' << d.y << ' ' << api.IsDoorOpen(d.x, d.y) << ' ' << Center.Alice.IsViewable(Grid(api.GetSelfInfo()->x, api.GetSelfInfo()->y).ToCell(), d, api.GetSelfInfo()->viewRange) << std::endl;

			// 公共操作
	if (this->playerID == 0)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		bool haveTricker = false;
		if (trickerT != THUAI6::TrickerType::ANoisyPerson)
		{
			if (api.GetSelfInfo()->dangerAlert >= 1)
				haveTricker = true;
		}
		else if (!triinfo.empty()) haveTricker = true;

		switch (CurrentState)
		{
		case sDefault:
			if (haveTricker)
				CurrentState = sFleeing;
			break;
		case sFleeing:
			if (!haveTricker)
				CurrentState = sDefault;
			break;
		}

		std::cerr << "CurrentState: " << ((CurrentState == sFleeing) ? ("sFleeing\n") : ("sDefault\n"));

		switch (CurrentState)
		{
		case sDefault:
			if (Center.CountFinishedClassroom() >= 7)
			{
				if (!Center.CountOpenGate())
				{
					// api.EndAllAction();
					Center.DirectOpeningGate(true, true);
				}
				else
				{
					// api.EndAllAction();
					Center.DirectGraduate(true);
				}
			}
			else
			{
				// api.EndAllAction();
				if (Center.NearClassroom(true) && !Center.StraightAStudentWriteAnswersCD())
				{
					Center.StraightAStudentWriteAnswers();
				}
				Center.DirectLearning(false);
			}
			break;
		case sFleeing:
			if (!triinfo.empty())
			{
				Center.DirectHide(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), triinfo[0]->viewRange, Center.NearCell(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), 4) ? 1 : 0);
			}
			else
			{
				Center.DirectGrass(1);
			}
			break;
		}
	}
	else if (this->playerID == 1)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		bool haveTricker = false;
		if (trickerT != THUAI6::TrickerType::ANoisyPerson)
		{
			if (api.GetSelfInfo()->dangerAlert >= 1)
				haveTricker = true;
		}
		else if (!triinfo.empty()) haveTricker = true;

		switch (CurrentState)
		{
		case sDefault:
			if (haveTricker)
				CurrentState = sFleeing;
			break;
		case sFleeing:
			if (!haveTricker)
				CurrentState = sDefault;
			break;
		}

		switch (CurrentState)
		{
		case sDefault:
			if (Center.CountFinishedClassroom() >= 7)
			{
				if (!Center.CountOpenGate())
				{
					// api.EndAllAction();
					Center.DirectOpeningGate(true, true);
				}
				else
				{
					// api.EndAllAction();
					Center.DirectGraduate(true);
				}
			}
			else
			{
				// api.EndAllAction();
				if (Center.NearClassroom(true) && !Center.StraightAStudentWriteAnswersCD())
				{
					Center.StraightAStudentWriteAnswers();
				}
				Center.DirectLearning(false);
			}
			break;
		case sFleeing:
			//			std::cerr << "IMHERE";
			if (!triinfo.empty())
			{
				//				std::cerr << "IMHERE1";
				Center.DirectHide(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), triinfo[0]->viewRange, Center.NearCell(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), 4) ? 1 : 0);
			}
			else
			{
				//				std::cerr << "IMHERE2";
				Center.DirectGrass(1);
			}
			break;
		}

		// 玩家1执行操作
	}
	else if (this->playerID == 2)
	{
		//auto stuinfo = api.GetStudents();
		//auto triinfo = api.GetTrickers();

		//static bool visitClassroom[10];
		//static bool visitClassroomUpdated[10];
		//static int countVisitedClassroom = 0;
		//bool haveTricker = false;
		//if (!triinfo.empty())
		//	haveTricker = true;
		//static bool ChaseIt = false;
		//static Grid ChaseDest;
		//bool haveAddictedStudent = false;
		//int AddictedId = -1;
		//for (int i = 0; i < stuinfo.size(); i++)
		//{
		//	if (stuinfo[i]->playerState == THUAI6::PlayerState::Addicted && stuinfo[i]->guid != api.GetSelfInfo()->guid)
		//	{
		//		haveAddictedStudent = true;
		//		AddictedId = i;
		//	}
		//}

		//switch (CurrentState)
		//{
		//case sDefault:
		//	if (haveTricker && !Center.AtheleteCanBeginToChargeCD())
		//		CurrentState = sAttackPlayer;
		//	else if (ChaseIt)
		//		CurrentState = sChasePlayer;
		//	else if (haveAddictedStudent)
		//		CurrentState = sRousing;
		//	else
		//		CurrentState = sFindPlayer;
		//	break;
		//case sFindPlayer:
		//	if (haveTricker)
		//		CurrentState = sAttackPlayer;
		//	break;
		//case sAttackPlayer:
		//	ChaseIt = true;
		//	if (!triinfo.empty())
		//		ChaseDest = Grid(triinfo[0]->x, triinfo[0]->y);
		//	if (!haveTricker && !Center.AtheleteCanBeginToChargeCD())
		//		CurrentState = sChasePlayer;
		//	if (Center.AtheleteCanBeginToChargeCD())
		//	{
		//		ChaseIt = false;
		//		CurrentState = sDefault;
		//	}
		//	break;
		//case sChasePlayer:
		//	if (haveTricker)
		//		CurrentState = sAttackPlayer;
		//	else if (Center.NearCell(ChaseDest.ToCell(), 2))
		//	{
		//		ChaseIt = false;
		//		CurrentState = sDefault;
		//	}
		//	break;
		//case sRousing:
		//	if (!haveAddictedStudent)
		//		CurrentState = sDefault;
		//	break;
		//	break;
		//}

		//switch (CurrentState)
		//{
		//case sDefault:
		//	std::cerr << "CurrentState: sDefault" << std::endl;
		//	break;
		//case sFindPlayer:
		//	std::cerr << "CurrentState: sFindPlayer" << std::endl;
		//	if (Center.NearClassroom(false))
		//	{
		//		for (int i = 0; i < 10; i++)
		//			if (Center.NearCell(Center.Classroom[i], 3))
		//			{
		//				visitClassroom[i] = true;
		//				// countVisitedClassroom++;
		//			}
		//	}
		//	for (int i = 0; i < 10; i++)
		//	{
		//		if (visitClassroom[i] && !visitClassroomUpdated[i])
		//		{
		//			countVisitedClassroom++;
		//			visitClassroomUpdated[i] = true;
		//		}
		//	}
		//	if (countVisitedClassroom == 10)
		//	{
		//		for (int i = 0; i < 10; i++)
		//		{
		//			visitClassroom[i] = false;
		//			visitClassroomUpdated[i] = false;
		//		}
		//		countVisitedClassroom = 0;
		//	}
		//	for (int i = 0; i < 10; i++)
		//		if (!visitClassroom[i])
		//		{
		//			Center.MoveTo(Center.Classroom[i], 1);
		//			break;
		//		}
		//	break;
		//case sAttackPlayer:
		//	std::cerr << "CurrentState: sAttackPlayer" << std::endl;

		//	if (!triinfo.empty())
		//	{
		//		auto tritype = triinfo[0]->trickerType;
		//		auto trirange = sqrt((triinfo[0]->x - api.GetSelfInfo()->x) * (triinfo[0]->x - api.GetSelfInfo()->x) + (triinfo[0]->y - api.GetSelfInfo()->y) * (triinfo[0]->y - api.GetSelfInfo()->y));
		//		if (!Center.AtheleteCanBeginToChargeCD() && (static_cast<int>(tritype) == 1 && trirange < 1954.5 || static_cast<int>(tritype) == 2 && trirange < 2050 || static_cast<int>(tritype) == 3 && trirange < 1981.31 || static_cast<int>(tritype) == 4 && trirange < 2050))
		//		// 攻击（满足有技能，在攻击范围内）
		//		{
		//			Center.AtheleteCanBeginToCharge();
		//			Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
		//		}
		//		if (api.GetSelfInfo()->speed > 6400 && static_cast<int>(triinfo[0]->playerState) != 8)
		//		{
		//			Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
		//		}
		//		else if ((api.GetSelfInfo()->x - stuinfo[1]->x) * (api.GetSelfInfo()->x - stuinfo[1]->x) + (api.GetSelfInfo()->y - stuinfo[1]->y) * (api.GetSelfInfo()->y - stuinfo[1]->y) < 25000000)
		//		{
		//			Center.MoveTo(Cell(stuinfo[1]->x / 1000, stuinfo[1]->y / 1000), true);
		//		}
		//		// 逃跑
		//		else
		//		{
		//			for (int i = 0; i < 10; i++)
		//				if (!visitClassroom[i])
		//				{
		//					Center.MoveTo(Center.Classroom[i], 1);
		//					break;
		//				}
		//		}
		//		//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
		//	}
		//	// else
		//	//{
		//	// api.EndAllAction();
		//	//	Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
		//	//}
		//	break;
		//case sChasePlayer:
		//	std::cerr << "CurrentState: sChasePlayer" << std::endl;
		//	Center.MoveTo(ChaseDest.ToCell(), true);
		//	break;
		//case sRousing:
		//	std::cerr << "CurrentState: sRousing" << std::endl;
		//	ChaseIt = true;
		//	if (stuinfo.size() > AddictedId)
		//	{
		//		ChaseDest = Grid(stuinfo[AddictedId]->x, stuinfo[AddictedId]->y);
		//		if (Center.NearCell(Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), 2))
		//		{
		//			api.StartRouseMate(stuinfo[AddictedId]->playerID);
		//		}
		//		else
		//		{
		//			Center.MoveTo(Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), true);
		//		}
		//	}
		//	break;
		//}
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		static bool visitClassroom[10];
		static bool visitClassroomUpdated[10];
		static int countVisitedClassroom = 0;
		bool haveTricker = false;
		if (!triinfo.empty())
			haveTricker = true;
		static bool ChaseIt = false;
		static Grid ChaseDest;
		static int CurrentState_Bef = sDefault;
		static Cell Bef;
		static Cell Bef_stu;



		switch (CurrentState)
		{
		case sDefault:
			if (haveTricker)
				CurrentState = sAttackPlayer;
			/*else if (ChaseIt)
				CurrentState = sChasePlayer;*/
			else if (!haveTricker && IsRescue)
				CurrentState = sRescuePlayer;
			else
				CurrentState = sFindPlayer;
			break;
		case sFindPlayer:
			if (haveTricker)
				CurrentState = sAttackPlayer;
			break;
		case sAttackPlayer:
			if (haveTricker)
			{
				if (Center.NearCell(Bef_stu, 6))
					CurrentState = sRunnerPlayer;
				/*ChaseIt = true;*/
				if (!Center.NearCell(ChaseDest.ToCell(), 4))
					ChaseDest = Grid(triinfo[0]->x, triinfo[0]->y);
				else
				{
					/*ChaseIt = false;*/
					CurrentState = sDefault;
				}
			}
			else
				CurrentState = sFindPlayer;
			break;
		case sRescuePlayer:
			if (haveTricker)
				CurrentState = sAttackPlayer;
			break;
		case sRunnerPlayer:
			if (haveTricker && !Center.NearCell(Bef_stu, 3))
				CurrentState = sAttackPlayer;
			else if (!haveTricker)
				CurrentState = sFindPlayer;
			break;
			//case sChasePlayer:
			//	if (haveTricker && !Center.NearCell(ChaseDest.ToCell(), 4))
			//		CurrentState = sAttackPlayer;
			//	else
			//	{
			//		/*ChaseIt = false;*/
			//		CurrentState = sDefault;
			//	}
			//	break;
				//case sChasePlayer:
				//	if (haveTricker && !Center.NearCell(ChaseDest.ToCell(), 4))
				//		CurrentState = sAttackPlayer;
				//	else
				//	{
				//		/*ChaseIt = false;*/
				//		CurrentState = sDefault;
				//	}
				//	break;
		}

		switch (CurrentState)
		{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			Bef_stu = Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000);
			break;
		case sFindPlayer:
			if (CurrentState_Bef == sAttackPlayer && !haveTricker && !Center.NearCell(Bef, 4) && !Center.NearCell(Bef_stu, 6))
				/*if (CurrentState_Bef == sAttackPlayer && !haveTricker && !Center.NearCell(Bef, 5) && !Center.NearCell(Bef_stu, 3))*/
				Center.MoveTo(Bef, true);
			std::cerr << "CurrentState: sFindPlayer" << std::endl;
			if (CurrentState_Bef == sAttackPlayer && !haveTricker)
			{
				if (Center.NearCell(Bef, 4)) CurrentState_Bef = sFindPlayer;
				Center.MoveTo(Bef, true);
				break;
			}
			for (int i = 0; i < 10; i++)
				if (Center.NearCell(Center.Classroom[i], 3))
				{
					visitClassroom[i] = true;
					// countVisitedClassroom++;
				}
			for (int i = 0; i < 10; i++)
			{
				if (visitClassroom[i] && !visitClassroomUpdated[i])
				{
					countVisitedClassroom++;
					visitClassroomUpdated[i] = true;
				}
			}
			if (countVisitedClassroom == 10)
			{
				for (int i = 0; i < 10; i++)
				{
					visitClassroom[i] = false;
					visitClassroomUpdated[i] = false;
				}
				countVisitedClassroom = 0;
			}
			for (int i = 0; i < 10; i++)
				if (!visitClassroom[i])
				{
					Center.MoveTo(Center.Classroom[i], 1);
					break;
				}
			Bef_stu = Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000);
			break;
		case sAttackPlayer:
			if (haveTricker)
				if (Center.Alice.IsViewable(Grid(api.GetSelfInfo()->x, api.GetSelfInfo()->y).ToCell(), Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), api.GetSelfInfo()->viewRange) && (triinfo[0]->playerState == THUAI6::PlayerState::Climbing || triinfo[0]->playerState == THUAI6::PlayerState::Locking || triinfo[0]->playerState == THUAI6::PlayerState::Attacking || triinfo[0]->playerState == THUAI6::PlayerState::Swinging))
				{
					if (!Center.TeacherPunishCD())
						Center.TeacherPunish();
					else
					{
						/*for (int i = 0; i < 10; i++)
							if (!visitClassroom[i])
							{
								Center.MoveTo(Center.Classroom[i], 1);
								break;
							}*/
						if (Center.NearCell(ChaseDest.ToCell(), 5))
							Center.MoveTo(Cell(2 * api.GetSelfInfo()->x / 1000 - triinfo[0]->x / 1000, 2 * api.GetSelfInfo()->y / 1000 - triinfo[0]->y / 1000), true);
						else
							Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
					}
					//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
				}
				else
				{
					// api.EndAllAction();
					/*Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);*/
					if (Center.NearCell(ChaseDest.ToCell(), 5))
						Center.MoveTo(Cell(2 * api.GetSelfInfo()->x / 1000 - triinfo[0]->x / 1000, 2 * api.GetSelfInfo()->y / 1000 - triinfo[0]->y / 1000), true);
					/*Center.MoveTo(Cell(2 * api.GetSelfInfo()->x / 1000 - triinfo[0]->x / 1000, 2 * api.GetSelfInfo()->y / 1000 - triinfo[0]->y / 1000), true);*/
					else
						Center.MoveTo(Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000), true);
				}

			Bef = Cell(triinfo[0]->x / 1000, triinfo[0]->y / 1000);
			Bef_stu = Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000);
			break;
		case sRescuePlayer:
			Center.MoveTo(Cell(stuinfo[3]->x / 1000, stuinfo[3]->y / 1000), true);
			Bef_stu = Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000);
			break;
		case sRunnerPlayer:
			std::cerr << "CurrentState: sFindPlayer" << std::endl;
			for (int i = 0; i < 10; i++)
				if (Center.NearCell(Center.Classroom[i], 3))
				{
					visitClassroom[i] = true;
					// countVisitedClassroom++;
				}
			for (int i = 0; i < 10; i++)
			{
				if (visitClassroom[i] && !visitClassroomUpdated[i])
				{
					countVisitedClassroom++;
					visitClassroomUpdated[i] = true;
				}
			}
			if (countVisitedClassroom == 10)
			{
				for (int i = 0; i < 10; i++)
				{
					visitClassroom[i] = false;
					visitClassroomUpdated[i] = false;
				}
				countVisitedClassroom = 0;
			}
			for (int i = 0; i < 10; i++)
				if (!visitClassroom[i])
				{
					Center.MoveTo(Center.Classroom[i], 1);
					break;
				}
			Bef_stu = Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000);
			break;
			/*case sChasePlayer:
				std::cerr << "CurrentState: sChasePlayer" << std::endl;
				Center.MoveTo(ChaseDest.ToCell(), true);
				if (Center.NearCell(ChaseDest.ToCell(), 3))
				{
					ChaseIt = false;
					CurrentState = sDefault;
				}
				break;*/
				/*case sChasePlayer:
					std::cerr << "CurrentState: sChasePlayer" << std::endl;
					Center.MoveTo(ChaseDest.ToCell(), true);
					if (Center.NearCell(ChaseDest.ToCell(), 3))
					{
						ChaseIt = false;
						CurrentState = sDefault;
					}
					break;*/
		}
		CurrentState_Bef = CurrentState;
		// 玩家2执行操作
	}

	else if (this->playerID == 3)
	{
		auto stuinfo = api.GetStudents();
		auto triinfo = api.GetTrickers();
		static bool visitClassroom[10];
		static bool visitClassroomUpdated[10];
		static int countVisitedClassroom = 0;
		bool haveTricker = false;
		if (trickerT != THUAI6::TrickerType::ANoisyPerson)
		{
			if (api.GetSelfInfo()->dangerAlert >= 1.5)
				haveTricker = true;
		}
		else if (!triinfo.empty()) haveTricker = true;
		bool haveAddictedStudent = false;
		int AddictedId = -1;
		static bool ChaseIt = false;
		static Cell ChaseDest;
		for (int i = 0; i < stuinfo.size(); i++)
		{
			if (stuinfo[i]->playerState == THUAI6::PlayerState::Addicted && stuinfo[i]->guid != api.GetSelfInfo()->guid)
			{
				haveAddictedStudent = true;
				AddictedId = i;
			}
		}
		bool Needhelp = false;
		int NeedhelpID = -1;
		for (int i = 0; i < stuinfo.size(); i++)
		{
			if (stuinfo[i]->determination < 750000 && stuinfo[i]->guid != api.GetSelfInfo()->guid)
			{
				Needhelp = true;
				NeedhelpID = i;
			}
		}

		switch (CurrentState)
		{
		case sDefault:
			if (haveAddictedStudent)
				CurrentState = sRousing;
			else if (Needhelp)
				CurrentState = sEncouraging;
			else if (haveTricker)
				CurrentState = sInspiring;
			else
				CurrentState = sDoClassroom;
			break;
		case sRousing:
			if (!haveAddictedStudent)
				CurrentState = sDefault;
			break;
		case sEncouraging:
			if (haveAddictedStudent)
				CurrentState = sRousing;
			else if (!Needhelp && !haveAddictedStudent)
				CurrentState = sDefault;
			break;
		case sInspiring:
			if (haveAddictedStudent)
				CurrentState = sRousing;
			else if (Needhelp)
				CurrentState = sEncouraging;
			else
				CurrentState = sDefault;
			break;
		case sDoClassroom:
			if (haveAddictedStudent)
				CurrentState = sRousing;
			else if (Needhelp)
				CurrentState = sEncouraging;
			else if (haveTricker)
				CurrentState = sInspiring;
			break;
		}
		switch (CurrentState)
		{
		case sDefault:
			std::cerr << "CurrentState: sDefault" << std::endl;
			Center.Gugu.sendRescue(2, false);
			break;
		case sRousing:
			std::cerr << "CurrentState: sRousing" << std::endl;
			ChaseIt = true;
			ChaseDest = Cell(stuinfo[AddictedId]->x, stuinfo[AddictedId]->y);
			Center.Gugu.sendRescue(2, true);
			if (!Center.SunshineRouseCD())
			{
				if (Center.Alice.IsViewable(Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000), Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), api.GetSelfInfo()->viewRange))
				{
					Center.SunshineRouse();
				}
				else
				{
					Center.MoveTo(Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), true);
				}
			}
			else
			{
				if (Center.NearCell(Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), 2))
				{
					api.StartRouseMate(stuinfo[AddictedId]->playerID);
				}
				else
				{
					Center.MoveTo(Cell(stuinfo[AddictedId]->x / 1000, stuinfo[AddictedId]->y / 1000), true);
				}
			}
			break;
		case sEncouraging:
			std::cerr << "CurrentState: sEncouraging" << std::endl;

			ChaseIt = true;
			ChaseDest = Cell(stuinfo[NeedhelpID]->x, stuinfo[NeedhelpID]->y);
			Center.Gugu.sendRescue(2, true);
			if (!Center.SunshineEncourageCD())
			{
				if (Center.Alice.IsViewable(Cell(api.GetSelfInfo()->x / 1000, api.GetSelfInfo()->y / 1000), Cell(stuinfo[NeedhelpID]->x / 1000, stuinfo[NeedhelpID]->y / 1000), api.GetSelfInfo()->viewRange))
				{
					Center.SunshineEncourage();
				}
				else
				{
					Center.MoveTo(Cell(stuinfo[NeedhelpID]->x / 1000, stuinfo[NeedhelpID]->y / 1000), true);
				}
			}
			else
			{
				if (Center.NearCell(Cell(stuinfo[NeedhelpID]->x / 1000, stuinfo[NeedhelpID]->y / 1000), 2))
				{
					api.StartEncourageMate(stuinfo[NeedhelpID]->playerID);
				}
				else
				{
					Center.MoveTo(Cell(stuinfo[NeedhelpID]->x / 1000, stuinfo[NeedhelpID]->y / 1000), true);
				}
			}
			break;
		case sInspiring:
			std::cerr << "CurrentState: sinspiring" << std::endl;
			if (!Center.SunshineInspireCD())
				Center.SunshineInspire();
			//			std::cerr << "IMHERE";
			if (!triinfo.empty())
			{
				//				std::cerr << "IMHERE1";
				Center.DirectHide(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), triinfo[0]->viewRange, Center.NearCell(Grid(triinfo[0]->x, triinfo[0]->y).ToCell(), 4) ? 1 : 0);
			}
			else
			{
				//				std::cerr << "IMHERE2";
				Center.DirectGrass(1);
			}
			break;
			//if ((api.GetSelfInfo()->x - stuinfo[1]->x) * (api.GetSelfInfo()->x - stuinfo[1]->x) + (api.GetSelfInfo()->y - stuinfo[1]->y) * (api.GetSelfInfo()->y - stuinfo[1]->y) < 25000000)
			//{
			//	Center.MoveTo(Cell(stuinfo[1]->x / 1000, stuinfo[1]->y / 1000), true);
			//}
			//else if ((api.GetSelfInfo()->x - stuinfo[2]->x) * (api.GetSelfInfo()->x - stuinfo[2]->x) + (api.GetSelfInfo()->y - stuinfo[2]->y) * (api.GetSelfInfo()->y - stuinfo[2]->y) < 25000000)
			//{
			//	Center.MoveTo(Cell(stuinfo[2]->x / 1000, stuinfo[2]->y / 1000), true);
			//}
			//else
			//{
			//	for (int i = 0; i < 10; i++)
			//		if (!visitClassroom[i])
			//		{
			//			Center.MoveTo(Center.Classroom[i], 1);
			//			break;
			//		}
			//}
			//break;
		case sDoClassroom:
			std::cerr << "CurrentState: sDoClassroom" << std::endl;//常规状态下，sunshine随学霸（playerID=1）运动
			Center.Gugu.sendRescue(2, false);
			if ((api.GetSelfInfo()->x - stuinfo[1]->x) * (api.GetSelfInfo()->x - stuinfo[1]->x) + (api.GetSelfInfo()->y - stuinfo[1]->y) * (api.GetSelfInfo()->y - stuinfo[1]->y) >= 25000000)
			{
				Center.MoveTo(Cell(stuinfo[1]->x / 1000, stuinfo[1]->y / 1000), true);
			}
			if (Center.CountFinishedClassroom() >= 7)
			{
				if (!Center.CountOpenGate())
				{
					// api.EndAllAction();
					Center.DirectOpeningGate(true, true);
				}
				else
				{
					// api.EndAllAction();
					Center.DirectGraduate(true);
				}
			}
			else
			{
				// api.EndAllAction();
				Center.DirectLearning(true);
			}
			break;
			// 玩家3执行操作


		}
	}
	// 当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
	//  公共操作
}
#endif

void AI::play(ITrickerAPI& api)
{
	/*	Encoder enc;
		enc.PushInfo((int)12345);
		enc.PushInfo((int)67890);
		std::string s = enc.ToString();
		Decoder dec(s);
		std::cerr << dec.ReadInfo<int>() << ' ' << dec.ReadInfo<int>() << std::endl;*/
		//	int cnt1 = 0;
		//	for (int i = 0; i < 10000000; i++)
		//	{
		//		cnt1 += Intersect(GeometrySegment(GeometryCell(rand(), rand()), GeometryCell(rand(), rand())), GeometrySegment(GeometryCell(rand(), rand()), GeometryCell(rand(), rand())));
		//	}
		//	std::cerr << cnt1 << std::endl;
		//	return;
	auto self = api.GetSelfInfo();
	api.PrintSelfInfo();

	static std::vector<unsigned char> Priority = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	static CommandPostTricker Center(api);
	static int CurrentState = sDefault;

	Cell TrickerTemp;

	//	Center.MoveTo(Cell(41, 9), true);
	//	return;
	Center.AutoUpdate();
	//	int ccnt = api.GetFrameCount();
	//	Cell find2 = Center.Bob.Recommend((ccnt/400)%4).first;
	//	ccnt++;
	//	Center.Bob._display((ccnt / 400) % 4);
	//	std::cerr << "FINDING " << ((ccnt / 400) % 4);
	//	Center.MoveTo(find2, true);
	//	return;

	auto stuinfo = api.GetStudents();

	static bool visitClassroom[10];
	static bool visitClassroomUpdated[10];
	static int countVisitedClassroom = 0;
	static int countAttackGrass = 0;
	static long long beginAttackGrass = -1;
	static bool attackingGrass = 0;
	static bool daijoubu = 0;
	std::cerr << "[visitClassroom]";
	for (int i = 0; i < 10; i++)std::cerr << visitClassroom[i];
	std::cerr << std::endl;
	std::cerr << "[visitClassroomUpdated]";
	for (int i = 0; i < 10; i++)std::cerr << visitClassroom[i];
	std::cerr << std::endl;
	std::cerr << "[countVisitedClassroom]" << countVisitedClassroom << std::endl;
	bool haveNonAddictedStudent = false;
	int nonAddictedId = -1;
	for (int i = 0; i < stuinfo.size(); i++)
	{
		if (stuinfo[i]->playerState != THUAI6::PlayerState::Addicted)
		{
			haveNonAddictedStudent = true;
			nonAddictedId = i;
		}
	}
	static bool ChaseIt = false;
	static int ChaseID = -1;

	for (int i = 0; i < 10; i++)
		if (Center.Alice.IsViewable(Cell(self->x / 1000, self->y / 1000), Cell(Center.Classroom[i].x, Center.Classroom[i].y), self->viewRange))
		{
			visitClassroom[i] = true;
			// countVisitedClassroom++;
		}
	for (int i = 0; i < 10; i++)
	{
		if (visitClassroom[i] && !visitClassroomUpdated[i])
		{
			countVisitedClassroom++;
			visitClassroomUpdated[i] = true;
		}
	}
	if (countVisitedClassroom == 10)
	{
		for (int i = 0; i < 10; i++)
		{
			visitClassroom[i] = false;
			visitClassroomUpdated[i] = false;
		}
		countVisitedClassroom = 0;
	}

	std::cerr << "UseSkillBegin" << std::endl;
	std::cerr << "UseSkillEnd" << std::endl;

	std::cerr << "[beginAttackGrass]" << beginAttackGrass << std::endl;
	std::cerr << "[attackingGrass]" << attackingGrass << std::endl;
	std::cerr << "[GetFrameCount]" << api.GetFrameCount() << std::endl;

	switch (CurrentState)
	{
	case sDefault:
		if (haveNonAddictedStudent)
			CurrentState = sAttackPlayer;
		else if (ChaseIt)
			CurrentState = sChasePlayer;
		else
			CurrentState = sFindPlayer;
		break;
	case sFindPlayer:
		if (haveNonAddictedStudent)
			CurrentState = sAttackPlayer;
		break;
	case sAttackPlayer:
		if (!haveNonAddictedStudent)
			CurrentState = sChasePlayer;
		break;
	case sChasePlayer:
		if (haveNonAddictedStudent)
			CurrentState = sAttackPlayer;
		//		else if (Center.NearCell(ChaseDest.ToCell(), 2))
		else
		{
			for (auto stu : api.GetStudents())
			{
				if (stu->playerID == ChaseID && stu->playerState == THUAI6::PlayerState::Addicted)
				{
					ChaseIt = false;
					ChaseID = -1;
					CurrentState = sDefault;
					break;
				}
			}
		}
		break;
	}

	if (CurrentState != sChasePlayer)
	{
		daijoubu = 0;
		attackingGrass = 0;
	}

	switch (CurrentState)
	{
	case sDefault:
		std::cerr << "CurrentState: sDefault" << std::endl;
		break;
	case sFindPlayer:
		std::cerr << "CurrentState: sFindPlayer" << std::endl;
		/*
				for (int i = 0; i < 10; i++)
					if (Center.Alice.IsViewable(Cell(self->x / 1000, self->y / 1000), Cell(Center.Classroom[i].x, Center.Classroom[i].y), self->viewRange))
					{
						visitClassroom[i] = true;
						// countVisitedClassroom++;
					}
				for (int i = 0; i < 10; i++)
				{
					if (visitClassroom[i] && !visitClassroomUpdated[i])
					{
						countVisitedClassroom++;
						visitClassroomUpdated[i] = true;
					}
				}
				if (countVisitedClassroom == 10)
				{
					for (int i = 0; i < 10; i++)
					{
						visitClassroom[i] = false;
						visitClassroomUpdated[i] = false;
					}
					countVisitedClassroom = 0;
				}
				{
					int minDistance = INT_MAX;
					int minNum = -1;
					int Distance = INT_MAX;
					Cell Self(self->x / 1000, self->y / 1000);
					if (!Center.Classroom.empty())
					{
						for (int i = 0; i < Center.Classroom.size(); i++)
						{
							//			if (API.GetClassroomProgress(Classroom[i].x, Classroom[i].y) < 10000000)
							if (Center.GetClassroomProgress(Center.Classroom[i].x, Center.Classroom[i].y) < 10000000)
							{
								try
								{
									Distance = Center.Alice.AStar(Self, Center.Classroom[i], 1).size();
									if (Distance < minDistance && Distance != 0 && !visitClassroom[i])
									{
										minDistance = Distance;
										minNum = i;
									}
								}
								catch (const noway_exception& e)
								{
									std::cerr << "[noway_exception]Noway." << std::endl;
								}
							}
						}
					}
					if (minNum >= 0)
					{
						Center.MoveTo(Center.Classroom[minNum], 1);
						break;
					}
				}*/
		Center.MoveTo(Center.Bob.SmartRecommend(), 1);
		break;
	case sAttackPlayer:
		std::cerr << "CurrentState: sAttackPlayer" << std::endl;
		std::cerr << "See student " << stuinfo.size() << std::endl;
		std::cerr << "Decide to attack " << stuinfo[nonAddictedId]->playerID << std::endl;
		if (!Center.KleeSparksNSplashCD()) Center.KleeSparksNSplash(stuinfo[nonAddictedId]->playerID);
		ChaseIt = true;
		//		ChaseDest = Grid(stuinfo[nonAddictedId]->x, stuinfo[nonAddictedId]->y);
		ChaseID = stuinfo[nonAddictedId]->playerID;
		if (abs(api.GetSelfInfo()->x - stuinfo[nonAddictedId]->x) + abs(api.GetSelfInfo()->y - stuinfo[nonAddictedId]->y) < 2000)
		{
			if (!Center.KleeJumpyBombCD()) Center.KleeJumpyBomb();
			Center.KleeDefaultAttack(stuinfo[nonAddictedId]->x, stuinfo[nonAddictedId]->y);
			//			api.Attack(atan2(-self->y + stuinfo[0]->y, -self->x + stuinfo[0]->x));
		}
		else
		{
			// api.EndAllAction();
			Center.MoveTo(Cell(stuinfo[nonAddictedId]->x / 1000, stuinfo[nonAddictedId]->y / 1000), 1);
		}
		break;
	case sChasePlayer:
		std::cerr << "CurrentState: sChasePlayer" << std::endl;
		//		Center.MoveTo(ChaseDest.ToCell(), true);
		if (self->trickDesire >= 8 && Center.IsStuck && stuinfo.empty())
		{
			if (!attackingGrass)
			{
				beginAttackGrass = api.GetFrameCount();
				attackingGrass = 1;
			}
			if (api.GetFrameCount() - beginAttackGrass <= 250)
			{
				Center.KleeDefaultAttack(Center.LastStuckDestinationGrid.x, Center.LastStuckDestinationGrid.y);
			}
			else
			{
				Center.MoveToNearestClassroom(1);
				daijoubu = 1;
			}
		}
		else if (daijoubu)
		{
			Center.MoveToNearestClassroom(1);
			if (Center.NearClassroom(0))
			{
				daijoubu = 0;
				attackingGrass = 0;
			}
		}
		else
		{
			Center.MoveTo(Center.Bob.SmartRecommend(), 1);
		}
		break;
	}
}
