using GameClass.GameObj;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;

namespace GameClass.Skill
{
    public abstract class Occupation
    {
        public abstract int MoveSpeed { get; }
        public abstract int MaxHp { get; }
        public abstract BulletType InitBullet { get; }
        public abstract List<IActiveSkill> ListOfIActiveSkill { get; }
        public abstract List<IPassiveSkill> ListOfIPassiveSkill { get; }
    }
    public class Assassin : Occupation
    {
        private const int moveSpeed = GameData.basicMoveSpeed/380*473;
        public override int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public override int MaxHp => maxHp;

        public override BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public override List<IActiveSkill> ListOfIActiveSkill => new(new IActiveSkill[]{new BecomeInvisible(),});
        public override List<IPassiveSkill> ListOfIPassiveSkill => new(new IPassiveSkill[] {});
    }

}
