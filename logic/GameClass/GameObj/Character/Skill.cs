using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public class BecomeVampire : IActiveSkill  // 化身吸血鬼
    {
        public int SkillCD => GameData.commonSkillCD / 3 * 4;
        public int DurationTime => GameData.commonSkillTime;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }
    public class CanBeginToCharge : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 5;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class BecomeInvisible : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class Punish : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class NuclearWeapon : IActiveSkill  // 核武器
    {
        public int SkillCD => GameData.commonSkillCD / 3 * 7;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class UseKnife : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 3 * 2;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class SuperFast : IActiveSkill  // 3倍速
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 4;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
    }

    public class NullSkill : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
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
                default:
                    return ActiveSkillType.Null;
            }
        }
    }
}