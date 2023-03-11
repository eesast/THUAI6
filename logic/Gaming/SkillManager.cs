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
            public bool UseActiveSkill(Map gamemap, Character character, ActiveSkillType activeSkillType)
            {
                return character.UseActiveSkill(gamemap, activeSkillType);
            }
            public void UsePassiveSkill(Map gamemap, Character character, PassiveSkillType passiveSkillType)
            {
                character.UsePassiveSkill(gamemap, passiveSkillType);
            }
            public void UseAllPassiveSkill(Map gamemap, Character character)
            {
                foreach (var passiveSkill in character.Occupation.ListOfIPassiveSkill)
                    character.UsePassiveSkill(gamemap, passiveSkill);
            }
        }
    }
}
