using GameClass.GameObj;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;

namespace GameClass.Skill
{
    public interface IOccupation
    {
        public int MoveSpeed { get; }
        public int MaxHp { get; }
        public BulletType InitBullet { get; }
        public int CD { get; }
        public int MaxBulletNum { get; }
        public List<IActiveSkill> ListOfIActiveSkill { get; }
        public List<IPassiveSkill> ListOfIPassiveSkill { get; }
    }
    public class Assassin : IOccupation
    {
        private const int moveSpeed = GameData.basicMoveSpeed / 380 * 473;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public const int cd = 0;
        public int CD => cd;

        public const int maxBulletNum = 1;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<IActiveSkill> ListOfIActiveSkill => new(new IActiveSkill[] { new BecomeInvisible(), });
        public List<IPassiveSkill> ListOfIPassiveSkill => new(new IPassiveSkill[] { });
    }

}
