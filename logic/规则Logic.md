# 规则Logic

## 通过接口了解规则喵

### 地图
- 地图为矩形区域，地图上的游戏对象坐标为（x, y），且x和y均为整数。
- **x坐标轴正方向竖直向下，y坐标轴正方向水平向右**；
- **极坐标以x坐标轴为极轴，角度逆时针为正方向**。
- 地图由50 * 50个格子构成，其中每个格子代表1000 * 1000的正方形。每个格子的编号(CellX,CellY)可以计算得到：
  - 略
- 地图上格子有自己的区域类型：陆地、墙、草地、教室、校门、隐藏校门、门、窗、箱子

### 移动
- `std::future<bool> Move(int64_t timeInMilliseconds, double angleInRadian)`:移动，`timeInMilliseconds` 为移动时间，单位毫秒；`angleInRadian` 表示移动方向，单位弧度，使用极坐标，**竖直向下方向为x轴，水平向右方向为y轴**  

### 使用技能
- `std::future<bool> UseSkill(int32_t skillID)`:使用对应序号的主动技能

### 人物
- 人物半径为800
- 人物共有17种不可叠加的状态：
1. （可）移动状态
- `std::future<bool> EndAllAction()`:可以使处在下者八项状态中的玩家终止交互
2. 学习
3. 被勉励
4. 在勉励
5. 开或锁门 
6. 翻箱
7. 使用技能
8. 开启校门
9. 唤醒他人中
- 之后八项为不可行动状态
10. 被唤醒中
11. 沉迷
12. 退学
13. 毕业
14. 被眩晕
15. 前摇 
16. 后摇
17. 翻窗

### 攻击
- `std::future<bool> Attack(double angleInRadian)`:`angleInRadian`为攻击方向，无论近战远程均产生bullet表示攻击
- 攻击类型CommonAttackOfGhost攻击未写完的作业，会造成对应攻击力的损坏
- 捣蛋鬼攻击交互状态或前后摇的学生，将使学生眩晕4.3s

|   攻击类型       |搞蛋鬼的一般攻击CommonAttackOfGhost|      飞刀FlyingKnife    |      蹦蹦炸弹BombBomb        |       JumpyDumpty        |
| :------------ |  :---------------------   |  :--------------------- | :--------------------- | :--------------------- |
|   子弹爆炸范围   |     0                  |        0                    |   1000                    |    500      | 
|   子弹攻击距离   |     1100               |       39000                 |   1100             |    2200      |
|   攻击力        |     1500000             |        1,200,000                    |   1,800,000               |    900000      |
|   移动速度/s   |     3700             |              7,400                    |   3000                |   4300      | 
|   前摇（ms）      |     297      |      500                    |   366                      |      0      |
|未攻击至目标时的后摇（ms）|     800      |      0                        |     3700            |    0      |
|攻击至目标时的后摇（ms）|    3700         |     0                      |         3700       |       0      |
|   CD(ms)            |      800             |      400               |    3000             |    -      |
|   最大子弹容量 |      1             |     1              |    1                            |   -      |

### 交互
- 除了翻窗，交互目标与交互者在一个九宫格方可交互
- 交互进度每毫秒增加对应交互速度的值
- 作业，门，箱子完成/开启进度达到10000000为完成

#### 破译与逃脱
- 每张地图都有10间教室，学生需要完成其中的**7间**教室的作业，才可以开启任意校门。
- `std::future<bool> StartLearning()`:在教室（旁）做作业
- `std::future<bool> StartOpenGate()`:开启校门，所需时间为18秒，开启的进度不清空
- 当**3间**教室的作业完成时，隐藏校门在3-5个刷新点之一随机显现；当只剩1名学生时，隐藏校门自动打开。
- `std::future<bool> Graduate()`:从开启的校门或隐藏校门毕业。

#### 勉励（Treat）
- `std::future<bool> StartTreatMate(int64_t mateID)`:勉励对应玩家ID的学生，当达到被勉励程度达到1500000或者最大学习毅力与当前学习毅力的差值时，勉励完成，学生毅力增加对应被勉励程度。
- 勉励中断时，被勉励程度保留；遭到攻击时被勉励程度清空

#### 沉迷与唤醒
- 当学生学习毅力归零时，学生原地进入沉迷状态，每毫秒增加1沉迷度
- `std::future<bool> StartRescueMate(int64_t mateID)`：唤醒对应玩家ID的沉迷的学生，需要时间1秒，之后学习毅力恢复至1/2。沉迷程度不清空。
- 进入沉迷状态时，如果学生原沉迷程度在（0，该玩家最大沉迷度/3）中，沉迷程度直接变为其最大沉迷度/3；原沉迷程度在[其最大沉迷度/3，其最大沉迷度x2/3）中，沉迷程度直接变为其最大沉迷度x2/3；原沉迷程度大于其最大沉迷度x2/3，从游戏中出局；
  - 当学生沉迷程度达到其最大沉迷程度时，从游戏中出局

#### 门
- 门分别属于三个教学区：三教，五教，六教
- 拥有对应教学区的钥匙才能开锁对应的门
  - 锁门过程中，门所在格子内有人会使锁门过程中断
- `std::future<bool> OpenDoor()`:开门
- `std::future<bool> CloseDoor()`:关门

#### 窗
- `std::future<bool> SkipWindow()`:翻窗时玩家应当在窗前后左右一个格子内

#### 箱子
- `std::future<bool> StartOpenChest()`:开箱后将有2个随机道具掉落在玩家位置。

### Bgm
1. 不详的感觉：捣蛋鬼进入（学生的警戒半径/捣蛋鬼的隐蔽度）时，学生收到；捣蛋鬼距离学生越近，Bgm音量越大。bgmVolume=（警戒半径/二者距离）
2. 期待搞事的感觉：学生进入（捣蛋鬼的警戒半径/学生的隐蔽度）时，捣蛋鬼收到；捣蛋鬼距离学生越近，Bgm音量越大。bgmVolume=（警戒半径/可被发觉的最近的学生距离）
3. 学习的声音: 捣蛋鬼警戒半径内有人学习时收到；bgmVolume=（警戒半径x学习进度百分比）/二者距离

### 队内信息
- `std::future<bool> SendMessage(int64_t, std::string)`：给同队的队友发送消息。第一个参数指定发送的对象，第二个参数指定发送的内容。
- `bool HaveMessage()`:是否有队友发来的尚未接收的信息。
- `std::pair<int64_t, std::string> GetMessage()`:从玩家ID为第一个参数的队友获取信息。

### 线程控制
- `bool Wait()`:阻塞当前线程，直到下一次消息更新时继续运行。

### 信息获取
`std::vector<std::shared_ptr<const THUAI6::Student>> GetStudents() const` ：返回所有可视学生的信息。
`std::vector<std::shared_ptr<const THUAI6::Tricker>> GetTrickers() const` ：返回所有可视捣蛋鬼的信息。
`std::vector<std::shared_ptr<const THUAI6::Prop>> GetProps() const` ：返回所有可视道具的信息。
`std::vector<std::vector<THUAI6::PlaceType>> GetFullMap() const`：返回整张地图的地形信息。

- 下面的 CellX 和 CellY 指的是地图格数，而非绝对坐标。

`THUAI6::PlaceType GetPlaceType(int32_t cellX, int32_t cellY)` ：返回某一位置场地种类信息。场地种类详见 structure.h 。
`bool IsDoorOpen(int32_t cellX, int32_t cellY) const`:查询特定位置门是否开启
`int32_t GetChestProgress(int32_t cellX, int32_t cellY) const`:查询特定位置箱子开启进度
`int32_t GetGateProgress(int32_t cellX, int32_t cellY) const`:查询特定位置校门开启进度
`int32_t GetClassroomProgress(int32_t cellX, int32_t cellY) const`:查询特定位置教室作业完成进度
`THUAI6::HiddenGateState GetHiddenGateState(int32_t cellX, int32_t cellY) const`：:查询特定位置隐藏校门状态
`int32_t GetDoorProgress(int32_t cellX, int32_t cellY) const`:查询特定位置门开启状态

`std::shared_ptr<const THUAI6::GameInfo> GetGameInfo() const`:查询当前游戏状态
`std::vector<int64_t> GetPlayerGUIDs() const`:获取所有玩家的GUID
`int GetFrameCount() const`:获取目前所进行的帧数
`std::shared_ptr<const THUAI6::Tricker> GetSelfInfo() const`或`std::shared_ptr<const THUAI6::Student> GetSelfInfo() const`：获取自己的信息

### 道具
- 玩家最多同时拥有三个道具
- `bool PickProp(THUAI6::PropType prop)`捡起与自己处于同一个格子（cell）的道具。
- `bool UseProp(THUAI6::PropType prop)`使用对应类型的道具
- `bool ThrowProp(THUAI6::PropType prop)`将对应类型的道具扔在原地

|   道具       |           对学生增益            |       [学生得分条件]               |                对搞蛋鬼增益        |       [搞蛋鬼得分条件]               |
| :-------- | :-------------------------------------- | :-----------------| :-------------------------------------- |:-----------------|
|   Key3   |                            能开启3教的门                            |不得分|      能开启3教的门                            |不得分| 
|   Key5   |                             能开启5教的门                           |不得分|      能开启5教的门                            |不得分| 
|   Key6   |                             能开启6教的门                            |不得分|      能开启6教的门                            |不得分| 
|   AddSpeed   |  提高移动速度，持续10s                            |得分？|  提高移动速度，持续10s                            |得分？|
|   AddLifeOrClairaudience   |若在10s内Hp归零，该增益消失以使Hp保留100|在10s内Hp归零，得分？      |10秒内下一次攻击增伤1800000|10秒内有一次攻击，得分？ |     
|   AddHpOrAp   |回血1500000   | 回血成功 |   10秒内下一次攻击增伤1800000|10秒内有一次攻击，得分？ |
|   ShieldOrSpear   | 10秒内能抵挡一次伤害  |   10秒内成功抵挡一次伤害       |10秒内下一次攻击能破盾，如果对方无盾，则增伤900000|   10秒内攻击中学生| 
|   RecoveryFromDizziness   | 使用瞬间从眩晕状态中恢复  |    成功从眩晕状态中恢复，得分？|使用瞬间从眩晕状态中恢复  |    成功从眩晕状态中恢复，得分？| 

## 得分

### 屠夫

- [Tricker对Student造成伤害时，得伤害*100/基本伤害（1500000）分。]
- *[使用道具/技能得分]*
  - 不同道具/技能有不同得分
- 使Student进入沉迷状态时，得50分。
- 使人类进入眩晕状态时，得25分。
- 每淘汰一个Student，得1000分
- 主动解除眩晕，得15分
- 开锁门

### 人类
- 修机得分
  - 人类每修n%的电机，得n分
  - 修完一台电机，额外得？分
- [牵制得分]
- 使用道具/技能得分
  - 不同道具/技能有不同得分
- 使屠夫进入特殊状态得分（如使之眩晕）
- 救人
- 勉励
- 逃脱
- 解除眩晕
- 开锁门

## 职业与技能

### 捣蛋鬼

|   捣蛋鬼职业       |   基本量                 |        Assassin            |        Klee        |            喧哗者ANoisyPerson        |
| :------------ | :--------------------- |  :--------------------- |  :--------------------- | :--------------------- |
|   移动速度/s   |  1,503                   |     1.1             |      1                    |   1.07      | 
|   隐蔽度   |      1.0                 |      1.5              |      1                            |     0.8     | 
|   警戒范围   |    17000                 |      1.3             |      1                            |   0.9      | 
|   视野范围   |    15000                 |      1.2             |      1                            |    1   | 
|   开锁门速度 |  4000               |     1             |    1                            |      1     |
|   翻窗速度   |   1270                 |   1                   |       1       |     1.1       |     
|   翻箱速度   |  1000           |           1      |   1.1         |     1      |

#### 刺客
- 普通攻击为 搞蛋鬼的一般攻击
- 主动技能
  - 隐身
    - CD：60s 持续时间：6s
    - 在持续时间内玩家隐身
    - 使用瞬间得分
  - 使用飞刀
    - CD：30s 持续时间：1s
    - 在持续时间内，攻击类型变为飞刀
    - 不直接得分

#### Klee
- 普通攻击为 搞蛋鬼的一般攻击
- 主动技能
  - 蹦蹦炸弹
    - CD：15s 持续时间：3s
    - 在持续时间内，攻击类型变为蹦蹦炸弹
    - 当蹦蹦炸弹因为碰撞而爆炸，向子弹方向上加上90°，270° 发出2个小炸弹
    - 2个小炸弹运动停止前会因为碰撞爆炸，停止运动后学生碰撞会造成眩晕3.07s
    - 不直接得分，通过眩晕等获得对应得分

#### 喧哗者
- 普通攻击为 搞蛋鬼的一般攻击
- 主动技能
  - 嚎叫
    - CD：25s 
    - 使用瞬间，在视野半径范围内（不是可视区域）的学生被眩晕6110ms，自己进入3070ms的后摇
    - 通过眩晕获得对应得分
- 特性
  - 在场所有学生Bgm系统被设为无用的值


### 学生（&老师）

|   学生职业       |   基本量             |        教师Teacher       |        健身狂Athlete    |      学霸StraightAStudent        |      开心果Sunshine        |
| :------------ | :--------------------- |  :--------------------- |  :--------------------- | :--------------------- | :--------------------- |
|   移动速度   |  1,270                   |     3 / 4             |      1.1                    |   0.8      |    1      | 
|   最大毅力值   |  3000000                   |     10             |      1                    |   1.1      |    32/30      |
|   最大沉迷度   |  60000                   |     10             |        0.9                    |   1.3      |    1.1      |
|   学习一科速度/s   |  1.23%             |       0             |      0.6                    |   1.1      |   1      | 
|   勉励速度   |      100                   |     0.7             |      0.8                    |   0.8      |      2      |
|   隐蔽度   |      1.0                 |      0.5              |      0.9                            |     0.9     |    1      |
|   警戒范围   |    15000                 |      0.5             |      1                            |   0.9      |    1      |
|   视野范围   |    10000                 |      0.9             |      1.1                            |    0.9   |    1      |
|   开锁门速度 |  4000               |     1              |    1                            |      1     |   1      |
|   翻窗速度   |   1270                 |       0.5             |       1.2       |     10/12       |           1     |
|   翻箱速度   |  1000           |            1          |   1         |     1      |      1     |

#### 运动员
- 主动技能 
  - 冲撞
    - CD：24s 持续时间：5s
    - 在持续时间内，速度变为三倍，期间撞到捣蛋鬼，会导致捣蛋鬼眩晕7.22s,学生眩晕2.09s
    - 通过眩晕获得对应得分

#### 教师
- 主动技能
  - 惩罚
    - CD：30s 
    - 使用瞬间，在可视范围内的使用技能状态中、攻击前后摇的捣蛋鬼会被眩晕（3070）ms，
    - 通过眩晕获得对应得分
- 特性
  - 教师无法获得牵制得分

#### 学霸
- 特性
  - 冥想
    - 当玩家处于可接受指令状态且不在修机时，会积累学习进度，速度为0.3%/ms
    - 受到攻击（并非伤害）或眩晕或翻窗（或攻击他人）学习进度清零
- 主动技能5 
  - 写答案
    - CD：30s
    - 使用瞬间，对于可互动范围内的一台电机增加这个学习进度
    - 通过修机获得对应得分

#### 开心果
- 主动技能
  - 唤醒
    - CD：60s 
    - 使用瞬间，唤醒可视范围内一个沉迷中的人
    - 通过唤醒获得对应得分
  - 勉励
    - CD：60s
    - 使用瞬间，勉励完成可视范围内一个毅力不足的人
    - 通过勉励获得对应得分
  - 鼓舞
    - CD：60s  
    - 使用瞬间，可视范围内学生（包括自己）获得持续6秒的1.6倍速Buff
    - 每鼓舞一个学生得分10

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
    virtual std::future<bool> StartTreatMate(int64_t mateID) = 0;
    virtual std::future<bool> StartRescueMate(int64_t mateID) = 0;
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

## 细则

### 特殊说明
- 不加说明，这里“学生”往往包括职业“教师”

### 移动
- 不鼓励选手面向地图编程，因为移动过程中你可以受到多种干扰使得移动结果不符合你的预期；因此建议小步移动，边移动边考虑之后的行为。
- `bool MoveRight(uint32_t timeInMilliseconds)`即向右移动,`MoveLeft`、`MoveDown`、`MoveUp`同理  

### 人物
- 被唤醒或被勉励不属于交互状态，翻窗属于交互状态

### 初始状态
- 玩家出生点固定且一定为空地

### 道具
- 使用钥匙相当于销毁

### 交互
- 在指令仍在进行时，重复发出同一类型的交互指令是无效的，你需要先发出Stop指令终止进行的指令
  - 实际上唤醒或勉励不同的人是有效的

### 破译与逃脱
- 隐藏校门与校门对于人有碰撞体积
- 一个校门同时最多可以由一人开启

### 攻击
- 每次学生受到攻击后会损失对应子弹的攻击力的学习毅力
- 前摇期间攻击被打断时，子弹消失。
- 此处，前摇指 从播放攻击动作开始 攻击者不能交互 的时间

### 沉迷与唤醒
- 在被救时沉迷度不增加
- 不能两人同时唤醒一个人

### 门
- 每个教学区都有2把钥匙
- 一扇门只允许同时一个人开锁门
- 开锁门未完成前，门状态表现为原来的状态
- 开锁门进度中断后清空

### 窗
- 攻击可以穿过窗，道具可以在窗上
- 翻越窗户是一种交互行为,翻窗一共有两个过程
  - 跳上窗：从当前位置到窗边缘中点，位置瞬移，时间=距离/爬窗速度。中断时,停留在原位置
  - 爬窗：从窗一侧边缘中点到另一侧格子中心，位置渐移，时间=距离/爬窗速度。中断时,停留在另一侧格子中心
- 通常情况下捣蛋鬼翻越窗户的速度高于学生。
- 有人正在翻越窗户时，其他玩家均不可以翻越该窗户。

### 箱子
- 地图上有8个箱子
- 同一时刻只允许一人进行开启
- 未开启完成的箱子在下一次需要重新开始开启。
- 箱子开启后其中道具才可以被观测和拿取
- 箱子道具不刷新

### 信息获取
- 查询对应物体状态时物体不存在则返回false

## 键鼠控制

| 键位         | 效果                                           |
| ------------ | ---------------------------------------------- |
| W/NumPad8    | （Both）向上移动                               |
| S/NumPad2    | （Both）向下移动                               |
| D/NumPad6    | （Both）向右移动                               |
| A/NumPad4    | （Both）向左移动                               |
| J            | （Tri）攻击，方向向上                          |
| 鼠标双击某点 | （Tri）攻击，方向与从Tricker指向该点的向量相同 |
| K            | （Stu）开始学习                                |
| R            | （Stu）开始营救（陷入沉迷状态的同伴）          |
| T            | （Stu）开始勉励（学习毅力下降的同伴）          |
| G            | （Stu）发出毕业请求                            |
| H            | （Stu）申请毕业（或称为开校门）                |
| O            | （Both）开（教学楼）门                         |
| P            | （Both）关（教学楼）门                         |
| U            | （Both）翻窗                                   |
| I            | （Both）翻箱子                                 |
| E            | （Both）结束当前行动，回到Idle状态             |
| F            | （Both）随机捡起一个在周围的道具               |
| C            | （Both）随机扔下一个已经持有的道具             |
| V            | （Both）随机使用一个已经持有的道具             |
| B            | （Both）使用0号技能                            |
| N            | （Both）使用1号技能                            |
| M            | （Both）使用2号技能                            |
