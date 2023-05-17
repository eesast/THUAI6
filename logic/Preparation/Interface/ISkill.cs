using Preparation.Interface;
using Preparation.Utility;
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
        public int IsBeingUsed { get; set; }
    }

    public abstract class ActiveSkill : IActiveSkill
    {
        public abstract int SkillCD { get; }
        public abstract int DurationTime { get; }

        private readonly object activeSkillUseLock = new();
        public object ActiveSkillUseLock => activeSkillUseLock;

        private readonly object skillLock = new();
        public object SkillLock => skillLock;

        private int timeUntilActiveSkillAvailable = 0;
        public int TimeUntilActiveSkillAvailable
        {
            get => Interlocked.CompareExchange(ref timeUntilActiveSkillAvailable, 0, 1);
            set
            {
                if (value < 0) value = 0;
                else if (value > SkillCD) value = SkillCD;
                Interlocked.Exchange(ref timeUntilActiveSkillAvailable, value);
            }
        }

        public int isBeingUsed = 0;//实为bool
        public int IsBeingUsed
        {
            get => Interlocked.CompareExchange(ref isBeingUsed, 0, 1);
            set => Interlocked.Exchange(ref isBeingUsed, value);
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
        public override int SkillCD => GameData.commonSkillCD;
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
            get => Interlocked.CompareExchange(ref nowPlayerID, 0, 1);
            set => Interlocked.Exchange(ref nowPlayerID, value);
        }
    }

    public class WriteAnswers : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD;
        public override int DurationTime => 0;

        private int degreeOfMeditation = 0;
        public int DegreeOfMeditation
        {
            get => Interlocked.CompareExchange(ref degreeOfMeditation, 0, 1);
            set => Interlocked.Exchange(ref degreeOfMeditation, value);
        }
    }

    public class SummonGolem : ActiveSkill
    {
        public override int SkillCD => GameData.commonSkillCD * 4 / 3;
        public override int DurationTime => 6;

        private bool[] golemIDArray = new bool[GameData.maxSummonedGolemNum] { false, false, false };
        public bool[] GolemIDArray
        {
            get
            {
                lock (SkillLock) { return golemIDArray; }
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
        public int AddGolem()
        {
            lock (SkillLock)
            {
                if (nowPtr == GameData.maxSummonedGolemNum) return GameData.maxSummonedGolemNum;
                int num = nowPtr;
                golemIDArray[nowPtr] = true;
                while ((++nowPtr) < GameData.maxSummonedGolemNum && golemIDArray[nowPtr]) ;
                return num;
            }
        }
        public void DeleteGolem(int num)
        {
            lock (SkillLock)
            {
                golemIDArray[num] = false;
                if (num < nowPtr)
                {
                    nowPtr = num;
                }
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
                case ActiveSkillType.JumpyBomb:
                    return new JumpyBomb();
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
                case JumpyBomb:
                    return ActiveSkillType.JumpyBomb;
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