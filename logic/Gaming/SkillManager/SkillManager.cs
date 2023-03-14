using System;
using Preparation.Utility;
using GameClass.GameObj;
using System.Reflection;
using GameEngine;

namespace Gaming
{
    public partial class Game
    {
        readonly SkillManager skillManager;
        private partial class SkillManager
        {
            public bool UseActiveSkill(Character character, ActiveSkillType activeSkillType)
            {
                if (character.Occupation.ListOfIActiveSkill.Contains(activeSkillType))
                    return FindIActiveSkill(activeSkillType).SkillEffect(character);
                return false;
            }
            public void UsePassiveSkill(Character character, PassiveSkillType passiveSkillType)
            {
                if (character.Occupation.ListOfIPassiveSkill.Contains(passiveSkillType))
                    switch (passiveSkillType)
                    {
                        default:
                            return;
                    }
                return;
            }
            public void UseAllPassiveSkill(Character character)
            {
                foreach (var passiveSkill in character.Occupation.ListOfIPassiveSkill)
                    switch (passiveSkill)
                    {
                        default:
                            return;
                    }
            }

            private readonly Map gameMap;
            private readonly ActionManager actionManager;
            private readonly AttackManager attackManager;
            private readonly PropManager propManager;
            public SkillManager(Map gameMap, ActionManager action, AttackManager attack, PropManager prop)
            {
                this.gameMap = gameMap;
                this.actionManager = action;
                this.propManager = prop;
                this.attackManager = attack;
            }
        }
    }
}
