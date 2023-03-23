using Preparation.Utility;
using Preparation.Interface;
using System;

namespace GameClass.GameObj
{
    public class Student : Character
    {

        private readonly object beAttackedLock = new();
        /// <summary>
        /// 遭受攻击
        /// </summary>
        /// <param name="subHP"></param>
        /// <param name="hasSpear"></param>
        /// <param name="attacker">伤害来源</param>
        /// <returns>人物在受到攻击后死了吗</returns>
        public bool BeAttacked(Bullet bullet)
        {

            lock (beAttackedLock)
            {
                if (hp <= 0 || NoHp())
                    return false;  // 原来已经死了
                if (bullet.Parent.TeamID != this.TeamID)
                {
                    if (CanBeAwed()) PlayerState = PlayerStateType.Stunned;
                    if (HasShield)
                    {
                        if (bullet.HasSpear)
                            _ = TrySubHp(bullet.AP);
                        else
                            return false;
                    }
                    else
                    {
                        bullet.Parent.HP = (int)(bullet.Parent.HP + (bullet.Parent.Vampire * TrySubHp(bullet.AP)));
                    }
#if DEBUG
                    Console.WriteLine($"PlayerID:{ID} is being shot! Now his hp is {hp}.");
#endif
                    if (hp <= 0)
                        TryActivatingLIFE();  // 如果有复活甲
                }
                return hp <= 0;
            }
        }

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