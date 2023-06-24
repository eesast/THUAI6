using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Immovable : GameObj
    {

        public override XY Position => position;

        public Immovable(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
