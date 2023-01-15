using System.Collections.Generic;
using GameClass.GameObj;
using System.Threading;
using Preparation.GameData;
using Preparation.Utility;
using System;
using Timothy.FrameRateTask;
using GameEngine;

namespace Gaming
{
    public partial class Game
    {
        private readonly PropManager propManager;
        private class PropManager
        {
            private readonly Map gameMap;

            private MoveEngine moveEngine;

            private bool isProducingProp = false;

            private readonly List<XY> availableCellForGenerateProp;
            public void StartProducing()
            {
                if (isProducingProp)
                    return;
                isProducingProp = true;
                ProduceProp();
            }

            public void UseProp(Character player)
            {
                if (player.IsResetting)
                    return;
                Prop? prop = player.UseProp();
                switch (prop?.GetPropType())
                {
                    case PropType.Spear:
                        player.AddSpear(GameData.PropDuration);
                        break;
                    case PropType.Shield:
                        player.AddShield(GameData.PropDuration);
                        break;
                    case PropType.addLIFE:
                        player.AddLIFE(GameData.PropDuration);
                        break;
                    case PropType.addSpeed:
                        player.AddMoveSpeed(GameData.PropDuration);
                        break;
                    default:
                        break;
                }
            }

            /// <summary>
            ///
            /// </summary>
            /// <param name="player"></param>
            /// <param name="propType">若不指定，则自动判断可捡起什么道具</param>
            /// <returns></returns>
            public bool PickProp(Character player, PropType propType = PropType.Null)
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

            public void ThrowProp(Character player, int timeInMilliseconds, double angle)
            {
                if (!gameMap.Timer.IsGaming)
                    return;
                if (player.IsResetting)  // 移动中也能扔，但由于“惯性”，可能初始位置会有点变化
                    return;
                Prop? prop = player.UseProp();
                if (prop == null)
                    return;

                prop.CanMove = true;
                prop.SetNewPos(player.Position);
                gameMap.GameObjLockDict[GameObjIdx.Prop].EnterWriteLock();
                try
                {
                    gameMap.GameObjDict[GameObjIdx.Prop].Add(prop);
                }
                finally
                {
                    gameMap.GameObjLockDict[GameObjIdx.Prop].ExitWriteLock();
                }
                timeInMilliseconds = timeInMilliseconds < GameData.PropMaxMoveDistance / prop.MoveSpeed * 1000 ? timeInMilliseconds : GameData.PropMaxMoveDistance / prop.MoveSpeed * 1000;
                moveEngine.MoveObj(prop, timeInMilliseconds, angle);
            }
            private void ProduceProp()
            {
                int len = availableCellForGenerateProp.Count;
                Random r = new Random(Environment.TickCount);
                new Thread
                (
                    () =>
                    {
                        while (!gameMap.Timer.IsGaming)
                            Thread.Sleep(1000);
                        new FrameRateTaskExecutor<int>(
                            () => gameMap.Timer.IsGaming,
                            () =>
                            {
                                int rand = r.Next(0, len);
                                XY randPos = availableCellForGenerateProp[rand];

                                gameMap.GameObjLockDict[GameObjIdx.Prop].EnterWriteLock();
                                try
                                {
                                    switch (r.Next(0, 4))
                                    {
                                        case 0:
                                            gameMap.GameObjDict[GameObjIdx.Prop].Add(new AddLIFE(randPos));
                                            break;
                                        case 1:
                                            gameMap.GameObjDict[GameObjIdx.Prop].Add(new AddSpeed(randPos));
                                            break;
                                        case 2:
                                            gameMap.GameObjDict[GameObjIdx.Prop].Add(new Shield(randPos));
                                            break;
                                        case 3:
                                            gameMap.GameObjDict[GameObjIdx.Prop].Add(new Spear(randPos));
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                finally
                                {
                                    gameMap.GameObjLockDict[GameObjIdx.Prop].ExitWriteLock();
                                }
                            },
                            GameData.PropProduceTime,
                            () => 0
                        )
                            .Start();
                    }
                )
                { IsBackground = true }.Start();
            }
            public PropManager(Map gameMap)  // 道具不能扔过墙
            {
                this.gameMap = gameMap;
                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collision, moveVec) =>
                    { return MoveEngine.AfterCollision.MoveMax; },
                    EndMove: obj =>
                    {
                        // obj.Place = gameMap.GetPlaceType((GameObj)obj);
                        obj.CanMove = false;
                        Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                    }
                );
                availableCellForGenerateProp = new List<XY>();
                for (int i = 0; i < gameMap.ProtoGameMap.GetLength(0); i++)
                {
                    for (int j = 0; j < gameMap.ProtoGameMap.GetLength(1); j++)
                    {
                        if (gameMap.ProtoGameMap[i, j] == (int)MapInfoObjType.Null)
                        {
                            availableCellForGenerateProp.Add(GameData.GetCellCenterPos(i, j));
                        }
                    }
                }
            }
        }
    }
}
