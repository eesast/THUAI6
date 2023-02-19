using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading;

namespace GameClass.GameObj
{
    public class Ghost : Character
    {
        public Ghost(XY initPos, int initRadius, PlaceType initPlace, CharacterType characterType) : base(initPos, initRadius, initPlace, characterType)
        {
        }
    }
}
