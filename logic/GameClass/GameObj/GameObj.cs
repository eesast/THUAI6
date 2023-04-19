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
        protected readonly object gameObjLock = new();
        public object GameLock => gameObjLock;

        protected readonly XY birthPos;

        private GameObjType type;
        public GameObjType Type => type;

        private static long currentMaxID = 0;         // 目前游戏对象的最大ID
        public const long invalidID = long.MaxValue;  // 无效的ID
        public const long noneID = long.MinValue;
        public long ID { get; }

        private XY position;
        public XY Position
        {
            get => position;
            protected
                set
            {
                lock (gameObjLock)
                {
                    position = value;
                }
            }
        }

        protected PlaceType place;
        public PlaceType Place { get => place; }

        private XY facingDirection = new(1, 0);
        public XY FacingDirection
        {
            get => facingDirection;
            set
            {
                lock (gameObjLock)
                    facingDirection = value;
            }
        }

        public abstract bool IsRigid { get; }

        public abstract ShapeType Shape { get; }

        private bool canMove;
        public bool CanMove
        {
            get => canMove;
            set
            {
                lock (gameObjLock)
                {
                    canMove = value;
                }
            }
        }

        private bool isResetting;
        public bool IsResetting
        {
            get => isResetting;
            set
            {
                lock (gameObjLock)
                {
                    isResetting = value;
                }
            }
        }

        public int Radius { get; }

        public virtual bool IgnoreCollideExecutor(IGameObj targetObj) => false;
        public GameObj(XY initPos, int initRadius, GameObjType initType)
        {
            this.Position = this.birthPos = initPos;
            this.Radius = initRadius;
            this.type = initType;
            ID = Interlocked.Increment(ref currentMaxID);
        }
    }
}
