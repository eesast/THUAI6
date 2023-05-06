using Preparation.Interface;
using Preparation.Utility;
using Protobuf;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 一切游戏元素的总基类，与THUAI4不同，继承IMoveable接口（出于一切物体其实都是可运动的指导思想）——LHR
    /// </summary>
    public abstract class Moveable : GameObj, IMoveable
    {
        protected readonly object moveObjLock = new();
        public object MoveLock => moveObjLock;

        public override XY Position
        {
            get
            {
                lock (moveObjLock)
                    return position;
            }
        }

        public override XY FacingDirection
        {
            get
            {
                lock (moveObjLock)
                    return facingDirection;
            }
        }

        private bool isMoving;
        public bool IsMoving
        {
            get
            {
                lock (moveObjLock)
                    return isMoving;
            }
            set
            {
                lock (moveObjLock)
                {
                    isMoving = value;
                }
            }
        }

        // 移动，改变坐标
        public long MovingSetPos(XY moveVec)
        {
            if (moveVec.x != 0 || moveVec.y != 0)
                lock (moveObjLock)
                {
                    facingDirection = moveVec;
                    this.position += moveVec;
                }
            return moveVec * moveVec;
        }

        public void ReSetPos(XY position) 
        {
            lock (moveObjLock)
            {
                this.position = position;
            }
        }

        public override bool CanMove
        {
            get
            {
                lock (gameObjLock)
                    return canMove;
            }
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
            get
            {
                lock (gameObjLock)
                    return isResetting;
            }
            set
            {
                lock (gameObjLock)
                {
                    isResetting = value;
                }
            }
        }

        public bool IsAvailable => !IsMoving && CanMove && !IsResetting;  // 是否能接收指令

        protected int moveSpeed;
        /// <summary>
        /// 移动速度
        /// </summary>
        public int MoveSpeed
        {
            get
            {
                lock (gameObjLock)
                    return moveSpeed;
            }
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
                       IsResetting = true;
                       this.Position = birthPos;
                       this.Place= place;
                   }
               }*/
        public Moveable(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
