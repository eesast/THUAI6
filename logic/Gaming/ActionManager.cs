using System;
using System.Runtime.InteropServices;
using GameClass.GameObj;
using GameEngine;
using Preparation.Utility;

namespace Gaming
{
    public partial class Game
    {
        private readonly ActionManager actionManager;
        private class ActionManager
        {

            // 人物移动
            public void MovePlayer(Character playerToMove, int moveTimeInMilliseconds, double moveDirection)
            {
                moveEngine.MoveObj(playerToMove, moveTimeInMilliseconds, moveDirection);
            }

            public bool TryToFix(Character player)// 自动检查有无发电机可修
            {
                if (player.IsResetting || player.IsGhost())
                    return false;
                Generator? generatorForFix = null;


                gameMap.GameObjLockDict[GameObjType.Generator].EnterReadLock();
                try
                {
                    foreach (Generator generator in gameMap.GameObjDict[GameObjType.Generator])
                    {
                        if (GameData.IsInTheSameCell(generator.Position, player.Position))
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


                if (generatorForFix != null)
                {
                    gameMap.GameObjLockDict[GameObjType.Generator].EnterReadLock();
                    try
                    {
                        if (generatorForFix.Repair(player.FixSpeed))
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
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Generator].ExitReadLock();
                    }
                    return true;
                }
                else
                    return false;
            }

            public bool TryToEscape(Character player)
            {
                if (player.IsResetting || player.IsGhost())
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
                    player.Escape();
                    return true;
                }
                else
                    return false;
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
