using System;
using System.Threading;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IMoveable : IGameObj
    {
        object ActionLock { get; }
        public int MoveSpeed { get; }
        public bool IsMoving { get; set; }
        public bool IsRemoved { get; }
        public bool IsAvailableForMove { get; }
        public long StateNum { get; }
        public Semaphore ThreadNum { get; }
        public long MovingSetPos(XY moveVec, long stateNum);
        public void ReSetCanMove(bool value);
        public bool WillCollideWith(IGameObj? targetObj, XY nextPos)  // 检查下一位置是否会和目标物碰撞
        {
            if (targetObj == null)
                return false;
            // 会移动的只有子弹和人物，都是Circle
            if (!targetObj.IsRigid || targetObj.ID == ID)
                return false;

            if (IgnoreCollideExecutor(targetObj) || targetObj.IgnoreCollideExecutor(this))
                return false;

            if (targetObj.Shape == ShapeType.Circle)
            {
                return XY.DistanceCeil3(nextPos, targetObj.Position) < targetObj.Radius + Radius;
            }
            else  // Square
            {
                long deltaX = Math.Abs(nextPos.x - targetObj.Position.x), deltaY = Math.Abs(nextPos.y - targetObj.Position.y);
                if (deltaX >= targetObj.Radius + Radius || deltaY >= targetObj.Radius + Radius)
                    return false;
                if (deltaX < targetObj.Radius || deltaY < targetObj.Radius)
                    return true;
                else
                    return ((long)(deltaX - targetObj.Radius) * (deltaX - targetObj.Radius)) + ((long)(deltaY - targetObj.Radius) * (deltaY - targetObj.Radius)) <= (long)Radius * (long)Radius;
            }
        }
    }
}
