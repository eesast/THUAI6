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
        public int ap = GameData.basicApOfGhost;
        public override int AP
        {
            get => ap;
            set
            {
                lock (gameObjLock)
                    ap = value;
            }
        }
        public override int Speed => GameData.basicBulletMoveSpeed;
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)BulletAttackRange / Speed;
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
            switch (gameObjType)
            {
                case GameObjType.Character:
                case GameObjType.Generator:
                    return true;
                default:
                    return false;
            }
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
        public int ap = GameData.basicApOfGhost * 4 / 5;
        public override int AP
        {
            get => ap;
            set
            {
                lock (gameObjLock)
                    ap = value;
            }
        }
        public override int Speed => GameData.basicBulletMoveSpeed * 2;
        public override bool IsRemoteAttack => true;

        public override int CastTime => GameData.basicCastTime;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = GameData.basicBackswing * 2 / 5 + 100;
        public override int CD => cd;

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
        public BombBomb(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange;
        public override double BulletAttackRange => GameData.basicAttackShortRange;
        public int ap = (int)(GameData.basicApOfGhost * 6.0 / 5);
        public override int AP
        {
            get => ap;
            set
            {
                lock (gameObjLock)
                    ap = value;
            }
        }
        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 0.8);
        public override bool IsRemoteAttack => false;

        public override int CastTime => (int)BulletAttackRange / Speed;
        public override int Backswing => GameData.basicBackswing;
        public override int RecoveryFromHit => GameData.basicRecoveryFromHit;
        public const int cd = GameData.basicCD;
        public override int CD => cd;

        public override bool CanAttack(GameObj target)
        {
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
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
        public override BulletType TypeOfBullet => BulletType.BombBomb;

    }
    internal sealed class JumpyDumpty : Bullet
    {
        public JumpyDumpty(Character player, PlaceType placeType, XY pos, int radius = GameData.bulletRadius) :
            base(player, radius, placeType, pos)
        {
        }
        public override double BulletBombRange => GameData.basicBulletBombRange / 2;
        public override double BulletAttackRange => GameData.basicAttackShortRange * 2;
        public int ap = (int)(GameData.basicApOfGhost * 0.6);
        public override int AP
        {
            get => ap;
            set
            {
                lock (gameObjLock)
                    ap = value;
            }
        }
        public override int Speed => (int)(GameData.basicBulletMoveSpeed * 1.2);
        public override bool IsRemoteAttack => false;

        public override int CastTime => 0;
        public override int Backswing => 0;
        public override int RecoveryFromHit => 0;
        public const int cd = 0;
        public override int CD => cd;

        public override bool CanAttack(GameObj target)
        {
            return XY.Distance(this.Position, target.Position) <= BulletBombRange;
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
        public override BulletType TypeOfBullet => BulletType.JumpyDumpty;

    }
}