using GameClass.Skill;
using Preparation.Utility;
using System.Collections.Generic;
using System;

namespace GameClass.GameObj
{
    public partial class Character
    {
        private readonly CharacterType characterType;
        public CharacterType CharacterType => characterType;
        private readonly IOccupation occupation;
        public IOccupation Occupation => occupation;

        private Dictionary<ActiveSkillType, int> timeUntilActiveSkillAvailable;
        public Dictionary<ActiveSkillType, int> TimeUntilActiveSkillAvailable => timeUntilActiveSkillAvailable;

        public bool SetTimeUntilActiveSkillAvailable(ActiveSkillType activeSkillType, int timeUntilActiveSkillAvailable)
        {
            if (TimeUntilActiveSkillAvailable.ContainsKey(activeSkillType))
            {
                lock (gameObjLock)
                    this.timeUntilActiveSkillAvailable[activeSkillType] = (timeUntilActiveSkillAvailable > 0) ? timeUntilActiveSkillAvailable : 0;
                return true;
            }
            return false;
        }
        public bool AddTimeUntilActiveSkillAvailable(ActiveSkillType activeSkillType, int addTimeUntilActiveSkillAvailable)
        {
            if (TimeUntilActiveSkillAvailable.ContainsKey(activeSkillType))
            {
                lock (gameObjLock)
                    this.timeUntilActiveSkillAvailable[activeSkillType] = (timeUntilActiveSkillAvailable[activeSkillType] + addTimeUntilActiveSkillAvailable > 0) ? timeUntilActiveSkillAvailable[activeSkillType] + addTimeUntilActiveSkillAvailable : 0;
                return true;
            }
            return false;
        }

        public bool UseActiveSkill(ActiveSkillType activeSkillType)
        {
            if (Occupation.ListOfIActiveSkill.Contains(ActiveSkillFactory.FindIActiveSkill(activeSkillType)))
                return ActiveSkillFactory.FindIActiveSkill(activeSkillType).SkillEffect(this);
            return false;
        }

        public void UsePassiveSkill(PassiveSkillType passiveSkillType)
        {
            if (Occupation.ListOfIPassiveSkill.Contains(PassiveSkillFactory.FindIPassiveSkill(passiveSkillType)))
                PassiveSkillFactory.FindIPassiveSkill(passiveSkillType).SkillEffect(this);
            return;
        }

        public bool IsGhost()
        {
            return this.CharacterType switch
            {
                CharacterType.Assassin => true,
                _ => false,
            };
        }

        protected Character(XY initPos, int initRadius, PlaceType initPlace, CharacterType characterType) :
            base(initPos, initRadius, initPlace, GameObjType.Character)
        {
            this.CanMove = true;
            this.score = 0;
            this.propInventory = null;
            this.buffManager = new BuffManager();
            switch (characterType)
            {
                case CharacterType.Assassin:
                    this.occupation = new Assassin();
                    break;
                case CharacterType.Athlete:
                    this.occupation = new Athlete();
                    break;
                default:
                    this.occupation = null;
                    break;
            }
            this.MaxHp = Occupation.MaxHp;
            this.hp = Occupation.MaxHp;
            this.OrgMoveSpeed = Occupation.MoveSpeed;
            this.moveSpeed = Occupation.MoveSpeed;
            this.cd = Occupation.CD;
            this.maxBulletNum = Occupation.MaxBulletNum;
            this.bulletNum = maxBulletNum;
            this.bulletOfPlayer = Occupation.InitBullet;
            this.OriBulletOfPlayer = Occupation.InitBullet;
            this.characterType = characterType;

            foreach (var activeSkill in this.Occupation.ListOfIActiveSkill)
            {
                this.TimeUntilActiveSkillAvailable.Add(ActiveSkillFactory.FindActiveSkillType(activeSkill), 0);
            }

            // UsePassiveSkill();  //创建player时开始被动技能，这一过程也可以放到gamestart时进行
            // 这可以放在AddPlayer中做

            Debugger.Output(this, "constructed!");
        }
    }
}
