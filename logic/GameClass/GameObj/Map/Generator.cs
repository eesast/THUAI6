using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 发电机
    /// </summary>
    public class Generator : GameObj
    {
        public Generator(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Generator)
        {
            this.place = PlaceType.Generator;
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private int degreeOfRepair = 0;
        public int DegreeOfRepair
        {
            get => degreeOfRepair;
            set
            {
                lock (gameObjLock)
                {
                    if (degreeOfRepair < GameData.degreeOfFixedGenerator)//不允许正常破坏已经修好的发电机
                        if (value < 0) degreeOfRepair = 0;
                        else degreeOfRepair = value > GameData.degreeOfFixedGenerator ? GameData.degreeOfFixedGenerator : value;
                }
            }
        }

        public bool Repair(int addDegree)
        {
            if (DegreeOfRepair == GameData.degreeOfFixedGenerator) return false;
            DegreeOfRepair += addDegree;
            if (DegreeOfRepair == GameData.degreeOfFixedGenerator)
                return true;
            else return false;
        }
    }
}
