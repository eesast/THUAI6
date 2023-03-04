using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;

namespace Server
{

    public static class CopyInfo
    {
        // 下面赋值为0的大概率是还没写完 2023-03-03
        /*public static MessageOfObj? Auto(GameObj gameObj)
        {
            if (gameObj.Type == Preparation.Utility.GameObjType.Character)
            {
                Character character = (Character)gameObj;
                if (character.IsGhost())
                    return Tri
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
        }*/

        private static MessageOfStudent? Student(Character player)
        {
            MessageOfStudent msg = new MessageOfStudent();
            if (player.IsGhost()) return null;

            msg.X = player.Position.x;
            msg.Y = player.Position.y;
            msg.Speed = player.MoveSpeed;
            msg.Determination = player.HP;
            msg.FailNum = 0;
            msg.TimeUntilSkillAvailable = 0;
            msg.StudentType = StudentType.NullStudentType; // 下面写
            msg.Guid = 0;
            msg.State = StudentState.NullStatus;
            msg.FailTime = 0;
            msg.EmoTime = 0;
            msg.PlayerId = 0;
            msg.ViewRange = 0;
            msg.Radius = 0;
            

            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                {
                    switch (kvp.Key) // StudentBuffType具体内容待定
                    {
                        case Preparation.Utility.BuffType.Spear:
                            msg.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddLIFE:
                            msg.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.Shield:
                            msg.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddSpeed:
                            msg.Buff.Add(StudentBuffType.NullSbuffType);
                            break;
                        default:
                            break;
                    }
                }
            }
            switch (player.Place)
            {
                case Preparation.Utility.PlaceType.EmergencyExit:
                    msg.Place = PlaceType.HiddenGate;
                    break;
                case Preparation.Utility.PlaceType.Doorway:
                    msg.Place = PlaceType.Gate;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.Place = PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.BirthPoint1:
                case Preparation.Utility.PlaceType.BirthPoint2:
                case Preparation.Utility.PlaceType.BirthPoint3:
                case Preparation.Utility.PlaceType.BirthPoint4:
                case Preparation.Utility.PlaceType.BirthPoint5:
                case Preparation.Utility.PlaceType.Null:
                    msg.Place = PlaceType.Land;
                    break;
                // case Preparation.Utility.PlaceType.Invisible:
                //     msg.MessageOfHuman.Place = Communication.Proto.PlaceType.Invisible;
                //     break;
                default:
                    msg.Place = PlaceType.NullPlaceType;
                    break;
            }

            //Character的储存方式可能得改，用enum type存道具和子弹，不应该用对象
            //现在懒得改了，有时间再重整一波
            if (player.PropInventory == null)
                msg.Prop.Add(PropType.NullPropType);
            else
            {
                switch (player.PropInventory.GetPropType())
                {
                    case Preparation.Utility.PropType.Gem:
                        msg.Prop.Add(PropType.NullPropType);
                        break;
                    /*case Preparation.Utility.PropType.addLIFE:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.AddLife;
                        break;
                    case Preparation.Utility.PropType.addSpeed:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.AddSpeed;
                        break;
                    case Preparation.Utility.PropType.Shield:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.Shield;
                        break;
                    case Preparation.Utility.PropType.Spear:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.Spear;
                        break;
                    default:
                        msg.Prop = PropType.NullPropType;
                        break;*/
                }
            }
            /*switch (player.PassiveSkillType) 需要对接一下，proto里似乎没有这个
            {
                case Preparation.Utility.PassiveSkillType.RecoverAfterBattle:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.RecoverAfterBattle;
                    break;
                case Preparation.Utility.PassiveSkillType.SpeedUpWhenLeavingGrass:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.SpeedUpWhenLeavingGrass;
                    break;
                case Preparation.Utility.PassiveSkillType.Vampire:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.Vampire;
                    break;
                default:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.NullPassiveSkillType;
                    break;
            }

            switch (player.CommonSkillType)
            {
                case Preparation.Utility.ActiveSkillType.BecomeAssassin:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeAssassin;
                    break;
                case Preparation.Utility.ActiveSkillType.BecomeVampire:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeVampire;
                    break;
                case Preparation.Utility.ActiveSkillType.NuclearWeapon:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.NuclearWeapon;
                    break;
                case Preparation.Utility.ActiveSkillType.SuperFast:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.SuperFast;
                    break;
                default:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.NullActiveSkillType;
                    break;
            }

            switch (player.BulletOfPlayer)
            {
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.LineBullet;
                    break;
                default:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.NullBulletType;
                    break;
            }*/

            return msg;
        }

        private static MessageOfTricker Butcher(Character player)
        {
            MessageOfTricker msg = new MessageOfTricker();
            if (!player.IsGhost()) return null;

            msg.X = player.Position.x;
            msg.Y = player.Position.y;
            msg.Speed = player.MoveSpeed;
            msg.Damage = 0;
            msg.TimeUntilSkillAvailable = 0;
            //msg.Place = 0; 下面写了
            //msg.Prop = PropType.NullPropType; // 下面写
            msg.TrickerType = TrickerType.NullTrickerType; // 下面写
            msg.Guid = 0;
            msg.Movable = false;
            msg.PlayerId = 0;
            msg.ViewRange = 0;
            msg.Radius = 0;
            //msg.Buff[0] = ButcherBuffType.NullSbuffType; 下面写了

            /* THUAI5中的内容
            msg.BulletNum = player.BulletNum;
            msg.CanMove = player.CanMove;
            msg.CD = player.CD;
            msg.GemNum = player.GemNum;
            msg.Guid = player.ID;
            msg.IsResetting = player.IsResetting;
            
            msg.LifeNum = player.DeathCount + 1;
            msg.Radius = player.Radius;
            
            msg.TimeUntilCommonSkillAvailable = player.TimeUntilCommonSkillAvailable;
            msg.TeamID = player.TeamID;
            msg.PlayerID = player.PlayerID;
            msg.IsInvisible = player.IsInvisible;
            msg.FacingDirection = player.FacingDirection;

            //应该要发队伍分数，这里先发个人分数
            msg.MessageOfHuman.Score = player.Score;

            //这条暂时没啥用
            msg.MessageOfHuman.TimeUntilUltimateSkillAvailable = 0;

            msg.MessageOfHuman.Vampire = player.Vampire;*/

            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                {
                    switch (kvp.Key) // ButcherBuffType具体内容待定
                    {
                        case Preparation.Utility.BuffType.Spear:
                            msg.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddLIFE:
                            msg.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.Shield:
                            msg.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        case Preparation.Utility.BuffType.AddSpeed:
                            msg.Buff.Add(TrickerBuffType.NullTbuffType);
                            break;
                        default:
                            break;
                    }
                }
            }
            /*switch (player.Place)
            {
                case Preparation.Utility.PlaceType.Land:
                    msg.Place = PlaceType.Land;
                    break;
                case Preparation.Utility.PlaceType.Grass1:
                    msg.Place = PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass2:
                    msg.Place = PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass3:
                    msg.Place = PlaceType.Grass;
                    break;
                // case Preparation.Utility.PlaceType.Invisible:
                //     msg.MessageOfHuman.Place = Communication.Proto.PlaceType.Invisible;
                //     break;
                default:
                    msg.Place = PlaceType.NullPlaceType;
                    break;
            }*/

            //Character的储存方式可能得改，用enum type存道具和子弹，不应该用对象
            //现在懒得改了，有时间再重整一波
            /*if (player.PropInventory == null)
                msg.Prop = PropType.NullPropType;
            else
            {
                switch (player.PropInventory.GetPropType())
                {
                    case Preparation.Utility.PropType.Gem:
                        msg.Prop = PropType.NullPropType;
                        break;
                    case Preparation.Utility.PropType.addLIFE:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.AddLife;
                        break;
                    case Preparation.Utility.PropType.addSpeed:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.AddSpeed;
                        break;
                    case Preparation.Utility.PropType.Shield:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.Shield;
                        break;
                    case Preparation.Utility.PropType.Spear:
                        msg.MessageOfHuman.Prop = Communication.Proto.PropType.Spear;
                        break;
                    default:
                        msg.Prop = PropType.NullPropType;
                        break;
                }
            }*/
            /*switch (player.PassiveSkillType) 需要对接一下，proto里似乎没有这个
            {
                case Preparation.Utility.PassiveSkillType.RecoverAfterBattle:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.RecoverAfterBattle;
                    break;
                case Preparation.Utility.PassiveSkillType.SpeedUpWhenLeavingGrass:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.SpeedUpWhenLeavingGrass;
                    break;
                case Preparation.Utility.PassiveSkillType.Vampire:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.Vampire;
                    break;
                default:
                    msg.MessageOfHuman.PassiveSkillType = Communication.Proto.PassiveSkillType.NullPassiveSkillType;
                    break;
            }

            switch (player.CommonSkillType)
            {
                case Preparation.Utility.ActiveSkillType.BecomeAssassin:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeAssassin;
                    break;
                case Preparation.Utility.ActiveSkillType.BecomeVampire:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeVampire;
                    break;
                case Preparation.Utility.ActiveSkillType.NuclearWeapon:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.NuclearWeapon;
                    break;
                case Preparation.Utility.ActiveSkillType.SuperFast:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.SuperFast;
                    break;
                default:
                    msg.MessageOfHuman.ActiveSkillType = Communication.Proto.ActiveSkillType.NullActiveSkillType;
                    break;
            }

            switch (player.BulletOfPlayer)
            {
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.LineBullet;
                    break;
                default:
                    msg.MessageOfHuman.BulletType = Communication.Proto.BulletType.NullBulletType;
                    break;
            }*/

            return msg;
        }

        /*private static MessageToClient.Types.GameObjMessage Bullet(Bullet bullet)
        {
            MessageToClient.Types.GameObjMessage msg = new MessageToClient.Types.GameObjMessage();
            msg.MessageOfBullet = new MessageOfBullet();
            msg.MessageOfBullet.FacingDirection = bullet.FacingDirection;
            msg.MessageOfBullet.Guid = bullet.ID;
            msg.MessageOfBullet.BombRange = BulletFactory.BulletBombRange(bullet.TypeOfBullet);
            switch (bullet.TypeOfBullet)
            {
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.MessageOfBullet.Type = Communication.Proto.BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.MessageOfBullet.Type = Communication.Proto.BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.MessageOfBullet.Type = Communication.Proto.BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.MessageOfBullet.Type = Communication.Proto.BulletType.LineBullet;
                    break;
                default:
                    msg.MessageOfBullet.Type = Communication.Proto.BulletType.NullBulletType;
                    break;
            }
            msg.MessageOfBullet.X = bullet.Position.x;
            msg.MessageOfBullet.Y = bullet.Position.y;
            if (bullet.Parent != null)
                msg.MessageOfBullet.ParentTeamID = bullet.Parent.TeamID;
            switch (bullet.Place)
            {
                case Preparation.Utility.PlaceType.Null:
                    msg.MessageOfBullet.Place = Communication.Proto.PlaceType.Null;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfBullet.Place = Communication.Proto.PlaceType.Grass;
                    break;
                default:
                    msg.MessageOfBullet.Place = Communication.Proto.PlacccceType.NullPlaceType;
                    break;
            }
            return msg;
        }*/

        private static MessageOfProp Prop(Prop prop)
        {
            MessageOfProp msg = new MessageOfProp();
            //msg.Type = PropType.NullPropType; 下面写
            msg.X = prop.Position.x;
            msg.Y = prop.Position.y;
            msg.FacingDirection = 0;
            msg.Guid = 0;
            msg.Place = PlaceType.NullPlaceType;
            msg.Size = 0;
            msg.IsMoving = false;
            /* THUAI5中的内容
            msg.MessageOfProp.FacingDirection = prop.FacingDirection;
            msg.MessageOfProp.Guid = prop.ID;
            msg.MessageOfProp.IsMoving = prop.IsMoving;*/

            switch (prop.GetPropType())
            {
                /*case Preparation.Utility.PropType.Gem:
                    msg.Type = PropType.Gem;
                    break;
                case Preparation.Utility.PropType.addLIFE:
                    msg.Type = PropType.AddLife;
                    break;
                case Preparation.Utility.PropType.addSpeed:
                    msg.Type = PropType.AddSpeed;
                    break;
                case Preparation.Utility.PropType.Shield:
                    msg.Type = PropType.Shield;
                    break;
                case Preparation.Utility.PropType.Spear:
                    msg.Type = PropType.Spear;
                    break;*/
                default:
                    msg.Type = PropType.NullPropType;
                    break;
            }

            /*if(prop is Gem)
            {
                msg.MessageOfProp.Size = ((Gem)prop).Size;
            }
            else
            {
                msg.MessageOfProp.Size = 1;
            }
            switch (prop.Place)
            {
                case Preparation.Utility.PlaceType.Null:
                    msg.MessageOfProp.Place = Communication.Proto.PlaceType.Null;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfProp.Place = Communication.Proto.PlaceType.Grass;
                    break;
                default:
                    msg.MessageOfProp.Place = Communication.Proto.PlacccceType.NullPlaceType;
                    break;
            }*/
            return msg;
        }

        /*private static MessageOfBombedBullet BombedBullet(BombedBullet bombedBullet)
        {
            MessageOfBombedBullet msg = new MessageOfBombedBullet;

            msg.FacingDirection = bombedBullet.FacingDirection;
            msg.X = bombedBullet.bulletHasBombed.Position.x;
            msg.Y = bombedBullet.bulletHasBombed.Position.y;
            msg.MappingID = bombedBullet.MappingID;
            msg.BombRange = BulletFactory.BulletBombRange(bombedBullet.bulletHasBombed.TypeOfBullet);
            switch (bombedBullet.bulletHasBombed.TypeOfBullet)
            {
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.MessageOfBombedBullet.Type = Communication.Proto.BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.MessageOfBombedBullet.Type = Communication.Proto.BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.MessageOfBombedBullet.Type = Communication.Proto.BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.MessageOfBombedBullet.Type = Communication.Proto.BulletType.LineBullet;
                    break;
                default:
                    msg.MessageOfBombedBullet.Type = Communication.Proto.BulletType.NullBulletType;
                    break;
            }
            return msg;
        }*/

        /*private static MessageToClient.Types.GameObjMessage PickedProp(PickedProp pickedProp)
        {
            MessageToClient.Types.GameObjMessage msg = new MessageToClient.Types.GameObjMessage();
            msg.MessageOfPickedProp = new MessageOfPickedProp();

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
            }
            return msg;
        }*/
    }
}
