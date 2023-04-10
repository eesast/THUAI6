using Protobuf;
using System.Collections.Generic;
using GameClass.GameObj;
using System.Numerics;
using Preparation.Utility;
using Gaming;

namespace Server
{

    public static class CopyInfo
    {
        public static MessageOfObj? Auto(GameObj gameObj, int time)
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
                    return Chest((Chest)gameObj, time);
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
                msg.StudentMessage.Prop.Add(Transformation.ToPropType(value.GetPropType()));

            msg.StudentMessage.Place = Transformation.ToPlaceType((Preparation.Utility.PlaceType)player.Place);
            msg.StudentMessage.Guid = player.ID;

            msg.StudentMessage.PlayerState = Transformation.ToPlayerState((PlayerStateType)player.PlayerState);
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
                    msg.StudentMessage.Buff.Add(Transformation.ToStudentBuffType(kvp.Key));
            }

            msg.StudentMessage.BulletType = Transformation.ToBulletType((Preparation.Utility.BulletType)player.BulletOfPlayer);
            msg.StudentMessage.LearningSpeed = player.FixSpeed;
            msg.StudentMessage.TreatSpeed = player.TreatSpeed;
            msg.StudentMessage.FacingDirection = player.FacingDirection.Angle();
            msg.StudentMessage.StudentType = Transformation.ToStudentType(player.CharacterType);
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

            msg.TrickerMessage.Place = Transformation.ToPlaceType((Preparation.Utility.PlaceType)player.Place);
            foreach (var value in player.PropInventory)
                msg.TrickerMessage.Prop.Add(Transformation.ToPropType(value.GetPropType()));

            msg.TrickerMessage.TrickerType = Transformation.ToTrickerType(player.CharacterType);
            msg.TrickerMessage.Guid = player.ID;
            msg.TrickerMessage.Score = player.Score;
            msg.TrickerMessage.PlayerId = player.PlayerID;
            msg.TrickerMessage.ViewRange = player.ViewRange;
            msg.TrickerMessage.Radius = player.Radius;
            msg.TrickerMessage.PlayerState = Transformation.ToPlayerState((PlayerStateType)player.PlayerState);
            msg.TrickerMessage.TrickDesire = (player.BgmDictionary.ContainsKey(BgmType.StudentIsApproaching)) ? player.BgmDictionary[BgmType.StudentIsApproaching] : 0;
            msg.TrickerMessage.ClassVolume = (player.BgmDictionary.ContainsKey(BgmType.GeneratorIsBeingFixed)) ? player.BgmDictionary[BgmType.GeneratorIsBeingFixed] : 0;
            msg.TrickerMessage.FacingDirection = player.FacingDirection.Angle();
            msg.TrickerMessage.BulletType = Transformation.ToBulletType((Preparation.Utility.BulletType)player.BulletOfPlayer);
            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                    msg.TrickerMessage.Buff.Add(Transformation.ToTrickerBuffType(kvp.Key));
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
            msg.BulletMessage.Place = Transformation.ToPlaceType((Preparation.Utility.PlaceType)bullet.Place);
            msg.BulletMessage.BombRange = bullet.BulletBombRange;
            msg.BulletMessage.Speed = bullet.Speed;
            return msg;
        }

        private static MessageOfObj Prop(Prop prop)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.PropMessage = new();
            msg.PropMessage.Type = Transformation.ToPropType(prop.GetPropType());
            msg.PropMessage.X = prop.Position.x;
            msg.PropMessage.Y = prop.Position.y;
            msg.PropMessage.FacingDirection = prop.FacingDirection.Angle();
            msg.PropMessage.Guid = prop.ID;
            msg.PropMessage.Place = Transformation.ToPlaceType((Preparation.Utility.PlaceType)prop.Place);
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
        private static MessageOfObj Chest(Chest chest, int time)
        {
            MessageOfObj msg = new MessageOfObj();
            msg.ChestMessage = new();
            msg.ChestMessage.X = chest.Position.x;
            msg.ChestMessage.Y = chest.Position.y;
            int progress = (chest.OpenStartTime > 0) ? ((time - chest.OpenStartTime) * chest.WhoOpen.SpeedOfOpenChest) : 0;
            msg.ChestMessage.Progress = (progress > GameData.degreeOfOpenedChest) ? GameData.degreeOfOpenedChest : progress;
            return msg;
        }
    }
}
