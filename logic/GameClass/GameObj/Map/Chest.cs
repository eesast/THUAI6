using Preparation.Utility;
using System.Collections.Generic;

namespace GameClass.GameObj
{
    /// <summary>
    /// 箱子
    /// </summary>
    public class Chest : GameObj
    {
        public Chest(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Chest)
        {
            this.place = PlaceType.Chest;
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private Prop[] propInChest = new Prop[GameData.maxNumOfPropInChest] { new NullProp(), new NullProp() };
        public Prop[] PropInChest => propInChest;

        private int openDegree = 0;
        public int OpenDegree
        {
            get => openDegree;
            set
            {
                if (value > 0)
                    lock (gameObjLock)
                        openDegree = (value > GameData.degreeOfOpeningChest) ? GameData.degreeOfOpeningChest : value;
                else
                    lock (gameObjLock)
                        openDegree = 0;
            }
        }
        public bool IsOpen() => (OpenDegree == GameData.degreeOfOpeningChest);
    }
}
