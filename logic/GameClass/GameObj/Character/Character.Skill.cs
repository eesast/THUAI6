using Preparation.Utility;
using Preparation.Interface;
using System.Collections.Generic;

namespace GameClass.GameObj
{
    public partial class Character
    {
        private readonly CharacterType characterType;
        public CharacterType CharacterType => characterType;
        private readonly IOccupation occupation;
        public IOccupation Occupation => occupation;

        private readonly Dictionary<ActiveSkillType, ActiveSkill> activeSkillDictionary = new();
        public Dictionary<ActiveSkillType, ActiveSkill> ActiveSkillDictionary => activeSkillDictionary;

        public ActiveSkill FindActiveSkill(ActiveSkillType activeSkillType)
        {
            if (Occupation.ListOfIActiveSkill.Contains(activeSkillType))
            {
                return ActiveSkillDictionary[activeSkillType];
            }
            return new NullSkill();
        }

        public bool IsGhost()
        {
            return GameData.IsGhost(CharacterType);
        }

        protected Character(XY initPos, int initRadius, CharacterType characterType) :
            base(initPos, initRadius, GameObjType.Character)
        {
            this.ReSetCanMove(true);
            this.score = 0;
            this.buffManager = new BuffManager();
            this.occupation = OccupationFactory.FindIOccupation(characterType);
            this.MaxHp = this.hp = Occupation.MaxHp;
            this.MoveSpeed = this.OrgMoveSpeed = Occupation.MoveSpeed;
            this.BulletOfPlayer = this.OriBulletOfPlayer = Occupation.InitBullet;
            this.concealment = Occupation.Concealment;
            this.alertnessRadius = Occupation.AlertnessRadius;
            this.viewRange = Occupation.ViewRange;
            this.characterType = characterType;
            this.speedOfOpeningOrLocking = Occupation.SpeedOfOpeningOrLocking;
            this.speedOfClimbingThroughWindows = Occupation.SpeedOfClimbingThroughWindows;
            this.speedOfOpenChest = Occupation.SpeedOfOpenChest;

            foreach (var activeSkill in this.Occupation.ListOfIActiveSkill)
            {
                this.ActiveSkillDictionary.Add(activeSkill, SkillFactory.FindActiveSkill(activeSkill));
            }

            // UsePassiveSkill();  //这一过程放到gamestart时进行

            Debugger.Output(this, "constructed!");
        }
    }
}