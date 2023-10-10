using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 箱子
    /// </summary>
    public class Chest : Immovable
    {
        public Chest(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Chest)
        {
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private readonly Gadget[] propInChest = new Gadget[GameData.maxNumOfPropInChest] { new NullProp(), new NullProp() };
        public Gadget[] PropInChest => propInChest;

        private TimeBasedProgressForInterrupting openProgress = new TimeBasedProgressForInterrupting();
        public TimeBasedProgressForInterrupting OpenProgress { get => openProgress; }
    }
}
