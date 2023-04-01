
namespace Preparation.Utility
{
    /// <summary>
    /// 存放所有用到的枚举类型
    /// </summary>
    public enum PlayerStateType
    {
        Null = 0,
        Addicted = 1,
        Escaped = 2,
        Swinging = 3,//指后摇
        Deceased = 4,
        Moving = 5,
        Treating = 6,
        Rescuing = 7,
        Fixing = 8,
        Treated = 9,
        Rescued = 10,
        Stunned = 11,
        TryingToAttack = 12,//指前摇
        LockingOrOpeningTheDoor = 13,
        OpeningTheChest = 14,
        ClimbingThroughWindows = 15,
        UsingSkill = 16,
        OpeningTheDoorway = 17,
    }
    public enum GameObjType
    {
        Null = 0,
        Character = 1,
        Prop = 2,
        PickedProp = 3,
        Bullet = 4,
        BombedBullet = 5,

        Wall = 6,
        Grass = 7,
        Generator = 8,  // 发电机
        Doorway = 9,
        EmergencyExit = 10,
        OutOfBoundBlock = 11,  // 范围外
        Window = 12,
        Door = 13,
        Chest = 14,
    }
    public enum ShapeType
    {
        Null = 0,
        Circle = 1,  // 子弹和人物为圆形，格子为方形
        Square = 2
    }
    public enum BulletType  // 子弹类型
    {
        Null = 0,
        FlyingKnife = 1,     //飞刀
        CommonAttackOfGhost = 2,
        JumpyDumpty = 3,
        BombBomb = 4,
        // Ram = 7,
    }
    public enum PropType  // 道具类型
    {//numOfPropSpecies 见Gamedata
        Null = 0,
        Key3 = 1,
        Key5 = 2,
        Key6 = 3,
        AddSpeed = 4,
        AddLifeOrClairaudience = 5,
        AddHpOrAp = 6,
        ShieldOrSpear = 7,
        RecoveryFromDizziness = 8,
    }
    public enum CharacterType  // 职业
    {
        Null = 0,
        Assassin = 1,
        Athlete = 2,
        Klee = 3,
        TechOtaku = 4,
        Teacher = 5,
        StraightAStudent = 6,
        ANoisyPerson = 7,
        Robot = 8,
    }
    public enum ActiveSkillType  // 主动技能
    {
        Null = 0,
        BecomeInvisible = 1,
        BecomeVampire = 2,
        JumpyBomb = 3,
        SuperFast = 4,
        UseKnife = 5,
        CanBeginToCharge = 6,
        Punish = 7,
        WriteAnswers = 8,
        Howl = 9,
    }
    public enum PassiveSkillType
    {
        Null = 0,
        BecomeInvisible = 1,
        BecomeVampire = 2,
        NuclearWeapon = 3,
        SuperFast = 4,
        Meditate = 5,
    }
    public enum BuffType  // buff
    {
        Null = 0,
        AddSpeed = 1,
        AddLife = 2,
        Shield = 3,
        Spear = 4,
        AddAp = 5,
        Clairaudience = 6,
        Invisible = 7,
    }
    public enum PlaceType
    {
        Null = 0,
        BirthPoint1 = 1,//必须从1开始
        BirthPoint2 = 2,
        BirthPoint3 = 3,
        BirthPoint4 = 4,
        BirthPoint5 = 5,
        Wall = 6,
        Grass = 7,
        Generator = 8,  // 发电机
        Doorway = 9,
        EmergencyExit = 10,
        Window = 11,
        Door3 = 12,
        Door5 = 13,
        Door6 = 14,
        Chest = 15,
    }
    public enum BgmType
    {
        Null = 0,
        GhostIsComing = 1,
        StudentIsApproaching = 2,
        GeneratorIsBeingFixed = 3,
    }
}