using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public class Ghost : Character
    {
        public Ghost(XY initPos, int initRadius, CharacterType characterType) : base(initPos, initRadius, characterType)
        {
        }
    }
}
