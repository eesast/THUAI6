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
                    switch (activeSkillType)
                    {
                        case ActiveSkillType.BecomeInvisible:
                            BecomeInvisible(character);
                            break;
                        case ActiveSkillType.UseKnife:
                            UseKnife(character);
                            break;
                        case ActiveSkillType.Howl:
                            Howl(character);
                            break;
                        case ActiveSkillType.CanBeginToCharge:
                            CanBeginToCharge(character);
                            break;
                        case ActiveSkillType.Punish:
                            Punish(character);
                            break;
                        case ActiveSkillType.JumpyBomb:
                            JumpyBomb(character);
                            break;
                        case ActiveSkillType.WriteAnswers:
                            WriteAnswers(character);
                            break;
                        default:
                            return false;
                    }
                return false;
            }
            public void UsePassiveSkill(Character character, PassiveSkillType passiveSkillType)
            {
                if (character.Occupation.ListOfIPassiveSkill.Contains(passiveSkillType))
                    switch (passiveSkillType)
                    {
                        case PassiveSkillType.Meditate:
                            Meditate(character);
                            break;
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
                        case PassiveSkillType.Meditate:
                            Meditate(character);
                            break;
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
