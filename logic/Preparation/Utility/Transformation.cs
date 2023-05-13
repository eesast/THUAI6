using Protobuf;

namespace Preparation.Utility
{
    public static class Transformation
    {

        public static Protobuf.PlaceType ToPlaceType(Preparation.Utility.PlaceType place)
        {
            switch (place)
            {
                case Preparation.Utility.PlaceType.Window:
                    return Protobuf.PlaceType.Window;
                case Preparation.Utility.PlaceType.EmergencyExit:
                    return Protobuf.PlaceType.HiddenGate;
                case Preparation.Utility.PlaceType.Doorway:
                    return Protobuf.PlaceType.Gate;
                case Preparation.Utility.PlaceType.Chest:
                    return Protobuf.PlaceType.Chest;
                case Preparation.Utility.PlaceType.Door3:
                    return Protobuf.PlaceType.Door3;
                case Preparation.Utility.PlaceType.Door5:
                    return Protobuf.PlaceType.Door5;
                case Preparation.Utility.PlaceType.Door6:
                    return Protobuf.PlaceType.Door6;
                case Preparation.Utility.PlaceType.Generator:
                    return Protobuf.PlaceType.Classroom;
                case Preparation.Utility.PlaceType.Grass:
                    return Protobuf.PlaceType.Grass;
                case Preparation.Utility.PlaceType.Wall:
                    return Protobuf.PlaceType.Wall;
                case Preparation.Utility.PlaceType.Null:
                case Preparation.Utility.PlaceType.BirthPoint1:
                case Preparation.Utility.PlaceType.BirthPoint2:
                case Preparation.Utility.PlaceType.BirthPoint3:
                case Preparation.Utility.PlaceType.BirthPoint4:
                case Preparation.Utility.PlaceType.BirthPoint5:
                    return Protobuf.PlaceType.Land;
                default:
                    return Protobuf.PlaceType.NullPlaceType;
            }
        }

        public static Protobuf.PropType ToPropType(Preparation.Utility.PropType prop)
        {
            switch (prop)
            {
                case Preparation.Utility.PropType.AddSpeed:
                    return Protobuf.PropType.AddSpeed;
                case Preparation.Utility.PropType.AddLifeOrClairaudience:
                    return Protobuf.PropType.AddLifeOrClairaudience;
                case Preparation.Utility.PropType.AddHpOrAp:
                    return Protobuf.PropType.AddHpOrAp;
                case Preparation.Utility.PropType.ShieldOrSpear:
                    return Protobuf.PropType.ShieldOrSpear;
                case Preparation.Utility.PropType.RecoveryFromDizziness:
                    return Protobuf.PropType.RecoveryFromDizziness;
                case Preparation.Utility.PropType.Key3:
                    return Protobuf.PropType.Key3;
                case Preparation.Utility.PropType.Key5:
                    return Protobuf.PropType.Key5;
                case Preparation.Utility.PropType.Key6:
                    return Protobuf.PropType.Key6;
                default:
                    return Protobuf.PropType.NullPropType;
            }
        }
        public static Preparation.Utility.PropType ToPropType(Protobuf.PropType prop)
        {
            switch (prop)
            {
                case Protobuf.PropType.AddSpeed:
                    return Preparation.Utility.PropType.AddSpeed;
                case Protobuf.PropType.AddLifeOrClairaudience:
                    return Preparation.Utility.PropType.AddLifeOrClairaudience;
                case Protobuf.PropType.AddHpOrAp:
                    return Preparation.Utility.PropType.AddHpOrAp;
                case Protobuf.PropType.ShieldOrSpear:
                    return Preparation.Utility.PropType.ShieldOrSpear;
                case Protobuf.PropType.RecoveryFromDizziness:
                    return Preparation.Utility.PropType.RecoveryFromDizziness;
                case Protobuf.PropType.Key3:
                    return Preparation.Utility.PropType.Key3;
                case Protobuf.PropType.Key5:
                    return Preparation.Utility.PropType.Key5;
                case Protobuf.PropType.Key6:
                    return Preparation.Utility.PropType.Key6;
                default:
                    return Preparation.Utility.PropType.Null;
            }
        }

        public static Protobuf.PlayerState ToPlayerState(Preparation.Utility.PlayerStateType playerState)
        {
            switch (playerState)
            {
                case Preparation.Utility.PlayerStateType.Moving:
                case Preparation.Utility.PlayerStateType.Null:
                    return PlayerState.Idle;
                case Preparation.Utility.PlayerStateType.Addicted:
                    return PlayerState.Addicted;
                case Preparation.Utility.PlayerStateType.ClimbingThroughWindows:
                    return PlayerState.Climbing;
                case Preparation.Utility.PlayerStateType.Deceased:
                    return PlayerState.Quit;
                case Preparation.Utility.PlayerStateType.Escaped:
                    return PlayerState.Graduated;
                case Preparation.Utility.PlayerStateType.Fixing:
                    return PlayerState.Learning;
                case Preparation.Utility.PlayerStateType.LockingOrOpeningTheDoor:
                    return PlayerState.Locking;
                case Preparation.Utility.PlayerStateType.OpeningTheChest:
                    return PlayerState.OpeningAChest;
                case Preparation.Utility.PlayerStateType.Rescued:
                    return PlayerState.Rescued;
                case Preparation.Utility.PlayerStateType.Rescuing:
                    return PlayerState.Rescuing;
                case Preparation.Utility.PlayerStateType.Stunned:
                case Preparation.Utility.PlayerStateType.Charmed:
                    return PlayerState.Stunned;
                case Preparation.Utility.PlayerStateType.Swinging:
                    return PlayerState.Swinging;
                case Preparation.Utility.PlayerStateType.Treated:
                    return PlayerState.Treated;
                case Preparation.Utility.PlayerStateType.Treating:
                    return PlayerState.Treating;
                case Preparation.Utility.PlayerStateType.TryingToAttack:
                    return PlayerState.Attacking;
                case Preparation.Utility.PlayerStateType.UsingSkill:
                    return PlayerState.UsingSpecialSkill;
                case Preparation.Utility.PlayerStateType.OpeningTheDoorway:
                    return PlayerState.OpeningAGate;
                default:
                    return PlayerState.NullStatus;
            }
        }

        public static Protobuf.StudentBuffType ToStudentBuffType(Preparation.Utility.BuffType buffType)
        {
            switch (buffType)
            {
                case Preparation.Utility.BuffType.AddSpeed:
                    return Protobuf.StudentBuffType.StudentAddSpeed;
                case Preparation.Utility.BuffType.AddLife:
                    return Protobuf.StudentBuffType.AddLife;
                case Preparation.Utility.BuffType.Shield:
                    return Protobuf.StudentBuffType.Shield;
                case Preparation.Utility.BuffType.Invisible:
                    return Protobuf.StudentBuffType.StudentInvisible;
                default:
                    return Protobuf.StudentBuffType.NullSbuffType;
            }
        }
        public static Protobuf.TrickerBuffType ToTrickerBuffType(Preparation.Utility.BuffType buffType)
        {
            switch (buffType)
            {
                case Preparation.Utility.BuffType.AddSpeed:
                    return Protobuf.TrickerBuffType.TrickerAddSpeed;
                case Preparation.Utility.BuffType.Spear:
                    return Protobuf.TrickerBuffType.Spear;
                case Preparation.Utility.BuffType.AddAp:
                    return Protobuf.TrickerBuffType.AddAp;
                case Preparation.Utility.BuffType.Clairaudience:
                    return Protobuf.TrickerBuffType.Clairaudience;
                case Preparation.Utility.BuffType.Invisible:
                    return Protobuf.TrickerBuffType.TrickerInvisible;
                default:
                    return Protobuf.TrickerBuffType.NullTbuffType;
            }
        }

        public static Protobuf.BulletType ToBulletType(Preparation.Utility.BulletType bulletType)
        {
            switch (bulletType)
            {
                case Preparation.Utility.BulletType.FlyingKnife:
                    return Protobuf.BulletType.FlyingKnife;
                case Preparation.Utility.BulletType.CommonAttackOfGhost:
                    return Protobuf.BulletType.CommonAttackOfTricker;
                case Preparation.Utility.BulletType.BombBomb:
                    return Protobuf.BulletType.BombBomb;
                case Preparation.Utility.BulletType.JumpyDumpty:
                    return Protobuf.BulletType.JumpyDumpty;
                default:
                    return Protobuf.BulletType.NullBulletType;
            }
        }

        public static Protobuf.StudentType ToStudentType(Preparation.Utility.CharacterType characterType)
        {
            switch (characterType)
            {
                case Preparation.Utility.CharacterType.Athlete:
                    return Protobuf.StudentType.Athlete;
                case Preparation.Utility.CharacterType.Teacher:
                    return Protobuf.StudentType.Teacher;
                case Preparation.Utility.CharacterType.StraightAStudent:
                    return Protobuf.StudentType.StraightAStudent;
                case Preparation.Utility.CharacterType.Robot:
                    return Protobuf.StudentType.Robot;
                case Preparation.Utility.CharacterType.TechOtaku:
                    return Protobuf.StudentType.TechOtaku;
                case Preparation.Utility.CharacterType.Sunshine:
                    return Protobuf.StudentType.Sunshine;
                default:
                    return Protobuf.StudentType.NullStudentType;
            }
        }
        public static Preparation.Utility.CharacterType ToStudentType(Protobuf.StudentType characterType)
        {
            switch (characterType)
            {
                case Protobuf.StudentType.Athlete:
                    return Preparation.Utility.CharacterType.Athlete;
                case Protobuf.StudentType.Teacher:
                    return Preparation.Utility.CharacterType.Teacher;
                case Protobuf.StudentType.StraightAStudent:
                    return Preparation.Utility.CharacterType.StraightAStudent;
                case Protobuf.StudentType.Robot:
                    return Preparation.Utility.CharacterType.Robot;
                case Protobuf.StudentType.TechOtaku:
                    return Preparation.Utility.CharacterType.TechOtaku;
                case Protobuf.StudentType.Sunshine:
                    return Preparation.Utility.CharacterType.Sunshine;
                default:
                    return Preparation.Utility.CharacterType.Null;
            }
        }

        public static Protobuf.TrickerType ToTrickerType(Preparation.Utility.CharacterType characterType)
        {
            switch (characterType)
            {
                case Preparation.Utility.CharacterType.Assassin:
                    return Protobuf.TrickerType.Assassin;
                case Preparation.Utility.CharacterType.Klee:
                    return Protobuf.TrickerType.Klee;
                case Preparation.Utility.CharacterType.ANoisyPerson:
                    return Protobuf.TrickerType.ANoisyPerson;
                case CharacterType.Idol:
                    return TrickerType.Idol;
                default:
                    return Protobuf.TrickerType.NullTrickerType;
            }
        }
        public static Preparation.Utility.CharacterType ToTrickerType(Protobuf.TrickerType characterType)
        {
            switch (characterType)
            {
                case Protobuf.TrickerType.Assassin:
                    return Preparation.Utility.CharacterType.Assassin;
                case Protobuf.TrickerType.Klee:
                    return Preparation.Utility.CharacterType.Klee;
                case Protobuf.TrickerType.ANoisyPerson:
                    return Preparation.Utility.CharacterType.ANoisyPerson;
                case TrickerType.Idol:
                    return CharacterType.Idol;
                default:
                    return Preparation.Utility.CharacterType.Null;
            }
        }
    }
}