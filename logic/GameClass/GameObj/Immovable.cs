using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Immovable : GameObj
    {

        public override XY Position => position;

        public override XY FacingDirection => facingDirection;

        public override bool CanMove => false;

        public Immovable(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
