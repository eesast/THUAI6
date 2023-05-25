using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;
using System.Threading;

namespace GameClass.GameObj
{
    public partial class Character : Moveable, ICharacter  // 负责人LHR摆烂终了
    {

        private readonly ReaderWriterLockSlim hpReaderWriterLock = new();
        public ReaderWriterLockSlim HPReadWriterLock => hpReaderWriterLock;
        private readonly object vampireLock = new();
        public object VampireLock => vampire;
        private readonly object inventoryLock = new();
        public object InventoryLock => inventoryLock;

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
                    Bullet? bullet = BulletFactory.GetBullet(this, res, this.bulletOfPlayer);
                    if (bullet == null) return null;
                    if (TryAddAp()) bullet.AddAP(GameData.ApPropAdd);
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
        private long maxHp;
        public long MaxHp
        {
            get
            {
                HPReadWriterLock.EnterReadLock();
                try
                {
                    return maxHp;
                }
                finally
                {
                    HPReadWriterLock.ExitReadLock();
                }
            }
            protected set
            {
                HPReadWriterLock.EnterWriteLock();
                try
                {
                    maxHp = value;
                    if (hp > maxHp) hp = maxHp;
                }
                finally
                {
                    HPReadWriterLock.ExitWriteLock();
                }
            }
        }  // 最大血量
        protected long hp;
        public long HP
        {
            get
            {
                HPReadWriterLock.EnterReadLock();
                try
                {
                    return hp;
                }
                finally
                {
                    HPReadWriterLock.ExitReadLock();
                }
            }
            set
            {
                HPReadWriterLock.EnterWriteLock();
                try
                {
                    if (value > 0)
                    {
                        hp = value <= maxHp ? value : maxHp;
                    }
                    else
                        hp = 0;
                }
                finally
                {
                    HPReadWriterLock.ExitWriteLock();
                }
            }
        }

        /// <summary>
        /// 尝试减血
        /// </summary>
        /// <param name="sub">减血量</param>
        public long TrySubHp(long sub)
        {
            HPReadWriterLock.EnterWriteLock();
            try
            {
                long previousHp = hp;
                if (hp <= sub)
                {
                    hp = 0;
                    return hp;
                }
                else
                {
                    hp -= sub;
                    return sub;
                }
            }
            finally
            {
                HPReadWriterLock.ExitWriteLock();
            }
        }

        private double vampire = 0;  // 回血率：0-1之间
        public double Vampire
        {
            get
            {
                lock (vampireLock)
                    return vampire;
            }
            set
            {
                lock (vampireLock)
                {
                    if (value > 1)
                        vampire = 1;
                    else if (value < 0)
                        vampire = 0;
                    else
                        vampire = value;
                }
            }
        }
        public double OriVampire { get; protected set; }
        #endregion
        #region 状态相关的基本属性与方法
        private PlayerStateType playerState = PlayerStateType.Null;
        public PlayerStateType PlayerState
        {
            get
            {
                lock (actionLock)
                {
                    if (playerState == PlayerStateType.Moving)
                        return (IsMoving) ? PlayerStateType.Moving : PlayerStateType.Null;
                    return playerState;
                }
            }
        }

        public bool NoHp()
        {
            lock (actionLock)
                return (playerState == PlayerStateType.Deceased || playerState == PlayerStateType.Escaped || playerState == PlayerStateType.Addicted || playerState == PlayerStateType.Rescued);
        }
        public bool Commandable()
        {
            lock (actionLock)
            {
                return (playerState != PlayerStateType.Deceased && playerState != PlayerStateType.Escaped
                           && playerState != PlayerStateType.Addicted && playerState != PlayerStateType.Rescued
                           && playerState != PlayerStateType.Swinging && playerState != PlayerStateType.TryingToAttack
                           && playerState != PlayerStateType.ClimbingThroughWindows
                           && playerState != PlayerStateType.Stunned && playerState != PlayerStateType.Charmed);
            }
        }
        public bool CanPinDown()
        {
            lock (actionLock)
            {
                return (playerState != PlayerStateType.Deceased && playerState != PlayerStateType.Escaped
                           && playerState != PlayerStateType.Addicted && playerState != PlayerStateType.Rescued
                           && playerState != PlayerStateType.Stunned && playerState != PlayerStateType.Charmed);
            }
        }
        public bool InteractingWithMapWithoutMoving()
        {
            lock (actionLock)
            {
                return (playerState == PlayerStateType.LockingTheDoor || playerState == PlayerStateType.OpeningTheDoor
                            || playerState == PlayerStateType.Fixing || playerState == PlayerStateType.OpeningTheChest);
            }
        }
        public bool NullOrMoving()
        {
            lock (actionLock)
            {
                return (playerState == PlayerStateType.Null || playerState == PlayerStateType.Moving);
            }
        }
        public bool CanBeAwed()
        {
            lock (actionLock)
                return !(playerState == PlayerStateType.Deceased || playerState == PlayerStateType.Escaped
                           || playerState == PlayerStateType.Addicted
                           || playerState == PlayerStateType.Rescued || playerState == PlayerStateType.Treated
                           || playerState == PlayerStateType.Stunned || playerState == PlayerStateType.Charmed
                           || playerState == PlayerStateType.Null || playerState == PlayerStateType.Moving);
        }
        private GameObj? whatInteractingWith = null;
        public GameObj? WhatInteractingWith
        {
            get
            {
                lock (actionLock)
                {
                    return whatInteractingWith;
                }
            }
        }

        public bool StartThread(long stateNum, RunningStateType runningState)
        {
            lock (ActionLock)
            {
                if (this.StateNum == stateNum)
                {
                    this.runningState = runningState;
                    return true;
                }
            }
            return false;
        }

        private long ChangePlayerState(RunningStateType running, PlayerStateType value = PlayerStateType.Null, GameObj? gameObj = null)
        {
            //只能被SetPlayerState引用
            if (runningState == RunningStateType.RunningSleepily)
            {
                ThreadNum.Release();
            }
            runningState = running;
            whatInteractingWith = gameObj;
            playerState = value;
            return ++stateNum;
        }

        private long ChangePlayerStateInOneThread(RunningStateType running, PlayerStateType value = PlayerStateType.Null, GameObj? gameObj = null)
        {
            if (runningState == RunningStateType.RunningSleepily)
            {
                ThreadNum.Release();
            }
            runningState = running;
            //只能被SetPlayerState引用
            whatInteractingWith = gameObj;
            playerState = value;
            return stateNum;
        }

        public long SetPlayerState(RunningStateType runningState, PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null)
        {
            GameObj? gameObj = (GameObj?)obj;
            lock (actionLock)
            {
                PlayerStateType nowPlayerState = PlayerState;
                if (nowPlayerState == value && value != PlayerStateType.UsingSkill) return -1;
                GameObj? lastObj = whatInteractingWith;
                switch (nowPlayerState)
                {
                    case PlayerStateType.Escaped:
                    case PlayerStateType.Deceased:
                        return -1;

                    case PlayerStateType.Addicted:
                        if (value == PlayerStateType.Rescued)
                            return ChangePlayerStateInOneThread(runningState, value, gameObj);
                        else if (value == PlayerStateType.Null || value == PlayerStateType.Deceased)
                            return ChangePlayerState(runningState, value, gameObj);
                        else return -1;
                    case PlayerStateType.Rescued:
                        if (value == PlayerStateType.Addicted)
                            return ChangePlayerStateInOneThread(runningState, value, gameObj);
                        else if (value == PlayerStateType.Null || value == PlayerStateType.Deceased)
                            return ChangePlayerState(runningState, value, gameObj);
                        else return -1;

                    case PlayerStateType.TryingToAttack:
                        if (value == PlayerStateType.Addicted || value == PlayerStateType.Swinging
                            || value == PlayerStateType.Deceased || value == PlayerStateType.Stunned
                            || value == PlayerStateType.Charmed || value == PlayerStateType.Null)
                            return ChangePlayerState(runningState, value, gameObj);
                        else return -1;
                    case PlayerStateType.Stunned:
                    case PlayerStateType.Charmed:
                        if (value == PlayerStateType.Addicted || value == PlayerStateType.Deceased
                            || value == PlayerStateType.Null)
                            return ChangePlayerState(runningState, value, gameObj);
                        else return -1;
                    case PlayerStateType.Swinging:
                        if (value == PlayerStateType.Addicted
                            || value == PlayerStateType.Deceased || value == PlayerStateType.Stunned
                            || value == PlayerStateType.Charmed || value == PlayerStateType.Null)
                            return ChangePlayerState(runningState, value, gameObj);
                        else return -1;
                    case PlayerStateType.ClimbingThroughWindows:
                        if (value == PlayerStateType.Addicted
                             || value == PlayerStateType.Deceased || value == PlayerStateType.Stunned
                             || value == PlayerStateType.Charmed || value == PlayerStateType.Null)
                        {
                            Window window = (Window)lastObj!;
                            if (window.Stage.x != 0) ReSetPos(window.Stage);
                            window.FinishClimbing();
                            return ChangePlayerState(runningState, value, gameObj);
                        }
                        else return -1;

                    case PlayerStateType.OpeningTheChest:
                        if (value == PlayerStateType.Rescued) return -1;
                        ((Chest)lastObj!).StopOpen();
                        return ChangePlayerState(runningState, value, gameObj);
                    case PlayerStateType.OpeningTheDoorway:
                        if (value == PlayerStateType.Rescued) return -1;
                        Doorway doorway = (Doorway)lastObj!;
                        doorway.StopOpenning();
                        return ChangePlayerState(runningState, value, gameObj);
                    case PlayerStateType.OpeningTheDoor:
                        if (value == PlayerStateType.Rescued) return -1;
                        Door door = (Door)lastObj!;
                        door.StopOpen();
                        ReleaseTool(door.DoorNum switch
                        {
                            3 => PropType.Key3,
                            5 => PropType.Key5,
                            _ => PropType.Key6,
                        }
                        );
                        return ChangePlayerState(runningState, value, gameObj);
                    case PlayerStateType.UsingSkill:
                        {
                            if (value == PlayerStateType.Rescued) return -1;
                            switch (CharacterType)
                            {
                                case CharacterType.TechOtaku:
                                    {
                                        if (typeof(CraftingBench).IsInstanceOfType(lastObj))
                                        {
                                            ((CraftingBench)lastObj!).StopSkill();
                                            return ChangePlayerState(runningState, value, gameObj);
                                        }
                                        else
                                        {
                                            if (value != PlayerStateType.UsingSkill)
                                                ((UseRobot)FindActiveSkill(ActiveSkillType.UseRobot)).NowPlayerID = (int)playerID;
                                            return ChangePlayerState(runningState, value, gameObj);
                                        }
                                    }
                                case CharacterType.Assassin:
                                    if (value == PlayerStateType.Moving) return StateNum;
                                    else
                                    {
                                        TryDeleteInvisible();
                                        return ChangePlayerState(runningState, value, gameObj);
                                    }
                                default:
                                    return ChangePlayerState(runningState, value, gameObj);
                            }
                        }
                    default:
                        if (value == PlayerStateType.Rescued) return -1;
                        return ChangePlayerState(runningState, value, gameObj);
                }
            }
        }

        public long SetPlayerStateNaturally()
        {
            lock (actionLock)
            {
                runningState = RunningStateType.Null;
                whatInteractingWith = null;
                playerState = PlayerStateType.Null;
                return ++stateNum;
            }
        }

        public bool ResetPlayerState(long state, RunningStateType running = RunningStateType.Null, PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null)
        {
            lock (actionLock)
            {
                if (state != stateNum) return false;
                this.runningState = running;
                whatInteractingWith = (GameObj?)obj;
                playerState = value;
                ++stateNum;
                return true;
            }
        }

        public bool TryToRemoveFromGame(PlayerStateType playerStateType)
        {
            lock (actionLock)
            {
                if (SetPlayerState(RunningStateType.RunningForcibly, playerStateType) == -1) return false;
                TryToRemove();
                ReSetCanMove(false);
                position = GameData.PosWhoDie;
            }
            return true;
        }
        #endregion

        private long score = 0;
        public long Score
        {
            get => Interlocked.Read(ref score);
        }

        /// <summary>
        /// 加分
        /// </summary>
        /// <param name="add">增加量</param>
        public virtual void AddScore(long add)
        {
            Interlocked.Add(ref score, add);
            //Debugger.Output(this, " 's score has been added to: " + score.ToString());
        }

        /// <summary>
        /// 角色所属队伍ID
        /// </summary>
        private long teamID = long.MaxValue;
        public long TeamID
        {
            get => Interlocked.Read(ref teamID);
            set => Interlocked.Exchange(ref teamID, value);
        }
        private long playerID = long.MaxValue;
        public long PlayerID
        {
            get => Interlocked.Read(ref playerID);
            set => Interlocked.Exchange(ref playerID, value);
        }

        #region 道具和buff相关属性、方法
        private Gadget[] propInventory = new Gadget[GameData.maxNumOfPropInPropInventory]
                                                {new NullProp(), new NullProp(),new NullProp() };
        public Gadget[] PropInventory
        {
            get
            {
                lock (inventoryLock)
                    return propInventory;
            }
            set
            {
                lock (inventoryLock)
                    propInventory = value;
            }
        }

        /// <summary>
        /// 使用物品栏中的道具
        /// </summary>
        /// <returns>被使用的道具</returns>
        public Gadget UseProp(int indexing)
        {
            if (indexing < 0 || indexing >= GameData.maxNumOfPropInPropInventory)
                return new NullProp();
            lock (inventoryLock)
            {
                Gadget prop = propInventory[indexing];
                if (!prop.IsUsable()) return new NullProp();
                PropInventory[indexing] = new NullProp();
                return prop;
            }
        }

        public Gadget UseProp(PropType propType)
        {
            if (propType == PropType.Null)
            {
                lock (inventoryLock)
                {
                    for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                    {
                        if (PropInventory[indexing].IsUsable())
                        {
                            Gadget prop = PropInventory[indexing];
                            PropInventory[indexing] = new NullProp();
                            return prop;
                        }
                    }
                }
            }
            else
            {
                lock (inventoryLock)
                {
                    for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                    {
                        if (PropInventory[indexing].GetPropType() == propType && PropInventory[indexing].IsUsable())
                        {
                            Gadget prop = PropInventory[indexing];
                            PropInventory[indexing] = new NullProp();
                            return prop;
                        }
                    }
                }
            }
            return new NullProp();
        }

        public bool UseTool(PropType propType)
        {
            lock (inventoryLock)
            {
                for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                {
                    if (PropInventory[indexing].GetPropType() == propType && PropInventory[indexing].IsUsable())
                    {
                        return ((Tool)PropInventory[indexing]).IsUsed = true;
                    }
                }
            }
            return false;
        }

        public void ReleaseTool(PropType propType)
        {
            lock (inventoryLock)
            {
                for (int indexing = 0; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                {
                    if (PropInventory[indexing].GetPropType() == propType && ((Tool)PropInventory[indexing]).IsUsed)
                    {
                        ((Tool)PropInventory[indexing]).IsUsed = false;
                        break;
                    }
                }
            }
        }

        /// <summary>
        /// 如果indexing==GameData.maxNumOfPropInPropInventory表明道具栏为满
        /// </summary>
        public int IndexingOfAddProp()
        {
            int indexing = 0;
            lock (inventoryLock)
                for (; indexing < GameData.maxNumOfPropInPropInventory; ++indexing)
                    if (propInventory[indexing].GetPropType() == PropType.Null)
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

        public bool TryDeleteInvisible()
        {
            return buffManager.TryDeleteInvisible();
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
            if (targetObj.Type == GameObjType.Gadget)
            {
                return true;
            }
            if (targetObj.Type == GameObjType.Character && XY.DistanceCeil3(targetObj.Position, this.Position) < this.Radius + targetObj.Radius - GameData.adjustLength)
                return true;
            return false;
        }
    }
}
