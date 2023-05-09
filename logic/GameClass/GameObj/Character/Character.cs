using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;

namespace GameClass.GameObj
{
    public partial class Character : Moveable, ICharacter  // 负责人LHR摆烂终了
    {
        #region 装弹、攻击相关的基本属性及方法
        /// <summary>
        /// 装弹冷却
        /// </summary>
        protected int cd;
        public int CD
        {
            get
            {
                lock (actionLock)
                {
                    return cd;
                }
            }
        }
        public int OrgCD { get; protected set; }

        public readonly BulletType OriBulletOfPlayer;
        private BulletType bulletOfPlayer;
        public BulletType BulletOfPlayer
        {
            get
            {
                lock (actionLock)
                {
                    return bulletOfPlayer;
                }
            }
            set
            {
                lock (actionLock)
                {
                    bulletOfPlayer = value;
                    cd = OrgCD = (BulletFactory.BulletCD(value));
                    Debugger.Output(this, string.Format("'s CD has been set to: {0}.", cd));
                    maxBulletNum = bulletNum = (BulletFactory.BulletNum(value));
                }
            }
        }

        protected int maxBulletNum;
        public int MaxBulletNum
        {
            get
            {
                lock (actionLock)
                {
                    return maxBulletNum;
                }
            }
        }
        private int bulletNum;
        private int updateTimeOfBulletNum = 0;

        public int UpdateBulletNum(int time)
        {
            lock (actionLock)
            {
                if (bulletNum < maxBulletNum)
                {
                    int add = Math.Min(maxBulletNum - bulletNum, (time - updateTimeOfBulletNum) / cd);
                    updateTimeOfBulletNum += add * cd;
                    return (bulletNum += add);
                }
                return maxBulletNum;
            }
        }

        /// <summary>
        /// 进行一次攻击
        /// </summary>
        /// <returns>攻击操作发出的子弹</returns>
        public Bullet? Attack(double angle, int time)
        {
            lock (actionLock)
            {
                if (bulletOfPlayer == BulletType.Null)
                    return null;
                if (UpdateBulletNum(time) > 0)
                {
                    if (bulletNum == maxBulletNum) updateTimeOfBulletNum = time;
                    --bulletNum;

                    XY res = Position + new XY  // 子弹紧贴人物生成。
                        (
                            (int)(Math.Abs((Radius + BulletFactory.BulletRadius(bulletOfPlayer)) * Math.Cos(angle))) * Math.Sign(Math.Cos(angle)),
                            (int)(Math.Abs((Radius + BulletFactory.BulletRadius(bulletOfPlayer)) * Math.Sin(angle))) * Math.Sign(Math.Sin(angle))
                        );
                    Bullet? bullet = BulletFactory.GetBullet(this, res);
                    if (bullet == null) return null;
                    bullet.AP += TryAddAp() ? GameData.ApPropAdd : 0;
                    facingDirection = new(angle, bullet.AttackDistance);
                    return bullet;
                }
                else
                    return null;
            }
        }

        /*
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
        }*/
        #endregion
        #region 感知相关的基本属性及方法
        private Dictionary<BgmType, double> bgmDictionary = new();
        public Dictionary<BgmType, double> BgmDictionary
        {
            get => bgmDictionary;
            private set
            {
                lock (gameObjLock)
                {
                    bgmDictionary = value;
                }
            }
        }
        public void AddBgm(BgmType bgm, double value)
        {
            if (BgmDictionary.ContainsKey(bgm))
                BgmDictionary[bgm] = value;
            else BgmDictionary.Add(bgm, value);
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

        private int viewRange;
        public int ViewRange
        {
            get => viewRange;
            set
            {
                lock (gameObjLock)
                {
                    viewRange = (value > 0) ? value : 0;
                }
            }
        }
        #endregion
        #region 交互相关的基本属性及方法
        private int speedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking
        {
            get => speedOfOpeningOrLocking;
            set
            {
                lock (gameObjLock)
                {
                    speedOfOpeningOrLocking = value;
                }
            }
        }

        private int speedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows
        {
            get => speedOfClimbingThroughWindows;
            set
            {
                lock (gameObjLock)
                {
                    speedOfClimbingThroughWindows = value;
                }
            }
        }

        private int speedOfOpenChest;
        public int SpeedOfOpenChest
        {
            get => speedOfOpenChest;
            set
            {
                lock (gameObjLock)
                {
                    speedOfOpenChest = value;
                }
            }
        }
        #endregion
        #region 血量相关的基本属性及方法
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

        /// <summary>
        /// 尝试减血
        /// </summary>
        /// <param name="sub">减血量</param>
        /// <returns>减操作是否成功</returns>
        public int TrySubHp(int sub)
        {
            int previousHp = hp;
            lock (gameObjLock)
                hp = hp <= sub ? 0 : hp - sub;
            Debugger.Output(this, " hp has subed to: " + hp.ToString());
            return previousHp - hp;
        }

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
        #endregion
        #region 状态相关的基本属性与方法
        private PlayerStateType playerState = PlayerStateType.Null;
        public PlayerStateType PlayerState
        {
            get
            {
                if (playerState == PlayerStateType.Null && IsMoving) return PlayerStateType.Moving;
                return playerState;
            }
        }

        public bool NoHp() => (playerState == PlayerStateType.Deceased || playerState == PlayerStateType.Escaped
                                                            || playerState == PlayerStateType.Addicted || playerState == PlayerStateType.Rescued);
        public bool Commandable() => (playerState != PlayerStateType.Deceased && playerState != PlayerStateType.Escaped
                                                            && playerState != PlayerStateType.Addicted && playerState != PlayerStateType.Rescued
                                                             && playerState != PlayerStateType.Swinging && playerState != PlayerStateType.TryingToAttack
                                                              && playerState != PlayerStateType.ClimbingThroughWindows && playerState != PlayerStateType.Stunned);
        public bool InteractingWithMapWithoutMoving() => (playerState == PlayerStateType.LockingOrOpeningTheDoor || playerState == PlayerStateType.Fixing || playerState == PlayerStateType.OpeningTheChest);
        public bool NullOrMoving() => (playerState == PlayerStateType.Null || playerState == PlayerStateType.Moving);
        public bool CanBeAwed() => !(playerState == PlayerStateType.Deceased || playerState == PlayerStateType.Escaped
                                                            || playerState == PlayerStateType.Addicted || playerState == PlayerStateType.Rescued
                                                            || playerState == PlayerStateType.Treated || playerState == PlayerStateType.Stunned
                                                            || playerState == PlayerStateType.Null || playerState == PlayerStateType.Moving);

        private GameObj? whatInteractingWith = null;
        public GameObj? WhatInteractingWith => whatInteractingWith;

        public void ChangePlayerState(PlayerStateType value = PlayerStateType.Null, GameObj? gameObj = null)
        {
            lock (actionLock)
            {
                ++stateNum;
                whatInteractingWith = gameObj;
                if (value != PlayerStateType.Moving)
                    IsMoving = false;
                playerState = (value == PlayerStateType.Moving) ? PlayerStateType.Null : value;
                //Debugger.Output(this,playerState.ToString()+" "+IsMoving.ToString());
            }
        }

        public void ChangePlayerStateInOneThread(PlayerStateType value = PlayerStateType.Null, GameObj? gameObj = null)
        {
            lock (actionLock)
            {
                whatInteractingWith = gameObj;
                if (value != PlayerStateType.Moving)
                    IsMoving = false;
                playerState = (value == PlayerStateType.Moving) ? PlayerStateType.Null : value;
                //Debugger.Output(this,playerState.ToString()+" "+IsMoving.ToString());
            }
        }

        public void SetPlayerStateNaturally()
        {
            lock (actionLock)
            {
                ++stateNum;
                whatInteractingWith = null;
                IsMoving = false;
                playerState = PlayerStateType.Null;
            }
        }

        public void RemoveFromGame(PlayerStateType playerStateType)
        {
            MoveReaderWriterLock.EnterWriteLock();
            try
            {
                lock (actionLock)
                {
                    playerState = playerStateType;
                    canMove = false;
                    isRemoved = true;
                    position = GameData.PosWhoDie;
                }
            }
            finally
            {
                MoveReaderWriterLock.ExitWriteLock();
            }
        }
        #endregion

        private int score = 0;
        public int Score
        {
            get => score;
        }

        /// <summary>
        /// 加分
        /// </summary>
        /// <param name="add">增加量</param>
        public virtual void AddScore(int add)
        {
            lock (gameObjLock)
            {
                score += add;
                //Debugger.Output(this, " 's score has been added to: " + score.ToString());
            }
        }

        /// <summary>
        /// 角色所属队伍ID
        /// </summary>
        private int teamID = int.MaxValue;
        public int TeamID
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
        private int playerID = int.MaxValue;
        public int PlayerID
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

        #region 道具和buff相关属性、方法
        private Prop[] propInventory = new Prop[GameData.maxNumOfPropInPropInventory]
                                                {new NullProp(), new NullProp(),new NullProp() };
        public Prop[] PropInventory
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
        public Prop UseProp(int indexing)
        {
            if (indexing < 0 || indexing >= GameData.maxNumOfPropInPropInventory)
                return new NullProp();
            lock (gameObjLock)
            {
                Prop prop = propInventory[indexing];
                PropInventory[indexing] = new NullProp();
                return prop;
            }
        }

        public Prop UseProp(PropType propType)
        {
            lock (gameObjLock)
            {
                if (propType == PropType.Null)
                {
                    for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                    {
                        if (PropInventory[indexing].GetPropType() != PropType.Null)
                        {
                            Prop prop = PropInventory[indexing];
                            PropInventory[indexing] = new NullProp();
                            return prop;
                        }
                    }
                }
                else
                    for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                    {
                        if (PropInventory[indexing].GetPropType() == propType)
                        {
                            Prop prop = PropInventory[indexing];
                            PropInventory[indexing] = new NullProp();
                            return prop;
                        }
                    }
                return new NullProp();
            }
        }

        /// <summary>
        /// 如果indexing==GameData.maxNumOfPropInPropInventory表明道具栏为满
        /// </summary>
        public int IndexingOfAddProp()
        {
            int indexing = 0;
            for (; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                if (PropInventory[indexing].GetPropType() == PropType.Null)
                    break;
            return indexing;
        }

        public void AddMoveSpeed(int buffTime, double add = 1.0) => buffManager.AddMoveSpeed(add, buffTime, newVal =>
                                                                                                            { MoveSpeed = newVal < GameData.characterMaxSpeed ? newVal : GameData.characterMaxSpeed; },
                                                                                             OrgMoveSpeed);
        public bool HasFasterSpeed => buffManager.HasFasterSpeed;

        public void AddShield(int shieldTime) => buffManager.AddShield(shieldTime);
        public bool HasShield => buffManager.HasShield;

        public void AddLife(int LIFETime) => buffManager.AddLife(LIFETime);
        public bool HasLIFE => buffManager.HasLIFE;

        public void AddAp(int time) => buffManager.AddAp(time);
        public bool HasAp => buffManager.HasAp;

        public void AddSpear(int spearTime) => buffManager.AddSpear(spearTime);
        public bool HasSpear => buffManager.HasSpear;

        public void AddClairaudience(int time) => buffManager.AddClairaudience(time);
        public bool HasClairaudience => buffManager.HasClairaudience;

        public void AddInvisible(int time) => buffManager.AddInvisible(time);
        public bool HasInvisible => buffManager.HasInvisible;

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
                case BuffType.AddLife:
                    return this.HasLIFE;
                case BuffType.AddAp:
                    return this.HasAp;
                case BuffType.Clairaudience:
                    return this.HasClairaudience;
                case BuffType.Invisible:
                    return this.HasInvisible;
                default:
                    return false;
            }
        }
        public void TryActivatingLIFE()
        {
            if (buffManager.TryActivatingLIFE())
            {
                AddScore(GameData.ScorePropRemainHp);
                hp = GameData.RemainHpWhenAddLife;
            }
        }

        public bool TryAddAp()
        {
            if (buffManager.TryAddAp())
            {
                AddScore(GameData.ScorePropAddAp);
                return true;
            }
            return false;
        }

        public bool TryUseSpear()
        {
            return buffManager.TryUseSpear();
        }

        public bool TryUseShield()
        {
            if (buffManager.TryUseShield())
            {
                AddScore(GameData.ScorePropUseShield);
                return true;
            }
            return false;
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

        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Circle;
        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (IsRemoved)
                return true;
            if (targetObj.Type == GameObjType.Prop)
            {
                return true;
            }
            if (targetObj.Type == GameObjType.Character && XY.DistanceCeil3(targetObj.Position, this.Position) < this.Radius + targetObj.Radius - GameData.adjustLength)
                return true;
            return false;
        }
    }
}
