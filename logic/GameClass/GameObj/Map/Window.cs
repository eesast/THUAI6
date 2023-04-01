using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 窗
    /// </summary>
    public class Window : GameObj
    {
        public Window(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Window)
        {
            this.place = PlaceType.Window;
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            if (targetObj == whoIsClimbing)
                return true;
            return false;
        }

        private Character? whoIsClimbing = null;
        public Character? WhoIsClimbing
        {
            get => whoIsClimbing;
            set
            {
                lock (gameObjLock)
                    whoIsClimbing = value;
            }
        }
    }
}
