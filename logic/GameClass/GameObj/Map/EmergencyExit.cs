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
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.EmergencyExit)
        {
            this.place = PlaceType.EmergencyExit;
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (!canOpen) return true;
            if (!IsOpen) return false;
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            return false;
        }


        private bool canOpen = false;
        public bool CanOpen
        {
            get => canOpen;
            set
            {
                lock (gameObjLock)
                    canOpen = value;
            }
        }

        private bool isOpen = false;
        public bool IsOpen
        {
            get => isOpen;
            set
            {
                lock (gameObjLock)
                    isOpen = value;
            }
        }
    }
}
