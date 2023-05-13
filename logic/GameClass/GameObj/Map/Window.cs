using Preparation.Interface;
using Preparation.Utility;
using System.Numerics;
using System;

namespace GameClass.GameObj
{
    /// <summary>
    /// 窗
    /// </summary>
    public class Window : Immovable
    {
        public Window(XY initPos, bool xIsWall) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Window)
        {
            this.xIsWall = xIsWall;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            if (whoIsClimbing != null && targetObj == whoIsClimbing)
            {
                return true;
            }
            return false;
        }

        public readonly bool xIsWall;

        private XY stage = new(0, 0);
        public XY Stage
        {
            get
            {
                GameObjReaderWriterLock.EnterReadLock();
                try
                {
                    return stage;
                }
                finally { GameObjReaderWriterLock.ExitReadLock(); }
            }
        }

        private Character? whoIsClimbing = null;
        public Character? WhoIsClimbing
        {
            get
            {
                GameObjReaderWriterLock.EnterReadLock();
                try
                {
                    return whoIsClimbing;
                }
                finally { GameObjReaderWriterLock.ExitReadLock(); }
            }
        }

        public bool TryToClimb(Character character)
        {
            GameObjReaderWriterLock.EnterWriteLock();
            try
            {
                if (whoIsClimbing == null)
                {
                    stage = new(0, 0);
                    whoIsClimbing = character;
                    return true;
                }
                else return false;
            }
            finally { GameObjReaderWriterLock.ExitWriteLock(); }
        }
        public void FinishClimbing()
        {
            GameObjReaderWriterLock.EnterWriteLock();
            try
            {
                whoIsClimbing = null;
            }
            finally { GameObjReaderWriterLock.ExitWriteLock(); }
        }
        public void Enter2Stage(XY xy)
        {
            GameObjReaderWriterLock.EnterWriteLock();
            try
            {
                stage = xy;
            }
            finally { GameObjReaderWriterLock.ExitWriteLock(); }
        }
    }
}
