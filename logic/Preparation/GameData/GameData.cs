using Preparation.Utility;

namespace Preparation.GameData
{
    public static class GameData
    {
        #region 基本常数与常方法
        public const int numOfPosGridPerCell = 1000;  // 每格的【坐标单位】数
        public const int numOfStepPerSecond = 20;     // 每秒行走的步数
        public const int lengthOfMap = 50000;         // 地图长度
        public const int rows = 50;                   // 行数
        public const int cols = 50;                   // 列数
        public const long gameDuration = 600000;      // 游戏时长600000ms = 10min
        public const long frameDuration = 50;         // 每帧时长

        public const int MinSpeed = 1;             // 最小速度
        public const int MaxSpeed = int.MaxValue;  // 最大速度

        public static XY GetCellCenterPos(int x, int y)  // 求格子的中心坐标
        {
            XY ret = new((x * numOfPosGridPerCell) + (numOfPosGridPerCell / 2), (y * numOfPosGridPerCell) + (numOfPosGridPerCell / 2));
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
        public static bool IsInTheSameCell(XY pos1, XY pos2)
        {
            return PosGridToCellX(pos1) == PosGridToCellX(pos2) && PosGridToCellY(pos1) == PosGridToCellY(pos2);
        }

        public static int numOfBirthPoint = 5;
        #endregion
        #region 角色相关
        /// <summary>
        /// 玩家相关
        /// </summary>
        public const int characterRadius = numOfPosGridPerCell / 2;  // 人物半径
        public const int basicAp = 3000;                             // 初始攻击力
        public const int basicHp = 6000;                             // 初始血量
        public const int basicCD = 3000;                             // 初始子弹冷却
        public const int basicBulletNum = 3;                         // 基本初始子弹量
        public const int MinAP = 0;                                  // 最小攻击力
        public const int MaxAP = int.MaxValue;                       // 最大攻击力
        public const double basicAttackRange = 9000;                 // 基本攻击范围
        public const double basicBulletBombRange = 3000;             // 基本子弹爆炸范围
        public const int basicMoveSpeed = 3000;                      // 基本移动速度，单位：s-1
        public const int basicBulletMoveSpeed = 3000;                // 基本子弹移动速度，单位：s-1
        public const int characterMaxSpeed = 12000;                  // 最大速度
        public const int addScoreWhenKillOneLevelPlayer = 30;        // 击杀一级角色获得的加分
        public const int commonSkillCD = 30000;                      // 普通技能标准冷却时间
        public const int commonSkillTime = 10000;                    // 普通技能标准持续时间
        public const int bulletRadius = 200;                         // 默认子弹半径
        public const int reviveTime = 30000;                         // 复活时间
        public const int shieldTimeAtBirth = 3000;                   // 复活时的护盾时间
        public const int gemToScore = 4;                             // 一个宝石的标准加分
        /// <summary>
        /// 道具相关
        /// </summary>
        public const int MinPropTypeNum = 1;
        public const int MaxPropTypeNum = 10;
        public const int PropRadius = numOfPosGridPerCell / 2;
        public const int PropMoveSpeed = 3000;
        public const int PropMaxMoveDistance = 15 * numOfPosGridPerCell;
        public const int MaxGemSize = 5;  // 随机生成的宝石最大size
        public const long GemProduceTime = 10000;
        public const long PropProduceTime = 10000;
        public const int PropDuration = 10000;
        #endregion
        #region 游戏帧相关
        public const long checkInterval = 50;  // 检查位置标志、补充子弹的帧时长
        #endregion
    }
}
