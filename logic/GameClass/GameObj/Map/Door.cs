using Preparation.Interface;
using Preparation.Utility;
using System;

namespace GameClass.GameObj
{
    /// <summary>
    /// 门
    /// </summary>
    public class Door : Immovable
    {
        public Door(XY initPos, PlaceType placeType) :
            base(initPos, GameData.numOfPosGridPerCell / 2, GameObjType.Door)
        {
            keyType = placeType switch
            {
                PlaceType.Door3 => PropType.Key3,
                PlaceType.Door5 => PropType.Key5,
                _ => PropType.Key6,
            };
        }

        private readonly PropType keyType;
        public PropType KeyType => keyType;

        public override bool IsRigid => !isOpen;
        public override ShapeType Shape => ShapeType.Square;

        private Character? whoLockOrOpen = null;
        public Character? WhoLockOrOpen
        {
            get
            {
                lock (gameObjLock)
                    return whoLockOrOpen;
            }
        }

        private bool isOpen = true;
        public bool IsOpen
        {
            get
            {
                lock (gameObjLock)
                    return isOpen;
            }
        }

        public AtomicInt LockDegree { get; } = new AtomicInt(0);

        private long openStartTime = 0;
        public long OpenStartTime
        {
            get
            {
                lock (gameObjLock)
                    return openStartTime;
            }
        }

        public bool TryOpen(Character character)
        {
            lock (gameObjLock)
            {
                if (isOpen) return false;
                if (whoLockOrOpen != null) return false;
                openStartTime = Environment.TickCount64;
                whoLockOrOpen = character;
                return true;
            }
        }
        public void StopOpen()
        {
            lock (gameObjLock)
            {
                if (whoLockOrOpen != null)
                {
                    if ((Environment.TickCount64 - openStartTime) >= GameData.degreeOfLockingOrOpeningTheDoor / whoLockOrOpen.SpeedOfOpeningOrLocking)
                        //现在框架没有问题，但是调用可变的SpeedOfOpeningOrLocking可能死锁
                        isOpen = true;
                    whoLockOrOpen = null;
                }
            }
        }
        public void FinishOpen()
        {
            lock (gameObjLock)
            {
                isOpen = true;
                whoLockOrOpen = null;
            }
        }

        public bool TryLock(Character character)
        {
            lock (gameObjLock)
            {
                if (!isOpen) return false;
                if (whoLockOrOpen != null) return false;
                LockDegree.Set(0);
                whoLockOrOpen = character;
                return true;
            }
        }
        public void StopLock()
        {
            lock (gameObjLock)
            {
                if (LockDegree >= GameData.degreeOfLockingOrOpeningTheDoor)
                    isOpen = false;
                whoLockOrOpen = null;
            }
        }
        public void FinishLock()
        {
            lock (gameObjLock)
            {
                isOpen = false;
                whoLockOrOpen = null;
            }
        }

        public void ForceToOpen()
        {
            Character? character;
            lock (gameObjLock)
            {
                character = whoLockOrOpen;
                whoLockOrOpen = null;
                isOpen = true;
            }
            if (character != null)
            {
                lock (character.ActionLock)
                {
                    if (character.PlayerState == PlayerStateType.OpeningTheDoor)
                    {
                        character.ReleaseTool(KeyType);
                        character.SetPlayerStateNaturally();
                    }
                    else if (character.PlayerState == PlayerStateType.LockingTheDoor)
                    {
                        character.SetPlayerStateNaturally();
                    }
                }
            }
        }
    }
}