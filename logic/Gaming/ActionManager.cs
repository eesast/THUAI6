﻿using System;
using System.Threading;
using GameClass.GameObj;
using GameEngine;
using Preparation.Interface;
using Preparation.Utility;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private readonly ActionManager actionManager;
        private class ActionManager
        {

            // 人物移动
            public bool MovePlayer(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (!playerToMove.Commandable()) return false;
                moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection);
                return true;
            }

            public static bool Stop(Character player)
            {
                if (player.Commandable())
                {
                    player.PlayerState = PlayerStateType.Null;
                    return true;
                }
                return false;
            }

            public bool Fix(Student player)// 自动检查有无发电机可修
            {
                if ((!player.Commandable()) || player.PlayerState == PlayerStateType.Fixing)
                    return false;
                Generator? generatorForFix = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);

                if (generatorForFix == null || generatorForFix.DegreeOfRepair == GameData.degreeOfFixedGenerator)
                    return false;

                player.PlayerState = PlayerStateType.Fixing;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => player.PlayerState == PlayerStateType.Fixing && gameMap.Timer.IsGaming && generatorForFix.DegreeOfRepair < GameData.degreeOfFixedGenerator,
                      loopToDo: () =>
                      {
                          generatorForFix.Repair(player.FixSpeed * GameData.frameDuration);
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )
                      .Start();
                  if (generatorForFix.DegreeOfRepair >= GameData.degreeOfFixedGenerator)
                  {
                      gameMap.NumOfRepairedGenerators++;
                      if (player.PlayerState == PlayerStateType.Fixing) player.PlayerState = PlayerStateType.Null;
                      if (gameMap.NumOfRepairedGenerators == GameData.numOfGeneratorRequiredForEmergencyExit)
                      {
                          gameMap.GameObjLockDict[GameObjType.EmergencyExit].EnterWriteLock();
                          try
                          {
                              Random r = new Random(Environment.TickCount);
                              ((EmergencyExit)(gameMap.GameObjDict[GameObjType.EmergencyExit][r.Next(0, gameMap.GameObjDict[GameObjType.EmergencyExit].Count)])).CanOpen = true;
                          }
                          finally
                          {
                              gameMap.GameObjLockDict[GameObjType.EmergencyExit].ExitWriteLock();
                          }
                      }
                      else
                      if (gameMap.NumOfRepairedGenerators == GameData.numOfGeneratorRequiredForRepair)
                      {
                          gameMap.GameObjLockDict[GameObjType.Doorway].EnterWriteLock();
                          try
                          {
                              foreach (Doorway doorway in gameMap.GameObjDict[GameObjType.Doorway])
                                  doorway.PowerSupply = true;
                          }
                          finally
                          {
                              gameMap.GameObjLockDict[GameObjType.Doorway].ExitWriteLock();
                          }
                      }

                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool OpenDoorway(Student player)
            {
                if (!(player.Commandable()) || player.PlayerState == PlayerStateType.OpeningTheDoorway)
                    return false;
                Doorway? doorwayToOpen = (Doorway?)gameMap.OneForInteract(player.Position, GameObjType.Doorway);
                if (doorwayToOpen == null || doorwayToOpen.IsOpening || !doorwayToOpen.PowerSupply)
                    return false;

                player.PlayerState = PlayerStateType.OpeningTheDoorway;
                doorwayToOpen.IsOpening = true;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => player.PlayerState == PlayerStateType.OpeningTheDoorway && gameMap.Timer.IsGaming && doorwayToOpen.OpenDegree < GameData.degreeOfOpenedDoorway,
                      loopToDo: () =>
                      {
                          doorwayToOpen.OpenDegree += GameData.frameDuration;
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )

                      .Start();
                  doorwayToOpen.IsOpening = false;
                  if (doorwayToOpen.OpenDegree >= GameData.degreeOfOpenedDoorway)
                  {
                      if (player.PlayerState == PlayerStateType.OpeningTheDoorway)
                          player.PlayerState = PlayerStateType.Null;
                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool Escape(Student player)
            {
                if (!(player.Commandable()))
                    return false;
                Doorway? doorwayForEscape = (Doorway?)gameMap.OneInTheSameCell(player.Position, GameObjType.Doorway);
                if (doorwayForEscape != null)
                {
                    if (doorwayForEscape.IsOpen())
                    {
                        player.Die(PlayerStateType.Escaped);
                        return true;
                    }
                    else
                        return false;
                }
                else
                {
                    EmergencyExit? emergencyExit = (EmergencyExit?)gameMap.OneInTheSameCell(player.Position, GameObjType.EmergencyExit);
                    if (emergencyExit != null && emergencyExit.IsOpen)
                    {
                        player.Die(PlayerStateType.Escaped);
                        return true;
                    }
                    else
                        return false;
                }
            }

            public bool Treat(Student player, Student playerTreated)
            {
                if ((!player.Commandable()) || player.PlayerState == PlayerStateType.Treating ||
                    (!playerTreated.Commandable()) ||
                    playerTreated.HP == playerTreated.MaxHp || !GameData.ApproachToInteract(playerTreated.Position, player.Position))
                    return false;

                if (playerTreated.HP + playerTreated.DegreeOfTreatment >= playerTreated.MaxHp)
                {
                    playerTreated.HP = playerTreated.MaxHp;
                    playerTreated.DegreeOfTreatment = 0;
                    return false;
                }

                if (playerTreated.DegreeOfTreatment >= GameData.basicTreatmentDegree)
                {
                    playerTreated.HP += GameData.basicTreatmentDegree;
                    playerTreated.DegreeOfTreatment = 0;
                    return false;
                }
                new Thread
           (
               () =>
               {
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => playerTreated.PlayerState == PlayerStateType.Treated && player.PlayerState == PlayerStateType.Treating && gameMap.Timer.IsGaming && playerTreated.HP + playerTreated.DegreeOfTreatment < playerTreated.MaxHp && playerTreated.DegreeOfTreatment < GameData.basicTreatmentDegree && GameData.ApproachToInteract(playerTreated.Position, player.Position),
                       loopToDo: () =>
                       {
                           playerTreated.DegreeOfTreatment += GameData.frameDuration * player.TreatSpeed;
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0
                   )

                       .Start();

                   if (playerTreated.PlayerState == PlayerStateType.Treated) playerTreated.PlayerState = PlayerStateType.Null;
                   if (player.PlayerState == PlayerStateType.Treating) player.PlayerState = PlayerStateType.Null;

                   if (playerTreated.HP + playerTreated.DegreeOfTreatment >= playerTreated.MaxHp)
                   {
                       playerTreated.HP = playerTreated.MaxHp;
                       playerTreated.DegreeOfTreatment = 0;
                   }
                   else
                   if (playerTreated.DegreeOfTreatment >= GameData.basicTreatmentDegree)
                   {
                       playerTreated.HP += GameData.basicTreatmentDegree;
                       playerTreated.DegreeOfTreatment = 0;
                   }
               }
           )
                { IsBackground = true }.Start();
                return true;
            }
            public bool Rescue(Student player, Student playerRescued)
            {
                if ((!player.Commandable()) || playerRescued.PlayerState != PlayerStateType.Addicted || !GameData.ApproachToInteract(playerRescued.Position, player.Position))
                    return false;
                player.PlayerState = PlayerStateType.Rescuing;
                playerRescued.PlayerState = PlayerStateType.Rescued;
                player.TimeOfRescue = 0;
                new Thread
           (
               () =>
               {
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => playerRescued.PlayerState == PlayerStateType.Rescued && player.PlayerState == PlayerStateType.Rescuing && gameMap.Timer.IsGaming && GameData.ApproachToInteract(playerRescued.Position, player.Position),
                       loopToDo: () =>
                       {
                           player.TimeOfRescue += GameData.frameDuration;
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0,
                       maxTotalDuration: GameData.basicTimeOfRescue
                   )
                       .Start();

                   if (playerRescued.PlayerState == PlayerStateType.Rescued) playerRescued.PlayerState = PlayerStateType.Null;
                   if (player.PlayerState == PlayerStateType.Rescuing) player.PlayerState = (player.TimeOfRescue >= GameData.basicTimeOfRescue) ? PlayerStateType.Null : PlayerStateType.Addicted;
                   player.TimeOfRescue = 0;
               }
           )
                { IsBackground = true }.Start();

                return true;
            }
            public bool OpenChest(Character player)
            {
                if ((!player.Commandable()) || player.PlayerState == PlayerStateType.OpeningTheChest)
                    return false;
                Chest? chestToOpen = (Chest?)gameMap.OneForInteract(player.Position, GameObjType.Chest);

                if (chestToOpen == null || chestToOpen.IsOpen() || chestToOpen.OpenDegree > 0)
                    return false;

                player.PlayerState = PlayerStateType.OpeningTheChest;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => player.PlayerState == PlayerStateType.OpeningTheChest && gameMap.Timer.IsGaming && (!chestToOpen.IsOpen()),
                      loopToDo: () =>
                      {
                          chestToOpen.OpenDegree += GameData.frameDuration * player.SpeedOfOpenChest;
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )

                      .Start();

                  if (chestToOpen.IsOpen())
                  {
                      if (player.PlayerState == PlayerStateType.OpeningTheChest)
                          player.PlayerState = PlayerStateType.Null;
                      for (int i = 0; i < GameData.maxNumOfPropInChest; ++i)
                      {
                          Prop prop = chestToOpen.PropInChest[i];
                          chestToOpen.PropInChest[i] = new NullProp();
                          prop.ReSetPos(player.Position, gameMap.GetPlaceType(player.Position));
                          gameMap.Add(prop);
                      }
                  }
                  else chestToOpen.OpenDegree = 0;

              }

          )
                { IsBackground = true }.Start();

                return true;
            }
            public bool ClimbingThroughWindow(Character player)
            {
                if (!player.Commandable())
                    return false;
                Window? windowForClimb = (Window?)gameMap.OneForInteractInACross(player.Position, GameObjType.Window);

                if (windowForClimb == null || windowForClimb.WhoIsClimbing != null)
                    return false;

                player.PlayerState = PlayerStateType.ClimbingThroughWindows;
                windowForClimb.WhoIsClimbing = player;
                XY windowToPlayer = new XY(
                      (Math.Abs(player.Position.x - windowForClimb.Position.x) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.x > windowForClimb.Position.x ? 1 : -1)) : 0,
                      (Math.Abs(player.Position.y - windowForClimb.Position.y) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.y > windowForClimb.Position.y ? 1 : -1)) : 0)
                    ;
                new Thread
          (
              () =>
              {

                  new FrameRateTaskExecutor<int>(
                    loopCondition: () => player.PlayerState == PlayerStateType.ClimbingThroughWindows && gameMap.Timer.IsGaming,
                    loopToDo: () => { },
                    timeInterval: GameData.frameDuration,
                    finallyReturn: () => 0,
                    maxTotalDuration: (int)((windowToPlayer + windowForClimb.Position - player.Position).Length() / player.MoveSpeed)
                    )
                    .Start();
                  if (player.PlayerState != PlayerStateType.ClimbingThroughWindows)
                  {
                      windowForClimb.WhoIsClimbing = null;
                      return;
                  }

                  player.ReSetPos(windowToPlayer + windowForClimb.Position, PlaceType.Window);
                  player.MoveSpeed = player.SpeedOfClimbingThroughWindows;
                  MovePlayer(player, (int)(windowToPlayer.Length() * 3.0 / player.MoveSpeed), (-1 * windowToPlayer).Angle());
                  new FrameRateTaskExecutor<int>(
                    loopCondition: () => player.PlayerState == PlayerStateType.ClimbingThroughWindows && player.IsMoving && gameMap.Timer.IsGaming,
                    loopToDo: () => { },
                    timeInterval: GameData.frameDuration,
                    finallyReturn: () => 0,
                    maxTotalDuration: (int)(windowToPlayer.Length() * 3.0 / player.MoveSpeed)
                    )
                    .Start();
                  XY PosJumpOff = windowForClimb.Position - 2 * windowToPlayer;
                  player.ReSetPos(PosJumpOff, gameMap.GetPlaceType(PosJumpOff));
                  player.MoveSpeed = player.ReCalculateBuff(BuffType.AddSpeed, player.OrgMoveSpeed, GameData.MaxSpeed, GameData.MinSpeed);
                  windowForClimb.WhoIsClimbing = null;
                  if (player.PlayerState == PlayerStateType.ClimbingThroughWindows)
                  {
                      player.PlayerState = PlayerStateType.Null;
                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }
            public bool LockOrOpenDoor(Character player)
            {
                if (!(player.Commandable()) || player.PlayerState == PlayerStateType.LockingOrOpeningTheDoor)
                    return false;
                Door? doorToLock = (Door?)gameMap.OneForInteract(player.Position, GameObjType.Door);
                if (doorToLock == null || doorToLock.OpenOrLockDegree > 0)
                    return false;
                bool flag = false;
                foreach (Prop prop in player.PropInventory)
                {
                    switch (prop.GetPropType())
                    {
                        case PropType.Key3:
                            if (doorToLock.Place == PlaceType.Door3)
                                flag = true;
                            break;
                        case PropType.Key5:
                            if (doorToLock.Place == PlaceType.Door5)
                                flag = true;
                            break;
                        case PropType.Key6:
                            if (doorToLock.Place == PlaceType.Door6)
                                flag = true;
                            break;
                        default:
                            break;
                    }
                    if (flag) break;
                }
                if (!flag) return false;

                player.PlayerState = PlayerStateType.LockingOrOpeningTheDoor;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => flag && player.PlayerState == PlayerStateType.LockingOrOpeningTheDoor && gameMap.Timer.IsGaming && doorToLock.OpenOrLockDegree < GameData.degreeOfLockingOrOpeningTheDoor,
                      loopToDo: () =>
                      {
                          flag = ((gameMap.OneInTheSameCell(doorToLock.Position, GameObjType.Character)) != null);
                          doorToLock.OpenOrLockDegree += GameData.frameDuration * player.SpeedOfOpeningOrLocking;
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )

                      .Start();
                  if (doorToLock.OpenOrLockDegree >= GameData.degreeOfLockingOrOpeningTheDoor)
                  {
                      doorToLock.IsOpen = (!doorToLock.IsOpen);
                  }
                  if (player.PlayerState == PlayerStateType.LockingOrOpeningTheDoor)
                      player.PlayerState = PlayerStateType.Null;
                  doorToLock.OpenOrLockDegree = 0;
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
            private readonly MoveEngine moveEngine;
            public ActionManager(Map gameMap)
            {
                this.gameMap = gameMap;
                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collisionObj, moveVec) =>
                    {
                        //if (collisionObj is Mine)
                        //{
                        //    ActivateMine((Character)obj, (Mine)collisionObj);
                        //    return MoveEngine.AfterCollision.ContinueCheck;
                        //}
                        return MoveEngine.AfterCollision.MoveMax;
                    },
                    EndMove: obj =>
                    {
                        // Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                    }
                );
            }
        }
    }
}
