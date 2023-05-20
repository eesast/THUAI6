using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 逻辑墙
    /// </summary>
    public class OutOfBoundBlock : Immovable, IOutOfBound
    {
        public OutOfBoundBlock(XY initPos) :
            base(initPos, int.MaxValue, GameObjType.OutOfBoundBlock)
        {
        }

        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
    }
}
