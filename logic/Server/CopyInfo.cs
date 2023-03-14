using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;

namespace Server
{

    public static class CopyInfo
    {
        // 下面赋值为0的大概率是还没写完 2023-03-03
        public static MessageOfObj? Auto(GameObj gameObj)
        {
            if (gameObj.Type == Preparation.Utility.GameObjType.Character)
            {
                Character character = (Character)gameObj;
                if (character.IsGhost())
                    return Tricker((Character)character);
                else return Student((Character)character);
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

        private static MessageOfObj? Student(Character player)
        {
            MessageOfObj msg = new MessageOfObj();
            if (player.IsGhost()) return null;
            msg.StudentMessage = new();

            msg.StudentMessage.X = player.Position.x;
            msg.StudentMessage.Y = player.Position.y;
            msg.StudentMessage.Speed = player.MoveSpeed;
            msg.StudentMessage.Determination = player.HP;
            //msg.StudentMessage.FailNum = 0;
            foreach (var keyValue in player.TimeUntilActiveSkillAvailable)
                msg.StudentMessage.TimeUntilSkillAvailable.Add(keyValue.Value);
            //msg.StudentMessage.StudentType; // 下面写
            msg.StudentMessage.Guid = player.ID;
            // msg.StudentMessage.State = player.PlayerState;
            msg.StudentMessage.FailTime = 0;
            msg.StudentMessage.EmoTime = 0;
            msg.StudentMessage.PlayerId = 0;
            msg.StudentMessage.ViewRange = 0;
            msg.StudentMessage.Radius = 0;
            msg.StudentMessage.Damage = 0;
            msg.StudentMessage.DangerAlert = 0;
            msg.StudentMessage.Score = 0;
            msg.StudentMessage.TreatProgress = 0;
            msg.StudentMessage.RescueProgress = 0;

            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                {
                    switch (kvp.Key) // StudentBuffType具体内容待定
                    {
                        case Preparation.Utility.BuffType.Spear:
                            msg.StudentMessage.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddLIFE:
                            msg.StudentMessage.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.Shield:
                            msg.StudentMessage.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddSpeed:
                            msg.StudentMessage.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        default:
                            break;
                    }
                }
            }

            //Character的储存方式可能得改，用enum type存道具和子弹，不应该用对象
            //现在懒得改了，有时间再重整一波
            if (player.PropInventory == null)
                msg.StudentMessage.Prop.Add(PropType.NullPropType);
            else
            {
                switch (player.PropInventory.GetPropType())
                {
                    case Preparation.Utility.PropType.Gem:
                        msg.StudentMessage.Prop.Add(PropType.NullPropType);
                        break;
                        /*case Preparation.Utility.PropType.addLIFE:
                           msg.StudentMessage.MessageOfHuman.Prop = Communication.Proto.PropType.AddLife;
                            break;
                        case Preparation.Utility.PropType.addSpeed:
                           msg.StudentMessage.MessageOfHuman.Prop = Communication.Proto.PropType.AddSpeed;
                            break;
                        case Preparation.Utility.PropType.Shield:
                           msg.StudentMessage.MessageOfHuman.Prop = Communication.Proto.PropType.Shield;
                            break;
                        case Preparation.Utility.PropType.Spear:
                           msg.StudentMessage.MessageOfHuman.Prop = Communication.Proto.PropType.Spear;
                            break;
                        default:
                           msg.StudentMessage.Prop = PropType.NullPropType;
                            break;*/
                }
            }

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
            msg.TrickerMessage.Damage = 0;
            msg.TrickerMessage.TimeUntilSkillAvailable = 0;
            //msg.TrickerMessage.Place = 0; 下面写了
            //msg.TrickerMessage.Prop = PropType.NullPropType; // 下面写
            msg.TrickerMessage.TrickerType = TrickerType.NullTrickerType; // 下面写
            msg.TrickerMessage.Guid = 0;
            msg.TrickerMessage.Movable = false;
            msg.TrickerMessage.PlayerId = 0;
            msg.TrickerMessage.ViewRange = 0;
            msg.TrickerMessage.Radius = 0;
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
