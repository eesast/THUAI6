using GameClass.GameObj;
using Preparation.Utility;
using System.Collections.Generic;

namespace GameClass.Skill
{
    public interface ISkill
    {
    }
    public interface IPassiveSkill: ISkill
    {
        public void SkillEffect(Character player);
    }
    public interface IActiveSkill : ISkill
    {
        public int SkillCD { get; }
        public int DurationTime { get; } //技能持续时间
        public object ActiveSkillLock { get; }
        public bool SkillEffect(Character player);
    }
    public interface ICharacterType
    {
        public int MoveSpeed { get; }
        public int MaxHp { get; }
        public BulletType InitBullet { get; }
        public List<IActiveSkill> ListOfIActiveSkill { get; }
        public List<IPassiveSkill> ListOfIPassiveSkill { get; }
    }
}
