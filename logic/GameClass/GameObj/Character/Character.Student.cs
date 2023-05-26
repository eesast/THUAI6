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

        public int MaxGamingAddiction { get; protected set; }
        private int gamingAddiction;
        public int GamingAddiction
        {
            get => gamingAddiction;
            set
            {
                if (value > 0)
                    lock (gameObjLock)
                        gamingAddiction = value <= MaxGamingAddiction ? value : MaxGamingAddiction;
                else
                    lock (gameObjLock)
                        gamingAddiction = 0;
            }
        }

        private int selfHealingTimes = 1;//剩余的自愈次数
        public int SelfHealingTimes
        {
            get => selfHealingTimes;
            set
            {
                lock (gameObjLock)
                    selfHealingTimes = (value > 0) ? value : 0;
            }
        }

        private int degreeOfTreatment = 0;
        public int DegreeOfTreatment
        {
            get => degreeOfTreatment;
            private set
            {
                degreeOfTreatment = value;
            }
        }
        public void SetDegreeOfTreatment0()
        {
            DegreeOfTreatment = 0;
        }
        public bool SetDegreeOfTreatment(int value, Student whoTreatYou)
        {
            if (value <= 0) { degreeOfTreatment = 0; return false; }
            if (value >= MaxHp - HP)
            {
                whoTreatYou.AddScore(GameData.StudentScoreTreat(MaxHp - HP));
                HP = MaxHp;
                degreeOfTreatment = 0;
                return true;
            }
            if (value >= GameData.basicTreatmentDegree)
            {
                whoTreatYou.AddScore(GameData.StudentScoreTreat(GameData.basicTreatmentDegree));
                HP += GameData.basicTreatmentDegree;
                DegreeOfTreatment = 0;
                return true;
            }
            DegreeOfTreatment = value;
            return false;
        }
        public bool AddDegreeOfTreatment(int value, Student student)
        {
            return SetDegreeOfTreatment(value + degreeOfTreatment, student);
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