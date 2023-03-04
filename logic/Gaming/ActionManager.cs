using System;
using System.Runtime.InteropServices;
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

            // 人物移动
            public bool MovePlayer(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                if (playerToMove.PlayerState != PlayerStateType.Null) return false;
                moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection);
                return true;
            }

            public bool Fix(Student player)// 自动检查有无发电机可修
            {
                if (player.PlayerState != PlayerStateType.Null || player.IsGhost())
                    return false;
                Generator? generatorForFix = null;


                gameMap.GameObjLockDict[GameObjType.Generator].EnterReadLock();
                try
                {
                    foreach (Generator generator in gameMap.GameObjDict[GameObjType.Generator])
                    {
                        if (GameData.ApproachToInteract(generator.Position, player.Position))
                        {
                            generatorForFix = generator;
                            break;
                        }
                    }
                }
                finally
                {
                    gameMap.GameObjLockDict[GameObjType.Generator].ExitReadLock();
                }

                if (generatorForFix == null || generatorForFix.DegreeOfFRepair == GameData.degreeOfFixedGenerator)
                    return false;

                player.PlayerState = PlayerStateType.IsFixing;
                new Thread
          (
              () =>
              {
                  new FrameRateTaskExecutor<int>(
                      loopCondition: () => player.PlayerState == PlayerStateType.IsFixing && gameMap.Timer.IsGaming && generatorForFix.DegreeOfFRepair < GameData.degreeOfFixedGenerator && GameData.ApproachToInteract(player.Position, generatorForFix.Position),
                      loopToDo: () =>
                      {
                          return !generatorForFix.Repair(player.FixSpeed * GameData.frameDuration);
                      },
                      timeInterval: GameData.frameDuration,
                      finallyReturn: () => 0
                  )

                      .Start();
                  if (generatorForFix.DegreeOfFRepair == GameData.degreeOfFixedGenerator)
                  {
                      gameMap.GameObjLockDict[GameObjType.Generator].EnterReadLock();
                      try
                      {
                          Doorway exit = (Doorway)gameMap.GameObjDict[GameObjType.Doorway][1];
                          if (!exit.PowerSupply)
                          {
                              int numOfFixedGenerator = 0;
                              foreach (Generator generator in gameMap.GameObjDict[GameObjType.Generator])
                                  if (generator.DegreeOfFRepair == GameData.degreeOfFixedGenerator)
                                      ++numOfFixedGenerator;
                              if (numOfFixedGenerator >= GameData.numOfGeneratorRequiredForRepair)
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

                      finally
                      {
                          gameMap.GameObjLockDict[GameObjType.Generator].ExitReadLock();
                      }
                  }
              }

          )
                { IsBackground = true }.Start();

                return true;
            }

            public bool Escape(Student player)
            {
                if (!(player.PlayerState == PlayerStateType.Null || player.PlayerState == PlayerStateType.IsMoving) || player.IsGhost())
                    return false;
                Doorway? doorwayForEscape = null;

                gameMap.GameObjLockDict[GameObjType.Doorway].EnterReadLock();
                try
                {
                    foreach (Doorway doorway in gameMap.GameObjDict[GameObjType.Doorway])
                    {
                        if (GameData.IsInTheSameCell(doorway.Position, player.Position))
                        {
                            doorwayForEscape = doorway;
                            break;
                        }
                    }
                }
                finally
                {
                    gameMap.GameObjLockDict[GameObjType.Doorway].ExitReadLock();
                }


                if (doorwayForEscape != null && doorwayForEscape.IsOpen)
                {
                    player.Die(PlayerStateType.IsEscaped);
                    return true;
                }
                else
                    return false;
            }

            public bool Treat(Student player, Student playerTreated)
            {
                if (playerTreated.PlayerState == PlayerStateType.Null || player.PlayerState == PlayerStateType.Null || playerTreated.HP == playerTreated.MaxHp || !GameData.ApproachToInteract(playerTreated.Position, player.Position))
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
                       loopCondition: () => playerTreated.PlayerState == PlayerStateType.IsTreated && player.PlayerState == PlayerStateType.IsTreating && gameMap.Timer.IsGaming && playerTreated.HP + playerTreated.DegreeOfTreatment < playerTreated.MaxHp && playerTreated.DegreeOfTreatment >= GameData.basicTreatmentDegree && GameData.ApproachToInteract(playerTreated.Position, player.Position),
                       loopToDo: () =>
                       {
                           playerTreated.DegreeOfTreatment += GameData.frameDuration * player.TreatSpeed;
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0
                   )

                       .Start();

                   if (playerTreated.PlayerState == PlayerStateType.IsTreated) playerTreated.PlayerState = PlayerStateType.Null;
                   if (player.PlayerState == PlayerStateType.IsTreating) player.PlayerState = PlayerStateType.Null;

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
                if (player.PlayerState != PlayerStateType.Null || playerRescued.PlayerState != PlayerStateType.IsAddicted || !GameData.ApproachToInteract(playerRescued.Position, player.Position))
                    return false;
                player.PlayerState = PlayerStateType.IsRescuing;
                playerRescued.PlayerState = PlayerStateType.IsRescued;
                int rescuedDegree = 0;
                new Thread
           (
               () =>
               {
                   new FrameRateTaskExecutor<int>(
                       loopCondition: () => playerRescued.PlayerState == PlayerStateType.IsRescued && player.PlayerState == PlayerStateType.IsRescuing && gameMap.Timer.IsGaming && GameData.ApproachToInteract(playerRescued.Position, player.Position),
                       loopToDo: () =>
                       {
                           rescuedDegree += GameData.frameDuration;
                       },
                       timeInterval: GameData.frameDuration,
                       finallyReturn: () => 0,
                       maxTotalDuration: 1000
                   )

                       .Start();

                   if (rescuedDegree == 1000)
                   {
                       if (playerRescued.PlayerState == PlayerStateType.IsRescued) playerRescued.PlayerState = PlayerStateType.Null;
                       if (player.PlayerState == PlayerStateType.IsRescuing) player.PlayerState = PlayerStateType.Null;
                   }
                   else
                   {
                       if (playerRescued.PlayerState == PlayerStateType.IsRescued) playerRescued.PlayerState = PlayerStateType.Null;
                       if (player.PlayerState == PlayerStateType.IsRescuing) player.PlayerState = PlayerStateType.IsAddicted;
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
