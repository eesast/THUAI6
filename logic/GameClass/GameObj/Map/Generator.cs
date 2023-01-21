using Preparation.Utility;
using Preparation.GameData;

namespace GameClass.GameObj
{
    /// <summary>
    /// 发电机
    /// </summary>
    public class Generator : GameObj
    {
        public Generator(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, PlaceType.Land, GameObjType.Generator)
        {
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
    }
}
