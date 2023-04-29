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

        private int openStartTime = 0;
        public int OpenStartTime => openStartTime;
        private Character? whoOpen = null;
        public Character? WhoOpen => whoOpen;
        public void Open(int startTime, Character character)
        {
            lock (gameObjLock)
            {
                openStartTime = startTime;
                whoOpen = character;
            }
        }
        public void StopOpen()
        {
            lock (gameObjLock)
            {
                openStartTime = 0;
                whoOpen = null;
            }
        }
    }
}
