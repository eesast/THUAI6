using Preparation.Utility;
using System.Collections.Generic;

namespace Preparation.Interface
{
    public interface IOccupation
    {
        public int MoveSpeed { get; }
        public int MaxHp { get; }
        public BulletType InitBullet { get; }
        public int CD { get; }
        public int MaxBulletNum { get; }
        public List<ActiveSkillType> ListOfIActiveSkill { get; }
        public List<PassiveSkillType> ListOfIPassiveSkill { get; }
        public double Concealment { get; }
        public int AlertnessRadius { get; }
        public int TimeOfOpeningOrLocking { get; }
        public int TimeOfClimbingThroughWindows { get; }
        public int TimeOfOpenChest { get; }
    }

    public interface IGhost : IOccupation
    {
    }

    public interface IStudent : IOccupation
    {
        public int FixSpeed { get; }
    }

    public class Assassin : IGhost
    {
        private const int moveSpeed = GameData.basicMoveSpeed / 380 * 473;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public const int cd = 0;
        public int CD => cd;

        public const int maxBulletNum = 1;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.BecomeInvisible, ActiveSkillType.UseKnife });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 1.5;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.3);
        public int AlertnessRadius => alertnessRadius;

        public int timeOfOpeningOrLocking = GameData.basicTimeOfOpeningOrLocking;
        public int TimeOfOpeningOrLocking => timeOfOpeningOrLocking;

        public int timeOfClimbingThroughWindows = GameData.basicTimeOfClimbingThroughWindows;
        public int TimeOfClimbingThroughWindows => timeOfClimbingThroughWindows;

        public int timeOfOpenChest = GameData.basicTimeOfOpenChest;
        public int TimeOfOpenChest => timeOfOpenChest;
    }
    public class Athlete : IStudent
    {
        private const int moveSpeed = GameData.basicMoveSpeed / 38 * 40;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp / 30 * 32;
        public int MaxHp => maxHp;

        public const int cd = 0;
        public int CD => cd;

        public const int maxBulletNum = 0;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.CanBeginToCharge });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = GameData.basicFixSpeed / 10 * 6;
        public int FixSpeed => fixSpeed;

        public const double concealment = GameData.basicConcealment * 0.9;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int timeOfOpeningOrLocking = GameData.basicTimeOfOpeningOrLocking * 12 / 10;
        public int TimeOfOpeningOrLocking => timeOfOpeningOrLocking;

        public int timeOfClimbingThroughWindows = GameData.basicTimeOfClimbingThroughWindows / 87 * 80;
        public int TimeOfClimbingThroughWindows => timeOfClimbingThroughWindows;

        public int timeOfOpenChest = GameData.basicTimeOfOpenChest;
        public int TimeOfOpenChest => timeOfOpenChest;
    }
}
