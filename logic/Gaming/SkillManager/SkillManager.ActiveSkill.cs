using GameClass.GameObj;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private partial class SkillManager
        {
            public static bool CanBeginToCharge(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill skill = player.FindActiveSkill(ActiveSkillType.CanBeginToCharge);
                Debugger.Output(player, "can begin to charge!");


                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 3.0);
                    //See SkillWhenColliding in ActionManager
                },
                                                      () =>
                                                      { });
            }

            public bool ShowTime(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill skill = player.FindActiveSkill(ActiveSkillType.ShowTime);

                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 0.8);
                    new Thread
                    (
                   () =>
                   {
                       new FrameRateTaskExecutor<int>(
                       loopCondition: () => player.Commandable() && gameMap.Timer.IsGaming,
                             loopToDo: () =>
                             {
                                 gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                                 try
                                 {
                                     foreach (Character person in gameMap.GameObjDict[GameObjType.Character])
                                     {
                                         if (!person.IsGhost() && person.CharacterType != CharacterType.Robot && !person.NoHp())
                                         {
                                             double dis = XY.DistanceFloor3(person.Position, player.Position);
                                             if (dis >= player.AlertnessRadius)
                                             {
                                                 person.AddMoveSpeed(GameData.checkIntervalWhenShowTime, dis / player.AlertnessRadius);
                                                 actionManager.MovePlayerWhenStunned(person, GameData.checkIntervalWhenShowTime, (player.Position - person.Position).Angle());
                                             }
                                             else if (dis >= player.ViewRange)
                                             {
                                                 Student student = (Student)person;
                                                 student.GamingAddiction += GameData.checkIntervalWhenShowTime;
                                                 if (student.GamingAddiction == student.MaxGamingAddiction)
                                                 {
                                                     characterManager.Die(student);
                                                 }
                                             }
                                         }
                                     }
                                 }
                                 finally
                                 {
                                     gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                                 }
                             },
                             timeInterval: GameData.checkIntervalWhenShowTime,
                             maxTotalDuration: skill.DurationTime,
                             finallyReturn: () => 0
                         )

                             .Start();
                   }
                    )
                    { IsBackground = true }.Start();
                },
                                                      () =>
                                                      {
                                                      }
                                                      );
            }

            public static bool BecomeInvisible(Character player)
            {
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.BecomeInvisible);
                long stateNum = player.SetPlayerState(RunningStateType.RunningForcibly, PlayerStateType.UsingSkill);
                if (stateNum == -1)
                {
                    return false;
                }
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    player.AddScore(GameData.ScoreBecomeInvisible);
                    player.AddInvisible(activeSkill.DurationTime);
                    Debugger.Output(player, "become invisible!");
                },
                () =>
                 {
                     player.ResetPlayerState(stateNum);
                 }
                );
            }

            public static bool UseRobot(Character player, int robotID)
            {
                if ((robotID - player.PlayerID) % GameData.numOfPeople != 0) return false;
                if ((robotID - (int)player.PlayerID) / GameData.numOfPeople < 0 || (robotID - (int)player.PlayerID) / GameData.numOfPeople > GameData.maxSummonedGolemNum) return false;
                UseRobot activeSkill = (UseRobot)player.FindActiveSkill(ActiveSkillType.UseRobot);
                lock (activeSkill.ActiveSkillUseLock)
                {
                    if (robotID == player.PlayerID)
                    {
                        lock (player.ActionLock)
                        {
                            if (player.PlayerState == PlayerStateType.UsingSkill && player.WhatInteractingWith == null)
                                player.SetPlayerStateNaturally();
                            activeSkill.NowPlayerID = robotID;
                        }
                    }
                    else
                    {
                        SummonGolem summonGolemSkill = (SummonGolem)player.FindActiveSkill(ActiveSkillType.SummonGolem);
                        if (summonGolemSkill.GolemStateArray[(robotID - (int)player.PlayerID) / GameData.numOfPeople - 1] == 2)
                        {
                            activeSkill.NowPlayerID = robotID;
                        }
                        else return false;
                        long stateNum = player.SetPlayerState(RunningStateType.RunningForcibly, PlayerStateType.UsingSkill);
                        if (stateNum == -1)
                        {
                            activeSkill.NowPlayerID = (int)player.PlayerID;
                            return false;
                        }
                    }
                    return ActiveSkillEffect(activeSkill, player, () => { }, () => { }
                    );
                }
            }

            public static bool JumpyBomb(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.JumpyBomb), player, () =>
                {
                    player.BulletOfPlayer = BulletType.BombBomb;
                    Debugger.Output(player, "uses jumpybomb!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }

            public bool SparksNSplash(Character player, int AttackID)
            {
                Character? whoAttacked = gameMap.FindPlayerInPlayerID(AttackID);
                if (whoAttacked == null || whoAttacked.NoHp())
                    return false;
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.SparksNSplash);

                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    new Thread
                    (
                   () =>
                   {
                       Bullet? homingMissile = null;
                       double dis;
                       new FrameRateTaskExecutor<int>(
                       loopCondition: () => gameMap.Timer.IsGaming && !whoAttacked.NoHp(),
                             loopToDo: () =>
                             {
                                 dis = ((homingMissile == null || homingMissile.IsRemoved) ? double.MaxValue : XY.DistanceFloor3(homingMissile.Position, whoAttacked.Position));
                                 gameMap.GameObjLockDict[GameObjType.Bullet].EnterReadLock();
                                 try
                                 {
                                     foreach (Bullet bullet in gameMap.GameObjDict[GameObjType.Bullet])
                                     {
                                         if (!bullet.CanMove && XY.DistanceFloor3(bullet.Position, whoAttacked.Position) < dis && bullet.TypeOfBullet == BulletType.JumpyDumpty)
                                         {
                                             homingMissile = bullet;
                                             dis = XY.DistanceFloor3(bullet.Position, whoAttacked.Position);
                                         }
                                     }
                                 }
                                 finally
                                 {
                                     gameMap.GameObjLockDict[GameObjType.Bullet].ExitReadLock();
                                 }
                                 if (homingMissile != null)
                                 {
                                     homingMissile.CanMove.SetReturnOri(true);
                                     attackManager.moveEngine.MoveObj(homingMissile, GameData.checkIntervalWhenSparksNSplash - 1, (whoAttacked.Position - homingMissile.Position).Angle(), ++homingMissile.StateNum);
                                 }
                             },
                             timeInterval: GameData.checkIntervalWhenSparksNSplash,
                             maxTotalDuration: activeSkill.DurationTime,
                             finallyReturn: () => 0
                         )

                             .Start();
                   }
                    )
                    { IsBackground = true }.Start();
                    Debugger.Output(player, "uses sparks n splash!");
                },
                                                      () =>
                                                      { });
            }

            public bool WriteAnswers(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.WriteAnswers);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    Generator? generator = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);
                    if (generator != null)
                    {
                        if (generator.Repair(((WriteAnswers)activeSkill).DegreeOfMeditation, player))
                            gameMap.AddNumOfRepairedGenerators();
                        Debugger.Output(player, "uses WriteAnswers in" + generator.ToString() + "with " + (((WriteAnswers)activeSkill).DegreeOfMeditation).ToString());
                        ((WriteAnswers)activeSkill).DegreeOfMeditation.SetReturnOri(0);
                    }
                },
                                                      () =>
                                                      { });
            }

            public bool SummonGolem(Character player)
            {
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.SummonGolem);
                int num = ((SummonGolem)activeSkill).BuildGolem();
                if (num >= GameData.maxSummonedGolemNum) return false;

                XY res = player.Position + new XY(player.FacingDirection, player.Radius * 2);
                lock (activeSkill.ActiveSkillUseLock)
                {
                    CraftingBench craftingBench = new(res, player, num);

                    long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.UsingSkill, craftingBench);
                    if (stateNum == -1)
                    {
                        ((SummonGolem)activeSkill).DeleteGolem(num);
                        return false;
                    }


                    return ActiveSkillEffect(activeSkill, player, () =>
                    {
                        player.ThreadNum.WaitOne();
                        if (!player.StartThread(stateNum, RunningStateType.RunningSleepily))
                        {
                            ((SummonGolem)activeSkill).DeleteGolem(num);
                            player.ThreadNum.Release();
                        }
                        else
                        {
                            if (actionManager.moveEngine.CheckCollision(craftingBench, res) != null)
                            {
                                ((SummonGolem)activeSkill).DeleteGolem(num);
                                if (player.ResetPlayerState(stateNum))
                                    player.ThreadNum.Release();
                            }
                            else
                            {
                                craftingBench.ParentStateNum = stateNum;
                                gameMap.Add(craftingBench);
                            }
                        }
                    },
                    () =>
                    {
                        if (player.ResetPlayerState(stateNum))
                        {
                            gameMap.RemoveJustFromMap(craftingBench);
                            Golem? golem = (Golem?)characterManager.AddPlayer(res, player.TeamID, (num + 1) * GameData.numOfPeople + player.PlayerID, CharacterType.Robot, player);
                            if (golem == null)
                            {
                                ((SummonGolem)activeSkill).AddGolem(num);
                            }
                            player.ThreadNum.Release();
                        }
                    }
                     );
                }
            }

            public static bool UseKnife(Character player)
            {
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.UseKnife), player, () =>
                {
                    player.BulletOfPlayer = BulletType.FlyingKnife;
                    Debugger.Output(player, "uses flyingknife!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }

            public bool Howl(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Howl), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (!character.IsGhost() && !character.NoHp() && XY.DistanceFloor3(character.Position, player.Position) <= player.ViewRange)
                            {
                                if (CharacterManager.BeStunned(character, GameData.timeOfStudentStunnedWhenHowl) > 0)
                                    player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.timeOfStudentStunnedWhenHowl));
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    characterManager.BackSwing(player, GameData.timeOfGhostSwingingAfterHowl);
                    Debugger.Output(player, "howled!");
                },
                                                      () =>
                                                      { });
            }

            public bool Punish(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Punish), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (character.IsGhost() &&
                                (character.PlayerState == PlayerStateType.TryingToAttack || character.PlayerState == PlayerStateType.Swinging
                                || character.PlayerState == PlayerStateType.UsingSkill
                                || character.PlayerState == PlayerStateType.LockingTheDoor || character.PlayerState == PlayerStateType.OpeningTheDoor
                                || character.PlayerState == PlayerStateType.ClimbingThroughWindows)
                                && XY.DistanceFloor3(character.Position, player.Position) <= player.ViewRange / 3)
                            {
                                int stunTime = (GameData.timeOfGhostStunnedWhenPunish + (int)((GameData.factorOfTimeStunnedWhenPunish * (player.HP.GetMaxV() - player.HP) / GameData.basicApOfGhost))) / characterManager.FactorTeacher;
                                if (CharacterManager.BeStunned(character, stunTime) > 0)
                                    player.AddScore(GameData.StudentScoreTrickerBeStunned(stunTime) / characterManager.FactorTeacher);
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "uses punishing!");
                },
                                                      () =>
                                                      { });
            }

            public bool HaveTea(Character player, int angle1000)
            {
                XY res = player.Position + new XY(angle1000 / 1000.0, GameData.distanceOfHaveTea);
                if (res.x <= GameData.numOfPosGridPerCell || res.y <= GameData.numOfPosGridPerCell || res.x >= GameData.numOfPosGridPerCell * (GameData.rows - 1) || res.y >= GameData.numOfPosGridPerCell * (GameData.cols - 1))
                    return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.UsingSkill);
                if (stateNum == -1)
                {
                    return false;
                }

                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.HaveTea), player, () =>
                {
                    player.ThreadNum.WaitOne();

                    if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                    {
                        player.ThreadNum.Release();
                    }
                    else
                    {
                        if (actionManager.moveEngine.CheckCollision(player, res) != null)
                        {
                            player.ThreadNum.Release();
                        }
                        else
                        {
                            Debugger.Output("NO Collision!");
                            player.ReSetPos(res);
                            player.ThreadNum.Release();
                            player.ResetPlayerState(stateNum);
                            Debugger.Output(player, "have tea!");
                        }
                    }
                },
                                                      () =>
                                                      { });
            }

            public bool Rouse(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Rouse), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            lock (character.ActionLock)
                            {
                                if ((character.PlayerState == PlayerStateType.Addicted) && gameMap.CanSee(player, character))
                                {
                                    character.SetPlayerStateNaturally();
                                    character.HP.SetPositiveV(GameData.RemainHpWhenAddLife);
                                    ((Student)character).SetTimeOfRescue(0);
                                    player.AddScore(GameData.StudentScoreRescue);
                                    break;
                                }
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "rouse someone!");
                },
                                                      () =>
                                                      { });
            }

            public bool Encourage(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Encourage), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if ((character.HP < character.HP.GetMaxV()) && gameMap.CanSee(player, character))
                            {
                                player.AddScore(GameData.StudentScoreTreat(GameData.addHpWhenEncourage));
                                character.HP.AddPositiveV(GameData.addHpWhenEncourage);
                                ((Student)character).SetDegreeOfTreatment0();
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "encourage someone!");
                },
                                                      () =>
                                                      { });
            }

            public bool Inspire(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Inspire), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (gameMap.CanSee(player, character) && !character.IsGhost())
                            {
                                player.AddScore(GameData.ScoreInspire);
                                character.AddMoveSpeed(GameData.timeOfAddingSpeedWhenInspire, GameData.addedTimeOfSpeedWhenInspire);
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "inspires!");
                },
                                                      () =>
                                                      { });
            }

            public static bool ActiveSkillEffect(ActiveSkill activeSkill, Character player, Action startSkill, Action endSkill)
            {
                lock (activeSkill.ActiveSkillUseLock)
                {
                    if (activeSkill.StartSkill())
                    {
                        new Thread
                        (() =>
                        {
                            startSkill();
                            activeSkill.IsBeingUsed = true;
                            Thread.Sleep(activeSkill.DurationTime);

                            endSkill();
                            activeSkill.IsBeingUsed = false;
                            Debugger.Output(player, "return to normal.");
                        }
                        )
                        { IsBackground = true }.Start();

                        return true;
                    }
                    else
                    {
                        Debugger.Output(player, "CommonSkill is cooling down!");
                        return false;
                    }
                }
            }
        }
    }
}