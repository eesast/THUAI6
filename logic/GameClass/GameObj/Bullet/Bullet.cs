using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Bullet : ObjOfCharacter
    {
        /// <summary>
        /// //攻击力
        /// </summary>
        public abstract double BulletBombRange { get; }
        public abstract double AttackDistance { get; }
        protected int ap;
        public int AP
        {
            get => Interlocked.CompareExchange(ref ap, 0, 1);
        }
        public void AddAP(int addAp)
        {
            Interlocked.Add(ref ap, addAp);
        }

        public abstract int Speed { get; }
        public abstract bool IsRemoteAttack { get; }
        public abstract int CastTime { get; }
        public abstract int Backswing { get; }
        public abstract int RecoveryFromHit { get; }
        public abstract int CD { get; }
        public abstract int MaxBulletNum { get; }

        private readonly bool hasSpear;
        /// <summary>
        /// 是否有矛
        /// </summary>
        public bool HasSpear => hasSpear;

        /// <summary>
        /// </summary>
        /// <param name="target">被尝试攻击者</param>
        /// <returns>是否可以攻击到</returns>
        public abstract bool CanAttack(GameObj target);
        public abstract bool CanBeBombed(GameObjType gameObjType);

        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj == Parent) return true;
            if (targetObj.Type == GameObjType.Gadget || targetObj.Type == GameObjType.Bullet)
                return true;
            return false;
        }
        public Bullet(Character player, int radius, XY Position) :
            base(Position, radius, GameObjType.Bullet)
        {
            this.canMove = true;
            this.MoveSpeed = this.Speed;
            this.hasSpear = player.TryUseSpear();
            this.Parent = player;
        }
        public override bool IsRigid => true;                 // 默认为true
        public override ShapeType Shape => ShapeType.Circle;  // 默认为圆形
        public abstract BulletType TypeOfBullet { get; }
    }

    public static class BulletFactory
    {
        public static Bullet? GetBullet(Character character, XY pos, BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.FlyingKnife:
                    return new FlyingKnife(character, pos);
                case BulletType.CommonAttackOfGhost:
                    return new CommonAttackOfGhost(character, pos);
                case BulletType.JumpyDumpty:
                    return new JumpyDumpty(character, pos);
                case BulletType.BombBomb:
                    return new BombBomb(character, pos);
                default:
                    return null;
            }
        }
        public static int BulletRadius(BulletType bulletType)
        {
            switch (bulletType)
            {
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
                case BulletType.BombBomb:
                    return BombBomb.cd;
                case BulletType.JumpyDumpty:
                    return JumpyDumpty.cd;
                default:
                    return GameData.basicCD;
            }
        }
        public static int BulletNum(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.CommonAttackOfGhost:
                    return CommonAttackOfGhost.maxBulletNum;
                case BulletType.FlyingKnife:
                    return FlyingKnife.maxBulletNum;
                case BulletType.BombBomb:
                    return BombBomb.maxBulletNum;
                case BulletType.JumpyDumpty:
                    return JumpyDumpty.maxBulletNum;
                default:
                    return 0;
            }
        }
    }
}
