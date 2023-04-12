# CAPI接口
- [CAPI接口](#capi接口)
  - [接口解释](#接口解释)
    - [移动](#移动)
    - [使用技能](#使用技能)
    - [人物](#人物)
    - [攻击](#攻击)
      - [学习与毕业](#学习与毕业)
      - [勉励](#勉励)
      - [沉迷与唤醒](#沉迷与唤醒)
      - [门](#门)
      - [窗](#窗)
      - [箱子](#箱子)
    - [队内信息](#队内信息)
    - [信息获取](#信息获取)
  - [辅助函数](#辅助函数)
    - [道具](#道具)
  - [接口一览](#接口一览)

## 接口解释

### 移动
- `std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian)`:移动，`timeInMilliseconds` 为移动时间，单位毫秒；`angleInRadian` 表示移动方向，单位弧度，使用极坐标，**竖直向下方向为x轴，水平向右方向为y轴**  
- `bool MoveRight(uint32_t timeInMilliseconds)`即向右移动,`MoveLeft`、`MoveDown`、`MoveUp`同理  

### 使用技能
- `std::future<bool> UseSkill(int32_t skillID)`:使用对应序号的主动技能

### 人物
- `std::future<bool> EndAllAction()`:可以使不处在不可行动状态中的玩家终止当前行动

### 攻击
- `std::future<bool> Attack(double angleInRadian)`:`angleInRadian`为攻击方向

#### 学习与毕业
- `std::future<bool> StartLearning()`:在教室里开始做作业
- `std::future<bool> StartOpenGate()`:开始开启校门
- `std::future<bool> Graduate()`:从开启的校门或隐藏校门毕业。

#### 勉励
- `std::future<bool> StartEncourageMate(int64_t mateID)`:勉励对应玩家ID的学生，

#### 沉迷与唤醒
- `std::future<bool> StartRouseMate(int64_t mateID)`：唤醒对应玩家ID的沉迷的学生。

#### 门
- `std::future<bool> OpenDoor()`:开门
- `std::future<bool> CloseDoor()`:关门

#### 窗
- `std::future<bool> SkipWindow()`:翻窗

#### 箱子
- `std::future<bool> StartOpenChest()`:开箱

### 队内信息
- `std::future<bool> SendMessage(int64_t, std::string)`：给同队的队友发送消息。第一个参数指定发送的对象，第二个参数指定发送的内容，不得超过256字节。
- `bool HaveMessage()`:是否有队友发来的尚未接收的信息。
- `std::pair<int64_t, std::string> GetMessage()`:从玩家ID为第一个参数的队友获取信息。

### 信息获取
- 查询可视范围内的信息
  - `std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const` ：返回所有可视学生的信息。
  - `std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const` ：返回所有可视捣蛋鬼的信息。
  - `std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const` ：返回所有可视道具的信息。
  - `std::vector<std::shared_ptr<const THUAI6::Bullet>> GetBullets() const` ：返回所有可视子弹（攻击）的信息。

- 查询特定位置物体的信息，下面的 CellX 和 CellY 指的是地图格数，而非绝对坐标。
  - `THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY)` ：返回某一位置场地种类信息。场地种类详见 structure.h 。
  - `bool IsDoorOpen(int32_t cellX, int32_t cellY) const`:查询特定位置门是否开启
  - `int32_t GetChestProgress(int32_t cellX, int32_t cellY) const`:查询特定位置箱子开启进度
  - `int32_t GetGateProgress(int32_t cellX, int32_t cellY) const`:查询特定位置校门开启进度
  - `int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const`:查询特定位置教室作业完成进度
  - `THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const`：:查询特定位置隐藏校门状态
  - `int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const`:查询特定位置门开启状态
- 其他
  - `std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const`:查询当前游戏状态\
  - `std::vector<int64_t> GetPlayerGUIDs() const`:获取所有玩家的GUID\
  - `int GetFrameCount() const`:获取目前所进行的帧数\
  - `std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const`或`std::shared_ptr<const THUAI6::Student> GetSelfInfo() const`：获取自己的信息
  - `std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const`：返回整张地图的地形信息。

## 辅助函数
`static inline int CellToGrid(int cell) noexcept`:将地图格数 cell 转换为绝对坐标grid。

`static inline int GridToCell(int grid) noexcept`:将绝对坐标 grid 转换为地图格数cell。

下面为用于DEBUG的输出函数，选手仅在开启Debug模式的情况下可以使用
~~~c
    void Print(std::string str) const；
    void PrintStudent() const；
    void PrintTricker() const；
    void PrintProp() const；
    void PrintSelfInfo() const；
~~~

### 道具
- `bool PickProp(THUAI6::PropType prop)`捡起与自己处于同一个格子（cell）的道具。
- `bool UseProp(THUAI6::PropType prop)`使用对应类型的道具
- `bool ThrowProp(THUAI6::PropType prop)`将对应类型的道具扔在原地

## 接口一览
~~~csharp
    // 指挥本角色进行移动，`timeInMilliseconds` 为移动时间，单位为毫秒；`angleInRadian` 表示移动的方向，单位是弧度，使用极坐标——竖直向下方向为 x 轴，水平向右方向为 y 轴
    virtual std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian) = 0;

    // 向特定方向移动
    virtual std::future<bool> MoveRight(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveUp(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveLeft(int64_t timeInMilliseconds) = 0;
    virtual std::future<bool> MoveDown(int64_t timeInMilliseconds) = 0;

    // 捡道具、使用技能
    virtual std::future<bool> PickProp(THUAI6::PropType prop) = 0;
    virtual std::future<bool> UseProp(THUAI6::PropType prop) = 0;
    virtual std::future<bool> UseSkill(int32_t skillID) = 0;
    virtual std::future<bool> Attack(double angleInRadian) = 0;

    virtual std::future<bool> OpenDoor() = 0;
    virtual std::future<bool> CloseDoor() = 0;
    virtual std::future<bool> SkipWindow() = 0;
    virtual std::future<bool> StartOpenGate() = 0;
    virtual std::future<bool> StartOpenChest() = 0;
    virtual std::future<bool> EndAllAction() = 0;

    // 发送信息、接受信息，注意收消息时无消息则返回nullopt
    virtual std::future<bool> SendMessage(int64_t, std::string) = 0;
    [[nodiscard]] virtual bool HaveMessage() = 0;
    [[nodiscard]] virtual std::pair<int64_t, std::string> GetMessage() = 0;

    // 等待下一帧
    virtual std::future<bool> Wait() = 0;

    // 获取视野内可见的学生/捣蛋鬼的信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const = 0;

    // 获取视野内可见的道具信息
    [[nodiscard]] virtual std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const = 0;

    // 获取地图信息，视野外的地图统一为Land
    [[nodiscard]] virtual std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const = 0;
    [[nodiscard]] virtual THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual bool IsDoorOpen(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetChestProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetGateProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const = 0;
    [[nodiscard]] virtual int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const = 0;

    [[nodiscard]] virtual std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const = 0;

    // 获取所有玩家的GUID
    [[nodiscard]] virtual std::vector<int64_t> GetPlayerGUIDs() const = 0;

    // 获取游戏目前所进行的帧数
    [[nodiscard]] virtual int GetFrameCount() const = 0;

    /*****选手可能用的辅助函数*****/

    // 获取指定格子中心的坐标
    [[nodiscard]] static inline int CellToGrid(int cell) noexcept
    {
        return cell * numOfGridPerCell + numOfGridPerCell / 2;
    }

    // 获取指定坐标点所位于的格子的 X 序号
    [[nodiscard]] static inline int GridToCell(int grid) noexcept
    {
        return grid / numOfGridPerCell;
    }

    // 用于DEBUG的输出函数，选手仅在开启Debug模式的情况下可以使用

    virtual void Print(std::string str) const = 0;
    virtual void PrintStudent() const = 0;
    virtual void PrintTricker() const = 0;
    virtual void PrintProp() const = 0;
    virtual void PrintSelfInfo() const = 0;
};

class IStudentAPI : public IAPI
{
public:
    /*****学生阵营的特定函数*****/

    virtual std::future<bool> StartLearning() = 0;
    virtual std::future<bool> StartEncourageMate(int64_t mateID) = 0;
    virtual std::future<bool> StartRouseMate(int64_t mateID) = 0;
    virtual std::future<bool> Graduate() = 0;
    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Student> GetSelfInfo() const = 0;
};

class ITrickerAPI : public IAPI
{
public:
    /*****捣蛋鬼阵营的特定函数*****/

    [[nodiscard]] virtual std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const = 0;
};
~~~