﻿using System;
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
            readonly CharacterManager characterManager;

            public AttackManager(Map gameMap, CharacterManager characterManager)
            {
                this.gameMap = gameMap;
                this.moveEngine = new MoveEngine(
                    gameMap: gameMap,
                    OnCollision: (obj, collisionObj, moveVec) =>
                    {
                        BulletBomb((Bullet)obj, (GameObj)collisionObj);
                        return MoveEngine.AfterCollision.Destroyed;
                    },
                    EndMove: obj =>
                    {
                        Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                        if (obj.CanMove && ((Bullet)obj).TypeOfBullet != BulletType.JumpyDumpty)
                            BulletBomb((Bullet)obj, null);
                        obj.CanMove = false;
                    }
                );
                this.characterManager = characterManager;
            }

            private void BombObj(Bullet bullet, GameObj objBeingShot)
            {
#if DEBUG
                Debugger.Output(bullet, "bombed " + objBeingShot.ToString());
#endif
                switch (objBeingShot.Type)
                {
                    case GameObjType.Character:

                        if ((!(((Character)objBeingShot).IsGhost())) && bullet.Parent.IsGhost())
                        {
                            characterManager.BeAttacked((Student)objBeingShot, bullet);
                        }
                        //       if (((Character)objBeingShot).IsGhost() && !bullet.Parent.IsGhost() && bullet.TypeOfBullet == BulletType.Ram)
                        //          BeStunned((Character)objBeingShot, bullet.AP);
                        break;
                    case GameObjType.Generator:
                        if (bullet.CanBeBombed(GameObjType.Generator))
                            ((Generator)objBeingShot).Repair(-bullet.AP * GameData.factorDamageGenerator, (Character)bullet.Parent);
                        break;
                    default:
                        break;
                }
            }

            public bool TryRemoveBullet(Bullet bullet)
            {
                bullet.CanMove = false;
                if (gameMap.Remove(bullet))
                {
                    if (bullet.BulletBombRange > 0)
                    {
                        BombedBullet bombedBullet = new(bullet);
                        gameMap.Add(bombedBullet);
                        new Thread
                                    (() =>
                                    {
                                        Thread.Sleep(GameData.frameDuration * 5);
                                        gameMap.RemoveJustFromMap(bombedBullet);
                                    }
                                    )
                        { IsBackground = true }.Start();
                    }
                    return true;
                }
                else return false;
            }

            private void BulletBomb(Bullet bullet, GameObj? objBeingShot)
            {
#if DEBUG
                if (objBeingShot != null)
                    Debugger.Output(bullet, "bombed with" + objBeingShot.ToString());
                else
                    Debugger.Output(bullet, "bombed without objBeingShot");
#endif
                if (!TryRemoveBullet(bullet)) return;

                if (bullet.BulletBombRange == 0)
                {
                    if (objBeingShot == null)
                    {
                        characterManager.BackSwing((Character)bullet.Parent, bullet.Backswing);
                        return;
                    }

                    BombObj(bullet, objBeingShot);
                    characterManager.BackSwing((Character)bullet.Parent, bullet.RecoveryFromHit);
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

                if (bullet.TypeOfBullet == BulletType.BombBomb && objBeingShot != null)
                {
                    bullet.Parent.BulletOfPlayer = BulletType.JumpyDumpty;
                    Debugger.Output(bullet.Parent, bullet.Parent.CharacterType.ToString() + " " + bullet.Parent.BulletNum.ToString());
                    Attack((Character)bullet.Parent, bullet.FacingDirection.Angle() + Math.PI / 2.0);
                    Attack((Character)bullet.Parent, bullet.FacingDirection.Angle() + Math.PI * 3.0 / 2.0);
                }

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
                beAttackedList.Clear();

                if (objBeingShot == null)
                {
                    characterManager.BackSwing((Character)bullet.Parent, bullet.Backswing);
                }
                else
                    characterManager.BackSwing((Character)bullet.Parent, bullet.RecoveryFromHit);
            }

            public bool Attack(Character player, double angle)
            {                                                    // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (player.BulletOfPlayer == BulletType.Null)
                    return false;
                Debugger.Output(player, player.CharacterType.ToString() + "Attack in " + player.BulletOfPlayer.ToString());

                Debugger.Output(player, player.Position.ToString() + " " + player.Radius.ToString() + " " + BulletFactory.BulletRadius(player.BulletOfPlayer).ToString());
                XY res = player.Position + new XY  // 子弹紧贴人物生成。
                    (
                        (int)(Math.Abs((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Cos(angle))) * ((Math.Cos(angle) > 0) ? 1 : -1),
                        (int)(Math.Abs((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Sin(angle))) * ((Math.Sin(angle) > 0) ? 1 : -1)
                    );

                Bullet? bullet = player.Attack(res, gameMap.GetPlaceType(res));

                if (bullet != null)
                {
                    player.FacingDirection = new(angle, bullet.BulletAttackRange);
                    Debugger.Output(bullet, "Attack in " + bullet.Position.ToString());
                    bullet.AP += player.TryAddAp() ? GameData.ApPropAdd : 0;
                    bullet.CanMove = true;
                    gameMap.Add(bullet);
                    moveEngine.MoveObj(bullet, (int)((bullet.BulletAttackRange - player.Radius - BulletFactory.BulletRadius(player.BulletOfPlayer)) * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms
                    if (bullet.CastTime > 0)
                    {
                        characterManager.SetPlayerState(player, PlayerStateType.TryingToAttack);

                        new Thread
                                (() =>
                                {
                                    new FrameRateTaskExecutor<int>(
                                    loopCondition: () => player.PlayerState == PlayerStateType.TryingToAttack && gameMap.Timer.IsGaming,
                                    loopToDo: () =>
                                    {
                                    },
                                    timeInterval: GameData.checkInterval,
                                    finallyReturn: () => 0,
                                    maxTotalDuration: bullet.CastTime
                      )
                          .Start();

                                    if (gameMap.Timer.IsGaming)
                                    {
                                        if (player.PlayerState == PlayerStateType.TryingToAttack)
                                        {
                                            characterManager.SetPlayerState(player);
                                        }
                                        else TryRemoveBullet(bullet);
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