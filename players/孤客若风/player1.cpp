#include <vector>
#include <thread>
#include <cmath>
#include <array>
#include "AI.h"
#include "constants.h"
#define MeaningfulValue1 1  // F above this value means the student is valuable enough to use prop AddSpeed
#define MeaningfulValue2 2  // F above this value means the student is valuable enough to use prop ClairAudience
using namespace std;

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = true;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

// 可以在AI.cpp内部声明变量与函数

#pragma region 常量
const int myGridPerSquare = 100;
const int numSquarePerCell = numOfGridPerCell / myGridPerSquare;
const int myRow = Constants::rows * numOfGridPerCell / myGridPerSquare;
const int myCol = Constants::cols * numOfGridPerCell / myGridPerSquare;  // NEW CONSTANT ADDED
#define framet Constants::frameDuration
#pragma endregion

bool firstTime = false;
std::shared_ptr<const THUAI6::Student> selfInfoStudent;
std::shared_ptr<const THUAI6::Tricker> selfInfoTricker;
std::shared_ptr<const THUAI6::Player> selfInfo;
std::shared_ptr<const THUAI6::GameInfo> gameInfo;
std::shared_ptr<const THUAI6::Student> students[4];
int studentsFrame[4];
int TrickerIdlePhase = 0;
double F_parameters[5][7] = {{0, 0, 10000, 0, 0, 0, 0}, {0, 1, 10000, 1, 1, 1, 1}, {0, 20000, 0, 20000, 20000, 20000, 20000}, {0, 200, 0, 200, 200, 200, 200}, {0, 0.4, 0, 0.4, 0.4, 0.4, 0.4}};

int myOriVelocity;
int myVelocity;
#pragma region 自定义辅助
class XY
{
public:
    int x, y;
    XY(int xx, int yy) :
        x(xx),
        y(yy)
    {
    }
    XY()
    {
        x = y = 0;
    }
    XY operator-(const XY& A) const;
    XY operator+(const XY& A) const;
};

XY XY::operator-(const XY& A) const
{
    XY B;
    B.x = this->x - A.x;
    B.y = this->y - A.y;
    return B;
}
XY XY::operator+(const XY& A) const
{
    XY B;
    B.x = this->x + A.x;
    B.y = this->y + A.y;
    return B;
}

class XYSquare : public XY
{
public:
    XYSquare(int xx, int yy) :
        XY(xx, yy)
    {
    }
    XYSquare() :
        XY()
    {
    }
    friend bool operator==(XYSquare comp1, XYSquare comp2);
} selfSquare;

bool operator==(XYSquare comp1, XYSquare comp2)
{
    return (comp1.x == comp2.x && comp1.y == comp2.y);
}

class XYGrid : public XY
{
public:
    XYGrid(int xx, int yy) :
        XY(xx, yy)
    {
    }
    XYGrid() :
        XY()
    {
    }
};
XYSquare GridToSquare(XYGrid xy)
{
    return XYSquare(xy.x / myGridPerSquare, xy.y / myGridPerSquare);
}

XYGrid SquareToGrid(XYSquare xy)
{
    return XYGrid(xy.x * myGridPerSquare + myGridPerSquare / 2, xy.y * myGridPerSquare + myGridPerSquare / 2);
}

class XYCell : public XY
{
public:
    XYCell(int xx, int yy) :
        XY(xx, yy)
    {
    }
    XYCell() :
        XY()
    {
    }
};

XYGrid CellToGrid(XYCell xy)
{
    return XYGrid(xy.x * numOfGridPerCell + numOfGridPerCell / 2, xy.y * numOfGridPerCell + numOfGridPerCell / 2);
}
XYCell numGridToXYCell(int x, int y)
{
    return XYCell(x / numOfGridPerCell, y / numOfGridPerCell);
}
XYCell XYToCell(XY xy)
{
    return XYCell(xy.x, xy.y);
}
int numGridToSquare(int grid)  // Converting no. of grids in a given row to its corresponding square number
{
    return grid / myGridPerSquare;
}

int numGridToCell(int grid)
{
    return grid / numOfGridPerCell;
}

XYSquare numGridToXYSquare(int gridx, int gridy)  // Converting grid coordinates row to its corresponding square instance
{
    return XYSquare(gridx / myGridPerSquare, gridy / myGridPerSquare);
}

XYCell numSquareToXYCell(int squarex, int squarey)
{
    return XYCell(squarex / numSquarePerCell, squarey / numSquarePerCell);
}

bool IsApproach(XYCell cell)
{
    return abs(numGridToCell(selfInfo->x) - cell.x) <= 1 && abs(numGridToCell(selfInfo->y) - cell.y) <= 1;
}
bool ApproachToInteractInACross(XYCell cell)
{
    return (abs(numGridToCell(selfInfo->x) - cell.x) + abs(numGridToCell(selfInfo->y) - cell.y)) <= 1;
}
XYCell SquareToCell(XYSquare xy)
{
    return XYCell(xy.x / numSquarePerCell, xy.y / numSquarePerCell);
}

inline double dist(const XYGrid& vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

int DistanceUP(int x, int y)
{
    return int(sqrt((x - selfInfo->x) * (x - selfInfo->x) + (y - selfInfo->y) * (y - selfInfo->y)) + 1);
}
#pragma endregion

#pragma region Map
std::vector<std::vector<THUAI6::PlaceType>> oriMap;
int speedOriMap[myRow][myRow];   // 0 Wall
int untilTimeMap[myRow][myRow];  //处理门，隐藏校门等
int disMap[myRow][myRow];        //实际距离
XYSquare fromMap[myRow][myRow];

XYCell ClassroomCell[Constants::numOfClassroom];
XYCell openedGateCell[2 + 1];
XYCell GateCell[2];
// XYCell DoorCell
#pragma endregion
// The region above is moved to use oriMap

XYSquare IdleAim = XYSquare(-1, -1);
class BGM_Utilize
{
public:
    void UpdateBGM(double trickDesire, double classVolume);
    inline bool Decreasing() const;
    XYSquare GradientAim() const;
    inline bool DecreasingClassVolume() const;
    inline void GetDir(double dirs[2]) const;
    inline void GetField(bool row, double ret[2]) const;

private:
    double bgm[2][3] = {0};
    double PastDir[2] = {0};
} BGM;

void BGM_Utilize::UpdateBGM(double trickDesire, double classVolume)
{
    this->bgm[0][2] = this->bgm[0][1], this->bgm[1][2] = this->bgm[1][1];
    this->bgm[0][1] = this->bgm[0][0], this->bgm[1][1] = this->bgm[1][0];
    this->bgm[0][0] = trickDesire, this->bgm[1][2] = classVolume;
    PastDir[1] = PastDir[0];
    PastDir[0] = selfInfo->facingDirection;
    return;
}

inline bool BGM_Utilize::Decreasing() const
{
    return (this->bgm[0][0] < this->bgm[0][2] && this->bgm[1][0] < this->bgm[1][2]);
}

inline bool BGM_Utilize::DecreasingClassVolume() const
{
    return this->bgm[1][0] < this->bgm[1][2];
}

inline void BGM_Utilize::GetDir(double dirs[2]) const
{
    dirs[0] = this->PastDir[0];
    dirs[1] = this->PastDir[1];
    return;
}

inline void BGM_Utilize::GetField(bool row, double ret[2]) const
{
    if (row)
    {
        ret[0] = this->bgm[1][0] - this->bgm[1][1];
        ret[1] = this->bgm[1][1] - this->bgm[1][2];
    }
    else
    {
        ret[0] = this->bgm[0][0] - this->bgm[0][1];
        ret[1] = this->bgm[0][1] - this->bgm[0][2];
    }
    return;
}

double SeekDir(double dir1, double dir2, double grad1, double grad2)
{
    double PI = acos(-1.0);
    double a = dir2 - dir1;
    double x = -grad1 + grad2 * cos(a);
    double y = grad2 * sin(a);
    double k = tan(a + PI / 2);
    double b, r;
    b = y + k * (0 - x);
    if (grad1 == 0)
    {
        if (b > 0)
        {
            r = PI / 2;
        }
        else
        {
            r = -PI / 2;
        }
    }
    else
    {
        r = atan2(b, grad1);
    }
    return (r + dir1);
}

XYSquare BGM_Utilize::GradientAim() const
{
    double dirs[2] = {0};
    BGM.GetDir(dirs);
    bool shrink = true;
    double grads[2] = {0};
    printf("GradientAimBegin\n");
    if (DecreasingClassVolume())
        BGM.GetField(true, grads);  // Find larger classVolume
    else
        BGM.GetField(false, grads);                                 // Normally does not happen, but try to be compatible to find larger TrickDesire
    double asylum = SeekDir(dirs[0], dirs[1], grads[0], grads[1]);  // Direction of ascending BGM
    printf("asylum Found\n");
    XYGrid aiming((int)(selfInfoTricker->x + 20000 / bgm[1][2] * cos(asylum)), (int)(selfInfoTricker->y + 20000 / bgm[1][2] * sin(asylum)));
    while ((aiming.x >= 1000 && aiming.x < 49000 && aiming.y >= 1000 && aiming.y < 49000) &&
           (oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Grass && oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Land))
    {
        if (shrink)
        {
            aiming.x -= (int)(round(1000 * cos(asylum)));
            aiming.y -= (int)(round(1000 * sin(asylum)));
            printf("Route shrinking\n");
            if (dist(XYGrid(aiming.x - selfInfoTricker->x, aiming.y - selfInfoTricker->y)) < 1000)
            {
                aiming.x = 2 * selfInfoTricker->x - 25000;
                aiming.y = 2 * selfInfoTricker->y - 25000;
                shrink = false;
            }
        }
        else
        {
            while ((aiming.x >= 1000 && aiming.x < 49000 && aiming.y >= 1000 && aiming.y < 49000) &&
                   (oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Grass && oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Land))
            {
                aiming.x += (int)(round(1000 * cos(asylum)));
                aiming.y += (int)(round(1000 * sin(asylum)));
                printf("Route expanding\n");
            }
            if ((oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Grass && oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Land))
            {
                return GridToSquare(CellToGrid(XYCell(25, 13)));
            }
        }
    }
    return GridToSquare(CellToGrid(SquareToCell(GridToSquare(aiming))));  // Repeated procedures turn target to center of the cell
}

// How a Tricker perceives a Student
class TrickerPerspect
{
public:
    short StudentNumber;
    char MovementInUse;
    THUAI6::StudentType career;
    bool isFixed;
    //-2 symbolizes only former is updated, -1 symbolizes NotUpdated but retains former value, 0 symbolizes NotFoundEver, 1 symbolizes useable and is updating
    TrickerPerspect(short number) :
        StudentNumber(number)
    {
        MovementInUse = 0;
        movement[0] = XYGrid(-1, -1);
        movement[1] = XYGrid(-1, -1);
        this->playerstate = THUAI6::PlayerState::Idle;
        this->F_value = 0;
        this->career = THUAI6::StudentType::NullStudentType;  // Means not acquired yet
        this->isFixed = false;
        this->last_blood_value = 30000000;
        this->addiction = 0;
        this->speed = 3000;
        this->radius = 800;
        this->lastframe = -1;
        this->blacklisted = false;
    }
    inline long long GetLastBlood() const;
    inline XYGrid GetLatestCooridinates() const;
    inline void UpdateBlood(long long new_blood);
    inline void UpdateMovement(XYGrid grid);
    inline void UpdateFValue(long long value);
    inline bool GetMovement(XYGrid* copy) const;
    inline unsigned long long GetFValue() const;
    THUAI6::PlayerState playerstate;
    int lastframe;
    long long addiction;
    long long speed;
    int radius;
    bool blacklisted;

private:
    XYGrid movement[3];
    unsigned long long F_value;
    unsigned long long last_blood_value;
};

inline long long TrickerPerspect::GetLastBlood() const
{
    return this->last_blood_value;
}

inline void TrickerPerspect::UpdateBlood(long long new_blood)
{
    this->last_blood_value = new_blood;
    return;
}

inline void TrickerPerspect::UpdateMovement(XYGrid grid)
{
    if (!((grid.x == -1) && (grid.y == -1)))
    {
        // Update cooridnates
        this->movement[1] = this->movement[0];
        this->movement[0] = grid;
        // Update MovementInUse
        if (MovementInUse == 0 || MovementInUse == -1)
            MovementInUse = -2;
        else if (MovementInUse == -2)
            MovementInUse = 1;
    }
    else
        MovementInUse = -1;
    return;
}

inline XYGrid TrickerPerspect::GetLatestCooridinates() const
{
    return movement[0];
}

inline void TrickerPerspect::UpdateFValue(long long value)
{
    if (value >= 0)
        this->F_value = value;
    return;
}

inline unsigned long long TrickerPerspect::GetFValue() const
{
    return this->F_value;
}

inline bool TrickerPerspect::GetMovement(XYGrid* copy) const
{
    if (MovementInUse == -2 || MovementInUse == 0)
        return false;  // Unable to acquire
    else
    {
        copy[0] = movement[0];
        copy[1] = movement[1];
        return true;
    }
}
TrickerPerspect Trickers_Students[4] = {TrickerPerspect(0), TrickerPerspect(1), TrickerPerspect(2), TrickerPerspect(3)};
short TrickerStatus = -1;  // Initialize. Show the lasting status of the Tricker
short fixation = -1;       // Symbolizes the status of the Tricker's fixation. 0~3 shows Student Number, -1 means not fixed

THUAI6::PlaceType CellPlaceType(XYCell cell)
{
    return oriMap[cell.x][cell.y];
}

void Initialize(const IStudentAPI& api)
{
    if (firstTime)
        return;
    memset(speedOriMap, 0x3f, sizeof(speedOriMap));
    firstTime = true;
    oriMap = api.GetFullMap();
    myOriVelocity = api.GetSelfInfo()->speed;
    int i = 0, j = 0, numClassroom = 0, numGate = 0;
    int l = 0;  // temp

    for (i = 0; i < Constants::rows; ++i)  // traverse all Cells
    {
        for (j = 0; j < Constants::rows; ++j)
        {
            switch (oriMap[i][j])  // To seek the placetype of this square, convert coordinates to cells first
            {
                case THUAI6::PlaceType::Wall:
                case THUAI6::PlaceType::NullPlaceType:
                case THUAI6::PlaceType::Chest:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < min((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 490); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < min((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 490); ++jj)
                            speedOriMap[ii][jj] = 0;
                    break;
                case THUAI6::PlaceType::Gate:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = 0;
                    GateCell[numGate++] = XYCell(i, j);
                    break;
                case THUAI6::PlaceType::ClassRoom:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = 0;
                    ClassroomCell[numClassroom++] = XYCell(i, j);
                    break;
                case THUAI6::PlaceType::Window:  // Cases with windows differ with player type and is non-linear with myOriVelocity
                    switch (api.GetSelfInfo()->studentType)
                    {
                        case THUAI6::StudentType::Athlete:
                            l = Constants::Athlete::speedOfClimbingThroughWindows;
                            break;
                        case THUAI6::StudentType::Teacher:
                            l = Constants::Teacher::speedOfClimbingThroughWindows;
                            break;
                        case THUAI6::StudentType::StraightAStudent:
                            l = Constants::StraightAStudent::speedOfClimbingThroughWindows;
                            break;
                        case THUAI6::StudentType::Sunshine:
                            l = Constants::Sunshine::speedOfClimbingThroughWindows;
                            break;
                        default:
                            l = 0;
                            break;
                    }
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = l;
                    break;
                case THUAI6::PlaceType::Land:
                case THUAI6::PlaceType::Grass:
                default:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = myOriVelocity;
                    break;
            }
        }
    }
}

void Initialize(const ITrickerAPI& api)
{
    if (firstTime)
        return;
    memset(speedOriMap, 0x3f, sizeof(speedOriMap));
    firstTime = true;
    oriMap = api.GetFullMap();
    myOriVelocity = api.GetSelfInfo()->speed;
    int i = 0, j = 0, numClassroom = 0, numGate = 0;
    int l = 0;  // temp

    for (i = 0; i < Constants::rows; ++i)  // traverse all Cells
    {
        for (j = 0; j < Constants::rows; ++j)
        {
            switch (oriMap[i][j])  // To seek the placetype of this square, convert coordinates to cells first
            {
                case THUAI6::PlaceType::Wall:
                case THUAI6::PlaceType::NullPlaceType:
                case THUAI6::PlaceType::Chest:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < min((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 490); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < min((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 490); ++jj)
                            speedOriMap[ii][jj] = 0;
                    break;
                case THUAI6::PlaceType::Gate:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = 0;
                    GateCell[numGate++] = XYCell(i, j);
                    break;
                case THUAI6::PlaceType::ClassRoom:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = 0;
                    ClassroomCell[numClassroom++] = XYCell(i, j);
                    break;
                case THUAI6::PlaceType::Window:  // Cases with windows differ with player type and is non-linear with myOriVelocity
                    l = Constants::Assassin::speedOfClimbingThroughWindows;
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = l;
                    break;
                case THUAI6::PlaceType::Land:
                case THUAI6::PlaceType::Grass:
                default:
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            speedOriMap[ii][jj] = myOriVelocity;
                    break;
            }
        }
    }
    for (l = 0; l < 4; ++l)
        Trickers_Students[i] = TrickerPerspect(l);
}
void drawMap(const ITrickerAPI& api)
{
    memset(untilTimeMap, 0x3f, sizeof(untilTimeMap));
    for (int i = 0; i < Constants::rows; ++i)  // traverse all Cells
    {
        for (int j = 0; j < Constants::rows; ++j)
        {
            switch (oriMap[i][j])  // To seek the placetype of this square, convert coordinates to cells first
            {
                case THUAI6::PlaceType::Wall:
                case THUAI6::PlaceType::NullPlaceType:
                case THUAI6::PlaceType::ClassRoom:
                case THUAI6::PlaceType::Gate:
                case THUAI6::PlaceType::Chest:
                    //
                case THUAI6::PlaceType::Door6:
                case THUAI6::PlaceType::Door3:
                case THUAI6::PlaceType::Door5:
                case THUAI6::PlaceType::HiddenGate:
                    //   for (int k=)
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            untilTimeMap[ii][jj] = -1;
                    break;
                default:
                    break;
            }
        }
    }
    auto tl = api.GetStudents();
    for (int n = 0; n < tl.size(); ++n)
        if (Trickers_Students[tl[n]->playerID].addiction > 0 || Trickers_Students[tl[n]->playerID].career == THUAI6::StudentType::Teacher)
        {
            int tx = numGridToSquare(tl[n]->x), ty = numGridToSquare(tl[n]->y);
            for (int i = max(tx - 10, 5); i < min(tx + 10, 496); ++i)
                for (int j = max(ty - 10, 5); j < min(ty + 10, 496); ++j)
                    untilTimeMap[i][j] = -1;
        }
}

void drawMap(const IStudentAPI& api)
{
    memset(untilTimeMap, 0x3f, sizeof(untilTimeMap));
    for (int i = 0; i < Constants::rows; ++i)  // traverse all Cells
    {
        for (int j = 0; j < Constants::rows; ++j)
        {
            switch (oriMap[i][j])  // To seek the placetype of this square, convert coordinates to cells first
            {
                case THUAI6::PlaceType::Wall:
                case THUAI6::PlaceType::NullPlaceType:
                case THUAI6::PlaceType::ClassRoom:
                case THUAI6::PlaceType::Gate:
                case THUAI6::PlaceType::Chest:
                    //
                case THUAI6::PlaceType::Door6:
                case THUAI6::PlaceType::Door3:
                case THUAI6::PlaceType::Door5:
                case THUAI6::PlaceType::HiddenGate:
                    //   for (int k=)
                    for (int ii = max((i - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); ii < max((i + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++ii)
                        for (int jj = max((j - 1) * numSquarePerCell + numSquarePerCell / 2 + 1, 0); jj < max((j + 1) * numSquarePerCell + numSquarePerCell / 2 - 1, 0); ++jj)
                            untilTimeMap[ii][jj] = -1;
                    break;
                default:
                    break;
            }
        }
    }
    auto t = api.GetTrickers();
    if (t.size())
    {
        int tx = numGridToSquare(t[0]->x), ty = numGridToSquare(t[0]->y);
        for (int i = max(tx - 30, 5); i < min(tx + 30, 496); ++i)
            for (int j = max(ty - 30, 5); j < min(ty + 30, 496); ++j)
                untilTimeMap[i][j] = -1;
    }
    auto tl = api.GetStudents();
    for (int n = 0; n < 4; ++n)
        if (tl[n]->playerID != selfInfo->playerID)
        {
            int tx = numGridToSquare(tl[n]->x), ty = numGridToSquare(tl[n]->y);
            for (int i = max(tx - 10, 5); i < min(tx + 10, 496); ++i)
                for (int j = max(ty - 10, 5); j < min(ty + 10, 496); ++j)
                    untilTimeMap[i][j] = -1;
        }
}

int xq[myRow * myRow << 2], yq[myRow * myRow << 2];
int v[myRow][myRow];
inline void SPFAin(int x, int y, int xx, int yy, int vd, int l, int& r)
{
    if (untilTimeMap[x][yy] > framet && untilTimeMap[xx][y] > framet)
    {
        // printf("%d %d %d %d SPFA\n", x, y, xx, yy);
        if ((disMap[x][y] + vd) * 1000 < myVelocity * ((long long)untilTimeMap[xx][yy] - framet))
            if (disMap[xx][yy] > disMap[x][y] + vd)
            {
                //        if(xx==225)  printf("%d %d\n",xx,yy);
                disMap[xx][yy] = disMap[x][y] + vd;
                fromMap[xx][yy] = XYSquare(x, y);
                if (!v[xx][yy])
                    xq[++r] = xx, yq[r] = yy, v[xx][yy] = 1;
            }
    }
}

inline void SPFA()
{
    memset(disMap, 0x3f, sizeof(disMap));
    memset(v, 0, sizeof(v));  // 节点标记
    int l = 0, r = 1;
    disMap[selfSquare.x][selfSquare.y] = disMap[selfSquare.x][selfSquare.y] = 0;
    v[selfSquare.x][selfSquare.y] = 1;
    fromMap[selfSquare.x][selfSquare.y] = selfSquare;
    xq[1] = selfSquare.x;
    yq[1] = selfSquare.y;
    while (l < r)
    {
        ++l;
        int x = xq[l], y = yq[l];
        v[x][y] = 0;

        int xx = x - 1, yy = y - 1;
        int vd;
        vd = int(round((1 + 1.4142 * myGridPerSquare) * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity)));
        SPFAin(x, y, xx, yy, vd, l, r);

        xx += 2;
        vd = int(round((1 + 1.4142 * myGridPerSquare) * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity)));
        SPFAin(x, y, xx, yy, vd, l, r);

        yy += 2;
        vd = int(round((1 + 1.4142 * myGridPerSquare) * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity)));
        SPFAin(x, y, xx, yy, vd, l, r);

        xx -= 2;
        vd = int(round((1 + 1.4142 * myGridPerSquare) * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity)));
        SPFAin(x, y, xx, yy, vd, l, r);

        --yy;
        vd = myGridPerSquare * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity);
        SPFAin(x, y, xx, yy, vd, l, r);

        xx += 2;
        vd = myGridPerSquare * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity);
        SPFAin(x, y, xx, yy, vd, l, r);

        --xx;
        --yy;
        vd = myGridPerSquare * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity);
        SPFAin(x, y, xx, yy, vd, l, r);

        yy += 2;
        vd = myGridPerSquare * (speedOriMap[x][y] + speedOriMap[xx][yy]) / (2 * myOriVelocity);
        SPFAin(x, y, xx, yy, vd, l, r);
    }
}

void Move(IAPI& api, XYSquare toMove)
{
    // 包括翻窗
    api.EndAllAction();
    XYCell cell = XYToCell(SquareToCell(selfSquare) + (toMove - selfSquare));
    if (CellPlaceType(cell) == THUAI6::PlaceType::Window && ApproachToInteractInACross(cell))
    {
        api.SkipWindow();
        return;
    }
    int x = SquareToGrid(toMove).x;
    int y = SquareToGrid(toMove).y;
    api.Print("AboutToMove\n");
    api.Move(60, atan2(y - selfInfo->y, x - selfInfo->x));
    api.Print("Moved\n");
}

XYSquare FindMoveNext(XYSquare toMove)
{
    // printf("%d %d MoveNext\n", toMove.x, toMove.y);
    int x = toMove.x, y = toMove.y, lx = toMove.x, ly = toMove.x;
    if (x <= 0 || y <= 0)
    {
        printf("Erroneous pos at 714\n");
        return selfSquare;
    }
    while (x != selfSquare.x || y != selfSquare.y)
    {
        lx = x;
        ly = y;
        x = fromMap[lx][ly].x;
        y = fromMap[lx][ly].y;
        printf("MoveNextFound\n");
        if (x == lx && y == ly && (x != selfSquare.x || y != selfSquare.y))
        {
            printf("x= %d, y = %d, lx = %d, ly = %d\n", x, y, lx, ly);
            break;
        }
    }
    return XYSquare(lx, ly);
}

XYSquare FindNearInCell(XYCell cell)
{
    int i = max((cell.x - 1) * numSquarePerCell, 10);
    int j = max((cell.y - 1) * numSquarePerCell, 10);
    int d = disMap[i][j];
    XYSquare toFind = XYSquare(i, j);
    for (; j < min(490, (cell.y + 2) * numSquarePerCell); ++j)
        if (disMap[i][j] < d)
        {
            d = disMap[i][j];
            toFind = XYSquare(i, j);
        }
    --j;
    for (; i < min(490, (cell.x + 2) * numSquarePerCell); ++i)
        if (disMap[i][j] < d)
        {
            d = disMap[i][j];
            toFind = XYSquare(i, j);
        }
    --i;
    for (; j >= max((cell.y - 1) * numSquarePerCell, 10); --j)
        if (disMap[i][j] < d)
        {
            d = disMap[i][j];
            toFind = XYSquare(i, j);
        }
    ++j;
    for (; i >= max((cell.x - 1) * numSquarePerCell, 10); --i)
        if (disMap[i][j] < d)
        {
            d = disMap[i][j];
            toFind = XYSquare(i, j);
        }
    return toFind;
}

XYCell FindActualClassroom(ITrickerAPI& api)  // Instead of finding a cell near a classroom, we need actual coordinates of one
{
    double min = 100000;
    int i = 0, ind = -1;
    for (i = 0; i < 10; ++i)
    {
        XYGrid vec = CellToGrid(ClassroomCell[i]);
        vec.x -= selfInfo->x, vec.y -= selfInfo->y;
        if (dist(vec) < min)
        {
            ind = i;
            min = dist(vec);
        }
    }
    if (ind == -1)
        return XYCell(-1, -1);
    else
        return ClassroomCell[ind];
}

XYSquare FindClassroom()
{
    XYSquare toFind = FindNearInCell(ClassroomCell[0]);
    int d = disMap[toFind.x][toFind.y];
    // printf("%d %d %d Class\n", toFind.x, toFind.y, disMap[toFind.x][toFind.y]);
    for (int i = 1; i < Constants::numOfClassroom; ++i)
    {
        XYSquare t = FindNearInCell(ClassroomCell[i]);
        if (disMap[t.x][t.y] < d)
        {
            toFind = t;
            d = disMap[toFind.x][toFind.y];
        }
        // printf("%d %d %d %d Class\n", t.x, t.y, i, disMap[t.x][t.y]);
    }
    return toFind;
}

XYSquare FindGate()
{
    // if (gameInfo->studentGraduated + gameInfo->studentQuited==3&& gameInfo->subjectFinished >= Constants::numOfRequiredClassroomForHiddenGate)
    if (gameInfo->subjectFinished < Constants::numOfRequiredClassroomForGate)
        return XYSquare(0, 0);

    XYSquare toFind = FindNearInCell(GateCell[0]);
    int d = disMap[toFind.x][toFind.y];

    XYSquare t = FindNearInCell(GateCell[1]);
    if (disMap[t.x][t.y] < d)
    {
        toFind = t;
        d = disMap[toFind.x][toFind.y];
    }
    return toFind;
    //隐藏校门或校门
    // return (0,0)false
}

bool Commandable(const IAPI& api)
{
    auto p = selfInfo->playerState;
    //  printf("ok\n");
    //   api.PrintSelfInfo();
    return p != THUAI6::PlayerState::Addicted && p != THUAI6::PlayerState::Roused && p != THUAI6::PlayerState::Climbing && p != THUAI6::PlayerState::Attacking && p != THUAI6::PlayerState::Graduated && p != THUAI6::PlayerState::Quit && p != THUAI6::PlayerState::Stunned && p != THUAI6::PlayerState::Swinging;
}

void Update(const IStudentAPI& api)
{
    gameInfo = api.GetGameInfo();
    selfInfoStudent = api.GetSelfInfo();
    selfInfo = selfInfoStudent;
    selfSquare = numGridToXYSquare(selfInfoStudent->x, selfInfoStudent->y);
    // printf("%d %d self\n", selfSquare.x, selfSquare.y);
    for (int i = 0; i < Constants::numOfClassroom; ++i)
        if (api.GetClassroomProgress(ClassroomCell[i].x, ClassroomCell[i].y) == 10000000)
            ClassroomCell[i] = XYCell(0, 0);
    myVelocity = selfInfo->speed;
    // openedGateCell
    for (int i = 0; i < 2; ++i)
        if (api.GetGateProgress(ClassroomCell[i].x, ClassroomCell[i].y) == 18000)
            openedGateCell[i] = GateCell[i];
}

inline long long getMaxBlood(int number)
{
    switch (Trickers_Students[number].career)
    {
        case THUAI6::StudentType::Teacher:
            return 30000000;
            break;
        case THUAI6::StudentType::Athlete:
            return 3000000;
            break;
        case THUAI6::StudentType::StraightAStudent:
            return 3300000;
            break;
        case THUAI6::StudentType::Sunshine:
            return 3200000;
            break;
        case THUAI6::StudentType::Robot:
            return 3000;  // Needs replacing
            break;
        case THUAI6::StudentType::TechOtaku:
            return 900000;  // Needs replacing
            break;
        default:
            return 30000000;
            break;
    }
}

inline long long getMaxAddiction(short fixation)
{
    switch (Trickers_Students[fixation].career)
    {
        case THUAI6::StudentType::Teacher:
            return 600000;
            break;
        case THUAI6::StudentType::Athlete:
            return 54000;
            break;
        case THUAI6::StudentType::StraightAStudent:
            return 78000;
            break;
        case THUAI6::StudentType::Sunshine:
            return 66000;
            break;
        case THUAI6::StudentType::Robot:
            return 3000;  // Needs replacing
            break;
        case THUAI6::StudentType::TechOtaku:
            return 90000;  // Needs replacing
            break;
        default:
            return 600000;
            break;
    }
}

void Update(const ITrickerAPI& api)
{
    gameInfo = api.GetGameInfo();
    selfInfoTricker = api.GetSelfInfo();
    selfInfo = selfInfoTricker;
    selfSquare = numGridToXYSquare(selfInfoTricker->x, selfInfoTricker->y);
    for (int i = 0; i < Constants::numOfClassroom; ++i)
    {
        if (api.GetClassroomProgress(ClassroomCell[i].x, ClassroomCell[i].y) == 10000000)
            ClassroomCell[i] = XYCell(0, 0);
    }
    myVelocity = selfInfo->speed;
    auto stu = api.GetStudents();
    for (int i = 0; i < 4; ++i)
        Trickers_Students[i].blacklisted = false;
    if (stu.size() > 0)
    {
        for (int i = 0; i < stu.size(); ++i)
        {
            students[stu[i]->playerID] = stu[i], studentsFrame[stu[i]->playerID] = api.GetFrameCount();
            if (Trickers_Students[stu[i]->playerID].career == THUAI6::StudentType::NullStudentType)
            {
                Trickers_Students[stu[i]->playerID].career = stu[i]->studentType;
                Trickers_Students[stu[i]->playerID].StudentNumber = short(stu[i]->playerID);
                Trickers_Students[stu[i]->playerID].radius = stu[i]->radius;
                Trickers_Students[stu[i]->playerID].speed = stu[i]->speed;
            }
            if (Trickers_Students[stu[i]->playerID].addiction > getMaxAddiction((short)(stu[i]->playerID)) * 0.12)
                Trickers_Students[stu[i]->playerID].blacklisted = true;
            Trickers_Students[stu[i]->playerID].UpdateMovement(XYGrid(stu[i]->x, stu[i]->y));
            Trickers_Students[stu[i]->playerID].addiction = stu[i]->addiction;
            Trickers_Students[stu[i]->playerID].playerstate = stu[i]->playerState;
            Trickers_Students[stu[i]->playerID].UpdateBlood(stu[i]->determination);
            Trickers_Students[stu[i]->playerID].lastframe = api.GetFrameCount();
        }
    }
    // openedGateCell
    for (int i = 0; i < 2; ++i)
        if (api.GetGateProgress(ClassroomCell[i].x, ClassroomCell[i].y) == 18000)
            openedGateCell[i] = GateCell[i];
}

void Flee(IStudentAPI& api)
{
    int i = selfSquare.x - 1, j = selfSquare.y - 1;
    int maxtime = untilTimeMap[i][j], maxi = i, maxj = j;
    for (; i <= selfSquare.x + 1; ++i)
        for (j = selfSquare.y - 1; j <= selfSquare.y + 1; ++j)
            if (maxtime < untilTimeMap[i][j])
            {
                maxtime = untilTimeMap[i][j];
                maxi = i;
                maxj = j;
            }
    Move(api,XYSquare(maxi,maxj));
}

bool TryToLearn(IStudentAPI& api)
{
    for (int i = 0; i < Constants::numOfClassroom; ++i)
        if (ClassroomCell[i].x != 0 && IsApproach(ClassroomCell[i]))
        {
            if (selfInfo->timeUntilSkillAvailable[0] == 0 && selfInfo->playerState != THUAI6::PlayerState::Learning)
                api.UseSkill(0);
            api.StartLearning();
            return true;
        }
    return false;
}

bool TryToGraduate(IStudentAPI& api)
{
    return api.Graduate().get();
}

bool TryToOpenGate(IStudentAPI& api)
{
    for (int i = 0; i < 2; ++i)
        if (GateCell[i].x != 0 && IsApproach(GateCell[i]))
        {
            // if (selfInfo->)
            api.StartOpenGate();
            printf("OK!");
            return true;
        }
    return false;
}

void AI::play(IStudentAPI& api)
{
    Update(api);
    Initialize(api);
    api.Print("update\n");
    if (Commandable(api) && !TryToGraduate(api))
    {
        drawMap(api);
        SPFA();
        api.Print("SPFA\n");
        if (untilTimeMap[selfSquare.x][selfSquare.y] <= framet)Flee(api);
        else
        {
        if (!TryToOpenGate(api))
        {
            XYSquare toGateSquare = FindGate();
            if (toGateSquare.x != 0)
            {
                Move(api, FindMoveNext(toGateSquare));
                // api.Print("!!!\n");
            }
            else
            {
                if (!TryToLearn(api))
                {
                    //    api.Print("!!!!!\n");
                    Move(api, FindMoveNext(FindClassroom()));
                }
            }
        }
        }
    }
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
}

XYSquare FindStudent()
{
    int i = 0;
    while (students[i] == nullptr && i < 4)
        ++i;
    if (i == 4)
        return XYSquare(-1, -1);
    XYSquare toFind = FindNearInCell(numGridToXYCell(students[i]->x, students[i]->y));
    int d = disMap[toFind.x][toFind.y];
    // printf("%d %d %d Class\n", toFind.x, toFind.y, disMap[toFind.x][toFind.y]);
    for (; i < 4; ++i)
        if (students[i] != nullptr && students[i]->playerState != THUAI6::PlayerState::Addicted && students[i]->playerState != THUAI6::PlayerState::Roused)
        {
            XYSquare t = FindNearInCell(numGridToXYCell(students[i]->x, students[i]->y));
            if (disMap[t.x][t.y] < d)
            {
                toFind = t;
                d = disMap[toFind.x][toFind.y];
            }
        }
    return toFind;
}

bool TryToAttack(ITrickerAPI& api, short maxstudent)
{
    int attackrange = int((selfInfo->bulletType == THUAI6::BulletType::CommonAttackOfTricker) ? Constants::CommonAttackOfTricker::BulletAttackRange : Constants::FlyingKnife::BulletAttackRange);
    int xx = Trickers_Students[maxstudent].GetLatestCooridinates().x, yy = Trickers_Students[maxstudent].GetLatestCooridinates().y;
    THUAI6::PlaceType aimed = oriMap[SquareToCell(GridToSquare(Trickers_Students[maxstudent].GetLatestCooridinates())).x]
                                    [SquareToCell(GridToSquare(Trickers_Students[maxstudent].GetLatestCooridinates())).y];
    if ((Trickers_Students[maxstudent].playerstate != THUAI6::PlayerState::Addicted && Trickers_Students[maxstudent].playerstate != THUAI6::PlayerState::Roused) && (aimed == THUAI6::PlaceType::Grass || aimed == THUAI6::PlaceType::Land))
    {
        if (selfInfo->timeUntilSkillAvailable[0] == 0)
            api.UseSkill(0);
        XYGrid TrickerToStudent(xx - selfInfo->x, yy - selfInfo->y);
        if (dist(TrickerToStudent) + 1 < attackrange)
        {
            api.Attack(atan2(TrickerToStudent.y, TrickerToStudent.x));
            return true;
        }
        else
        {
            if (selfInfo->timeUntilSkillAvailable[1] == 0)
            {
                api.UseSkill(1);
                api.Attack(atan2(TrickerToStudent.y, TrickerToStudent.x));
                return true;
            }
            else
                return false;
        }
    }
    return false;
}

void UpdateF(ITrickerAPI& api, int number)
{
    if (Trickers_Students[number].playerstate == THUAI6::PlayerState::Quit)
    {
        Trickers_Students[number].UpdateFValue(0);
        return;
    }
    int defaultParameter = 333;
    XYSquare MachineSquare = FindClassroom();
    // Xs-Xm
    XYGrid StudentToMachine(abs(Trickers_Students[number].GetLatestCooridinates().x - SquareToGrid(MachineSquare).x), abs(Trickers_Students[number].GetLatestCooridinates().y - SquareToGrid(MachineSquare).y));
    // Xs-Xt
    XYGrid StudentToTricker(abs(Trickers_Students[number].GetLatestCooridinates().x - selfInfo->x), abs(Trickers_Students[number].GetLatestCooridinates().y - selfInfo->y));
    api.Print("923\n");
    if (Trickers_Students[number].career == THUAI6::StudentType::Teacher)
    {
        if (api.GetGameInfo()->studentGraduated + api.GetGameInfo()->studentQuited == 3)  // Only one left in play
        {
            Trickers_Students[number].UpdateFValue((long long)(round((F_parameters[0][int(THUAI6::StudentType::Teacher)]) + dist(StudentToMachine) / (F_parameters[1][int(THUAI6::StudentType::Teacher)] * dist(StudentToTricker)))));
        }
        else
            Trickers_Students[number].UpdateFValue(0);
        api.Print("931\n");
    }
    else
    {
        int i = 0, j = 0;
        double smDist = dist(StudentToMachine);
        double stDist = dist(StudentToTricker);
        api.Print("937\n");
        double R = 5 - 2 * log2(4 - api.GetGameInfo()->studentGraduated - api.GetGameInfo()->studentQuited);  // Coefficient
        auto stu = api.GetStudents();
        bool WithinSight = false;
        // Check whether students[number] is within sight
        for (i = 0; i < stu.size(); i++)
        {
            if (stu[i]->playerID == number)
            {
                WithinSight = true;
                break;
            }
        }
        double Blood = 0;
        api.Print("950\n");
        if (WithinSight)
            Blood = double(Trickers_Students[number].GetLastBlood());
        else
        {
            long long fullBlood = getMaxBlood(number);
            if (Trickers_Students[number].career == THUAI6::StudentType::NullStudentType)
                Blood = fullBlood - (fullBlood - Trickers_Students[number].GetLastBlood()) * pow(0.998, api.GetFrameCount() - Trickers_Students[number].lastframe);
            else
                Blood = double(fullBlood);
        }
        if (WithinSight)
        {
            double RD = F_parameters[2][int(Trickers_Students[number].career)] / smDist;
            double Cluster = 0;
            if (stu.size() > 1)
            {
                for (i = 0; i < stu.size(); i++)
                {
                    if (i != number && Trickers_Students[number].playerstate != THUAI6::PlayerState::Quit)
                        Cluster += F_parameters[3][int(Trickers_Students[number].career)] * F_parameters[3][int(Trickers_Students[stu[i]->playerID].career)] /
                                   (Trickers_Students[number].radius * 2 + dist(XYGrid(abs(Trickers_Students[number].GetLatestCooridinates().x - stu[i]->x), abs(Trickers_Students[number].GetLatestCooridinates().y - stu[i]->y))));
                }
            }
            double Dir = 1 + F_parameters[1][int(Trickers_Students[number].career)] *
                                 (StudentToMachine.x * StudentToTricker.x + StudentToMachine.y * StudentToTricker.y) /
                                 (StudentToMachine.x * StudentToMachine.x + StudentToMachine.y * StudentToMachine.y);
            Trickers_Students[number].UpdateFValue((long long)(round(R * RD * Dir * (1 - F_parameters[4][int(Trickers_Students[number].career)] * Blood + Cluster))));
        }
        else
        {
            Trickers_Students[number].UpdateFValue((long long)(round(R * Blood * defaultParameter)));
        }
        api.Print("982\n");
    }
    api.Print("983\n");
    return;
}

short FindMaxStudent(ITrickerAPI& api)  // Find Student with highest F-value
{
    std::vector<std::shared_ptr<const THUAI6::Student>> stu = api.GetStudents();
    int i = 0;
    short ret = 0;
    long long max = 0, curr = 0;
    for (i = 0; i < stu.size(); ++i)
    {
        if (Trickers_Students[stu[i]->playerID].isFixed && Trickers_Students[stu[i]->playerID].blacklisted == false)
            curr = Trickers_Students[stu[i]->playerID].GetFValue() + 50;  // Constant needs replacement
        else if (Trickers_Students[stu[i]->playerID].blacklisted == false)
            curr = Trickers_Students[stu[i]->playerID].GetFValue();
        else
            curr = 0;
        if (curr > max)
        {
            max = curr;
            ret = short(stu[i]->playerID);
        }
    }
    api.Print("1004\n");
    if (max > 0)
        return ret;
    else
        return -1;
}

XYSquare FindStudentSquare(short no)  // Find destination when chasing a Student
{
    XYGrid movement[2];
    bool getmove = Trickers_Students[no].GetMovement(movement);
    if (getmove)  // Able to acquire movement
    {
        int delta_frame = int(round(myGridPerSquare * numSquarePerCell / Trickers_Students[no].speed * 1000 / framet));
        XYGrid deltaGrid((movement[0].x - movement[1].x) * delta_frame, (movement[0].y - movement[1].y) * delta_frame);
        XYCell aimCell = SquareToCell(GridToSquare(XYGrid(movement[0].x + deltaGrid.x, movement[0].y + deltaGrid.y)));
        if (oriMap[aimCell.x][aimCell.y] == THUAI6::PlaceType::Land || oriMap[aimCell.x][aimCell.y] == THUAI6::PlaceType::Grass)
            return GridToSquare(CellToGrid(SquareToCell(GridToSquare(XYGrid(movement[0].x + deltaGrid.x, movement[0].y + deltaGrid.y)))));
        else
            return GridToSquare(CellToGrid(SquareToCell(GridToSquare(movement[0]))));
    }
    else
        return GridToSquare(CellToGrid(SquareToCell(GridToSquare(movement[0]))));
}

short FindNextMaxStudent(ITrickerAPI& api, short avoid)  // When max is addicted, find next
{
    std::vector<std::shared_ptr<const THUAI6::Student>> stu = api.GetStudents();
    short i = 0;
    short ret = 0;
    api.Print("1029\n");
    long long max = 0, curr = 0;
    for (i = 0; i < stu.size(); ++i)
    {
        if (i == avoid || Trickers_Students[stu[i]->playerID].blacklisted)
            continue;
        curr = Trickers_Students[stu[i]->playerID].GetFValue();
        if (curr > max)
        {
            max = curr;
            ret = short(stu[i]->playerID);
        }
    }
    api.Print("1041\n");
    if (max > 0)
        return ret;
    else
        return -1;
}

XYSquare FindGuardSquare(ITrickerAPI& api, short guard, short target)  // Find square to sit vigil
{
    XYGrid arr(Trickers_Students[target].GetLatestCooridinates().x - Trickers_Students[guard].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y - Trickers_Students[guard].GetLatestCooridinates().y);
    double theta = atan2(arr.y, arr.x);
    if (dist(arr) > 5 * Trickers_Students[guard].radius * 3)  // Able to intercept
    {
        XYCell aim(SquareToCell(GridToSquare(XYGrid((int)(round(Trickers_Students[guard].GetLatestCooridinates().x + Trickers_Students[guard].radius * 3 * cos(theta))), int(round(Trickers_Students[guard].GetLatestCooridinates().y + Trickers_Students[guard].radius * 6 * sin(theta)))))));
        if (aim.x <= 0)
            return XYSquare(-1, -1);
        THUAI6::PlaceType aimed = oriMap[aim.x][aim.y];
        api.Print("1055\n");
        while (aimed != THUAI6::PlaceType::Grass && aimed != THUAI6::PlaceType::Land && aimed != THUAI6::PlaceType::HiddenGate)  // Out of reach
        {
            XYGrid arr1(selfInfo->x - Trickers_Students[guard].GetLatestCooridinates().x, selfInfo->y - Trickers_Students[guard].GetLatestCooridinates().y);
            double theta1 = atan2(arr1.y, arr1.x);
            theta += (theta1 - theta) * 0.1;
            aim = SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[guard].GetLatestCooridinates().x + Trickers_Students[guard].radius * 3 * cos(theta))), int(round(Trickers_Students[guard].GetLatestCooridinates().y + Trickers_Students[guard].radius * 6 * sin(theta))))));
            aimed = oriMap[aim.x][aim.y];
        }
        return GridToSquare(CellToGrid(aim));
    }
    else  // Unable to intercept
    {
        api.Print("1067\n");
        XYGrid arr1(selfInfo->x - Trickers_Students[guard].GetLatestCooridinates().x, selfInfo->y - Trickers_Students[guard].GetLatestCooridinates().y);
        double theta1 = atan2(arr1.y, arr1.x);
        XYCell aim(SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[guard].GetLatestCooridinates().x + Trickers_Students[guard].radius * 3 * cos(0.5 * theta + 0.5 * theta1))), int(round(Trickers_Students[guard].GetLatestCooridinates().y + Trickers_Students[guard].radius * 3 * sin(0.5 * theta + 0.5 * theta1)))))));
        THUAI6::PlaceType aimed = api.GetPlaceType(aim.x, aim.y);
        while (aimed != THUAI6::PlaceType::Grass && aimed != THUAI6::PlaceType::Land && aimed != THUAI6::PlaceType::HiddenGate)  // Unable to enter
        {
            theta += (theta1 - theta) * 0.2;
            aim = SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[guard].GetLatestCooridinates().x + Trickers_Students[guard].radius * 3 * cos(0.5 * theta + 0.5 * theta1))), int(round(Trickers_Students[guard].GetLatestCooridinates().y + Trickers_Students[guard].radius * 3 * sin(0.5 * theta + 0.5 * theta1))))));
            aimed = api.GetPlaceType(aim.x, aim.y);
        }
        return GridToSquare(CellToGrid(SquareToCell(GridToSquare(CellToGrid(aim)))));
    }
}

XYSquare SeekDisappearSquare(ITrickerAPI& api, short target)
{
    if (Trickers_Students[target].MovementInUse == -1 || (Trickers_Students[target].MovementInUse == 1))  // Able to get movement
    {
        XYGrid movement[3];
        api.Print("1086\n");
        Trickers_Students[target].GetMovement(movement);
        movement[2] = XYGrid(movement[0].x + movement[0].x - movement[1].x, movement[0].y + movement[0].y - movement[1].y);
        XYCell cell = SquareToCell(GridToSquare(movement[2]));  // Convert to xycell
        THUAI6::PlaceType aim = oriMap[cell.x][cell.y];
        if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
            aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
        {
            if (api.HaveView(movement[2].x, movement[2].y) &&
                (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(cell.x, cell.y)))  // Can see a locked door
                return GridToSquare(movement[0]);                                                                                                              // Not worthy of pursuing
            else
                return GridToSquare(CellToGrid(cell));  // Suppose continue on a straight line
        }
    }
    api.Print("1100\n");
    // Traverse adjacent cells
    XYCell suppose = SquareToCell(GridToSquare(Trickers_Students[target].GetLatestCooridinates()));
    if (suppose.x < 0)
        return XYSquare(-1, -1);  // ERROR
    THUAI6::PlaceType aim = oriMap[suppose.x][suppose.y];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(suppose.x, suppose.y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                             // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(suppose));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x + 1][suppose.y];
    int delta = int(round(Trickers_Students[target].speed / 1000 * framet));
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(Trickers_Students[target].GetLatestCooridinates().x + delta, Trickers_Students[target].GetLatestCooridinates().y) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x + delta, Trickers_Students[target].GetLatestCooridinates().y))).x, SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x + delta, Trickers_Students[target].GetLatestCooridinates().y))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                       // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x + 1, suppose.y)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x][suppose.y + 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y + delta) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y + delta))).x, SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y + delta))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                       // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x, suppose.y + 1)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x - 1][suppose.y];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(Trickers_Students[target].GetLatestCooridinates().x - delta, Trickers_Students[target].GetLatestCooridinates().y) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x - delta, Trickers_Students[target].GetLatestCooridinates().y))).x, SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x - delta, Trickers_Students[target].GetLatestCooridinates().y + delta))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                               // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x - 1, suppose.y)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x][suppose.y - 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y - delta) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y - delta))).x, SquareToCell(GridToSquare(XYGrid(Trickers_Students[target].GetLatestCooridinates().x, Trickers_Students[target].GetLatestCooridinates().y - delta))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                       // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x, suppose.y - 1)));  // Suppose continue on a straight line
    }

    // Now for diagonals
    aim = oriMap[suppose.x + 1][suppose.y + 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))))).x, SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x + 1, suppose.y + 1)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x - 1][suppose.y + 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))))).x, SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y + 0.8 * delta))))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x - 1, suppose.y + 1)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x - 1][suppose.y - 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))))).x, SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x - 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x - 1, suppose.y - 1)));  // Suppose continue on a straight line
    }
    aim = oriMap[suppose.x + 1][suppose.y - 1];
    if (aim == THUAI6::PlaceType::Grass || aim == THUAI6::PlaceType::Window ||
        aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6)
    {
        if (api.HaveView(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))) &&
            (aim == THUAI6::PlaceType::Door3 || aim == THUAI6::PlaceType::Door5 || aim == THUAI6::PlaceType::Door6) && !(api.IsDoorOpen(SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))))).x, SquareToCell(GridToSquare(XYGrid(int(round(Trickers_Students[target].GetLatestCooridinates().x + 0.8 * delta)), int(round(Trickers_Students[target].GetLatestCooridinates().y - 0.8 * delta))))).y)))  // Can see a locked door
            return XYSquare(-1, -1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // Not worthy of pursuing
        else
            return GridToSquare(CellToGrid(XYCell(suppose.x + 1, suppose.y - 1)));  // Suppose continue on a straight line
    }
    api.Print("1195\n");
    return XYSquare(-1, -1);  // Disappearing square not found
}

XYSquare SeekInvisibleStudent(ITrickerAPI& api)
{
    XYSquare ret(-1, -1);
    double grads[2] = {0};
    BGM.GetField(true, grads);
    api.Print("SeekInvisibleStudentBegin\n");
    if (abs(grads[0]) < 0.02 && abs(grads[1]) < 0.02)  // No significant change
    {
        BGM.GetField(false, grads);
        api.Print("Field false Got\n");
        if (abs(grads[0]) < 0.02 && abs(grads[1]) < 0.02)  // Still no change
        {
            XYGrid aiming(2 * selfInfo->x - 25000, 2 * selfInfo->y - 25000);
            api.Print("Seekinvisiblestudent->nochange");
            double asylum = atan2(aiming.y - selfInfo->y, aiming.x - selfInfo->x);  // Direction of Cross-Diagonals
            while (!(aiming.x < 1000 || aiming.x >= 49000 || aiming.y < 1000 || aiming.y >= 49000) &&
                   (oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Grass && oriMap[SquareToCell(GridToSquare(aiming)).x][SquareToCell(GridToSquare(aiming)).y] != THUAI6::PlaceType::Land))
            {
                aiming.x += (int)(round(1000 * cos(asylum)));
                aiming.y += (int)(round(1000 * sin(asylum)));  // On the opposite, try aiming further
            }
            return GridToSquare(CellToGrid(SquareToCell(GridToSquare(aiming))));  // Repeated procedures turn target to center of the cell
        }
        else
        {
            ret = BGM.GradientAim();
            api.Print("SeekInvisibleStudent->if->else\n");
        }
    }
    else
    {
        ret = BGM.GradientAim();
        api.Print("Seekinvisiblestudent->GradientAim");
    }
    return ret;
}

void Idle(ITrickerAPI& api)
{
    printf("IdleStart %d,%d\n", IdleAim.x, IdleAim.y);
    if (oriMap[SquareToCell(IdleAim).x][SquareToCell(IdleAim).y] != THUAI6::PlaceType::Land &&
        oriMap[SquareToCell(IdleAim).x][SquareToCell(IdleAim).y] != THUAI6::PlaceType::Grass)
    {
        IdleAim = XYSquare(-1, -1);
        api.Print("ReInit requested\n");
    }
    else if (dist(XYGrid(SquareToGrid(IdleAim).x - selfInfo->x, SquareToGrid(IdleAim).y - selfInfo->y)) < 1000)
    {
        XYSquare IdleAim1 = SeekInvisibleStudent(api);
        if (IdleAim1.x > 0)
        {
            api.Print("Idle0 called.");
            printf("IdleAim = %d, %d\n", IdleAim1.x, IdleAim1.y);
            Move(api, FindMoveNext(IdleAim1));
            IdleAim = IdleAim1;
        }
        else
        {
            IdleAim = FindClassroom();
            api.Print("FindClassroomCalled.\n");
            Move(api, FindMoveNext(IdleAim));
        }
        TrickerIdlePhase = 1;
    }
    else
        IdleAim = GridToSquare(CellToGrid(SquareToCell(IdleAim)));
    if (selfInfo->playerState != THUAI6::PlayerState::Idle)
    {
        api.Print("IdleQuit\n");
        TrickerIdlePhase = 1;
    }
    else if (IdleAim == XYSquare(-1, -1))  // Initialization
    {
        IdleAim = FindClassroom();
        api.Print("TryToInit\n");
        if (dist(XYGrid(SquareToGrid(IdleAim).x - selfInfo->x, SquareToGrid(IdleAim).y - selfInfo->y)) < 1000)
        {
            XYSquare IdleAim1 = SeekInvisibleStudent(api);
            if (IdleAim1.x > 0)
            {
                api.Print("Idle0 called.");
                printf("IdleAim = %d, %d\n", IdleAim1.x, IdleAim1.y);
                if (IdleAim1.x > 0)
                {
                    Move(api, FindMoveNext(IdleAim1));
                    IdleAim = IdleAim1;
                }
                else
                    Move(api, FindMoveNext(IdleAim));
            }
            else
            {
                IdleAim = FindClassroom();
                api.Print("FindClassroomCalled.\n");
                Move(api, FindMoveNext(IdleAim));
            }
            TrickerIdlePhase = 1;
        }
        api.Print("IdleEndInit\n");
        Move(api, FindMoveNext(IdleAim));
        TrickerIdlePhase = 1;
    }
    else if (api.GetGateProgress(GateCell[0].x, GateCell[0].y) > 0)
    {
        IdleAim = FindGate();
        printf("Gate %d %d\n", IdleAim.x, IdleAim.y);
        if (IdleAim.x > 0)
        {
            Move(api, FindMoveNext(IdleAim));
        }
    }
    else
    {
        if (TrickerIdlePhase == -1)  // This phase confirms the direction if bgm is decreasing
        {
            bool moved = false;
            BGM.UpdateBGM(selfInfoTricker->trickDesire, selfInfoTricker->classVolume);
            api.Print("Idlephase=-1");
            XYSquare IdleAim1 = SeekInvisibleStudent(api);
            api.Print("InvisibleStudentSought\n");
            if (IdleAim1.x > 0)
            {
                api.Print("Idle1 called.");
                printf("IdleAim = %d, %d\n", IdleAim1.x, IdleAim1.y);
                if (IdleAim1.x > 0)
                {
                    Move(api, FindMoveNext(IdleAim1));
                    IdleAim = IdleAim1;
                }
                else
                    Move(api, FindMoveNext(IdleAim));
                moved = true;
            }
            else
            {
                IdleAim = FindClassroom();
                api.Print("FindClassroomCalled.\n");
                if (IdleAim.x > 0)
                {
                    Move(api, FindMoveNext(IdleAim));
                    moved = true;
                }
            }
            if (moved)
                TrickerIdlePhase = 1;
        }
        else if (TrickerIdlePhase == 8)  // Approaching end of a period
        {
            BGM.UpdateBGM(selfInfoTricker->trickDesire, selfInfoTricker->classVolume);  // Update
            printf("%d,%d\n", IdleAim.x, IdleAim.y);
            Move(api, FindMoveNext(IdleAim));
            api.Print("Idlephase=8\n");
            TrickerIdlePhase = 9;
        }
        else if (TrickerIdlePhase == 9)  // At the end of a period
        {
            BGM.UpdateBGM(selfInfoTricker->trickDesire, selfInfoTricker->classVolume);
            api.Print("IdlePhase=9\n");
            if (BGM.Decreasing())
            {
                bool moveable = true;
                if (abs(selfInfo->facingDirection) < 0.02 || abs(selfInfo->facingDirection) > 3.123)
                    TrickerIdlePhase = 0;
                else if (oriMap[SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).x][SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).y + 1] == THUAI6::PlaceType::Land)
                    api.MoveRight(50);
                else if (oriMap[SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).x + 1][SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).y] == THUAI6::PlaceType::Land)
                    api.MoveDown(50);
                else if (oriMap[SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).x][SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).y - 1] == THUAI6::PlaceType::Land)
                    api.MoveLeft(50);
                else if (oriMap[SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).x - 1][SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).y] == THUAI6::PlaceType::Land)
                    api.MoveUp(50);
                else
                    moveable = false;
                api.Print("EndTestRoute\n");
                if (moveable)
                    TrickerIdlePhase = -1;
                else
                    TrickerIdlePhase = 0;
            }
            else
            {
                Move(api, FindMoveNext(IdleAim));
                TrickerIdlePhase = 0;
                api.Print("BGM Increasing\n");
            }
        }
        else  // Within a period
        {
            if (IdleAim.x > 0)
            {
                Move(api, FindMoveNext(IdleAim));
            }
            TrickerIdlePhase = (TrickerIdlePhase + 1) % 10;
            api.Print("Intra-period mod\n");
        }
        ////XYCell a = FindActualClassroom(api); api.Print("1201\n"); printf("Classroom %d %d\n", a.x, a.y);
        ////if (a.x > 0)
        ////{
        ////    api.Print("1203\n");
        ////    if (api.GetClassroomProgress(a.x, a.y) > 0)
        ////    {
        ////        api.Attack(atan2(CellToGrid(a).y - selfInfo->y, CellToGrid(a).x - selfInfo->x)); api.Print("1206\n");
        ////    }//Attack classroom
        // XYSquare b = FindClassroom();
        // if (b.x > 0)
        //{
        //     Move(api, FindMoveNext(b)); api.Print("1204\n");
        // }
        ////}
    }
    return;
}

void AI::play(ITrickerAPI& api)
{
    Update(api);
    Initialize(api);
    api.Print("!\n");
    drawMap(api);
    SPFA();
    int i = 0;
    for (i = 0; i < 4; ++i)
    {
        UpdateF(api, i);
    }
    if (Commandable(api))
    {
        if (api.GetGateProgress(GateCell[0].x, GateCell[0].y) > 0)
        {
            XYSquare toGateSquare = FindGate();
            if (toGateSquare.x > 0)
            {
                Move(api, FindMoveNext(toGateSquare));
            }
        }
        api.Print("1253\n");
        if (TrickerStatus == 8 && fixation >= 0)  // Sitting vigil
        {
            XYCell vec = XYCell(SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).x - SquareToCell(GridToSquare(Trickers_Students[fixation].GetLatestCooridinates())).x, SquareToCell(GridToSquare(XYGrid(selfInfo->x, selfInfo->y))).y - SquareToCell(GridToSquare(Trickers_Students[fixation].GetLatestCooridinates())).y);  // Cell distance from self to addicted
            if (abs(vec.x) > 1 || abs(vec.y) > 1)
                Move(api, FindMoveNext(GridToSquare(CellToGrid(SquareToCell(GridToSquare(Trickers_Students[fixation].GetLatestCooridinates()))))));
            short approachingStudent = FindNextMaxStudent(api, fixation);
            api.Print("1259\n");
            if (approachingStudent >= 0 && Trickers_Students[approachingStudent].addiction > 0)
            {
                if (TryToAttack(api, approachingStudent))  // Try to attack approaching student
                    TrickerStatus = 4;                     // Become dizzy no matter successful or not
                else                                       // Unable to attack
                {
                    XYSquare toGuardSquare = FindGuardSquare(api, fixation, approachingStudent);
                    api.Print("1266\n");
                    printf("%d,%d\n", toGuardSquare.x, toGuardSquare.y);
                    if (toGuardSquare.x > 0 && toGuardSquare.y > 0 && (toGuardSquare.x != selfInfo->x || toGuardSquare.y != selfInfo->y))  // Not already in it
                    {
                        Move(api, FindMoveNext(toGuardSquare));
                        api.Print("Guard pos found\n");
                    }
                    else
                        api.Print("Guard pos not found\n");
                }
            }
            if (Trickers_Students[fixation].addiction > (long long)(0.12 * getMaxAddiction(fixation)))  // End sitting vigil
            {
                short fixation1 = FindMaxStudent(api);
                if (fixation1 < 0 || (fixation1 >= 0 && Trickers_Students[fixation1].addiction > (long long)(0.12 * getMaxAddiction(fixation1))))
                {
                    Trickers_Students[fixation].isFixed = false;
                    TrickerStatus = 0;
                    api.Print("1275\n");
                    fixation = -1;
                    Idle(api);
                }
                else if (fixation1 >= 0)
                {
                    Trickers_Students[fixation].isFixed = false;
                    Trickers_Students[fixation1].isFixed = true;
                    fixation = fixation1;
                    TrickerStatus = (Trickers_Students[fixation].addiction > 0) ? 8 : 1;
                }
            }
            else if (Trickers_Students[fixation].playerstate != THUAI6::PlayerState::Addicted && Trickers_Students[fixation].playerstate != THUAI6::PlayerState::Roused)  // Successfully roused
            {
                TrickerStatus = 1;
                api.Print("1281\n");
            }
        }
        else
        {
            api.Print("1285\n");
            if (TrickerStatus == -1)
                TrickerStatus = 0;   // End true "initialization"
            if (TrickerStatus == 4)  // Was dizzy
            {
                TrickerStatus = 0;
                fixation = -1;
            }
            api.Print("1292\n");
            printf("Stat = %d\n", TrickerStatus);
            short maxstudent = FindMaxStudent(api);
            if (maxstudent >= 0)  // Such a student is found
            {
                api.Print("1295\n");
                if (maxstudent != fixation)  // Target changed
                {
                    if (fixation >= 0)
                        Trickers_Students[fixation].isFixed = false;
                    fixation = maxstudent;
                    api.Print("1300\n");
                    Trickers_Students[maxstudent].isFixed = true;
                }
                api.Print("1302\n");
                if (Trickers_Students[maxstudent].addiction > 0 && Trickers_Students[maxstudent].addiction < (long long)(0.12 * getMaxAddiction(fixation)))
                    TrickerStatus = 8;  // Sit Vigil
                else if (Trickers_Students[maxstudent].addiction == 0)
                {
                    api.Print("1306\n");
                    if (TryToAttack(api, maxstudent))  // Try to attack maxstudent
                        TrickerStatus = 4;             // Become dizzy no matter successful or not
                    // if (Trickers_Students[maxstudent].GetFValue() > MeaningfulValue1)
                    //{
                    //     // Try using prop AddSpeed
                    // }
                    XYSquare toStudentSquare = FindStudentSquare(maxstudent);
                    api.Print("1315\n");
                    printf("%d,%d\n", toStudentSquare.x, toStudentSquare.y);
                    if (toStudentSquare.x > 9 && dist(XYGrid(SquareToGrid(toStudentSquare).x - selfInfo->x, SquareToGrid(toStudentSquare).y - selfInfo->y)) > 1000)
                    {
                        Move(api, FindMoveNext(toStudentSquare));
                        api.Print("1318\n");
                    }
                    TrickerStatus = 1;
                }
            }
            else  // Student not found
            {
                api.Print("1325\n");
                if (fixation >= 0)  // Existent in last run, target lost
                {
                    if (api.GetFrameCount() - Trickers_Students[fixation].lastframe > 600)  // Too long! give up for lost
                    {
                        Trickers_Students[fixation].isFixed = false;
                        fixation = -1;
                        api.Print("1331\n");
                        TrickerStatus = 0;
                    }
                    // if (Trickers_Students[fixation].GetFValue() > MeaningfulValue2)
                    //{
                    //     // Try using prop clairaudience
                    // }
                    // XYSquare toDisappearSquare = SeekDisappearSquare(api, fixation);
                    // if (toDisappearSquare.x > 0)
                    //{
                    //    Move(api, FindMoveNext(toDisappearSquare)); api.Print("1341\n");
                    //}
                    TrickerIdlePhase = -1;
                    TrickerStatus = 0;
                    api.Print("Student lost\n");
                    fixation = -1;
                }
            }
        }
        api.Print("1345\n");
        if (!TrickerStatus)
        {
            api.Print("1345\n");
            fixation = -1;
            Idle(api);
        }
    }
    else
    {
        TrickerStatus = 4;  // Dizzy
        api.EndAllAction();
        api.Print("1352\n");
    }
    return;
}