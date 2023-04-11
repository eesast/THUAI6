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

    public interface IGhostType : IOccupation
    {
    }

    public interface IStudentType : IOccupation
    {
        public int FixSpeed { get; }
        public int TreatSpeed { get; }
        public int MaxGamingAddiction { get; }
    }

    public class Assassin : IGhostType
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

        public int alertnessRadius = (int)(GameData.basicGhostAlertnessRadius * 1.3);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicGhostViewRange * 1.2);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Klee : IGhostType
    {
        private const int moveSpeed = GameData.basicGhostMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.JumpyBomb });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment;
        public double Concealment => concealment;

        public int alertnessRadius = GameData.basicGhostAlertnessRadius;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicGhostViewRange;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = (int)(GameData.basicSpeedOfOpenChest * 1.1);
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Idol : IGhostType
    {
        private const int moveSpeed = GameData.basicGhostMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.ShowTime });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 3 / 4;
        public double Concealment => concealment;

        public int alertnessRadius = GameData.basicGhostAlertnessRadius;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicGhostViewRange * 11 / 10;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicGhostSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class ANoisyPerson : IGhostType
    {
        private const int moveSpeed = (int)(GameData.basicGhostMoveSpeed * 1.07);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 12 / 10;
        public int MaxHp => maxHp;

        public BulletType InitBullet => BulletType.CommonAttackOfGhost;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Howl });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public double concealment = GameData.basicConcealment * 0.8;
        public double Concealment => concealment;

        public int alertnessRadius = (int)(GameData.basicGhostAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicGhostViewRange);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicGhostSpeedOfClimbingThroughWindows * 1.1);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = (int)(GameData.basicSpeedOfOpenChest);
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Teacher : IStudentType
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

        public const int treatSpeed = (int)(GameData.basicTreatSpeed * 0.7);
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 0.5;
        public double Concealment => concealment;

        public const int alertnessRadius = GameData.basicStudentAlertnessRadius / 2;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicStudentViewRange * 9 / 10;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows / 2;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Athlete : IStudentType
    {
        private const int moveSpeed = GameData.basicStudentMoveSpeed * 11 / 10;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
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

        public const int alertnessRadius = GameData.basicStudentAlertnessRadius;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicStudentViewRange * 1.1);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows * 12 / 10;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class StraightAStudent : IStudentType
    {
        private const int moveSpeed = (int)(GameData.basicStudentMoveSpeed * 0.8);
        public int MoveSpeed => moveSpeed;

        private const int maxHp = (int)(GameData.basicHp * 1.1);
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = (int)(GameData.basicMaxGamingAddiction * 1.3);
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.WriteAnswers });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { PassiveSkillType.Meditate });

        public const int fixSpeed = GameData.basicFixSpeed * 11 / 10;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 8 / 10;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment * 0.9;
        public double Concealment => concealment;

        public const int alertnessRadius = (int)(GameData.basicStudentAlertnessRadius * 0.9);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicStudentViewRange * 0.9);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicStudentSpeedOfClimbingThroughWindows * 10 / 12.0);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Robot : IStudentType
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

        public const int alertnessRadius = (int)(GameData.basicStudentAlertnessRadius);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicStudentViewRange;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = 1;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class TechOtaku : IStudentType
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

        public const int alertnessRadius = (int)(GameData.basicStudentAlertnessRadius);
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = (int)(GameData.basicStudentViewRange * 0.9);
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = (int)(GameData.basicStudentSpeedOfClimbingThroughWindows * 3 / 4);
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }
    public class Sunshine : IStudentType
    {
        private const int moveSpeed = GameData.basicStudentMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp * 32 / 30;
        public int MaxHp => maxHp;

        private const int maxGamingAddiction = GameData.basicMaxGamingAddiction * 11 / 10;
        public int MaxGamingAddiction => maxGamingAddiction;

        public BulletType InitBullet => BulletType.Null;

        public List<ActiveSkillType> ListOfIActiveSkill => new(new ActiveSkillType[] { ActiveSkillType.Rouse, ActiveSkillType.Encourage, ActiveSkillType.Inspire });
        public List<PassiveSkillType> ListOfIPassiveSkill => new(new PassiveSkillType[] { });

        public const int fixSpeed = GameData.basicFixSpeed;
        public int FixSpeed => fixSpeed;

        public const int treatSpeed = GameData.basicTreatSpeed * 2;
        public int TreatSpeed => treatSpeed;

        public const double concealment = GameData.basicConcealment;
        public double Concealment => concealment;

        public const int alertnessRadius = GameData.basicStudentAlertnessRadius;
        public int AlertnessRadius => alertnessRadius;

        public int viewRange = GameData.basicStudentViewRange;
        public int ViewRange => viewRange;

        public int speedOfOpeningOrLocking = GameData.basicSpeedOfOpeningOrLocking;
        public int SpeedOfOpeningOrLocking => speedOfOpeningOrLocking;

        public int speedOfClimbingThroughWindows = GameData.basicStudentSpeedOfClimbingThroughWindows;
        public int SpeedOfClimbingThroughWindows => speedOfClimbingThroughWindows;

        public int speedOfOpenChest = GameData.basicSpeedOfOpenChest;
        public int SpeedOfOpenChest => speedOfOpenChest;
    }

    public static class OccupationFactory
    {
        public static IOccupation FindIOccupation(CharacterType characterType)
        {
            switch (characterType)
            {
                case CharacterType.Assassin:
                    return new Assassin();
                case CharacterType.Robot:
                    return new Robot();
                case CharacterType.Teacher:
                    return new Teacher();
                case CharacterType.Klee:
                    return new Klee();
                case CharacterType.StraightAStudent:
                    return new StraightAStudent();
                case CharacterType.ANoisyPerson:
                    return new ANoisyPerson();
                case CharacterType.TechOtaku:
                    return new TechOtaku();
                case CharacterType.Idol:
                    return new Idol();
                case CharacterType.Athlete:
                default:
                    return new Athlete();
            }
        }
    }
}