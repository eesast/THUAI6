using System;
using System.Threading;
using System.Collections.Generic;
using GameClass.GameObj;
using Preparation.GameData;
using Preparation.Utility;
using GameEngine;

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
                         return MoveEngine.AfterCollision.Destroyed; },
                    EndMove: obj =>
                    {
#if DEBUG
                         Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);

#endif
                         BulletBomb((Bullet)obj, null); }
                );
            }
            private void BombOnePlayer(Bullet bullet, Character playerBeingShot)
            {
                if (playerBeingShot.BeAttack(bullet))
                {
                    playerBeingShot.CanMove = false;
                    playerBeingShot.IsResetting = true;
                    // gameMap.GameObjLockDict[GameObjIdx.Player].EnterWriteLock();
                    // try
                    //{
                    //     gameMap.GameObjDict[GameObjIdx.Player].Remove(playerBeingShot);
                    // }
                    // finally
                    //{
                    //     gameMap.GameObjLockDict[GameObjIdx.Player].ExitWriteLock();
                    // }                    

                    Prop? dropProp = null;
                    if (playerBeingShot.PropInventory != null)  // 若角色原来有道具，则原始道具掉落在原地
                    {
                        dropProp = playerBeingShot.PropInventory;
                        dropProp.SetNewPos(GameData.GetCellCenterPos(playerBeingShot.Position.x / GameData.numOfPosGridPerCell, playerBeingShot.Position.y / GameData.numOfPosGridPerCell));
                    }
                    gameMap.GameObjLockDict[GameObjIdx.Prop].EnterWriteLock();
                    try
                    {
                        if (dropProp != null)
                            gameMap.GameObjDict[GameObjIdx.Prop].Add(dropProp);
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjIdx.Prop].ExitWriteLock();
                    }

                    playerBeingShot.Reset();
                    ((Character?)bullet.Parent)?.AddScore(GameData.addScoreWhenKillOneLevelPlayer);  // 给击杀者加分

                    new Thread
                        (() =>
                        {

                            Thread.Sleep(GameData.reviveTime);

                    playerBeingShot.AddShield(GameData.shieldTimeAtBirth);  // 复活加个盾

                    // gameMap.GameObjLockDict[GameObjIdx.Player].EnterWriteLock();
                    // try
                    //{
                    //     gameMap.GameObjDict[GameObjIdx.Player].Add(playerBeingShot);
                    // }
                    // finally { gameMap.GameObjLockDict[GameObjIdx.Player].ExitWriteLock(); }

                    if (gameMap.Timer.IsGaming)
                    {
                        playerBeingShot.CanMove = true;
                    }
                    playerBeingShot.IsResetting = false;
                }
                        )
                    { IsBackground = true }.Start();
            }
        }
        private void BulletBomb(Bullet bullet, GameObj? objBeingShot)
        {
#if DEBUG
            Debugger.Output(bullet, "bombed!");
#endif
            bullet.CanMove = false;
            gameMap.GameObjLockDict[GameObjIdx.Bullet].EnterWriteLock();
            try
            {
                        foreach (ObjOfCharacter _bullet in gameMap.GameObjDict[GameObjIdx.Bullet])
                        {
                    if (_bullet.ID == bullet.ID)
                    {
                        gameMap.GameObjLockDict[GameObjIdx.BombedBullet].EnterWriteLock();
                        try
                        {
                            gameMap.GameObjDict[GameObjIdx.BombedBullet].Add(new BombedBullet(bullet));
                        }
                        finally
                        {
                            gameMap.GameObjLockDict[GameObjIdx.BombedBullet].ExitWriteLock();
                        }
                        gameMap.GameObjDict[GameObjIdx.Bullet].Remove(_bullet);
                        break;
                    }
                        }
            }
            finally
            {
                        gameMap.GameObjLockDict[GameObjIdx.Bullet].ExitWriteLock();
            }

            /*if (objBeingShot != null)
            {
                else if (objBeingShot is Bullet)        //子弹不能相互引爆，若要更改这一设定，取消注释即可。
                {
                    new Thread(() => { BulletBomb((Bullet)objBeingShot, null); }) { IsBackground = true }.Start();
                }
            }*/

            // 子弹爆炸会发生的事↓↓↓
            var beAttackedList = new List<Character>();
            gameMap.GameObjLockDict[GameObjIdx.Player].EnterReadLock();
            try
            {
                        foreach (Character player in gameMap.GameObjDict[GameObjIdx.Player])
                        {
                    if (bullet.CanAttack(player))
                    {
                        beAttackedList.Add(player);
                        if (player.ID != bullet.Parent.ID)
                            bullet.Parent.HP = (int)(bullet.Parent.HP + (bullet.Parent.Vampire * bullet.AP));
                    }
                        }
            }
            finally
            {
                        gameMap.GameObjLockDict[GameObjIdx.Player].ExitReadLock();
            }

            foreach (Character beAttackedPlayer in beAttackedList)
            {
                        BombOnePlayer(bullet, beAttackedPlayer);
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

            if (player.IsResetting)
                        return false;
            Bullet? bullet = player.RemoteAttack(
                new XY  // 子弹紧贴人物生成。
                (
                    (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Cos(angle)),
                    (int)((player.Radius + BulletFactory.BulletRadius(player.BulletOfPlayer)) * Math.Sin(angle))
                )
            );
            if (bullet != null)
            {
                        bullet.CanMove = true;
                        gameMap.GameObjLockDict[GameObjIdx.Bullet].EnterWriteLock();
                        try
                        {
                    gameMap.GameObjDict[GameObjIdx.Bullet].Add(bullet);
                        }
                        finally
                        {
                    gameMap.GameObjLockDict[GameObjIdx.Bullet].ExitWriteLock();
                        }
                        moveEngine.MoveObj(bullet, (int)((player.AttackRange - player.Radius - BulletFactory.BulletRadius(player.BulletOfPlayer)) * 1000 / bullet.MoveSpeed), angle);  // 这里时间参数除出来的单位要是ms
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
