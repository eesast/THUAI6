using System;
using GameClass.GameObj;

namespace Gaming
{
    public partial class Game
    {
        readonly SkillManager skillManager;
        private class SkillManager
        {
            public bool UseCommonSkill(Character character)
            {
                return character.UseCommonSkill();
            }
            public void UsePassiveSkill(Character character)
            {
                character.UsePassiveSkill();
            }
        }
    }
}
