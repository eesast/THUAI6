using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 一切游戏元素的总基类，与THUAI4不同，继承IMoveable接口（出于一切物体其实都是可运动的指导思想）——LHR
    /// </summary>
    public abstract class GameObj : IMoveable
    {
        protected readonly object gameObjLock = new();
        /// <summary>
        /// 可移动物体专用锁
        /// </summary>
        public object MoveLock => gameObjLock;

        protected readonly XY birthPos;

        private GameObjType type;
        public GameObjType Type => type;

        private static long currentMaxID = 0;           //目前游戏对象的最大ID
        public const long invalidID = long.MaxValue;            //无效的ID
        public const long noneID = long.MinValue;
        public long ID { get; }

        private XY position;
        public XY Position
        {
            get => position;
            protected set
            {
                lock (gameObjLock)
                {
                    position = value;
                }
            }
        }
        public abstract bool IsRigid { get; }

        private XY facingDirection = new(1,0);
        public XY FacingDirection
        {
            get => facingDirection;
            set
            {
                lock (gameObjLock)
                    facingDirection = value;
            }
        }
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

        private bool isMoving;
        public bool IsMoving
        {
            get => isMoving;
            set
            {
                lock (gameObjLock)
                {
                    isMoving = value;
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
        public bool IsAvailable => !IsMoving && CanMove && !IsResetting;    //是否能接收指令
        public int Radius { get; }

        private PlaceType place;
        public PlaceType Place
        {
            get => place;
            set
            {
                lock (gameObjLock)
                {
                    place = value;
                }
            }
        }
        protected int moveSpeed;
        /// <summary>
        /// 移动速度
        /// </summary>
        public int MoveSpeed
        {
            get => moveSpeed;
            set
            {
                lock (gameObjLock)
                {
                    moveSpeed = value;
                }
            }
        }
        /// <summary>
        /// 原初移动速度
        /// </summary>
        private int orgMoveSpeed;
        public int OrgMoveSpeed { get => orgMoveSpeed; protected set { orgMoveSpeed = value; } }

        // 移动，改变坐标
        public long Move(XY moveVec)
        {
            lock (gameObjLock)
            {
                FacingDirection = moveVec;
                this.Position += moveVec;
            }
            return (long)(moveVec * moveVec);
        }
        /// <summary>
        /// 设置位置
        /// </summary>
        /// <param name="newpos">新位置</param>
        public void SetPosition(XY newpos)
        {
            Position = newpos;
        }
        /// <summary>
        /// 设置移动速度
        /// </summary>
        /// <param name="newMoveSpeed">新速度</param>
        public void SetMoveSpeed(int newMoveSpeed)
        {
            MoveSpeed = newMoveSpeed;
        }
        /// <summary>
        /// 复活时数据重置
        /// </summary>
        public virtual void Reset()
        {
            lock (gameObjLock)
            {
                facingDirection = new XY(1,0);
                isMoving = false;
                canMove = false;
                isResetting = true;
                this.position = birthPos;
            }
        }
        /// <summary>
        /// 为了使IgnoreCollide多态化并使GameObj能不报错地继承IMoveable
        /// 在xfgg点播下设计了这个抽象辅助方法，在具体类中实现
        /// </summary>
        /// <returns> 依具体类及该方法参数而定，默认为false </returns> 
        protected virtual bool IgnoreCollideExecutor(IGameObj targetObj) => false;
        bool IMoveable.IgnoreCollide(IGameObj targetObj) => IgnoreCollideExecutor(targetObj);
        public GameObj(XY initPos, int initRadius, PlaceType initPlace, GameObjType initType)
        {
            this.Position = this.birthPos = initPos;
            this.Radius = initRadius;
            this.place = initPlace;
            this.type = initType;
            ID = Interlocked.Increment(ref currentMaxID);
        }
    }
}
