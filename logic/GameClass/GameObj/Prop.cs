using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Prop : ObjOfCharacter
    {
        public override bool IsRigid => true;

        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Prop || targetObj.Type == GameObjType.Bullet
                || targetObj.Type == GameObjType.Character || targetObj.Type == GameObjType.Chest)
                return true;
            return false;
        }

        public override ShapeType Shape => ShapeType.Square;

        public abstract PropType GetPropType();

        public Prop(XY initPos, int radius = GameData.PropRadius) :
            base(initPos, radius, GameObjType.Prop)
        {
            this.canMove = false;
            this.MoveSpeed = GameData.PropMoveSpeed;
        }
    }


    ///// <summary>
    ///// 坑人地雷
    ///// </summary>
    // public abstract class DebuffMine : Prop
    //{
    //     public DebuffMine(XYPosition initPos) : base(initPos) { }
    // }

    public sealed class CraftingBench : Prop
    {
        public override bool IsRigid => true;
        public override bool IgnoreCollideExecutor(IGameObj targetObj) => false;
        public CraftingBench(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.CraftingBench;
    }

    public abstract class BuffProp : Prop
    {
        public BuffProp(XY initPos) : base(initPos) { }
    }

    #region 所有增益道具
    /// <summary>
    /// 增加速度
    /// </summary>
    public sealed class AddSpeed : BuffProp
    {
        public AddSpeed(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.AddSpeed;
    }

    /// <summary>
    /// 复活甲
    /// </summary>
    public sealed class AddLifeOrClairaudience : BuffProp
    {
        public AddLifeOrClairaudience(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.AddLifeOrClairaudience;
    }
    public sealed class AddHpOrAp : BuffProp
    {
        public AddHpOrAp(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.AddHpOrAp;
    }
    public sealed class RecoveryFromDizziness : BuffProp
    {
        public RecoveryFromDizziness(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.RecoveryFromDizziness;
    }
    /// <summary>
    /// 矛盾
    /// </summary>
    public sealed class ShieldOrSpear : BuffProp
    {
        public ShieldOrSpear(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.ShieldOrSpear;
    }
    public sealed class Key3 : BuffProp
    {
        public Key3(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key3;
    }
    public sealed class Key5 : BuffProp
    {
        public Key5(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key5;
    }
    public sealed class Key6 : BuffProp
    {
        public Key6(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key6;
    }
    public sealed class NullProp : Prop
    {
        public NullProp() : base(new XY(1, 1))
        {
        }
        public override PropType GetPropType() => PropType.Null;
    }
    #endregion
    // #region 所有坑人地雷
    ///// <summary>
    ///// 减速
    ///// </summary>
    // public sealed class MinusSpeed : DebuffMine
    //{
    //     public MinusSpeed(XYPosition initPos) : base(initPos) { }
    //     public override PropType GetPropType() => PropType.minusSpeed;
    // }
    ///// <summary>
    ///// 减少攻击力
    ///// </summary>
    // public sealed class MinusAP : DebuffMine
    //{
    //     public MinusAP(XYPosition initPos) : base(initPos) { }
    //     public override PropType GetPropType() => PropType.minusAP;
    // }
    ///// <summary>
    ///// 增加冷却
    ///// </summary>
    // public sealed class AddCD : DebuffMine
    //{
    //     public AddCD(XYPosition initPos) : base(initPos) { }
    //     public override PropType GetPropType() => PropType.addCD;
    // }
    // #endregion
    public static class PropFactory
    {
        public static Prop GetProp(PropType propType, XY pos)
        {
            switch (propType)
            {
                case PropType.CraftingBench:
                    return new CraftingBench(pos);
                case PropType.AddSpeed:
                    return new AddSpeed(pos);
                case PropType.AddLifeOrClairaudience:
                    return new AddLifeOrClairaudience(pos);
                case PropType.ShieldOrSpear:
                    return new ShieldOrSpear(pos);
                case PropType.AddHpOrAp:
                    return new AddHpOrAp(pos);
                case PropType.RecoveryFromDizziness:
                    return new RecoveryFromDizziness(pos);
                case PropType.Key3:
                    return new Key3(pos);
                case PropType.Key5:
                    return new Key5(pos);
                case PropType.Key6:
                    return new Key6(pos);
                default:
                    return new NullProp();
            }
        }
    }
}