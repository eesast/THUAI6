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
        public abstract int AP { get; }
        public abstract int Speed { get; }

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
            if (targetObj.Type == GameObjType.BirthPoint || targetObj.Type == GameObjType.Prop || targetObj.Type == GameObjType.Bullet)
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

    internal sealed class AtomBomb : Bullet
    {
        public AtomBomb(Character player, int radius = GameData.bulletRadius) :
            base(player, radius)
        {
        }
        public const double BulletBombRange = GameData.basicBulletBombRange / 3 * 7;
        public const double BulletAttackRange = GameData.basicAttackRange / 9 * 7;
        public override int AP => GameData.basicAp / 3 * 7;
        public override int Speed => GameData.basicBulletMoveSpeed / 3 * 2;
        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }

        public override BulletType TypeOfBullet => BulletType.AtomBomb;
    }

    internal sealed class OrdinaryBullet : Bullet  // 1倍攻击范围，1倍攻击力，一倍速
    {
        public OrdinaryBullet(Character player, int radius = GameData.bulletRadius) :
            base(player, radius)
        {
        }
        public const double BulletBombRange = GameData.basicBulletBombRange / 6 * 5;
        public const double BulletAttackRange = GameData.basicAttackRange / 2;
        public override int AP => GameData.basicAp / 6 * 5;
        public override int Speed => GameData.basicBulletMoveSpeed / 6 * 5;
        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }

        public override BulletType TypeOfBullet => BulletType.OrdinaryBullet;
    }

    internal sealed class FastBullet : Bullet  // 1倍攻击范围，0.2倍攻击力，2倍速
    {
        public FastBullet(Character player, int radius = GameData.bulletRadius) :
            base(player, radius)
        {
        }
        public const double BulletBombRange = GameData.basicBulletBombRange / 4 * 2;
        public const double BulletAttackRange = GameData.basicAttackRange;
        public override int AP => (int)(0.5 * GameData.basicAp);
        public override int Speed => 5 * GameData.basicBulletMoveSpeed / 3;

        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }

        public override BulletType TypeOfBullet => BulletType.FastBullet;
    }

    internal sealed class LineBullet : Bullet  // 直线爆炸，宽度1格，长度为2倍爆炸范围
    {
        public LineBullet(Character player, int radius = GameData.bulletRadius) :
            base(player, radius)
        {
        }
        public const double BulletBombRange = GameData.basicBulletBombRange / 3 * 4;
        public const double BulletAttackRange = 0.1 * GameData.basicAttackRange;
        public override int AP => GameData.basicAp / 3 * 2;
        public override int Speed => GameData.basicBulletMoveSpeed / 3;

        public override bool CanAttack(GameObj target)
        {
            double FacingAngle = Math.Atan2(this.FacingDirection.y, this.FacingDirection.x);
            if (Math.Abs(FacingAngle - Math.PI / 2) < 1e-2)
            {
                if (target.Position.y - this.Position.y > BulletBombRange)
                    return false;
                if (target.Position.x < this.Position.x + GameData.numOfPosGridPerCell / 2 && target.Position.x > this.Position.x - GameData.numOfPosGridPerCell / 2)
                    return true;
                return false;
            }
            else if (Math.Abs(FacingAngle - Math.PI * 3 / 2) < 1e-2)
            {
                if (target.Position.y - this.Position.y < -BulletBombRange)
                    return false;
                if (target.Position.x < this.Position.x + GameData.numOfPosGridPerCell / 2 && target.Position.x > this.Position.x - GameData.numOfPosGridPerCell / 2)
                    return true;
                return false;
            }
            else if (Math.Abs(FacingAngle) < 1e-2)
            {
                if (target.Position.x - this.Position.x > BulletBombRange)
                    return false;
                if (target.Position.y < this.Position.y + GameData.numOfPosGridPerCell / 2 && target.Position.y > this.Position.y - GameData.numOfPosGridPerCell / 2)
                    return true;
                return false;
            }
            else if (Math.Abs(FacingAngle - Math.PI) < 1e-2)
            {
                if (target.Position.x - this.Position.x < -BulletBombRange)
                    return false;
                if (target.Position.y < this.Position.y + GameData.numOfPosGridPerCell / 2 && target.Position.y > this.Position.y - GameData.numOfPosGridPerCell / 2)
                    return true;
                return false;
            }
            double vertical = Math.Tan(FacingAngle + Math.PI / 2);
            bool posValue = vertical * Math.Cos(FacingAngle) - Math.Sin(FacingAngle) > 0;
            double dist;
            dist = vertical * (target.Position.x - this.Position.x) - (target.Position.y - this.Position.y) / Math.Sqrt(1 + vertical * vertical);
            if (Math.Abs(dist) > BulletBombRange)
                return false;
            else if (dist < 0 && posValue)  // 位于直线两侧
                return false;
            vertical = Math.Tan(FacingAngle);
            dist = Math.Abs(vertical * (target.Position.x - this.Position.x) - (target.Position.y - this.Position.y)) / Math.Sqrt(1 + vertical * vertical);
            if (dist > GameData.numOfPosGridPerCell / 2)
                return false;
            return true;
        }

        public override BulletType TypeOfBullet => BulletType.LineBullet;
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
        public static double BulletAttackRange(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.AtomBomb:
                    return AtomBomb.BulletAttackRange;
                case BulletType.LineBullet:
                    return LineBullet.BulletAttackRange;
                case BulletType.FastBullet:
                    return FastBullet.BulletAttackRange;
                case BulletType.OrdinaryBullet:
                    return OrdinaryBullet.BulletAttackRange;
                default:
                    return 0;
            }
        }
        public static double BulletBombRange(BulletType bulletType)
        {
            switch (bulletType)
            {
                case BulletType.AtomBomb:
                    return AtomBomb.BulletBombRange;
                case BulletType.LineBullet:
                    return LineBullet.BulletBombRange;
                case BulletType.FastBullet:
                    return FastBullet.BulletBombRange;
                case BulletType.OrdinaryBullet:
                    return OrdinaryBullet.BulletBombRange;
                default:
                    return 0;
            }
        }
    }
}
