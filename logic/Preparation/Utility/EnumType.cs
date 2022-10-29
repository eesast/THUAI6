
namespace Preparation.Utility
{
    /// <summary>
    /// 存放所有用到的枚举类型
    /// </summary>
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
        BirthPoint = 9,
        Exit = 10,
        EmergencyExit = 11,
        OutOfBoundBlock = 12,  // 范围外
    }
    public enum ShapeType
    {
        Null = 0,
        Circle = 1,  // 子弹和人物为圆形，格子为方形
        Square = 2
    }
    public enum PlaceType  // 位置标志，包括陆地，建筑，草丛。游戏中每一帧都要刷新各个物体的该属性
    {
        Null = 0,
        Land = 1,
        Building = 2,
        Grass1 = 3,
        Grass2 = 4,
        Grass3 = 5,
        Grass4 = 6,
        Grass5 = 7,
    }
    public enum BulletType  // 子弹类型
    {
        Null = 0,
        OrdinaryBullet = 1,  // 普通子弹
        AtomBomb = 2,        // 原子弹
        FastBullet = 3,      // 快速子弹
        LineBullet = 4       // 直线子弹
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
        RecoverAfterBattle = 1,
        SpeedUpWhenLeavingGrass = 2,
        Vampire = 3,
        PSkill3 = 4,
        PSkill4 = 5,
        PSkill5 = 6
    }
    public enum ActiveSkillType  // 主动技能
    {
        Null = 0,
        BecomeVampire = 1,
        BecomeAssassin = 2,
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
    public enum GameObjIdx
    {
        None = 0,
        Player = 1,
        Bullet = 2,
        Prop = 3,
        Gem = 4,
        Map = 5,
        BombedBullet = 6,
        PickedProp = 7
    }
}
