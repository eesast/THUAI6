using Preparation.Utility;
using System.Collections.Generic;

namespace Preparation.Interface
{
    public interface IOccupation
    {
        public int MoveSpeed { get; }
        public int MaxHp { get; }
        public BulletType InitBullet { get; }
        public List<ActiveSkillType> ListOfIActiveSkill { get; }
        public List<PassiveSkillType> ListOfIPassiveSkill { get; }
        public double Concealment { get; }
        public int AlertnessRadius { get; }
        public int ViewRange { get; }
        public int SpeedOfOpeningOrLocking { get; }
        public int SpeedOfClimbingThroughWindows { get; }
        public int SpeedOfOpenChest { get; }
    }

    public interface IGhost : IOccupation
    {
    }

    public interface IStudent : IOccupation
    {
        public int FixSpeed { get; }
        public int TreatSpeed { get; }
        public int MaxGamingAddiction { get; }
    }

    public class Assassin : IGhost
    {
        private const int moveSpeed = (int)(GameData.basicGhostMoveSpeed * 1.1);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.BecomeInvisible, ActiveSkillType.UseKnife });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 1.5;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.3);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 1.3);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Klee : IGhost
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed * 155 / 127);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.JumpyBomb });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.069);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 1.1);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = (int)(GameData.basicSpeedOfOpeningOrLocking / 1.1);
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicGhostSpeedOfClimbingThroughWindows / 1.1);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = (int)(GameData.basicSpeedOfOpenChest * 1.1);
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class ANoisyPerson : IGhost
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed * 1.07);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 12 / 10;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Howl });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 0.8;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicGhostSpeedOfClimbingThroughWindows * 1.1);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = (int)(GameData.basicSpeedOfOpenChest);
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Teacher : IStudent
    {
        private const int moveSpeed = GameData.basicStudentMoveSpeed * 3 / 4;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 10;
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = GameData.basicMaxGamingAddiction * 10;
        public int MaxGamingAddiction => maxGamingAddiction;

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

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows / 2;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Athlete : IStudent
    {
        private const int moveSpeed = GameData.basicStudentMoveSpeed * 40 / 38;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 32 / 30;
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = GameData.basicMaxGamingAddiction * 9 / 10;
        public int MaxGamingAddiction => maxGamingAddiction;

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

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking * 12 / 10;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class StraightAStudent : IStudent
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed * 0.8);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = (int)(GameData.basicHp * 1.1);
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = (int)(GameData.basicMaxGamingAddiction * 1.5);
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.WriteAnswers });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { PassiveSkillType.Meditate });

        public const int fixSpeed = GameData.basicFixSpeed * 13 / 10;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 8 / 10;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 0.9;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 0.9);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicStudentSpeedOfClimbingThroughWindows * 10 / 12.0);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Robot : IStudent
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = (int)(GameData.basicHp / 2.5);
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = 0;
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = GameData.basicFixSpeed;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 8 / 10;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicViewRange;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = 1;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class TechOtaku : IStudent
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed * 0.75);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = (int)(GameData.basicHp * 0.9);
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = (int)(GameData.basicMaxGamingAddiction * 1.1);
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.SummonGolem, ActiveSkillType.UseRobot });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = (int)(GameData.basicFixSpeed * 1.1);
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 9 / 10;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 0.9);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicStudentSpeedOfClimbingThroughWindows * 3 / 4);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Sunshine : IStudent
    {
        private const int moveSpeed = GameData.basicStudentMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 32 / 30;
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = GameData.basicMaxGamingAddiction * 11 / 10;
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Rouse });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = GameData.basicFixSpeed * 11 / 10;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 2;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 1.2;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicAlertnessRadius * 1.2);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicViewRange * 1.1);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
}