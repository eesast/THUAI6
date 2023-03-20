using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;
using System.Numerics;
using Preparation.Utility;

namespace Server
{

    public static class CopyInfo
    {
        private static Protobuf.PlaceType ToPlaceType(Preparation.Utility.PlaceType place)
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
                case Preparation.Utility.PlaceType.Door:
                    return Protobuf.PlaceType.Door;
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

        private static Protobuf.PropType ToPropType(Preparation.Utility.PropType prop)
        {
            switch (prop)
            {
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

        private static Protobuf.PlayerState ToPlayerState(Preparation.Utility.PlayerStateType playerState)
        {
            switch (playerState)
            {
                case Preparation.Utility.PlayerStateType.IsMoving:
                case Preparation.Utility.PlayerStateType.Null:
                    return PlayerState.Idle;
                case Preparation.Utility.PlayerStateType.IsAddicted:
                    return PlayerState.Addicted;
                case Preparation.Utility.PlayerStateType.IsClimbingThroughWindows:
                    return PlayerState.Climbing;
                case Preparation.Utility.PlayerStateType.IsDeceased:
                    return PlayerState.Quit;
                case Preparation.Utility.PlayerStateType.IsEscaped:
                    return PlayerState.Graduated;
                case Preparation.Utility.PlayerStateType.IsFixing:
                    return PlayerState.Learning;
                case Preparation.Utility.PlayerStateType.IsLockingTheDoor:
                    return PlayerState.Locking;
                case Preparation.Utility.PlayerStateType.IsOpeningTheChest:
                    return PlayerState.OpeningAChest;
                case Preparation.Utility.PlayerStateType.IsRescued:
                    return PlayerState.Rescued;
                case Preparation.Utility.PlayerStateType.IsRescuing:
                    return PlayerState.Rescuing;
                case Preparation.Utility.PlayerStateType.IsStunned:
                    return PlayerState.Stunned;
                case Preparation.Utility.PlayerStateType.IsSwinging:
                    return PlayerState.Swinging;
                case Preparation.Utility.PlayerStateType.IsTreated:
                    return PlayerState.Treated;
                case Preparation.Utility.PlayerStateType.IsTreating:
                    return PlayerState.Treating;
                case Preparation.Utility.PlayerStateType.IsTryingToAttack:
                    return PlayerState.Attacking;
                case Preparation.Utility.PlayerStateType.IsUsingSpecialSkill:
                    return PlayerState.UsingSpecialSkill;
                default:
                    return PlayerState.NullStatus;
            }
        }

        private static Protobuf.StudentBuffType ToStudentBuffType(Preparation.Utility.BuffType buffType)
        {
            switch (buffType)
            {
                case Preparation.Utility.BuffType.Null:
                default:
                    return Protobuf.StudentBuffType.NullSbuffType;
            }
        }

        private static Protobuf.BulletType ToBulletType(Preparation.Utility.BulletType bulletType)
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

        private static Protobuf.StudentType ToStudentType(Preparation.Utility.CharacterType characterType)
        {
            switch (characterType)
            {
                case Preparation.Utility.CharacterType.Athlete:
                    return Protobuf.StudentType.Athlete;
                default:
                    return Protobuf.StudentType.NullStudentType;
            }
        }
        private static Protobuf.TrickerType ToTrickerType(Preparation.Utility.CharacterType characterType)
        {
            switch (characterType)
            {
                case Preparation.Utility.CharacterType.Assassin:
                    return Protobuf.TrickerType.Assassin;
                default:
                    return Protobuf.TrickerType.NullTrickerType;
            }
        }

        public static MessageOfObj? Auto(GameObj gameObj)
        {
            if (gameObj.Type == Preparation.Utility.GameObjType.Character)
            {
                Character character = (Character)gameObj;
                if (character.IsGhost())
                    return Tricker((Ghost)character);
                else return Student((Student)character);
            }
            else if (gameObj.Type == Preparation.Utility.GameObjType.Bullet)
                return Bullet((Bullet)gameObj);
            else if (gameObj.Type == Preparation.Utility.GameObjType.Prop)
                return Prop((Prop)gameObj);
            else if (gameObj.Type == Preparation.Utility.GameObjType.BombedBullet)
                return BombedBullet((BombedBullet)gameObj);
            else if (gameObj.Type == Preparation.Utility.GameObjType.PickedProp)
                return PickedProp((PickedProp)gameObj);
            else return null;  //先写着防报错
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

            foreach (var Value in player.PropInventory)
                msg.StudentMessage.Prop.Add(ToPropType(Value.GetPropType()));

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
            msg.TrickerMessage.Place = ToPlaceType(player.Place);
            foreach (var Value in player.PropInventory)
                msg.StudentMessage.Prop.Add(ToPropType(Value.GetPropType()));

            msg.TrickerMessage.TrickerType = ToTrickerType(player.CharacterType); // 下面写
            msg.TrickerMessage.Guid = player.ID;
            msg.TrickerMessage.Score = player.Score;
            msg.TrickerMessage.PlayerId = player.PlayerID;
            msg.TrickerMessage.ViewRange = player.ViewRange;
            msg.TrickerMessage.Radius = player.Radius;
            //msg.TrickerMessage.Buff[0] = ButcherBuffType.NullSbuffType; 下面写了



            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                {
                    switch (kvp.Key) // ButcherBuffType具体内容待定
                    {
                        case Preparation.Utility.BuffType.Spear:
                            msg.TrickerMessage.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddLIFE:
                            msg.TrickerMessage.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.Shield:
                            msg.TrickerMessage.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddSpeed:
                            msg.TrickerMessage.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        default:
                            break;
                    }
                }
            }
            /*switch (player.Place)
            {
                case Preparation.Utility.PlaceType.Land:
                    msg.TrickerMessage.Place = PlaceType.Land;
                    break;
                case Preparation.Utility.PlaceType.Grass1:
                    msg.TrickerMessage.Place = PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass2:
                    msg.TrickerMessage.Place = PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass3:
                    msg.TrickerMessage.Place = PlaceType.Grass;
                    break;
                // case Preparation.Utility.PlaceType.Invisible:
                //     msg.TrickerMessage.MessageOfHuman.Place = Communication.Proto.PlaceType.Invisible;
                //     break;
                default:
                    msg.TrickerMessage.Place = PlaceType.NullPlaceType;
                    break;
            }*/

            //Character的储存方式可能得改，用enum type存道具和子弹，不应该用对象
            //现在懒得改了，有时间再重整一波
            /*if (player.PropInventory == null)
                msg.TrickerMessage.Prop = PropType.NullPropType;
            else
            {
                switch (player.PropInventory.GetPropType())
                {
                    case Preparation.Utility.PropType.Gem:
                        msg.TrickerMessage.Prop = PropType.NullPropType;
                        break;
                    case Preparation.Utility.PropType.addLIFE:
                        msg.TrickerMessage.MessageOfHuman.Prop = Communication.Proto.PropType.AddLife;
                        break;
                    case Preparation.Utility.PropType.addSpeed:
                        msg.TrickerMessage.MessageOfHuman.Prop = Communication.Proto.PropType.AddSpeed;
                        break;
                    case Preparation.Utility.PropType.Shield:
                        msg.TrickerMessage.MessageOfHuman.Prop = Communication.Proto.PropType.Shield;
                        break;
                    case Preparation.Utility.PropType.Spear:
                        msg.TrickerMessage.MessageOfHuman.Prop = Communication.Proto.PropType.Spear;
                        break;
                    default:
                        msg.TrickerMessage.Prop = PropType.NullPropType;
                        break;
                }
            }*/

            return msg;
        }

        private static MessageOfObj Bullet(Bullet bullet)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.BulletMessage = new();
            msg.BulletMessage.X = bullet.Position.x;
            msg.BulletMessage.Y = bullet.Position.y;
            //msg.BulletMessage.FacingDirection = bullet.FacingDirection; // XY转double?
            msg.BulletMessage.Guid = bullet.ID;
            msg.BulletMessage.Team = PlayerType.NullPlayerType;
            msg.BulletMessage.Place = PlaceType.NullPlaceType;
            msg.BulletMessage.BombRange = 0;
            switch (bullet.TypeOfBullet)
            {
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.BulletMessage.Type = BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.BulletMessage.Type = BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.BulletMessage.Type = BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.BulletMessage.Type = BulletType.LineBullet;
                    break;
                default:
                    msg.BulletMessage.Type = BulletType.NullBulletType;
                    break;
            }
            //if (bullet.Parent != null)
            //msg.BulletMessage.MessageOfBullet.ParentTeamID = bullet.Parent.TeamID;
            /*switch (bullet.Place)
            {
                case Preparation.Utility.PlaceType.Null:
                    msg.BulletMessage.MessageOfBullet.Place = Communication.Proto.PlaceType.Null;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.BulletMessage.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.BulletMessage.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.BulletMessage.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                default:
                    msg.BulletMessage.MessageOfBullet.Place = Communication.Proto.PlacccceType.NullPlaceType;
                    break;
            }*/
            return msg;
        }

        private static MessageOfObj Prop(Prop prop)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.PropMessage = new();
            //msg.PropMessage.Type = PropType.NullPropType; 下面写
            msg.PropMessage.X = prop.Position.x;
            msg.PropMessage.Y = prop.Position.y;
            msg.PropMessage.FacingDirection = 0;
            msg.PropMessage.Guid = 0;
            msg.PropMessage.Place = PlaceType.NullPlaceType;
            msg.PropMessage.Size = 0;
            msg.PropMessage.IsMoving = false;

            switch (prop.GetPropType())
            {
                /*case Preparation.Utility.PropType.Gem:
                    msg.PropMessage.Type = PropType.Gem;
                    break;
                case Preparation.Utility.PropType.addLIFE:
                    msg.PropMessage.Type = PropType.AddLife;
                    break;
                case Preparation.Utility.PropType.addSpeed:
                    msg.PropMessage.Type = PropType.AddSpeed;
                    break;
                case Preparation.Utility.PropType.Shield:
                    msg.PropMessage.Type = PropType.Shield;
                    break;
                case Preparation.Utility.PropType.Spear:
                    msg.PropMessage.Type = PropType.Spear;
                    break;*/
                default:
                    msg.PropMessage.Type = PropType.NullPropType;
                    break;
            }

            /*if(prop is Gem)
            {
                msg.PropMessage.MessageOfProp.Size = ((Gem)prop).Size;
            }
            else
            {
                msg.PropMessage.MessageOfProp.Size = 1;
            }
            switch (prop.Place)
            {
                case Preparation.Utility.PlaceType.Null:
                    msg.PropMessage.MessageOfProp.Place = Communication.Proto.PlaceType.Null;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.PropMessage.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.PropMessage.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.PropMessage.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                default:
                    msg.PropMessage.MessageOfProp.Place = Communication.Proto.PlacccceType.NullPlaceType;
                    break;
            }*/
            return msg;
        }

        private static MessageOfObj BombedBullet(BombedBullet bombedBullet)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.BombedBulletMessage = new();
            msg.BombedBulletMessage.X = bombedBullet.bulletHasBombed.Position.x;
            msg.BombedBulletMessage.Y = bombedBullet.bulletHasBombed.Position.y;
            //msg.BombedBulletMessage.FacingDirection = bombedBullet.FacingDirection; XY类型转double?
            msg.BombedBulletMessage.MappingId = bombedBullet.MappingID;
            msg.BombedBulletMessage.BombRange = BulletFactory.BulletRadius(bombedBullet.bulletHasBombed.TypeOfBullet); // 待确认
            switch (bombedBullet.bulletHasBombed.TypeOfBullet)
            {
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.BombedBulletMessage.Type = BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.BombedBulletMessage.Type = BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.BombedBulletMessage.Type = BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.BombedBulletMessage.Type = BulletType.LineBullet;
                    break;
                default:
                    msg.BombedBulletMessage.Type = BulletType.NullBulletType;
                    break;
            }
            return msg;
        }

        private static MessageOfObj PickedProp(PickedProp pickedProp)
        {
            MessageOfObj msg = new MessageOfObj(); // MessageOfObj中没有PickedProp
            /*msg.MessageOfPickedProp = new MessageOfPickedProp();

            msg.MessageOfPickedProp.MappingID = pickedProp.MappingID;
            msg.MessageOfPickedProp.X = pickedProp.PropHasPicked.Position.x;
            msg.MessageOfPickedProp.Y = pickedProp.PropHasPicked.Position.y;
            msg.MessageOfPickedProp.FacingDirection = pickedProp.PropHasPicked.FacingDirection;
            switch (pickedProp.PropHasPicked.GetPropType())
            {
                case Preparation.Utility.PropType.Gem:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.Gem;
                    break;
                case Preparation.Utility.PropType.addLIFE:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.AddLife;
                    break;
                case Preparation.Utility.PropType.addSpeed:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.AddSpeed;
                    break;
                case Preparation.Utility.PropType.Shield:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.Shield;
                    break;
                case Preparation.Utility.PropType.Spear:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.Spear;
                    break;
                default:
                    msg.MessageOfPickedProp.Type = Communication.Proto.PropType.NullPropType;
                    break;
            }*/
            return msg;
        }
    }
}
