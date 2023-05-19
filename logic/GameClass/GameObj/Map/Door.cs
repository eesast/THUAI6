using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Threading;

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
            switch (placeType)
            {
                case PlaceType.Door3:
                    doorNum = 3;
                    break;
                case PlaceType.Door5:
                    doorNum = 5;
                    break;
                case PlaceType.Door6:
                default:
                    doorNum = 6;
                    break;
            }
        }

        private readonly int doorNum;
        public int DoorNum => doorNum;

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

        private int lockDegree = 0;
        public int LockDegree
        {
            get
            {
                lock (gameObjLock)
                    return lockDegree;
            }
            set
            {
                value = (value > GameData.degreeOfLockingOrOpeningTheDoor) ? GameData.degreeOfLockingOrOpeningTheDoor : value;
                lock (gameObjLock)
                    lockDegree = value;
            }
        }

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
                lockDegree = 0;
                whoLockOrOpen = character;
                return true;
            }
        }
        public void StopLock()
        {
            lock (gameObjLock)
            {
                if (lockDegree >= GameData.degreeOfLockingOrOpeningTheDoor)
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
                    if (character.PlayerState == PlayerStateType.OpeningTheDoor)
                        character.SetPlayerState();
            }
        }
    }
}
