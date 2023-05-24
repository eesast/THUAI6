﻿using Preparation.Interface;
using Preparation.Utility;
using System;

namespace GameClass.GameObj
{
    /// <summary>
    /// 箱子
    /// </summary>
    public class Chest : Immovable, IChest
    {
        public Chest(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Chest)
        {
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;

        private readonly Gadget[] propInChest = new Gadget[GameData.maxNumOfPropInChest] { new NullProp(), new NullProp() };
        public Gadget[] PropInChest => propInChest;

        private int openStartTime = 0;
        public int OpenStartTime => openStartTime;
        private Character? whoOpen = null;
        public Character? WhoOpen => whoOpen;
        public bool Open(Character character)
        {
            lock (GameObjReaderWriterLock)
            {
                if (whoOpen != null) return false;
                openStartTime = Environment.TickCount;
                whoOpen = character;
            }
            return true;
        }
        public void StopOpen()
        {
            lock (GameObjReaderWriterLock)
            {
                whoOpen = null;
            }
        }
    }
}
