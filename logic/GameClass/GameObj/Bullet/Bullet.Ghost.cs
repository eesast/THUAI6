using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    internal sealed class CommonAttackOfGhost : Bullet
    {
        public CommonAttackOfGhost(Character player, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, pos)
        {
            AP.Set(GameData.basicApOfGhost);
        }
        public override double BulletBombRange => 0;
        public override double AttackDistance => GameData.basicAttackShortRange;
        public override int Speed => GameData.basicBulletMoveSpeed;
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)AttackDistance * 1000 / Speed;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicBackswing;
        public override int CD => cd;
        public const int maxBulletNum = 1;
        public override int MaxBulletNum => maxBulletNum;

        public override bool CanAttack(GameObj target)
        {
            return false;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                    return true;
                default:
                    return false;
            }
        }
        public override BulletType TypeOfBullet => BulletType.CommonAttackOfGhost;
    }

    internal sealed class Strike : Bullet
    {
        public Strike(Character player, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, pos)
        {
            AP.Set(GameData.basicApOfGhost * 16 / 15);
        }
        public override double BulletBombRange => 0;
        public override double AttackDistance => GameData.basicAttackShortRange * 20 / 22;
        public override int Speed => GameData.basicBulletMoveSpeed * 625 / 740;
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)AttackDistance * 1000 / Speed;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicBackswing;
        public override int CD => cd;
        public const int maxBulletNum = 1;
        public override int MaxBulletNum => maxBulletNum;

        public override bool CanAttack(GameObj target)
        {
            return false;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                    return true;
                default:
                    return false;
            }
        }
        public override BulletType TypeOfBullet => BulletType.Strike;
    }

    internal sealed class FlyingKnife : Bullet
    {
        public FlyingKnife(Character player, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, pos)
        {
            AP.Set(GameData.basicApOfGhost * 4 / 5);
        }
        public override double BulletBombRange => 0;
        public override double AttackDistance => GameData.basicRemoteAttackRange * 13;

        public override int Speed => GameData.basicBulletMoveSpeed * 25 / 10;
        public override bool IsRemoteAttack => true;

        public const int castTime = GameData.basicCastTime * 6 / 5;
        public override int CastTime => castTime;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = castTime;
        public override int CD => cd;
        public const int maxBulletNum = 1;
        public override int MaxBulletNum => maxBulletNum;

        public override bool CanAttack(GameObj target)
        {
            return false;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                    return true;
                default:
                    return false;
            }
        }

        public override BulletType TypeOfBullet => BulletType.FlyingKnife;

    }

    internal sealed class BombBomb : Bullet
    {
        public BombBomb(Character player, XY pos, int radius = GameData.bulletRadius) : base(player, radius, pos)
        {
            AP.Set((int)(GameData.basicApOfGhost * 6.0 / 5));
        }
        public override double BulletBombRange => GameData.basicBulletBombRange;
        public override double AttackDistance => GameData.basicAttackShortRange;

        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 30 / 37);
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)(AttackDistance * 1000 / Speed);
        public override int Backswing => GameData.basicBackswing * 3 / 2;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;
        public const int maxBulletNum = 1;
        public override int MaxBulletNum => maxBulletNum;

        public override bool CanAttack(GameObj target)
        {
            return XY.DistanceFloor3(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                case GameObjType.Door:
                    return true;
                default:
                    return false;
            }
        }
        public override BulletType TypeOfBullet => BulletType.BombBomb;

    }

    internal sealed class JumpyDumpty : Bullet
    {
        public JumpyDumpty(Character player, XY pos, int radius = GameData.bulletRadius) : base(player, radius, pos)
        {
            AP.Set((int)(GameData.basicApOfGhost * 0.6));
        }
        public override double BulletBombRange => GameData.basicBulletBombRange / 2;
        public override double AttackDistance => GameData.basicAttackShortRange * 18 / 22;

        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 43 / 37);
        public override bool IsRemoteAttack => false;

        public override int CastTime => 0;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = 0;
        public override int CD => cd;
        public const int maxBulletNum = 4;
        public override int MaxBulletNum => maxBulletNum;

        public override bool CanAttack(GameObj target)
        {
            return XY.DistanceFloor3(this.Position, target.Position) <= BulletBombRange;
        }
        public override bool CanBeBombed(GameObjType gameObjType)
        {
            switch (gameObjType)
            {
                case GameObjType.Character:
                    return true;
                default:
                    return false;
            }
        }
        public override BulletType TypeOfBullet => BulletType.JumpyDumpty;

    }
}