using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 出口
    /// </summary>
    public class Doorway : GameObj
    {
        public Doorway(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, PlaceType.Null, GameObjType.Doorway)
        {
            this.CanMove = false;
        }
        public override bool IsRigid => true;
        public override ShapeType Shape => ShapeType.Square;
        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type != GameObjType.Character)
                return true;  // 非玩家不碰撞
            else if (!((Character)targetObj).IsGhost())
                return true;  // 不是鬼不碰撞
            return false;
        }

        private bool powerSupply = false;
        public bool PowerSupply
        {
            get => powerSupply;
            set
            {
                lock (gameObjLock)
                    powerSupply = value;
            }
        }

        public bool IsOpen => powerSupply;
    }
}
