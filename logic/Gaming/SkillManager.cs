using System;
using Preparation.Utility;
using GameClass.GameObj;
using System.Reflection;
using GameClass.Skill;

namespace Gaming
{
    public partial class Game
    {
        readonly SkillManager skillManager;
        private class SkillManager
        {
            public bool UseActiveSkill(Character character, ActiveSkillType activeSkillType)
            {
                return character.UseActiveSkill(activeSkillType);
            }
            public void UsePassiveSkill(Character character, PassiveSkillType passiveSkillType)
            {
                character.UsePassiveSkill(passiveSkillType);
            }
            public void UseAllPassiveSkill(Character character)
            {
                foreach (var passiveSkill in character.Occupation.ListOfIPassiveSkill)
                    character.UsePassiveSkill(PassiveSkillFactory.FindpassiveSkillType(passiveSkill));
            }
        }
    }
}
