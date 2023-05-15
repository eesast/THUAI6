using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 窗
    /// </summary>
    public class Window : Immovable, IWindow
    {
        public Window(XY initPos) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Window)
        {
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

        private XY stage = new(0, 0);
        public XY Stage
        {
            get
            {
                lock (gameObjLock)
                    return stage;
            }
        }

        private Character? whoIsClimbing = null;
        public Character? WhoIsClimbing
        {
            get
            {
                lock (gameObjLock)
                    return whoIsClimbing;
            }
        }

        public bool TryToClimb(ICharacter character)
        {
            lock (gameObjLock)
                if (whoIsClimbing == null)
                {
                    stage = new(0, 0);
                    whoIsClimbing = (Character)character;
                    return true;
                }
                else return false;
        }
        public void FinishClimbing()
        {
            lock (gameObjLock)
                whoIsClimbing = null;
        }
        public void Enter2Stage(XY xy)
        {
            lock (gameObjLock)
                stage = xy;
        }
    }
}
