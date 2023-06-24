#include <array>
#include <cmath>
#include <queue>
#include <sstream>
#include <stack>
#include <thread>
#include <utility>
#include <vector>
#include "AI.h"
#include "constants.h"
#define PI 3.14159265358979323846

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义
extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent
};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数
struct CellPos {
	int x, y;
};
struct GridPos {
	int x, y;
};

class Action;
class BFS;
class Communication;
class Escape;
class GameState;

class Action {
public:
	enum class Direction {
		null, up, down, left, right,
		up_left, up_right, down_left, down_right
	};															//移动方式
	static const std::vector <Direction> allMove;				//所有的移动方式
	static int isAtCellFalseCnt;
	static int classNo;											//要去的教室位置
	static CellPos lastNextPos;
	static CellPos lastDesPos;
	static Direction lastMove;
	static CellPos PredictPos;

	static bool isValidMove(CellPos judgePos, Direction comeDir, GameState state, IStudentAPI& api);
	static bool isValidMove(CellPos judgePos, Direction comeDir, GameState state, ITrickerAPI& api);
	static double getAngle(GridPos grid1, GridPos grid2);													//计算两点间方位角
	static void Move(CellPos des, IStudentAPI& api);
	static void Move(CellPos des, ITrickerAPI& api);
	static void Move(Direction directon, IStudentAPI& api);
	static void Move(Direction direction, ITrickerAPI& api);
	static CellPos directionToPos(CellPos nowPos, Direction direction);										//根据当前坐标和移动方式计算出下一个坐标
	static Direction posToDirection(CellPos nowPos, CellPos nextPos);										//目标点的坐标转化为移动方式
};
class BFS {
public:
	static bool vis[Constants::rows][Constants::cols];
	static CellPos now;
	static CellPos nxt;
	static CellPos pre[Constants::rows][Constants::cols];

	static std::pair<CellPos, int> bfs(CellPos start, CellPos destination, ITrickerAPI& api);
	static std::pair<std::pair<CellPos, CellPos>, int> bfs(CellPos start, THUAI6::PlaceType destination, ITrickerAPI& api);		//return pair<next_point,destination>
};
class Communication {
public:
	static void receiveMessage(IStudentAPI& api, int64_t myID);
	static void sendClassroomPos(IStudentAPI& api, int64_t myID, CellPos classroomPos);
	static void sendTrickerPos(IStudentAPI& api, int64_t myID);
	/*
		通信协议：AA BB CC DD
		（AA, BB)为学完的教室的坐标
		（CC, DD)为看到的捣蛋鬼的坐标
		若AA, BB均为-1，则忽略教室坐标，只读捣蛋鬼坐标
		若CC, DD均为-1，则忽略捣蛋鬼坐标，只读教室坐标
	*/
};
class Escape {
public:
	//static std::pair<CellPos, Action::Direction> minimax(CellPos start, THUAI6::PlaceType destination);   //规避算法
};
class GameState {
public:
	static int readCnt;
	static int clsFinishCnt;
	THUAI6::PlaceType gameMap[Constants::rows][Constants::cols];				//存图信息
	std::vector <CellPos> ClassRoom;
	std::vector <CellPos> Gate;
	std::vector <CellPos> HiddenGate;
	std::vector <CellPos> Window;
	std::vector <CellPos> Door3;
	std::vector <CellPos> Door5;
	std::vector <CellPos> Door6;
	std::vector <CellPos> Chest;
	std::vector <std::pair<GridPos,int>> ValidMoveBuffer;
	CellPos studentPos[4];									//四个学生的坐标
	GridPos studentGrid[4];
	CellPos trickerPos;										//捣蛋鬼的坐标

	bool isAtCell(GridPos myPos, GridPos cellPos) const;										//判断人物是否完全在格点内
	bool isAtPosition(CellPos myPos, CellPos pos) const;
	bool isAtPos(CellPos myPos, CellPos pos) const;		//判断character坐标与pos坐标是否在一个九宫格内
	bool isAttackable(GridPos trickerPos, GridPos studentPos) const;							//判断是否可以攻击
	bool isClassroomFinished(CellPos classroomPos, IStudentAPI& studentapi) const;				//判断教室是否学完了
	bool isDoor(CellPos pos) const;
	bool isGateFinished(CellPos gatePos, IStudentAPI& api) const;								//判断大门是否开了
	bool isTrickerVisible() const;																//判断tricker是否可见
	bool isType(CellPos pos, THUAI6::PlaceType type) const;
	bool canReach(CellPos pos, ITrickerAPI& api) const;
	bool readyToGraduate() const;																//判断当前局面是否可毕业
	double gridDistance(GridPos grid1, GridPos grid2) const;									//计算两点间以grid计的距离
	void deleteStudiedClassroom(CellPos classroomPos);											//把学完的教室删除，变成墙
	void getMap(IStudentAPI& api);																//获取地图格点信息
	void getMap(ITrickerAPI& api);																//获取地图格点信息
	std::pair<std::pair<bool, std::pair<CellPos, GridPos>>, std::pair<double, double>> getClosestStudent(GridPos trickerPos, ITrickerAPI& api);		//找到视野中最近的学生
	//CellPos isGrassAround(CellPos, IStudentAPI& api) const;
};

const std::vector <Action::Direction> Action::allMove = {
	Direction::up,
	Direction::down,
	Direction::left,
	Direction::right,
	Direction::up_left,
	Direction::up_right,
	Direction::down_left,
	Direction::down_right
};
int Action::isAtCellFalseCnt = 0;
int Action::classNo = 0;
CellPos Action::lastNextPos = { -1,-1 };
Action::Direction Action::lastMove = Direction::null;
CellPos Action::lastDesPos = { -1,-1 };
bool BFS::vis[Constants::rows][Constants::cols] = { false };
CellPos BFS::now = { -1,-1 };
CellPos BFS::nxt = { -1,-1 };
CellPos BFS::pre[Constants::rows][Constants::cols] = { {-1,-1} };

int GameState::readCnt = 0;
int GameState::clsFinishCnt = 0;
GameState gameState;

bool Action::isValidMove(CellPos judgePos, Direction comeDir, GameState state, IStudentAPI& api)
{
	auto students = api.GetStudents();

	for (auto it = students.begin(); it != students.end(); it++)
	{

		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
	}
	auto trickers = api.GetTrickers();
	for (auto it = trickers.begin(); it != trickers.end(); it++)
	{
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
	}
	switch (comeDir)
	{
	case Action::Direction::up_left: {
		if (state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Grass)return false;
		if (state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Grass)return false;
		break;
	}
	case Action::Direction::up_right: {
		if (state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Grass)return false;
		if (state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Grass)return false;
		break;
	}
	case Action::Direction::down_right: {
		if (state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Grass)return false;
		if (state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Grass)return false;
		break;
	}
	case Action::Direction::down_left: {
		if (state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Grass)return false;
		if (state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Land && state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Grass)return false;
		break;
	}
	default:break;
	}
	return state.gameMap[judgePos.x][judgePos.y] == THUAI6::PlaceType::Land
		|| state.gameMap[judgePos.x][judgePos.y] == THUAI6::PlaceType::Grass
		|| state.gameMap[judgePos.x][judgePos.y] == THUAI6::PlaceType::Window
		|| (state.isDoor(judgePos) && api.IsDoorOpen(judgePos.x, judgePos.y));
}
bool Action::isValidMove(CellPos judgePos, Direction comeDir, GameState state, ITrickerAPI& api)
{
	for (int i=0; i<gameState.ValidMoveBuffer.size(); i++) {
		auto it = &gameState.ValidMoveBuffer[i].first;
		if (api.HaveView(it->x, it->y)|| (api.GetFrameCount()- gameState.ValidMoveBuffer[i].second)>=100) {
			gameState.ValidMoveBuffer.erase(gameState.ValidMoveBuffer.begin()+i);
			i--;
		}
		else {
			state.gameMap[api.GridToCell(it->x)][api.GridToCell(it->y)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x + 390)][api.GridToCell(it->y)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x - 390)][api.GridToCell(it->y)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x)][api.GridToCell(it->y + 390)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x)][api.GridToCell(it->y - 390)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x + 280)][api.GridToCell(it->y + 280)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x + 280)][api.GridToCell(it->y - 280)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x - 280)][api.GridToCell(it->y + 280)] = THUAI6::PlaceType::Wall;
			state.gameMap[api.GridToCell(it->x - 280)][api.GridToCell(it->y - 280)] = THUAI6::PlaceType::Wall;
		}
	}
	auto students = api.GetStudents();
	for (auto it = students.begin(); it != students.end(); it++)
	{
		if ((*it)->determination>0)
			continue;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 390)][api.GridToCell((*it)->y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x)][api.GridToCell((*it)->y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x + 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*it)->x - 280)][api.GridToCell((*it)->y - 280)] = THUAI6::PlaceType::Wall;
		gameState.ValidMoveBuffer.push_back({ { (*it)->x ,(*it)->y },api.GetFrameCount()});
	}
	switch (comeDir)
	{
	case Action::Direction::up_left: {

		if (state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Land )return false;
		if (state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Land )return false;
		break;
	}
	case Action::Direction::up_right: {

		if (state.gameMap[judgePos.x + 1][judgePos.y] != THUAI6::PlaceType::Land )return false;
		if (state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Land )return false;
		break;
	}
	case Action::Direction::down_right: {

		if (state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Land )return false;
		if (state.gameMap[judgePos.x][judgePos.y - 1] != THUAI6::PlaceType::Land )return false;
		break;
	}
	case Action::Direction::down_left: {

		if (state.gameMap[judgePos.x - 1][judgePos.y] != THUAI6::PlaceType::Land )return false;
		if (state.gameMap[judgePos.x][judgePos.y + 1] != THUAI6::PlaceType::Land )return false;
		break;
	}
	default:break;
	}
	return state.canReach(judgePos, api);
}
double Action::getAngle(GridPos grid1, GridPos grid2)
{
	return atan2(grid2.y - grid1.y, grid2.x - grid1.x);
}
void Action::Move(CellPos destination, IStudentAPI& api)
{
	int desX = api.CellToGrid(destination.x);
	int desY = api.CellToGrid(destination.y);
	auto self = api.GetSelfInfo();
	api.Move(50, getAngle({ self->x,self->y }, { desX,desY }));
}
void Action::Move(CellPos destination, ITrickerAPI& api)
{
	int desX = api.CellToGrid(destination.x);
	int desY = api.CellToGrid(destination.y);
	auto self = api.GetSelfInfo();
	api.Move(50, getAngle({ self->x,self->y }, { desX,desY }));
}

void Action::Move(Direction direction, IStudentAPI& api)
{
	auto self = api.GetSelfInfo();
	int x = api.GridToCell(self->x);
	int y = api.GridToCell(self->y);
	Move(Action::directionToPos({ x, y }, direction), api);
}
void Action::Move(Direction direction, ITrickerAPI& api)
{
	auto self = api.GetSelfInfo();
	int x = api.GridToCell(self->x);
	int y = api.GridToCell(self->y);
	Move(Action::directionToPos({ x, y }, direction), api);
}
CellPos Action::directionToPos(CellPos nowPos, Direction direction)
{
	switch (direction)
	{
	case Action::Direction::up:
		return { nowPos.x - 1, nowPos.y };
	case Action::Direction::down:
		return { nowPos.x + 1, nowPos.y };
	case Action::Direction::left:
		return { nowPos.x, nowPos.y - 1 };
	case Action::Direction::right:
		return { nowPos.x, nowPos.y + 1 };
	case Action::Direction::up_left:
		return { nowPos.x - 1, nowPos.y - 1 };
	case Action::Direction::up_right:
		return { nowPos.x - 1, nowPos.y + 1 };
	case Action::Direction::down_left:
		return { nowPos.x + 1, nowPos.y - 1 };
	case Action::Direction::down_right:
		return { nowPos.x + 1, nowPos.y + 1 };
	}
	return { -1,-1 };
}
Action::Direction Action::posToDirection(CellPos nowPos, CellPos nextPos)
{
	if (nextPos.x == nowPos.x + 1 && nextPos.y == nowPos.y)
		return Action::Direction::down;
	if (nextPos.x == nowPos.x - 1 && nextPos.y == nowPos.y)
		return Action::Direction::up;
	if (nextPos.x == nowPos.x && nextPos.y == nowPos.y + 1)
		return Action::Direction::right;
	if (nextPos.x == nowPos.x && nextPos.y == nowPos.y - 1)
		return Action::Direction::left;
	if (nextPos.x == nowPos.x + 1 && nextPos.y == nowPos.y + 1)
		return Action::Direction::down_right;
	if (nextPos.x == nowPos.x - 1 && nextPos.y == nowPos.y + 1)
		return Action::Direction::up_right;
	if (nextPos.x == nowPos.x + 1 && nextPos.y == nowPos.y - 1)
		return Action::Direction::down_left;
	if (nextPos.x == nowPos.x - 1 && nextPos.y == nowPos.y - 1)
		return Action::Direction::up_left;
	return Action::Direction::null;
}

std::pair<CellPos, int> BFS::bfs(CellPos start, CellPos destination, ITrickerAPI& api)
{
	int length = 0;
	std::queue<CellPos> posQ;

	memset(vis, false, sizeof(vis));
	vis[start.x][start.y] = true;
	posQ.push(start);
	while (!posQ.empty())
	{
		now = posQ.front();
		posQ.pop();
		for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++)
		{
			nxt = Action::directionToPos(now, *move);
			if (nxt.x == destination.x && nxt.y == destination.y && !gameState.canReach(nxt, api))
			{
				pre[nxt.x][nxt.y] = now;
				now = nxt;
				break;
			}
			if (!Action::isValidMove(nxt, *move, gameState, api) || vis[nxt.x][nxt.y])
				continue;
			if (nxt.x == destination.x && nxt.y == destination.y)
			{
				pre[nxt.x][nxt.y] = now;
				now = nxt;
				break;
			}
			vis[nxt.x][nxt.y] = true;
			posQ.push(nxt);
			pre[nxt.x][nxt.y] = now;
		}
		if (now.x == nxt.x && now.y == nxt.y)
		{
			while (pre[now.x][now.y].x != start.x || pre[now.x][now.y].y != start.y)
			{
				now = pre[now.x][now.y];
				length++;
			}
			return { now, length + 1 };
		}
	}
	return { {-1,-1},-1 };
}
std::pair<std::pair<CellPos, CellPos>,int> BFS::bfs(CellPos start, THUAI6::PlaceType destination, ITrickerAPI& api)
{
	int length = 0;
	std::queue<CellPos> posQ;
	if (gameState.gameMap[start.x][start.y] == destination) {
		return { {start,start},0 };
	}
	memset(vis, false, sizeof(vis));
	vis[start.x][start.y] = true;
	posQ.push(start);
	while (!posQ.empty())
	{
		now = posQ.front();
		posQ.pop();
		for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++)
		{
			nxt = Action::directionToPos(now, *move);
			if (gameState.gameMap[nxt.x][nxt.y] == destination && !gameState.canReach(nxt, api))
			{
				pre[nxt.x][nxt.y] = now;
				now = nxt;
				break;
			}
			if (!Action::isValidMove(nxt, *move, gameState, api) || vis[nxt.x][nxt.y])
				continue;
			if (gameState.gameMap[nxt.x][nxt.y] == destination)
			{
				pre[nxt.x][nxt.y] = now;
				now = nxt;
				break;
			}
			vis[nxt.x][nxt.y] = true;
			posQ.push(nxt);
			pre[nxt.x][nxt.y] = now;
		}
		if (now.x == nxt.x && now.y == nxt.y)
		{
			while (pre[now.x][now.y].x != start.x || pre[now.x][now.y].y != start.y) {
				length++;
				now = pre[now.x][now.y];
			}				
			return { { now,nxt },length };
		}
	}
	return { { {-1,-1},{-1,-1} },1000 };
}

void Communication::sendClassroomPos(IStudentAPI& api, int64_t myID, CellPos classroomPos) {
	auto message = fmt::format("{} {} {} {}", classroomPos.x, classroomPos.y, -1, -1);
	for (int id = 0; id <= 3; id++)
		if (id != myID)
			api.SendTextMessage(id, message);
}
void Communication::sendTrickerPos(IStudentAPI& api, int64_t myID) {
	auto trickers = api.GetTrickers();
	if (trickers.empty())
		return;
	CellPos trickerPos = { api.GridToCell(trickers[0]->x), api.GridToCell(trickers[0]->y) };
	auto message = fmt::format("{} {} {} {}", -1, -1, trickerPos.x, trickerPos.y);
	for (int i = 0; i <= 3; i++)
		if (i != myID)
			api.SendTextMessage(i, message);
}
void Communication::receiveMessage(IStudentAPI& api, int64_t myID) {
	CellPos classroomPos, trickerPos;
	std::pair<int64_t, std::string> message;
	std::stringstream ss;
	gameState.trickerPos = { -1, -1 };
	auto trickers = api.GetTrickers();
	if (!trickers.empty())
		gameState.trickerPos = { api.GridToCell(trickers[0]->x), api.GridToCell(trickers[0]->y) };
	while (api.HaveMessage()) {
		ss.clear();
		message = api.GetMessage();
		ss.str(message.second);
		ss >> classroomPos.x >> classroomPos.y >> trickerPos.x >> trickerPos.y;
		if (classroomPos.x != -1 && classroomPos.y != -1)
			gameState.deleteStudiedClassroom(classroomPos);
		if (trickerPos.x != -1 && trickerPos.y != -1)
			gameState.trickerPos = trickerPos;
	}
}

inline bool GameState::isAtCell(GridPos myPos, GridPos cellPos) const {
	return abs(myPos.x - cellPos.x) < 100 && abs(myPos.y - cellPos.y) < 100;
}
inline bool GameState::isAtPosition(CellPos character, CellPos pos) const {
	return (abs(character.x - pos.x) <= 1 && abs(character.y - pos.y) <= 1);
}
inline bool GameState::isAtPos(CellPos character, CellPos pos) const {
	return (abs(character.x - pos.x) <= 1 && abs(character.y - pos.y) == 0) || (abs(character.y - pos.y) <= 1 && abs(character.x - pos.x) == 0);
}
inline bool GameState::isAttackable(GridPos trickerPos, GridPos studentPos) const {
	return gridDistance(trickerPos, studentPos) <= 1500;
}
inline bool GameState::isClassroomFinished(CellPos classroomPos, IStudentAPI& api) const {
	return api.GetClassroomProgress(classroomPos.x, classroomPos.y) >= Constants::maxClassroomProgress;
}
inline bool GameState::isDoor(CellPos pos) const {
	return gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door3
		|| gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door5
		|| gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door6;
}
inline bool GameState::isGateFinished(CellPos gatePos, IStudentAPI& api) const {
	return api.GetGateProgress(gatePos.x, gatePos.y) >= Constants::maxGateProgress;
}
inline bool GameState::isTrickerVisible() const {
	return trickerPos.x == -1 && trickerPos.y == -1;
}
inline bool GameState::isType(CellPos pos, THUAI6::PlaceType type) const {
	return gameMap[pos.x][pos.y] == type;
}
inline bool GameState::canReach(CellPos pos, ITrickerAPI& api) const {
	return gameMap[pos.x][pos.y] == THUAI6::PlaceType::Window
		|| gameMap[pos.x][pos.y] == THUAI6::PlaceType::Grass
		|| gameMap[pos.x][pos.y] == THUAI6::PlaceType::Land
		|| (isDoor(pos) && api.IsDoorOpen(pos.x, pos.y));
}
inline bool GameState::readyToGraduate() const {
	return clsFinishCnt >= Constants::numOfRequiredClassroomForGate;
}
double GameState::gridDistance(GridPos grid1, GridPos grid2) const {
	return sqrt(pow((grid1.x - grid2.x), 2) + pow((grid1.y - grid2.y), 2));
}
void GameState::deleteStudiedClassroom(CellPos classroomPos) {
	for (auto it = gameState.ClassRoom.begin(); it != gameState.ClassRoom.end(); it++)
		if (it->x == classroomPos.x && it->y == classroomPos.y)
		{
			ClassRoom.erase(it);
			gameState.gameMap[classroomPos.x][classroomPos.y] = THUAI6::PlaceType::Wall;
			clsFinishCnt++;
			break;
		}
}
void GameState::getMap(IStudentAPI& api) {
	for (int i = 0; i < Constants::rows; i++) {
		for (int j = 0; j < Constants::cols; j++) {
			gameMap[i][j] = (api.GetFullMap()[i][j]);
			if (gameMap[i][j] == THUAI6::PlaceType::ClassRoom) ClassRoom.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Gate) Gate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::HiddenGate) HiddenGate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Window) Window.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Chest) Chest.push_back({ i, j });
		}
	}
}
void GameState::getMap(ITrickerAPI& api) {
	for (int i = 0; i < Constants::rows; i++) {
		for (int j = 0; j < Constants::cols; j++) {
			gameMap[i][j] = (api.GetFullMap()[i][j]);
			if (gameMap[i][j] == THUAI6::PlaceType::ClassRoom) ClassRoom.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Gate) Gate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::HiddenGate) HiddenGate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Window) Window.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Chest) Chest.push_back({ i, j });
		}
	}
}
std::pair<std::pair<bool, std::pair<CellPos,GridPos>>, std::pair<double, double>> GameState::getClosestStudent(GridPos trickerPos, ITrickerAPI& api)
{
	auto students = api.GetStudents();
	double minDistance = 1e5;
	CellPos studentPos = { -1,-1 };
	GridPos studentGrid = { -1,-1 };
	bool updated[4] = { false };
	bool inSight = false;
	double angle[5] = { -1,-1,-1,-1,-1 };
	int picked = 4;
	for (auto it = students.begin(); it != students.end(); it++)
	{
		if ((*it)->determination<=0) {
			gameState.studentPos[(*it)->playerID].x = -1;
			gameState.studentPos[(*it)->playerID].y = -1;
			gameState.studentGrid[(*it)->playerID].x = -1;
			gameState.studentGrid[(*it)->playerID].y = -1;
			continue;
		}
		angle[(*it)->playerID] = (*it)->facingDirection;
		gameState.studentPos[(*it)->playerID].x = api.GridToCell((*it)->x);
		gameState.studentPos[(*it)->playerID].y = api.GridToCell((*it)->y);
		gameState.studentGrid[(*it)->playerID].x = (*it)->x;
		gameState.studentGrid[(*it)->playerID].y = (*it)->y;
		updated[(*it)->playerID] = true;
	}
	for (int i = 0; i < 4; i++) {
		if (minDistance > gridDistance(trickerPos, { api.CellToGrid(gameState.studentPos[i].x),api.CellToGrid(gameState.studentPos[i].y) }) && gameState.studentPos[i].x >= 0)
		{
			minDistance = gridDistance(trickerPos, { api.CellToGrid(gameState.studentPos[i].x),api.CellToGrid(gameState.studentPos[i].y) });
			studentPos = gameState.studentPos[i];
			studentGrid = gameState.studentGrid[i];
			inSight = updated[i];
			picked = i;
		}
	}
	
	return { {inSight,{studentPos,studentGrid}},{minDistance,angle[picked]} };
}

void AI::play(IStudentAPI& api) {}
int AttackCount = 0;
int needNew = 0;
bool del=false;
GridPos lastGrid = { -1,-1 };
int stuck = 0;
int classcount = 0;
int pointing = -1;
//bool attackToGrass = false;
std::vector <CellPos> traversal;
std::vector <CellPos> gates;
void AI::play(ITrickerAPI& api)
{
	if (gameState.readCnt < 1)
	{
		gameState.getMap(api);
		gameState.readCnt++;
		for (int i = 0; i < 4; i++) {
			//studentPos赋初值，代表视野没有，也没记住
			gameState.studentPos[i] = { -1,-1 };
			gameState.studentGrid[i] = { -1,-1 };
		}
		if (gameState.gameMap[5][9] == THUAI6::PlaceType::Grass) {
			traversal.push_back({ 6,24 });
			traversal.push_back({ 8,31 });//class
			traversal.push_back({ 10,38 });//class
			traversal.push_back({ 12,46 });
			traversal.push_back({ 19,41 });//class
			//traversal.push_back({ 18,46 });
			//traversal.push_back({ 23,40 });
			traversal.push_back({ 21,27 });
			traversal.push_back({ 28,26 });//class
			traversal.push_back({ 33,40 });//class
			//traversal.push_back({ 39,38 });
			traversal.push_back({ 44,32 });//class
			traversal.push_back({ 40,12 });//class
			//traversal.push_back({ 46,9 });
			//traversal.push_back({ 36,7 });
			traversal.push_back({ 30,7 });//class
			traversal.push_back({ 22,18 });//class
			traversal.push_back({ 18,5 });//class
			traversal.push_back({ 8,16 });
			traversal.push_back({ 7,21 });
			gates.push_back({ 46,45 });
			gates.push_back({ 5,6 });
		}
		else if (gameState.gameMap[5][9] == THUAI6::PlaceType::Wall) {
			traversal.push_back({ 16,20 });
			traversal.push_back({ 16,16 });//class
			traversal.push_back({ 20,16 });
			traversal.push_back({ 29,16 });
			traversal.push_back({ 33,16 });//class
			traversal.push_back({ 33,20 });
			traversal.push_back({ 33,29 });
			traversal.push_back({ 33,33 });//class
			traversal.push_back({ 29,33 });
			traversal.push_back({ 20,33 });
			traversal.push_back({ 16,33 });//class
			traversal.push_back({ 16,29 });
			traversal.push_back({ 6,13 });//class
			traversal.push_back({ 6,36 });//class
			traversal.push_back({ 13,43 });//class
			traversal.push_back({ 34,45 });
			traversal.push_back({ 43,36 });//class
			traversal.push_back({ 43,13 });//class
			traversal.push_back({ 36,6 });//class
			traversal.push_back({ 15,4 });
			gates.push_back({ 47,6 });
			gates.push_back({ 2,43 });
		}
		else {
			traversal = gameState.ClassRoom;
		}
		api.Print("Map read");

	}

	if (traversal.size())
		Action::classNo = Action::classNo % traversal.size();
	else
		Action::classNo = -1;

	auto self = api.GetSelfInfo();
	
	GridPos myGrid = { self->x,self->y };

	CellPos myCell = { api.GridToCell(self->x), api.GridToCell(self->y) };
	api.Print(fmt::format("mypos {} {}", myCell.x, myCell.y));
	if (self->playerState == THUAI6::PlayerState::Swinging || self->playerState == THUAI6::PlayerState::Attacking || self->playerState == THUAI6::PlayerState::Stunned) {
		gameState.getClosestStudent(myGrid, api);
		api.Print("not idle");
		return;
	}
	if (gameState.isAtCell(myGrid, { api.CellToGrid(myCell.x),api.CellToGrid(myCell.y) }) || Action::isAtCellFalseCnt > 15)
		//if (gameState.isAtCell(myGrid, { api.CellToGrid(myCell.x),api.CellToGrid(myCell.y) }))
	{
		//在格点上或者连续10次进不到格点
		bool notAtCell = Action::isAtCellFalseCnt > 15;
		Action::isAtCellFalseCnt = 0;
		api.Print(fmt::format("notAtCell {}", notAtCell));
		if (!notAtCell) {
			if (abs(myGrid.x - lastGrid.x) <= 5 && abs(myGrid.y - lastGrid.y) <= 5) {
				stuck++;
				api.Print(fmt::format("stuck {}", stuck));
			}
			else {
				stuck = 0;
				api.Print("stuck zero");
			}
			if (stuck >= 5) {
				if (pointing <= 1) {

					auto angle = Action::getAngle({ api.CellToGrid(Action::directionToPos(myCell, Action::lastMove).x), api.CellToGrid(Action::directionToPos(myCell, Action::lastMove).y) }, { api.CellToGrid(myCell.x), api.CellToGrid(myCell.y) });
					angle = angle + pointing * PI / 4;
					api.Attack(angle);
					pointing++;
				}
				else {
					pointing = -1;
					stuck = 0;
				}
				api.Print(fmt::format("{} {} set to wall", Action::lastNextPos.x, Action::lastNextPos.y));
				stuck = 0;
				gameState.gameMap[Action::lastNextPos.x][Action::lastNextPos.y] = THUAI6::PlaceType::Wall;
				for (int i = 0; i < 4; i++) {
					if (gameState.studentPos[i].x == Action::lastNextPos.x && gameState.studentPos[i].y == Action::lastNextPos.y) {
						gameState.studentPos[i].x = -1;
						gameState.studentPos[i].y = -1;
						gameState.studentGrid[i].x = -1;
						gameState.studentGrid[i].y = -1;
					}
				}
			}
			lastGrid = myGrid;
		}
		else{
			if (gameState.gameMap[Action::lastNextPos.x][Action::lastNextPos.y] == THUAI6::PlaceType::Grass && AttackCount < 5) {

				GridPos target = { api.CellToGrid(Action::lastNextPos.x),api.CellToGrid(Action::lastNextPos.y) };
				api.Attack(Action::getAngle(myGrid, target));
				api.Print(fmt::format("attacknum {}", AttackCount));
				api.Print(fmt::format("attackto {} {}", Action::lastNextPos.x, Action::lastNextPos.y));
				AttackCount++;
			}
			else {
				AttackCount = 0;
				del = true;
				//Action::lastNextPos = Action::directionToPos(Action::lastNextPos, Action::lastMove);
				api.Print(fmt::format("delete {} {}", Action::lastNextPos.x, Action::lastNextPos.y));
				gameState.gameMap[Action::lastNextPos.x][Action::lastNextPos.y] = THUAI6::PlaceType::Wall;
				for (int i = 0; i < 4; i++) {
					if (gameState.studentPos[i].x == Action::lastNextPos.x && gameState.studentPos[i].y == Action::lastNextPos.y) {
						gameState.studentPos[i].x = -1;
						gameState.studentPos[i].y = -1;
						gameState.studentGrid[i].x = -1;
						gameState.studentGrid[i].y = -1;
					}
				}
			}

			Action::lastNextPos = Action::directionToPos(Action::lastNextPos, Action::lastMove);
			return;
		}

		auto closestStudent = gameState.getClosestStudent(myGrid, api);
		bool inSight = closestStudent.first.first;
		auto stuCellPos = closestStudent.first.second.first;
		GridPos stuGridPos = closestStudent.first.second.second;
		auto distance = closestStudent.second.first;
		api.Print(fmt::format("insight {} stupos {} {} distance {}", inSight, stuCellPos.x, stuCellPos.y, distance));
		if (distance < Constants::basicTrickerViewRange)//如果视野里有学生，且未沉迷，则判断攻击条件，不满足则追击学生
		{
			Action::lastDesPos = { stuCellPos.x,stuCellPos.y };
			bool isClose;
			if (inSight) {
				isClose = gameState.isAtPos(myCell, stuCellPos);
				api.UseSkill(0);
				//if ((!api.GetSelfInfo()->buff.empty())&&api.GetSelfInfo()->buff[0]==THUAI6::TrickerBuffType::Invisible) 
					//api.Print("invisible");
				api.UseSkill(1);
				if (api.GetSelfInfo()->bulletType == THUAI6::BulletType::FlyingKnife) {
					//api.Print("knife");
					api.Attack(Action::getAngle(myGrid, stuGridPos));
				}

			}
			else {

				isClose = gameState.isAtCell(myGrid, {api.CellToGrid(stuCellPos.x),api.CellToGrid(stuCellPos.y)}) || (gameState.isAtPosition(myCell, stuCellPos) && notAtCell);
			}
			api.Print(fmt::format("isclose {}", isClose));
			if (isClose) {

				if (inSight) {
					api.Attack(Action::getAngle(myGrid, stuGridPos));

				}
				/*
				else if (notAtCell && AttackCount < 3) {
					api.Attack(Action::getAngle(myGrid, stuGridPos));
					AttackCount++;
				}

				else if (del) {
					//AttackCount = 0;
					for (int i = 0; i < 4; i++) {
						if (gameState.studentPos[i].x == stuCellPos.x && gameState.studentPos[i].y == stuCellPos.y) {
							gameState.studentPos[i].x = -1;
							gameState.studentPos[i].y = -1;
						}
					}
					del = false;
				}*/
				else {

					
					if (gameState.gameMap[myCell.x][myCell.y] != THUAI6::PlaceType::Grass) {
						int newx, newy;
						/*
						if (gameState.gameMap[stuCellPos.x][stuCellPos.y] == THUAI6::PlaceType::Grass) {
							newx = stuCellPos.x;
							newy = stuCellPos.y;
						}
						
						for (int i = -1; i <= 1; i = i + 2) {
							if (gameState.gameMap[stuCellPos.x + i][stuCellPos.y] == THUAI6::PlaceType::Grass) {
								newx = stuCellPos.x + i;
								newy = stuCellPos.y;
							}
						}
						for (int j = -1; j <= 1; j = j + 2) {
							if (gameState.gameMap[stuCellPos.x][stuCellPos.y + j] == THUAI6::PlaceType::Grass) {
								newx = stuCellPos.x;
								newy = stuCellPos.y + j;
							}
						}
						*/
						api.Print("bfsing");
						auto result = BFS::bfs(myCell, THUAI6::PlaceType::Grass, api);
						api.Print(fmt::format("newdist {}", result.second));
						if (result.second < 5&&result.second>=0) {
							newx = result.first.second.x;
							newy = result.first.second.y;
							for (int i = 0; i < 4; i++) {
								if (gameState.studentPos[i].x == stuCellPos.x && gameState.studentPos[i].y == stuCellPos.y) {
									gameState.studentPos[i].x = newx;
									gameState.studentPos[i].y = newy;
									gameState.studentGrid[i].x = api.CellToGrid(newx);
									gameState.studentGrid[i].y = api.CellToGrid(newy);
								}
							}
							api.Print(fmt::format("newxy {} {}", newx, newy));
						}


						//needNew=3;
						//return;
					}
					else {
						api.Print("not bfsing");
						for (int i = 0; i < 4; i++) {
							if (gameState.studentPos[i].x == stuCellPos.x && gameState.studentPos[i].y == stuCellPos.y) {
								gameState.studentPos[i].x = -1;
								gameState.studentPos[i].y = -1;
								gameState.studentGrid[i].x = -1;
								gameState.studentGrid[i].y = -1;
							}
						}
					}




				}
			}
			else
			{
				api.Print(fmt::format("my pos {} {}", myCell.x, myCell.y));
				api.Print(fmt::format("stu pos {} {}", stuCellPos.x, stuCellPos.y));
				auto result = BFS::bfs(myCell, stuCellPos, api);
				auto nextPos = result.first;
				api.Print(fmt::format("next {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1) {
					for (int i = 0; i < 4; i++) {
						if (gameState.studentPos[i].x == stuCellPos.x && gameState.studentPos[i].y == stuCellPos.y) {
							gameState.studentPos[i].x = -1;
							gameState.studentPos[i].y = -1;
						}
					}
					return;
				}
				if (gameState.isType(nextPos, THUAI6::PlaceType::Window))
					api.SkipWindow();
				Action::Move(nextPos, api);
				Action::lastNextPos = nextPos;
				Action::lastMove = Action::posToDirection(nextPos, myCell);
			}
		}
		else//视野里没有学生，就遍历教室并前往
		{
			if (classcount >= 7) {
				auto classroomCellPos = gates[0];
				GridPos classroomGridPos = { api.CellToGrid(classroomCellPos.x),api.CellToGrid(classroomCellPos.y) };
				api.Print(fmt::format("class pos {} {}", classroomCellPos.x, classroomCellPos.y));
				auto result = BFS::bfs(myCell, classroomCellPos, api);
				auto nextPos = result.first;
				api.Print(fmt::format("next pos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1) {
					return;
				}

				if (!gameState.isAtPosition(myCell, classroomCellPos))
				{
					if (gameState.isType(nextPos, THUAI6::PlaceType::Window))
						api.SkipWindow();
					Action::Move(nextPos, api);
					Action::lastNextPos = nextPos;
					Action::lastMove = Action::posToDirection(nextPos, myCell);
				}
				return;
			}
			auto classroomCellPos = traversal[Action::classNo];
			GridPos classroomGridPos = { api.CellToGrid(classroomCellPos.x),api.CellToGrid(classroomCellPos.y) };
			if (gameState.gameMap[classroomCellPos.x][classroomCellPos.y] == THUAI6::PlaceType::ClassRoom) {
				if (api.GetClassroomProgress(classroomCellPos.x, classroomCellPos.y) >= Constants::maxClassroomProgress) {
					traversal.erase(traversal.begin() + Action::classNo);
					classcount++;
				}
				//else if (api.GetClassroomProgress(classroomCellPos.x, classroomCellPos.y) == 0 && api.HaveView(classroomCellPos.x, classroomCellPos.y))
				else if (api.GetClassroomProgress(classroomCellPos.x, classroomCellPos.y) == 0 && api.HaveView(classroomGridPos.x, classroomGridPos.y))
					Action::classNo++;
				else
				{
					api.Print(fmt::format("class pos {} {}", classroomCellPos.x, classroomCellPos.y));
					auto result = BFS::bfs(myCell, classroomCellPos, api);
					auto nextPos = result.first;
					api.Print(fmt::format("next pos {} {}", nextPos.x, nextPos.y));
					if (nextPos.x == -1) {
						Action::classNo++;
						return;
					}
					if (!gameState.isAtPosition(myCell, classroomCellPos))
					{
						if (gameState.isType(nextPos, THUAI6::PlaceType::Window))
							api.SkipWindow();
						Action::Move(nextPos, api);
						Action::lastNextPos = nextPos;
						Action::lastMove = Action::posToDirection(nextPos, myCell);
					}
					else
					{
						//if (api.GetClassroomProgress(classroomCellPos.x, classroomCellPos.y) > 0)
							//api.Attack(Action::getAngle(myGrid, classroomGridPos));
						//else
						Action::classNo++;
					}
				}
			}
			else if (gameState.gameMap[classroomCellPos.x][classroomCellPos.y] == THUAI6::PlaceType::Grass) {
				api.Print(fmt::format("grass pos {} {}", classroomCellPos.x, classroomCellPos.y));
				auto result = BFS::bfs(myCell, classroomCellPos, api);
				auto nextPos = result.first;
				api.Print(fmt::format("next pos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1) {
					Action::classNo++;
					return;
				}
				if (!gameState.isAtCell(myGrid, classroomGridPos))
				{
					if (gameState.isType(nextPos, THUAI6::PlaceType::Window))
						api.SkipWindow();
					Action::Move(nextPos, api);
					Action::lastNextPos = nextPos;
					Action::lastMove = Action::posToDirection(nextPos, myCell);
				}
				else
				{
					Action::classNo++;
				}
			}
			else {
				Action::classNo++;
			}

		}
		//if (needNew > 0) {
			//needNew--;
		//}
	}
	else
	{
		auto self = api.GetSelfInfo();
		GridPos myGrid = { self->x,self->y };
		CellPos myCell = { api.GridToCell(self->x), api.GridToCell(self->y) };
		gameState.getClosestStudent(myGrid, api);
		Action::Move(Action::lastNextPos, api);
		Action::isAtCellFalseCnt++;
	}
}