using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 一切游戏元素的总基类，与THUAI4不同，继承IMoveable接口（出于一切物体其实都是可运动的指导思想）——LHR
    /// </summary>
    public abstract class Tile : GameObj
    {

        public override XY Position => position;

        public override XY FacingDirection => facingDirection;

        public override bool CanMove => false;

        public Tile(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
