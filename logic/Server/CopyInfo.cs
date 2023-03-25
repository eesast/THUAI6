using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;
using System.Numerics;
using Preparation.Utility;

namespace Server
{

    public static class CopyInfo
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
                case Preparation.Utility.PropType.AddLifeOrAp:
                    return Protobuf.PropType.AddLifeOrAp;
                case Preparation.Utility.PropType.AddHpOrAp:
                    return Protobuf.PropType.AddHpOrAp;
                case Preparation.Utility.PropType.ShieldOrSpear:
                    return Protobuf.PropType.ShieldOrSpear;
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
                case Protobuf.PropType.AddLifeOrAp:
                    return Preparation.Utility.PropType.AddLifeOrAp;
                case Protobuf.PropType.AddHpOrAp:
                    return Preparation.Utility.PropType.AddHpOrAp;
                case Protobuf.PropType.ShieldOrSpear:
                    return Preparation.Utility.PropType.ShieldOrSpear;
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
                case Preparation.Utility.BuffType.Null:
                default:
                    return Protobuf.StudentBuffType.NullSbuffType;
            }
        }
        public static Protobuf.TrickerBuffType ToTrickerBuffType(Preparation.Utility.BuffType buffType)
        {
            switch (buffType)
            {
                case Preparation.Utility.BuffType.Null:
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
                    return StudentType._2;
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
                default:
                    return Preparation.Utility.CharacterType.Null;
            }
        }

        public static MessageOfObj? Auto(GameObj gameObj)
        {
            switch (gameObj.Type)
            {
                case Preparation.Utility.GameObjType.Character:
                    Character character = (Character)gameObj;
                    if (character.IsGhost())
                        return Tricker((Ghost)character);
                    else return Student((Student)character);
                case Preparation.Utility.GameObjType.Bullet:
                    return Bullet((Bullet)gameObj);
                case Preparation.Utility.GameObjType.Prop:
                    return Prop((Prop)gameObj);
                case Preparation.Utility.GameObjType.BombedBullet:
                    return BombedBullet((BombedBullet)gameObj);
                case Preparation.Utility.GameObjType.PickedProp:
                    return PickedProp((PickedProp)gameObj);
                case Preparation.Utility.GameObjType.Generator:
                    return Classroom((Generator)gameObj);
                case Preparation.Utility.GameObjType.Chest:
                    return Chest((Chest)gameObj);
                case Preparation.Utility.GameObjType.Doorway:
                    return Gate((Doorway)gameObj);
                case Preparation.Utility.GameObjType.EmergencyExit:
                    if (((EmergencyExit)gameObj).CanOpen)
                        return HiddenGate((EmergencyExit)gameObj);
                    else return null;
                case Preparation.Utility.GameObjType.Door:
                    return Door((Door)gameObj);
                default: return null;
            }
        }
        public static MessageOfObj? Auto(MessageOfNews news)
        {
            MessageOfObj objMsg = new();
            objMsg.NewsMessage = news;
            return objMsg;
        }

        private static MessageOfObj? Student(Student player)
        {
            MessageOfObj msg = new MessageOfObj();
            if (player.IsGhost()) return null;
            msg.StudentMessage = new();

            msg.StudentMessage.X = player.Position.x;
            msg.StudentMessage.Y = player.Position.y;
            msg.StudentMessage.Speed = player.MoveSpeed;
            msg.StudentMessage.Determination = player.HP;
            msg.StudentMessage.Addiction = player.GamingAddiction;

            foreach (var keyValue in player.TimeUntilActiveSkillAvailable)
                msg.StudentMessage.TimeUntilSkillAvailable.Add(keyValue.Value);
            for (int i = 0; i < GameData.maxNumOfSkill - player.TimeUntilActiveSkillAvailable.Count(); ++i)
                msg.StudentMessage.TimeUntilSkillAvailable.Add(-1);

            foreach (var value in player.PropInventory)
                msg.StudentMessage.Prop.Add(ToPropType(value.GetPropType()));

            msg.StudentMessage.Place = ToPlaceType(player.Place);
            msg.StudentMessage.Guid = player.ID;

            msg.StudentMessage.PlayerState = ToPlayerState(player.PlayerState);
            msg.StudentMessage.PlayerId = player.PlayerID;
            msg.StudentMessage.ViewRange = player.ViewRange;
            msg.StudentMessage.Radius = player.Radius;
            msg.StudentMessage.DangerAlert = (player.BgmDictionary.ContainsKey(BgmType.GhostIsComing)) ? player.BgmDictionary[BgmType.GhostIsComing] : 0;
            msg.StudentMessage.Score = player.Score;
            msg.StudentMessage.TreatProgress = player.DegreeOfTreatment;
            msg.StudentMessage.RescueProgress = player.TimeOfRescue;

            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                    msg.StudentMessage.Buff.Add(ToStudentBuffType(kvp.Key));
            }

            msg.StudentMessage.BulletType = ToBulletType(player.BulletOfPlayer);
            msg.StudentMessage.LearningSpeed = player.FixSpeed;
            msg.StudentMessage.TreatSpeed = player.TreatSpeed;
            msg.StudentMessage.FacingDirection = player.FacingDirection.Angle();
            msg.StudentMessage.StudentType = ToStudentType(player.CharacterType);
            return msg;
        }

        private static MessageOfObj? Tricker(Character player)
        {
            MessageOfObj msg = new MessageOfObj();
            if (!player.IsGhost()) return null;
            msg.TrickerMessage = new();

            msg.TrickerMessage.X = player.Position.x;
            msg.TrickerMessage.Y = player.Position.y;
            msg.TrickerMessage.Speed = player.MoveSpeed;
            foreach (var keyValue in player.TimeUntilActiveSkillAvailable)
                msg.TrickerMessage.TimeUntilSkillAvailable.Add(keyValue.Value);
            for (int i = 0; i < GameData.maxNumOfSkill - player.TimeUntilActiveSkillAvailable.Count(); ++i)
                msg.TrickerMessage.TimeUntilSkillAvailable.Add(-1);

            msg.TrickerMessage.Place = ToPlaceType(player.Place);
            foreach (var value in player.PropInventory)
                msg.TrickerMessage.Prop.Add(ToPropType(value.GetPropType()));

            msg.TrickerMessage.TrickerType = ToTrickerType(player.CharacterType);
            msg.TrickerMessage.Guid = player.ID;
            msg.TrickerMessage.Score = player.Score;
            msg.TrickerMessage.PlayerId = player.PlayerID;
            msg.TrickerMessage.ViewRange = player.ViewRange;
            msg.TrickerMessage.Radius = player.Radius;
            msg.TrickerMessage.PlayerState = ToPlayerState(player.PlayerState);
            msg.TrickerMessage.TrickDesire = (player.BgmDictionary.ContainsKey(BgmType.StudentIsApproaching)) ? player.BgmDictionary[BgmType.StudentIsApproaching] : 0;
            msg.TrickerMessage.ClassVolume = (player.BgmDictionary.ContainsKey(BgmType.GeneratorIsBeingFixed)) ? player.BgmDictionary[BgmType.GeneratorIsBeingFixed] : 0;
            msg.TrickerMessage.FacingDirection = player.FacingDirection.Angle();
            msg.TrickerMessage.BulletType = ToBulletType(player.BulletOfPlayer);
            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                    msg.TrickerMessage.Buff.Add(ToTrickerBuffType(kvp.Key));
            }


            return msg;
        }

        private static MessageOfObj Bullet(Bullet bullet)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.BulletMessage = new();
            msg.BulletMessage.X = bullet.Position.x;
            msg.BulletMessage.Y = bullet.Position.y;
            msg.BulletMessage.FacingDirection = bullet.FacingDirection.Angle();
            msg.BulletMessage.Guid = bullet.ID;
            msg.BulletMessage.Team = (bullet.Parent.IsGhost()) ? PlayerType.TrickerPlayer : PlayerType.StudentPlayer;
            msg.BulletMessage.Place = ToPlaceType(bullet.Place);
            msg.BulletMessage.BombRange = bullet.BulletBombRange;
            msg.BulletMessage.Speed = bullet.Speed;
            return msg;
        }

        private static MessageOfObj Prop(Prop prop)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.PropMessage = new();
            msg.PropMessage.Type = ToPropType(prop.GetPropType());
            msg.PropMessage.X = prop.Position.x;
            msg.PropMessage.Y = prop.Position.y;
            msg.PropMessage.FacingDirection = prop.FacingDirection.Angle();
            msg.PropMessage.Guid = prop.ID;
            msg.PropMessage.Place = ToPlaceType(prop.Place);
            return msg;
        }

        private static MessageOfObj BombedBullet(BombedBullet bombedBullet)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.BombedBulletMessage = new();
            msg.BombedBulletMessage.X = bombedBullet.bulletHasBombed.Position.x;
            msg.BombedBulletMessage.Y = bombedBullet.bulletHasBombed.Position.y;
            msg.BombedBulletMessage.FacingDirection = bombedBullet.FacingDirection.Angle();
            msg.BombedBulletMessage.MappingId = bombedBullet.MappingID;
            msg.BombedBulletMessage.BombRange = bombedBullet.bulletHasBombed.BulletBombRange;
            return msg;
        }

        private static MessageOfObj PickedProp(PickedProp pickedProp)
        {
            MessageOfObj msg = new MessageOfObj(); // MessageOfObj中没有PickedProp
            /*msg.MessageOfPickedProp = new MessageOfPickedProp();

            msg.MessageOfPickedProp.MappingID = pickedProp.MappingID;
            msg.MessageOfPickedProp.X = pickedProp.PropHasPicked.Position.x;
            msg.MessageOfPickedProp.Y = pickedProp.PropHasPicked.Position.y;
            msg.MessageOfPickedProp.FacingDirection = pickedProp.PropHasPicked.FacingDirection;*/
            return msg;
        }

        private static MessageOfObj Classroom(Generator generator)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.ClassroomMessage = new();
            msg.ClassroomMessage.X = generator.Position.x;
            msg.ClassroomMessage.Y = generator.Position.y;
            msg.ClassroomMessage.Progress = generator.DegreeOfRepair;
            return msg;
        }
        private static MessageOfObj Gate(Doorway doorway)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.GateMessage = new();
            msg.GateMessage.X = doorway.Position.x;
            msg.GateMessage.Y = doorway.Position.y;
            msg.GateMessage.Progress = doorway.OpenDegree;
            return msg;
        }
        private static MessageOfObj HiddenGate(EmergencyExit Exit)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.HiddenGateMessage = new();
            msg.HiddenGateMessage.X = Exit.Position.x;
            msg.HiddenGateMessage.Y = Exit.Position.y;
            msg.HiddenGateMessage.Opened = Exit.IsOpen;
            return msg;
        }

        private static MessageOfObj Door(Door door)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.DoorMessage = new();
            msg.DoorMessage.X = door.Position.x;
            msg.DoorMessage.Y = door.Position.y;
            msg.DoorMessage.Progress = door.OpenOrLockDegree;
            msg.DoorMessage.IsOpen = door.IsOpen;
            return msg;
        }
        private static MessageOfObj Chest(Chest chest)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.ChestMessage = new();
            msg.ChestMessage.X = chest.Position.x;
            msg.ChestMessage.Y = chest.Position.y;
            msg.ChestMessage.Progress = chest.OpenDegree;
            return msg;
        }
    }
}
