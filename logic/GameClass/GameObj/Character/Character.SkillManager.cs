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
        private readonly Occupation occupation;
        public Occupation Occupation => occupation;

        private Dictionary<ActiveSkillType, int> TimeUntilActiveSkillAvailable { get; set; }

        public bool SetTimeUntilActiveSkillAvailable(ActiveSkillType activeSkillType, int timeUntilActiveSkillAvailable)
        {
            lock (gameObjLock)
            if (TimeUntilActiveSkillAvailable.ContainsKey(activeSkillType))
            {
                TimeUntilActiveSkillAvailable[activeSkillType] = (timeUntilActiveSkillAvailable > 0) ? timeUntilActiveSkillAvailable : 0;
                return true;
            }
            return false;
        }
        public bool UseActiveSkill(ActiveSkillType activeSkillType)
        {
            if (Occupation.ListOfIActiveSkill.Contains(ActiveSkillFactory.FindIActiveSkill(activeSkillType)))
                return ActiveSkillFactory.FindIActiveSkill(activeSkillType).;
            else false;
        }

        readonly CharacterPassiveSkill passiveSkill;
        public void UsePassiveSkill()
        {
            passiveSkill(this);
            return;
        }

        public bool IsGhost()
        {
            return this.characterType switch
            {
                this.CharacterType.Assassin => true,
                this.CharacterType.Vampire => true,

                this.CharacterType.Null => false,
                this.CharacterType.RecoverAfterBattle => false,
                this.CharacterType.SpeedUpWhenLeavingGrass => false,
                this.CharacterType.PSkill4 => false,
                this.CharacterType.PSkill5 => false,
                _ => false,
            };
        }

        public Character(XY initPos, int initRadius, PlaceType initPlace, CharacterType characterType, ActiveSkillType commonSkillType) :
            base(initPos, initRadius, initPlace, GameObjType.Character)
        {
            this.CanMove = true;
            this.score = 0;
            this.propInventory = null;
            this.buffManeger = new BuffManeger();
            IPassiveSkill pSkill;
            IActiveSkill cSkill;
            switch (characterType)
            {
                case this.CharacterType.Assassin:
                    pSkill = null;
                    break;
                case this.CharacterType.RecoverAfterBattle:
                    pSkill = new RecoverAfterBattle();
                    break;
                case this.CharacterType.SpeedUpWhenLeavingGrass:
                    pSkill = new SpeedUpWhenLeavingGrass();
                    break;
                case this.CharacterType.Vampire:
                    pSkill = new Vampire();
                    break;
                default:
                    pSkill = new NoPassiveSkill();
                    break;
            }
            switch (commonSkillType)
            {
                case ActiveSkillType.BecomeAssassin:
                    cSkill = new BecomeInvisible();
                    break;
                case ActiveSkillType.BecomeVampire:
                    cSkill = new BecomeVampire();
                    break;
                case ActiveSkillType.NuclearWeapon:
                    cSkill = new NuclearWeapon();
                    break;
                case ActiveSkillType.SuperFast:
                    cSkill = new SuperFast();
                    break;
                default:
                    cSkill = new NoCommonSkill();
                    break;
            }
            this.MaxHp = cSkill.MaxHp;
            this.hp = cSkill.MaxHp;
            this.OrgMoveSpeed = cSkill.MoveSpeed;
            this.moveSpeed = cSkill.MoveSpeed;
            this.maxBulletNum = cSkill.MaxBulletNum;
            this.bulletNum = maxBulletNum;
            this.bulletOfPlayer = pSkill.InitBullet;
            this.OriBulletOfPlayer = pSkill.InitBullet;
            this.passiveSkill = pSkill.SkillEffect;
            this.commonSkill = cSkill.SkillEffect;
            this.characterType = characterType;
            this.commonSkillType = commonSkillType;

            // UsePassiveSkill();  //创建player时开始被动技能，这一过程也可以放到gamestart时进行
            // 这可以放在AddPlayer中做

            Debugger.Output(this, "constructed!");
        }
    }
}
