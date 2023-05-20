using Protobuf;
using GameClass.GameObj;
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
                case Preparation.Utility.GameObjType.BombedBullet:
                    return BombedBullet((BombedBullet)gameObj);
                case Preparation.Utility.GameObjType.Generator:
                    return Classroom((Generator)gameObj);
                case Preparation.Utility.GameObjType.Chest:
                    return Chest((Chest)gameObj, time);
                case Preparation.Utility.GameObjType.Doorway:
                    return Gate((Doorway)gameObj, time);
                case Preparation.Utility.GameObjType.EmergencyExit:
                    if (((EmergencyExit)gameObj).CanOpen)
                        return HiddenGate((EmergencyExit)gameObj);
                    else return null;
                case Preparation.Utility.GameObjType.Door:
                    return Door((Door)gameObj);
                case GameObjType.Item:
                    return Prop((Item)gameObj);
                case Preparation.Utility.GameObjType.Gadget:
                    return Prop((Gadget)gameObj);
                default: return null;
            }
        }
        public static MessageOfObj? Auto(MessageOfNews news)
        {
            MessageOfObj objMsg = new()
            {
                NewsMessage = news
            };
            return objMsg;
        }

        private static MessageOfObj? Student(Student player)
        {
            if (player.IsGhost()) return null;
            MessageOfObj msg = new()
            {
                StudentMessage = new()
                {
                    X = player.Position.x,
                    Y = player.Position.y,
                    Speed = player.MoveSpeed,
                    Determination = (int)player.HP,
                    Addiction = player.GamingAddiction,
                    Guid = player.ID,

                    PlayerState = Transformation.ToPlayerState((PlayerStateType)player.PlayerState),
                    PlayerId = player.PlayerID,
                    ViewRange = player.ViewRange,
                    Radius = player.Radius,
                    DangerAlert = (player.BgmDictionary.ContainsKey(BgmType.GhostIsComing)) ? player.BgmDictionary[BgmType.GhostIsComing] : 0,
                    Score = (int)player.Score,
                    TreatProgress = player.DegreeOfTreatment,
                    RescueProgress = player.TimeOfRescue,

                    BulletType = Transformation.ToBulletType((Preparation.Utility.BulletType)player.BulletOfPlayer),
                    LearningSpeed = player.FixSpeed,
                    TreatSpeed = player.TreatSpeed,
                    FacingDirection = player.FacingDirection.Angle(),
                    StudentType = Transformation.ToStudentType(player.CharacterType)
                }
            };

            foreach (var keyValue in player.ActiveSkillDictionary)
                msg.StudentMessage.TimeUntilSkillAvailable.Add(keyValue.Value.TimeUntilActiveSkillAvailable);
            for (int i = 0; i < GameData.maxNumOfSkill - player.ActiveSkillDictionary.Count; ++i)
                msg.StudentMessage.TimeUntilSkillAvailable.Add(-1);

            foreach (var value in player.PropInventory)
                msg.StudentMessage.Prop.Add(Transformation.ToPropType(value.GetPropType()));

            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                    msg.StudentMessage.Buff.Add(Transformation.ToStudentBuffType(kvp.Key));
            }

            return msg;
        }

        private static MessageOfObj? Tricker(Character player)
        {
            if (!player.IsGhost()) return null;
            MessageOfObj msg = new()
            {
                TrickerMessage = new()
                {
                    X = player.Position.x,
                    Y = player.Position.y,
                    Speed = (int)player.MoveSpeed,

                    TrickerType = Transformation.ToTrickerType(player.CharacterType),
                    Guid = player.ID,
                    Score = (int)player.Score,
                    PlayerId = player.PlayerID,
                    ViewRange = player.ViewRange,
                    Radius = player.Radius,
                    PlayerState = Transformation.ToPlayerState((PlayerStateType)player.PlayerState),
                    TrickDesire = (player.BgmDictionary.ContainsKey(BgmType.StudentIsApproaching)) ? player.BgmDictionary[BgmType.StudentIsApproaching] : 0,
                    ClassVolume = (player.BgmDictionary.ContainsKey(BgmType.GeneratorIsBeingFixed)) ? player.BgmDictionary[BgmType.GeneratorIsBeingFixed] : 0,
                    FacingDirection = player.FacingDirection.Angle(),
                    BulletType = Transformation.ToBulletType((Preparation.Utility.BulletType)player.BulletOfPlayer)
                }
            };
            foreach (var keyValue in player.ActiveSkillDictionary)
                msg.TrickerMessage.TimeUntilSkillAvailable.Add(keyValue.Value.TimeUntilActiveSkillAvailable);
            for (int i = 0; i < GameData.maxNumOfSkill - player.ActiveSkillDictionary.Count; ++i)
                msg.TrickerMessage.TimeUntilSkillAvailable.Add(-1);

            foreach (var value in player.PropInventory)
                msg.TrickerMessage.Prop.Add(Transformation.ToPropType(value.GetPropType()));
            foreach (KeyValuePair<Preparation.Utility.BuffType, bool> kvp in player.Buff)
            {
                if (kvp.Value)
                    msg.TrickerMessage.Buff.Add(Transformation.ToTrickerBuffType(kvp.Key));
            }

            return msg;
        }

        private static MessageOfObj Bullet(Bullet bullet)
        {
            MessageOfObj msg = new()
            {
                BulletMessage = new()
                {
                    Type = Transformation.ToBulletType(bullet.TypeOfBullet),
                    X = bullet.Position.x,
                    Y = bullet.Position.y,
                    FacingDirection = bullet.FacingDirection.Angle(),
                    Guid = bullet.ID,
                    Team = (bullet.Parent!.IsGhost()) ? PlayerType.TrickerPlayer : PlayerType.StudentPlayer,
                    BombRange = bullet.BulletBombRange,
                    Speed = bullet.Speed
                }
            };
            return msg;
        }

        private static MessageOfObj Prop(Gadget prop)
        {
            MessageOfObj msg = new()
            {
                PropMessage = new()
                {
                    Type = Transformation.ToPropType(prop.GetPropType()),
                    X = prop.Position.x,
                    Y = prop.Position.y,
                    FacingDirection = prop.FacingDirection.Angle(),
                    Guid = prop.ID
                }
            };
            return msg;
        }

        private static MessageOfObj Prop(Item prop)
        {
            MessageOfObj msg = new()
            {
                PropMessage = new()
                {
                    Type = Transformation.ToPropType(prop.GetPropType()),
                    X = prop.Position.x,
                    Y = prop.Position.y,
                    FacingDirection = prop.FacingDirection.Angle(),
                    Guid = prop.ID
                }
            };
            return msg;
        }

        private static MessageOfObj BombedBullet(BombedBullet bombedBullet)
        {
            MessageOfObj msg = new()
            {
                BombedBulletMessage = new()
                {
                    Type = Transformation.ToBulletType(bombedBullet.bulletHasBombed.TypeOfBullet),
                    X = bombedBullet.bulletHasBombed.Position.x,
                    Y = bombedBullet.bulletHasBombed.Position.y,
                    FacingDirection = bombedBullet.FacingDirection.Angle(),
                    MappingId = bombedBullet.MappingID,
                    BombRange = bombedBullet.bulletHasBombed.BulletBombRange
                }
            };
            //   Debugger.Output(bombedBullet, bombedBullet.Place.ToString()+" "+bombedBullet.Position.ToString());
            return msg;
        }

        private static MessageOfObj Classroom(Generator generator)
        {
            MessageOfObj msg = new()
            {
                ClassroomMessage = new()
                {
                    X = generator.Position.x,
                    Y = generator.Position.y,
                    Progress = generator.DegreeOfRepair
                }
            };
            return msg;
        }
        private static MessageOfObj Gate(Doorway doorway, int time)
        {
            MessageOfObj msg = new()
            {
                GateMessage = new()
                {
                    X = doorway.Position.x,
                    Y = doorway.Position.y
                }
            };
            int progress = ((doorway.OpenStartTime > 0) ? (time - doorway.OpenStartTime) : 0) + doorway.OpenDegree;
            msg.GateMessage.Progress = (progress > GameData.degreeOfOpenedDoorway) ? GameData.degreeOfOpenedDoorway : progress;
            return msg;
        }
        private static MessageOfObj HiddenGate(EmergencyExit Exit)
        {
            MessageOfObj msg = new()
            {
                HiddenGateMessage = new()
                {
                    X = Exit.Position.x,
                    Y = Exit.Position.y,
                    Opened = Exit.IsOpen
                }
            };
            return msg;
        }

        private static MessageOfObj Door(Door door)
        {
            MessageOfObj msg = new()
            {
                DoorMessage = new()
                {
                    X = door.Position.x,
                    Y = door.Position.y,
                    Progress = door.LockDegree,
                    IsOpen = door.IsOpen
                }
            };
            return msg;
        }
        private static MessageOfObj Chest(Chest chest, int time)
        {
            MessageOfObj msg = new()
            {
                ChestMessage = new()
                {
                    X = chest.Position.x,
                    Y = chest.Position.y
                }
            };
            int progress = (chest.WhoOpen != null) ? ((time - chest.OpenStartTime) * chest.WhoOpen.SpeedOfOpenChest) : 0;
            msg.ChestMessage.Progress = (progress > GameData.degreeOfOpenedChest) ? GameData.degreeOfOpenedChest : progress;
            return msg;
        }
    }
}
