using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading;

namespace GameClass.GameObj
{
    public partial class Character : Moveable, ICharacter  // 负责人LHR摆烂终了
    {
        private readonly object beAttackedLock = new();

        #region 角色的基本属性及方法，包括与道具的交互方法
        /// <summary>
        /// 装弹冷却
        /// </summary>
        protected int cd;
        public int CD
        {
            get => cd;
            private set
            {
                lock (gameObjLock)
                {
                    cd = value;
                    Debugger.Output(this, string.Format("'s CD has been set to: {0}.", value));
                }
            }
        }
        public int OrgCD { get; protected set; }

        protected int maxBulletNum;
        public int MaxBulletNum => maxBulletNum;  // 人物最大子弹数
        protected int bulletNum;
        public int BulletNum => bulletNum;  // 目前持有的子弹数

        public int MaxHp { get; protected set; }  // 最大血量
        protected int hp;
        public int HP
        {
            get => hp;
            set
            {
                if (value > 0)
                {
                    lock (gameObjLock)
                        hp = value <= MaxHp ? value : MaxHp;
                }
                else
                    lock (gameObjLock)
                        hp = 0;
            }
        }

        private PlayerStateType playerState = PlayerStateType.Null;
        public PlayerStateType PlayerState
        {
            get
            {
                if (IsMoving) return PlayerStateType.IsMoving;
                return playerState;
            }
            set
            {
                if (!(value == PlayerStateType.IsMoving))
                    lock (gameObjLock)
                        IsMoving = false;

                lock (gameObjLock) playerState = (value == PlayerStateType.IsMoving) ? PlayerStateType.Null : value;
            }
        }

        public bool Commandable() => (playerState != PlayerStateType.IsDeceased && playerState != PlayerStateType.IsEscaped
                                                            && playerState != PlayerStateType.IsAddicted && playerState != PlayerStateType.IsStunned
                                                             && playerState != PlayerStateType.IsSwinging && playerState != PlayerStateType.IsTryingToAttack
                                                              && playerState != PlayerStateType.IsClimbingThroughWindows);
        public bool InteractingWithMapWithoutMoving() => (playerState == PlayerStateType.IsLockingTheDoor || playerState == PlayerStateType.IsFixing || playerState == PlayerStateType.IsRummagingInTheChest);
        public bool NullOrMoving() => (playerState == PlayerStateType.Null || playerState == PlayerStateType.IsMoving);

        //        private int deathCount = 0;
        //       public int DeathCount => deathCount;  // 玩家的死亡次数

        private int score = 0;
        public int Score
        {
            get => score;
        }

        //      public double AttackRange => BulletFactory.BulletAttackRange(this.BulletOfPlayer);

        private double vampire = 0;  // 回血率：0-1之间
        public double Vampire
        {
            get => vampire;
            set
            {
                if (value > 1)
                    lock (gameObjLock)
                        vampire = 1;
                else if (value < 0)
                    lock (gameObjLock)
                        vampire = 0;
                else
                    lock (gameObjLock)
                        vampire = value;
            }
        }
        private double oriVampire = 0;
        public double OriVampire
        {
            get => oriVampire;
            set
            {
                if (value > 1)
                    lock (gameObjLock)
                        vampire = 1;
                else if (value < 0)
                    lock (gameObjLock)
                        vampire = 0;
                else
                    lock (gameObjLock)
                        vampire = value;
            }
        }

        public readonly BulletType OriBulletOfPlayer;
        private BulletType bulletOfPlayer;
        public BulletType BulletOfPlayer
        {
            get => bulletOfPlayer;
            set
            {
                lock (gameObjLock)
                    bulletOfPlayer = value;
            }
        }

        private Prop? propInventory;
        public Prop? PropInventory  // 持有的道具
        {
            get => propInventory;
            set
            {
                lock (gameObjLock)
                {
                    propInventory = value;
                    Debugger.Output(this, " prop becomes " + (PropInventory == null ? "null" : PropInventory.ToString()));
                }
            }
        }

        /// <summary>
        /// 使用物品栏中的道具
        /// </summary>
        /// <returns>被使用的道具</returns>
        public Prop? UseProp()
        {
            lock (gameObjLock)
            {
                var oldProp = PropInventory;
                PropInventory = null;
                return oldProp;
            }
        }

        /// <summary>
        /// 是否在隐身
        /// </summary>
        private bool isInvisible = false;
        public bool IsInvisible
        {
            get => isInvisible;
            set
            {
                lock (gameObjLock)
                {
                    isInvisible = value;
                }
            }
        }

        private Dictionary<BgmType, double> bgmDictionary = new();
        public Dictionary<BgmType, double> BgmDictionary
        {
            get => bgmDictionary;
            set
            {
                lock (gameObjLock)
                {
                    bgmDictionary = value;
                }
            }
        }

        private int alertnessRadius;
        public int AlertnessRadius
        {
            get => alertnessRadius;
            set
            {
                lock (gameObjLock)
                {
                    alertnessRadius = value;
                }
            }
        }

        private double concealment;
        public double Concealment
        {
            get => concealment;
            set
            {
                lock (gameObjLock)
                {
                    concealment = value;
                }
            }
        }

        private int timeOfOpeningOrLocking;
        public int TimeOfOpeningOrLocking
        {
            get => timeOfOpeningOrLocking;
            set
            {
                lock (gameObjLock)
                {
                    timeOfOpeningOrLocking = value;
                }
            }
        }

        private int timeOfClimbingThroughWindows;
        public int TimeOfClimbingThroughWindows
        {
            get => timeOfClimbingThroughWindows;
            set
            {
                lock (gameObjLock)
                {
                    timeOfClimbingThroughWindows = value;
                }
            }
        }

        /// <summary>
        /// 进行一次攻击
        /// </summary>
        /// <returns>攻击操作发出的子弹</returns>
        public Bullet? Attack(XY pos, PlaceType place)
        {
            if (TrySubBulletNum())
                return BulletFactory.GetBullet(this, place, pos);
            else
                return null;
        }

        /// <summary>
        /// 尝试将子弹数量减1
        /// </summary>
        /// <returns>减操作是否成功</returns>
        private bool TrySubBulletNum()
        {
            lock (gameObjLock)
            {
                if (bulletNum > 0)
                {
                    --bulletNum;
                    return true;
                }
                return false;
            }
        }
        /// <summary>
        /// 尝试将子弹数量加1
        /// </summary>
        /// <returns>加操作是否成功</returns>
        public bool TryAddBulletNum()
        {
            lock (gameObjLock)
            {
                if (bulletNum < maxBulletNum)
                {
                    ++bulletNum;
                    return true;
                }
                return false;
            }
        }

        /// <summary>
        /// 尝试加血
        /// </summary>
        /// <param name="add">欲加量</param>
        /// <returns>加操作是否成功</returns>
        public bool TryAddHp(int add)
        {
            if (hp < MaxHp)
            {
                lock (gameObjLock)
                    hp = MaxHp > hp + add ? hp + add : MaxHp;
                Debugger.Output(this, " hp has added to: " + hp.ToString());
                return true;
            }
            return false;
        }
        /// <summary>
        /// 尝试减血
        /// </summary>
        /// <param name="sub">减血量</param>
        /// <returns>减操作是否成功</returns>
        public int TrySubHp(int sub)
        {
            int previousHp = hp;
            lock (gameObjLock)
                hp = hp >= sub ? 0 : hp - sub;
            Debugger.Output(this, " hp has subed to: " + hp.ToString());
            return previousHp - hp;
        }
        /*       /// <summary>
               /// 增加死亡次数
               /// </summary>
               /// <returns>当前死亡次数</returns>
               private int AddDeathCount()
               {
                   lock (gameObjLock)
                   {
                       ++deathCount;
                       return deathCount;
                   }
               }*/
        /// <summary>
        /// 加分
        /// </summary>
        /// <param name="add">增加量</param>
        public void AddScore(int add)
        {
            lock (gameObjLock)
            {
                score += add;
                Debugger.Output(this, " 's score has been added to: " + score.ToString());
            }
        }
        /// <summary>
        /// 减分
        /// </summary>
        /// <param name="sub">减少量</param>
        public void SubScore(int sub)
        {
            lock (gameObjLock)
            {
                score -= sub;
                Debugger.Output(this, " 's score has been subed to: " + score.ToString());
            }
        }

        /// <summary>
        /// 遭受攻击
        /// </summary>
        /// <param name="subHP"></param>
        /// <param name="hasSpear"></param>
        /// <param name="attacker">伤害来源</param>
        /// <returns>人物在受到攻击后死了吗</returns>
        public bool BeAttacked(Bullet bullet)
        {

            lock (beAttackedLock)
            {
                if (hp <= 0)
                    return false;  // 原来已经死了
                if (bullet.Parent.TeamID != this.TeamID)
                {

                    if (HasShield)
                    {
                        if (bullet.HasSpear)
                            _ = TrySubHp(bullet.AP);
                        else
                            return false;
                    }
                    else
                    {
                        bullet.Parent.HP = (int)(bullet.Parent.HP + (bullet.Parent.Vampire * TrySubHp(bullet.AP)));
                    }
#if DEBUG
                    Console.WriteLine($"PlayerID:{ID} is being shot! Now his hp is {hp}.");
#endif
                    if (hp <= 0)
                        TryActivatingLIFE();  // 如果有复活甲
                }
                return hp <= 0;
            }
        }
        /// <summary>
        /// 攻击被反弹，反弹伤害不会再被反弹
        /// </summary>
        /// <param name="subHP"></param>
        /// <param name="hasSpear"></param>
        /// <param name="bouncer">反弹伤害者</param>
        /// <returns>是否因反弹伤害而死</returns>
        private bool BeBounced(int subHP, bool hasSpear, Character? bouncer)
        {
            lock (beAttackedLock)
            {
                if (hp <= 0)
                    return false;
                if (!(bouncer?.TeamID == this.TeamID))
                {
                    if (hasSpear || !HasShield)
                        _ = TrySubHp(subHP);
                    if (hp <= 0)
                        TryActivatingLIFE();
                }
                return hp <= 0;
            }
        }

        /// <summary>
        /// 角色所属队伍ID
        /// </summary>
        private long teamID = long.MaxValue;
        public long TeamID
        {
            get => teamID;
            set
            {
                lock (gameObjLock)
                {
                    teamID = value;
                    Debugger.Output(this, " joins in the team: " + value.ToString());
                }
            }
        }
        private long playerID = long.MaxValue;
        public long PlayerID
        {
            get => playerID;
            set
            {
                lock (gameObjLock)
                {
                    playerID = value;
                }
            }
        }
        /// <summary>
        /// 角色携带的信息
        /// </summary>
        private string message = "THUAI6";
        public string Message
        {
            get => message;
            set
            {
                lock (gameObjLock)
                {
                    message = value;
                }
            }
        }
        #endregion

        #region 角色拥有的buff相关属性、方法
        public void AddMoveSpeed(int buffTime, double add = 2.0) => buffManager.AddMoveSpeed(add, buffTime, newVal =>
                                                                                                            { MoveSpeed = newVal < GameData.characterMaxSpeed ? newVal : GameData.characterMaxSpeed; },
                                                                                             OrgMoveSpeed);
        public bool HasFasterSpeed => buffManager.HasFasterSpeed;

        public void AddShield(int shieldTime) => buffManager.AddShield(shieldTime);
        public bool HasShield => buffManager.HasShield;

        public void AddLIFE(int LIFETime) => buffManager.AddLIFE(LIFETime);
        public bool HasLIFE => buffManager.HasLIFE;

        public void AddSpear(int spearTime) => buffManager.AddSpear(spearTime);
        public bool HasSpear => buffManager.HasSpear;

        private Array buffTypeArray = Enum.GetValues(typeof(BuffType));
        public Dictionary<BuffType, bool> Buff
        {
            get
            {
                Dictionary<BuffType, bool> buff = new Dictionary<BuffType, bool>();
                foreach (BuffType type in buffTypeArray)
                {
                    if (type != BuffType.Null)
                        buff.Add(type, GetBuffStatus(type));
                }
                return buff;
            }
        }
        private bool GetBuffStatus(BuffType type)
        {
            switch (type)
            {
                case BuffType.Spear:
                    return this.HasSpear;
                case BuffType.AddSpeed:
                    return this.HasFasterSpeed;
                case BuffType.Shield:
                    return this.HasShield;
                case BuffType.AddLIFE:
                    return this.HasLIFE;
                default:
                    return false;
            }
        }
        private void TryActivatingLIFE()
        {
            if (buffManager.TryActivatingLIFE())
            {
                hp = MaxHp;
            }
        }
        #endregion
        /*     public override void Reset()  // 要加锁吗？
             {
                 lock (gameObjLock)
                 {
                     //         _ = AddDeathCount();
                     base.Reset();
                     this.MoveSpeed = OrgMoveSpeed;
                     HP = MaxHp;
                     PropInventory = null;
                     BulletOfPlayer = OriBulletOfPlayer;
                     lock (gameObjLock)
                         bulletNum = maxBulletNum;

                     buffManager.ClearAll();
                     IsInvisible = false;
                     this.Vampire = this.OriVampire;
                 }
             }*/
        public void Die(PlayerStateType playerStateType)
        {
            lock (gameObjLock)
            {
                playerState = playerStateType;
                CanMove = false;
                IsResetting = true;
                Position = GameData.PosWhoDie;
                place = PlaceType.Grass;
            }
        }

        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Circle;
        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (IsResetting)
                return true;
            if (targetObj.Type == GameObjType.Prop)  // 自己队的地雷忽略碰撞
            {
                return true;
            }
            return false;
        }
    }
}
