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
            public bool UseActiveSkill(Character character, ActiveSkillType activeSkillType, int parameter)
            {
                if (character.Occupation.ListOfIActiveSkill.Contains(activeSkillType))
                    switch (activeSkillType)
                    {
                        case ActiveSkillType.BecomeInvisible:
                            return BecomeInvisible(character);
                        case ActiveSkillType.UseKnife:
                            return UseKnife(character);
                        case ActiveSkillType.Howl:
                            return Howl(character);
                        case ActiveSkillType.CanBeginToCharge:
                            return CanBeginToCharge(character);
                        case ActiveSkillType.Inspire:
                            return Inspire(character);
                        case ActiveSkillType.Encourage:
                            return Encourage(character);
                        case ActiveSkillType.Punish:
                            return Punish(character);
                        case ActiveSkillType.JumpyBomb:
                            return JumpyBomb(character);
                        case ActiveSkillType.WriteAnswers:
                            return WriteAnswers(character);
                        case ActiveSkillType.SummonGolem:
                            return SummonGolem(character);
                        case ActiveSkillType.UseRobot:
                            return UseRobot(character, parameter);
                        case ActiveSkillType.Rouse:
                            return Rouse(character);
                        case ActiveSkillType.ShowTime:
                            return ShowTime(character);
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
            private readonly CharacterManager characterManager;
            private readonly ActionManager actionManager;
            private readonly AttackManager attackManager;
            private readonly PropManager propManager;
            public SkillManager(Map gameMap, ActionManager action, AttackManager attack, PropManager prop, CharacterManager characterManager)
            {
                this.gameMap = gameMap;
                this.actionManager = action;
                this.propManager = prop;
                this.attackManager = attack;
                this.characterManager = characterManager;
            }
        }
    }
}
