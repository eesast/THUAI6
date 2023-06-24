#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"

#include <cmath>
#include <queue>
#include <chrono>
#include <thread>

const double pi = acos(-1.0);

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义
using namespace THUAI6;
using namespace std;

// 选手的学生职业
extern const std::array<THUAI6::StudentType, 4> studentType = {
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::StraightAStudent,
	THUAI6::StudentType::Teacher,
	THUAI6::StudentType::StraightAStudent };

// 选手的捣蛋鬼职业
extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 映射表，用于将职业映射为最大沉迷度
inline std::map<StudentType, int> maxAddiction{
	{StudentType::StraightAStudent, Constants::StraightAStudent::maxAddiction},
	{StudentType::Sunshine, Constants::Sunshine::maxAddiction},
	{StudentType::Athlete, Constants::Athlete::maxAddiction},
	{StudentType::Teacher, Constants::Teacher::maxAddiction}
};

// 映射表，用于将职业映射为翻窗速度
inline std::map<StudentType, int> speedClimbing{
	{StudentType::StraightAStudent, Constants::StraightAStudent::speedOfClimbingThroughWindows},
	{StudentType::Sunshine, Constants::Sunshine::speedOfClimbingThroughWindows},
	{StudentType::Athlete, Constants::Athlete::speedOfClimbingThroughWindows},
	{StudentType::Teacher, Constants::Teacher::speedOfClimbingThroughWindows}
};

// 映射表，用于将职业映射为翻窗速度
inline std::map<TrickerType, int> trickerspeedClimbing{
	{TrickerType::Assassin , Constants::Assassin::speedOfClimbingThroughWindows }
};
inline std::map<TrickerType, int> TrickerSpeed{
	{TrickerType::Assassin , Constants::Assassin::moveSpeed },
	{TrickerType::Klee , Constants::Klee::moveSpeed },
	{TrickerType::ANoisyPerson , Constants::ANoisyPerson::moveSpeed },
	{TrickerType::Idol , Constants::Idol::moveSpeed }
};

inline std::map<StudentType, int>StudentAlertRadius{
	{StudentType::StraightAStudent , Constants::StraightAStudent::alertnessRadius},
	{StudentType::Sunshine , Constants::Sunshine::alertnessRadius},
	{StudentType::Athlete , Constants::Athlete::alertnessRadius},
	{StudentType::Teacher , Constants::Teacher::alertnessRadius}
};

inline std::map<StudentType, int>StudentSpeed{
	{StudentType::StraightAStudent , Constants::StraightAStudent::moveSpeed},
	{StudentType::Sunshine , Constants::Sunshine::moveSpeed},
	{StudentType::Athlete , Constants::Athlete::moveSpeed},
	{StudentType::Teacher , Constants::Teacher::moveSpeed}
};


//映射表，用于将子弹类型映射为子弹速度，攻击范围，前摇时间,攻击力，用tuple存储
inline std::map<BulletType, tuple<double, double, double, double>> bulletInfo{
	{BulletType::CommonAttackOfTricker , make_tuple(Constants::CommonAttackOfTricker::Speed,
		Constants::CommonAttackOfTricker::BulletAttackRange,
		Constants::CommonAttackOfTricker::CastTime,
		Constants::CommonAttackOfTricker::ap)},
	{BulletType::FlyingKnife , make_tuple(Constants::FlyingKnife::Speed,
		Constants::FlyingKnife::BulletAttackRange,
		Constants::FlyingKnife::CastTime,
		Constants::FlyingKnife::ap)},
	{BulletType::BombBomb , make_tuple(Constants::BombBomb::Speed,
		Constants::BombBomb::BulletAttackRange,
		Constants::BombBomb::CastTime,
		Constants::BombBomb::ap)}
};


struct PKT48 {
	TrickerType TheTrickerType = TrickerType::ANoisyPerson;
	vector<vector<PlaceType> >mp;
	vector<vector<int> >vis, exi;
	vector<vector<long long> >upt;
	vector<pair<double, double> >bb;//bombbomb
	vector<pair<int, int> >adr = vector<pair<int, int> >(4, pair<int, int>(-1, -1)), pos = adr, cls, gate;
	pair<int, int>now = pair<int, int>(-1, -1), lstpos = now, lstgrid = now, tripos = now;
	vector<pair<int, int> >line;
	vector<pair<int, int> >Grassland, Door_and_Window;
	//dtm
	int linecnt, lstgridcnt, svable[4];
	long long lstmvtime;
	double lstdir;
	long long getTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	void addExist(pair<int, int>p, int r, int val)
	{
		if (p.first == p.second && p.first == -1)return;
		if (p.first <= 0 || p.second <= 0 || p.first / 1000 >= mp.size() || p.second / 1000 >= mp.size())
			return;
		int x = p.first / 1000, y = p.second / 1000;
		if (x <= 0 || y <= 0 || x / 1000 >= mp.size() || y / 1000 >= mp[0].size())
			return;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if (sqrt((p.first - (x + i) * 1000 - 500) * (p.first - (x + i) * 1000 - 500)
					+ (p.second - (y + j) * 1000 - 500) * (p.second - (y + j) * 1000 - 500)) <= r + 410)
					exi[x + i][y + j] += val;
		return;
	}
	bool checkStandableCell(pair<int, int>p)
	{
		if ((mp[p.first][p.second] != PlaceType::Land &&
			mp[p.first][p.second] != PlaceType::Grass &&
			mp[p.first][p.second] != PlaceType::Door3 &&
			mp[p.first][p.second] != PlaceType::Door5 &&
			mp[p.first][p.second] != PlaceType::Door6) ||
			exi[p.first][p.second])
			return false;
		return true;
	}
	double getDistance(pair<int, int>x, pair<int, int>y)
	{
		return sqrt(1.0 * (x.first - y.first) * (x.first - y.first) +
			1.0 * (x.second - y.second) * (x.second - y.second));
	}
	bool checkStandableGrid(pair<int, int>p, IStudentAPI& api)
	{
		int x = p.first / 1000, y = p.second / 1000;
		/*for (auto i : api.GetStudents())
			if (getDistance(p, make_pair(i->x, i->y)) < 1000)
				return false;
		for (auto i : api.GetTrickers())
			if (getDistance(p, make_pair(i->x, i->y)) < 1000)
				return false;*/
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)if (i | j)
				if (!checkStandableCell(make_pair(x + i, y + j)))
				{
					int xx = x + i, yy = y + j;
					//cout << x << ' ' << y << ' ' << xx << ' ' << yy << endl;
					for (int a = 0; a <= 1; a++)
						for (int b = 0; b <= 1; b++)
							if (getDistance(p, make_pair((xx + a) * 1000, (yy + b) * 1000)) < 450)
								return false;
					if (xx * 1000 <= p.first && p.first <= xx * 1000 + 1000)
						if (abs(p.second - yy * 1000 - 500) < 910)
							return false;
					if (yy * 1000 <= p.second && p.second <= yy * 1000 + 1000)
						if (abs(p.first - xx * 1000 - 500) < 910)
							return false;
				}
		return true;
	}
	inline bool haveWay(double x, double y, double newX, double newY, IStudentAPI& api)
	{
		//cout << "haveWay" << ' ' << x << ' ' << y << ' ' << newX << ' ' << newY << endl;
		double divide = max(abs(newX - x), abs(newY - y)) / 10.0;
		if (divide == 0)
			return true;
		double dx = (newX - x) / divide, dy = (newY - y) / divide;
		double nowx = double(x), nowy = double(y);
		for (int i = 0; i < divide; i++)
		{
			nowx += dx;
			nowy += dy;
			if (!checkStandableGrid(make_pair(nowx, nowy), api))
				return false;
		}
		return true;
	}
	void updateMap(IStudentAPI& api)
	{
		vector<vector<PlaceType> >p = api.GetFullMap();
		if (mp.empty())
		{
			mp = p;
			vis = vector<vector<int> >(p.size(), vector<int>(p[0].size(), -1));
			exi = vector<vector<int> >(p.size(), vector<int>(p[0].size(), 0));
			upt = vector<vector<long long> >(p.size(), vector<long long>(p[0].size(), 0));
			for (int i = p.size() - 1; i >= 0; i--)
				for (int j = p[0].size() - 1; j >= 0; j--)
				{
					if (p[i][j] == PlaceType::ClassRoom)
						cls.push_back(make_pair(i, j));
					if (p[i][j] == PlaceType::Gate)
						gate.push_back(make_pair(i, j));
					if (p[i][j] == PlaceType::Door3 ||
						p[i][j] == PlaceType::Door5 ||
						p[i][j] == PlaceType::Door6)
						vis[i][j] = 0,
						Door_and_Window.push_back(make_pair(i, j));
					if (p[i][j] == PlaceType::Window)
						Door_and_Window.push_back(make_pair(i, j));
					if (p[i][j] == PlaceType::Grass)
						Grassland.push_back(make_pair(i, j));
				}
		}
		for (int i = p.size() - 1; i >= 0; i--)
			for (int j = p[0].size() - 1; j >= 0; j--)
			{
				if (mp[i][j] != p[i][j])mp[i][j] = p[i][j];
				if (!api.HaveView(1000 * i + 500, 1000 * j + 500))continue;
				if (p[i][j] == PlaceType::ClassRoom)
				{
					if (upt[i][j] >= getTime() - 200)continue;
					vis[i][j] = api.GetClassroomProgress(i, j);
					if (vis[i][j] != 1e7 && upt[i][j] >= getTime() - 3000)continue;
					upt[i][j] = getTime();
					for (int k = 0; k <= 3; k++)
						if (k != api.GetSelfInfo()->playerID)
							api.SendTextMessage(k, "5 " + to_string(i) + ' ' + to_string(j)
								+ ' ' + to_string(api.GetClassroomProgress(i, j)));

				}
				if (p[i][j] == PlaceType::Door3 ||
					p[i][j] == PlaceType::Door5 ||
					p[i][j] == PlaceType::Door6)
				{
					if (upt[i][j] >= getTime() - 200)continue;
					upt[i][j] = getTime();
					if (vis[i][j] != api.IsDoorOpen(i, j))
					{
						exi[i][j] += api.IsDoorOpen(i, j) - vis[i][j];
						vis[i][j] = api.IsDoorOpen(i, j);
						for (int k = 0; k <= 3; k++)
							if (k != api.GetSelfInfo()->playerID)
								api.SendTextMessage(k, "6 " + to_string(i) + ' ' + to_string(j)
									+ ' ' + to_string((int)api.IsDoorOpen(i, j)));
					}
				}
				if (p[i][j] == PlaceType::Gate)
				{
					if (vis[i][j] < 18000)
					{
						if (upt[i][j] >= getTime() - 200)continue;
						vis[i][j] = api.GetGateProgress(i, j);
						if (vis[i][j] != 18000 && upt[i][j] >= getTime() - 3000)continue;
						upt[i][j] = getTime();
						for (int k = 0; k <= 3; k++)
							if (k != api.GetSelfInfo()->playerID)
								api.SendTextMessage(k, "6 " + to_string(i) + ' ' + to_string(j)
									+ ' ' + to_string(api.GetGateProgress(i, j)));
					}
				}
			}

		for (auto i : api.GetStudents())
			if (api.GetSelfInfo()->playerID != i->playerID)
			{
				addExist(pos[i->playerID], 400, -1);
				pos[i->playerID] = make_pair(i->x, i->y);
				addExist(pos[i->playerID], 400, 1);
			}

		static pair<int, int>tri;
		if (!api.GetTrickers().empty())
		{
			if (TrickerType::ANoisyPerson != api.GetTrickers()[0]->trickerType && api.GetTrickers()[0]->trickerType != TheTrickerType) {
				if (api.GetTrickers()[0]->trickerType != TrickerType::ANoisyPerson)
					for (int k = 0; k <= 3; k++)
						api.SendTextMessage(k, "114514");
				TheTrickerType = api.GetTrickers()[0]->trickerType;
			}
			addExist(tri, 400, -1);
			tripos = make_pair(api.GetTrickers()[0]->x, api.GetTrickers()[0]->y);
			tri = tripos;
			addExist(tri, 400, 1);
			if (upt[0][1] < getTime() - 900)
			{
				upt[0][1] = getTime();
				for (int k = 0; k <= 3; k++)
					if (k != api.GetSelfInfo()->playerID)
						api.SendTextMessage(k, "7 " + to_string(tripos.first) + ' '
							+ to_string(tripos.second) + ' ' + to_string(upt[0][1]));
			}
		}
		else {
			addExist(tri, 400, -1);
			tri = make_pair(-1, -1);
		}
	}
	long long readMessage(int& p, string& s)
	{
		long long res = 0, f = 1;
		while (p < s.size() && (s[p] < '0' || s[p]>'9'))
			if (s[p++] == '-')f = -1;
		while (p < s.size() && (s[p] >= '0' && s[p] <= '9'))
			res = res * 10 + s[p++] - '0';
		return res * f;
	}
	void changeAddr(IStudentAPI& api, pair<int, int>p)
	{
		if (p == adr[api.GetSelfInfo()->playerID])return;
		if (p != make_pair(-1, -1))
			for (int i = 0; i <= 3; i++)
				if (i != api.GetSelfInfo()->playerID)
					api.SendTextMessage(i, "1 " + to_string(p.first) + ' ' + to_string(p.second));
		adr[api.GetSelfInfo()->playerID] = p;
		//api.Print("Change " + to_string(p.first) + ' ' + to_string(p.second));
		now = make_pair(-1, -1);
		linecnt = 0;
		line.clear();
	}
	void updateMessage(IStudentAPI& api)//消息格式：首个数字+后续
	{
		//api.Print("ERROR?-1");
		//return;
		while (api.HaveMessage())
		{
			//api.Print("ERROR?0");
			auto m = api.GetMessage();
			//api.Print("ERROR?1");
			auto a = m.first; auto b = m.second; int p = 0;
			//api.Print(to_string(a) + ' ' + b);
			long long x, y, z;
			switch (readMessage(p, b))
			{
			case 1://目标变更
				//cout << api.GetSelfInfo()->playerID << ' ' << a << ' ' << b << endl;
				x = readMessage(p, b), y = readMessage(p, b);
				adr[a] = make_pair(x, y);
				break;
			case 2://位置改变
				x = readMessage(p, b), y = readMessage(p, b);
				addExist(pos[a], 400, -1);
				pos[a] = make_pair(x, y);
				addExist(pos[a], 400, 1);
				break;
			case 3://教学楼门状态更新
				x = readMessage(p, b), y = readMessage(p, b), z = readMessage(p, b);
				exi[x][y] += z - vis[x][y];
				vis[x][y] = z;
				break;
			case 4://箱子状态更新
				x = readMessage(p, b), y = readMessage(p, b), z = readMessage(p, b);
				vis[x][y] = z;
				break;
			case 5://教室状态更新
				x = readMessage(p, b), y = readMessage(p, b), z = readMessage(p, b);
				vis[x][y] = z;
				break;
			case 6://校门状态更新
				x = readMessage(p, b), y = readMessage(p, b), z = readMessage(p, b);
				vis[x][y] = z;
				break;
			case 7://捣蛋鬼状态更新
				x = readMessage(p, b), y = readMessage(p, b), z = readMessage(p, b);
				addExist(tripos, 400, -1);
				tripos = make_pair(x, y);
				upt[0][1] = z;
				addExist(tripos, 400, 1);
				break;
			case 8://视野范围内有捣蛋鬼则暂时不救
				svable[a] = readMessage(p, b);
				break;
			case 999://毕业
				adr[a] = make_pair(-1, -1);
				addExist(pos[a], 400, -1);
				break;
			case 114514: // Anoisy
				TheTrickerType = TrickerType::Assassin;
				break;
			}
		}
		//api.Print("ERROR?2");
	}
	bool checkMap(int a, int b)
	{
		if (mp[a][b] != PlaceType::Land ||
			mp[a][b] != PlaceType::Grass)
			return false;
		else return true;
	}
	pair<int, int>findRecent(double x, double y)
	{
		double dis = 1e18;
		int px = -1, py = -1;
		for (int i = mp.size() - 1; i >= 0; i--)
			for (int j = mp[0].size() - 1; j >= 0; j--)
				if (checkStandableCell(make_pair(i, j)))
				{
					double check =
						(x - i * 1000 - 500) * (x - i * 1000 - 500) +
						(y - j * 1000 - 500) * (y - j * 1000 - 500);
					if (check < dis)
					{
						px = i; py = j;
						dis = check;
					}
				}
		return make_pair(px, py);
	}
	vector<vector<int> >dis, lst, ThroughDoors;
	vector<vector<int> >dis2, ThroughDoors2;
	queue<pair<int, int> >q;
	int checkSidable(pair<int, int>p)//不可到达返回-1否则返回九宫格距离
	{
		int d = 1e9;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if (dis[p.first + i][p.second + j] != -1)
					d = min(d, dis[p.first + i][p.second + j]);
		if (d == 1e9) return -1;
		else return d;
	}
	int checkSidable2(pair<int, int>p)//不可到达返回-1否则返回九宫格距离
	{
		int d = 1e9;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if (dis2[p.first + i][p.second + j] != -1)
					d = min(d, dis[p.first + i][p.second + j]);
		if (d == 1e9) return -1;
		else return d;
	}
	long long BFS(int p, pair<int, int>s, pair<int, int>t, IStudentAPI& api, int avoidtag = 0)
	{
		long long t1 = getTime();
		bool is = 0;
		dis = lst = ThroughDoors = vector<vector<int> >(mp.size(), vector<int>(mp[0].size(), -1));
		dis[s.first][s.second] = 0, ThroughDoors[s.first][s.second] = 0;
		while (!q.empty())
			q.pop();
		q.push(make_pair(s.first, s.second));
		bool canstand = 0;
		if (t != make_pair(-1, -1) && checkStandableCell(t)) canstand = 1;
		while (!q.empty())
		{
			pair<int, int>p = q.front(); q.pop();
			int x = p.first, y = p.second;
			if (mp[x][y] == PlaceType::Door3 || mp[x][y] == PlaceType::Door5 || mp[x][y] == PlaceType::Door6 || mp[x][y] == PlaceType::Window)
				ThroughDoors[x][y] ++;
			if (avoidtag == 1 && ThroughDoors2[x][y] == 0 && dis2[x][y] <= 15) {
				ThroughDoors[x][y] = dis[x][y] = lst[x][y] = -1;
				continue;
			}
			if (avoidtag == -1 && (getTime() - upt[0][1] < 8000 && (ThroughDoors2[x][y] == 0 && dis2[x][y] < min(11, dis2[s.first][s.second])))) {
				ThroughDoors[x][y] = dis[x][y] = lst[x][y] = -1;
				continue;
			}
			if (abs(x - t.first) <= 1 && abs(y - t.second) <= 1 && canstand == 0 && checkStandableCell(make_pair(x, y)))
			{
				t = make_pair(x, y);
				is = 1;
				break;
			}
			if (abs(x - t.first) + abs(y - t.second) <= 0)
			{
				t = make_pair(x, y);
				is = 1;
				break;
			}
			for (int i = 0; i <= 7; i++)if (!(i & 1))
			{
				int a = 0, b = 0;
				if (i == 0 || i == 1 || i == 7)a = 1;
				if (i == 3 || i == 4 || i == 5)a = -1;
				if (i == 1 || i == 2 || i == 3)b = 1;
				if (i == 5 || i == 6 || i == 7)b = -1;
				if (checkStandableCell(make_pair(x + a, y + b))
					|| mp[x + a][y + b] == PlaceType::Window)
				{
					if (dis[x + a][y + b] == -1)
					{
						dis[x + a][y + b] = dis[x][y] + 1;
						ThroughDoors[x + a][y + b] = ThroughDoors[x][y];
						q.push(make_pair(x + a, y + b));
						lst[x + a][y + b] = i;
					}
				}
			}
		}
		line.clear();
		if (t != make_pair(-1, -1))
		{
			if (!is)
				return getTime() - t1;
			for (int x = t.first, y = t.second; x != s.first || y != s.second;) // line 从前往后 为终点到起点的路径
			{
				if (mp[x][y] != PlaceType::Window)
					line.push_back(make_pair(x, y));
				int i = lst[x][y], a = 0, b = 0;
				if (i == 0 || i == 1 || i == 7)a = 1;
				if (i == 3 || i == 4 || i == 5)a = -1;
				if (i == 1 || i == 2 || i == 3)b = 1;
				if (i == 5 || i == 6 || i == 7)b = -1;
				x -= a; y -= b;
			}

			if (!line.empty() && abs(line.back().first - s.first) + abs(line.back().second - s.second) == 2)
			{
				;
			}
			else
				line.push_back(make_pair(s.first, s.second));
		}
		return getTime() - t1;
	}
	long long BFS2(int p, pair<int, int>s, pair<int, int>t, IStudentAPI& api)
	{
		long long t1 = getTime();
		bool is = 0;
		dis2 = ThroughDoors2 = vector<vector<int> >(mp.size(), vector<int>(mp[0].size(), -1));
		dis2[s.first][s.second] = ThroughDoors2[s.first][s.second] = 0;
		while (!q.empty())
			q.pop();
		q.push(make_pair(s.first, s.second));
		while (!q.empty())
		{
			pair<int, int>p = q.front(); q.pop();
			int x = p.first, y = p.second;
			if (mp[x][y] == PlaceType::Door3 || mp[x][y] == PlaceType::Door5 || mp[x][y] == PlaceType::Door6 || mp[x][y] == PlaceType::Window)
				ThroughDoors2[x][y] ++;
			for (int i = 0; i <= 7; i++)if (!(i & 1))
			{
				int a = 0, b = 0;
				if (i == 0 || i == 1 || i == 7)a = 1;
				if (i == 3 || i == 4 || i == 5)a = -1;
				if (i == 1 || i == 2 || i == 3)b = 1;
				if (i == 5 || i == 6 || i == 7)b = -1;
				if (checkStandableCell(make_pair(x + a, y + b))
					|| mp[x + a][y + b] == PlaceType::Window)
				{
					if (dis2[x + a][y + b] == -1)
					{
						ThroughDoors2[x + a][y + b] = ThroughDoors2[x][y];
						dis2[x + a][y + b] = dis2[x][y] + 1;
						q.push(make_pair(x + a, y + b));
					}
				}
			}
		}
		return getTime() - t1;
	}
}tts;

void AI::play(IStudentAPI& api)
{
	if (api.GetSelfInfo()->playerState == PlayerState::Graduated)return;
	auto self = api.GetSelfInfo();
	int id = this->playerID;

	tts.updateMap(api);
	tts.updateMessage(api);

	int x = api.GetSelfInfo()->x / 1000, y = api.GetSelfInfo()->y / 1000;

	//无法操作时，直接返回
	if (self->playerState == PlayerState::Attacking || self->playerState == PlayerState::Swinging
		|| self->playerState == PlayerState::Stunned || self->playerState == PlayerState::Climbing
		|| self->playerState == PlayerState::Addicted) {
		tts.lstmvtime = tts.getTime();
		return;
	}
	//救人的简单判定

	//nextaddction()
	auto nextaddction = [&](int x) {
		return x <= 78000 / 3 ? 78000 / 3 : x <= 78000 * 2 / 3 ? 78000 * 2 / 3 : 78000;
	};

	if (!api.GetStudents().empty()) {
		for (auto i : api.GetStudents())
			if (i->playerID != id)
				if (i->determination == 0) {
					if (abs(i->x / 1000 - x) <= 1 && abs(i->y / 1000 - y) <= 1)
					{
						if (i->addiction < nextaddction(i->addiction) - 3000 &&
							Constants::Teacher::alertnessRadius <= 8000 * (tts.TheTrickerType == TrickerType::ANoisyPerson ? 0 : self->dangerAlert))
							continue;
						api.StartRouseMate(i->playerID);
						this_thread::sleep_for(chrono::milliseconds(1050));
						return;
					}
				}
	}

	static int priority = 0x3f3f3f3f;
	int avoidtag = -1;

	if (tts.getTime() - tts.upt[0][1] <= 1000) {
		tts.BFS2(id, make_pair(api.GridToCell(tts.tripos.first), api.GridToCell(tts.tripos.second)), tts.adr[id], api);
	}

	//卡人的简单判定
	for (auto& s : api.GetTrickers()) {
		if (1ll * (s->x - self->x) * (s->x - self->x) + 1ll * (s->y - self->y) * (s->y - self->y) <= 810 * 810) {
			api.Move(50, atan2(s->y - self->y, s->x - self->x) + pi);
			this_thread::sleep_for(chrono::milliseconds(50));
			return;
		}
	}

	for (auto& s : api.GetStudents()) if (s->playerID != self->playerID) {
		if (1ll * (s->x - self->x) * (s->x - self->x) + 1ll * (s->y - self->y) * (s->y - self->y) <= 810 * 810) {
			api.Move(50, atan2(s->y - self->y, s->x - self->x) + pi);

			this_thread::sleep_for(chrono::milliseconds(50));
			return;
		}
	}



	if (api.GetSelfInfo()->studentType == StudentType::Teacher) {
		//输出tts.tripos.first , tts.tripos.second
		//api.Print(to_string(tts.tripos.first) + ' ' + to_string(tts.tripos.second));
		//输出tts.upt[0][1] , tts.getTime()
		//api.Print(fmt::format("time = {}, upt = {}", tts.getTime(), tts.upt[0][1]));


		static long long kjsj = tts.getTime();
		if (tts.getTime() == kjsj)
			api.UseSkill(1, 0);

		if (!api.GetTrickers().empty() && 1.0 * (api.GetTrickers()[0]->x - self->x) * (api.GetTrickers()[0]->x - self->x) + 1.0 * (api.GetTrickers()[0]->y - self->y) * (api.GetTrickers()[0]->y - self->y) < 5000 * 5000) {

			if (api.GetTrickers()[0]->playerState == PlayerState::Attacking) {
				api.EndAllAction();
				this_thread::sleep_for(chrono::milliseconds(5));
				double alpha = atan2(api.GetTrickers()[0]->y - self->y, api.GetTrickers()[0]->x - self->x);
				double xx = self->x + 600 * cos(alpha + pi / 2), yy = self->y + 600 * sin(alpha + pi / 2);
				//输出getTime
				if (tts.haveWay(self->x, self->y, xx, yy, api)) {
					api.Move(360, alpha + pi / 2);
					this_thread::sleep_for(chrono::milliseconds(200));
				}
				else {
					xx = self->x + 600 * cos(alpha - pi / 2), yy = self->y + 600 * sin(alpha - pi / 2);
					//if (tts.haveWay(self->x, self->y, xx, yy, api)) 
					{
						api.Move(360, alpha - pi / 2);
						this_thread::sleep_for(chrono::milliseconds(200));
					}
					//无路可逃
				}
				return;
			}
		}

		//考虑对抗
		if (!api.GetTrickers().empty() && 1.0 * (api.GetTrickers()[0]->x - self->x) * (api.GetTrickers()[0]->x - self->x) + 1.0 * (api.GetTrickers()[0]->y - self->y) * (api.GetTrickers()[0]->y - self->y) < 3000 * 3000) {
			if (api.GetTrickers()[0]->playerState == PlayerState::Attacking || api.GetTrickers()[0]->playerState == PlayerState::Swinging
				|| api.GetTrickers()[0]->playerState == PlayerState::Climbing || api.GetTrickers()[0]->playerState == PlayerState::UsingSpecialSkill) {
				if (api.GetSelfInfo()->timeUntilSkillAvailable[0] == 0)
					api.UseSkill(0);
			}
		}


		for (auto i : api.GetStudents())
			if (i->determination == 0 && i->playerState != PlayerState::Quit)
			{
				priority = 0;
				tts.changeAddr(api, make_pair(api.GridToCell(i->x), api.GridToCell(i->y)));
				tts.BFS(id, make_pair(x, y), tts.adr[id], api);
				goto getsettoroll;
			}

		//保持距离
		if (!api.GetTrickers().empty() && api.GetTrickers()[0]->trickerType == TrickerType::Klee) {
			if (1.0 * (api.GetTrickers()[0]->x - self->x) * (api.GetTrickers()[0]->x - self->x) + (api.GetTrickers()[0]->y - self->y) * (api.GetTrickers()[0]->y - self->y) < 3000 * 3000)
			{
				double theta = atan2(api.GetTrickers()[0]->y - self->y, api.GetTrickers()[0]->x - self->x) + pi;
				for (int i = 0; i <= 5; i++) {
					double alpha = theta + i * pi / 5;
					double xx = api.GetTrickers()[0]->x + 4000 * cos(alpha), yy = api.GetTrickers()[0]->y + 4000 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
					alpha = theta - i * pi / 5;
					xx = api.GetTrickers()[0]->x + 4000 * cos(alpha), yy = api.GetTrickers()[0]->y + 4000 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
				}
			}
		}
		if (self->determination > 25000000) { // 可以靠近

		}
		else if (!api.GetTrickers().empty() && self->determination > 20000000) { // 老师要和坏学生保持距离
			/*double d = api.GetTrickers()[0]->facingDirection + atan2(api.GetTrickers()[0]->y - self->y, api.GetTrickers()[0]->x - self->x);
			d = fmod(d, 2 * pi);
			if (d > pi) d -= 2 * pi;
			api.Print(fmt :: format("api.GetTrickers()[0]->facingDirection = {} , d = {}", api.GetTrickers()[0]->facingDirection , d));
			if (fabs(d) < pi * 0.1)//对上眼了*/ // tmd射击不改变facingdirection
			if (1.0 * (api.GetTrickers()[0]->x - self->x) * (api.GetTrickers()[0]->x - self->x) + (api.GetTrickers()[0]->y - self->y) * (api.GetTrickers()[0]->y - self->y) < 2000 * 2000)
			{
				double theta = atan2(api.GetTrickers()[0]->y - self->y, api.GetTrickers()[0]->x - self->x) + pi;
				for (int i = 0; i <= 30; i++) {
					double alpha = theta + i * pi / 30;
					double xx = api.GetTrickers()[0]->x + 2500 * cos(alpha), yy = api.GetTrickers()[0]->y + 2500 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
					alpha = theta - i * pi / 30;
					xx = api.GetTrickers()[0]->x + 2100 * cos(alpha), yy = api.GetTrickers()[0]->y + 2100 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
				}
				// 无路可逃？
			}
		}
		else if (!api.GetTrickers().empty() && self->determination <= 20000000) {
			if (1.0 * (api.GetTrickers()[0]->x - self->x) * (api.GetTrickers()[0]->x - self->x) + (api.GetTrickers()[0]->y - self->y) * (api.GetTrickers()[0]->y - self->y) < 2500 * 2500)
			{
				double theta = atan2(api.GetTrickers()[0]->y - self->y, api.GetTrickers()[0]->x - self->x) + pi;
				for (int i = 0; i <= 5; i++) {
					double alpha = theta + i * pi / 5;
					double xx = api.GetTrickers()[0]->x + 3000 * cos(alpha), yy = api.GetTrickers()[0]->y + 3000 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
					alpha = theta - i * pi / 5;
					xx = api.GetTrickers()[0]->x + 3000 * cos(alpha), yy = api.GetTrickers()[0]->y + 3000 * sin(alpha);
					if (tts.haveWay(self->x, self->y, xx, yy, api)) {
						int t = 1000 * sqrt((xx - self->x) * (xx - self->x) + (yy - self->y) * (yy - self->y)) / self->speed;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(5));
						api.Move(t, alpha);
						return;
					}
				}
			}
		}



		if (!api.GetTrickers().empty()) {
			priority = 1;
			tts.changeAddr(api, make_pair(api.GridToCell(api.GetTrickers()[0]->x), api.GridToCell(api.GetTrickers()[0]->y)));
			tts.BFS(id, make_pair(x, y), tts.adr[id], api);
			goto getsettoroll;
		}

		if (tts.getTime() - tts.upt[0][1] <= 1000) {
			priority = 2;
			tts.changeAddr(api, make_pair(api.GridToCell(tts.tripos.first), api.GridToCell(tts.tripos.second)));
			tts.BFS(id, make_pair(x, y), tts.adr[id], api);
			goto getsettoroll;
		}

		if (tts.getTime() - tts.upt[0][1] <= 10000) {
			priority = 3;
			tts.changeAddr(api, make_pair(api.GridToCell(tts.tripos.first), api.GridToCell(tts.tripos.second)));
			tts.BFS(id, make_pair(x, y), tts.adr[id], api);
			goto getsettoroll;
		}
		if (0) { // 准备写的 ， 某个学霸听到在附近，请求教师协助。

		}
		goto getsettoroll;
	}

	/*
	if (api.GetSelfInfo()->studentType == StudentType::Athlete)
	{
		api.Print("check!" + to_string(api.GetSelfInfo()->timeUntilSkillAvailable[0]));
		if (!api.GetTrickers().empty() && api.GetTrickers()[0]->playerState != PlayerState::Stunned)
		{
			int ch = id;
			long long mn = 1e18;
			tts.updateMessage(api);
			for (auto i : api.GetStudents())
				if (i->studentType == StudentType::Athlete && i->determination > 0)
					if (i->timeUntilSkillAvailable[0] > 57000)
					{
						mn = -1;
						ch = i->playerID;
					}
					else if (!i->timeUntilSkillAvailable[0])
					{
						long long tx = api.GetTrickers()[0]->x - i->x;
						long long ty = api.GetTrickers()[0]->y - i->y;
						if (tx * tx + ty * ty < mn)
						{
							mn = tx * tx + ty * ty;
							ch = i->playerID;
						}
					}
			int skt = api.GetSelfInfo()->timeUntilSkillAvailable[0];
			if (ch == id && (!skt || skt > 58000))
			{
				api.Print("BRAVE!");
				tts.addExist(tts.tripos, 500, -1);
				if (tts.haveWay(api.GetSelfInfo()->x, api.GetSelfInfo()->y,
					api.GetTrickers()[0]->x, api.GetTrickers()[0]->y, api))
				{
					tts.addExist(tts.tripos, 500, 1);
					long long tx = api.GetTrickers()[0]->x - api.GetSelfInfo()->x;
					long long ty = api.GetTrickers()[0]->y - api.GetSelfInfo()->y;
					int t = 1e3 * sqrt(tx * tx + ty * ty) / api.GetSelfInfo()->speed;
					if (!skt && t < 600) { api.UseSkill(0); api.Print("SKILL!"); }
					if (skt > 57000)t /= 3;
					t = min(t, 50);
					api.Move(t, tts.lstdir = (ty > 0 ? acos(tx / sqrt(tx * tx + ty * ty)) :
						2 * acos(-1) - acos(tx / sqrt(tx * tx + ty * ty))));
					//tts.BFS(id, make_pair(x, y), tts.adr[id], api);
					return;
				}
				else
				{
					tts.addExist(tts.tripos, 500, 1);
					if (tts.adr[id] != make_pair(tts.tripos.first / 1000, tts.tripos.second / 1000))
					{
						tts.changeAddr(api, make_pair(tts.tripos.first / 1000, tts.tripos.second / 1000));
						tts.BFS(id, make_pair(x, y), tts.adr[id], api);
					}
				}
			}
		}
	}
	*/

	/*for (auto i : api.GetStudents())
	{
		//api.Print("STUDENT "+to_string(i->x)+' '+to)
	}
	*/

	//输出tts.tripos.first, tts.tripos.second ， tts.getTime()， tts.upt[0][1]
	//api.Print(fmt :: format("tripos = ({}, {}), time = {}, upt = {}", tts.tripos.first, tts.tripos.second, tts.getTime(), tts.upt[0][1]));


	if (priority == 1) {
		if ((tts.TheTrickerType != TrickerType::ANoisyPerson && StudentAlertRadius[self->studentType] > self->dangerAlert * 8000)
			|| (tts.getTime() - tts.upt[0][1] <= 1000 && tts.dis2[self->x / 1000][self->y / 1000] > 11))
			priority = 0x3f3f3f3f;
	}

	if (priority == 2)
	{
		for (auto i : api.GetStudents())
			if (i->studentType == StudentType::Teacher)
				if (i->determination > 0 || (i->dangerAlert && tts.TheTrickerType != TrickerType::ANoisyPerson))
					priority = 0x3f3f3f3f;
	}
	if (tts.adr[id] != make_pair(-1, -1) && priority == 3)
	{
		if (tts.vis[tts.adr[id].first][tts.adr[id].second] == 1e7)
			priority = 0x3f3f3f3f;
		if (api.GetGameInfo()->subjectFinished >= 7)
			priority = 0x3f3f3f3f;
	}

	if (priority >= 1) {
		//考虑加入tricker不动的时候
		if (0) {

		}


		if (tts.getTime() - tts.upt[0][1] <= 8000 && tts.dis2[self->x / 1000][self->y / 1000] <= 11)
		{
			tts.BFS(id, make_pair(x, y), make_pair(-1, -1), api);
			int td = 0x3f3f3f3f;
			for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++)
				if (tts.ThroughDoors[api.GridToCell(tts.tripos.first) + i][api.GridToCell(tts.tripos.second) + j] != -1)
					td = min(td, tts.ThroughDoors[api.GridToCell(tts.tripos.first) + i][api.GridToCell(tts.tripos.second) + j]);
			//符号函数
			auto sgn = [](double x) {return x < 0 ? -1 : 1; };
			if (tts.mp[self->x / 1000][self->y / 1000] == PlaceType::Grass && tts.mp[api.GridToCell(tts.tripos.first)][api.GridToCell(tts.tripos.second)] != PlaceType::Grass
				&& tts.mp[self->x / 1000 + sgn(tts.tripos.first - self->x)][self->y / 1000] != PlaceType::Grass
				&& tts.mp[self->x / 1000][self->y / 1000 + sgn(tts.tripos.second - self->y)] != PlaceType::Grass
				&& td == 0
				&& StudentAlertRadius[self->studentType] <= 4000 * (tts.TheTrickerType == TrickerType::ANoisyPerson ? 0 : self->dangerAlert)) { // 很近了

				api.EndAllAction();
				this_thread::sleep_for(chrono::milliseconds(5));
				api.Move(50, atan2(api.CellToGrid(self->y) - tts.tripos.second, api.CellToGrid(self->x) - tts.tripos.first));
				return;
			}
			else if (td == 0) {// 在同一空间 ， 寻找能够制造最多路程差的门
				double mx = -1e9;
				pair<int, int> ch = make_pair(-1, -1);
				for (auto s : tts.Door_and_Window) {
					for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) if (i == 0 || j == 0) if (i | j)
						if (tts.checkStandableCell(make_pair(s.first + i, s.second + j)) && tts.ThroughDoors[s.first + i][s.second + j] == 1 &&
							(tts.dis[s.first + i][s.second + j] <= tts.dis2[s.first + i][s.second + j] - 1 || tts.dis[s.first + i][s.second + j] > 10)) {
							double t = tts.dis[s.first + i][s.second + j] * 1000 / StudentSpeed[self->studentType];
							double ret = tts.dis2[s.first + i][s.second + j] * 1000 - t * TrickerSpeed[tts.TheTrickerType];
							if (ret > mx) {
								mx = ret;
								ch = make_pair(s.first + i, s.second + j);
							}
						}
				}
				if (ch.first != -1 && ch.second != -1) {
					priority = 1;
					tts.changeAddr(api, ch);
					tts.BFS(id, make_pair(x, y), tts.adr[id], api);
					tts.linecnt = 0;
					goto getsettoroll;
				}
				else {
					//跑不过了，躲草丛。
					tts.BFS(id, make_pair(x, y), make_pair(-1, -1), api);
					//寻找制造距离差最大的草丛
					double mx = -0x3f3f3f3f;
					pair<int, int> ch = make_pair(-1, -1);
					for (auto s : tts.Grassland)
						if (tts.dis[s.first][s.second] != -1 && tts.ThroughDoors[s.first][s.second] == 0)
						{
							double t = tts.dis[s.first][s.second] * 1000 / StudentSpeed[self->studentType];
							double ret = tts.dis2[s.first][s.second] * 1000 - t * TrickerSpeed[tts.TheTrickerType];
							bool check = 1;
							for (int j = 0; j <= 3; j++)
								if ((j != id &&
									tts.adr[j].first == s.first &&
									tts.adr[j].second == s.second))
									check = 0;
							if (check && ret > mx)
								mx = ret,
								ch = s;
						}
					if (ch != make_pair(-1, -1)) // 必成立
					{
						priority = 1;
						tts.changeAddr(api, ch);
						tts.BFS(id, make_pair(x, y), tts.adr[id], api);
						tts.linecnt = 0;
						goto getsettoroll;
					}
				}
			}
			else if (td >= 2) {
				//远着呢，不用跑，等后面重新分配目标把
			}
			else { // 和tricker 一墙之隔 , 不跑进去自投罗网就是胜利
				avoidtag = 1;
				priority = 0x3f3f3f3f;
			}
		}
		else if (StudentAlertRadius[self->studentType] <= 8000 * (tts.TheTrickerType == TrickerType::ANoisyPerson ? 0 : self->dangerAlert) && priority > 1) // 听到了但是没人看见
		{
			static double lastalert = 0, lt = 0;

			//完全没有位置信息，只能躲草丛
			tts.BFS(id, make_pair(x, y), make_pair(-1, -1), api);
			//寻找最近不穿墙的草丛
			double mn = 1e18;
			pair<int, int> ch = make_pair(-1, -1);
			for (auto s : tts.Grassland)
				if (tts.dis[s.first][s.second] < mn && tts.dis[s.first][s.second] != -1 && tts.ThroughDoors[s.first][s.second] == 0)
				{
					bool check = 1;
					for (int j = 0; j <= 3; j++)
						if ((j != id &&
							tts.adr[j].first == s.first &&
							tts.adr[j].second == s.second))
							check = 0;
					if (check && tts.dis[s.first][s.second] < mn)
						mn = tts.dis[s.first][s.second],
						ch = s;
				}
			if (ch != make_pair(-1, -1)) // 必成立
			{
				priority = 1;
				tts.changeAddr(api, ch);
				tts.BFS(id, make_pair(x, y), tts.adr[id], api);
				tts.linecnt = 0;
				goto getsettoroll;
			}
		}
	}


	//api.Print(to_string(tts.adr[id].first) + ' ' + to_string(tts.adr[id].second));



	if (priority > 2)
	{
		bool check = 0;
		int a = 0, b = 0;
		int countlife = 0;

		for (auto i : api.GetStudents())
			if (i->studentType != StudentType::Teacher && i->playerState != PlayerState::Quit && i->playerState != PlayerState::Graduated)
				countlife++;
		for (auto i : api.GetStudents())
			if (i->studentType == StudentType::Teacher)
				if (i->determination == 0 && (i->dangerAlert == 0 || tts.TheTrickerType == TrickerType::ANoisyPerson) && (
					(api.GetGameInfo()->subjectFinished < 7)
					|| countlife >= 2))
				{
					check = 1;
					a = i->x / 1000;
					b = i->y / 1000;
				}
		if (check)
		{
			int ch = 4, mn = 1e9;
			for (auto i : api.GetStudents())
				if (i->studentType != StudentType::Teacher && i->playerState != PlayerState::Addicted)
					if (mn > i->addiction || (mn == i->addiction && ch > i->playerID))
					{
						mn = i->addiction;
						ch = i->playerID;
					}
			if (ch == id)
			{
				tts.changeAddr(api, make_pair(a, b));
				tts.BFS(id, make_pair(x, y), tts.adr[id], api, avoidtag);
				tts.linecnt = 0;
				priority = 2;
				goto getsettoroll;
			}
		}
	}



	if (priority > 3) {
		{
			tts.updateMessage(api);
			tts.BFS(id, make_pair(x, y), make_pair(-1, -1), api, avoidtag);
			int ch = -1;
			double mn = 1e9;

			vector<pair<int, int> >tgt;
			if (api.GetGameInfo()->subjectFinished < 7)
				tgt = tts.cls;// 目标选择教室
			else
				tgt = tts.gate;// 目标选择门

			for (int i = tgt.size() - 1; i >= 0; i--)
			{
				bool check = (tts.checkSidable(tgt[i]) != -1) &&
					(tts.vis[tgt[i].first][tgt[i].second] < 1e7) &&
					(tts.getTime() - tts.upt[0][1] >= 8000 ||
						tts.checkSidable2(make_pair(tgt[i].first, tgt[i].second)) > 11);

				if (check && mn > tts.checkSidable(tgt[i])) //前往距离最近的目标？
				{
					mn = tts.checkSidable(tgt[i]);
					ch = i;
				}
			}
			if (ch >= 0)
			{
				tts.changeAddr(api, make_pair(tgt[ch].first, tgt[ch].second));
				tts.BFS(id, make_pair(x, y), tts.adr[id], api, avoidtag);
				tts.linecnt = 0;
				priority = 3;
			}
		}
	}


getsettoroll://准备去卷

	if (abs(api.GetSelfInfo()->x - tts.adr[id].first * 1000 - 500) > 1040 ||
		abs(api.GetSelfInfo()->y - tts.adr[id].second * 1000 - 500) > 1040 ||
		(tts.checkStandableCell(tts.adr[id]) && abs(api.GetSelfInfo()->x - tts.adr[id].first * 1000 - 500) > 200) ||
		(tts.checkStandableCell(tts.adr[id]) && abs(api.GetSelfInfo()->y - tts.adr[id].second * 1000 - 500) > 200))
	{
		if ((abs(api.GetSelfInfo()->x - tts.now.first * 1000 - 500) < 20 &&
			abs(api.GetSelfInfo()->y - tts.now.second * 1000 - 500) < 20) ||
			(!tts.haveWay(self->x, self->y, tts.now.first * 1000 + 500, tts.now.second * 1000 + 500, api)))
			tts.now = make_pair(-1, -1);

		//api.Print(to_string(tts.now.first) + ' ' + to_string(tts.now.second));

		if (tts.now == make_pair(-1, -1))
		{
			if (tts.linecnt > 0 || tts.line.empty())
			{
				tts.BFS(id, make_pair(x, y), tts.adr[id], api, self->studentType == StudentType::Teacher ? 0 : avoidtag);
				tts.linecnt = 0;
				//输出tts.line.size()
				if (tts.line.empty())
				{
					tts.now = make_pair(-1, -1);
					return;
				}
			}
			int check = -1;
			for (int i = max(0, (int)tts.line.size() - 8); i < tts.line.size() && check == -1; i++) {
				if (tts.haveWay(api.GetSelfInfo()->x, api.GetSelfInfo()->y,
					1000 * tts.line[i].first + 500,
					1000 * tts.line[i].second + 500, api))
					check = i;
			}
			tts.linecnt++;
			if (check != -1)
				while (tts.line.size() > check + 1)
					tts.line.pop_back();
			tts.now = tts.line.back();
			tts.line.pop_back();
		}
	}
	else
		tts.now = make_pair(-1, -1);


	if (tts.now != make_pair(-1, -1))
	{
		if (((abs(x - tts.now.first) == 2 && abs(y - tts.now.second) == 0) ||
			(abs(x - tts.now.first) == 0 && abs(y - tts.now.second) == 2)) &&
			tts.mp[(x + tts.now.first) / 2][(y + tts.now.second) / 2] == PlaceType::Window)
		{
			int t = 2000 * 1e3 / speedClimbing[api.GetSelfInfo()->studentType];
			api.SkipWindow();
			api.EndAllAction();
			tts.now = make_pair(-1, -1);
			this_thread::sleep_for(chrono::milliseconds(t));
		}
		else if (abs(api.GetSelfInfo()->x - tts.now.first * 1000 - 500) > 20 ||
			abs(api.GetSelfInfo()->y - tts.now.second * 1000 - 500) > 20)
		{
			int tx = tts.now.first * 1000 + 500 - api.GetSelfInfo()->x,
				ty = tts.now.second * 1000 + 500 - api.GetSelfInfo()->y;
			int t = (int)(sqrt(tx * tx + ty * ty) * 1e3 / api.GetSelfInfo()->speed);
			if (api.GetSelfInfo()->studentType == StudentType::Athlete &&
				api.GetSelfInfo()->timeUntilSkillAvailable[0] > 57000)
				t /= 3;
			if (self->studentType == StudentType::Teacher && t >= 1200 && self->timeUntilSkillAvailable[1] == 0) {
				api.EndAllAction();
				api.UseSkill(1, (int)(atan2(ty, tx) * 1000));
			}
			else {
				t = min(t, 100);
				api.Move(t, tts.lstdir = atan2(ty, tx));
			}
			return;
		}
		else tts.now = make_pair(-1, -1);
	}
	if (abs(api.GetSelfInfo()->x - tts.adr[id].first * 1000 - 500) > 1040 ||
		abs(api.GetSelfInfo()->y - tts.adr[id].second * 1000 - 500) > 1040 ||
		(tts.checkStandableCell(tts.adr[id]) && abs(api.GetSelfInfo()->x - tts.adr[id].first * 1000 - 500) > 200) ||
		(tts.checkStandableCell(tts.adr[id]) && abs(api.GetSelfInfo()->y - tts.adr[id].second * 1000 - 500) > 200)) {
		return;
	}
	else {
		if (self->playerState == PlayerState::Learning || self->playerState == PlayerState::OpeningAGate)
			return;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if ((i || j) && tts.adr[id] == make_pair(x + i, y + j))
					if (tts.mp[x + i][y + j] == PlaceType::ClassRoom)
					{
						//cout << x + i << ' ' << y + j << endl;
						//api.Print("CLSCHECK" + to_string(x + i) + ' ' + to_string(y + j));
						if (tts.vis[x + i][y + j] < 1e7 &&
							api.GetGameInfo()->subjectFinished < 7)
						{
							//cout << api.GetClassroomProgress(x + i, y + j) << endl;
							api.EndAllAction();
							this_thread::sleep_for(chrono::milliseconds(5));
							if (api.GetSelfInfo()->studentType == StudentType::StraightAStudent)
								api.UseSkill(0);
							api.StartLearning();
							return;
						}
					}
					else if (tts.mp[x + i][y + j] == PlaceType::Gate)
					{
						if (api.GetGameInfo()->subjectFinished >= 7)
						{
							api.EndAllAction();
							this_thread::sleep_for(chrono::milliseconds(5));
							if (api.GetGateProgress(x + i, y + j) < 18000)
								api.StartOpenGate();
							else {
								for (int k = 0; k <= 3; k++)
									if (k != id)
										api.SendTextMessage(k, string("999"));
								api.Graduate();
							}
							return;
						}
					}
	}
}

// 抓PKT48的捣蛋鬼结构体
struct CKT48 {
	vector<vector<PlaceType> >mp;
	vector<vector<int> >vis, exi;
	vector<vector<long long> >upt;
	vector<pair<double, double> >bb;//bombbomb
	pair<int, int>adr;
	vector<pair<int, int> > cls, gate;
	pair<int, int>now = pair<int, int>(-1, -1), lstpos = now, lstgrid = now, tripos = now;
	vector<pair<int, int> >line;
	int linecnt, lstgridcnt;
	// lstdir 上一次的方向
	double lstdir;
	// getTime 返回当前时间，单位为毫秒
	long long getTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
	}
	void addExist(pair<int, int>p, int r, int val)
	{
		if (p.first == p.second && p.first == -1)return;
		if (p.first <= 0 || p.second <= 0 || p.first / 1000 >= mp.size() || p.second / 1000 >= mp.size())
			return;
		int x = p.first / 1000, y = p.second / 1000;
		if (x <= 0 || y <= 0 || x / 1000 >= mp.size() || y / 1000 >= mp[0].size())
			return;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if (sqrt((p.first - (x + i) * 1000 - 500) * (p.first - (x + i) * 1000 - 500)
					+ (p.second - (y + j) * 1000 - 500) * (p.second - (y + j) * 1000 - 500)) <= r + 410)
					exi[x + i][y + j] += val;
		return;
	}
	bool checkStandableCell(pair<int, int>p)
	{
		if ((mp[p.first][p.second] != PlaceType::Land &&
			mp[p.first][p.second] != PlaceType::Grass &&
			mp[p.first][p.second] != PlaceType::Door3 &&
			mp[p.first][p.second] != PlaceType::Door5 &&
			mp[p.first][p.second] != PlaceType::Door6) ||
			exi[p.first][p.second])
			return false;
		return true;
	}
	bool checkStandablebulletCell(pair<int, int>p)
	{
		if ((mp[p.first][p.second] != PlaceType::Land &&
			mp[p.first][p.second] != PlaceType::Grass &&
			mp[p.first][p.second] != PlaceType::Door3 &&
			mp[p.first][p.second] != PlaceType::Door5 &&
			mp[p.first][p.second] != PlaceType::Door6 &&
			mp[p.first][p.second] != PlaceType::Window) ||
			exi[p.first][p.second])
			return false;
		return true;
	}

	double getDistance(pair<int, int>x, pair<int, int>y)
	{
		return sqrt(1.0 * (x.first - y.first) * (x.first - y.first) +
			1.0 * (x.second - y.second) * (x.second - y.second));
	}
	bool checkStandableGrid(pair<int, int>p)
	{
		int x = p.first / 1000, y = p.second / 1000;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)if (i | j)
				if (!checkStandableCell(make_pair(x + i, y + j)))
				{
					int xx = x + i, yy = y + j;
					for (int a = 0; a <= 1; a++)
						for (int b = 0; b <= 1; b++)
							if (getDistance(p, make_pair((xx + a) * 1000, (yy + b) * 1000)) < 410)
								return false;
					if (xx * 1000 <= p.first && p.first <= xx * 1000 + 1000)
						if (abs(p.second - yy * 1000 - 500) < 910)
							return false;
					if (yy * 1000 <= p.second && p.second <= yy * 1000 + 1000)
						if (abs(p.first - xx * 1000 - 500) < 910)
							return false;
				}
		return true;
	}
	inline bool haveWay(double x, double y, double newX, double newY)
	{
		double divide = max(abs(newX - x), abs(newY - y)) / 10;
		if (divide == 0)return true;
		double dx = (newX - x) / divide, dy = (newY - y) / divide;
		double nowx = double(x), nowy = double(y);
		for (int i = 0; i < divide; i++)
		{
			nowx += dx;
			nowy += dy;
			if (!checkStandableGrid(make_pair(nowx, nowy))) {
				return false;
			}
		}
		return true;
	}
	bool checkbulletStandableGrid(pair<double, double>p)
	{
		int x = int(p.first / 1000), y = int(p.second / 1000);
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)if (i | j)
				if (!checkStandablebulletCell(make_pair(x + i, y + j)))
				{
					int xx = x + i, yy = y + j;
					for (int a = 0; a <= 1; a++)
						for (int b = 0; b <= 1; b++)
							if (getDistance(p, make_pair((xx + a) * 1000, (yy + b) * 1000)) < 210)
								return false;
					if (xx * 1000 <= p.first && p.first <= xx * 1000 + 1000)
						if (abs(p.second - yy * 1000 - 500) < 710)
							return false;
					if (yy * 1000 <= p.second && p.second <= yy * 1000 + 1000)
						if (abs(p.first - xx * 1000 - 500) < 710)
							return false;
				}
		return true;
	}
	bool havebulletWay(double x, double y, double newX, double newY)
	{
		//输出一下
		//printf("x = %lf, y = %lf, newX = %lf, newY = %lf\n", x, y, newX, newY);
		double theta = atan2(newY - y, newX - x);
		x += cos(theta) * 400, y += sin(theta) * 400;
		newX -= cos(theta) * 400, newX -= sin(theta) * 400;
		double divide = max(abs(newX - x), abs(newY - y)) / 10.0;
		if (divide == 0)return true;
		double dx = (newX - x) / divide, dy = (newY - y) / divide;
		double nowx = double(x), nowy = double(y);
		for (int i = 0; i < divide; i++)
		{
			nowx += dx;
			nowy += dy;
			if (!checkbulletStandableGrid(make_pair(nowx, nowy))) {
				return false;
			}
		}
		return true;
	}
	// updateMap 更新地图
	void updateMap(ITrickerAPI& api)
	{
		vector<vector<PlaceType> >p = api.GetFullMap();
		if (mp.empty())
		{
			mp = p;
			vis = vector<vector<int> >(p.size(), vector<int>(p[0].size(), -1));
			exi = vector<vector<int> >(p.size(), vector<int>(p[0].size(), 0));
			for (int i = p.size() - 1; i >= 0; i--)
				for (int j = p[0].size() - 1; j >= 0; j--)
				{
					if (p[i][j] == PlaceType::ClassRoom)
						cls.push_back(make_pair(i, j));
					if (p[i][j] == PlaceType::Gate)
						gate.push_back(make_pair(i, j));
				}
		}
	}

	void changeAddr(ITrickerAPI& api, pair<int, int>p)
	{
		adr = p;
		now = make_pair(-1, -1);
		linecnt = 0;
		line.clear();
	}
	bool checkMap(int a, int b)
	{
		if (mp[a][b] != PlaceType::Land ||
			mp[a][b] != PlaceType::Grass)
			return false;
		else return true;
	}
	pair<int, int>findRecent(double x, double y)
	{
		double dis = 1e18;
		int px = -1, py = -1;
		for (int i = mp.size() - 1; i >= 0; i--)
			for (int j = mp[0].size() - 1; j >= 0; j--)
				if (checkStandableCell(make_pair(i, j)))
				{
					double check =
						(x - i * 1000 - 500) * (x - i * 1000 - 500) +
						(y - j * 1000 - 500) * (y - j * 1000 - 500);
					if (check < dis)
					{
						px = i; py = j;
						dis = check;
					}
				}
		return make_pair(px, py);
	}
	vector<vector<int> >dis, lst;
	queue<pair<int, int> >q;
	int checkSidable(pair<int, int>p)//不可到达返回-1否则返回九宫格距离
	{
		int d = 1e9;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				if (dis[p.first + i][p.second + j] != -1)
					d = min(d, dis[p.first + i][p.second + j]);
		if (d == 1e9)return -1;
		else return d;
	}
	long long BFS(pair<int, int>s, pair<int, int>t, ITrickerAPI& api, bool canstand = 0)//spd：翻窗/普通移速
	{
		long long t1 = getTime();
		bool is = 0;
		dis = lst = vector<vector<int> >(mp.size(), vector<int>(mp[0].size(), -1));
		dis[s.first][s.second] = 0;
		while (!q.empty())q.pop();
		q.push(make_pair(s.first, s.second));
		//cout << s.first << ' ' << s.second << ' ' << t.first << ' ' << t.second << endl;
		/*for (int i = 0; i < mp.size(); i++)
		{
			string s;
			for (int j = 0; j < mp[0].size(); j++)
				s += to_string(exi[i][j]) + ' ';
			api.Print(s);
		}*/
		if (checkStandableCell(t)) canstand = 1;
		while (!q.empty())
		{
			pair<int, int>p = q.front(); q.pop();
			int x = p.first, y = p.second;
			//cout << x << ' ' << y << ' ' << dis[x][y] << endl;
			if (abs(x - t.first) + abs(y - t.second) <= 1 && canstand == 0 && checkStandableCell(make_pair(x, y)))
			{
				//cout << x << ' ' << t.first << ' ' << y << ' ' << t.second << endl;
				t = make_pair(x, y);
				is = 1;
				break;
			}
			if (abs(x - t.first) + abs(y - t.second) <= 0)
			{
				//cout << x << ' ' << t.first << ' ' << y << ' ' << t.second << endl;
				t = make_pair(x, y);
				is = 1;
				break;
			}
			for (int i = 0; i <= 7; i++)if (!(i & 1))
			{
				int a = 0, b = 0;
				if (i == 0 || i == 1 || i == 7)a = 1;
				if (i == 3 || i == 4 || i == 5)a = -1;
				if (i == 1 || i == 2 || i == 3)b = 1;
				if (i == 5 || i == 6 || i == 7)b = -1;
				//cout << x << ' ' << y << ' ' << a << ' ' << b << endl;
				if (checkStandableCell(make_pair(x + a, y + b))
					|| mp[x + a][y + b] == PlaceType::Window)
				{
					if (dis[x + a][y + b] == -1)
					{
						dis[x + a][y + b] = dis[x][y] + 1;
						q.push(make_pair(x + a, y + b));
						lst[x + a][y + b] = i;
					}
				}
			}
		}
		line.clear();
		/*for (int i = 0; i < mp.size(); i++, cout << endl)
			for (int j = 0; j < mp[0].size(); j++)
				cout << dis[i][j] << ' ';
		for (int i = 0; i < mp.size(); i++, cout << endl)
			for (int j = 0; j < mp[0].size(); j++)
				cout << lst[i][j] << ' ';
		for (int i = 0; i < mp.size(); i++, cout << endl)
			for (int j = 0; j < mp[0].size(); j++)
				cout << exi[i][j] << ' ';*/
		if (t != make_pair(-1, -1))
		{
			if (!is)return getTime() - t1;
			for (int x = t.first, y = t.second; x != s.first || y != s.second;)
			{
				if (mp[x][y] != PlaceType::Window)
					line.push_back(make_pair(x, y));
				//cout << lst[x][y] << endl;
				int i = lst[x][y], a = 0, b = 0;
				if (i == 0 || i == 1 || i == 7)a = 1;
				if (i == 3 || i == 4 || i == 5)a = -1;
				if (i == 1 || i == 2 || i == 3)b = 1;
				if (i == 5 || i == 6 || i == 7)b = -1;
				x -= a; y -= b;
				/*cout << x << ' ' << y << endl;
				cout << "check" << ' ' << s.first << ' ' << s.second << endl;
				cout << "check" << ' ' << t.first << ' ' << t.second << endl;*/
			}

			if (!line.empty() && abs(line.back().first - s.first) + abs(line.back().second - s.second) == 2)
			{
				;
			}
			else
				line.push_back(make_pair(s.first, s.second));
		}
		return getTime() - t1;
	}

}TTSF;

pair<int, int> lastLoc[4] = { make_pair(-1,-1),make_pair(-1,-1),make_pair(-1,-1),make_pair(-1,-1) };
long long lastSeen[4] = { -1,-1,-1,-1 };
int playerstate[4] = { 0 , 0 , 0 , 0 };
double player_v[4] = { 0.0 , 0.0 , 0.0 , 0.0 }; // -1 代表无法计算
bool TAGONLYTEACHER = 0;

pair<int, double> Shoot(ITrickerAPI& api, tuple<double, double, double, double>bullet) {
	auto students = api.GetStudents();
	auto self = api.GetSelfInfo();
	// 经测试facing direction是移动方向，但是他不动我也没有办法。
	vector< tuple<double, double, int> > studentsWithAngle; // theta , time , id
	int timid = 0;
	for (auto& s : students) {
		timid++;
		int id = s->playerID, v = player_v[id];
		if (v != -1) {
			double t = 0.1; // s 
			double theta = 0.0; // rad
			double Speed = get<0>(bullet); // Replace 0.0 with the known value of Speed
			double CastTime = get<2>(bullet) / 1000; // Replace 0.0 with the known value of CastTime
			double self_x = self->x; // Replace 0.0 with the known value of self->x
			double self_y = self->y; // Replace 0.0 with the known value of self->y
			double sx = s->x; // Replace 0.0 with the known value of sx
			double sy = s->y; // Replace 0.0 with the known value of sy
			double tx, ty;
			if (v == 0) {
				t = sqrt(1.0 * (self->x - s->x) * (self->x - s->x) + 1.0 * (self->y - s->y) * (self->y - s->y)) / Speed;
				theta = atan2(s->y - self->y, s->x - self->x);
				tx = sx + v * cos(s->facingDirection) * (t + CastTime);
				ty = sy + v * sin(s->facingDirection) * (t + CastTime);
			}
			else if (Speed < 70000) {
				/*
					解方程组
					sx + vx * (t + CastTime) = self -> x + t * Speed * cos(theta)
					sy + vy * (t + CastTime) = self -> y + t * Speed * sin(theta)
				*/
				double vx = v * cos(s->facingDirection); // Replace 0.0 with the known value of vx
				double vy = v * sin(s->facingDirection); // Replace 0.0 with the known value of vy
				//api.Print(fmt::format("vx = {}, vy = {}", vx, vy));

				// 使用二分查找解t
				int l = 0, r = 1001, mid;
				while (l < r) {
					mid = (l + r) >> 1;
					double tx = sx + vx * (mid / 1000.0 + CastTime);
					double ty = sy + vy * (mid / 1000.0 + CastTime);
					double dis = sqrt((self->x - tx) * (self->x - tx) + (self->y - ty) * (self->y - ty));
					if (dis < Speed * (mid / 1000.0)) {
						r = mid;
					}
					else {
						l = mid + 1;
					}
				}
				t = l / 1000.0;
				//tx = sx + v * cos(s->facingDirection) * (t + CastTime);
				//ty = sy + v * sin(s->facingDirection) * (t + CastTime);
				tx = sx, ty = sy;
				theta = atan2(ty - self->y, tx - self->x);
			}
			else {
				t = sqrt(1.0 * (self->x - s->x) * (self->x - s->x) + (self->y - s->y) * (self->y - s->y)) / Speed;
				theta = atan2(s->y - self->y, s->x - self->x);
				tx = sx + v * cos(s->facingDirection) * (t + CastTime);
				ty = sy + v * sin(s->facingDirection) * (t + CastTime);
			}
			//判断是否仍在射程之内
			if (t >= get<1>(bullet) / Speed) continue; //不在射程内
			t -= 1200.0 / Speed; //实际上子弹出来的时候是和发射者相切的，那么飞行总距离要小1200
			//避免鞭尸的射击判定
			bool blocked_by_body = false;
			for (auto& s : students) if (s->playerState == PlayerState::Addicted) {
				//求(s->x , s->y)到线段(tx , ty) , (self ->x , self->y)的距离
				double a = self->y - ty;
				double b = tx - self->x;
				double c = self->x * ty - tx * self->y;
				double dis = abs(a * s->x + b * s->y + c) / sqrt(a * a + b * b);
				if (dis <= 400) {
					blocked_by_body = true;
					break;
				}
			}
			//在射程内，可以尝试射击
			if (blocked_by_body == false && TTSF.havebulletWay(self->x, self->y, tx, ty)) {
				studentsWithAngle.push_back(make_tuple(theta, t, timid - 1));
			}
		}
	}

	/* 射击优先级设计 射击分数最大的
		一次能射死 1000 ， 学霸 1000 + （身上的冥想进度 / 100） , 开心果 800 ， 运动员500 ， 教师 0
		沉迷度三档 2000 ， 1000 ， 0
		正在开校门的10000 ， 正在做作业的 3000 ， 正在开箱子的800
		有二倍速的 - 2000 ， 有复活甲的 - 500 ， 如果在此之上还一次能射死的再 - 2000 ， 有盾的 - 1000 ， 如果自己有破盾就变为+1000
		正在唤醒的 + 3000
		t * Speed<=200的 + 2000 ， <= 300 的 + 1500 ， <= 400的 + 500 ，  <= 500 的 + 0 ， <= 700 的-500 ， 剩余的 - 5000
		已经沉迷了的 - 20000
	*/
	int maxScore = -0x3f3f3f3f;
	double maxScoreangle = 0;
	int Damage = get<3>(bullet);
	for (auto& s : studentsWithAngle) {
		int Score = 0;
		int id = get<2>(s);

		auto& p = students[id];
		// 一次能射死 1000
		if (p->determination <= Damage) {
			Score += 1000;
		}
		// 学霸 1000 + （身上的冥想进度 / 100） , 开心果 800 ， 运动员500 ， 教师 0
		if (p->studentType == StudentType::StraightAStudent) Score += 1000;//没有查看冥想进度的接口，你没资格啊
		else if (p->studentType == StudentType::Sunshine) Score += 800;
		else if (p->studentType == StudentType::Athlete) Score += 600;
		else if (p->studentType == StudentType::Teacher) Score += -3000;

		if (TAGONLYTEACHER) Score += 3000;
		// 沉迷度三档 2000 ， 1000 ， 0
		// 使用lambda表达式计算沉迷度的档次，addiction < maxAddiction * 0.33为最低档，addiction > maxAddiction * 0.66为最高档
		auto addictionLevel = [&](int addiction, int maxAddiction) {
			if (addiction < maxAddiction * 0.33) return 0;
			else if (addiction > maxAddiction * 0.66) return 2;
			else return 1;
		};
		Score += addictionLevel(p->addiction, maxAddiction[p->studentType]) * 1000;
		// 正在开校门的10000 ， 正在做作业的 3000 ， 正在开箱子的800 , 正在唤醒的 + 3000, 正在爬树的 + 800, 正在鼓舞的 + 800, 正在被鼓舞的 + 800
		if (p->playerState == PlayerState::OpeningAGate) Score += 10000;
		else if (p->playerState == PlayerState::Learning
			|| p->playerState == PlayerState::Stunned) Score += 3000;
		else if (p->playerState == PlayerState::OpeningAChest
			|| p->playerState == PlayerState::Encouraging
			|| p->playerState == PlayerState::Encouraged
			|| p->playerState == PlayerState::Rousing
			|| p->playerState == PlayerState::Climbing
			) Score += 7000;
		else if (p->playerState == PlayerState::Addicted
			|| p->playerState == PlayerState::Roused)
			Score -= 20000;
		// 有二倍速的 - 2000 ， 有复活甲的 - 500 ， 如果在此之上还一次能射死的再 - 2000 ， 有盾的 - 1000 ， 如果自己有破盾就变为+1000
		// 太复杂了，暂时不写了
		// t * Speed<=200的 + 2000 ， <= 300 的 + 1500 ， <= 400的 + 500 ，  <= 500 的 + 0 ， <= 700 的-500 ， 剩余的 - 5000
		double t = get<1>(s);
		double speed = p->speed * abs(std::cos(p->facingDirection - get<0>(s) - pi / 2));
		//api.Print(fmt::format("t: {}, speed: {}", t, speed));
		if (t * speed <= 200) Score += 2000;
		else if (t * speed <= 300) Score += 1500;
		else if (t * speed <= 400) Score += 500;
		else if (t * speed <= 500) Score += 0;
		else if (t * speed <= 800) Score -= 500;
		else Score -= 5000;
		// 优先级最高的
		if (Score > maxScore) {
			maxScore = Score;
			maxScoreangle = get<0>(s);
		}

		//api.Print(fmt::format("id = {} , score = {}", id, Score));
	}
	//输出maxScore
	//api.Print(fmt::format("maxScore: {}", maxScore));
	// 如果周围有cd好了的教师则 - 10000 ， 除非自己的buff中有invisible

	bool will_be_shocked = false;
	for (auto& s : students) if (s->studentType == StudentType::Teacher) {
		//输出技能cd
		//api.Print(fmt::format("teacher skill cd: {}", s->timeUntilSkillAvailable[0]));
		if (s->timeUntilSkillAvailable[0] < get<2>(bullet)) {
			will_be_shocked = true;
			break;
		}
	}
	// 加入了判断是否为刺客，如果是刺客，且隐身技能cd好了，就用技能
	if (will_be_shocked && maxScore > 0 && self->trickerType == TrickerType::Assassin && self->timeUntilSkillAvailable[0] == 0) {
		api.UseSkill(0);
		this_thread::sleep_for(chrono::milliseconds(3));
	}
	// 如果自己身上有invisible，就不用管了
	bool isInvisible = false;
	for (auto& t : self->buff) if (t == TrickerBuffType::Invisible) {
		isInvisible = true;
		break;
	}
	if (isInvisible == false && will_be_shocked == true) {
		maxScore -= 1000; // 不管了，杀
	}
	api.Print(fmt::format(" maxScore : {} ", maxScore));

	// 是否射击？
	return make_pair(maxScore, maxScoreangle);
}

void AI::play(ITrickerAPI& api)
{
	auto self = api.GetSelfInfo();
	auto actualtime = TTSF.getTime();
	auto students = api.GetStudents();
	TTSF.updateMap(api);

	// 计算视野内学生的速度，以及上次被看见的时间，是否是尸体等等信息的预处理
	static int TeacherID = 0, IsTeacherAddicted = 0;
	for (auto& s : students) {
		int id = s->playerID, v = -1;
		if (actualtime - lastSeen[id] <= 200)
			if (lastLoc[id].first == s->x && lastLoc[id].second == s->y)
				v = 0;
			else
				v = s->speed;
		//api.Print(fmt::format("id: {} , x: {} , y：{} , sx:{} , sy:{} , v: {}", id, s->x,s->y,self->x,self->y, v));
		player_v[id] = v;

		//把尸体看作路障
		if (s->studentType == THUAI6::StudentType::Teacher) {
			TTSF.addExist(lastLoc[id], 400, -1);
			TTSF.addExist(make_pair(s->x, s->y), 400, 1);
			playerstate[id] = 1;
			TeacherID = id;

			if (s->playerState == PlayerState::Addicted)
				IsTeacherAddicted = 1;
			else
				IsTeacherAddicted = 0;

		}
		else {
			if (s->playerState == PlayerState::Addicted) {
				if (playerstate[id] == 0)
					TTSF.addExist(make_pair(s->x, s->y), 400, 1);
				playerstate[id] = 1;
			}
			else { //复活了就去除路障
				if (playerstate[id] == 1)
					TTSF.addExist(lastLoc[id], 400, -1);
				playerstate[id] = 0;
			}
		}
		//更新上一次见到的位置

		lastSeen[id] = actualtime;
		lastLoc[id] = make_pair(s->x, s->y);
	}

	static int kaqiangcleantag = 0;
	//无法操作时，直接返回
	if (self->playerState == PlayerState::Attacking || self->playerState == PlayerState::Swinging
		|| self->playerState == PlayerState::Stunned || self->playerState == PlayerState::Climbing) {
		kaqiangcleantag = 1;//无法操作时无法判断是否卡墙了。
		return;
	}

	static int priority = 0x3f3f3f3f;
	{
		static int lx = -1, ly = -1;
		static double lt = 0, ltheta = 0;
		if (kaqiangcleantag) {
			lx = ly = -1;
			kaqiangcleantag = 0;
		}
		bool bodyonface = 0;
		for (auto s : students) {
			if (1.0 * (self->x - s->x) * (self->x - s->x) + (self->y - s->y) * (self->y - s->y) < 1000 * 1000 && s->playerState != PlayerState::Addicted) {
				bodyonface = 1;
				break;
			}
		}
		if (lx == self->x && ly == self->y && priority < 0x3f3f3f3f) {//有目标但是不动
			if (Constants::Assassin::alertnessRadius / self->trickDesire <= 1000 && !bodyonface) {
				int x = api.GridToCell(self->x), y = api.GridToCell(self->y);
				double alpha = 0;
				bool flg = 0;
				for (int i = -1; i <= 1; i++)
					for (int j = -1; j <= 1; j++)
						if (i == 0 || j == 0)
							if (i | j)
								if (TTSF.mp[x + i][y + j] == THUAI6::PlaceType::Grass) {
									alpha = atan2(api.CellToGrid(y + j) - self->y, api.CellToGrid(x + i) - self->x);
									flg = 1;
									goto outside;
								}
			outside:
				if (TTSF.mp[TTSF.adr.first][TTSF.adr.second] == THUAI6::PlaceType::Grass) {
					alpha = atan2(api.CellToGrid(TTSF.adr.second) - self->y, api.CellToGrid(TTSF.adr.first) - self->x);
					flg = 1;
				}
				if (flg) {
					if (self->timeUntilSkillAvailable[0] == 0)
						api.UseSkill(0);
					if (self->timeUntilSkillAvailable[1] == 0)
						api.UseSkill(1);
					api.Attack(alpha);
				}
			}
			if (actualtime - lt > 300) {
				api.EndAllAction();
				this_thread::sleep_for(chrono::milliseconds(5));
				static double lstdir = TTSF.lstdir;
				api.Move(300, lstdir + pi);
				lstdir += pi / 5;
				this_thread::sleep_for(chrono::milliseconds(300));
				return;
			}
		}
		else {
			lt = actualtime;
		}
		lx = self->x, ly = self->y;
	}

	// 射击判断
	if (self->timeUntilSkillAvailable[1] == 0) {
		auto Knife = Shoot(api, bulletInfo[BulletType::FlyingKnife]);
		if (Knife.first >= 5000) {
			api.EndAllAction();
			this_thread::sleep_for(chrono::milliseconds(1));
			api.UseSkill(1);
			this_thread::sleep_for(chrono::milliseconds(1));
			api.Attack(Knife.second);
			return;
		}
	}
	auto ShootItem = Shoot(api, bulletInfo[BulletType::CommonAttackOfTricker]);
	//输出ShootItem
	//api.Print(fmt::format("ShootItem.first = {} , ShootItem.second = {}", ShootItem.first, ShootItem.second));
	if (ShootItem.first >= 0) {
		api.EndAllAction();
		api.Attack(ShootItem.second);
		return;
	}

	bool OnlyTeacher = 1;
	for (int i = 0; i < 3; i++) if (playerstate[i] == 0)
		OnlyTeacher = 0;

	//OnlyTeacher = 1;

	if (OnlyTeacher && !IsTeacherAddicted) {
		playerstate[TeacherID] = 0;

	}

	TAGONLYTEACHER = OnlyTeacher;

	// 优先级：1.追看得见的 , 2.追1s内的最后出现位置 ， 3/4 先听，然后去找作业/听到的位置(可能按照距离决定先后) , 5 追10s内出现的最后位置 ， 6 面向地图编程
	//追看的见的 优先级为1 同优先级时替换目标
	{
		if (students.empty() == false && priority >= 1) {
			// 找出欧几里得距离最近的不是教师的student


			long long minDist = 1e18;
			int minDistid = -1;
			for (auto s : students)if (s->playerState != PlayerState::Addicted && ((OnlyTeacher) || s->studentType != StudentType::Teacher)) {
				long long dist = (s->x - self->x) * (s->x - self->x) + (s->y - self->y) * (s->y - self->y);
				if (dist < minDist) {
					minDist = dist;
					minDistid = s->playerID;
				}
			}

			if (minDistid != -1) {
				TTSF.changeAddr(api, make_pair(lastLoc[minDistid].first / 1000, lastLoc[minDistid].second / 1000));
				TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);//应用翻窗速度。。。
				priority = 1;
				goto getsetgo;
			}
		}
	}
	//追最后1s内目击地点 优先级为2 同优先级不替换
	if (priority > 2) {
		long long LstSeenTim = -1;
		pair<int, int> LstSeenPos = make_pair(-1, -1);
		for (int i = 0; i < 4; i++) if (playerstate[i] == 0)
		{
			if (LstSeenTim < lastSeen[i])
			{
				LstSeenTim = lastSeen[i];
				LstSeenPos = lastLoc[i];
			}
		}
		if (actualtime - LstSeenTim <= 1000) {
			TTSF.changeAddr(api, make_pair(LstSeenPos.first / 1000, LstSeenPos.second / 1000));
			TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
			priority = 2;
			goto getsetgo;
		}
	}
	//追听得见的 优先级为3/4 同优先级不替换

	{
		static double lastclassVolume = 0, dv = 0, lt = 0;
		static int lx = -1, ly = -1, tim = 0;
		static int targetcls = -1;
		static set<int>candidate;
		static double sumerr[11] = {};
		static int lprogress[11] = {}, progress[11] = {};

		memset(progress, -1, sizeof progress);
		for (int i = 0; i < TTSF.cls.size(); i++)
			if (api.HaveView(api.CellToGrid(TTSF.cls[i].first), api.CellToGrid(TTSF.cls[i].second)))
				progress[i] = api.GetClassroomProgress(TTSF.cls[i].first, TTSF.cls[i].second);

		if (priority >= 3) {
			if (self->classVolume > 0) {

				auto isclear = [&](double x, double y, int id) {
					if (self->trickDesire * (sqrt((x - self->x) * (x - self->x) + (y - self->y) * (y - self->y)) + 2000) < Constants::Assassin::alertnessRadius)
						return true;
					if (api.HaveView(int(x), int(y)))
						if (progress[id] == lprogress[id])
							return true;
					return false;
				};

				auto chk = [&](double x, double y, int id) {
					if ((x - self->x) * (x - self->x) + (y - self->y) * (y - self->y) > Constants::Assassin::alertnessRadius * Constants::Assassin::alertnessRadius)
						return false;
					if (self->classVolume * sqrt((x - self->x) * (x - self->x) + (y - self->y) * (y - self->y)) > Constants::Assassin::alertnessRadius)
						return false;
					if (self->trickDesire * (sqrt((x - self->x) * (x - self->x) + (y - self->y) * (y - self->y)) + 2000) < Constants::Assassin::alertnessRadius)
						return false;
					if (api.HaveView(int(x), int(y))) {
						if (progress[id] == lprogress[id])
							return false;
					}
					return true;
				};

				if (targetcls != -1 && isclear(api.CellToGrid(TTSF.cls[targetcls].first), api.CellToGrid(TTSF.cls[targetcls].second), targetcls)) {// 突然听不到目标点周围有人在做作业了，说明清剿完毕
					lastclassVolume = dv = lt = 0;
					lx = ly = -1;
					targetcls = -1;
					memset(sumerr, 0, sizeof sumerr);
					tim = 0;
					candidate.clear();
				}

				bool vis[11] = {};
				for (int i = 0; i < TTSF.cls.size(); i++)
					if (chk(api.CellToGrid(TTSF.cls[i].first), api.CellToGrid(TTSF.cls[i].second), i)) {
						vis[i] = 1;
					}
					else vis[i] = 0;
				if (candidate.empty()) {
					for (int i = 0; i < TTSF.cls.size(); i++)
						if (vis[i])
							candidate.insert(i);
				}
				else {
					for (int i = 0; i < TTSF.cls.size(); i++)
						if (!vis[i] && candidate.count(i))
							candidate.erase(i);
					if (candidate.empty()) {
						lastclassVolume = dv = lt = 0;
						lx = ly = -1;
						memset(sumerr, 0, sizeof sumerr);
						targetcls = -1;
						tim = 0;
						candidate.clear();
					}
				}

				{
					if (lx == self->x && ly == self->y) {
						dv = (self->classVolume - lastclassVolume) / lastclassVolume / (actualtime - lt);
					}
					if (lx == -1 && ly == -1) {
						lx = self->x, ly = self->y;
						lt = actualtime;
						lastclassVolume = self->classVolume;
						api.EndAllAction();
						this_thread::sleep_for(chrono::milliseconds(48));
						priority = 0x3f3f3f3f;
						return;
					}
					double calclassVolume = lastclassVolume * (1 + dv * (actualtime - lt));

					for (auto s : candidate) {
						sumerr[s] += fabs(calclassVolume
							* sqrt(1.0 * (api.CellToGrid(TTSF.cls[s].first) - lx) * (api.CellToGrid(TTSF.cls[s].first) - lx) + 1.0 * (api.CellToGrid(TTSF.cls[s].second) - ly) * (api.CellToGrid(TTSF.cls[s].second) - ly))
							- self->classVolume
							* sqrt(1.0 * (api.CellToGrid(TTSF.cls[s].first) - self->x) * (api.CellToGrid(TTSF.cls[s].first) - self->x) + 1.0 * (api.CellToGrid(TTSF.cls[s].second) - self->y) * (api.CellToGrid(TTSF.cls[s].second) - self->y)));
					}

					//每2s选择sumerr最小的
					if ((tim++) % 20 == 0) {
						double minerr = 1e20;
						targetcls = -1;
						for (auto s : candidate) {
							if (minerr > sumerr[s]) {
								minerr = sumerr[s];
								targetcls = s;
							}
							//输出cls[s].first , cls[s].second
							//api.Print(fmt :: format("cls[{}] = ({}, {})", s, TTSF.cls[s].first, TTSF.cls[s].second));
						}
						if (targetcls != -1) {
							TTSF.changeAddr(api, make_pair(TTSF.cls[targetcls].first, TTSF.cls[targetcls].second));
							TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
							priority = 3;
							goto getsetgo;
						}
					}
				}
			}
			else {
				if (lx != -1) { // 突然听不见了
					for (auto s : TTSF.cls)
						if (fabs(1.0 * (api.CellToGrid(s.first) - self->x) * (api.CellToGrid(s.first) - self->x)
							+ 1.0 * (api.CellToGrid(s.second) - self->y) * (api.CellToGrid(s.second) - self->y)
							- Constants::Assassin::alertnessRadius * Constants::Assassin::alertnessRadius) < 1000 * 1000) {
							TTSF.changeAddr(api, make_pair(s.first, s.second));
							TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
							priority = 3;
							goto getsetgo;

						}
				}

				lastclassVolume = dv = lt = 0;
				lx = ly = -1;
				targetcls = -1;
				memset(sumerr, 0, sizeof sumerr);
				tim = 0;
				candidate.clear();
			}
		}

		memcpy(lprogress, progress, sizeof progress);
	}

	static double Nottoheartill = 0;
	static bool hearing = false; // 是否正在听
	static vector< tuple<double, double, double, double> > circles; // 圆心坐标，半径 , 过了的时间 ， 此数组用于听trickDesire
	if (priority > 4 && actualtime > Nottoheartill) {
		static double lstalpha = 0, lsttheta = 0; // 上一次的alpha和theta
		if (hearing == true) {
			if (self->trickDesire <= 0) { // 听不到了
				hearing = false;
				goto priority3;
			}
			else if (circles.empty() == false) {
				// 还在听
				double lastangle = atan2(self->y - get<1>(circles.back()), self->x - get<0>(circles.back()));
				circles.push_back(make_tuple(self->x, self->y,
					Constants::Assassin::alertnessRadius / self->trickDesire, actualtime));

				if (circles.size() >= 3) { // 尝试计算目标起始点和速度
					//具体思路 写出三个圆的方程，然后代入两个变量theta（为初始时的极角）和alpha（为对方速度极角），然后使用牛顿迭代数值求解
					int id = circles.size() - 1;
					double x[3] = { get<0>(circles[id - 2]), get<0>(circles[id - 1]), get<0>(circles[id]) },
						y[3] = { get<1>(circles[id - 2]), get<1>(circles[id - 1]), get<1>(circles[id]) },
						r[3] = { get<2>(circles[id - 2]), get<2>(circles[id - 1]), get<2>(circles[id]) },
						t[3] = { get<3>(circles[id - 2]), get<3>(circles[id - 1]), get<3>(circles[id]) };

					//如果三点共线，则无法计算
					if (fabs((x[0] - x[1]) * (y[1] - y[2]) - (x[1] - x[2]) * (y[0] - y[1])) < 1e-2) {
						goto continuetohear;
					}

					//api.Print(fmt::format("x[{}]= {}, y[{}] = {}, r[{}] = {}, t[{}] = {}", 0, x[0], 0, y[0], 0, r[0], 0, t[0]));
					//api.Print(fmt::format("x[{}]= {}, y[{}] = {}, r[{}] = {}, t[{}] = {}", 1, x[1], 1, y[1], 1, r[1], 1, t[1]));
					//api.Print(fmt::format("x[{}]= {}, y[{}] = {}, r[{}] = {}, t[{}] = {}", 2, x[2], 2, y[2], 2, r[2], 2, t[2]));
					//先判断三个圆是否有一个交点
					//if(0) // disabled
					{
						/*
							圆方程为 (x - x[i])^2 + (y - y[i])^2 = r[i]^2
						*/
						// 计算前两个圆的两个交点

						auto distance = [](double x1, double y1, double x2, double y2) {
							return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
						};
						double d = distance(x[0], y[0], x[1], y[1]);
						if (d > r[0] + r[1] || d < std::abs(r[0] - r[1])) {
							//api.Print("No intersection points.");
							goto continuetohear;
						}
						double a = (r[0] * r[0] - r[1] * r[1] + d * d) / (2 * d);
						double h = std::sqrt(r[0] * r[0] - a * a);
						double x3 = x[0] + a * (x[1] - x[0]) / d;
						double y3 = y[0] + a * (y[1] - y[0]) / d;
						double x1 = x3 + h * (y[1] - y[0]) / d;
						double y1 = y3 - h * (x[1] - x[0]) / d;
						double x2 = x3 - h * (y[1] - y[0]) / d;
						double y2 = y3 + h * (x[1] - x[0]) / d;
						//api.Print(fmt::format("x1 = {} , y1 = {}", x1, y1));
						//api.Print(fmt::format("dis = {}", (x1 - x[2]) * (x1 - x[2]) + (y1 - y[2]) * (y1 - y[2]) - r[2] * r[2]));
						//代入两个交点，判断是否在第三个圆上
						if (fabs((x1 - x[2]) * (x1 - x[2]) + (y1 - y[2]) * (y1 - y[2]) - r[2] * r[2]) < 100.0 && x1 >= 1000 && x1 < 49000 && y1 >= 1000 && y1 < 49000) {
							//	api.Print(fmt::format("x1 = {} , y1 = {}", x1, y1));

							hearing = false;

							for (int i = 0; i < 4; i++)
								if (playerstate[i] == 1)
									if ((x1 - lastLoc[i].first) * (x1 - lastLoc[i].first) + (y1 - lastLoc[i].second) * (y1 - lastLoc[i].second) < 22000 * 22000) {
										Nottoheartill = actualtime + 6000;
										goto priority3;
									}

							priority = 4;
							TTSF.changeAddr(api, make_pair((int)x1 / 1000, (int)y1 / 1000));
							TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
							lsttheta = atan2(y1 - y[2], x1 - x[2]);
							goto getsetgo;
						}
						//api.Print(fmt::format("x2 = {} , y2 = {}", x2, y2));
						if (fabs((x2 - x[2]) * (x2 - x[2]) + (y2 - y[2]) * (y2 - y[2]) - r[2] * r[2]) < 100.0 && x2 >= 1000 && x2 < 49000 && y2 >= 1000 && y2 < 49000) {
							//api.Print(fmt::format("x2 = {} , y2 = {}", x2, y2));
							hearing = false;

							for (int i = 0; i < 4; i++)
								if (playerstate[i] == 1)
									if ((x2 - lastLoc[i].first) * (x2 - lastLoc[i].first) + (y2 - lastLoc[i].second) * (y2 - lastLoc[i].second) < 22000 * 22000) {
										Nottoheartill = actualtime + 6000;
										goto priority3;
									}

							priority = 4;
							TTSF.changeAddr(api, make_pair((int)x2 / 1000, (int)y2 / 1000));
							TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
							lsttheta = atan2(y2 - y[2], x2 - x[2]);
							goto getsetgo;
						}

					}
					t[1] -= t[0], t[2] -= t[0], t[0] = 0;
					x[1] -= x[0], x[2] -= x[0], x[0] = 0;
					y[1] -= y[0], y[2] -= y[0], y[0] = 0;
					//无交点则v = 3000	
					{
						//初始坐标为(r\cos\theta,r\sin\theta) , 速度为(v\cos\alpha,v\sin\alpha)
						/*
							(r\cos\theta + v\cos\alpha t[1] - x[1])^2 + (r\sin\theta + v\sin\alpha t[1] - y[1])^2 = r[1]^2
							(r\cos\theta + v\cos\alpha t[2] - x[2])^2 + (r\sin\theta + v\sin\alpha t[2] - y[2])^2 = r[2]^2

						*/
						// 牛顿迭代

						// Define the known values
						double R = r[0];
						double v = 3;
						// Define the system of equations
						auto f1 = [&](double theta, double alpha) {
							return std::pow(R * std::cos(theta) + v * std::cos(alpha) * t[1] - x[1], 2) +
								std::pow(R * std::sin(theta) + v * std::sin(alpha) * t[1] - y[1], 2) - std::pow(r[1], 2);
						};
						auto f2 = [&](double theta, double alpha) {
							return std::pow(R * std::cos(theta) + v * std::cos(alpha) * t[2] - x[2], 2) +
								std::pow(R * std::sin(theta) + v * std::sin(alpha) * t[2] - y[2], 2) - std::pow(r[2], 2);
						};
						// Define the Jacobian matrix
						double delta = 1e-6;
						std::array<std::array<std::function<double(double, double)>, 2>, 2> J{ {
							{{[&](double theta, double alpha) { return (f1(theta + delta, alpha) - f1(theta, alpha)) / delta; },
							  [&](double theta, double alpha) { return (f1(theta, alpha + delta) - f1(theta, alpha)) / delta; }}},
							{{[&](double theta, double alpha) { return (f2(theta + delta, alpha) - f2(theta, alpha)) / delta; },
							  [&](double theta, double alpha) { return (f2(theta, alpha + delta) - f2(theta, alpha)) / delta; }}}
						} };
						// Initial guess
						double theta = 0.0;
						double alpha = 0.0;
						int tim = 0, max_iter = 30;
						double eps = 1e-6;
						// Newton-Raphson method
						for (int i = 0; i < max_iter; ++i, tim++) {
							double J11 = J[0][0](theta, alpha);
							double J12 = J[0][1](theta, alpha);
							double J21 = J[1][0](theta, alpha);
							double J22 = J[1][1](theta, alpha);
							double det = J11 * J22 - J12 * J21;
							if (fabs(det) < 1e-20) {
								tim = max_iter;
								break;
							}
							//api.Print(fmt::format("J11 = {}, J12 = {}, J21 = {}, J22 = {}, det = {}", J11, J12, J21, J22, det));
							double dx = (-J22 * f1(theta, alpha) + J12 * f2(theta, alpha)) / det;
							double dy = (J21 * f1(theta, alpha) - J11 * f2(theta, alpha)) / det;
							theta += dx;
							alpha += dy;
							if (std::abs(dx) < eps && std::abs(dy) < eps)
								break;
						}
						//api.Print(fmt::format("theta = {}, alpha = {}", theta, alpha));
						if (tim >= max_iter) {
							//api.Print("No solution");
							goto continuetohear;
						}
						lsttheta = theta, lstalpha = alpha;
						double tx = r[0] * cos(theta) + v * cos(alpha) * t[2] + get<0>(circles[id - 2]),
							ty = r[0] * sin(theta) + v * sin(alpha) * t[2] + get<1>(circles[id - 2]);

						if (tx < 1000 || tx > 49000 || ty < 1000 || ty > 49000)
							goto continuetohear;

						for (int i = 0; i < 4; i++)
							if (playerstate[i] == 1)
								if ((tx - lastLoc[i].first) * (tx - lastLoc[i].first) + (ty - lastLoc[i].second) * (ty - lastLoc[i].second) < 2000 * 2000) {
									hearing = false;
									goto priority3;
								}

						//	api.Print(fmt::format("tx = {}, ty = {}", tx, ty));
						priority = 4;
						TTSF.changeAddr(api, make_pair((int)tx / 1000, (int)ty / 1000));
						TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
						hearing = false;
						goto getsetgo;
					}
				}
			continuetohear:
				api.EndAllAction();

				//输出lastangle和当前位置
				//api.Print(fmt::format("lastangle = {}, x = {}, y = {}", lastangle, self->x, self->y));
				api.Move(100, lastangle + pi / 2);
				this_thread::sleep_for(chrono::milliseconds(48));
				return;
			}
		}
		else if (self->trickDesire > 0) {
			circles.clear();
			double R0 = Constants::Assassin::alertnessRadius / self->trickDesire;
			if (Constants::Assassin::alertnessRadius - R0 <= 1000) {
				// 不能听
				goto priority3;
			}
			hearing = true;// 开始听
			api.EndAllAction();
			circles.push_back(make_tuple(self->x, self->y, R0, actualtime));

			int mx = api.CellToGrid(self->x / 1000), my = api.CellToGrid(self->y / 1000);
			double alpha = 0;
			if (self->x >= mx && self->y >= my)  alpha = pi;
			else if (self->x >= mx && self->y < my) alpha = pi / 2;
			else if (self->x < mx && self->y >= my) alpha = pi * 3 / 2;
			else alpha = 0;

			api.Move(50, alpha);
			this_thread::sleep_for(chrono::milliseconds(48));
			return;
		}
	}
priority3://以后可能要改下名字

	// 听到的是尸体

	//听不到 则前往最后看到的目标的10s内的最后地点 优先级为5 同优先级不替换
	if (priority > 5) {
		long long LstSeenTim = -1;
		pair<int, int> LstSeenPos = make_pair(-1, -1);
		for (int i = 0; i < 4; i++) if (playerstate[i] == 0)
		{
			if (LstSeenTim < lastSeen[i])
			{
				LstSeenTim = lastSeen[i];
				LstSeenPos = lastLoc[i];
			}
		}
		if (actualtime - LstSeenTim <= 10000) { // 我感觉没有时间限制比较好 ， 在没有别的线索的情况下
			TTSF.changeAddr(api, make_pair(LstSeenPos.first / 1000, LstSeenPos.second / 1000));
			TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
			priority = 5;
			goto getsetgo;
		}
	}



	//去找作业 优先级为6 同优先级不替换
	if (priority > 7) {
		double maxdis = 0;
		pair<int, int>target = make_pair(-1, -1);
		for (auto s : TTSF.cls) {
			double dis = sqrt(1.0 * (api.CellToGrid(s.first) - self->x) * (api.CellToGrid(s.first) - self->x) + 1.0 * (api.CellToGrid(s.second) - self->y) * (api.CellToGrid(s.second) - self->y));
			if (dis > maxdis) {
				maxdis = dis;
				target = s;
			}
		}
		if (target.first != -1) {
			TTSF.changeAddr(api, target);
			TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
			priority = 7;
			goto getsetgo;
		}
	}
	// 寻路
getsetgo://案批
	if (priority <= 4) hearing = false; // 有目标则中断听
	//输出TTSF.adr ， TTSF.now , self ->x ,self->y



	if (TTSF.checkStandableCell(TTSF.adr)) {
		if (self->x / 1000 != TTSF.adr.first
			|| self->y / 1000 != TTSF.adr.second) {
			if (abs(api.GetSelfInfo()->x - TTSF.now.first * 1000 - 500) < 20 &&
				abs(api.GetSelfInfo()->y - TTSF.now.second * 1000 - 500) < 20)
				TTSF.now = make_pair(-1, -1);
			if (TTSF.now == make_pair(-1, -1))
			{
				if (TTSF.linecnt > 0 || TTSF.line.empty())
				{
					TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
					TTSF.linecnt = 0;
					if (TTSF.line.empty())
					{
						TTSF.now = make_pair(-1, -1);
						return;
					}
				}
				int check = -1;
				//for (int i = tts.line.size() - 1; i >= 0 && check == -1; i--)
				for (int i = 0; i < TTSF.line.size() && check == -1; i++)
					//if (abs(tts.line[i].first - x) == abs(tts.line[i].second - y))
				{
					//pair<int, int>a = tts.line[i], b = make_pair(x, y);
					//api.Print(to_string(a.first) + ' ' + to_string(a.second));
					if (TTSF.haveWay(api.GetSelfInfo()->x, api.GetSelfInfo()->y,
						1000 * TTSF.line[i].first + 500,
						1000 * TTSF.line[i].second + 500))
						check = i;
					/*
					if (a.second > b.second)swap(a, b);
					if ((a.first < b.first ?
						TTSF.crl[b.first][b.second] : TTSF.crr[b.first][b.second]) < a.second)
						check = i;*/
				}
				TTSF.linecnt++;
				//api.Print("CHECK!!! " + to_string(check));
				//cout << "ERROR?" << ' ' << check << endl;
				if (check != -1)
					while (TTSF.line.size() > check + 1)TTSF.line.pop_back();
				TTSF.now = TTSF.line.back();
				TTSF.line.pop_back();
				//else api.Move((i & 1 ? sqrt(2) : 1.0) * 1e6 / api.GetSelfInfo()->speed, acos(-1) * i / 4);
				//cout << i << ' ' << api.GetSelfInfo()->speed << endl;
			}
		}
		else {
			TTSF.now = make_pair(-1, -1), priority = 0x3f3f3f3f;
		}
	}
	else {
		if (abs(api.GetSelfInfo()->x - TTSF.adr.first * 1000 - 500) +
			abs(api.GetSelfInfo()->y - TTSF.adr.second * 1000 - 500) > 1100)
		{
			if (abs(api.GetSelfInfo()->x - TTSF.now.first * 1000 - 500) < 50 &&
				abs(api.GetSelfInfo()->y - TTSF.now.second * 1000 - 500) < 50)
				TTSF.now = make_pair(-1, -1);
			if (TTSF.now == make_pair(-1, -1))
			{
				if (TTSF.linecnt > 0 || TTSF.line.empty())
				{
					TTSF.BFS(make_pair(self->x / 1000, self->y / 1000), TTSF.adr, api);
					TTSF.linecnt = 0;
					if (TTSF.line.empty())
					{
						TTSF.now = make_pair(-1, -1);
						return;
					}
				}
				int check = -1;
				//for (int i = tts.line.size() - 1; i >= 0 && check == -1; i--)
				for (int i = 0; i < TTSF.line.size() && check == -1; i++)
					//if (abs(tts.line[i].first - x) == abs(tts.line[i].second - y))
				{
					//pair<int, int>a = tts.line[i], b = make_pair(x, y);
					//api.Print(to_string(a.first) + ' ' + to_string(a.second));
					if (TTSF.haveWay(api.GetSelfInfo()->x, api.GetSelfInfo()->y,
						1000 * TTSF.line[i].first + 500,
						1000 * TTSF.line[i].second + 500))
						check = i;
					/*
					if (a.second > b.second)swap(a, b);
					if ((a.first < b.first ?
						TTSF.crl[b.first][b.second] : TTSF.crr[b.first][b.second]) < a.second)
						check = i;*/
				}
				TTSF.linecnt++;
				//api.Print("CHECK!!! " + to_string(check));
				//cout << "ERROR?" << ' ' << check << endl;
				if (check != -1)
					while (TTSF.line.size() > check + 1)TTSF.line.pop_back();
				TTSF.now = TTSF.line.back();
				TTSF.line.pop_back();
				//else api.Move((i & 1 ? sqrt(2) : 1.0) * 1e6 / api.GetSelfInfo()->speed, acos(-1) * i / 4);
				//cout << i << ' ' << api.GetSelfInfo()->speed << endl;
			}
		}
		else {
			TTSF.now = make_pair(-1, -1), priority = 0x3f3f3f3f;
		}
	}
	if (abs(api.GetSelfInfo()->x - TTSF.adr.first * 1000 - 500) +
		abs(api.GetSelfInfo()->y - TTSF.adr.second * 1000 - 500) < 2000 && priority == 1) { //目标点在尸体， 被尸体卡了
		TTSF.now = make_pair(-1, -1), priority = 0x3f3f3f3f;
	}

	if (TTSF.now != make_pair(-1, -1))
	{
		if (((abs(self->x / 1000 - TTSF.now.first) == 2 && abs(self->y / 1000 - TTSF.now.second) == 0) ||
			(abs(self->x / 1000 - TTSF.now.first) == 0 && abs(self->y / 1000 - TTSF.now.second) == 2)) &&
			TTSF.mp[(self->x / 1000 + TTSF.now.first) / 2][(self->y / 1000 + TTSF.now.second) / 2] == PlaceType::Window)
		{
			int t = 2000 * 1e3 / trickerspeedClimbing[api.GetSelfInfo()->trickerType];
			api.EndAllAction();
			api.SkipWindow();
			TTSF.now = make_pair(-1, -1);
			this_thread::sleep_for(chrono::milliseconds(t));
		}
		else if (abs(api.GetSelfInfo()->x - TTSF.now.first * 1000 - 500) > 20 ||
			abs(api.GetSelfInfo()->y - TTSF.now.second * 1000 - 500) > 20)
		{
			int tx = TTSF.now.first * 1000 + 500 - api.GetSelfInfo()->x,
				ty = TTSF.now.second * 1000 + 500 - api.GetSelfInfo()->y;
			int t = min(200, (int)(sqrt(1.0 * tx * tx + 1.0 * ty * ty) * 1e3 / api.GetSelfInfo()->speed));
			/*if (TTSF.lstgrid == make_pair(api.GetSelfInfo()->x / 1000,
				api.GetSelfInfo()->y / 1000))
			{
				if (api.GetSelfInfo()->playerState != PlayerState::Stunned)
					TTSF.lstgridcnt++;
			}
			else {
				TTSF.lstgrid = make_pair(api.GetSelfInfo()->x / 1000,
					api.GetSelfInfo()->y / 1000);
				TTSF.lstgridcnt = 0;
			}
			if (TTSF.lstgridcnt > 10)
			{
				pair<int, int>to = TTSF.findRecent(api.GetSelfInfo()->x,
					api.GetSelfInfo()->y);
				tx = to.first * 1000 + 500 - api.GetSelfInfo()->x;
				ty = to.second * 1000 + 500 - api.GetSelfInfo()->y;
				api.Move(t = (int)(sqrt(tx * tx + ty * ty) * 1e3 / api.GetSelfInfo()->speed),
					TTSF.lstdir = (ty > 0 ? acos(tx / sqrt(tx * tx + ty * ty)) :
						2 * acos(-1) - acos(tx / sqrt(tx * tx + ty * ty))));
				TTSF.BFS(id, make_pair(x, y), TTSF.adr[id]);
				TTSF.now = make_pair(-1, -1);
				TTSF.lstgridcnt = 0;
			}
			else if (make_pair(api.GetSelfInfo()->x, api.GetSelfInfo()->y) == TTSF.lstpos
				&& api.GetSelfInfo()->playerState != PlayerState::Stunned)//添加按时间（非按次数）判定机制
			{
				api.Move(t = 50, fmod(TTSF.lstdir + acos(-1), 2 * acos(-1)));
				TTSF.BFS(id, make_pair(x, y), TTSF.adr[id]);
				TTSF.now = make_pair(-1, -1);
			}
			else*/
			{
				TTSF.lstpos = make_pair(api.GetSelfInfo()->x, api.GetSelfInfo()->y);
				api.Move(t, TTSF.lstdir = atan2(ty, tx));
			}
			return;
		}
		else TTSF.now = make_pair(-1, -1);
	}
}