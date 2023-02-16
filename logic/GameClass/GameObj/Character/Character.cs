using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading;

namespace GameClass.GameObj
{
    public partial class Character : GameObj, ICharacter  // 负责人LHR摆烂终了
    {
        private readonly object beAttackedLock = new();

        #region 角色的基本属性及方法，包括与道具的交互方法

        protected int fixSpeed = 1;
        /// <summary>
        /// 修理电机速度
        /// </summary>
        public int FixSpeed
        {
            get => fixSpeed;
            set
            {
                lock (gameObjLock)
                {
                    fixSpeed = value;
                }
            }
        }
        /// <summary>
        /// 原初修理电机速度
        /// </summary>
        public int OrgFixSpeed { get; protected set; }

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
                lock (gameObjLock)
                    hp = value <= MaxHp ? value : MaxHp;
            }
        }

        private bool isEscaped = false;
        public bool IsEscaped
        {
            get => isEscaped;
            set
            {
                lock (gameObjLock)
                    if (!isEscaped && !IsGhost())
                        isEscaped = value;
            }
        }

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
        /// 是否正在更换道具（包括捡起与抛出）
        /// </summary>
        private bool isModifyingProp = false;
        public bool IsModifyingProp
        {
            get => isModifyingProp;
            set
            {
                lock (gameObjLock)
                {
                    isModifyingProp = value;
                }
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

        /// <summary>
        /// 进行一次远程攻击
        /// </summary>
        /// <param name="posOffset">子弹初始位置偏差值</param>
        /// <returns>攻击操作发出的子弹</returns>
        public Bullet? RemoteAttack(XY posOffset)
        {
            if (TrySubBulletNum())
                return ProduceOneBullet(this.Position + posOffset);
            else
                return null;
        }
        protected Bullet? ProduceOneBullet(XY initPos)
        {
            var newBullet = BulletFactory.GetBullet(this);
            newBullet?.SetPosition(initPos);
            return newBullet;
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
        public void AddMoveSpeed(int buffTime, double add = 2.0) => buffManeger.AddMoveSpeed(add, buffTime, newVal =>
                                                                                                            { MoveSpeed = newVal < GameData.characterMaxSpeed ? newVal : GameData.characterMaxSpeed; },
                                                                                             OrgMoveSpeed);
        public bool HasFasterSpeed => buffManeger.HasFasterSpeed;

        public void AddShield(int shieldTime) => buffManeger.AddShield(shieldTime);
        public bool HasShield => buffManeger.HasShield;

        public void AddLIFE(int LIFETime) => buffManeger.AddLIFE(LIFETime);
        public bool HasLIFE => buffManeger.HasLIFE;

        public void AddSpear(int spearTime) => buffManeger.AddSpear(spearTime);
        public bool HasSpear => buffManeger.HasSpear;

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
            if (buffManeger.TryActivatingLIFE())
            {
                hp = MaxHp;
            }
        }
        #endregion
        public override void Reset()  // 要加锁吗？
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

                buffManeger.ClearAll();
                IsInvisible = false;
                this.Vampire = this.OriVampire;
            }
        }

        public void Escape()
        {
            lock (gameObjLock)
                IsResetting = IsEscaped = true;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Circle;
        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.BirthPoint)
            {
                if (object.ReferenceEquals(((BirthPoint)targetObj).Parent, this))  // 自己的出生点可以忽略碰撞
                {
                    return true;
                }
            }
            else if (targetObj.Type == GameObjType.Prop)  // 自己队的地雷忽略碰撞
            {
                return true;
            }
            return false;
        }
    }
}
