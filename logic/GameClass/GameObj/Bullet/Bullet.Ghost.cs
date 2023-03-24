using Preparation.Interface;
using Preparation.Utility;
using System;

namespace GameClass.GameObj
{
    internal sealed class CommonAttackOfGhost : Bullet
    {
        public CommonAttackOfGhost(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => 0;
        public override double BulletAttackRange => GameData.basicAttackShortRange;
        public override int AP => GameData.basicApOfGhost;
        public override int Speed => GameData.basicBulletMoveSpeed;
        public override bool IsToBomb => false;

        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicBackswing;
        public override int CD => cd;

        public override bool CanAttack(GameObj target)
        {
            return false;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }
        public override BulletType TypeOfBullet => BulletType.CommonAttackOfGhost;

    }
    internal sealed class FlyingKnife : Bullet
    {
        public FlyingKnife(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => 0;
        public override double BulletAttackRange => GameData.basicRemoteAttackRange * 13;
        public override int AP => GameData.basicApOfGhost * 4 / 5;
        public override int Speed => GameData.basicBulletMoveSpeed * 2;
        public override bool IsToBomb => false;

        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing * 2 / 5;
        public override int RecoveryFromHit => GameData.basicBackswing * 3 / 4;
        public const int cd = GameData.basicBackswing * 2 / 5 + 100;
        public override int CD => cd;

        public override bool CanAttack(GameObj target)
        {
            return false;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }

        public override BulletType TypeOfBullet => BulletType.FlyingKnife;

    }

    internal sealed class AtomBomb : Bullet
    {
        public AtomBomb(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange * 7 / 3;
        public override double BulletAttackRange => GameData.basicAttackShortRange * 7 / 9;
        public override int AP => GameData.basicApOfGhost * 7 / 3;
        public override int Speed => GameData.basicBulletMoveSpeed * 2 / 3;

        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;

        public override bool IsToBomb => true;
        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }

        public override BulletType TypeOfBullet => BulletType.AtomBomb;

    }

    internal sealed class OrdinaryBullet : Bullet  // 1倍攻击范围，1倍攻击力，一倍速
    {
        public OrdinaryBullet(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange * 5 / 6;
        public override double BulletAttackRange => GameData.basicAttackShortRange / 2;
        public override int AP => GameData.basicApOfGhost * 5 / 6;
        public override int Speed => GameData.basicBulletMoveSpeed * 5 / 6;

        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;

        public override bool IsToBomb => true;
        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }

        public override BulletType TypeOfBullet => BulletType.OrdinaryBullet;
    }

    internal sealed class FastBullet : Bullet  // 1倍攻击范围，0.2倍攻击力，2倍速
    {
        public FastBullet(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange * 2 / 4;
        public override double BulletAttackRange => GameData.basicAttackShortRange;
        public override int AP => (int)(0.5 * GameData.basicApOfGhost);
        public override int Speed => 5 * GameData.basicBulletMoveSpeed / 3;
        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;
        public override bool IsToBomb => true;

        public override bool CanAttack(GameObj target)
        {
            // 圆形攻击范围
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }

        public override BulletType TypeOfBullet => BulletType.FastBullet;
    }

    internal sealed class LineBullet : Bullet  // 直线爆炸，宽度1格，长度为2倍爆炸范围
    {
        public LineBullet(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange * 4 / 3;
        public override double BulletAttackRange => 0.1 * GameData.basicAttackShortRange;
        public override int AP => GameData.basicApOfGhost * 2 / 3;
        public override int Speed => GameData.basicBulletMoveSpeed / 3;
        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;
        public override bool IsToBomb => true;

        public override bool CanBeBombed(GameObjType gameObjType)
        {
            if (gameObjType == GameObjType.Character) return true;
            return false;
        }
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

}
