using System;
using System.Collections.Generic;
using System.Numerics;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
namespace GameEngine
{
    internal class CollisionChecker
    {
        public IGameObj? CheckCollision(IMoveable obj, XY Pos)
        {
            // 在列表中检查碰撞
            Func<IEnumerable<IGameObj>, ReaderWriterLockSlim, IGameObj?> CheckCollisionInList =
                (IEnumerable<IGameObj> lst, ReaderWriterLockSlim listLock) =>
                {
                    IGameObj? collisionObj = null;
                    listLock.EnterReadLock();
                    try
                    {
                        foreach (var listObj in lst)
                        {
                            if (obj.WillCollideWith(listObj, Pos))
                            {
                                collisionObj = listObj;
                                break;
                            }
                        }
                    }
                    finally
                    {
                        listLock.ExitReadLock();
                    }
                    return collisionObj;
                };

            IGameObj? collisionObj;
            foreach (var list in lists)
            {
                if ((collisionObj = CheckCollisionInList(list.Item1, list.Item2)) != null)
                {
                    return collisionObj;
                }
            }

            return null;
        }
        /// <summary>
        /// 碰撞检测，如果这样行走是否会与之碰撞，返回与之碰撞的物体
        /// </summary>
        /// <param name="obj">移动的物体</param>
        /// <param name="moveVec">移动的位移向量</param>
        /// <returns>和它碰撞的物体</returns>
        public IGameObj? CheckCollisionWhenMoving(IMoveable obj, XY moveVec)
        {
            XY nextPos = obj.Position + moveVec;
            if (!obj.IsRigid)
            {
                if (gameMap.IsOutOfBound(obj))
                    return gameMap.GetOutOfBound(nextPos);
                return null;
            }
            return CheckCollision(obj, nextPos);
        }
        /// <summary>
        /// /// 可移动物体（圆）向矩形物体移动时，可移动且不会碰撞的最大距离。直接用double计算，防止误差
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="square">矩形的中心坐标</param>
        /// <returns></returns>
        // private double MaxMoveToSquare(IMoveable obj, IGameObj square)
        //{
        //     double tmpMax;
        //     double angle = Math.Atan2(square.Position.y - obj.Position.y, square.Position.x - obj.Position.x);
        //     if (obj.WillCollideWith(square, obj.Position))
        //         tmpMax = 0;
        //     else tmpMax =
        //         Math.Abs(XYPosition.Distance(obj.Position, square.Position) - obj.Radius -
        //         (square.Radius / Math.Min(Math.Abs(Math.Cos(angle)), Math.Abs(Math.Sin(angle)))));
        //     return tmpMax;
        // }

        // private double FindMaxOnlyConsiderWall(IMoveable obj, Vector moveVec)
        //{
        //     var desination = moveVec;
        //     double maxOnlyConsiderWall = moveVec.length;
        //     if (desination.length > 0)  //如果length足够长，还是有可能穿墙的
        //     {
        //         XYPosition nextXY = Vector.Vector2XY(desination) + obj.Position + new XYPosition((int)(obj.Radius * Math.Cos(moveVec.angle)), (int)(obj.Radius * Math.Sin(moveVec.angle)));
        //         if (gameMap.IsWall(nextXY)) //对下一步的位置进行检查，但这里只是考虑移动物体的宽度，只是考虑下一步能达到的最远位置
        //         {
        //             maxOnlyConsiderWall = MaxMoveToSquare(obj, gameMap.GetCell(nextXY));
        //         }
        //         else //考虑物体宽度
        //         {
        //             double dist = 0;
        //             XYPosition nextXYConsiderWidth;
        //             nextXYConsiderWidth = nextXY + new XYPosition((int)(obj.Radius * Math.Cos(moveVec.angle + Math.PI / 4)), (int)(obj.Radius * Math.Sin(moveVec.angle + Math.PI / 4)));
        //             if (gameMap.IsWall(nextXYConsiderWidth)) //对下一步的位置进行检查，但这里只是考虑移动物体的宽度，只是考虑下一步能达到的最远位置
        //             {
        //                 dist = MaxMoveToSquare(obj, gameMap.GetCell(nextXYConsiderWidth));
        //                 if (dist < maxOnlyConsiderWall)
        //                     maxOnlyConsiderWall = dist;
        //             }
        //             nextXYConsiderWidth = nextXY + new XYPosition((int)(obj.Radius * Math.Cos(moveVec.angle - Math.PI / 4)), (int)(obj.Radius * Math.Sin(moveVec.angle - Math.PI / 4)));
        //             if (gameMap.IsWall(nextXYConsiderWidth)) //对下一步的位置进行检查，但这里只是考虑移动物体的宽度，只是考虑下一步能达到的最远位置
        //             {
        //                 dist = MaxMoveToSquare(obj, gameMap.GetCell(nextXYConsiderWidth));
        //                 if (dist < maxOnlyConsiderWall)
        //                     maxOnlyConsiderWall = dist;
        //             }
        //         }
        //     }
        //     return maxOnlyConsiderWall;
        // }

        /// <summary>
        /// 寻找最大可能移动距离
        /// </summary>
        /// <param name="obj">移动物体，默认obj.Rigid为true</param>
        /// <param name="nextPos">下一步要到达的位置</param>
        /// <param name="moveVec">移动的位移向量，默认与nextPos协调</param>
        /// <returns>最大可能的移动距离</returns>
        public double FindMax(IMoveable obj, XY nextPos, XY moveVec)
        {
            double tmpMax = uint.MaxValue;  // 暂存最大值

            double maxDistance = uint.MaxValue;
            foreach (var listWithLock in lists)
            {
                var lst = listWithLock.Item1;
                var listLock = listWithLock.Item2;
                listLock.EnterReadLock();
                try
                {
                    foreach (IGameObj listObj in lst)
                    {
                        // 如果再走一步发生碰撞
                        if (obj.WillCollideWith(listObj, nextPos))
                        {
                            {
                                switch (listObj.Shape)  // 默认obj为圆形
                                {
                                    case ShapeType.Circle:
                                        {
                                            // 计算两者之间的距离
                                            double mod = XY.Distance(listObj.Position, obj.Position);
                                            int orgDeltaX = listObj.Position.x - obj.Position.x;
                                            int orgDeltaY = listObj.Position.y - obj.Position.y;

                                            if (mod < listObj.Radius + obj.Radius)  // 如果两者已经重叠
                                            {
                                                tmpMax = 0;
                                            }
                                            else
                                            {
                                                double tmp = mod - obj.Radius - listObj.Radius;
                                                // 计算能走的最长距离，好像这么算有一点误差？
                                                tmp = ((int)(tmp*1000 / Math.Cos(Math.Atan2(orgDeltaY, orgDeltaX) - moveVec.Angle())));
                                                if (tmp < 0 || tmp > uint.MaxValue || double.IsNaN(tmp))
                                                {
                                                    tmpMax = uint.MaxValue;
                                                }
                                                else
                                                    tmpMax = tmp/1000.0;
                                            }
                                            break;
                                        }
                                    case ShapeType.Square:
                                        {
                                            // if (obj.WillCollideWith(listObj, obj.Position))
                                            //     tmpMax = 0;
                                            // else tmpMax = MaxMoveToSquare(obj, listObj);
                                            // break;
                                            if (obj.WillCollideWith(listObj, obj.Position))
                                                tmpMax = 0;
                                            else
                                            {
                                                // 二分查找最大可能移动距离
                                                int left = 0, right = (int)moveVec.Length();
                                                while (left < right - 1)
                                                {
                                                    int mid = (right - left) / 2 + left;
                                                    if (obj.WillCollideWith(listObj, obj.Position + new XY((int)(mid * Math.Cos(moveVec.Angle())), (int)(mid * Math.Sin(moveVec.Angle())))))
                                                    {
                                                        right = mid;
                                                    }
                                                    else
                                                        left = mid;
                                                }
                                                tmpMax = (uint)left;
                                            }
                                            break;
                                        }
                                    default:
                                        tmpMax = uint.MaxValue;
                                        break;
                                }
                                if (tmpMax < maxDistance)
                                    maxDistance = tmpMax;
                            }
                        }
                    }
                }
                finally
                {
                    listLock.ExitReadLock();
                }
            }
            return maxDistance;
        }

        readonly IMap gameMap;
        private readonly Tuple<IEnumerable<IGameObj>, ReaderWriterLockSlim>[] lists;

        public CollisionChecker(IMap gameMap)
        {
            this.gameMap = gameMap;
            lists = new Tuple<IEnumerable<IGameObj>, ReaderWriterLockSlim>[gameMap.GameObjDict.Count];
            int i = 0;
            foreach (var keyValuePair in gameMap.GameObjDict)
            {
                lists[i++] = new Tuple<IEnumerable<IGameObj>, ReaderWriterLockSlim>(keyValuePair.Value as IList<IGameObj>, gameMap.GameObjLockDict[keyValuePair.Key]);
            }
        }
    }
}
