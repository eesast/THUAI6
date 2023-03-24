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
}