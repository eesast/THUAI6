using Preparation.Utility;
using System.Collections.Generic;
using System;

namespace GameClass.GameObj
{
    public partial class Character
    {
        private delegate bool CharacterActiveSkill(Character player);  // 返回值：是否成功释放了技能
        private delegate void CharacterPassiveSkill(Character player);
        private readonly CharacterActiveSkill commonSkill;
        private readonly ActiveSkillType commonSkillType;
        public ActiveSkillType CommonSkillType => commonSkillType;

        private readonly CharacterType passiveSkillType;
        public CharacterType PassiveSkillType => passiveSkillType;
        public bool UseCommonSkill()
        {
            return commonSkill(this);
        }
        private int timeUntilCommonSkillAvailable = 0;  // 还剩多少时间可以使用普通技能
        public int TimeUntilCommonSkillAvailable
        {
            get => timeUntilCommonSkillAvailable;
            set {
                lock (gameObjLock)
                    timeUntilCommonSkillAvailable = value < 0 ? 0 : value;
            }
        }

        readonly CharacterPassiveSkill passiveSkill;
        public void UsePassiveSkill()
        {
            passiveSkill(this);
            return;
        }
        public Character(XY initPos, int initRadius, PlaceType initPlace, CharacterType passiveSkillType, ActiveSkillType commonSkillType) :
            base(initPos, initRadius, initPlace, GameObjType.Character)
        {
            this.CanMove = true;
            this.score = 0;
            this.propInventory = null;
            this.buffManeger = new BuffManeger();

            // UsePassiveSkill();  //创建player时开始被动技能，这一过程也可以放到gamestart时进行
            // 这可以放在AddPlayer中做

            Debugger.Output(this, "constructed!");
        }
    }
}
