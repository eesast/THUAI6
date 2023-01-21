using System;
using GameClass.GameObj;
using GameEngine;
using Preparation.GameData;
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

            public bool Fix(Character player, Generator? generator = null)
            {
                if (player.IsResetting)
                    return false;


                Prop? pickProp = null;
                if (propType == PropType.Null)  // 自动检查有无道具可捡
                {
                    gameMap.GameObjLockDict[GameObjIdx.Prop].EnterReadLock();
                    try
                    {
                        foreach (Prop prop in gameMap.GameObjDict[GameObjIdx.Prop])
                        {
                            if (GameData.IsInTheSameCell(prop.Position, player.Position) && prop.CanMove == false)
                            {
                                pickProp = prop;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjIdx.Prop].ExitReadLock();
                    }
                }
                else
                {
                    gameMap.GameObjLockDict[GameObjIdx.Prop].EnterReadLock();
                    try
                    {
                        foreach (Prop prop in gameMap.GameObjDict[GameObjIdx.Prop])
                        {
                            if (prop.GetPropType() == propType)
                            {
                                if (GameData.IsInTheSameCell(prop.Position, player.Position) && prop.CanMove == false)
                                {
                                    pickProp = prop;
                                }
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjIdx.Prop].ExitReadLock();
                    }
                }

                if (pickProp != null)
                {
                    // pickProp.CanMove = false;
                    Prop? dropProp = null;
                    if (player.PropInventory != null)  // 若角色原来有道具，则原始道具掉落在原地
                    {
                        dropProp = player.PropInventory;
                        dropProp.SetNewPos(GameData.GetCellCenterPos(player.Position.x / GameData.numOfPosGridPerCell, player.Position.y / GameData.numOfPosGridPerCell));
                    }
                    player.PropInventory = pickProp;
                    gameMap.GameObjLockDict[GameObjIdx.Prop].EnterWriteLock();
                    try
                    {
                        gameMap.GameObjDict[GameObjIdx.Prop].Remove(pickProp);
                        if (dropProp != null)
                            gameMap.GameObjDict[GameObjIdx.Prop].Add(dropProp);
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjIdx.Prop].ExitWriteLock();
                    }
                    gameMap.GameObjLockDict[GameObjIdx.PickedProp].EnterWriteLock();
                    try
                    {
                        gameMap.GameObjDict[GameObjIdx.PickedProp].Add(new PickedProp(pickProp));
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjIdx.PickedProp].ExitWriteLock();
                    }

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

            // private readonly Map gameMap;
            private readonly MoveEngine moveEngine;
            public ActionManager(Map gameMap)
            {
                // this.gameMap = gameMap;
                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collisionObj, moveVec) =>
                    {
                        //if (collisionObj is Mine)
                        //{
                        //    ActivateMine((Character)obj, (Mine)collisionObj);
                        //    return MoveEngine.AfterCollision.ContinueCheck;
                        //}
                        return MoveEngine.AfterCollision.MoveMax; },
                    EndMove: obj =>
                    {
                        // Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                    }
                );
            }
        }
    }
}
