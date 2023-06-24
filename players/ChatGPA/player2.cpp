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
short map[2][48][48][48][48];
// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::Sunshine
};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;

//可以在AI.cpp内部声明变量与函数
//坐标的结构体
struct posStruct {
	int x, y;
	bool operator!=(const posStruct b) {
		return x != b.x || y != b.y;
	}
	bool operator==(const posStruct b) {
		return x == b.x && y == b.y;
	}
};
//new
struct gridPos {
	int x, y;
};
//end new
class GameState {
public:
	static int readCnt;
	static int clsFinishCnt;
	static const int sizeX = Constants::rows;    //地图X轴最大值
	static const int sizeY = Constants::cols;    //地图Y轴最大值
	static int timeCounter;
	THUAI6::PlaceType gameMap[sizeX][sizeY];	//存图信息
	std::vector <posStruct> ClassRoom;  //线性列出所有ClassRoom坐标（方便遍历）（下同）
	std::vector <posStruct> Gate;
	std::vector <posStruct> HiddenGate;
	std::vector <posStruct> Window;
	std::vector <posStruct> Door3;
	std::vector <posStruct> Door5;
	std::vector <posStruct> Door6;
	std::vector <posStruct> Chest;
	posStruct posStudent[4];    //四个学生的坐标
	posStruct posTricker;       //捣蛋鬼的坐标
	static posStruct helpStudent[4];//学生求救位置
	static int helpcount;//已接受到的学生求救的数量
	static int saveID;//要救的人的ID
	static bool isDead[4];//是不是要去救人
	static bool isHurt[4];//sunshine专用
	static bool ifQuit[4];
	static int iftosave[4];
	int isTrickerSwinging;
	int mapType;
	int determination;
	bool isStudentAttacked;
	bool isTrickerStill;
	void getMap(IStudentAPI& api);              //获取地图格点信息
	void getMap(ITrickerAPI& api);              //获取地图格点信息
	bool isDoor(posStruct pos) const;
	//new
	bool isGrass(posStruct pos) const;
	bool isClassroomNear(posStruct pos) const;
	//
	bool isWindow(posStruct pos) const;
	bool readyToGraduate() const;     //判断当前局面是否可毕业
	bool isTrickerClose(posStruct studentPos, IStudentAPI& api, int maxDistance) const;   //判断student和tricker是否接近，是否要开始规避
	bool canReach(posStruct pos, GameState state, IStudentAPI& api);
	//new
	posStruct isGrassNear(posStruct studentPos, IStudentAPI& api) const;
	double gridDistance(gridPos grid1, gridPos grid2) const;	//计算两点间以grid计的距离
	double getAngle(gridPos grid1, gridPos grid2) const;		//计算两点间方位角
	bool isAttackable(gridPos trickerPos, gridPos studentPos) const;	//判断是否可以攻击
	std::pair<int, double> getClosestStu(gridPos trickerPos, std::vector<std::shared_ptr<const THUAI6::Student>> Students);	//找到视野中最近的学生
	//endnew
	bool isClassroomFinished(posStruct classroomPos, IStudentAPI& studentapi) const; //判断教室是否学完了
	bool isAtCell(int characterX, int characterY, int desX, int desY) const; //判断人物是否完全在格点内
	bool atPosition(posStruct character, posStruct pos) const;    //判断character坐标与pos坐标是否在一个九宫格内
	bool isGateFinished(posStruct gatePos, IStudentAPI& api) const;   //判断大门是否开了
	void deleteStudiedClassroom(posStruct classroomPos);    //把学完的教室删除，变成墙
	bool isTrickerVisible() const;	//判断tricker是否可见
	int distance(posStruct A, posStruct B);
	void getMapType();
	bool isTrickerAttackGrass(IStudentAPI& api, posStruct studentPos);
};

class Action {
public:
	enum class Direction {
		up, down, left, right,
		up_left, up_right, down_left, down_right
	};    //移动方式
	static int isAtCellFalseCnt;
	static const std::vector <Direction> allMove; //所有的移动方式
	static posStruct getNextPos(posStruct nowPos, Direction direction);    //根据当前坐标和移动方式计算出下一个坐标
	static Direction posToDirection(posStruct nowPos, posStruct nextPos); //目标点的坐标转化为Direction
	static bool isValidMove(posStruct judgePos, Direction comedir, GameState state, IStudentAPI& api);//判断judgePos是不是一个合法的位置
	static void Move(posStruct des, IStudentAPI& api);
	static void Move(posStruct des, ITrickerAPI& api);
	static void Move(Direction directon, IStudentAPI& api);
	static void Move(Direction direction, ITrickerAPI& api);
};//所有成员均为静态

class Communicate {
public:
	static void sendClassroonPos(IStudentAPI& api, int64_t myID, posStruct classroomPos);
	static void sendTrickerPos(IStudentAPI& api, int64_t myID);
	static void receiveMessage(IStudentAPI& api, int64_t myID);
	static void sendHelpSignal(IStudentAPI& api, int64_t myID);
	static void getMateinfo(IStudentAPI& api);
	static void sendHelpingSignal(IStudentAPI& api, int64_t myID, int64_t savingID);
	/*
		通信协议：AA BB CC DD
		（AA, BB)为学完的教室的坐标
		（CC, DD)为看到的捣蛋鬼的坐标
		若AA, BB均为-1，则忽略教室坐标，只读捣蛋鬼坐标
		若CC, DD均为-1，则忽略捣蛋鬼坐标，只读教室坐标
	*/
};
int minimax(posStruct studentPos, posStruct tricerPos, int alpha, int beta, bool isStudent, IStudentAPI& api, int depth, bool trickerSwinging, bool isTeacher);
std::pair<int, posStruct > bfs(posStruct start, posStruct destination, IStudentAPI& api);
int GameState::readCnt = 0;
int GameState::clsFinishCnt = 0;
int GameState::timeCounter = 0;
int GameState::saveID = -1;
int GameState::iftosave[4] = { -1,-1,-1,-1 };
bool GameState::isDead[4] = { 0,0,0,0 };
bool GameState::isHurt[4] = { 0,0,0,0 };
int GameState::helpcount = 0;
bool GameState::ifQuit[4] = { 0,0,0,0 };
posStruct GameState::helpStudent[4] = { {-1,-1},{-1,-1},{-1,-1},{-1,-1} };
GameState gameState;

Action::Direction lastMove;  //记录上一次移动方向，用于还没移动到格点时保持向格点移动
//记录上一次的目标点，用于还没移动到格点时保持向格点移动（此处的用法与lastMove等价，最好可以用语义更明确的lastMove，但因为posToDirection还没完善就先用lastNextPos）
int Action::isAtCellFalseCnt = 0;

const std::vector <Action::Direction> Action::allMove = {
	Action::Direction::up,
	Action::Direction::down,
	Action::Direction::left,
	Action::Direction::right,
	Action::Direction::up_left,
	Action::Direction::up_right,
	Action::Direction::down_left,
	Action::Direction::down_right
};   //所有的移动方式

posStruct lastNextPos;
std::pair<posStruct, posStruct> bfs(posStruct start, THUAI6::PlaceType destination, IStudentAPI& api);
//std::pair<posStruct, Action::Direction> minimax(posStruct start, THUAI6::PlaceType destination);   //规避算法（目前还是一个饼）

void GameState::getMap(IStudentAPI& api) {
	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			gameMap[i][j] = (api.GetFullMap()[i][j]);
			if (gameMap[i][j] == THUAI6::PlaceType::ClassRoom) ClassRoom.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Gate) Gate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::HiddenGate) HiddenGate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Window) Window.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Chest) Chest.push_back({ i, j });
		}
	}
	/*
		待完成：学生通过通讯向GameState传入他们的坐标和捣蛋鬼的坐标
	*/
}

void GameState::getMap(ITrickerAPI& api) {
	for (int i = 0; i < sizeX; i++) {
		for (int j = 0; j < sizeY; j++) {
			gameMap[i][j] = (api.GetFullMap()[i][j]);
			if (gameMap[i][j] == THUAI6::PlaceType::ClassRoom) ClassRoom.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Gate) Gate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::HiddenGate) HiddenGate.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Window) Window.push_back({ i, j });
			if (gameMap[i][j] == THUAI6::PlaceType::Chest) Chest.push_back({ i, j });
		}
	}
	/*
		待完成：学生通过通讯向GameState传入他们的坐标和捣蛋鬼的坐标
	*/
}

inline bool GameState::isDoor(posStruct pos) const {
	return gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door3 ||
		gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door5 ||
		gameMap[pos.x][pos.y] == THUAI6::PlaceType::Door6;
}     //判断是不是门

inline bool GameState::isWindow(posStruct pos) const {
	return gameMap[pos.x][pos.y] == THUAI6::PlaceType::Window;
}

//new
inline bool GameState::isGrass(posStruct pos) const {
	return gameMap[pos.x][pos.y] == THUAI6::PlaceType::Grass;
}
//

inline bool GameState::isAtCell(int characterX, int characterY, int desX, int desY) const {
	return abs(characterX - desX) < 100 && abs(characterY - desY) < 100;
}

inline bool GameState::atPosition(posStruct character, posStruct pos) const {
	return (abs(character.x - pos.x) <= 1 && abs(character.y - pos.y) <= 1);
}

bool GameState::readyToGraduate() const {
	if (clsFinishCnt >= Constants::numOfRequiredClassroomForGate)
		return true;
	return false;
}
inline bool GameState::isTrickerVisible() const {
	return !(posTricker.x == -1 && posTricker.y == -1);
}

inline bool GameState::isClassroomNear(posStruct myPos) const {
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (gameState.gameMap[myPos.x + i][myPos.y + j] == THUAI6::PlaceType::ClassRoom) {
				return 1;
			}
		}
	}
	return 0;
}
bool GameState::isTrickerClose(posStruct studentPos, IStudentAPI& api, int maxDistance) const {
	if (!isTrickerVisible()) {
		api.Print(fmt::format("Not visible!!"));
		return 0;
	}
	int distance = gameState.distance(studentPos, gameState.posTricker);
	api.Print(fmt::format("distance:{}", distance));
	return distance <= maxDistance;
}

bool GameState::canReach(posStruct pos, GameState state, IStudentAPI& api) {
	auto students = api.GetStudents();
	for (auto it = students.begin(); it != students.end(); it++)
        {
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y + 390)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y - 390)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
        }
	auto trickers = api.GetTrickers();
	for (auto it = trickers.begin(); it != trickers.end(); it++)
	{
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
	}
	auto bullets = api.GetBullets();
	for (auto it = bullets.begin(); it != bullets.end(); it++)
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
	return
		state.gameMap[pos.x][pos.y] == THUAI6::PlaceType::Land ||
		state.gameMap[pos.x][pos.y] == THUAI6::PlaceType::Grass ||
		state.gameMap[pos.x][pos.y] == THUAI6::PlaceType::Window ||
		(isDoor(pos) && api.IsDoorOpen(pos.x, pos.y));
}


bool GameState::isClassroomFinished(posStruct classroomPos, IStudentAPI& api) const {
	int xcls = classroomPos.x, ycls = classroomPos.y;
	api.Print(fmt::format("Classroom Progress {}", api.GetClassroomProgress(xcls, ycls)));
	if (api.GetClassroomProgress(xcls, ycls) >= Constants::maxClassroomProgress)
		return true;
	return false;
}

posStruct GameState::isGrassNear(posStruct studentPos, IStudentAPI& api) const {
	/*posStruct grassPos = bfs(studentPos, THUAI6::PlaceType::Grass, api).second;
	if (bfs(studentPos, grassPos, api) <= 2) {
		return 1;
	}*/
	auto students = api.GetStudents();
	auto state = gameState;
	for (auto it = students.begin(); it != students.end(); it++)
	{
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
	}
	for (int i = -5; i <= 5; i++) {
		if (studentPos.x + i >= 0 && studentPos.x + i < 50)
			for (int j = -5; j <= 5; j++) {
				if (studentPos.y + j >= 0 && studentPos.y + j < 50)
					if (state.isGrass({ studentPos.x + i, studentPos.y + j })) {
						return { studentPos.x + i, studentPos.y + j };
					}
			}
	}
	return { -1, -1 };
}

bool GameState::isGateFinished(posStruct gatePos, IStudentAPI& api) const {
	int xg = gatePos.x, yg = gatePos.y;
	api.Print(fmt::format("Gate Progress {}", api.GetGateProgress(xg, yg)));
	if (api.GetGateProgress(xg, yg) >= Constants::maxGateProgress)
		return true;
	return false;
}
double GameState::gridDistance(gridPos grid1, gridPos grid2) const {
	double dist;
	dist = std::sqrt(pow((grid1.x - grid2.x), 2) + pow((grid1.y - grid2.y), 2));
	return dist;
}
bool GameState::isAttackable(gridPos trickerPos, gridPos studentPos) const {
	double dist;
	dist = std::sqrt(pow((studentPos.x - trickerPos.x), 2) + pow((studentPos.y - trickerPos.y), 2));
	if (dist < 1500) {
		return true;
	}
	else {
		return false;
	}
}

std::pair<int, double> GameState::getClosestStu(gridPos trickerPos, std::vector<std::shared_ptr<const THUAI6::Student>> Students) {
	int num;
	int min = 0;
	double distance = 1000000;
	gridPos stu;
	for (num = 0; num < Students.size(); num++) {
		if (Students[num]->determination <= 0) {
			continue;
		}
		stu.x = Students[num]->x;
		stu.y = Students[num]->y;
		if (distance > gridDistance(trickerPos, stu)) {
			distance = gridDistance(trickerPos, stu);
			min = num;
		}
	}
	if (distance == 1000000) {
		min = -1;
	}
	return { min,distance };
}
double GameState::getAngle(gridPos grid1, gridPos grid2) const {
	return atan2(grid2.y - grid1.y, grid2.x - grid1.x);
}
void GameState::deleteStudiedClassroom(posStruct classroomPos) {
	int xcls = classroomPos.x, ycls = classroomPos.y;
	if (gameState.gameMap[xcls][ycls] == THUAI6::PlaceType::ClassRoom)
	{
		gameState.gameMap[xcls][ycls] = THUAI6::PlaceType::Wall;    //在map里变成wall
		clsFinishCnt++;
		for (auto it = gameState.ClassRoom.begin(); it != gameState.ClassRoom.end(); it++)
			if ((*it).x == classroomPos.x && (*it).y == classroomPos.y)
			{
				ClassRoom.erase(it);
				break;
			}   //在classroom数组里删除
	}
}

inline int GameState::distance(posStruct A, posStruct B){
	if (A.x >= 1 && A.y >= 1 && B.x >= 1 && B.y >= 1)
		return map[mapType][A.x - 1][A.y - 1][B.x - 1][B.y - 1];
}

void GameState::getMapType(){
	if (gameMap[5][9] == THUAI6::PlaceType::Grass){
		mapType = 0;
	}
	else
		mapType = 1;
}

bool GameState::isTrickerAttackGrass(IStudentAPI &api, posStruct studentPos){
	if (isGrass(studentPos) && isStudentAttacked){
		return true;
	}
	return false;
}

posStruct Action::getNextPos(posStruct nowPos, Direction direction) {
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
}

Action::Direction Action::posToDirection(posStruct myPos, posStruct nextPos)
{
	if (nextPos.x == myPos.x + 1 && nextPos.y == myPos.y)
		return Action::Direction::down;
	if (nextPos.x == myPos.x - 1 && nextPos.y == myPos.y)
		return Action::Direction::up;
	if (nextPos.x == myPos.x && nextPos.y == myPos.y + 1)
		return Action::Direction::right;
	if (nextPos.x == myPos.x && nextPos.y == myPos.y - 1)
		return Action::Direction::left;
	if (nextPos.x == myPos.x + 1 && nextPos.y == myPos.y + 1)
		return Action::Direction::down_right;
	if (nextPos.x == myPos.x - 1 && nextPos.y == myPos.y + 1)
		return Action::Direction::up_right;
	if (nextPos.x == myPos.x + 1 && nextPos.y == myPos.y - 1)
		return Action::Direction::down_left;
	if (nextPos.x == myPos.x - 1 && nextPos.y == myPos.y - 1)
		return Action::Direction::up_left;
}

inline bool Action::isValidMove(posStruct judgePos, Direction comeDir, GameState state, IStudentAPI& api)
{
    auto students = api.GetStudents();
	for (auto it = students.begin(); it != students.end(); it++)
    {
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y + 390)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y - 390)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
            state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
    }
	auto trickers = api.GetTrickers();
	for (auto it = trickers.begin(); it != trickers.end(); it++)
	{
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 390)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y + 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y - 390)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x + 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y + 280)] = THUAI6::PlaceType::Wall;
		state.gameMap[api.GridToCell((*(*it)).x - 280)][api.GridToCell((*(*it)).y - 280)] = THUAI6::PlaceType::Wall;
	}
	auto bullets = api.GetBullets();
	for (auto it = bullets.begin(); it != bullets.end(); it++)
		state.gameMap[api.GridToCell((*(*it)).x)][api.GridToCell((*(*it)).y)] = THUAI6::PlaceType::Wall;
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
	}//这里判断走斜线时会不会被目标周围点卡住，只有当目标周围点（两个）为Land
	return state.canReach(judgePos, state, api);
}

void Action::Move(posStruct destination, IStudentAPI& api) {
	int desX = api.CellToGrid(destination.x);
	int desY = api.CellToGrid(destination.y);
	auto self = api.GetSelfInfo();
	auto directRad = atan2(1.0 * desY - (*self).y, 1.0 * desX - (*self).x);
	api.Move(50, directRad);
}

void Action::Move(posStruct destination, ITrickerAPI& api) {
	int desX = api.CellToGrid(destination.x);
	int desY = api.CellToGrid(destination.y);
	auto self = api.GetSelfInfo();
	auto directRad = atan2(1.0 * desY - (*self).y, 1.0 * desX - (*self).x);
	api.Move(50, directRad);
}

void Action::Move(Action::Direction direction, IStudentAPI& api) {
	auto self = api.GetSelfInfo();
	int x = api.GridToCell((*self).x), y = api.GridToCell((*self).y);
	Move(Action::getNextPos({ x, y }, direction), api);
}

void Action::Move(Action::Direction direction, ITrickerAPI& api) {
	auto self = api.GetSelfInfo();
	int x = api.GridToCell((*self).x), y = api.GridToCell((*self).y);
	Move(Action::getNextPos({ x, y }, direction), api);
}

void Communicate::sendClassroonPos(IStudentAPI& api, int64_t myID, posStruct classroomPos) {
	auto message = fmt::format("p {} {} {} {} {}", classroomPos.x, classroomPos.y, -1, -1, 0);
	//api.Print(message);
	for (int id = 0; id <= 3; id++) {
		if (id != myID) {
			api.SendTextMessage(id, message);
		}
	}
}
void Communicate::getMateinfo(IStudentAPI& api) {
	auto students = api.GetStudents();
	gameState.helpcount = 0;
	for (int i = 0; i <= 3; i++) {
		gameState.isDead[i] = 0;
		gameState.isHurt[i] = 0;
	}
	for (auto it = students.begin(); it != students.end(); it++) {
		auto stu = (*(*it));
		auto state = stu.playerState;
		gameState.posStudent[stu.playerID].x = api.GridToCell(stu.x);
		gameState.posStudent[stu.playerID].y = api.GridToCell(stu.y);
		if (state == THUAI6::PlayerState::Quit)gameState.ifQuit[stu.playerID] = 1;
		if (stu.determination <= 0&&state!=THUAI6::PlayerState::Quit)gameState.isDead[stu.playerID] = 1;
		else {
			switch (stu.playerID) {
			case 0: {if (stu.determination < 3000000)gameState.isHurt[0] = 1; }break;
			case 1: {if (stu.determination < 3000000)gameState.isHurt[1] = 1; }break;
			case 2: {if (stu.determination < 29000000)gameState.isHurt[2] = 1; }break;
			case 3: {if (stu.determination < 3200000)gameState.isHurt[3] = 1; }break;
			default:break;
			}
		}
	}
	for (int i = 0; i < 4; i++)api.Print(fmt::format(" isDead {} is {}", i, gameState.isDead[i]));
	for (int i = 0; i < 4; i++)api.Print(fmt::format(" isHurt {} is {}", i, gameState.isHurt[i]));
}
void Communicate::sendHelpSignal(IStudentAPI& api, int64_t myID) {
	for (int i = 0; i < 4; i++) {
		if (gameState.iftosave[i] == myID)return;//如果有人来救自己，就不发了
	}
	int trickergridx = api.CellToGrid(gameState.posTricker.x);
	int trickergridy = api.CellToGrid(gameState.posTricker.y);
	if (api.HaveView(trickergridx, trickergridy))return;//如果还能看到tricker，就不发了
	auto self = api.GetSelfInfo();
	std::pair<int, posStruct>bfsResult;
	int targetID = -1;
	int minstep = 9999;
	posStruct myPos, nextPos;
	myPos.x = api.GridToCell(self->x);
	myPos.y = api.GridToCell(self->y);
	auto message = fmt::format("h  {}", myID);
	for (int i = 0; i <= 3; i++) {
		if (!gameState.isDead[3] && (gameState.iftosave[3] == -1 || gameState.iftosave[3] == myID)&&!gameState.ifQuit[i]) {
			targetID = 3;
			break;
		}
		if (!gameState.isDead[i] && (gameState.iftosave[i] == -1 || gameState.iftosave[i] == myID)&&!gameState.ifQuit[i]) {
			posStruct nowstudent = { -1,-1 };
			nowstudent.x = gameState.posStudent[i].x;
			nowstudent.y = gameState.posStudent[i].y;
			bfsResult = bfs(myPos, nowstudent, api);
			if (bfsResult.first < minstep) {
				targetID = i;
				api.Print(fmt::format("targetID changed to {}", i));
				minstep = bfsResult.first;
				nextPos = bfsResult.second;
			}
		}
	}
	api.SendTextMessage(targetID, message);
	api.Print(fmt::format("sos sent to {}", targetID));
}
void Communicate::sendHelpingSignal(IStudentAPI& api, int64_t myID, int64_t savingID) {
	auto message = fmt::format("s {} {}", myID, savingID);
	for (int i = 0; i <= 3; i++) {
		if (i != myID)api.SendTextMessage(i, message);
	}
}
//new
void Communicate::sendTrickerPos(IStudentAPI& api, int64_t myID) {
	auto trickerPtrVector = api.GetTrickers();
	if (trickerPtrVector.empty()) { return; }
	posStruct trickerPos = { api.GridToCell(trickerPtrVector[0]->x), api.GridToCell(trickerPtrVector[0]->y) };
	int trickerSwinging = trickerPtrVector[0]->playerState == THUAI6::PlayerState::Swinging;
	std::string message = fmt::format("p {} {} {} {} {}", -1, -1, trickerPos.x, trickerPos.y, trickerSwinging);
	for (int i = 0; i <= 3; i++) {
		if (i != myID) {
			api.SendTextMessage(i, message);
		}
	}
}

void Communicate::receiveMessage(IStudentAPI& api, int64_t myID) {
	std::pair<int64_t, std::string> message;
	posStruct classroomPos, trickerPos, studentPos;
	int64_t helpID = 5, tobesaved, savingid, saverid;
	char type;
	int messageOfTrickerSwinging;
	std::stringstream ss;
	if (GameState::timeCounter >= 300)
	{
		gameState.posTricker = { -1, -1 };
		gameState.isTrickerSwinging = 0;
		GameState::timeCounter = 0;
		gameState.isStudentAttacked = false;
	}
	auto myViewOfTricker = api.GetTrickers();
	if (!myViewOfTricker.empty()) {
		gameState.posTricker = { api.GridToCell(myViewOfTricker[0]->x), api.GridToCell(myViewOfTricker[0]->y) };
		gameState.isTrickerSwinging = myViewOfTricker[0]->playerState == THUAI6::PlayerState::Swinging;
	}
	while (api.HaveMessage()) {
		ss.clear();
		message = api.GetMessage();
		ss.str(message.second);
		ss >> type;
		if (type == 'p') {
			ss >> classroomPos.x >> classroomPos.y >> trickerPos.x >> trickerPos.y >> messageOfTrickerSwinging;
			if (classroomPos.x != -1 && classroomPos.y != -1) {
				gameState.deleteStudiedClassroom(classroomPos);
			}
			if (trickerPos.x != -1 && trickerPos.y != -1) {
				gameState.posTricker = trickerPos;
			}
			if (messageOfTrickerSwinging)
				gameState.isTrickerSwinging = messageOfTrickerSwinging;
		}
		else if (type == 'h') {
			ss >> savingid;
			gameState.iftosave[myID] = savingid;
			gameState.saveID = savingid;
			api.Print(fmt::format("{} to save {} is true", myID, savingid));
		}
		else if (type == 's') {
			ss >> saverid >> savingid;
			gameState.iftosave[saverid] = savingid;
			api.Print(fmt::format("{} 's iftosave {} is true", myID, saverid));
		}
	}
}


bool vis[GameState::sizeX][GameState::sizeY];
posStruct now, nxt, pre[GameState::sizeX][GameState::sizeY];
std::pair<posStruct, posStruct> bfs(posStruct start, THUAI6::PlaceType destination, IStudentAPI& api)
{//return pair<next_point,destination>
	std::queue<posStruct> posQ;

	memset(vis, false, sizeof(vis));
	vis[start.x][start.y] = true;
	posQ.push(start);
	while (!posQ.empty())
	{
		now = posQ.front();
		posQ.pop();
		for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++)
		{
			nxt = Action::getNextPos(now, *move);
			if (gameState.gameMap[nxt.x][nxt.y] == destination && !gameState.canReach(nxt, gameState, api))
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
			while (pre[now.x][now.y].x != start.x || pre[now.x][now.y].y != start.y)
				now = pre[now.x][now.y];
			return { now,nxt };
		}
	}
	return { {-1,-1},{-1,-1} };
}
std::pair<int, posStruct> bfs(posStruct start, posStruct destination, IStudentAPI& api)
{//return pair<next_point,destination>
	std::queue<std::pair <posStruct, int> > posQ;
	int length = 0;
	memset(vis, false, sizeof(vis));
	vis[start.x][start.y] = true;
	posQ.push({ start, 0 });
	while (!posQ.empty())
	{
		auto node = posQ.front();
		now = node.first;
		length = node.second;
		posQ.pop();
		for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++)
		{
			nxt = Action::getNextPos(now, *move);
			if (nxt.x == destination.x && nxt.y == destination.y && !gameState.canReach(nxt, gameState, api))
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
			posQ.push({ nxt, length + 1 });
			pre[nxt.x][nxt.y] = now;
		}
		if (now.x == nxt.x && now.y == nxt.y)
		{
			while (pre[now.x][now.y].x != start.x || pre[now.x][now.y].y != start.y)
				now = pre[now.x][now.y];
			return { length + 1, now };
		}
	}
	return { -1, {-1, -1} };
}

int minimax(posStruct studentPos, posStruct trickerPos, int alpha, int beta, bool isStudent, IStudentAPI& api, int depth, bool trickerSwinging, bool isTeacher) {
	if (depth == 0) {
		if (gameState.isTrickerVisible()){
			// return (studentPos.x - trickerPos.x) * (studentPos.x - trickerPos.x) + (studentPos.y - trickerPos.y) * (studentPos.y - trickerPos.y);
            if (!isTeacher)
                return gameState.distance(studentPos, trickerPos);
            else{
                auto students = api.GetStudents();
				int targetDistance = 10000;
				posStruct targetStudent;
                for (auto it = students.begin(); it != students.end(); it++){
                    posStruct studentsPos = {api.GridToCell((*(*it)).x), api.GridToCell((*(*it)).y)};
                    if (gameState.distance(studentsPos, gameState.posTricker) < targetDistance){
                        targetDistance = gameState.distance(studentsPos, gameState.posTricker);
                        targetStudent = studentsPos;
                    }
                }
                return 3 * (gameState.distance(studentPos, trickerPos) + gameState.distance(studentPos, targetStudent));
            }
		}
		else 
			return 0;

	}
	if (isStudent) {
		for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
			posStruct nextPos = Action::getNextPos(studentPos, *move);
			if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
			alpha = std::max(alpha, minimax(nextPos, trickerPos, alpha, beta, !isStudent, api, depth, trickerSwinging, isTeacher));
			if (beta <= alpha) {
				break;
			}
		}
		return alpha;
	}
	else {
		if (trickerSwinging) {
			beta = std::min(beta, minimax(studentPos, trickerPos, alpha, beta, !isStudent, api, depth - 1, trickerSwinging, isTeacher));
		}
		else {
			for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
				posStruct nextPos = Action::getNextPos(trickerPos, *move);
				if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
				beta = std::min(beta, minimax(studentPos, nextPos, alpha, beta, !isStudent, api, depth - 1, trickerSwinging, isTeacher));
				if (beta <= alpha)
					break;
			}
		}
		return beta;
	}
}
void AI::play(IStudentAPI& api)
{
	if (this->playerID == 0 || this->playerID == 1)
	{
		// 玩家0执行操作
			// 公共操作
		GameState::timeCounter++;

		if (gameState.readCnt < 1)
		{
			gameState.getMap(api);
			gameState.readCnt++;
			gameState.getMapType();
			api.PrintSelfInfo();
			gameState.determination = api.GetSelfInfo()->determination;
		}
		auto self = api.GetSelfInfo();
		if (self->playerState == THUAI6::PlayerState::Quit) return;

		if (self->determination < gameState.determination) {
			gameState.isStudentAttacked = true;
		}
		gameState.determination = self->determination;
		posStruct myPos = { api.GridToCell((*self).x), api.GridToCell((*self).y) };
		api.Print(fmt::format("cls finish cnt {}", GameState::clsFinishCnt));
		if ((*self).determination <= 0)Communicate::sendHelpSignal(api, this->playerID);
		Communicate::sendTrickerPos(api, this->playerID);
		Communicate::receiveMessage(api, this->playerID);
		Communicate::getMateinfo(api);//获取友军位置和死亡情况
		if (gameState.saveID != -1) {//初始化救人信息
			if (!gameState.isDead[gameState.saveID]) {
				gameState.saveID = -1;
				api.Print(fmt::format("saveID reset"));
			}
			else api.StartRouseMate(gameState.saveID);//开局救一下人
		}
		// if (gameState.isClassroomNear(myPos)) {
		// 	api.UseSkill(0);
		// 	api.StartLearning();
		// }//直接学一下
		api.Print(fmt::format("{} save id is {}", this->playerID, gameState.saveID));
		if (gameState.isAtCell((*self).x, (*self).y, api.CellToGrid(myPos.x), api.CellToGrid(myPos.y)) || Action::isAtCellFalseCnt > 10)
		{
			Action::isAtCellFalseCnt = 0;
			if (gameState.isTrickerClose(myPos, api, 10))
			{
				//离Tricker过近
				//规避Tricker（待完成）
				api.Print(fmt::format("start escaping!!!"));
				posStruct grassOccupied = { -1, -1 };
				posStruct nearGrass = gameState.isGrassNear(myPos, api);
				if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
					auto students = api.GetStudents();
					auto grass = bfs(myPos, nearGrass, api);
					posStruct nextPos = grass.second;
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
				}
				else if (gameState.isGrass(myPos) && !gameState.isGrass(gameState.posTricker) && !gameState.isStudentAttacked) {

				}
				else
				{
					if (gameState.isTrickerSwinging) {
						api.Print(fmt::format("Tricker is swinging, run!"));
					}
					int alpha = -1000000;
					int beta = 1000000;
					int ans = -100000;
					Action::Direction ans_action;
					bool flag = 0;
					posStruct nextPos;
					for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
						nextPos = Action::getNextPos(myPos, *move);
						if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
						int possibleValue;
						if (gameState.isTrickerAttackGrass(api, myPos)){
							possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, true, false);
						}
						else {
							possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, gameState.isTrickerSwinging, false);
						}
						api.Print(fmt::format("Possiblevalue:{}", possibleValue));
						if (possibleValue > ans) {
							ans = possibleValue;
							ans_action = *move;
							flag = 1;
							api.Print(fmt::format("ans_action updated!!"));

						}
					}
					if (flag) {
						nextPos = Action::getNextPos(myPos, ans_action);
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(ans_action, api);
						lastNextPos = nextPos;
						api.Print(fmt::format("ans_action:{}", (int)ans_action));
					}
				}
			}//规避
			else if (api.GetSelfInfo()->dangerAlert && api.GetSelfInfo()->dangerAlert < 1000000) {
				if (gameState.isGrass(myPos)) {
					//如果在草里，就待着
				}
				else {
					//如果不在草里，就进草
					api.Print(fmt::format("dangerAlert: {}", api.GetSelfInfo()->dangerAlert));
					auto grass = bfs(myPos, THUAI6::PlaceType::Grass, api);

					posStruct grassOccupied = { -1, -1 };
					// posStruct nearGrass = gameState.isGrassNear(myPos, api);
					posStruct nearGrass = grass.first;
					if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
						auto students = api.GetStudents();
						auto grass = bfs(myPos, nearGrass, api);
						posStruct nextPos = grass.second;
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(nextPos, api);
						lastNextPos = nextPos;
					}
				}
			}
			else if (gameState.saveID != -1)//离Tricker有一段距离且有人需要救,去救人
			{
				posStruct nextPos = { -1,-1 };
				std::pair<int, posStruct>bfsResult;
				bfsResult = bfs(myPos, gameState.posStudent[gameState.saveID], api);
				nextPos = bfsResult.second;
				Communicate::sendHelpingSignal(api, this->playerID, gameState.saveID);
				api.Print(fmt::format("go to save {}", gameState.saveID));

				if (!gameState.atPosition(myPos, gameState.posStudent[gameState.saveID])) {//如果还没到救人位置
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
					api.Print(fmt::format("moving to save {}", gameState.saveID));
				}
			}
			else if (!gameState.readyToGraduate())
			{
				//还没达到毕业要求
				//bfs搜教室
				auto tmp = bfs(myPos, THUAI6::PlaceType::ClassRoom, api);
				auto nextPos = tmp.first;
				auto classroomPos = tmp.second;
				api.Print(fmt::format("Classroom nextPos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1)
					return;
				if (!gameState.atPosition(myPos, classroomPos))
				{
					//还没到达教室,前往教室
					//如果是窗，就翻窗
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
				}
				else
				{
					//到达教室，开始学习
					api.Print("reach classroom");
					if (gameState.isClassroomFinished(classroomPos, api))
					{
						//如果学完了,就把这个教室从教室列表中删掉
						api.Print("finish classroom");
						gameState.deleteStudiedClassroom(classroomPos);
						Communicate::sendClassroonPos(api, this->playerID, classroomPos);
					}
					else
					{
						//如果没学完，就在教室学
						api.UseSkill(0);
						api.StartLearning();
					}
				}
			}
			else
			{
				//达到毕业要求，前往校门处开门
				api.Print("ready to graduate");
				auto tmp = bfs(myPos, THUAI6::PlaceType::Gate, api);
				auto nextPos = tmp.first;
				auto gatePos = tmp.second;
				api.Print(fmt::format("Gate nextPos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1)
					return;
				if (!gameState.atPosition(myPos, gatePos))
				{
					//还没到达大门,前往大门
					//如果是窗，就翻窗
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
				}
				else
				{
					//到达大门，开始开门
					//api.Print("reach gate");
					if (gameState.isGateFinished(gatePos, api))
					{
						//如果开门了,就毕业
						//api.Print("gate opened");
						api.Graduate();
					}
					else
					{
						//如果没开好，就开门
						api.StartOpenGate();
					}
				}
			}
		}
		else
		{
			//如果不在完整格点内，就先移动到格点
			Action::isAtCellFalseCnt++;
			Action::Move(lastNextPos, api);
		}
	}
	else if (this->playerID == 2)
	{
		// 公共操作
		GameState::timeCounter++;
		if (gameState.readCnt < 1)
		{
			gameState.getMap(api);
			gameState.readCnt++;
			gameState.getMapType();
			api.PrintSelfInfo();
		}
		auto self = api.GetSelfInfo();
		if (self->playerState == THUAI6::PlayerState::Quit)return;
		posStruct myPos = { api.GridToCell((*self).x), api.GridToCell((*self).y) };
		api.Print(fmt::format("cls finish cnt {}", GameState::clsFinishCnt));
		if ((*self).determination <= 0)Communicate::sendHelpSignal(api, this->playerID);
		Communicate::sendTrickerPos(api, this->playerID);
		Communicate::receiveMessage(api, this->playerID);
		Communicate::getMateinfo(api);//获取友军位置和死亡情况
		if (gameState.saveID != -1) {//初始化救人信息
			if (!gameState.isDead[gameState.saveID]) {
				gameState.saveID = -1;
				api.Print(fmt::format("saveID reset"));
			}
			else api.StartRouseMate(gameState.saveID);//开局救一下人
		}
		// if (gameState.isClassroomNear(myPos)) {
		// 	api.UseSkill(0);
		// 	api.StartLearning();
		// }
		api.Print(fmt::format("{} save id is {}", this->playerID, gameState.saveID));
		if (gameState.isAtCell((*self).x, (*self).y, api.CellToGrid(myPos.x), api.CellToGrid(myPos.y)) || Action::isAtCellFalseCnt > 10){
			Action::isAtCellFalseCnt = 0;
			if (gameState.saveID != -1)//离Tricker有一段距离且有人需要救,去救人
			{
				posStruct nextPos = { -1,-1 };
				std::pair<int, posStruct>bfsResult;
				bfsResult = bfs(myPos, gameState.posStudent[gameState.saveID], api);
				nextPos = bfsResult.second;
				Communicate::sendHelpingSignal(api, this->playerID, gameState.saveID);
				api.Print(fmt::format("go to save {}", gameState.saveID));

				if (!gameState.atPosition(myPos, gameState.posStudent[gameState.saveID])) {//如果还没到救人位置
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
					api.Print(fmt::format("moving to save {}", gameState.saveID));
				}
			}
			else if(!gameState.readyToGraduate()) {
			//----------------------找教室------------------------
			if (!gameState.isTrickerVisible()) {
				auto tmp = bfs(myPos, THUAI6::PlaceType::ClassRoom, api);
				auto nextPos = tmp.first;
				auto classroomPos = tmp.second;
				api.Print(fmt::format("Classroom nextPos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1)
					return;
				if (!gameState.atPosition(myPos, classroomPos))
				{
					//还没到达教室,前往教室
					//如果是窗，就翻窗
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
				}
				else
				{
					//到达教室，开始学习
					api.Print("reach classroom");
					if (gameState.isClassroomFinished(classroomPos, api))
					{
						//如果学完了,就把这个教室从教室列表中删掉
						api.Print("finish classroom");
						gameState.deleteStudiedClassroom(classroomPos);
						Communicate::sendClassroonPos(api, this->playerID, classroomPos);
					}
					else
					{
						//如果没学完，就在教室学
						api.StartLearning();
					}
				}
			
			}
			else if (gameState.distance(myPos, gameState.posTricker) >= 10) {
				//---------------离tricker有一定距离，寻找tricker---------------
				auto tricker = bfs(myPos, gameState.posTricker, api);
				posStruct nextPos = tricker.second;
				if (gameState.isWindow(nextPos))
					api.SkipWindow();
				Action::Move(nextPos, api);
				lastNextPos = nextPos;
			}
			else {
				//------------离tricker很近---------------------
				auto self = api.GetSelfInfo();
				double punishTime = self->timeUntilSkillAvailable[0];
				double haveTeaTime = self->timeUntilSkillAvailable[1];
				if (punishTime > 0) {
					//-------------------------技能还没好，先逃跑---------
					posStruct grassOccupied = { -1, -1 };
					posStruct nearGrass = gameState.isGrassNear(myPos, api);
					if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
						auto students = api.GetStudents();
						auto grass = bfs(myPos, nearGrass, api);
						posStruct nextPos = grass.second;
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(nextPos, api);
						lastNextPos = nextPos;
					}
					else if (gameState.isGrass(myPos) && !gameState.isGrass(gameState.posTricker) && !gameState.isStudentAttacked) {

					}
					else {
						int alpha = -1000000;
						int beta = 1000000;
						int ans = -100000;
						Action::Direction ans_action;
						bool flag = 0;
						posStruct nextPos;
						for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
							nextPos = Action::getNextPos(myPos, *move);
							if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
							int possibleValue;
							if (gameState.isTrickerAttackGrass(api, myPos)){
								possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, true, false);
							}
							else {
								possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, gameState.isTrickerSwinging, false);
							}
							api.Print(fmt::format("Possiblevalue:{}", possibleValue));
							if (possibleValue > ans) {
								ans = possibleValue;
								ans_action = *move;
								flag = 1;
								api.Print(fmt::format("ans_action updated!!"));
							}
						}
						if (flag) {
							nextPos = Action::getNextPos(myPos, ans_action);
							//如果闪现好了，就闪现
							if (haveTeaTime == 0) {
								gridPos myGrid, nextGrid;
								myGrid = { api.CellToGrid(myPos.x), api.CellToGrid(myPos.y) };
								nextGrid = { api.CellToGrid(nextPos.x), api.CellToGrid(nextPos.y) };
								double angle = gameState.getAngle(myGrid, nextGrid);
								api.UseSkill(1, angle * 1000);
							}
							else {
								if (gameState.isWindow(nextPos))
									api.SkipWindow();
								Action::Move(ans_action, api);
								lastNextPos = nextPos;
								api.Print(fmt::format("ans_action:{}", (int)ans_action));
							}
						}
					}
				}
				else {
					//----------------------技能好了，给tricker迎头痛击！---------
					auto tricker = bfs(myPos, gameState.posTricker, api);
					posStruct nextPos = tricker.second;
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					gridPos myGrid, trickerGrid;
					myGrid = { self->x, self->y };
					trickerGrid = { api.CellToGrid(gameState.posTricker.x), api.CellToGrid(gameState.posTricker.y) };
					if (sqrt((myGrid.x - trickerGrid.x) * (myGrid.x - trickerGrid.x) + (myGrid.y - trickerGrid.y) * (myGrid.y - trickerGrid.y)) <= self->viewRange / 3) {
						api.UseSkill(0);
					}
				}
			}
			}
			else{
				auto self = api.GetSelfInfo();
				double punishTime = self->timeUntilSkillAvailable[0];
				double haveTeaTime = self->timeUntilSkillAvailable[1];
				posStruct grassOccupied = { -1, -1 };
					posStruct nearGrass = gameState.isGrassNear(myPos, api);
					if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
						auto students = api.GetStudents();
						auto grass = bfs(myPos, nearGrass, api);
						posStruct nextPos = grass.second;
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(nextPos, api);
						lastNextPos = nextPos;
					}
					else if (gameState.isGrass(myPos) && !gameState.isGrass(gameState.posTricker) && !gameState.isStudentAttacked) {

					}
					else {
						int alpha = -1000000;
						int beta = 1000000;
						int ans = -100000;
						Action::Direction ans_action;
						bool flag = 0;
						posStruct nextPos;
						for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
							nextPos = Action::getNextPos(myPos, *move);
							if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
							int possibleValue;
							if (gameState.isTrickerAttackGrass(api, myPos)){
								possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, true, false);
							}
							else {
								possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, gameState.isTrickerSwinging, false);
							}
							api.Print(fmt::format("Possiblevalue:{}", possibleValue));
							if (possibleValue > ans) {
								ans = possibleValue;
								ans_action = *move;
								flag = 1;
								api.Print(fmt::format("ans_action updated!!"));
							}
						}
						if (flag) {
							nextPos = Action::getNextPos(myPos, ans_action);
							//如果闪现好了，就闪现
							if (haveTeaTime == 0) {
								gridPos myGrid, nextGrid;
								myGrid = { api.CellToGrid(myPos.x), api.CellToGrid(myPos.y) };
								nextGrid = { api.CellToGrid(nextPos.x), api.CellToGrid(nextPos.y) };
								double angle = gameState.getAngle(myGrid, nextGrid);
								api.UseSkill(1, angle * 1000);
							}
							else {
								if (gameState.isWindow(nextPos))
									api.SkipWindow();
								Action::Move(ans_action, api);
								lastNextPos = nextPos;
								api.Print(fmt::format("ans_action:{}", (int)ans_action));
							}
						}
					}
			}
		}
		else
		{
			//如果不在完整格点内，就先移动到格点
			Action::isAtCellFalseCnt++;
			Action::Move(lastNextPos, api);
		}
	}
	//当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
	//公共操作
	else if (this->playerID == 3){
		GameState::timeCounter++;
		
		if (gameState.readCnt < 1)
		{
			gameState.getMap(api);
			gameState.readCnt++;
			gameState.getMapType();
			api.PrintSelfInfo();
		}
		auto self = api.GetSelfInfo();
		if (self->playerState == THUAI6::PlayerState::Quit)return;
		bool iftocure = 0;//是否去给人回血
		int curetarget = -1;
		posStruct myPos = { api.GridToCell((*self).x), api.GridToCell((*self).y) };
		api.Print(fmt::format("cls finish cnt {}", GameState::clsFinishCnt));
		if ((*self).determination <= 0)Communicate::sendHelpSignal(api, this->playerID);
		Communicate::sendTrickerPos(api, this->playerID);
		Communicate::receiveMessage(api, this->playerID);
		Communicate::getMateinfo(api);//获取友军位置和死亡情况
		if (gameState.saveID != -1) {//初始化救人信息
			if (!gameState.isDead[gameState.saveID]) {
				gameState.saveID = -1;
				api.Print(fmt::format("saveID reset"));
			}
			else {
				int32_t gridx = api.CellToGrid(gameState.posStudent[gameState.saveID].x);
				int32_t gridy = api.CellToGrid(gameState.posStudent[gameState.saveID].y);
				if (api.HaveView(gridx, gridy)) {
					api.UseSkill(0);
					api.UseSkill(1);
				}
				api.StartRouseMate(gameState.saveID);
			}
		}
		else {
			for (int i = 0; i <= 3; i++)
				if (gameState.isHurt[i]) {
					int32_t gridx = api.CellToGrid(gameState.posStudent[i].x);
					int32_t gridy = api.CellToGrid(gameState.posStudent[i].y);
					if (api.HaveView(gridx, gridy)) {
						api.UseSkill(1);
						api.UseSkill(2);
						api.Print(fmt::format("useskill 1 and 2"));
					}
					else {
						if (self->timeUntilSkillAvailable[1] == 0)
							curetarget = i;
					}
				}
		}
		//直接学一下
		/*for (int i = 0; i < 4; i++) {
			if (gameState.isDead[i])api.StartRouseMate(i);
		}//直接救一下*/
		api.Print(fmt::format("{} save id is {}", this->playerID, gameState.saveID));
		api.Print(fmt::format("{} curetarget is {}", this->playerID, curetarget));
		if (gameState.isAtCell((*self).x, (*self).y, api.CellToGrid(myPos.x), api.CellToGrid(myPos.y)) || Action::isAtCellFalseCnt > 10)
		{
			Action::isAtCellFalseCnt = 0;
			if (gameState.isTrickerClose(myPos, api, 10))
			{
				//离Tricker过近
				//规避Tricker（待完成）
				api.Print(fmt::format("start escaping!!!"));
				posStruct grassOccupied = { -1, -1 };
				posStruct nearGrass = gameState.isGrassNear(myPos, api);
				if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
					auto students = api.GetStudents();
					auto grass = bfs(myPos, nearGrass, api);
					posStruct nextPos = grass.second;
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
				}
				else if (gameState.isGrass(myPos) && !gameState.isGrass(gameState.posTricker) && !gameState.isStudentAttacked) {

				}
				else
				{
					if (gameState.isTrickerSwinging) {
						api.Print(fmt::format("Tricker is swinging, run!"));
					}
					int alpha = -1000000;
					int beta = 1000000;
					int ans = -100000;
					Action::Direction ans_action;
					bool flag = 0;
					posStruct nextPos;
					for (auto move = Action::allMove.begin(); move != Action::allMove.end(); move++) {
						nextPos = Action::getNextPos(myPos, *move);
						if (!Action::isValidMove(nextPos, *move, gameState, api)) continue;
						int possibleValue;
						if (gameState.isTrickerAttackGrass(api, myPos)){
							possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, true, false);
						}
						else {
							possibleValue = minimax(nextPos, gameState.posTricker, alpha, beta, 0, api, 3, gameState.isTrickerSwinging, false);
						}
						api.Print(fmt::format("Possiblevalue:{}", possibleValue));
						if (possibleValue > ans) {
							ans = possibleValue;
							ans_action = *move;
							flag = 1;
							api.Print(fmt::format("ans_action updated!!"));

						}
					}
					if (flag) {
						nextPos = Action::getNextPos(myPos, ans_action);
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(ans_action, api);
						lastNextPos = nextPos;
						api.Print(fmt::format("ans_action:{}", (int)ans_action));
					}
				}
			}//规避
			else if (api.GetSelfInfo()->dangerAlert && api.GetSelfInfo()->dangerAlert < 1000000) {
				if (gameState.isGrass(myPos)) {

				}
				else {
					api.Print(fmt::format("dangerAlert: {}", api.GetSelfInfo()->dangerAlert));
					auto grass = bfs(myPos, THUAI6::PlaceType::Grass, api);
					api.UseSkill(2);
					posStruct grassOccupied = { -1, -1 };
					posStruct nearGrass = grass.first;
					if (!gameState.isGrass(myPos) && nearGrass != grassOccupied && !gameState.isGrass(gameState.posTricker)) {
						auto students = api.GetStudents();
						auto grass = bfs(myPos, nearGrass, api);
						posStruct nextPos = grass.second;
						if (gameState.isWindow(nextPos))
							api.SkipWindow();
						Action::Move(nextPos, api);
						lastNextPos = nextPos;
					}

					// posStruct nextPos = grass.first;
					// posStruct grassPos = grass.second;
					// api.Print(fmt::format("grass: ({}, {})", grassPos.x, grassPos.y));
					// if (gameState.isWindow(nextPos))
					// 	api.SkipWindow();
					// Action::Move(nextPos, api);
					// lastNextPos = nextPos;
				}
			}
			else if (gameState.saveID != -1)//离Tricker有一段距离且有人需要救,去救人
			{
				posStruct nextPos = { -1,-1 };
				std::pair<int, posStruct>bfsResult;
				bfsResult = bfs(myPos, gameState.posStudent[gameState.saveID], api);
				nextPos = bfsResult.second;
				Communicate::sendHelpingSignal(api, this->playerID, gameState.saveID);
				api.Print(fmt::format("go to save {}", gameState.saveID));

				if (!gameState.atPosition(myPos, gameState.posStudent[gameState.saveID])) {//如果还没到救人位置
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
					api.Print(fmt::format("moving to save {}", gameState.saveID));
				}


			}
			else if (curetarget != -1) {//有人残血，去回血
			posStruct nextPos = { -1,-1 };
			std::pair<int, posStruct>bfsResult;
			bfsResult = bfs(myPos, gameState.posStudent[curetarget], api);
			nextPos = bfsResult.second;
			api.Print(fmt::format("help nxtPos {} {}",nextPos.x,nextPos.y)); 
			if (!gameState.atPosition(myPos, gameState.posStudent[curetarget])) {//如果还没到救人位置
				if (gameState.isWindow(nextPos))
					api.SkipWindow();
				Action::Move(nextPos, api);
				lastNextPos = nextPos;
				lastMove = Action::posToDirection(myPos, nextPos);
				api.Print(fmt::format("moving to cure {}", curetarget));
			}

			}
			else if (!gameState.readyToGraduate())
			{
				//还没达到毕业要求
				//bfs搜教室
				auto tmp = bfs(myPos, THUAI6::PlaceType::ClassRoom, api);
				auto nextPos = tmp.first;
				auto classroomPos = tmp.second;
				api.Print(fmt::format("Classroom nextPos {} {}", nextPos.x, nextPos.y));
                api.Print(fmt::format("cls pos {} {}",classroomPos.x,classroomPos.y));
				if (nextPos.x == -1)
					return;
				if (!gameState.atPosition(myPos, classroomPos))
				{
					//还没到达教室,前往教室
					//如果是窗，就翻窗
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
				}
				else
				{
					//到达教室，开始学习
					api.Print("reach classroom");
					if (gameState.isClassroomFinished(classroomPos, api))
					{
						//如果学完了,就把这个教室从教室列表中删掉
						api.Print("finish classroom");
						gameState.deleteStudiedClassroom(classroomPos);
						Communicate::sendClassroonPos(api, this->playerID, classroomPos);
					}
					else
					{
					//	//如果没学完，就在教室学
					//	api.UseSkill(0);
						api.StartLearning();
					}
				}
			}
			else
			{
				//达到毕业要求，前往校门处开门
				api.Print("ready to graduate");
				auto tmp = bfs(myPos, THUAI6::PlaceType::Gate, api);
				auto nextPos = tmp.first;
				auto gatePos = tmp.second;
				api.Print(fmt::format("Gate nextPos {} {}", nextPos.x, nextPos.y));
				if (nextPos.x == -1)
					return;
				if (!gameState.atPosition(myPos, gatePos))
				{
					//还没到达大门,前往大门
					//如果是窗，就翻窗
					if (gameState.isWindow(nextPos))
						api.SkipWindow();
					Action::Move(nextPos, api);
					lastNextPos = nextPos;
					lastMove = Action::posToDirection(myPos, nextPos);
				}
				else
				{
					//到达大门，开始开门
					//api.Print("reach gate");
					if (gameState.isGateFinished(gatePos, api))
					{
						//如果开门了,就毕业
						//api.Print("gate opened");
						api.Graduate();
					}
					else
					{
						//如果没开好，就开门
						api.StartOpenGate();
					}
				}
			}
		}
		else
		{
			//如果不在完整格点内，就先移动到格点
			Action::isAtCellFalseCnt++;
			Action::Move(lastNextPos, api);
			api.Print(fmt::format("moving to complete gedian"));
		}
}
}
void AI::play(ITrickerAPI& api) {
	api.UseSkill(0);
}