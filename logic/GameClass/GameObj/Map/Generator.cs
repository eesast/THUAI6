using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 发电机
    /// </summary>
    public class Generator : GameObj
    {
        public Generator(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, PlaceType.Land, GameObjType.Generator)
        {
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private int degreeOfFRepair = 0;
        public int DegreeOfFRepair
        {
            get => degreeOfFRepair;
            set
            {
                lock (gameObjLock)
                {
                    if (degreeOfFRepair < GameData.degreeOfFixedGenerator)//不允许正常破坏已经修好的发电机
                        if (value < 0) degreeOfFRepair = 0;
                        else degreeOfFRepair = value > GameData.degreeOfFixedGenerator ? GameData.degreeOfFixedGenerator : value;
                }
            }
        }

        public bool Repair(int addDegree)
        {
            if (DegreeOfFRepair == GameData.degreeOfFixedGenerator) return false;
            DegreeOfFRepair += addDegree;
            if (DegreeOfFRepair == GameData.degreeOfFixedGenerator)
                return true;
            else return false;
        }
    }
}
