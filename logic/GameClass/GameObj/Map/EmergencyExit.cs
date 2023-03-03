using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 紧急出口
    /// </summary>
    public class EmergencyExit : GameObj
    {
        public EmergencyExit(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, PlaceType.Null, GameObjType.EmergencyExit)
        {
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            else if (!((Character)targetObj).IsGhost())
                return true;  // 不是鬼不碰撞
            return false;
        }
    }
}
