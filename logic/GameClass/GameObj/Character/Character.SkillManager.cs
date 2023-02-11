using GameClass.Skill;
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

        private readonly CharacterType characterType;
        public CharacterType CharacterType => characterType;
        public bool UseCommonSkill()
        {
            return commonSkill(this);
        }
        private int timeUntilCommonSkillAvailable = 0;  // 还剩多少时间可以使用普通技能
        public int TimeUntilCommonSkillAvailable
        {
            get => timeUntilCommonSkillAvailable;
            set
            {
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

        public bool IsGhost()
        {
            return this.characterType switch
            {
                CharacterType.Assassin => true,
                CharacterType.Vampire => true,

                CharacterType.Null => false,
                CharacterType.RecoverAfterBattle => false,
                CharacterType.SpeedUpWhenLeavingGrass => false,
                CharacterType.PSkill4 => false,
                CharacterType.PSkill5 => false,
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
            IPassiveSkill? pSkill;
            ICommonSkill cSkill;
            switch (characterType)
            {
                case CharacterType.Assassin:
                    pSkill = null;
                    break;
                case CharacterType.RecoverAfterBattle:
                    pSkill = new RecoverAfterBattle();
                    break;
                case CharacterType.SpeedUpWhenLeavingGrass:
                    pSkill = new SpeedUpWhenLeavingGrass();
                    break;
                case CharacterType.Vampire:
                    pSkill = new Vampire();
                    break;
                default:
                    pSkill = new NoPassiveSkill();
                    break;
            }
            switch (commonSkillType)
            {
                case ActiveSkillType.BecomeAssassin:
                    cSkill = new BecomeAssassin();
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
            this.cd = cSkill.CD;
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
