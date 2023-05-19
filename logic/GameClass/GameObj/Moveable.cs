using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Moveable : GameObj, IMoveable
    {
        protected readonly object actionLock = new();
        public object ActionLock => actionLock;
        //player.actionLock>其他.actionLock
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

        public override XY Position
        {
            get
            {
                lock (actionLock)
                    return position;
            }
        }

        public override XY FacingDirection
        {
            get
            {
                lock (actionLock)
                    return facingDirection;
            }
        }

        private int isMoving = 0;
        public int IsMoving
        {
            get => Interlocked.CompareExchange(ref isMoving, 0, 1);
            set => Interlocked.Exchange(ref isMoving, value);
        }

        // 移动，改变坐标
        public long MovingSetPos(XY moveVec, long stateNo)
        {

            if (moveVec.x != 0 || moveVec.y != 0)
            {
                lock (actionLock)
                {
                    moveReaderWriterLock.EnterReadLock();
                    try
                    {
                        if (!canMove || isRemoved) return -1;
                    }
                    finally
                    {
                        moveReaderWriterLock.ExitReadLock();
                    }
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

        public override bool CanMove
        {
            get
            {
                moveReaderWriterLock.EnterReadLock();
                try
                {
                    return canMove;
                }
                finally
                {
                    moveReaderWriterLock.ExitReadLock();
                }
            }
        }

        public void ReSetCanMove(bool value)
        {
            moveReaderWriterLock.EnterWriteLock();
            try
            {
                canMove = value;
            }
            finally
            {
                moveReaderWriterLock.ExitWriteLock();
            }
        }

        public override bool IsRemoved
        {
            get
            {
                moveReaderWriterLock.EnterReadLock();
                try
                {
                    return isRemoved;
                }
                finally
                {
                    moveReaderWriterLock.ExitReadLock();
                }
            }
        }

        public bool IsAvailableForMove // 是否能接收移动指令
        {
            get
            {
                lock (actionLock)
                {
                    moveReaderWriterLock.EnterReadLock();
                    try
                    {
                        return isMoving == 0 && canMove && !isRemoved;
                    }
                    finally
                    {
                        moveReaderWriterLock.ExitReadLock();
                    }
                }
            }
        }

        protected int moveSpeed;
        /// <summary>
        /// 移动速度
        /// </summary>
        public int MoveSpeed
        {
            get => Interlocked.CompareExchange(ref moveSpeed, 0, 1);
            set => Interlocked.Exchange(ref moveSpeed, value);
        }
        /// <summary>
        /// 原初移动速度
        /// </summary>
        public int OrgMoveSpeed { get; protected set; }

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
