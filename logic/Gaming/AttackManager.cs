using System;
using System.Threading;
using System.Collections.Generic;
using GameClass.GameObj;
using Preparation.Utility;
using GameEngine;
using Preparation.Interface;
using Timothy.FrameRateTask;
using System.Numerics;

namespace Gaming
{
    public partial class Game
    {
        private readonly AttackManager attackManager;
        private class AttackManager
        {
            readonly Map gameMap;
            readonly MoveEngine moveEngine;
            public AttackManager(Map gameMap)
            {
                this.gameMap = gameMap;
                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collisionObj, moveVec) =>
                    {
                        //BulletBomb((Bullet)obj, (GameObj)collisionObj);
                        return MoveEngine.AfterCollision.Destroyed;
                    },
                    EndMove: obj =>
                    {
#if DEBUG
                        Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);

#endif
                        BulletBomb((Bullet)obj, null);
                    }
                );
            }

            private void BeAddictedToGame(Student player)
            {
                new Thread
                    (() =>
                    {
                        if (player.GamingAddiction > GameData.BeginGamingAddiction && player.GamingAddiction < GameData.MidGamingAddiction)
                            player.GamingAddiction = GameData.MidGamingAddiction;
                        player.PlayerState = PlayerStateType.IsAddicted;
                        new FrameRateTaskExecutor<int>(
                            () => player.PlayerState == PlayerStateType.IsAddicted && player.GamingAddiction < player.MaxGamingAddiction && gameMap.Timer.IsGaming,
                            () =>
                            {
                                player.GamingAddiction += GameData.frameDuration;
                            },
                            timeInterval: GameData.frameDuration,
                            () =>
                            {
                                if (player.GamingAddiction == player.MaxGamingAddiction && gameMap.Timer.IsGaming)
                                {
                                    Die(player);
                                }
                                return 0;
                            }
                        )
                            .Start();
                    }
                    )
                { IsBackground = true }.Start();
            }

            public void BeStunned(Character player, int time)
            {
                new Thread
                    (() =>
                    {
                        player.PlayerState = PlayerStateType.IsStunned;
                        new FrameRateTaskExecutor<int>(
                            () => player.PlayerState == PlayerStateType.IsStunned && gameMap.Timer.IsGaming,
                            () =>
                            {
                            },
                            timeInterval: GameData.frameDuration,
                            () =>
                            {
                                if (player.PlayerState == PlayerStateType.IsStunned)
                                    player.PlayerState = PlayerStateType.Null;
                                return 0;
                            },
                            maxTotalDuration: time
                        )
                            .Start();
                    }
                    )
                { IsBackground = true }.Start();
            }

            private void Die(Character player)
            {

                player.Die(PlayerStateType.IsDeceased);
                // gameMap.GameObjLockDict[GameObjType.Character].EnterWriteLock();
                // try
                //{
                //     gameMap.GameObjDict[GameObjType.Character].Remove(playerBeingShot);
                // }
                // finally
                //{
                //     gameMap.GameObjLockDict[GameObjType.Character].ExitWriteLock();
                // }

                Prop? dropProp = null;
                if (player.PropInventory != null)  // 若角色原来有道具，则原始道具掉落在原地
                {
                    dropProp = player.PropInventory;
                    XY res = GameData.GetCellCenterPos(player.Position.x / GameData.numOfPosGridPerCell, player.Position.y / GameData.numOfPosGridPerCell);
                    dropProp.ReSetPos(res, gameMap.GetPlaceType(res));
                }

                if (dropProp != null)
                    gameMap.Add(dropProp);

                //  player.Reset();
                //    ((Character?)bullet.Parent)?.AddScore(GameData.addScoreWhenKillOneLevelPlayer);  // 给击杀者加分

                /*    new Thread
                        (() =>
                        {

                            Thread.Sleep(GameData.reviveTime);

                            playerBeingShot.AddShield(GameData.shieldTimeAtBirth);  // 复活加个盾

                            // gameMap.GameObjLockDict[GameObjType.Character].EnterWriteLock();
                            // try
                            //{
                            //     gameMap.GameObjDict[GameObjType.Character].Add(playerBeingShot);
                            // }
                            // finally { gameMap.GameObjLockDict[GameObjType.Character].ExitWriteLock(); }

                            if (gameMap.Timer.IsGaming)
                            {
                                playerBeingShot.CanMove = true;
                            }
                            playerBeingShot.IsDeceased = false;
                        }
                        )
                    { IsBackground = true }.Start();
                */
            }

            private void BombObj(Bullet bullet, GameObj objBeingShot)
            {
                switch (objBeingShot.Type)
                {
                    case GameObjType.Character:

                        if ((!((Character)objBeingShot).IsGhost()) && bullet.Parent.IsGhost())
                            if (((Character)objBeingShot).BeAttacked(bullet))
                            {
                                BeAddictedToGame((Student)objBeingShot);
                            }
                        if (((Character)objBeingShot).IsGhost() && !bullet.Parent.IsGhost() && bullet.TypeOfBullet == BulletType.Ram)
                            BeStunned((Character)objBeingShot, bullet.AP);
                        break;
                    default:
                        break;
                }
            }

            private void BulletBomb(Bullet bullet, GameObj? objBeingShot)
            {
#if DEBUG
                Debugger.Output(bullet, "bombed!");
#endif
                bullet.CanMove = false;

                if (gameMap.Remove(bullet) && bullet.IsToBomb)
                    gameMap.Add(new BombedBullet(bullet));

                if (!bullet.IsToBomb)
                {
                    if (objBeingShot == null)
                    {
                        if (bullet.Backswing > 0)
                        {
                            bullet.Parent.PlayerState = PlayerStateType.IsSwinging;

                            new Thread
                                    (() =>
                                    {
                                        Thread.Sleep(bullet.Backswing);

                                        if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.IsSwinging)
                                        {
                                            bullet.Parent.PlayerState = PlayerStateType.Null;
                                        }
                                    }
                                    )
                            { IsBackground = true }.Start();
                        }
                        return;
                    }


                    BombObj(bullet, objBeingShot);
                    if (bullet.RecoveryFromHit > 0)
                    {
                        bullet.Parent.PlayerState = PlayerStateType.IsSwinging;

                        new Thread
                                (() =>
                                {

                                    Thread.Sleep(bullet.RecoveryFromHit);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.IsSwinging)
                                    {
                                        bullet.Parent.PlayerState = PlayerStateType.Null;
                                    }
                                }
                                )
                        { IsBackground = true }.Start();
                    }
                    return;
                }

                /*if (objBeingShot != null)
                {
                    else if (objBeingShot is Bullet)        //子弹不能相互引爆，若要更改这一设定，取消注释即可。
                    {
                        new Thread(() => { BulletBomb((Bullet)objBeingShot, null); }) { IsBackground = true }.Start();
                    }
                }*/

                // 子弹爆炸会发生的事↓↓↓
                var beAttackedList = new List<IGameObj>();

                foreach (var kvp in gameMap.GameObjDict)
                {
                    if (bullet.CanBeBombed(kvp.Key))
                    {
                        gameMap.GameObjLockDict[kvp.Key].EnterReadLock();
                        try
                        {
                            foreach (var item in gameMap.GameObjDict[kvp.Key])
                                if (bullet.CanAttack((GameObj)item))
                                {
                                    beAttackedList.Add(item);
                                }

                        }
                        finally
                        {
                            gameMap.GameObjLockDict[kvp.Key].ExitReadLock();
                        }
                    }
                }

                foreach (GameObj beAttackedObj in beAttackedList)
                {
                    BombObj(bullet, beAttackedObj);
                }
                if (objBeingShot == null)
                {
                    if (bullet.Backswing > 0)
                    {
                        bullet.Parent.PlayerState = PlayerStateType.IsSwinging;

                        new Thread
                                (() =>
                                {
                                    Thread.Sleep(bullet.Backswing);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.IsSwinging)
                                    {
                                        bullet.Parent.PlayerState = PlayerStateType.Null;
                                    }
                                }
                                )
                        { IsBackground = true }.Start();
                    }
                }
                else
                {
                    if (bullet.RecoveryFromHit > 0)
                    {
                        bullet.Parent.PlayerState = PlayerStateType.IsSwinging;

                        new Thread
                                (() =>
                                {

                                    Thread.Sleep(bullet.RecoveryFromHit);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.IsSwinging)
                                    {
                                        bullet.Parent.PlayerState = PlayerStateType.Null;
                                    }
                                }
                                )
                        { IsBackground = true }.Start();
                    }
                }
                beAttackedList.Clear();
            }

            public bool Attack(Character? player, double angle)  // 射出去的子弹泼出去的水（狗头）
            {                                                    // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (player == null)
                {
#if DEBUG
                    Console.WriteLine("the player who will attack is NULL!");
#endif
                    return false;
                }

                if (player.PlayerState != PlayerStateType.Null || player.PlayerState != PlayerStateType.IsMoving)
                    return false;

                XY res = new XY  // 子弹紧贴人物生成。
                    (
                        (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Cos(angle)),
                        (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Sin(angle))
                    );

                Bullet? bullet = player.Attack(res, gameMap.GetPlaceType(res));

                if (bullet != null)
                {
                    bullet.CanMove = true;
                    gameMap.Add(bullet);
                    moveEngine.MoveObj(bullet, (int)((bullet.BulletAttackRange - player.Radius - BulletFactory.BulletRadius(player.BulletOfPlayer)) * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms


                    if (bullet.CastTime > 0)
                    {
                        player.PlayerState = PlayerStateType.IsTryingToAttack;

                        new Thread
                                (() =>
                                {
                                    new FrameRateTaskExecutor<int>(
                                    loopCondition: () => player.PlayerState == PlayerStateType.IsTryingToAttack && gameMap.Timer.IsGaming,
                                    loopToDo: () =>
                                    {
                                    },
                                    timeInterval: GameData.frameDuration,
                                    finallyReturn: () => 0,
                                    maxTotalDuration: bullet.CastTime
                      )

                          .Start();

                                    if (gameMap.Timer.IsGaming)
                                    {
                                        if (player.PlayerState == PlayerStateType.IsTryingToAttack)
                                        {
                                            player.PlayerState = PlayerStateType.Null;
                                        }
                                        else
                                            bullet.IsMoving = false;
                                        gameMap.Remove(bullet);
                                    }
                                }
                                )
                        { IsBackground = true }.Start();
                    }
                }
                if (bullet != null)
                {
#if DEBUG
                    Console.WriteLine($"playerID:{player.ID} successfully attacked!");
#endif
                    return true;
                }
                else
                {
#if DEBUG
                    Console.WriteLine($"playerID:{player.ID} has no bullets so that he can't attack!");
#endif
                    return false;
                }
            }
        }
    }
}