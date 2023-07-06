using System;
using System.Threading;
using System.Collections.Generic;
using GameClass.GameObj;
using Preparation.Utility;
using GameEngine;
using Preparation.Interface;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private readonly AttackManager attackManager;
        private class AttackManager
        {
            readonly Map gameMap;
            public readonly MoveEngine moveEngine;
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
                        obj.CanMove.SetReturnOri(false);
                    }
                );
                this.characterManager = characterManager;
            }

            public void ProduceBulletNaturally(BulletType bulletType, Character player, double angle, XY pos)
            {
                // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (bulletType == BulletType.Null) return;
                Bullet? bullet = BulletFactory.GetBullet(player, pos, bulletType);
                if (bullet == null) return;
                Debugger.Output(bullet, "Attack in " + pos.ToString());
                gameMap.Add(bullet);
                moveEngine.MoveObj(bullet, (int)(bullet.AttackDistance * 1000 / bullet.MoveSpeed), angle, ++bullet.StateNum);  // 这里时间参数除出来的单位要是ms
            }

            private void BombObj(Bullet bullet, GameObj objBeingShot)
            {
                Debugger.Output(bullet, "bombed " + objBeingShot.ToString());

                switch (objBeingShot.Type)
                {
                    case GameObjType.Character:

                        if ((!(((Character)objBeingShot).IsGhost())) && bullet.Parent!.IsGhost())
                        {
                            characterManager.BeAttacked((Student)objBeingShot, bullet);
                        }
                        //       if (((Character)objBeingShot).IsGhost() && !bullet.Parent.IsGhost() && bullet.TypeOfBullet == BulletType.Ram)
                        //          BeStunned((Character)objBeingShot, bullet.AP);
                        break;
                    case GameObjType.Generator:
                        if (bullet.CanBeBombed(GameObjType.Generator))
                            ((Generator)objBeingShot).Repair(-bullet.AP * GameData.factorDamageGenerator, (Character)bullet.Parent!);
                        break;
                    case GameObjType.Door:
                        if (bullet.CanBeBombed(GameObjType.Door))
                            ((Door)objBeingShot).ForceToOpen();
                        break;
                    case GameObjType.Item:
                        if (((Item)objBeingShot).GetPropType() == PropType.CraftingBench)
                        {
                            ((CraftingBench)objBeingShot).TryStopSkill();
                            gameMap.Remove(objBeingShot);
                        }
                        break;
                    default:
                        break;
                }
            }

            public bool TryRemoveBullet(Bullet bullet)
            {
                if (gameMap.Remove(bullet))
                {
                    bullet.CanMove.SetReturnOri(false);
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

            private void ProduceBombBomb(Bullet bullet, double angle)
            {
                angle += bullet.FacingDirection.Angle();
                XY pos = bullet.Position + new XY
                (
                (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Cos(angle))) * Math.Sign(Math.Cos(angle)),
                (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Sin(angle))) * Math.Sign(Math.Sin(angle))
                );
                ProduceBulletNaturally(BulletType.JumpyDumpty, (Character)bullet.Parent!, angle, pos);
            }

            private void BulletBomb(Bullet bullet, GameObj? objBeingShot)
            {
                if (objBeingShot != null)
                    Debugger.Output(bullet, "bombed with" + objBeingShot.ToString());
                else
                    Debugger.Output(bullet, "bombed without objBeingShot");

                if (!TryRemoveBullet(bullet)) return;

                if (bullet.BulletBombRange == 0)
                {
                    if (objBeingShot == null)
                    {
                        characterManager.BackSwing((Character)bullet.Parent!, bullet.Backswing);
                        return;
                    }

                    BombObj(bullet, objBeingShot);
                    characterManager.BackSwing((Character)bullet.Parent!, bullet.RecoveryFromHit);
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
                    ProduceBombBomb(bullet, Math.PI / 2);
                    ProduceBombBomb(bullet, Math.PI * 2 / 3);
                    ProduceBombBomb(bullet, Math.PI * 5 / 6);
                    ProduceBombBomb(bullet, Math.PI);
                    ProduceBombBomb(bullet, Math.PI * 7 / 6);
                    ProduceBombBomb(bullet, Math.PI * 4 / 3);
                    ProduceBombBomb(bullet, Math.PI * 3 / 2);
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
                    characterManager.BackSwing((Character)bullet.Parent!, bullet.Backswing);
                }
                else
                    characterManager.BackSwing((Character)bullet.Parent!, bullet.RecoveryFromHit);
            }

            public bool Attack(Character player, double angle)
            {                                                    // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (!player.Commandable()) return false;

                Bullet? bullet = player.Attack(angle);

                if (bullet != null)
                {
                    Debugger.Output(bullet, "Attack in " + bullet.Position.ToString());
                    gameMap.Add(bullet);

                    moveEngine.MoveObj(bullet, (int)(bullet.AttackDistance * 1000 / bullet.MoveSpeed), angle, ++bullet.StateNum);  // 这里时间参数除出来的单位要是ms

                    if (bullet.CastTime > 0)
                    {
                        long stateNum = player.SetPlayerState(RunningStateType.Waiting, PlayerStateType.TryingToAttack);
                        if (stateNum == -1)
                        {
                            TryRemoveBullet(bullet);
                            return false;
                        }

                        new Thread
                                (() =>
                                {
                                    player.ThreadNum.WaitOne();
                                    if (!player.StartThread(stateNum, RunningStateType.RunningActively))
                                    {
                                        TryRemoveBullet(bullet);
                                        player.ThreadNum.Release();
                                        return;
                                    }
                                    new FrameRateTaskExecutor<int>(
                                    loopCondition: () => stateNum == player.StateNum && gameMap.Timer.IsGaming,
                                    loopToDo: () =>
                                    {
                                    },
                                    timeInterval: GameData.checkInterval,
                                    finallyReturn: () => 0,
                                    maxTotalDuration: bullet.CastTime
                      )
                          .Start();

                                    player.ThreadNum.Release();
                                    if (gameMap.Timer.IsGaming)
                                    {
                                        if (!player.ResetPlayerState(stateNum))
                                            TryRemoveBullet(bullet);
                                    }
                                }
                                )
                        { IsBackground = true }.Start();
                    }
                }
                if (bullet != null)
                {
                    Debugger.Output($"playerID:{player.ID} successfully attacked!");
                    return true;
                }
                else
                {
                    Debugger.Output($"playerID:{player.ID} has no bullets so that he can't attack!");
                    return false;
                }
            }
        }
    }
}