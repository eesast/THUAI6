using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 一切游戏元素的总基类，与THUAI4不同，继承IMoveable接口（出于一切物体其实都是可运动的指导思想）——LHR
    /// </summary>
    public abstract class GameObj : IGameObj
    {
        private ReaderWriterLockSlim gameObjReaderWriterLock = new();
        public ReaderWriterLockSlim GameObjReaderWriterLock => gameObjReaderWriterLock;
        protected readonly object gameObjLock = new();
        public object GameLock => gameObjLock;

        protected readonly XY birthPos;

        private readonly GameObjType type;
        public GameObjType Type => type;

        private static long currentMaxID = 0;         // 目前游戏对象的最大ID
        public const long invalidID = long.MaxValue;  // 无效的ID
        public long ID { get; }

        protected XY position;
        public virtual XY Position { get; set; }

        protected XY facingDirection = new(1, 0);
        public virtual XY FacingDirection { get; set; }

        public abstract bool IsRigid { get; }

        public abstract ShapeType Shape { get; }

        protected bool canMove;
        public virtual bool CanMove { get; set; }

        public int Radius { get; }

        public virtual bool IgnoreCollideExecutor(IGameObj targetObj) => false;
        public GameObj(XY initPos, int initRadius, GameObjType initType)
        {
            this.position = this.birthPos = initPos;
            this.Radius = initRadius;
            this.type = initType;
            ID = Interlocked.Increment(ref currentMaxID);
        }
    }
}
