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

        public bool IsBeingUsed { get; set; }
    }
    public class CanBeginToCharge : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 5;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool IsBeingUsed { get; set; }

        public const int TimeOfGhostFainting = 7220;
        public const int TimeOfStudentFainting = 2090;
    }

    public class BecomeInvisible : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool IsBeingUsed { get; set; }
    }

    public class NuclearWeapon : IActiveSkill  // 核武器
    {
        public int SkillCD => GameData.commonSkillCD / 3 * 7;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool IsBeingUsed { get; set; }
    }

    public class UseKnife : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD / 3 * 2;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool IsBeingUsed { get; set; }
    }

    public class SuperFast : IActiveSkill  // 3倍速
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 4;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool IsBeingUsed { get; set; }
    }

    public static class SkillFactory
    {
        public static IActiveSkill? FindIActiveSkill(ActiveSkillType activeSkillType)
        {
            switch (activeSkillType)
            {
                case ActiveSkillType.BecomeInvisible:
                    return new BecomeInvisible();
                case ActiveSkillType.UseKnife:
                    return new UseKnife();
                default:
                    return null;
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
                default:
                    return ActiveSkillType.Null;
            }
        }
    }
}