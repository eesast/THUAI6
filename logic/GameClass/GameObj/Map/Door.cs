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
        public override bool IsRigid => !isOpen;
        public override ShapeType Shape => ShapeType.Square;

        private bool isOpen = true;
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
                if (value > 0)
                    lock (gameObjLock)
                        openOrLockDegree = (value > GameData.degreeOfLockingOrOpeningTheDoor) ? GameData.degreeOfLockingOrOpeningTheDoor : value;
                else
                    lock (gameObjLock)
                        openOrLockDegree = 0;
            }
        }
    }
}
