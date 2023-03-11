using Preparation.Interface;
using Preparation.Utility;
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

        public bool IsAvailable => !IsMoving && CanMove && !IsResetting;  // 是否能接收指令

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
        public int OrgMoveSpeed { get; protected set; }

        // 移动，改变坐标
        public long MovingSetPos(XY moveVec, PlaceType place)
        {
            lock (gameObjLock)
            {
                FacingDirection = moveVec;
                this.Position += moveVec;
                this.place = place;
            }
            return moveVec * moveVec;
        }

        public void ReSetPos(XY pos, PlaceType place)
        {
            lock (gameObjLock)
            {
                this.Position = pos;
                this.place = place;
            }
        }

        /// <summary>
        /// 设置移动速度
        /// </summary>
        /// <param name="newMoveSpeed">新速度</param>
        public void SetMoveSpeed(int newMoveSpeed)
        {
            MoveSpeed = newMoveSpeed;
        }
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
        /// <summary>
        /// 为了使IgnoreCollide多态化并使GameObj能不报错地继承IMoveable
        /// 在xfgg点播下设计了这个抽象辅助方法，在具体类中实现
        /// </summary>
        /// <returns> 依具体类及该方法参数而定，默认为false </returns>
        public Moveable(XY initPos, int initRadius, GameObjType initType) : base(initPos, initRadius, initType)
        {
        }
    }
}
