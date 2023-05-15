using Google.Protobuf.WellKnownTypes;
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

        private bool powerSupply = false;
        public bool PowerSupply
        {
            get
            {
                lock (gameObjLock)
                    return powerSupply;
            }
            set
            {
                lock (gameObjLock)
                    powerSupply = value;
            }
        }

        private int openStartTime = 0;
        public int OpenStartTime
        {
            get
            {
                lock (gameObjLock)
                    return openStartTime;
            }
        }
        public bool TryToOpen()
        {
            lock (gameObjLock)
            {
                if (!powerSupply || openStartTime > 0) return false;
                openStartTime = Environment.TickCount;
                return true;
            }
        }

        public bool StopOpenning()
        {
            lock (gameObjLock)
            {
                if (openDegree + Environment.TickCount - openStartTime >= GameData.degreeOfOpenedDoorway)
                {
                    openDegree = GameData.degreeOfOpenedDoorway;
                    return true;
                }
                else
                {
                    openDegree = openDegree + Environment.TickCount - openStartTime;
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
