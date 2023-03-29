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
                        BulletBomb((Bullet)obj, (GameObj)collisionObj);
                        return MoveEngine.AfterCollision.Destroyed;
                    },
                    EndMove: obj =>
                    {
#if DEBUG
                        Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);

#endif
                        if (obj.CanMove && ((Bullet)obj).TypeOfBullet != BulletType.JumpyDumpty)
                            BulletBomb((Bullet)obj, null);
                    }
                );
            }

            private void BeAddictedToGame(Student player, Ghost ghost)
            {
                ghost.AddScore(GameData.TrickerScoreStudentBeAddicted);
                new Thread
                    (() =>
                    {
                        if (player.GamingAddiction > GameData.BeginGamingAddiction && player.GamingAddiction < GameData.MidGamingAddiction)
                            player.GamingAddiction = GameData.MidGamingAddiction;
                        player.PlayerState = PlayerStateType.Addicted;
#if DEBUG
                        Debugger.Output(player, " is addicted ");
#endif
                        new FrameRateTaskExecutor<int>(
                            () => (player.PlayerState == PlayerStateType.Addicted || player.PlayerState == PlayerStateType.Rescued) && player.GamingAddiction < player.MaxGamingAddiction && gameMap.Timer.IsGaming,
                            () =>
                            {
                                player.GamingAddiction += (player.PlayerState == PlayerStateType.Addicted) ? GameData.frameDuration : 0;
                            },
                            timeInterval: GameData.frameDuration,
                            () =>
                            {
                                if (player.GamingAddiction == player.MaxGamingAddiction && gameMap.Timer.IsGaming)
                                {
                                    ghost.AddScore(GameData.TrickerScoreStudentDie);
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

            public static bool BeStunned(Character player, int time)
            {
                if (player.PlayerState == PlayerStateType.Stunned || player.NoHp()) return false;
                new Thread
                    (() =>
                    {
                        player.PlayerState = PlayerStateType.Stunned;
                        Thread.Sleep(time);
                        if (player.PlayerState == PlayerStateType.Stunned)
                            player.PlayerState = PlayerStateType.Null;
                    }
                    )
                { IsBackground = true }.Start();
                return true;
            }

            private void Die(Character player)
            {
#if DEBUG
                Debugger.Output(player, "die.");
#endif
                player.Die(PlayerStateType.Deceased);

                for (int i = 0; i < GameData.maxNumOfPropInPropInventory; i++)
                {
                    Prop? prop = player.UseProp(i);
                    if (prop != null)
                    {
                        prop.ReSetPos(player.Position, gameMap.GetPlaceType(player.Position));
                        gameMap.Add(prop);
                    }
                }
                ++gameMap.NumOfDeceasedStudent;
                if (GameData.numOfStudent - gameMap.NumOfDeceasedStudent - gameMap.NumOfEscapedStudent == 1)
                {
                    gameMap.GameObjLockDict[GameObjType.EmergencyExit].EnterReadLock();
                    try
                    {
                        foreach (EmergencyExit emergencyExit in gameMap.GameObjDict[GameObjType.EmergencyExit])
                            if (emergencyExit.CanOpen)
                            {
                                emergencyExit.IsOpen = true;
                                break;
                            }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.EmergencyExit].ExitReadLock();
                    }
                }
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
                            Student whoBeAttacked = (Student)objBeingShot;
                            if (whoBeAttacked.BeAttacked(bullet))
                            {
                                BeAddictedToGame(whoBeAttacked, (Ghost)bullet.Parent);
                            }
                            if (whoBeAttacked.CanBeAwed())
                            {
                                if (BeStunned(whoBeAttacked, GameData.basicStunnedTimeOfStudent))
                                    bullet.Parent.AddScore(GameData.TrickerScoreStudentBeStunned);
                            }
                        }
                        //       if (((Character)objBeingShot).IsGhost() && !bullet.Parent.IsGhost() && bullet.TypeOfBullet == BulletType.Ram)
                        //          BeStunned((Character)objBeingShot, bullet.AP);
                        break;
                    case GameObjType.Generator:
                        if (bullet.CanBeBombed(GameObjType.Generator))
                            ((Generator)objBeingShot).DegreeOfRepair -= bullet.AP * GameData.factorDamageGenerator;
                        break;
                    default:
                        break;
                }
            }

            private void BulletBomb(Bullet bullet, GameObj? objBeingShot)
            {
#if DEBUG
                if (objBeingShot != null)
                    Debugger.Output(bullet, "bombed with" + objBeingShot.ToString());
                else
                    Debugger.Output(bullet, "bombed without objBeingShot");
#endif
                bullet.CanMove = false;

                if (gameMap.Remove(bullet) && bullet.BulletBombRange > 0)
                    gameMap.Add(new BombedBullet(bullet));

                if (bullet.BulletBombRange == 0)
                {
                    if (objBeingShot == null)
                    {
                        if (bullet.Backswing > 0)
                        {
                            bullet.Parent.PlayerState = PlayerStateType.Swinging;

                            new Thread
                                    (() =>
                                    {
                                        Thread.Sleep(bullet.Backswing);

                                        if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.Swinging)
                                        {
                                            bullet.Parent.PlayerState = PlayerStateType.Null;
                                        }
                                    }
                                    )
                            { IsBackground = true }.Start();
                        }
                        return;
                    }

                    Debugger.Output(bullet, bullet.TypeOfBullet.ToString());

                    BombObj(bullet, objBeingShot);
                    if (bullet.RecoveryFromHit > 0)
                    {
                        bullet.Parent.PlayerState = PlayerStateType.Swinging;

                        new Thread
                                (() =>
                                {
                                    Thread.Sleep(bullet.RecoveryFromHit);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.Swinging)
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

                if (bullet.TypeOfBullet == BulletType.BombBomb && objBeingShot != null)
                {
                    bullet.Parent.BulletOfPlayer = BulletType.JumpyDumpty;
                    Debugger.Output(bullet, "JumpyDumpty!");
                    Attack((Character)bullet.Parent, bullet.FacingDirection.Angle());
                    Attack((Character)bullet.Parent, bullet.FacingDirection.Angle() + Math.PI);
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
                if (objBeingShot == null)
                {
                    if (bullet.Backswing > 0)
                    {
                        bullet.Parent.PlayerState = PlayerStateType.Swinging;

                        new Thread
                                (() =>
                                {
                                    Thread.Sleep(bullet.Backswing);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.Swinging)
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
                        bullet.Parent.PlayerState = PlayerStateType.Swinging;

                        new Thread
                                (() =>
                                {

                                    Thread.Sleep(bullet.RecoveryFromHit);

                                    if (gameMap.Timer.IsGaming && bullet.Parent.PlayerState == PlayerStateType.Swinging)
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

            public bool Attack(Character? player, double angle)
            {                                                    // 子弹如果没有和其他物体碰撞，将会一直向前直到超出人物的attackRange
                if (player == null)
                {
                    return false;
                }

                if (!player.Commandable())
                    return false;

                XY res = player.Position + new XY  // 子弹紧贴人物生成。
                    (
                        (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Cos(angle)),
                        (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Sin(angle))
                    );

                Bullet? bullet = player.Attack(res, gameMap.GetPlaceType(res));

                if (bullet != null)
                {
                    Debugger.Output(player, "Attack in" + bullet.ToString());
                    bullet.AP += player.TryAddAp() ? GameData.ApPropAdd : 0;
                    bullet.CanMove = true;
                    gameMap.Add(bullet);
                    moveEngine.MoveObj(bullet, (int)((bullet.BulletAttackRange - player.Radius - BulletFactory.BulletRadius(player.BulletOfPlayer)) * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms

                    if (bullet.CastTime > 0)
                    {
                        player.PlayerState = PlayerStateType.TryingToAttack;

                        new Thread
                                (() =>
                                {
                                    new FrameRateTaskExecutor<int>(
                                    loopCondition: () => player.PlayerState == PlayerStateType.TryingToAttack && gameMap.Timer.IsGaming,
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
                                        if (player.PlayerState == PlayerStateType.TryingToAttack)
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