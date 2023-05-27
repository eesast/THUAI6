using Preparation.Utility;
using Preparation.Interface;
using System;
using System.Threading;

namespace GameClass.GameObj
{
    public class Student : Character
    {
        private readonly object fixLock = new();
        protected int fixSpeed;
        /// <summary>
        /// 修理电机速度
        /// </summary>
        public int FixSpeed
        {
            get
            {
                lock (fixLock)
                    return fixSpeed;
            }
            set
            {
                lock (fixLock)
                {
                    fixSpeed = value;
                }
            }
        }
        /// <summary>
        /// 原初修理电机速度
        /// </summary>
        protected readonly int orgFixSpeed;

        private readonly object treatLock = new();
        protected int treatSpeed = GameData.basicTreatSpeed;
        public int TreatSpeed
        {
            get
            {
                lock (treatLock)
                    return treatSpeed;
            }
            set
            {
                lock (treatLock)
                {
                    treatSpeed = value;
                }
            }
        }
        protected readonly int orgTreatSpeed;

        private readonly object addictionLock = new();
        private int maxGamingAddiction;
        public int MaxGamingAddiction
        {
            get
            {
                lock (addictionLock)
                    return maxGamingAddiction;
            }
            protected set
            {
                lock (addictionLock)
                {
                    if (value < gamingAddiction) gamingAddiction = value;
                    maxGamingAddiction = value;
                }
            }
        }
        private int gamingAddiction;
        public int GamingAddiction
        {
            get
            {
                lock (addictionLock)
                    return gamingAddiction;
            }
            set
            {
                if (value > 0)
                    lock (addictionLock)
                        gamingAddiction = value <= maxGamingAddiction ? value : maxGamingAddiction;
                else
                    lock (addictionLock)
                        gamingAddiction = 0;
            }
        }

        private int timeOfRescue = 0;
        public int TimeOfRescue
        {
            get => Interlocked.CompareExchange(ref timeOfRescue, -1, -1);
        }
        public bool AddTimeOfRescue(int value)
        {
            return Interlocked.Add(ref timeOfRescue, value) >= GameData.basicTimeOfRescue;
        }
        public void SetTimeOfRescue(int value)
        {
            Interlocked.Exchange(ref timeOfRescue, value);
        }

        public Student(XY initPos, int initRadius, CharacterType characterType) : base(initPos, initRadius, characterType)
        {
            this.orgFixSpeed = this.fixSpeed = ((IStudentType)Occupation).FixSpeed;
            this.TreatSpeed = this.orgTreatSpeed = ((IStudentType)Occupation).TreatSpeed;
            this.MaxGamingAddiction = ((IStudentType)Occupation).MaxGamingAddiction;
        }
    }
    public class Golem : Student, IGolem
    {
        private Character? parent;  // 主人
        public Character? Parent
        {
            get => parent;
            set
            {
                lock (gameObjLock)
                {
                    parent = value;
                }
            }
        }
        public override void AddScore(long add)
        {
            if (parent == null)
                base.AddScore(add);
            else parent.AddScore(add);
        }
        public Golem(XY initPos, int initRadius, Character? parent) : base(initPos, initRadius, CharacterType.Robot)
        {
            this.parent = parent;
        }
    }
}