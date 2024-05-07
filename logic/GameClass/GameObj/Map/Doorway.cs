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
            if (!ProgressOfDoorway.IsFinished()) return false;
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            return false;
        }

        public AtomicBool PowerSupply { get; } = new(false);
        public TimeBasedProgressAtVariableSpeed ProgressOfDoorway { get; } = new(GameData.degreeOfOpenedDoorway, 1);
        public bool TryToOpen()
        {
            if (!PowerSupply) return false;
            return ProgressOfDoorway.Start();
        }
    }
}
