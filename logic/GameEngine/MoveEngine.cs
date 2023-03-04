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
        private void MoveMax(IMoveable obj, XY moveVec)
        {

            /*由于四周是墙，所以人物永远不可能与越界方块碰撞*/
            XY nextPos = obj.Position + moveVec;
            double maxLen = collisionChecker.FindMax(obj, nextPos, moveVec);
            maxLen = Math.Min(maxLen, obj.MoveSpeed / GameData.numOfStepPerSecond);
            _ = obj.Move(new XY(moveVec.Angle(), maxLen));
        }

        public void MoveObj(IMoveable obj, int moveTime, double direction)
        {
            if (obj.IsMoving)  // 已经移动的物体不能再移动
                return;
            new Thread
            (
                () =>
                {
                    if (!obj.IsAvailable && gameTimer.IsGaming)
                        return;
                    lock (obj.MoveLock)
                        obj.IsMoving = true;

                    double moveVecLength = 0.0;
                    double deltaLen = moveVecLength - Math.Sqrt(obj.Move(new XY(direction, moveVecLength)));  // 转向，并用deltaLen存储行走的误差
                    IGameObj? collisionObj = null;
                    bool isDestroyed = false;
                    new FrameRateTaskExecutor<int>(
                        () => gameTimer.IsGaming && obj.CanMove && !obj.IsResetting && obj.IsMoving,
                        () =>
                        {
                            moveVecLength = obj.MoveSpeed / GameData.numOfStepPerSecond;

                            // 越界情况处理：如果越界，则与越界方块碰撞
                            bool flag;  // 循环标志
                            do
                            {
                                flag = false;
                                collisionObj = collisionChecker.CheckCollision(obj, new XY(direction, moveVecLength));
                                if (collisionObj == null)
                                    break;

                                switch (OnCollision(obj, collisionObj, new XY(direction, moveVecLength)))
                                {
                                    case AfterCollision.ContinueCheck:
                                        flag = true;
                                        break;
                                    case AfterCollision.Destroyed:
                                        Debugger.Output(obj, " collide with " + collisionObj.ToString() + " and has been removed from the game.");
                                        isDestroyed = true;
                                        return false;
                                    case AfterCollision.MoveMax:
                                        MoveMax(obj, new XY(direction, moveVecLength));
                                        moveVecLength = 0;
                                        break;
                                }
                            } while (flag);

                            deltaLen += moveVecLength - Math.Sqrt(obj.Move(new XY(direction, moveVecLength)));

                            return true;
                        },
                        GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond,
                        () =>
                        {
                            int leftTime = moveTime % (GameData.numOfPosGridPerCell / GameData.numOfStepPerSecond);
                            bool flag;
                            do
                            {
                                flag = false;
                                if (!isDestroyed)
                                {
                                    moveVecLength = deltaLen + leftTime * obj.MoveSpeed / GameData.numOfPosGridPerCell;
                                    if ((collisionObj = collisionChecker.CheckCollision(obj, new XY(direction, moveVecLength))) == null)
                                    {
                                        obj.Move(new XY(direction, moveVecLength));
                                    }
                                    else
                                    {
                                        switch (OnCollision(obj, collisionObj, new XY(direction, moveVecLength)))
                                        {
                                            case AfterCollision.ContinueCheck:
                                                flag = true;
                                                break;
                                            case AfterCollision.Destroyed:
                                                Debugger.Output(obj, " collide with " + collisionObj.ToString() + " and has been removed from the game.");
                                                isDestroyed = true;
                                                break;
                                            case AfterCollision.MoveMax:
                                                MoveMax(obj, new XY(direction, moveVecLength));
                                                moveVecLength = 0;
                                                break;
                                        }
                                    }
                                }
                            } while (flag);
                            if (leftTime > 0 && obj.IsMoving)
                            {
                                Thread.Sleep(leftTime);  // 多移动的在这里补回来
                            }
                            lock (obj.MoveLock)
                                obj.IsMoving = false;  // 结束移动
                            EndMove(obj);
                            return 0;
                        },
                        maxTotalDuration: moveTime
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
                }
            ).Start();
        }
    }
}
