using System;
using System.Net.NetworkInformation;

namespace Preparation.Utility
{
    public static class GameData
    {
        #region 基本常数
        public const int numOfStepPerSecond = 20;     // 每秒行走的步数
        public const int frameDuration = 50;         // 每帧时长
        public const int checkInterval = 50;  // 检查位置标志、补充子弹的帧时长

        public const long gameDuration = 600000;      // 游戏时长600000ms = 10min

        public const int MinSpeed = 1;             // 最小速度
        public const int MaxSpeed = int.MaxValue;  // 最大速度
        #endregion
        #region 地图相关
        public const int numOfPosGridPerCell = 1000;  // 每格的【坐标单位】数
        public const int lengthOfMap = 50000;         // 地图长度
        public const int rows = 50;                   // 行数
        public const int cols = 50;                   // 列数

        public const int numOfBirthPoint = 5;
        //        public const int numOfGenerator = 9;
        public const int numOfChest = 8;

        private const int numOfObjNotMap = 5;
        public static bool IsMap(GameObjType gameObjType)
        {
            return (uint)gameObjType > numOfObjNotMap;
        }

        public static XY GetCellCenterPos(int x, int y)  // 求格子的中心坐标
        {
            XY ret = new(x * numOfPosGridPerCell + numOfPosGridPerCell / 2, y * numOfPosGridPerCell + numOfPosGridPerCell / 2);
            return ret;
        }
        public static int PosGridToCellX(XY pos)  // 求坐标所在的格子的x坐标
        {
            return pos.x / numOfPosGridPerCell;
        }
        public static int PosGridToCellY(XY pos)  // 求坐标所在的格子的y坐标
        {
            return pos.y / numOfPosGridPerCell;
        }
        public static XY PosGridToCellXY(XY pos)  // 求坐标所在的格子的y坐标
        {
            return new XY(pos.x / numOfPosGridPerCell, pos.y / numOfPosGridPerCell);
        }
        public static bool IsInTheSameCell(XY pos1, XY pos2)
        {
            return PosGridToCellX(pos1) == PosGridToCellX(pos2) && PosGridToCellY(pos1) == PosGridToCellY(pos2);
        }
        public static bool ApproachToInteract(XY pos1, XY pos2)
        {
            return Math.Abs(PosGridToCellX(pos1) - PosGridToCellX(pos2)) <= 1 && Math.Abs(PosGridToCellY(pos1) - PosGridToCellY(pos2)) <= 1;
        }
        public static bool ApproachToInteractInACross(XY pos1, XY pos2)
        {
            return (Math.Abs(PosGridToCellX(pos1) - PosGridToCellX(pos2)) + Math.Abs(PosGridToCellY(pos1) - PosGridToCellY(pos2))) <= 1;
        }
        #endregion
        #region 角色相关
        public const int numOfStudent = 4;
        public const int characterRadius = numOfPosGridPerCell / 2 / 5 * 4;  // 人物半径

        public const int basicTreatSpeed = 100;
        public const int basicFixSpeed = 100;
        public const int basicSpeedOfOpeningOrLocking = 3280;
        public const int basicStudentSpeedOfClimbingThroughWindows = 611;
        public const int basicGhostSpeedOfClimbingThroughWindows = 1270;
        public const int basicSpeedOfOpenChest = 1000;

        public const int basicHp = 3000000;                             // 初始血量
        public const int basicMaxGamingAddiction = 60000;//基本完全沉迷时间
        public const int BeginGamingAddiction = 10003;
        public const int MidGamingAddiction = 30000;
        public const int basicTreatmentDegree = 1500000;
        public const int basicTimeOfRescue = 1000;

        public const int basicMoveSpeed = 1270;                      // 基本移动速度，单位：s-1
        public const int characterMaxSpeed = 12000;                  // 最大速度
        public const int basicBulletMoveSpeed = 2700;                // 基本子弹移动速度，单位：s-1

        public const double basicConcealment = 1.0;
        public const int basicAlertnessRadius = 30700;
        public const int basicViewRange = 5 * numOfPosGridPerCell;
        public const int maxNumOfPropInPropInventory = 3;

        public static XY PosWhoDie = new XY(1, 1);

        public static bool IsGhost(CharacterType characterType)
        {
            return characterType switch
            {
                CharacterType.Assassin => true,
                _ => false,
            };
        }
        #endregion
        #region 得分相关
        public static int TrickerScoreAttackStudent(int damage)
        {
            return damage * 100 / basicApOfGhost;
        }
        public const int TrickerScoreStudentBeAddicted = 50;
        public const int TrickerScoreStudentBeStunned = 25;
        public const int TrickerScoreStudentDie = 1000;

        public static int StudentScoreFix(int degreeOfFix)
        {
            return degreeOfFix;
        }
        public const int StudentScoreFixed = 25;
        public static int StudentScorePinDown(int timeOfPiningDown)
        {
            return 0;
        }
        public const int StudentScoreTrickerBeStunned = 25;
        public const int StudentScoreRescue = 100;
        public static int StudentScoreTreat(int degree)
        {
            return degree;
        }
        public const int StudentScoreEscape = 1000;
        public const int ScorePropRemainHp = 20;
        public const int ScorePropUseShield = 20;
        public const int ScorePropUseSpear = 20;
        public const int ScorePropAddAp = 10;

        public static int ScoreUseProp(PropType prop, bool IsGhost)
        {
            return 0;
        }
        public static int ScoreUseSkill(ActiveSkillType activeSkillType)
        {
            return 0;
        }
        #endregion
        #region 攻击与子弹相关
        public const int basicApOfGhost = 1500000;                             // 捣蛋鬼攻击力
        public const int MinAP = 0;                                  // 最小攻击力
        public const int MaxAP = int.MaxValue;                       // 最大攻击力

        public const int basicCD = 3000;    // 初始子弹冷却
        public const int basicCastTime = 500;//基本前摇时间
        public const int basicBackswing = 818;//基本后摇时间
        public const int basicRecoveryFromHit = 4300;//基本命中攻击恢复时长
        public const int basicStunnedTimeOfStudent = 4130;

        public const int bulletRadius = 200;                         // 默认子弹半径
        public const int basicBulletNum = 3;                         // 基本初始子弹量
        public const double basicRemoteAttackRange = 9000;  // 基本远程攻击范围
        public const double basicAttackShortRange = 2700;                 // 基本近程攻击范围
        public const double basicBulletBombRange = 3000;             // 基本子弹爆炸范围
        #endregion
        #region 技能相关
        public const int maxNumOfSkill = 3;
        public const int commonSkillCD = 30000;                      // 普通技能标准冷却时间
        public const int commonSkillTime = 10000;                    // 普通技能标准持续时间
        /// <summary>
        /// BeginToCharge
        /// </summary>
        public const int TimeOfGhostFainting = 7220;//=AP of Ram
        public const int TimeOfStudentFainting = 2090;

        #endregion
        #region 道具相关
        public const int MinPropTypeNum = 1;
        public const int MaxPropTypeNum = 10;
        public const int PropRadius = numOfPosGridPerCell / 2;
        public const int PropMoveSpeed = 3000;
        public const int PropMaxMoveDistance = 15 * numOfPosGridPerCell;
        public const long PropProduceTime = 10000;
        public const int PropDuration = 10000;

        public const int ApPropAdd = basicApOfGhost * 12 / 10;
        public const int ApSpearAdd = basicApOfGhost * 6 / 10;
        public const int RemainHpWhenAddLife = 100;

        public const int numOfKeyEachArea = 2;
        public const int numOfPropTypeNotKey = 4;
        public const int numOfTeachingBuilding = 3;
        #endregion
        #region 物体相关
        public const int degreeOfFixedGenerator = 10300000;
        public const int degreeOfLockingOrOpeningTheDoor = 10000;
        public const int degreeOfOpeningChest = 10000;
        public const int degreeOfOpenedDoorway = 18000;
        public const int maxNumOfPropInChest = 2;
        public const int numOfGeneratorRequiredForRepair = 7;
        public const int numOfGeneratorRequiredForEmergencyExit = 3;
        #endregion
    }
}
