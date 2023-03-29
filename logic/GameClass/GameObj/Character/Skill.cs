using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public class BecomeVampire : IActiveSkill  // 化身吸血鬼
    {
        public int SkillCD => GameData.commonSkillCD * 4 / 3;
        public int DurationTime => GameData.commonSkillTime;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }
    public class CanBeginToCharge : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 5;
        public int DurationTime => GameData.commonSkillTime * 6 / 10;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class BecomeInvisible : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime * 6 / 10;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class Punish : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => 0;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class JumpyBomb : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 30 * 5;
        public int DurationTime => GameData.commonSkillTime / 2;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class UseKnife : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD * 2 / 3;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class SuperFast : IActiveSkill  // 3倍速
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime * 4 / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class WriteAnswers : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => 0;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        private int degreeOfMeditation = 0;
        public int DegreeOfMeditation
        {
            get => degreeOfMeditation;
            set
            {
                lock (commonSkillLock)
                {
                    degreeOfMeditation = value;
                }
            }
        }

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = value;
        }
    }

    public class NullSkill : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool isBeingUsed = false;
        public bool IsBeingUsed
        {
            get => isBeingUsed; set => isBeingUsed = false;
        }
    }

    public static class SkillFactory
    {
        public static IActiveSkill FindIActiveSkill(ActiveSkillType activeSkillType)
        {
            switch (activeSkillType)
            {
                case ActiveSkillType.BecomeInvisible:
                    return new BecomeInvisible();
                case ActiveSkillType.UseKnife:
                    return new UseKnife();
                case ActiveSkillType.CanBeginToCharge:
                    return new CanBeginToCharge();
                case ActiveSkillType.Punish:
                    return new Punish();
                case ActiveSkillType.JumpyBomb:
                    return new JumpyBomb();
                case ActiveSkillType.WriteAnswers:
                    return new WriteAnswers();
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
                case UseKnife:
                    return ActiveSkillType.UseKnife;
                case CanBeginToCharge:
                    return ActiveSkillType.CanBeginToCharge;
                case Punish:
                    return ActiveSkillType.Punish;
                case JumpyBomb:
                    return ActiveSkillType.JumpyBomb;
                case WriteAnswers:
                    return ActiveSkillType.WriteAnswers;
                default:
                    return ActiveSkillType.Null;
            }
        }
    }
}