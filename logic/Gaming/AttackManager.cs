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
                        obj.ReSetCanMove(false);
                    }
                );
                this.characterManager = characterManager;
            }

            public void ProduceBulletNaturally(BulletType bulletType, Character player, double angle, XY pos)
            {
                // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (bulletType == BulletType.Null) return;
                Bullet? bullet = BulletFactory.GetBullet(player, pos);
                if (bullet == null) return;
                Debugger.Output(bullet, "Attack in " + pos.ToString());
                gameMap.Add(bullet);
                moveEngine.MoveObj(bullet, (int)(bullet.AttackDistance * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms
            }

            private void BombObj(Bullet bullet, GameObj objBeingShot)
            {
#if DEBUG
                Debugger.Output(bullet, "bombed " + objBeingShot.ToString());
#endif
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
                    default:
                        break;
                }
            }

            public bool TryRemoveBullet(Bullet bullet)
            {
                if (gameMap.Remove(bullet))
                {
                    bullet.ReSetCanMove(false);
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
                    double angle = bullet.FacingDirection.Angle() + Math.PI / 2.0;
                    XY pos = bullet.Position + new XY  // 子弹紧贴人物生成。
                    (
                    (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Cos(angle))) * Math.Sign(Math.Cos(angle)),
                    (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Sin(angle))) * Math.Sign(Math.Sin(angle))
                    );
                    ProduceBulletNaturally(BulletType.JumpyDumpty, (Character)bullet.Parent, angle, pos);

                    angle = bullet.FacingDirection.Angle() + Math.PI * 3.0 / 2.0;
                    pos = bullet.Position + new XY  // 子弹紧贴人物生成。
                    (
                    (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Cos(angle))) * Math.Sign(Math.Cos(angle)),
                    (int)(Math.Abs((bullet.Radius + BulletFactory.BulletRadius(BulletType.JumpyDumpty)) * Math.Sin(angle))) * Math.Sign(Math.Sin(angle))
                    );
                    ProduceBulletNaturally(BulletType.JumpyDumpty, (Character)bullet.Parent, angle, pos);
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
                Bullet? bullet = player.Attack(angle, gameMap.Timer.nowTime());

                if (bullet != null)
                {
                    Debugger.Output(bullet, "Attack in " + bullet.Position.ToString());
                    bullet.AP += player.TryAddAp() ? GameData.ApPropAdd : 0;
                    gameMap.Add(bullet);
                    moveEngine.MoveObj(bullet, (int)(bullet.AttackDistance * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms
                    if (bullet.CastTime > 0)
                    {
                        characterManager.SetPlayerState(player, PlayerStateType.TryingToAttack);
                        long threadNum = player.StateNum;

                        new Thread
                                (() =>
                                {
                                    new FrameRateTaskExecutor<int>(
                                    loopCondition: () => threadNum == player.StateNum && gameMap.Timer.IsGaming,
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
                                        if (threadNum == player.StateNum)
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