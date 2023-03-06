using Preparation.Utility;
using GameClass.GameObj;
using System.Collections.Generic;

namespace GameClass.Skill
{
    public interface ISkill
    {
    }
    public interface IPassiveSkill : ISkill
    {
        public void SkillEffect(Map map, Character player);
    }
    public interface IActiveSkill : ISkill
    {
        public int SkillCD { get; }
        public int DurationTime { get; } //技能持续时间
        public object ActiveSkillLock { get; }
        public bool SkillEffect(Map map, Character player);
    }
}