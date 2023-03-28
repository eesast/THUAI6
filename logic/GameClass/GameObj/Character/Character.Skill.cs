using Preparation.Utility;
using Preparation.Interface;
using System.Collections.Generic;
using System;
using System.Numerics;

namespace GameClass.GameObj
{
    public partial class Character
    {
        private readonly CharacterType characterType;
        public CharacterType CharacterType => characterType;
        private readonly IOccupation occupation;
        public IOccupation Occupation => occupation;

        private Dictionary<ActiveSkillType, int> timeUntilActiveSkillAvailable = new();
        public Dictionary<ActiveSkillType, int> TimeUntilActiveSkillAvailable => timeUntilActiveSkillAvailable;

        private Dictionary<ActiveSkillType, IActiveSkill> iActiveSkillDictionary = new();
        public Dictionary<ActiveSkillType, IActiveSkill> IActiveSkillDictionary => iActiveSkillDictionary;

        public IActiveSkill UseIActiveSkill(ActiveSkillType activeSkillType)
        {
            if (Occupation.ListOfIActiveSkill.Contains(activeSkillType))
            {
                return IActiveSkillDictionary[activeSkillType];
            }
            return new NullSkill();
        }

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

        public bool IsGhost()
        {
            return GameData.IsGhost(CharacterType);
        }

        protected Character(XY initPos, int initRadius, CharacterType characterType) :
            base(initPos, initRadius, GameObjType.Character)
        {
            this.place = PlaceType.Null;
            this.CanMove = true;
            this.score = 0;
            this.buffManager = new BuffManager();
            switch (characterType)
            {
                case CharacterType.Assassin:
                    this.occupation = new Assassin();
                    break;
                case CharacterType.Athlete:
                default:
                    this.occupation = new Athlete();
                    break;
            }
            this.MaxHp = this.hp = Occupation.MaxHp;
            this.OrgMoveSpeed = this.moveSpeed = Occupation.MoveSpeed;
            this.OrgCD = this.cd = BulletFactory.BulletCD(Occupation.InitBullet);
            this.bulletNum = this.maxBulletNum = Occupation.MaxBulletNum;
            this.bulletOfPlayer = this.OriBulletOfPlayer = Occupation.InitBullet;
            this.concealment = Occupation.Concealment;
            this.alertnessRadius = Occupation.AlertnessRadius;
            this.ViewRange = Occupation.ViewRange;
            this.characterType = characterType;
            this.SpeedOfOpeningOrLocking = Occupation.TimeOfOpeningOrLocking;
            this.SpeedOfClimbingThroughWindows = Occupation.SpeedOfClimbingThroughWindows;
            this.SpeedOfOpenChest = Occupation.TimeOfOpenChest;

            foreach (var activeSkill in this.Occupation.ListOfIActiveSkill)
            {
                this.TimeUntilActiveSkillAvailable.Add(activeSkill, 0);
                this.IActiveSkillDictionary.Add(activeSkill, SkillFactory.FindIActiveSkill(activeSkill));
            }

            // UsePassiveSkill();  //创建player时开始被动技能，这一过程也可以放到gamestart时进行
            // 这可以放在AddPlayer中做

            Debugger.Output(this, "constructed!");
        }
    }
}