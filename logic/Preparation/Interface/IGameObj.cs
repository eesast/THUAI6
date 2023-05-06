using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IGameObj
    {
        public GameObjType Type { get; }
        public long ID { get; }
        public XY Position { get; }  // if Square, Pos equals the center
        public XY FacingDirection { get; }
        public bool IsRigid { get; }
        public ShapeType Shape { get; }
        public bool CanMove { get; }
        public int Radius { get; }  // if Square, Radius equals half length of one side
        public bool IgnoreCollideExecutor(IGameObj targetObj);             // 忽略碰撞，在具体类中实现
    }
}
