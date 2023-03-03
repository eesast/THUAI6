using Preparation.Interface;
using Preparation.Utility;
using System;

namespace GameClass.GameObj
{
    public abstract class Bullet : ObjOfCharacter
    {
        /// <summary>
        /// //攻击力
        /// </summary>
        public abstract double BulletBombRange { get; }
        public abstract double BulletAttackRange { get; }
        public abstract int AP { get; }
        public abstract int Speed { get; }
        public abstract bool IsToBomb { get; }
        public abstract int CastTime { get; }
        public abstract int Backswing { get; }
        public abstract int RecoveryFromHit { get; }

        private readonly bool hasSpear;
        /// <summary>
        /// 是否有矛
        /// </summary>
        public bool HasSpear => hasSpear;

        /// <summary>
        /// 与THUAI4不同的一个攻击判定方案，通过这个函数判断爆炸时能否伤害到target
        /// </summary>
        /// <param name="target">被尝试攻击者</param>
        /// <returns>是否可以攻击到</returns>
        public abstract bool CanAttack(GameObj target);

        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Prop || targetObj.Type == GameObjType.Bullet)
                return true;
            return false;
        }
        public Bullet(Character player, int radius) :
            base(player.Position, radius, PlaceType.Null, GameObjType.Bullet)
        {
            this.CanMove = true;
            this.moveSpeed = this.Speed;
            this.hasSpear = player.HasSpear;
            this.Parent = player;
        }
        public override bool IsRigid => true;                 // 默认为true
        public override ShapeType Shape => ShapeType.Circle;  // 默认为圆形
        public abstract BulletType TypeOfBullet { get; }
    }

    public static class BulletFactory
    {
        public static Bullet? GetBullet(Character character)
        {
            Bullet? newBullet = null;
            switch (character.BulletOfPlayer)
            {
                case BulletType.AtomBomb:
                    newBullet = new AtomBomb(character);
                    break;
                case BulletType.LineBullet:
                    newBullet = new LineBullet(character);
                    break;
                case BulletType.FastBullet:
                    newBullet = new FastBullet(character);
                    break;
                case BulletType.OrdinaryBullet:
                    newBullet = new OrdinaryBullet(character);
                    break;
                default:
                    break;
            }
            return newBullet;
        }
        public static int BulletRadius(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.AtomBomb:
                case BulletType.LineBullet:
                case BulletType.FastBullet:
                case BulletType.OrdinaryBullet:
                default:
                    return GameData.bulletRadius;
            }
        }
    }
}
