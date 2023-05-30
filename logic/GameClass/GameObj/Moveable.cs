using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Moveable : GameObj, IMoveable
    {
        protected readonly object actionLock = new();
        public object ActionLock => actionLock;
        //player.actionLock>其他.actionLock/其他Lock,应当避免两个player的actionlock互锁
        private readonly ReaderWriterLockSlim moveReaderWriterLock = new();
        public ReaderWriterLockSlim MoveReaderWriterLock => moveReaderWriterLock;
        //规定moveReaderWriterLock<actionLock

        public Semaphore ThreadNum { get; } = new(1, 1);

        protected long stateNum = 0;
        public long StateNum
        {
            get
            {
                lock (actionLock)
                    return stateNum;
            }
            set
            {
                lock (actionLock) stateNum = value;
            }
        }

        protected RunningStateType runningState = RunningStateType.Null;
        public RunningStateType RunningState
        {
            get
            {
                lock (actionLock) return runningState;
            }
            set
            {
                lock (actionLock)
                    runningState = value;
            }
        }

        public override XY Position
        {
            get
            {
                lock (actionLock)
                    return position;
            }
        }

        protected XY facingDirection = new(1, 0);
        public XY FacingDirection
        {
            get
            {
                lock (actionLock)
                    return facingDirection;
            }
            set
            {
                lock (actionLock)
                    facingDirection = value;
            }
        }

        public AtomicBool IsMoving { get; } = new(false);

        // 移动，改变坐标
        public long MovingSetPos(XY moveVec, long stateNo)
        {

            if (moveVec.x != 0 || moveVec.y != 0)
            {
                lock (actionLock)
                {
                    if (!CanMove || IsRemoved) return -1;
                    if (stateNo != stateNum) return -1;
                    facingDirection = moveVec;
                    this.position += moveVec;
                }
            }
            return moveVec * moveVec;
        }

        public void ReSetPos(XY position)
        {
            lock (actionLock)
            {
                this.position = position;
            }
        }

        public AtomicBool CanMove { get; }

        public bool IsAvailableForMove => !IsMoving && CanMove && !IsRemoved; // 是否能接收移动指令

        protected int moveSpeed;
        /// <summary>
        /// 移动速度
        /// </summary>
        public int MoveSpeed
        {
            get => Interlocked.CompareExchange(ref moveSpeed, 0, 0);
            set => Interlocked.Exchange(ref moveSpeed, value);
        }
        /// <summary>
        /// 原初移动速度
        /// </summary>
        protected int orgMoveSpeed;
        public int OrgMoveSpeed => orgMoveSpeed;

        /*       /// <summary>
               /// 复活时数据重置
               /// </summary>
               public virtual void Reset(PlaceType place)
               {
                   lock (gameObjLock)
                   {
                       this.FacingDirection = new XY(1, 0);
                       isMoving = false;
                       CanMove = false;
                       IsRemoved = true;
                       this.Position = birthPos;
                       this.Place= place;
                   }
               }*/
        public Moveable(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
