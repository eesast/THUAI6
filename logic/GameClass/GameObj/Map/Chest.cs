﻿using Preparation.Interface;
using Preparation.Utility;
using System;

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

        public LongProgressContinuously OpenProgress { get; } = new LongProgressContinuously();
    }
}
