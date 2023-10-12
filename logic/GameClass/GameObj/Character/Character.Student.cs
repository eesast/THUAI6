using Preparation.Utility;
using Preparation.Interface;
using System;
using System.Threading;

namespace GameClass.GameObj
{
    public class Student : Character
    {
        public AtomicInt FixSpeed { get; } = new AtomicInt(0);
        /// <summary>
        /// 原初修理电机速度
        /// </summary>
        protected readonly int orgFixSpeed;

        public AtomicInt TreatSpeed { get; } = new AtomicInt(GameData.basicTreatSpeed);
        protected readonly int orgTreatSpeed;

        public IntInTheVariableRange GamingAddiction { get; } = new IntInTheVariableRange(0, 0);

        public AtomicInt TimeOfRescue { get; } = new AtomicInt(0);

        public Student(XY initPos, int initRadius, CharacterType characterType) : base(initPos, initRadius, characterType)
        {
            this.FixSpeed.SetReturnOri(this.orgFixSpeed = ((IStudentType)Occupation).FixSpeed);
            this.TreatSpeed.SetReturnOri(this.orgTreatSpeed = ((IStudentType)Occupation).TreatSpeed);
            this.GamingAddiction.SetPositiveMaxV(((IStudentType)Occupation).MaxGamingAddiction);
        }
    }
    public class Golem : Student, IGolem
    {
        private readonly object parentLock = new();
        private Character? parent;  // 主人
        public Character? Parent
        {
            get
            {
                lock (parentLock)
                    return parent;
            }
            set
            {
                lock (parentLock)
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