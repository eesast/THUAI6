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
        public object ActiveSkillLock { get; }
        public bool IsBeingUsed { get; set; }
    }
}