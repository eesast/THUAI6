using System;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using Timothy.FrameRateTask;

namespace GameEngine
{
    public class MoveEngine
    {
        /// <summary>
        /// 碰撞结束后要做的事情
        /// </summary>
        public enum AfterCollision
        {
            ContinueCheck = 0,  // 碰撞后继续检查其他碰撞,暂时没用
            MoveMax = 1,        // 行走最远距离
            Destroyed = 2       // 物体已经毁坏
        }

        private readonly ITimer gameTimer;
        private readonly Action<IMoveable> EndMove;

        public IGameObj? CheckCollision(IMoveable obj, XY Pos)
        {
            return collisionChecker.CheckCollision(obj, Pos);
        }

        private readonly CollisionChecker collisionChecker;
        private readonly Func<IMoveable, IGameObj, XY, AfterCollision> OnCollision;
        /// <summary>
        /// Constrctor
        /// </summary>
        /// <param name="gameMap">游戏地图</param>
        /// <param name="OnCollision">发生碰撞时要做的事情，第一个参数为移动的物体，第二个参数为撞到的物体，第三个参数为移动的位移向量，返回值见AfterCollision的定义</param>
        /// <param name="EndMove">结束碰撞时要做的事情</param>
        public MoveEngine(
            IMap gameMap,
            Func<IMoveable, IGameObj, XY, AfterCollision> OnCollision,
            Action<IMoveable> EndMove
        )
        {
            this.gameTimer = gameMap.Timer;
            this.EndMove = EndMove;
            this.OnCollision = OnCollision;
            this.collisionChecker = new CollisionChecker(gameMap);
        }

        /// <summary>
        /// 在无碰撞的前提下行走最远的距离
        /// </summary>
        /// <param name="obj">移动物体，默认obj.Rigid为true</param>
        /// <param name="moveVec">移动的位移向量</param>
        private bool MoveMax(IMoveable obj, XY moveVec, long stateNum)
        {
            /*由于四周是墙，所以人物永远不可能与越界方块碰撞*/
            XY nextPos = obj.Position + moveVec;
            double maxLen = collisionChecker.FindMax(obj, nextPos, moveVec);
            maxLen = Math.Min(maxLen, obj.MoveSpeed / GameData.numOfStepPerSecond);
            return (obj.MovingSetPos(new XY(moveVec, maxLen), stateNum)) >= 0;
        }

        private bool LoopDo(IMoveable obj, double direction, ref double deltaLen, long stateNum)
        {
            double moveVecLength = obj.MoveSpeed / GameData.numOfStepPerSecond;
            XY res = new(direction, moveVecLength);

            // 越界情况处理：如果越界，则与越界方块碰撞
            bool flag;  // 循环标志
            do
            {
                flag = false;
                IGameObj? collisionObj = collisionChecker.CheckCollisionWhenMoving(obj, res);
                if (collisionObj == null)
                    break;

                switch (OnCollision(obj, collisionObj, res))
                {
                    case AfterCollision.ContinueCheck:
                        flag = true;
                        break;
                    case AfterCollision.Destroyed:
                        Debugger.Output(obj, " collide with " + collisionObj.ToString() + " and has been removed from the game.");
                        return false;
                    case AfterCollision.MoveMax:
                        if (!MoveMax(obj, res, stateNum)) return false;
                        moveVecLength = 0;
                        res = new XY(direction, moveVecLength);
                        break;
                }
            } while (flag);

            long moveL = obj.MovingSetPos(res, stateNum);
            if (moveL == -1) return false;
            deltaLen = deltaLen + moveVecLength - Math.Sqrt(moveL);
            return true;
        }

        public void MoveObj(IMoveable obj, int moveTime, double direction, long stateNum)
        {
            if (!gameTimer.IsGaming) return;
            lock (obj.ActionLock)
            {
                if (!obj.IsAvailableForMove) { EndMove(obj); return; }
                obj.IsMoving = true;
            }
            new Thread
            (
                () =>
                {
                    double moveVecLength = 0.0;
                    XY res = new(direction, moveVecLength);
                    double deltaLen = (double)0.0;  // 转向，并用deltaLen存储行走的误差
                    IGameObj? collisionObj = null;
                    bool isEnded = false;

                    bool flag;  // 循环标志
                    do
                    {
                        flag = false;
                        collisionObj = collisionChecker.CheckCollision(obj, obj.Position);
                        if (collisionObj == null)
                            break;

                        switch (OnCollision(obj, collisionObj, res))
                        {
                            case AfterCollision.ContinueCheck:
                                flag = true;
                                break;
                            case AfterCollision.Destroyed:
                                Debugger.Output(obj, " collide with " + collisionObj.ToString() + " and has been removed from the game.");
                                isEnded = true;
                                break;
                            case AfterCollision.MoveMax:
                                break;
                        }
                    } while (flag);

                    if (isEnded)
                    {
                        obj.IsMoving = false;
                        EndMove(obj);
                        return;
                    }
                    else
                    {
                        if (moveTime >= GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond)
                        {
                            Thread.Sleep(GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond);
                            new FrameRateTaskExecutor<int>(
                                () => gameTimer.IsGaming,
                                () =>
                                {
                                    if (obj.StateNum == stateNum && obj.CanMove && !obj.IsRemoved)
                                        return !(isEnded = true);
                                    return !(isEnded = !LoopDo(obj, direction, ref deltaLen, stateNum));
                                },
                                GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond,
                                () =>
                                {
                                    return 0;
                                },
                                maxTotalDuration: moveTime - GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond
                            )
                            {
                                AllowTimeExceed = true,
                                MaxTolerantTimeExceedCount = ulong.MaxValue,
                                TimeExceedAction = b =>
                                {
                                    if (b)
                                        Console.WriteLine("Fatal Error: The computer runs so slow that the object cannot finish moving during this time!!!!!!");

#if DEBUG
                                    else
                                    {
                                        Console.WriteLine("Debug info: Object moving time exceed for once.");
                                    }
#endif
                                }
                            }.Start();
                            if (!isEnded && obj.StateNum == stateNum && obj.CanMove && !obj.IsRemoved)
                                isEnded = !LoopDo(obj, direction, ref deltaLen, stateNum);
                        }
                        if (isEnded)
                        {
                            obj.IsMoving = false;
                            EndMove(obj);
                            return;
                        }
                        if (obj.StateNum == stateNum && obj.CanMove && !obj.IsRemoved)
                        {
                            int leftTime = moveTime % (GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond);
                            if (leftTime > 0)
                            {
                                Thread.Sleep(leftTime);  // 多移动的在这里补回来
                            }
                            do
                            {
                                flag = false;
                                moveVecLength = (double)deltaLen + leftTime * obj.MoveSpeed / GameData.numOfPosGridPerCell;
                                res = new XY(direction, moveVecLength);
                                if ((collisionObj = collisionChecker.CheckCollisionWhenMoving(obj, res)) == null)
                                {
                                    obj.MovingSetPos(res, stateNum);
                                }
                                else
                                {
                                    switch (OnCollision(obj, collisionObj, res))
                                    {
                                        case AfterCollision.ContinueCheck:
                                            flag = true;
                                            break;
                                        case AfterCollision.Destroyed:
                                            Debugger.Output(obj, " collide with " + collisionObj.ToString() + " and has been removed from the game.");
                                            isEnded = true;
                                            break;
                                        case AfterCollision.MoveMax:
                                            MoveMax(obj, res, stateNum);
                                            moveVecLength = 0;
                                            res = new XY(direction, moveVecLength);
                                            break;
                                    }
                                }
                            } while (flag);
                        }
                        obj.IsMoving = false;  // 结束移动
                        EndMove(obj);
                    }
                }
            ).Start();
        }
    }
}
