using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 门
    /// </summary>
    public class Door : GameObj
    {
        public Door(XY initPos, PlaceType placeType) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Door)
        {
            this.place = placeType;
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            return isOpen;
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

        private int openOrLockDegree = 0;
        public int OpenOrLockDegree
        {
            get => openOrLockDegree;
            set
            {
                lock (gameObjLock)
                    openOrLockDegree = (value > 0) ? value : 0;
            }
        }
    }
}
