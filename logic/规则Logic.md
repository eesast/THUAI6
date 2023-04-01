# 规则Logic

## 说明
- 版本V2.0
- 该规则直接服务于Sever,并非选手版本
- *斜体表示Logic底层尚未（完全）实现*
- []表示待决定
- ~~表示暂不实现~~

## 游戏简介
- 1位监管者对抗4位求生者的非对称竞技模式
- [本届THUAI电子系赛道为以4名同学和1名捣蛋鬼的求学与阻挠展开的非对称竞技模式，同学需要完成足够的家庭作业和考试，相互督促以避免沉迷娱乐生活，利用道具地形躲避捣蛋鬼的各种干扰诱惑，完成学业；捣蛋鬼则要极力阻止。]
- [我们的设计是一个非对称游戏，类似第五人格，分为学生、捣蛋鬼两个阵营。在游戏中，学生修完若干课程之后通过考试即可顺利毕业，捣蛋鬼试图干扰学生使其沉迷游戏，以致于无法修完规定课程，直至挂科、退学。]
[对于选手来说，需要提前制定好学生的学习方案以抵御对方捣蛋鬼的干扰，类似地，也需要制定好捣蛋鬼的行动策略以影响对方学生的学习，也即每队至少要写好两份代码以执行不同阵营的不同策略。]
[当一局比赛结束（场上的学生有且仅有两种状态：退学或毕业）时，分别记录双方总得分；之后双方换边进行下半场比赛。最终将每队的学生方、捣蛋鬼方的得分相加，比较总得分判断胜负。]

## 简要规则

### 地图
- 地图为矩形区域，地图上的游戏对象坐标为（x, y），且x和y均为整数。
- **x坐标轴正方向竖直向下，y坐标轴正方向水平向右**；
- **极坐标以x坐标轴为极轴，角度逆时针为正方向**。
- 地图由50 * 50个格子构成，其中每个格子代表1000 * 1000的正方形。每个格子的编号(CellX,CellY)可以计算得到：
  - 略
- 地图上的每个格子有自己的区域类型：墙、草地、电机、出口、紧急出口、门、窗、箱子

### 交互
- 除了逃离和翻窗，交互目标与交互者在一个九宫格内则为可交互
  - 逃离或翻窗时玩家应当在目标前后左右一个格子内

### 破译与逃脱
- 每张地图都有10台电机，求生者需要破译其中的7台，并开启任意一个大门（总所需时间为18秒）后从任意一个开启的大门逃脱，亦或者在只剩1名求生者的情况下从紧急出口逃脱；
- 紧急出口会在电机破译完成3台的情况下在地图的3-5个固定紧急出口刷新点之一随机刷新。
- 当求生者只剩1名时，紧急出口将会自动打开，该求生者可从紧急出口逃脱。
- 大门开启的进度不清空

### 攻击
- 无论近战远程均产生子弹以表示攻击所至距离
- 攻击时，从播放攻击动作到可以开始产生伤害的期间，称为前摇。
（前摇阶段，搞蛋鬼产生通常为不可爆炸（部分搞蛋鬼能可以产生可爆炸）子弹（爆炸范围=0），[子弹大小待商榷]，期间监管者攻击被打断时，子弹消失）（）
- 无论搞蛋鬼或学生，攻击后，通常会出现一段无伤害判定的攻击动作后置时间，称为后摇。击中物体时后摇更长
- 假如监管者攻击或一些监管者的特定技能击中正在交互或处于攻击前后摇或使用部分技能（指PlayerState==UsingSkill）的求生者，将使求生者眩晕
1. 处于前摇或后摇
2. 治疗或解救他人
3. 修理电机
4. 开锁门
5. 翻窗
6. 开启箱子

## 细则

### 初始状态
- 所有玩家可以立刻使用主动技能
  - 虽然一开始会接到仍需的冷却时间，但实际上是CD
- 玩家出生点固定且一定为空地

### 交互
- 在指令仍在进行时，重复发出同一类型的交互指令是无效的，你需要先发出Stop指令终止进行的指令
  - 实际上救援或治疗不同的人是有效的

### 破译与逃脱
- 紧急出口与大门对于人有碰撞体积
- 一个大门同时最多可以由一人开启

### 攻击
- 每次求生者受到攻击后会损失对应子弹的攻击力的血量
- 小部分求生者通过手持物可以获得一定程度反击监管者的手段，可使其丧失行动能力（称为击晕），但监管者不会永久丧失行动能力。

### 门
- 门分别属于三个教学区：三教，五教，六教
- 监管者或求生者都需要拿到对应教学区的钥匙才能打开或锁住对应的门
- 锁门和开门都需要一定时间，进出门为正常移动过程
- 门只有开、锁两种状态，锁住时门有碰撞体积
- 当门所在格子内有人时，无法锁门（必须从门外锁门）
- 锁门时其他人可以进入门所在格子，锁门过程中断
- 钥匙只会出现在箱子中，每个教学区都有2把钥匙
- 一扇门只允许同时一个人开锁门
- 开锁门未完成前，门状态表现为原来的状态
- 开锁门进度中断后清空

### 窗
- 求生者和监管者都可以翻越窗户，但通常情况下监管者翻越窗户的速度**高于**求生者。
- 在求生者或监管者正在翻越窗户时，其他玩家均不可以翻越该窗户。
- 攻击可以穿过窗，道具可以在窗上
- 翻窗时玩家应当在窗前后一个格子内
- 翻越窗户是一种交互行为,翻窗一共有两个过程
  - 跳上窗：从当前位置到窗边缘中点，位置瞬移，时间=距离/爬窗速度，中断时,停留在原位置
  - 爬窗：从窗一侧边缘中点到另一侧格子中心，位置渐移，时间=距离/爬窗速度，中断时,停留在另一侧格子中心
- 窗必须在两个墙之间，另外两侧不能为墙

### 箱子
- 监管者和求生者都能与箱子交互,同一时刻只允许一人进行开启
- 开启箱子有不同概率获得不同道具。
- 开启箱子的基础持续时间为10秒。
- 未开启完成的箱子在下一次需要重新开始开启。
- 箱子开启后其中道具才可以被观测和拿取
- [箱子道具不刷新]
- [箱子不可被关闭]
- [箱子内道具最多两个]

### 治疗
- 可行动的求生者可以对受伤的其他求生者进行治疗，治疗完成后会回复被治疗程度的血量。
- 治疗时每毫秒增加相当于治疗者治疗速度的被治疗程度
- 当达到被治疗程度达到1500000或者最大血量与当前血量的差值时，治疗结束。
- 治疗他人未完成时重新发出治疗指令是无效的（无论是否要求治疗同一人）
- 治疗中断时，被治疗程度保留；被治疗者遭到攻击时被治疗程度清空

### 沉迷
- 当求生者血量归零时，求生者自动原地进入沉迷状态，每毫秒增加1沉迷度
- 在被救时沉迷度不增加
- 该求生者可由其他的求生者救下，救下后，*血量恢复至1/2并可以重新行动*。沉迷程度不清空。
- 进入沉迷状态时，如果求生者原本沉迷程度在【10003，30000】中，求生者沉迷程度直接变为30000
- 当求生者沉迷程度达到该玩家最大沉迷程度（学生有的属性）时，从游戏中出局

### 救人
- 一般情况下，救人时间为1秒。
- 不能两人同时救一个人

## 得分

### 屠夫

#### 伤害得分 
[Tricker对Student造成伤害时，得伤害*100/基本伤害（1500000）分。]

#### *[使用道具/技能得分]*

——需要造成一定效果才能获取分数，仅使用不得分

- 不同道具/技能有不同得分

#### 沉迷
使Student进入沉迷状态时，得50分。

#### 眩晕
使人类进入眩晕状态时，得25分。

#### 淘汰
每淘汰一个Student，得1000分

#### ~~主动/被动解除特殊状态得分（如解除眩晕）~~
~~解除眩晕，得15分~~

### 人类

#### 修机得分
- 人类每修n%的电机，得n分
- 修完一台电机，额外得？分

#### [牵制得分]

#### 使用道具/技能得分
- 不同道具/技能有不同得分

#### 使屠夫进入特殊状态得分（如使之眩晕）

#### 救人

#### 治疗

#### 逃脱

#### ~~解除眩晕~~

#### ~~自愈~~

## 道具
- 每次玩家试图捡起道具时，需要确保道具栏有空位
- indexing指道具栏数组下标从0开始
- 扔道具
  - Logic内实现
  ~~~csharp
    public void ThrowProp(long playerID, int indexing)
  ~~~
  - 对应下标出现空位，不会对数组进行重新排序
- 使用道具
  - Logic内实现
  ~~~csharp
    public void UseProp(long playerID,int indexing)
  ~~~
  - 对应下标出现空位，不会对数组进行重新排序

|   道具       |           对学生增益            |       [学生得分条件]               |                对搞蛋鬼增益        |       [搞蛋鬼得分条件]               |
| :-------- | :-------------------------------------- | :-----------------| :-------------------------------------- |:-----------------|
|   Key3   |                            能开启3教的门                            |不得分|      能开启3教的门                            |不得分| 
|   Key5   |                             能开启5教的门                           |不得分|      能开启3教的门                            |不得分| 
|   Key6   |                             能开启6教的门                            |不得分|      能开启3教的门                            |不得分| 
|   AddSpeed   |  提高移动速度，持续10s                            |得分？|  提高移动速度，持续10s                            |得分？|
|   AddLifeOrClairaudience   |若在10s内Hp归零，该增益消失以使Hp保留100|在10s内Hp归零，得分？      |10秒内下一次攻击增伤1800000|10秒内有一次攻击，得分？ |     
|   AddHpOrAp   |回血1500000   | 回血成功 |   10秒内下一次攻击增伤1800000|10秒内有一次攻击，得分？ |
|   ShieldOrSpear   | 10秒内能抵挡一次伤害  |   10秒内成功抵挡一次伤害       |10秒内下一次攻击能破盾，如果对方无盾，则增伤900000|   10秒内攻击中学生| 
|   RecoveryFromDizziness   | 使用瞬间从眩晕状态中恢复  |    成功从眩晕状态中恢复，得分？|使用瞬间从眩晕状态中恢复  |    成功从眩晕状态中恢复，得分？| 

## 职业与技能

### 捣蛋鬼
#### 刺客
  ~~~csharp
    public class Assassin : IGhost
    {
        private const int moveSpeed = GameData.basicMoveSpeed * 473 / 380;
        private const int maxHp = GameData.basicHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.BecomeInvisible, ActiveSkillType.UseKnife });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 1.5;
        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.3);
        public int viewRange = (int)(GameData.basicViewRange * 1.3);
        public int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
    }
  ~~~
- 普通攻击为 CommonAttackOfGhost
  ~~~csharp
        public CommonAttackOfGhost...
      {
        public override double BulletBombRange => 0;
        public override double BulletAttackRange => GameData.basicAttackShortRange;
        public int ap = GameData.basicApOfGhost;
        public override int Speed => GameData.basicBulletMoveSpeed;
        public override bool IsToBomb => false;
  
        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicBackswing;
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType) 
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                    return true;
                default: 
                    return false;
            }
        }
      }
  ~~~
- 主动技能
  - 隐身
    ~~~csharp
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;
    ~~~
    在DurationTime时间内玩家隐身
  - 使用飞刀
    ~~~csharp
        public int SkillCD => GameData.commonSkillCD* 2 / 3 ;
        public int DurationTime => GameData.commonSkillTime / 10;
    ~~~
    在DurationTime时间内，攻击类型变为飞刀
    ~~~csharp
    internal sealed class FlyingKnife : Bullet
    {
        public override double BulletBombRange => 0;
        public override double BulletAttackRange => GameData.basicRemoteAttackRange * 13;
        public int ap = GameData.basicApOfGhost * 4 / 5;
        public override int Speed => GameData.basicBulletMoveSpeed * 2;
        public override bool IsToBomb => false;
    
        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing * 2 / 5;
        public override int RecoveryFromHit => GameData.basicBackswing * 3 / 4;
        public const int cd = GameData.basicBackswing * 2 / 5 + 100;
    
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }
    }
    ~~~

#### Klee
  - 普通攻击为 CommonAttackOfGhost
  ~~~csharp
      int moveSpeed = (int)(GameData.basicMoveSpeed * 155 / 127);
      int maxHp = GameData.basicHp;
      int maxBulletNum = 1;
      BulletType InitBullet => BulletType.CommonAttackOfGhost;
      List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.JumpyBomb });
      List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

      double concealment = GameData.basicConcealment;
      int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.069);
      int viewRange = (int)(GameData.basicViewRange * 1.1);
      int timeOfOpeningOrLocking = (int)(GameData.basicSpeedOfOpeningOrLocking / 1.1);
      int speedOfClimbingThroughWindows = (int)(GameData.basicGhostSpeedOfClimbingThroughWindows / 1.1);
      int speedOfOpenChest = (int)(GameData.basicSpeedOfOpenChest * 1.1);
  ~~~
  - 主动技能
    - 蹦蹦炸弹
      ~~~csharp
        public int SkillCD => GameData.commonSkillCD / 30 * 5;
        public int DurationTime => GameData.commonSkillTime / 2;
      ~~~
    - 在DurationTime内，攻击类型变为蹦蹦炸弹
      ~~~csharp
        internal sealed class BombBomb : Bullet
        {
        public override double BulletBombRange => GameData.basicBulletBombRange;
        public override double BulletAttackRange => GameData.basicAttackShortRange;
        public int ap = (int)(GameData.basicApOfGhost * 6.0 / 5);
        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 0.8);
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)BulletAttackRange / Speed;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = GameData.basicCD;

        public override bool CanAttack(GameObj target)
        {
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                    return true;
                default:
                    return false;
            }
        }
      }
      ~~~
    - 当蹦蹦炸弹因为碰撞而爆炸，向子弹方向上加上0°，90°，180°，270° 发出四个小炸弹
    - 四个小炸弹只会因为碰撞爆炸，停止运动后学生碰撞会造成眩晕（AP / GameData.timeFactorOfGhostFainting）ms
    ~~~csharp
    internal sealed class JumpyDumpty : Bullet
    {
        public override double BulletBombRange => GameData.basicBulletBombRange / 2;
        public override double BulletAttackRange => GameData.basicAttackShortRange * 2;
        public int ap = (int)(GameData.basicApOfGhost * 0.6);

        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 1.2);
        public override bool IsRemoteAttack => false;

        public override int CastTime => 0;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = 0;
        public override int CD => cd;

        public override bool CanAttack(GameObj target)
        {
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                    return true;
                default:
                    return false;
            }
        }
    }
    ~~~

#### 喧哗者
  - 主动技能
    - 嚎叫
    使用瞬间，在视野半径范围内（不是可视区域）的学生被眩晕（GameData.TimeOfStudentFaintingWhenHowl）ms，自己进入（GameData.TimeOfGhostSwingingAfterHowl）ms的后摇
  - 被动技能
    在场所有学生Bgm系统被设为无用的值


### 学生（&老师）

#### 运动员
    ~~~csharp
        private const int moveSpeed = GameData.basicMoveSpeed * 40 / 38;
        private const int maxHp = GameData.basicHp * 32 / 30;
        public const int maxBulletNum = 0;
        public BulletType InitBullet => BulletType.Null;
    
        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.CanBeginToCharge });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });
    
        public const int fixSpeed = GameData.basicFixSpeed * 6 / 10;
        int treatSpeed = GameData.basicTreatSpeed * 8 / 10;
    
        public const double concealment = GameData.basicConcealment * 0.9;
        int alertnessRadius = (int)(GameData.basicAlertnessRadius * 0.9);
        int viewRange = (int)(GameData.basicViewRange * 1.1);
        int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking * 12 / 10;
        int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
    ~~~
  - 主动技能 
    - 冲撞
    ~~~csharp
        public int SkillCD => GameData.commonSkillCD / 5;
        public int DurationTime => GameData.commonSkillTime * 6 / 10;
    ~~~
    在DurationTime内，速度变为三倍，期间撞到捣蛋鬼，会导致捣蛋鬼眩晕7.22s,学生眩晕2.09s

#### 教师
    ~~~csharp
        private const int moveSpeed = GameData.basicMoveSpeed * 3 / 4;
        int maxHp = GameData.basicHp * 10;
        int maxBulletNum = 0;
        BulletType InitBullet => BulletType.Null;
    
        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Punish });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });
    
        public const int fixSpeed = 0;
        int treatSpeed = GameData.basicTreatSpeed;
        double concealment = GameData.basicConcealment * 0.5;
        int alertnessRadius = GameData.basicAlertnessRadius / 2;
        int viewRange = GameData.basicViewRange * 9 / 10;
        int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows /2;
        int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
    ~~~
- 主动技能
  - 惩罚
    ~~~csharp
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => 0;
    ~~~
  使用瞬间，在可视范围内的使用技能状态中、攻击前后摇、开锁门、开箱的捣蛋鬼会被眩晕（3070+ 玩家损失的血量 / 1000）ms，

#### 学霸
- 被动技能
  - 冥想
    -当玩家处于可接受指令状态且不在修机时，会积累学习进度，速度为（GameData.basicFixSpeed / 4）/ms
    -受到攻击或眩晕学习进度清零
-主动技能
  - 写答案
    -使用瞬间，对于可互动范围内的一台电机增加这个学习进度
  

## 游戏数据
请自行查看Logic/Preparation/Utility/GameData.cs


## 游戏内的枚举类型、类与属性
- 底层实现中的属性，不代表界面全部都需要展示，也可能需要额外展示信息
- 只展示外部需要的属性，部分属性被省略

### BgmType
- 枚举类BgmType
  1. 不详的感觉：监管者进入（求生者的警戒半径/监管者的隐蔽度）时，求生者收到；监管者距离求生者越近，Bgm音量越大。bgmVolume=（警戒半径/二者距离）
  2. 期待搞事的感觉：求生者进入（监管者的警戒半径/求生者的隐蔽度）时，监管者收到；监管者距离求生者越近，Bgm音量越大。bgmVolume=（警戒半径/可被发觉的最近的求生者距离）
  3. 修理电机的声音: 监管者警戒半径内有电机正在被修理时收到；bgmVolume=（警戒半径*电机修理程度/二者距离）/10300000
  ~~~csharp
    public enum BgmType
    {
        Null = 0,
        GhostIsComing = 1,
        StudentIsApproaching = 2,
        GeneratorIsBeingFixed = 3,
    }
  ~~~

### 技能
  ~~~csharp
    public enum ActiveSkillType  // 主动技能
    {
        Null = 0,
        BecomeInvisible = 1,
        ...
        UseKnife = 5,
        BeginToCharge = 6
    }
    public enum PassiveSkillType
    {
        Null = 0,
    }
  ~~~

### 物体
- 位置
- 位置地形
- ID
- 类型
- 面向角度
- 形状
- 半径
- 是否可移动
- 是否在移动
- 移动速度

### 人物：物体

- 装弹CD
- 持有子弹数量
- 血量
- 最大血量
- 玩家状态（不可叠加）
  ~~~csharp
    public enum PlayerStateType
    {
        Null = 0,
        Addicted = 1,
        Escaped = 2,
        Swinging = 3,//指后摇
        Deceased = 4,
        Moving = 5,
        Treating = 6,
        Rescuing = 7,
        Fixing = 8,
        Treated = 9,
        Rescued = 10,
        Stunned = 11,
        TryingToAttack = 12,//指前摇
        LockingOrOpeningTheDoor = 13,
        OpeningTheChest = 14,
        ClimbingThroughWindows = 15,
        UsingSkill = 16,
        OpeningTheDoorway = 17,
    }
  ~~~
- 可执行指令的（不用给选手）
  ~~~csharp
  public bool Commandable() => (playerState != PlayerStateType.IsDeceased && playerState != PlayerStateType.IsEscaped
                                && playerState != PlayerStateType.IsAddicted && playerState != PlayerStateType.IsRescuing
                                && playerState != PlayerStateType.IsSwinging && playerState != PlayerStateType.IsTryingToAttack
                                && playerState != PlayerStateType.IsClimbingThroughWindows && playerState != PlayerStateType.IsStunned);
  ~~~
- Bgm(字典)
- 得分
- ~~回血率/原始回血率~~
- 当前子弹类型
- 原始子弹类型
- 持有道具 （最多三个）(数组)
- 是否隐身
- 队伍ID
- 玩家ID
- 当前Buff
- 职业类型
- 拥有的被动技能(列表)
- 拥有的主动技能(列表)
- 各个主动技能CD(字典)
- 警戒半径
- double 隐蔽度
- 翻窗时间
- 开锁门速度
- 开箱速度
- 视野范围

### 学生：人物
- 修理电机速度
- 治疗速度
- ~~救人速度~~
- ~~自愈次数~~
- 沉迷游戏程度
- 最大沉迷游戏程度
- 被治疗程度
- *被救援程度*

### 搞蛋鬼：人物
无

### 队伍：
- 队伍ID
- 队伍得分
- 队伍成员

### 所有物：
- 主人

### 子弹：所有物
- 子弹攻击范围
- 子弹爆炸范围
- 子弹攻击力
- 是否可以穿墙
- 是否为远程攻击
- 移动速度
- 子弹类型

### 爆炸中的子弹：物体
- 原来的子弹ID
- 爆炸范围
- 子弹类型

### 道具：所有物
- 道具类型

### 被捡起的道具：物体
- 原来的道具ID
- 道具类型

### 出口：物体
- 电力供应（是否可以被打开）
- 开启进度

### 紧急出口：物体
- 是否显现
- 是否打开

### 墙：物体

### 窗：物体
- 正在翻窗的人

### 箱子：物体
- 开箱进度

### 门：物体
- 属于那个教学区
- 是否锁上
- 开锁门进度
- 不提供是否可以锁上的属性

### 电机（建议称为homework）：物体
- 修理程度

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
| T            | （Stu）开始治疗（学习毅力下降的同伴）          |
| G            | （Stu）发出毕业请求                            |
| H            | （Stu）申请毕业（或称为开大门）                |
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
