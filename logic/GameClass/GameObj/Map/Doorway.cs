using Preparation.Interface;
using Preparation.Utility;
using System;

namespace GameClass.GameObj
{
    /// <summary>
    /// 出口
    /// </summary>
    public class Doorway : Immovable, IDoorway
    {
        public Doorway(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Doorway)
        {
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (!IsOpen()) return false;
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            return false;
        }

        public AtomicBool PowerSupply { get; } = new(false);

        private long openStartTime = 0;
        public long OpenStartTime
        {
            get
            {
                lock (gameObjLock)
                    return openStartTime;
            }
        }
        public bool TryToOpen()
        {
            if (!PowerSupply) return false;
            lock (gameObjLock)
            {
                if (openStartTime > 0) return false;
                openStartTime = Environment.TickCount64;
                return true;
            }
        }

        public bool StopOpenning()
        {
            lock (gameObjLock)
            {
                if (Environment.TickCount64 - openStartTime + openDegree >= GameData.degreeOfOpenedDoorway)
                {
                    openDegree = GameData.degreeOfOpenedDoorway;
                    return true;
                }
                else
                {
                    openDegree = (int)(Environment.TickCount64 - openStartTime) + openDegree;
                    openStartTime = 0;
                    return false;
                }
            }
        }

        public void FinishOpenning()
        {
            lock (gameObjLock)
            {
                openDegree = GameData.degreeOfOpenedDoorway;
            }
        }

        private int openDegree = 0;
        public int OpenDegree
        {
            get
            {
                lock (gameObjLock)
                    return openDegree;
            }
        }

        public bool IsOpen() => (OpenDegree == GameData.degreeOfOpenedDoorway);
    }
}
