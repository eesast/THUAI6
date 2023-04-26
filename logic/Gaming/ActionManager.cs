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
            private void SkillWhenColliding(Character player, IGameObj collisionObj)
            {
                if (collisionObj.Type == GameObjType.Bullet)
                {
                    if (((Bullet)collisionObj).Parent != player && ((Bullet)collisionObj).TypeOfBullet == BulletType.JumpyDumpty)
                    {
                        if (characterManager.BeStunned((Character)player, GameData.timeOfStunnedWhenJumpyDumpty))
                            player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.timeOfStunnedWhenJumpyDumpty));
                        gameMap.Remove((GameObj)collisionObj);
                    }
                }
                if (player.FindIActiveSkill(ActiveSkillType.CanBeginToCharge).IsBeingUsed && collisionObj.Type == GameObjType.Character && ((Character)collisionObj).IsGhost())
                {
                    if (characterManager.BeStunned((Character)collisionObj, GameData.timeOfGhostStunnedWhenCharge))
                        player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.timeOfGhostStunnedWhenCharge));
                    characterManager.BeStunned(player, GameData.timeOfStudentStunnedWhenCharge);
                }
            }
            public bool MovePlayer(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (moveTimeInMilliseconds < 5) return false;
                if (!playerToMove.Commandable() || !TryToStop()) return false;
                characterManager.SetPlayerState(playerToMove, PlayerStateType.Moving);
                moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection);
                return true;
            }

            public bool MovePlayerWhenStunned(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (!playerToMove.Commandable() && playerToMove.PlayerState != PlayerStateType.Stunned) return false;
                characterManager.BeStunned(playerToMove, moveTimeInMilliseconds);
                moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection);
                return true;
            }

            public bool Stop(Character player)
            {
                if (player.Commandable() || !TryToStop())
                {
                    characterManager.SetPlayerState(player);
                    return true;
                }
                return false;
            }

            public bool Fix(Student player)// 自动检查有无发电机可修
            {
                if (player.CharacterType == CharacterType.Teacher || (!player.Commandable()) || player.PlayerState == PlayerStateType.Fixing)
                    return false;
                Generator? generatorForFix = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);

                if (generatorForFix == null || generatorForFix.DegreeOfRepair == GameData.degreeOfFixedGenerator)
                    return false;

                ++generatorForFix.NumOfFixing;
                characterManager.SetPlayerState(player, PlayerStateType.Fixing);
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => gameMap.Timer.IsGaming && player.PlayerState == PlayerStateType.Fixing,
                      loopToDo: () =>
                      {
                          if (generatorForFix.Repair(player.FixSpeed * GameData.frameDuration, player))
                              gameMap.NumOfRepairedGenerators++;
                          if (generatorForFix.DegreeOfRepair == GameData.degreeOfFixedGenerator)
                              characterManager.SetPlayerState(player);
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )
                      .Start();
                  --generatorForFix.NumOfFixing;
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
                if (doorwayToOpen == null || doorwayToOpen.OpenStartTime > 0 || !doorwayToOpen.PowerSupply)
                    return false;

                characterManager.SetPlayerState(player, PlayerStateType.OpeningTheDoorway, doorwayToOpen);
                int startTime = doorwayToOpen.OpenStartTime = gameMap.Timer.nowTime();
                new Thread
          (
              () =>
              {
                  Thread.Sleep(GameData.degreeOfOpenedDoorway - doorwayToOpen.OpenDegree);

                  if (doorwayToOpen.OpenStartTime == startTime)
                  {
                      doorwayToOpen.OpenDegree = GameData.degreeOfOpenedDoorway;
                      player.SetPlayerStateNaturally();
                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool Escape(Student player)
            {
                if (!(player.Commandable()) || player.CharacterType == CharacterType.Robot || player.CharacterType == CharacterType.Teacher)
                    return false;
                Doorway? doorwayForEscape = (Doorway?)gameMap.OneForInteract(player.Position, GameObjType.Doorway);
                if (doorwayForEscape != null && doorwayForEscape.IsOpen())
                {
                    player.AddScore(GameData.StudentScoreEscape);
                    ++gameMap.NumOfEscapedStudent;
                    player.RemoveFromGame(PlayerStateType.Escaped);
                    return true;
                }
                else
                {
                    EmergencyExit? emergencyExit = (EmergencyExit?)gameMap.OneForInteract(player.Position, GameObjType.EmergencyExit);
                    if (emergencyExit != null && emergencyExit.IsOpen)
                    {
                        player.AddScore(GameData.StudentScoreEscape);
                        ++gameMap.NumOfEscapedStudent;
                        player.RemoveFromGame(PlayerStateType.Escaped);
                        return true;
                    }
                    return false;
                }
            }

            public bool Treat(Student player, Student? playerTreated = null)
            {
                if (playerTreated == null)
                {
                    playerTreated = gameMap.StudentForInteract(player.Position);
                    if (playerTreated == null) return false;
                }
                if (player == playerTreated || (!player.Commandable()) || playerTreated.PlayerState == PlayerStateType.Treated ||
                    (!playerTreated.Commandable()) ||
                    playerTreated.HP == playerTreated.MaxHp || !GameData.ApproachToInteract(playerTreated.Position, player.Position))
                    return false;

                new Thread
           (
               () =>
               {
                   characterManager.SetPlayerState(playerTreated, PlayerStateType.Treated);
                   characterManager.SetPlayerState(player, PlayerStateType.Treating);
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => playerTreated.PlayerState == PlayerStateType.Treated && player.PlayerState == PlayerStateType.Treating && gameMap.Timer.IsGaming,
                       loopToDo: () =>
                       {
                           if (playerTreated.AddDegreeOfTreatment(GameData.frameDuration * player.TreatSpeed, player))
                               characterManager.SetPlayerState(playerTreated);
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0
                   )
                       .Start();

                   if (player.PlayerState == PlayerStateType.Treating) characterManager.SetPlayerState(player);
                   else if (playerTreated.PlayerState == PlayerStateType.Treated) characterManager.SetPlayerState(playerTreated);
               }
           )
                { IsBackground = true }.Start();
                return true;
            }
            public bool Rescue(Student player, Student? playerRescued = null)
            {
                if (playerRescued == null)
                {
                    playerRescued = gameMap.StudentForInteract(player.Position);
                    if (playerRescued == null) return false;
                }
                if ((!player.Commandable()) || playerRescued.PlayerState != PlayerStateType.Addicted || !GameData.ApproachToInteract(playerRescued.Position, player.Position))
                    return false;
                characterManager.SetPlayerState(player, PlayerStateType.Rescuing);
                characterManager.SetPlayerState(playerRescued, PlayerStateType.Rescued);

                new Thread
           (
               () =>
               {
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => playerRescued.PlayerState == PlayerStateType.Rescued && player.PlayerState == PlayerStateType.Rescuing && gameMap.Timer.IsGaming,
                       loopToDo: () =>
                       {
                           playerRescued.TimeOfRescue += GameData.frameDuration;
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0,
                       maxTotalDuration: GameData.basicTimeOfRescue
                   )
                       .Start();

                   if (playerRescued.PlayerState == PlayerStateType.Rescued)
                   {
                       if (playerRescued.TimeOfRescue >= GameData.basicTimeOfRescue)
                       {
                           characterManager.SetPlayerState(playerRescued);
                           playerRescued.HP = playerRescued.MaxHp / 2;
                           player.AddScore(GameData.StudentScoreRescue);
                       }
                       else
                           characterManager.SetPlayerState(playerRescued, PlayerStateType.Addicted);
                   }
                   if (player.PlayerState == PlayerStateType.Rescuing) characterManager.SetPlayerState(player);
                   playerRescued.TimeOfRescue = 0;
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

                if (chestToOpen == null || chestToOpen.OpenStartTime > 0)
                    return false;

                characterManager.SetPlayerState(player, PlayerStateType.OpeningTheChest, chestToOpen);
                int startTime = gameMap.Timer.nowTime();
                chestToOpen.Open(startTime, player);
                new Thread
          (
              () =>
              {
                  Thread.Sleep(GameData.degreeOfOpenedChest / player.SpeedOfOpenChest);

                  if (chestToOpen.OpenStartTime == startTime)
                  {
                      player.SetPlayerStateNaturally();
                      for (int i = 0; i < GameData.maxNumOfPropInChest; ++i)
                      {
                          Prop prop = chestToOpen.PropInChest[i];
                          chestToOpen.PropInChest[i] = new NullProp();
                          prop.ReSetPos(player.Position, gameMap.GetPlaceType(player.Position));
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
                if (!player.Commandable())
                    return false;
                Window? windowForClimb = (Window?)gameMap.OneForInteractInACross(player.Position, GameObjType.Window);

                if (windowForClimb == null || windowForClimb.WhoIsClimbing != null)
                    return false;

                XY windowToPlayer = new(
                      (Math.Abs(player.Position.x - windowForClimb.Position.x) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.x > windowForClimb.Position.x ? 1 : -1)) : 0,
                      (Math.Abs(player.Position.y - windowForClimb.Position.y) > GameData.numOfPosGridPerCell / 2) ? (GameData.numOfPosGridPerCell / 2 * (player.Position.y > windowForClimb.Position.y ? 1 : -1)) : 0);

                /*       Character? characterInWindow = (Character?)gameMap.OneInTheSameCell(windowForClimb.Position - 2 * windowToPlayer, GameObjType.Character);
                       if (characterInWindow != null)
                       {
                           if (player.IsGhost() && !characterInWindow.IsGhost())
                               characterManager.BeAttacked((Student)(characterInWindow), player.Attack(characterInWindow.Position, PlaceType.Null));
                           return false;
                       }*/

                //Wall addWall = new Wall(windowForClimb.Position - 2 * windowToPlayer);
                // gameMap.Add(addWall);

                characterManager.SetPlayerState(player, PlayerStateType.ClimbingThroughWindows);
                windowForClimb.WhoIsClimbing = player;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                  loopCondition: () => player.PlayerState == PlayerStateType.ClimbingThroughWindows && gameMap.Timer.IsGaming,
                  loopToDo: () => { },
                  timeInterval: GameData.frameDuration,
                  finallyReturn: () => 0,
                  maxTotalDuration: (int)((windowToPlayer + windowForClimb.Position - player.Position).Length() * 1000 / player.MoveSpeed)
                  )
                  .Start();
                  if (player.PlayerState != PlayerStateType.ClimbingThroughWindows)
                  {
                      windowForClimb.WhoIsClimbing = null;
                      return;
                  }

                  player.ReSetPos(windowToPlayer + windowForClimb.Position, PlaceType.Window);
                  player.MoveSpeed = player.SpeedOfClimbingThroughWindows;

                  moveEngine.MoveObj(player, (int)(windowToPlayer.Length() * 3.0 * 1000 / player.MoveSpeed), (-1 * windowToPlayer).Angle());

                  new FrameRateTaskExecutor<int>(
                    loopCondition: () => player.PlayerState == PlayerStateType.ClimbingThroughWindows && gameMap.Timer.IsGaming,
                    loopToDo: () =>
                    {
                    },
                    timeInterval: GameData.frameDuration,
                    finallyReturn: () => 0,
                    maxTotalDuration: (int)(windowToPlayer.Length() * 3.0 * 1000 / player.MoveSpeed)
                    )
                    .Start();
                  XY PosJumpOff = windowForClimb.Position - 2 * windowToPlayer;
                  player.ReSetPos(PosJumpOff, gameMap.GetPlaceType(PosJumpOff));
                  player.MoveSpeed = player.ReCalculateBuff(BuffType.AddSpeed, player.OrgMoveSpeed, GameData.MaxSpeed, GameData.MinSpeed);
                  windowForClimb.WhoIsClimbing = null;
                  //  gameMap.Remove(addWall);
                  if (player.PlayerState == PlayerStateType.ClimbingThroughWindows)
                  {
                      characterManager.SetPlayerState(player);
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
                if (doorToLock == null || doorToLock.OpenOrLockDegree > 0 || gameMap.PartInTheSameCell(doorToLock.Position, GameObjType.Character) != null)
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

                characterManager.SetPlayerState(player, PlayerStateType.LockingOrOpeningTheDoor);
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => flag && player.PlayerState == PlayerStateType.LockingOrOpeningTheDoor && gameMap.Timer.IsGaming && doorToLock.OpenOrLockDegree < GameData.degreeOfLockingOrOpeningTheDoor,
                      loopToDo: () =>
                      {
                          flag = ((gameMap.PartInTheSameCell(doorToLock.Position, GameObjType.Character)) == null);
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
                      characterManager.SetPlayerState(player);
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
            private object numLock = new object();
            private int lastTime = 0;
            private int numStop = 0;
            private int NumStop => numStop;
            private bool TryToStop()
            {
                lock (numLock)
                {
                    int time = gameMap.Timer.nowTime();
                    if (time / GameData.frameDuration > lastTime)
                    {
                        lastTime = time / GameData.frameDuration;
                        numStop = 1;
                        return true;
                    }
                    else
                    {
                        if (numStop == GameData.LimitOfStopAndMove)
                            return false;
                        else
                        {
                            ++numStop;
                            return true;
                        }
                    }
                }
            }

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
                        SkillWhenColliding((Character)obj, collisionObj);
                        Preparation.Utility.Debugger.Output(obj, " end move with " + collisionObj.ToString());
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
                this.characterManager = characterManager;
            }
        }
    }
}
