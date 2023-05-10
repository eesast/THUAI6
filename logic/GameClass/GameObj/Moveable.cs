using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Moveable : GameObj, IMoveable
    {
        protected readonly object actionLock = new();
        public object ActionLock => actionLock;
        private readonly ReaderWriterLockSlim moveReaderWriterLock = new();
        public ReaderWriterLockSlim MoveReaderWriterLock => moveReaderWriterLock;
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
        //规定moveReaderWriterLock>actionLock

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

        private bool isMoving;
        public bool IsMoving
        {
            get
            {
                lock (actionLock)
                    return isMoving;
            }
            set
            {
                lock (actionLock)
                {
                    isMoving = value;
                }
            }
        }

        // 移动，改变坐标
        public long MovingSetPos(XY moveVec)
        {
            if (moveVec.x != 0 || moveVec.y != 0)
                lock (actionLock)
                {
                    facingDirection = moveVec;
                    this.position += moveVec;
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
                lock (actionLock)
                {
                    canMove = value;
                }
            }
            finally
            {
                moveReaderWriterLock.ExitWriteLock();
            }
        }

        protected bool isRemoved;
        public bool IsRemoved
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
                moveReaderWriterLock.EnterReadLock();
                try
                {
                    lock (actionLock)
                        return !isMoving && canMove && !isRemoved;
                }
                finally
                {
                    moveReaderWriterLock.ExitReadLock();
                }
            }
        }

        protected int moveSpeed;
        /// <summary>
        /// 移动速度
        /// </summary>
        public int MoveSpeed
        {
            get
            {
                moveReaderWriterLock.EnterReadLock();
                try
                {
                    return moveSpeed;
                }
                finally
                {
                    moveReaderWriterLock.ExitReadLock();
                }
            }
            set
            {
                moveReaderWriterLock.EnterWriteLock();
                try
                {
                    lock (actionLock)
                    {
                        moveSpeed = value;
                    }
                }
                finally
                {
                    moveReaderWriterLock.ExitWriteLock();
                }
            }
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
