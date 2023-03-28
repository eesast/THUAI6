using Preparation.Utility;
using System.Collections.Generic;

namespace Preparation.Interface
{
    public interface IOccupation
    {
        public int MoveSpeed { get; }
        public int MaxHp { get; }
        public BulletType InitBullet { get; }
        public int MaxBulletNum { get; }
        public List<ActiveSkillType> ListOfIActiveSkill { get; }
        public List<PassiveSkillType> ListOfIPassiveSkill { get; }
        public double Concealment { get; }
        public int AlertnessRadius { get; }
        public int ViewRange { get; }
        public int TimeOfOpeningOrLocking { get; }
        public int SpeedOfClimbingThroughWindows { get; }
        public int TimeOfOpenChest { get; }
    }

    public interface IGhost : IOccupation
    {
    }

    public interface IStudent : IOccupation
    {
        public int FixSpeed { get; }
        public int TreatSpeed { get; }
    }

    public class Assassin : IGhost
    {
        private const int moveSpeed = GameData.basicMoveSpeed * 473 / 380;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public const int maxBulletNum = 1;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.BecomeInvisible, ActiveSkillType.UseKnife });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 1.5;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.3);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 1.3);
        public int ViewRange => viewRange;

        public int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int TimeOfOpeningOrLocking => timeOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int TimeOfOpenChest => timeOfOpenChest;
    }
    public class Teacher : IStudent
    {
        private const int moveSpeed = GameData.basicMoveSpeed * 3 / 4;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 10;
        public int MaxHp => maxHp;

        public const int maxBulletNum = 0;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Punish });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = 0;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 0.5;
        public double Concealment => concealment;

        public const int alertnessRadius = GameData.basicAlertnessRadius / 2;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicViewRange * 9 / 10;
        public int ViewRange => viewRange;

        public int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int TimeOfOpeningOrLocking => timeOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows / 2;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int TimeOfOpenChest => timeOfOpenChest;
    }
    public class Athlete : IStudent
    {
        private const int moveSpeed = GameData.basicMoveSpeed * 40 / 38;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 32 / 30;
        public int MaxHp => maxHp;

        public const int maxBulletNum = 0;
        public int MaxBulletNum => maxBulletNum;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.CanBeginToCharge });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = GameData.basicFixSpeed * 6 / 10;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 8 / 10;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 0.9;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 1.1);
        public int ViewRange => viewRange;

        public int timeOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking * 12 / 10;
        public int TimeOfOpeningOrLocking => timeOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int timeOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int TimeOfOpenChest => timeOfOpenChest;
    }
}
