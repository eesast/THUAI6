using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 逻辑墙
    /// </summary>
    public class OutOfBoundBlock : GameObj, IOutOfBound
    {
        public OutOfBoundBlock(XY initPos) :
            base(initPos, int.MaxValue, GameObjType.OutOfBoundBlock)
        {
            this.CanMove = false;
        }

        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
    }
}
