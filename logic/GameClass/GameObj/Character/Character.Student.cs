using GameClass.Skill;
using Preparation.Utility;

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
        public int OrgFixSpeed { get; protected set; } = GameData.basicFixSpeed;

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
        public int OrgTreatSpeed { get; protected set; } = GameData.basicTreatSpeed;

        protected int rescueSpeed = GameData.basicRescueSpeed;
        public int RescueSpeed
        {
            get => rescueSpeed;
            set
            {
                lock (gameObjLock)
                {
                    rescueSpeed = value;
                }
            }
        }
        public int OrgRescueSpeed { get; protected set; } = GameData.basicRescueSpeed;

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

        public Student(XY initPos, int initRadius, PlaceType initPlace, CharacterType characterType) : base(initPos, initRadius, initPlace, characterType)
        {
            lock (gameObjLock)
                IsResetting = true;
            PlayerState = PlayerStateType.IsEscaped;
        }
        public Student(XY initPos, int initRadius, PlaceType initPlace, CharacterType characterType) : base(initPos, initRadius, initPlace, characterType)
        {
            switch (characterType)
            {
                case CharacterType.Athlete:
                    this.Occupation = new Athlete();
                    break;
                default:
                    this.Occupation = null;
                    break;
            }
            this.fixSpeed = ((IStudent)Occupation).FixSpeed;
        }
    }
}
