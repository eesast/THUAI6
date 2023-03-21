using Preparation.Utility;
using Preparation.Interface;

namespace GameClass.GameObj
{
    public class Student : Character
    {
        protected int fixSpeed;
        /// <summary>
        /// 修理电机速度
        /// </summary>
        public int FixSpeed
        {
            get => fixSpeed;
            set
            {
                lock (gameObjLock)
                {
                    fixSpeed = value;
                }
            }
        }
        /// <summary>
        /// 原初修理电机速度
        /// </summary>
        public int OrgFixSpeed { get; protected set; }

        protected int treatSpeed = GameData.basicTreatSpeed;
        public int TreatSpeed
        {
            get => treatSpeed;
            set
            {
                lock (gameObjLock)
                {
                    treatSpeed = value;
                }
            }
        }
        public int OrgTreatSpeed { get; protected set; }

        public int MaxGamingAddiction { get; protected set; }
        private int gamingAddiction;
        public int GamingAddiction
        {
            get => gamingAddiction;
            set
            {
                if (gamingAddiction > 0)
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
            set
            {
                if (value > 0)
                    lock (gameObjLock)
                        degreeOfTreatment = (value < MaxHp - HP) ? value : MaxHp - HP;
                else
                    lock (gameObjLock)
                        degreeOfTreatment = 0;
            }
        }

        private int timeOfRescue = 0;
        public int TimeOfRescue
        {
            get => timeOfRescue;
            set
            {
                if (value > 0)
                    lock (gameObjLock)
                        timeOfRescue = (value < GameData.basicTimeOfRescue) ? value : GameData.basicTimeOfRescue;
                else
                    lock (gameObjLock)
                        timeOfRescue = 0;
            }
        }

        public Student(XY initPos, int initRadius, CharacterType characterType) : base(initPos, initRadius, characterType)
        {
            this.OrgFixSpeed = this.fixSpeed = ((IStudent)Occupation).FixSpeed;
        }
    }
}