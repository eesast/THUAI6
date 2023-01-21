using Preparation.Utility;
using Preparation.GameData;

namespace GameClass.GameObj
{
    /// <summary>
    /// 墙体
    /// </summary>
    public class Wall : GameObj
    {
        public Wall(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, PlaceType.Land, GameObjType.Wall)
        {
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
    }
}
