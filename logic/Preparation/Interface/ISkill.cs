using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Threading;

namespace Preparation.Interface
{
    public interface ISkill
    {
    }
    public interface IPassiveSkill : ISkill
    {
    }
    public interface IActiveSkill : ISkill
    {
        public int SkillCD { get; }
        public int DurationTime { get; } //技能持续时间
        public object ActiveSkillUseLock { get; }
        public bool IsBeingUsed { get; set; }
    }

    public abstract class ActiveSkill : IActiveSkill
    {
        public abstract int SkillCD { get; }
        public abstract int DurationTime { get; }

        private readonly object activeSkillUseLock = new();
        public object ActiveSkillUseLock => activeSkillUseLock;

        private readonly object skillLock = new();
        public object SkillLock => skillLock;

        private long startTime = Environment.TickCount64 - 600000;
        public long StartTime
        {
            get
            {
                lock (skillLock)
                    return startTime;
            }
        }
        public bool StartSkill()
        {
            lock (skillLock)
            {
                if (Environment.TickCount64 - startTime >= SkillCD)
                {
                    startTime = Environment.TickCount64;
                    return true;
                }
            }
            return false;
        }

        public int isBeingUsed = 0;//实为bool
        public bool IsBeingUsed
        {
            get => (Interlocked.CompareExchange(ref isBeingUsed, -1, -1) == 1);
            set => Interlocked.Exchange(ref isBeingUsed, value ? 1 : 0);
        }
    }

    public class CanBeginToCharge : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 2;
        public override int DurationTime => GameData.commonSkillTime * 3 / 10;
    }

    public class BecomeInvisible : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4 / 3;
        public override int DurationTime => GameData.commonSkillTime;
    }

    public class Punish : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 45 / 30;
        public override int DurationTime => 0;
    }

    public class HaveTea : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 3;
        public override int DurationTime => 0;
    }

    public class Rouse : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4;
        public override int DurationTime => 0;
    }

    public class Encourage : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4;
        public override int DurationTime => 0;
    }

    public class Inspire : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4;
        public override int DurationTime => 0;
    }

    public class Howl : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 25 / 30;
        public override int DurationTime => 0;
    }

    public class ShowTime : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 8 / 3;
        public override int DurationTime => GameData.commonSkillTime;
    }

    public class JumpyBomb : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD / 2;
        public override int DurationTime => GameData.commonSkillTime * 3 / 10;
    }

    public class SparksNSplash : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 45 / 30;
        public override int DurationTime => GameData.commonSkillTime;
    }

    public class UseKnife : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD;
        public override int DurationTime => GameData.commonSkillTime / 10;
    }

    public class UseRobot : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD / 300;
        public override int DurationTime => 0;

        private int nowPlayerID;
        public int NowPlayerID
        {
            get
            {
                lock (SkillLock)
                {
                    return nowPlayerID;
                }
            }
            set
            {
                lock (SkillLock)
                {
                    nowPlayerID = value;
                }
            }
        }
        public bool TryResetNowPlayerID(int tryPlayerID)
        {
            lock (SkillLock)
            {
                if (nowPlayerID == tryPlayerID)
                {
                    nowPlayerID %= GameData.numOfPeople;
                    return true;
                }
                else return false;
            }
        }
    }

    public class WriteAnswers : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD;
        public override int DurationTime => 0;

        public AtomicInt DegreeOfMeditation { get; } = new(0);
    }

    public class SummonGolem : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4 / 3;
        public override int DurationTime => 6000;

        private int[] golemStateArray = new int[GameData.maxSummonedGolemNum] { 0, 0, 0 };
        //0未建造，1建造中，2已建造
        public int[] GolemStateArray
        {
            get
            {
                lock (SkillLock) { return golemStateArray; }
            }
        }
        private int nowPtr = 0;
        public int NowPtr
        {
            get
            {
                lock (SkillLock) { return nowPtr; }
            }
        }
        public int BuildGolem()
        {
            lock (SkillLock)
            {
                if (nowPtr == GameData.maxSummonedGolemNum) return GameData.maxSummonedGolemNum;
                int num = nowPtr;
                golemStateArray[nowPtr] = 1;
                while ((++nowPtr) < GameData.maxSummonedGolemNum && golemStateArray[nowPtr] != 0) ;
                return num;
            }
        }
        public void DeleteGolem(int num)
        {
            lock (SkillLock)
            {
                golemStateArray[num] = 0;
                if (num < nowPtr)
                {
                    nowPtr = num;
                }
            }
        }
        public void AddGolem(int num)
        {
            lock (SkillLock)
            {
                golemStateArray[num] = 2;
            }
        }
    }

    public class NullSkill : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD;
        public override int DurationTime => GameData.commonSkillTime;
    }

    public static class SkillFactory
    {
        public static ActiveSkill FindActiveSkill(ActiveSkillType activeSkillType)
        {
            switch (activeSkillType)
            {
                case ActiveSkillType.BecomeInvisible:
                    return new BecomeInvisible();
                case ActiveSkillType.UseKnife:
                    return new UseKnife();
                case ActiveSkillType.Howl:
                    return new Howl();
                case ActiveSkillType.Encourage:
                    return new Encourage();
                case ActiveSkillType.CanBeginToCharge:
                    return new CanBeginToCharge();
                case ActiveSkillType.Punish:
                    return new Punish();
                case ActiveSkillType.HaveTea:
                    return new HaveTea();
                case ActiveSkillType.JumpyBomb:
                    return new JumpyBomb();
                case ActiveSkillType.SparksNSplash:
                    return new SparksNSplash();
                case ActiveSkillType.WriteAnswers:
                    return new WriteAnswers();
                case ActiveSkillType.SummonGolem:
                    return new SummonGolem();
                case ActiveSkillType.UseRobot:
                    return new UseRobot();
                case ActiveSkillType.Rouse:
                    return new Rouse();
                case ActiveSkillType.Inspire:
                    return new Inspire();
                case ActiveSkillType.ShowTime:
                    return new ShowTime();
                default:
                    return new NullSkill();
            }
        }

        public static ActiveSkillType FindActiveSkillType(IActiveSkill ActiveSkill)
        {
            switch (ActiveSkill)
            {
                case BecomeInvisible:
                    return ActiveSkillType.BecomeInvisible;
                case Howl:
                    return ActiveSkillType.Howl;
                case UseKnife:
                    return ActiveSkillType.UseKnife;
                case Encourage:
                    return ActiveSkillType.Encourage;
                case CanBeginToCharge:
                    return ActiveSkillType.CanBeginToCharge;
                case Inspire:
                    return ActiveSkillType.Inspire;
                case Punish:
                    return ActiveSkillType.Punish;
                case HaveTea:
                    return ActiveSkillType.HaveTea;
                case JumpyBomb:
                    return ActiveSkillType.JumpyBomb;
                case SparksNSplash:
                    return ActiveSkillType.SparksNSplash;
                case WriteAnswers:
                    return ActiveSkillType.WriteAnswers;
                case SummonGolem:
                    return ActiveSkillType.SummonGolem;
                case UseRobot:
                    return ActiveSkillType.UseRobot;
                case Rouse:
                    return ActiveSkillType.Rouse;
                case ShowTime:
                    return ActiveSkillType.ShowTime;
                default:
                    return ActiveSkillType.Null;
            }
        }
    }
}