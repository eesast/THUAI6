using System;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IMoveable : IGameObj
    {
        object MoveLock { get; }
        public int MoveSpeed { get; }
        public bool IsMoving { get; set; }
        public bool IsAvailable { get; }
        public long MovingSetPos(XY moveVec, PlaceType place);
        public void ReSetPos(XY pos, PlaceType place);
        public bool WillCollideWith(IGameObj? targetObj, XY nextPos)  // 检查下一位置是否会和目标物碰撞
        {
            if (targetObj == null)
                return false;
            // 会移动的只有子弹和人物，都是Circle
            if (!targetObj.IsRigid || targetObj.ID == ID)
                return false;
            if (IgnoreCollide(targetObj))
                return false;
            if (targetObj.Shape == ShapeType.Circle)
            {
                return XY.Distance(nextPos, targetObj.Position) < targetObj.Radius + Radius;
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
