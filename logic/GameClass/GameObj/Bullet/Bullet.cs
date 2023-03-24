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
        public abstract int CD { get; }

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
        public abstract bool CanBeBombed(GameObjType gameObjType);

        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Prop || targetObj.Type == GameObjType.Bullet)
                return true;
            return false;
        }
        public Bullet(Character player, int radius, PlaceType placeType, XY Position) :
            base(Position, radius, GameObjType.Bullet)
        {
            this.place = placeType;
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
        public static Bullet? GetBullet(Character character, PlaceType place, XY pos)
        {
            switch (character.BulletOfPlayer)
            {
                case BulletType.FlyingKnife:
                    return new FlyingKnife(character, place, pos);
                case BulletType.CommonAttackOfGhost:
                    return new CommonAttackOfGhost(character, place, pos);
                case BulletType.AtomBomb:
                    return new AtomBomb(character, place, pos);
                case BulletType.LineBullet:
                    return new LineBullet(character, place, pos);
                case BulletType.FastBullet:
                    return new FastBullet(character, place, pos);
                case BulletType.OrdinaryBullet:
                    return new OrdinaryBullet(character, place, pos);
                default:
                    return null;
            }
        }
        public static int BulletRadius(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.AtomBomb:
                case BulletType.LineBullet:
                case BulletType.FastBullet:
                case BulletType.OrdinaryBullet:
                case BulletType.FlyingKnife:
                default:
                    return GameData.bulletRadius;
            }
        }
        public static int BulletCD(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.CommonAttackOfGhost:
                    return CommonAttackOfGhost.cd;
                case BulletType.FlyingKnife:
                    return FlyingKnife.cd;
                case BulletType.AtomBomb:
                case BulletType.LineBullet:
                case BulletType.FastBullet:
                case BulletType.OrdinaryBullet:
                default:
                    return GameData.basicCD;
            }
        }
    }
}
