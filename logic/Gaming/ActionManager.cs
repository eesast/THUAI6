using System;
using System.Threading;
using GameClass.GameObj;
using GameEngine;
using Preparation.Utility;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private readonly ActionManager actionManager;
        private class ActionManager
        {
            public bool MovePlayer(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (moveTimeInMilliseconds < 5) return false;
                long stateNum = playerToMove.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Moving);
                if (stateNum == -1) return false;
                new Thread
                    (
                    () =>
                    {
                        playerToMove.ThreadNum.WaitOne();
                        if (!playerToMove.StartThread(stateNum, RunningStateType.RunningActively))
                        {
                            playerToMove.ThreadNum.Release();
                            return;
                        }
                        moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection, stateNum);
                    }
                    )
                { IsBackground = true }.Start();
                return true;
            }

            public bool MovePlayerWhenStunned(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (playerToMove.CharacterType == CharacterType.Robot) return false;
                long stateNum = playerToMove.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Charmed);
                if (stateNum == -1) return false;
                new Thread
                (() =>
                {
                    playerToMove.ThreadNum.WaitOne();
                    if (!playerToMove.StartThread(stateNum, RunningStateType.RunningActively))
                    {
                        playerToMove.ThreadNum.Release();
                        return;
                    }
                    else
                    {
                        moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection, playerToMove.StateNum);
                        Thread.Sleep(moveTimeInMilliseconds);
                        lock (playerToMove.ActionLock)
                        {
                            lock (playerToMove.ActionLock)
                            {
                                if (stateNum == playerToMove.StateNum)
                                    playerToMove.SetPlayerStateNaturally();
                            }
                        }
                    }
                }
                 )
                { IsBackground = true }.Start();
                return true;
            }

            public static bool Stop(Character player)
            {
                lock (player.ActionLock)
                {
                    if (player.Commandable())
                    {
                        player.SetPlayerState(RunningStateType.Null);
                        return true;
                    }
                }
                return false;
            }

            public bool Fix(Student player)// 自动检查有无发电机可修
            {
                Generator? generatorForFix = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);
                if (generatorForFix == null) return false;

                if (generatorForFix.DegreeOfRepair == GameData.degreeOfFixedGenerator)
                    return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Fixing);
                if (stateNum == -1) return false;

                new Thread
          (
              () =>
              {
                  player.ThreadNum.WaitOne();
                  if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                  {
                      player.ThreadNum.Release();
                      return;
                  }

                  generatorForFix.AddNumOfFixing();

                  Thread.Sleep(GameData.checkInterval);
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => stateNum == player.StateNum && gameMap.Timer.IsGaming,
                      loopToDo: () =>
                      {
                          if (generatorForFix.Repair(player.FixSpeed * GameData.checkInterval, player))
                              gameMap.AddNumOfRepairedGenerators();
                          if (generatorForFix.DegreeOfRepair == GameData.degreeOfFixedGenerator)
                          {
                              lock (player.ActionLock)
                              {
                                  if (stateNum == player.StateNum)
                                      player.SetPlayerState(RunningStateType.Null);
                              }
                              return false;
                          }
                          return true;
                      },
                      timeInterval: GameData.checkInterval,
                      finallyReturn: () => 0
                  )
                      .Start();
                  player.ThreadNum.Release();
                  generatorForFix.SubNumOfFixing();
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool OpenDoorway(Student player)
            {
                Doorway? doorwayToOpen = (Doorway?)gameMap.OneForInteract(player.Position, GameObjType.Doorway);
                if (doorwayToOpen == null) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.OpeningTheDoorway, doorwayToOpen);
                if (stateNum == -1) return false;
                new Thread
          (
              () =>
              {
                  player.ThreadNum.WaitOne();
                  if (!doorwayToOpen.TryToOpen())
                  {
                      player.ThreadNum.Release();
                      player.ResetPlayerState(stateNum);
                      return;
                  }
                  if (!player.StartThread(stateNum, RunningStateType.RunningSleepily))
                  {
                      player.ThreadNum.Release();
                      return;
                  }
                  Thread.Sleep(GameData.degreeOfOpenedDoorway - doorwayToOpen.OpenDegree);

                  if (player.ResetPlayerState(stateNum))
                  {
                      doorwayToOpen.FinishOpenning();
                      player.ThreadNum.Release();
                  }
              }
          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool Escape(Student player)
            {
                if (player.CharacterType == CharacterType.Robot || player.CharacterType == CharacterType.Teacher)
                    return false;

                Doorway? doorwayForEscape = (Doorway?)gameMap.OneForInteract(player.Position, GameObjType.Doorway);
                if (doorwayForEscape != null && doorwayForEscape.IsOpen())
                {
                    if (!player.TryToRemoveFromGame(PlayerStateType.Escaped)) return false;
                    player.AddScore(GameData.StudentScoreEscape);
                    gameMap.MapEscapeStudent();
                    return true;
                }
                else
                {
                    EmergencyExit? emergencyExit = (EmergencyExit?)gameMap.OneForInteract(player.Position, GameObjType.EmergencyExit);
                    if (emergencyExit != null && emergencyExit.IsOpen)
                    {
                        if (!player.TryToRemoveFromGame(PlayerStateType.Escaped)) return false;
                        player.AddScore(GameData.StudentScoreEscape);
                        gameMap.MapEscapeStudent();
                        return true;
                    }
                    return false;
                }
            }

            public bool Treat(Student player, Student? playerTreated = null)
            {
                if (player.CharacterType == CharacterType.Robot) return false;
                if (playerTreated == null)
                {
                    playerTreated = gameMap.StudentForInteract(player.Position);
                    if (playerTreated == null) return false;
                }
                else if (!GameData.ApproachToInteract(playerTreated.Position, player.Position)) return false;

                if (playerTreated.HP == playerTreated.MaxHp) return false;

                long stateNumTreated = playerTreated.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Treated);
                if (stateNumTreated == -1) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Treating);
                if (stateNum == -1)
                {
                    lock (playerTreated.ActionLock)
                    {
                        if (playerTreated.StateNum == stateNumTreated)
                            player.SetPlayerStateNaturally();
                    }
                    return false;
                }

                new Thread
           (
               () =>
               {
                   player.ThreadNum.WaitOne();
                   if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                   {
                       player.ThreadNum.Release();
                       lock (playerTreated.ActionLock)
                       {
                           if (playerTreated.StateNum == stateNumTreated)
                               playerTreated.SetPlayerStateNaturally();
                       }
                       return;
                   }

                   playerTreated.ThreadNum.WaitOne();
                   if (!playerTreated.StartThread(stateNum, RunningStateType.RunningActively))
                   {
                       playerTreated.ThreadNum.Release();
                       lock (player.ActionLock)
                       {
                           if (player.StateNum == stateNum)
                               player.SetPlayerStateNaturally();
                       }
                       player.ThreadNum.Release();
                       return;
                   }

                   Thread.Sleep(GameData.checkInterval);
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => stateNum == player.StateNum && gameMap.Timer.IsGaming,
                       loopToDo: () =>
                       {
                           lock (playerTreated.ActionLock)
                           {
                               if (playerTreated.StateNum == stateNumTreated)
                               {
                                   if (playerTreated.AddDegreeOfTreatment(GameData.checkInterval * player.TreatSpeed, player))
                                   {
                                       playerTreated.SetPlayerStateNaturally();
                                       return false;
                                   }
                               }
                               else return false;
                           }
                           return true;
                       },
                       timeInterval: GameData.checkInterval,
                       finallyReturn: () => 0
                   )
                       .Start();
                   player.ThreadNum.Release();
                   playerTreated.ThreadNum.Release();
                   if (player.ResetPlayerState(stateNum))
                       return;

                   playerTreated.ResetPlayerState(stateNumTreated);
               }
           )
                { IsBackground = true }.Start();
                return true;
            }

            public bool Rescue(Student player, Student? playerRescued = null)
            {
                if (player.CharacterType == CharacterType.Robot) return false;

                if (playerRescued == null)
                {
                    playerRescued = gameMap.StudentForInteract(player.Position);
                    if (playerRescued == null) return false;
                }
                else if (!GameData.ApproachToInteract(playerRescued.Position, player.Position)) return false;

                long stateNumRescued = playerRescued.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Rescued);
                if (stateNumRescued == -1) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.Rescuing);
                if (stateNum == -1)
                {
                    playerRescued.ResetPlayerStateInOneThread(stateNumRescued, RunningStateType.RunningForcibly, PlayerStateType.Addicted);
                    return false;
                }

                new Thread
           (
               () =>
               {
                   player.ThreadNum.WaitOne();
                   if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                   {
                       player.ThreadNum.Release();
                       playerRescued.ResetPlayerStateInOneThread(stateNumRescued, RunningStateType.RunningForcibly, PlayerStateType.Addicted);
                       return;
                   }

                   playerRescued.ThreadNum.WaitOne();

                   if (!playerRescued.StartThread(stateNumRescued, RunningStateType.RunningActively))
                   {
                       playerRescued.ThreadNum.Release();
                       if (!player.ResetPlayerState(stateNum))
                           player.ThreadNum.Release();
                       return;
                   }

                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => stateNum == player.StateNum && gameMap.Timer.IsGaming,
                       loopToDo: () =>
                       {
                           lock (playerRescued.ActionLock)
                           {
                               if (playerRescued.StateNum == stateNumRescued)
                               {
                                   if (playerRescued.AddTimeOfRescue(GameData.checkInterval))
                                   {
                                       playerRescued.SetPlayerStateNaturally();
                                       playerRescued.HP = playerRescued.MaxHp / 2;
                                       player.AddScore(GameData.StudentScoreRescue);
                                       return false;
                                   }
                               }
                               else return false;
                           }
                           return true;
                       },
                       timeInterval: GameData.checkInterval,
                       finallyReturn: () => 0
                   )
                       .Start();
                   playerRescued.SetTimeOfRescue(0);

                   player.ThreadNum.Release();
                   playerRescued.ThreadNum.Release();
                   if (player.ResetPlayerState(stateNum)) return;

                   playerRescued.ResetPlayerStateInOneThread(stateNumRescued, RunningStateType.RunningForcibly, PlayerStateType.Addicted);
               }
           )
                { IsBackground = true }.Start();

                return true;
            }

            public bool OpenChest(Character player)
            {
                Chest? chestToOpen = (Chest?)gameMap.OneForInteract(player.Position, GameObjType.Chest);
                if (chestToOpen == null) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.OpeningTheChest, chestToOpen);
                if (stateNum == -1) return false;

                new Thread
          (
              () =>
              {
                  player.ThreadNum.WaitOne();
                  lock (player.ActionLock)
                  {
                      if (!player.StartThread(stateNum, RunningStateType.RunningSleepily))
                      {
                          player.ThreadNum.Release();
                          return;
                      }
                      else
                      if (!chestToOpen.Open(player))
                      {
                          player.ThreadNum.Release();
                          player.SetPlayerStateNaturally();
                          return;
                      }
                  }

                  Thread.Sleep(GameData.degreeOfOpenedChest / player.SpeedOfOpenChest);

                  if (player.ResetPlayerState(stateNum))
                  {
                      player.ThreadNum.Release();
                      for (int i = 0; i < GameData.maxNumOfPropInChest; ++i)
                      {
                          Gadget prop = chestToOpen.PropInChest[i];
                          chestToOpen.PropInChest[i] = new NullProp();
                          prop.ReSetPos(player.Position);
                          gameMap.Add(prop);
                      }
                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool ClimbingThroughWindow(Character player)
            {
                Window? windowForClimb = (Window?)gameMap.OneForInteractInACross(player.Position, GameObjType.Window);
                if (windowForClimb == null) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.ClimbingThroughWindows, windowForClimb);
                if (stateNum == -1) return false;

                XY windowToPlayer = new(
                      (Math.Abs(player.Position.x - windowForClimb.Position.x) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.x > windowForClimb.Position.x ? 1 : -1)) : 0,
                      (Math.Abs(player.Position.y - windowForClimb.Position.y) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.y > windowForClimb.Position.y ? 1 : -1)) : 0);

                /*       Character? characterInWindow = (Character?)gameMap.OneInTheSameCell(windowForClimb.Position - 2 * windowToPlayer, GameObjType.Character);
                       if (characterInWindow != null)
                       {
                           if (player.IsGhost() && !characterInWindow.IsGhost())
                               characterManager.BeAttacked((Student)(characterInWindow), player.Attack(characterInWindow.Position));
                           return false;
                       }

                Wall addWall = new Wall(windowForClimb.Position - 2 * windowToPlayer);
                 gameMap.Add(addWall);*/

                new Thread
                (
                    () =>
                    {
                        player.ThreadNum.WaitOne();
                        lock (player.ActionLock)
                        {
                            if (!player.StartThread(stateNum, RunningStateType.RunningSleepily))
                            {
                                player.ThreadNum.Release();
                                return;
                            }
                            if (!windowForClimb.TryToClimb(player))
                            {
                                player.SetPlayerStateNaturally();
                                player.ThreadNum.Release();
                                return;
                            }
                        }

                        Thread.Sleep((int)((windowToPlayer + windowForClimb.Position - player.Position).Length() * 1000 / player.MoveSpeed));

                        lock (player.ActionLock)
                        {
                            if (!player.StartThread(stateNum, RunningStateType.RunningActively)) return;
                            windowForClimb.Enter2Stage(windowForClimb.Position - 2 * windowToPlayer);
                            player.ReSetPos(windowToPlayer + windowForClimb.Position);
                        }

                        player.MoveSpeed = player.SpeedOfClimbingThroughWindows;
                        moveEngine.MoveObj(player, (int)(GameData.numOfPosGridPerCell * 3 * 1000 / player.MoveSpeed / 2), (-1 * windowToPlayer).Angle(), stateNum);

                        Thread.Sleep((int)(GameData.numOfPosGridPerCell * 3 * 1000 / player.MoveSpeed / 2));

                        player.MoveSpeed = player.ReCalculateBuff(BuffType.AddSpeed, player.OrgMoveSpeed, GameData.MaxSpeed, GameData.MinSpeed);

                        lock (player.ActionLock)
                        {
                            if (stateNum == player.StateNum)
                            {
                                player.ReSetPos(windowForClimb.Stage);
                                player.SetPlayerStateNaturally();
                                windowForClimb.FinishClimbing();
                            }
                        }
                    }
                )
                { IsBackground = true }.Start();

                return true;
            }

            public bool LockDoor(Character player)
            {
                if (player.CharacterType == CharacterType.Robot) return false;
                Door? doorToLock = (Door?)gameMap.OneForInteract(player.Position, GameObjType.Door);
                if (doorToLock == null) return false;

                PropType propType = doorToLock.DoorNum switch
                {
                    3 => PropType.Key3,
                    5 => PropType.Key5,
                    _ => PropType.Key6,
                };

                if (!player.UseTool(propType)) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.LockingTheDoor, doorToLock);
                if (stateNum == -1)
                {
                    player.ReleaseTool(propType);
                    return false;
                }

                new Thread
                (
                    () =>
                    {
                        player.ThreadNum.WaitOne();
                        if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                        {
                            player.ReleaseTool(propType);
                            player.ThreadNum.Release();
                            return;
                        }
                        if (!doorToLock.TryLock(player))
                        {
                            player.ReleaseTool(propType);
                            player.ResetPlayerState(stateNum);
                            player.ThreadNum.Release();
                            return;
                        }
                        Thread.Sleep(GameData.checkInterval);
                        new FrameRateTaskExecutor<int>(
                        loopCondition: () => stateNum == player.StateNum && gameMap.Timer.IsGaming && doorToLock.LockDegree < GameData.degreeOfLockingOrOpeningTheDoor,
                        loopToDo: () =>
                        {
                            if ((gameMap.PartInTheSameCell(doorToLock.Position, GameObjType.Character)) != null)
                                return false;
                            doorToLock.LockDegree += GameData.checkInterval * player.SpeedOfOpeningOrLocking;
                            return true;
                        },
                  timeInterval: GameData.checkInterval,
                  finallyReturn: () => 0
                  )
                  .Start();
                        doorToLock.StopLock();
                        player.ReleaseTool(propType);
                        player.ThreadNum.Release();
                        player.ResetPlayerState(stateNum);
                    }
                 )
                { IsBackground = true }.Start();

                return true;
            }

            public bool OpenDoor(Character player)
            {
                if (player.CharacterType == CharacterType.Robot) return false;
                Door? doorToLock = (Door?)gameMap.OneForInteract(player.Position, GameObjType.Door);
                if (doorToLock == null) return false;

                PropType propType = doorToLock.DoorNum switch
                {
                    3 => PropType.Key3,
                    5 => PropType.Key5,
                    _ => PropType.Key6,
                };

                if (!player.UseTool(propType)) return false;

                long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.OpeningTheDoor, doorToLock);
                if (stateNum == -1)
                {
                    player.ReleaseTool(propType);
                    return false;
                }

                new Thread
                (
                    () =>
                    {
                        player.ThreadNum.WaitOne();
                        if (!player.StartThread(stateNum, RunningStateType.RunningSleepily))
                        {
                            player.ReleaseTool(propType);
                            player.ThreadNum.Release();
                            return;
                        }
                        if (!doorToLock.TryOpen(player))
                        {
                            player.ReleaseTool(propType);
                            if (player.ResetPlayerState(stateNum))
                                player.ThreadNum.Release();
                            return;
                        }
                        Thread.Sleep(GameData.degreeOfLockingOrOpeningTheDoor / player.SpeedOfOpeningOrLocking);

                        if (player.ResetPlayerState(stateNum))
                        {
                            doorToLock.StopOpen();
                            player.ReleaseTool(propType);
                            player.ThreadNum.Release();
                        }
                    }

                 )
                { IsBackground = true }.Start();

                return true;
            }

            /*
            private void ActivateMine(Character player, Mine mine)
            {
                gameMap.ObjListLock.EnterWriteLock();
                try { gameMap.ObjList.Remove(mine); }
                catch { }
                finally { gameMap.ObjListLock.ExitWriteLock(); }

                switch (mine.GetPropType())
                {
                    case PropType.Dirt:
                        player.AddMoveSpeed(Constant.dirtMoveSpeedDebuff, Constant.buffPropTime);
                        break;
                    case PropType.Attenuator:
                        player.AddAP(Constant.attenuatorAtkDebuff, Constant.buffPropTime);
                        break;
                    case PropType.Divider:
                        player.ChangeCD(Constant.dividerCdDiscount, Constant.buffPropTime);
                        break;
                }
            }
            */

            private readonly Map gameMap;
            private readonly CharacterManager characterManager;
            public readonly MoveEngine moveEngine;
            public ActionManager(Map gameMap, CharacterManager characterManager)
            {
                this.gameMap = gameMap;

                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collisionObj, moveVec) =>
                    {
                        Character player = (Character)obj;
                        switch (collisionObj.Type)
                        {
                            case GameObjType.Bullet:

                                if (((Bullet)collisionObj).Parent != player && ((Bullet)collisionObj).TypeOfBullet == BulletType.JumpyDumpty)
                                {
                                    if (CharacterManager.BeStunned((Character)player, GameData.timeOfStunnedWhenJumpyDumpty) > 0)
                                        player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.timeOfStunnedWhenJumpyDumpty));
                                    gameMap.Remove((GameObj)collisionObj);
                                }
                                break;
                            case GameObjType.Character:
                                if (player.FindActiveSkill(ActiveSkillType.CanBeginToCharge).IsBeingUsed == 1 && ((Character)collisionObj).IsGhost())
                                {
                                    if (CharacterManager.BeStunned((Character)collisionObj, GameData.timeOfGhostStunnedWhenCharge) > 0)
                                        player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.timeOfGhostStunnedWhenCharge));
                                    CharacterManager.BeStunned(player, GameData.timeOfStudentStunnedWhenCharge);
                                }
                                break;
                            case GameObjType.Item:
                                if (((Item)collisionObj).GetPropType() == PropType.CraftingBench)
                                {
                                    ((CraftingBench)collisionObj).TryStopSkill();
                                    gameMap.Remove((Item)collisionObj);
                                }
                                break;
                            default:
                                break;
                        }

                        //Preparation.Utility.Debugger.Output(obj, " end move with " + collisionObj.ToString());
                        //if (collisionObj is Mine)
                        //{
                        //    ActivateMine((Character)obj, (Mine)collisionObj);
                        //    return MoveEngine.AfterCollision.ContinueCheck;
                        //}
                        return MoveEngine.AfterCollision.MoveMax;
                    },
                    EndMove: obj =>
                    {
                        obj.ThreadNum.Release();
                        // Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                    }
                );
                this.characterManager = characterManager;
            }
        }
    }
}
