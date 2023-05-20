using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 发电机
    /// </summary>
    public class Generator : Immovable
    {
        public Generator(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Generator)
        {
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private int numOfFixing = 0;
        public int NumOfFixing
        {
            get => Interlocked.CompareExchange(ref numOfFixing, 0, 0);
        }
        public void AddNumOfFixing()
        {
            Interlocked.Increment(ref numOfFixing);
        }
        public void SubNumOfFixing()
        {
            Interlocked.Decrement(ref numOfFixing);
        }

        private int degreeOfRepair = 0;
        public int DegreeOfRepair
        {
            get
            {
                lock (gameObjLock)
                    return degreeOfRepair;
            }
        }

        public bool Repair(int addDegree, Character character)
        {
            int orgDegreeOfRepair, value;
            lock (gameObjLock)
            {
                if (degreeOfRepair == GameData.degreeOfFixedGenerator) return false;
                orgDegreeOfRepair = degreeOfRepair;

                degreeOfRepair += addDegree;
                if (degreeOfRepair < 0) degreeOfRepair = 0;
                else
                {
                    if (degreeOfRepair > GameData.degreeOfFixedGenerator) degreeOfRepair = GameData.degreeOfFixedGenerator;
                }
                value = degreeOfRepair;
            }

            if (value > orgDegreeOfRepair)
            {
                character.AddScore(GameData.StudentScoreFix(value) - GameData.StudentScoreFix(orgDegreeOfRepair));
                if (value == GameData.degreeOfFixedGenerator) return true;
            }
            else character.AddScore(GameData.TrickerScoreDamageGenerator(orgDegreeOfRepair) - GameData.TrickerScoreDamageGenerator(value));
            return false;
        }
    }
}