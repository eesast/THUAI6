
namespace Preparation.Utility
{
    /// <summary>
    /// 存放所有用到的枚举类型
    /// </summary>
    public enum PlayerStateType
    {
        Null = 0,
        IsAddicted = 1,
        IsEscaped = 2,
        IsSwinging = 3,//指后摇
        IsResetting = 4,
        IsMoving = 5,
        IsTreating = 6,
        IsRescuing = 7,
        IsFixing = 8,
        IsTreated = 9,
        IsRescued = 10,
        IsStunned = 11,
        IsTryingToAttack = 12,//指前摇
        IsOpeningDoor = 13,
        IsClosingDoor = 14,
        IsClimbingThtoughWindows = 15,
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
        OrdinaryBullet = 1,  // 普通子弹
        AtomBomb = 2,        // 原子弹
        FastBullet = 3,      // 快速子弹
        LineBullet = 4,      // 直线子弹
        FlyingKnife = 5,     //飞刀
        CommonAttackOfGhost = 6
    }
    public enum PropType  // 道具类型
    {
        Null = 0,
        addSpeed = 1,
        addLIFE = 2,
        Shield = 3,
        Spear = 4,
        Gem = 5,  // 新增：宝石
    }
    public enum CharacterType  // 职业
    {
        Null = 0,
        Assassin = 1,
        Athlete = 2,
        RecoverAfterBattle = 3,
        SpeedUpWhenLeavingGrass = 4,
        PSkill4 = 5,
        PSkill5 = 6
    }
    public enum ActiveSkillType  // 主动技能
    {
        Null = 0,
        BecomeInvisible = 1,
        BecomeVampire = 2,
        NuclearWeapon = 3,
        SuperFast = 4,
        UseKnife = 5,
        BeginToCharge = 6
    }
    public enum PassiveSkillType
    {
        Null = 0,
        BecomeInvisible = 1,
        BecomeVampire = 2,
        NuclearWeapon = 3,
        SuperFast = 4,
        ASkill4 = 5,
        ASkill5 = 6
    }
    public enum BuffType  // buff
    {
        Null = 0,
        AddSpeed = 1,
        AddLIFE = 2,
        Shield = 3,
        Spear = 4
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
        Door = 12,
    }
    public enum BgmType
    {
        Null = 0,
        GhostIsComing = 1,
    }
}