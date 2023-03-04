using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;
/*
namespace Server
{
    public static class CopyInfo
    {
        public static MessageToClient.Types.GameObjMessage? Auto(GameObj gameObj)
        {
            if (gameObj.Type == Preparation.Utility.GameObjType.Character)
                return Player((Character)gameObj);
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

        private static MessageToClient.Types.GameObjMessage Player(Character player)
        {
            MessageToClient.Types.GameObjMessage msg = new MessageToClient.Types.GameObjMessage();
            msg.MessageOfCharacter = new MessageOfCharacter();

            msg.MessageOfCharacter.X = player.Position.x;
            msg.MessageOfCharacter.Y = player.Position.y;
            msg.MessageOfCharacter.AttackRange = player.AttackRange;
            msg.MessageOfCharacter.BulletNum = player.BulletNum;
            msg.MessageOfCharacter.CanMove = player.CanMove;
            msg.MessageOfCharacter.CD = player.CD;
            msg.MessageOfCharacter.GemNum = player.GemNum;
            msg.MessageOfCharacter.Guid = player.ID;
            msg.MessageOfCharacter.IsResetting = player.IsResetting;
            msg.MessageOfCharacter.Life = player.HP;
            msg.MessageOfCharacter.LifeNum = player.DeathCount + 1;
            msg.MessageOfCharacter.Radius = player.Radius;
            msg.MessageOfCharacter.Speed = player.MoveSpeed;
            msg.MessageOfCharacter.TimeUntilCommonSkillAvailable = player.TimeUntilCommonSkillAvailable;
            msg.MessageOfCharacter.TeamID = player.TeamID;
            msg.MessageOfCharacter.PlayerID = player.PlayerID;
            msg.MessageOfCharacter.IsInvisible = player.IsInvisible;
            msg.MessageOfCharacter.FacingDirection = player.FacingDirection;

            //应该要发队伍分数，这里先发个人分数
            msg.MessageOfCharacter.Score = player.Score;

            //这条暂时没啥用
            msg.MessageOfCharacter.TimeUntilUltimateSkillAvailable = 0;

            msg.MessageOfCharacter.Vampire = player.Vampire;
            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                {
                    switch(kvp.Key)
                    {
                        case Preparation.Utility.BuffType.Spear:
                            msg.MessageOfCharacter.Buff.Add(BuffType.SpearBuff);
                            break;
                        case Preparation.Utility.BuffType.AddLIFE:
                            msg.MessageOfCharacter.Buff.Add(BuffType.AddLife);
                            break;
                        case Preparation.Utility.BuffType.Shield:
                            msg.MessageOfCharacter.Buff.Add(BuffType.ShieldBuff);
                            break;
                        case Preparation.Utility.BuffType.AddSpeed:
                            msg.MessageOfCharacter.Buff.Add(BuffType.MoveSpeed);
                            break;
                        default:
                            break;
                    }
                }
            }
            switch (player.Place)
            {
                case Preparation.Utility.PlaceType.Null:
                    msg.MessageOfCharacter.Place = Communication.Proto.PlaceType.Null;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfCharacter.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfCharacter.Place = Communication.Proto.PlaceType.Grass;
                    break;
                case Preparation.Utility.PlaceType.Grass:
                    msg.MessageOfCharacter.Place = Communication.Proto.PlaceType.Grass;
                    break;
               // case Preparation.Utility.PlacccceType.Invisible:
               //     msg.MessageOfCharacter.Place = Communication.Proto.PlacccceType.Invisible;
               //     break;
                default:
                    msg.MessageOfCharacter.Place = Communication.Proto.PlacccceType.NullPlaceType;
                    break;
            }

            //Character的储存方式可能得改，用enum type存道具和子弹，不应该用对象
            //现在懒得改了，有时间再重整一波
            if (player.PropInventory == null)
                msg.MessageOfCharacter.Prop = Communication.Proto.PropType.NullPropType;
            else
            {
                switch (player.PropInventory.GetPropType())
                {
                    case Preparation.Utility.PropType.Gem:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.Gem;
                        break;
                    case Preparation.Utility.PropType.addLIFE:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.AddLife;
                        break;
                    case Preparation.Utility.PropType.addSpeed:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.AddSpeed;
                        break;
                    case Preparation.Utility.PropType.Shield:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.Shield;
                        break;
                    case Preparation.Utility.PropType.Spear:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.Spear;
                        break;
                    default:
                        msg.MessageOfCharacter.Prop = Communication.Proto.PropType.NullPropType;
                        break;
                }
            }
            switch (player.PassiveSkillType)
            {
                case Preparation.Utility.PassiveSkillType.RecoverAfterBattle:
                    msg.MessageOfCharacter.PassiveSkillType = Communication.Proto.PassiveSkillType.RecoverAfterBattle;
                    break;
                case Preparation.Utility.PassiveSkillType.SpeedUpWhenLeavingGrass:
                    msg.MessageOfCharacter.PassiveSkillType = Communication.Proto.PassiveSkillType.SpeedUpWhenLeavingGrass;
                    break;
                case Preparation.Utility.PassiveSkillType.Vampire:
                    msg.MessageOfCharacter.PassiveSkillType = Communication.Proto.PassiveSkillType.Vampire;
                    break;
                default:
                    msg.MessageOfCharacter.PassiveSkillType = Communication.Proto.PassiveSkillType.NullPassiveSkillType;
                    break;
            }
            switch (player.CommonSkillType)
            {
                case Preparation.Utility.ActiveSkillType.BecomeAssassin:
                    msg.MessageOfCharacter.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeAssassin;
                    break;
                case Preparation.Utility.ActiveSkillType.BecomeVampire:
                    msg.MessageOfCharacter.ActiveSkillType = Communication.Proto.ActiveSkillType.BecomeVampire;
                    break;
                case Preparation.Utility.ActiveSkillType.NuclearWeapon:
                    msg.MessageOfCharacter.ActiveSkillType = Communication.Proto.ActiveSkillType.NuclearWeapon;
                    break;
                case Preparation.Utility.ActiveSkillType.SuperFast:
                    msg.MessageOfCharacter.ActiveSkillType = Communication.Proto.ActiveSkillType.SuperFast;
                    break;
                default:
                    msg.MessageOfCharacter.ActiveSkillType = Communication.Proto.ActiveSkillType.NullActiveSkillType;
                    break;
            }

            switch (player.BulletOfPlayer)
            {
                case Preparation.Utility.BulletType.AtomBomb:
                    msg.MessageOfCharacter.BulletType = Communication.Proto.BulletType.AtomBomb;
                    break;
                case Preparation.Utility.BulletType.OrdinaryBullet:
                    msg.MessageOfCharacter.BulletType = Communication.Proto.BulletType.OrdinaryBullet;
                    break;
                case Preparation.Utility.BulletType.FastBullet:
                    msg.MessageOfCharacter.BulletType = Communication.Proto.BulletType.FastBullet;
                    break;
                case Preparation.Utility.BulletType.LineBullet:
                    msg.MessageOfCharacter.BulletType = Communication.Proto.BulletType.LineBullet;
                    break;
                default:
                    msg.MessageOfCharacter.BulletType = Communication.Proto.BulletType.NullBulletType;
                    break;
            }

            return msg;
        }

        private static MessageToClient.Types.GameObjMessage Bullet(Bullet bullet)
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
        }

        private static MessageToClient.Types.GameObjMessage Prop(Prop prop)
        {
            MessageToClient.Types.GameObjMessage msg = new MessageToClient.Types.GameObjMessage();
            msg.MessageOfProp = new MessageOfProp();
            msg.MessageOfProp.FacingDirection = prop.FacingDirection;
            msg.MessageOfProp.Guid = prop.ID;
            msg.MessageOfProp.IsMoving = prop.IsMoving;
            switch (prop.GetPropType())
            {
                case Preparation.Utility.PropType.Gem:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.Gem;
                    break;
                case Preparation.Utility.PropType.addLIFE:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.AddLife;
                    break;
                case Preparation.Utility.PropType.addSpeed:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.AddSpeed;
                    break;
                case Preparation.Utility.PropType.Shield:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.Shield;
                    break;
                case Preparation.Utility.PropType.Spear:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.Spear;
                    break;
                default:
                    msg.MessageOfProp.Type = Communication.Proto.PropType.NullPropType;
                    break;
            }
            msg.MessageOfProp.X = prop.Position.x;
            msg.MessageOfProp.Y = prop.Position.y;
            if(prop is Gem)
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
            }
            return msg;
        }

        private static MessageToClient.Types.GameObjMessage BombedBullet(BombedBullet bombedBullet)
        {
            MessageToClient.Types.GameObjMessage msg = new MessageToClient.Types.GameObjMessage();
            msg.MessageOfBombedBullet = new MessageOfBombedBullet();

            msg.MessageOfBombedBullet.FacingDirection = bombedBullet.bulletHasBombed.FacingDirection;
            msg.MessageOfBombedBullet.X = bombedBullet.bulletHasBombed.Position.x;
            msg.MessageOfBombedBullet.Y = bombedBullet.bulletHasBombed.Position.y;
            msg.MessageOfBombedBullet.MappingID = bombedBullet.MappingID;
            msg.MessageOfBombedBullet.BombRange = BulletFactory.BulletBombRange(bombedBullet.bulletHasBombed.TypeOfBullet);
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
        }

        private static MessageToClient.Types.GameObjMessage PickedProp(PickedProp pickedProp)
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
        }
    }
}*/
// 等Preparation完成再写
