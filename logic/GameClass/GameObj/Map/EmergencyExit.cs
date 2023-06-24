using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 紧急出口
    /// </summary>
    public class EmergencyExit : Immovable
    {
        public EmergencyExit(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.EmergencyExit)
        {
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (!CanOpen) return true;
            if (!IsOpen) return false;
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            return false;
        }

        public AtomicBool CanOpen { get; } = new(false);

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
