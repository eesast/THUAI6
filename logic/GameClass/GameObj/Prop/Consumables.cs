using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Consumables : ObjOfCharacter
    {
        public override bool IsRigid => true;

        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Consumables || targetObj.Type == GameObjType.Bullet
                || targetObj.Type == GameObjType.Character || targetObj.Type == GameObjType.Chest)
                return true;
            return false;
        }

        public override ShapeType Shape => ShapeType.Square;

        public abstract PropType GetPropType();

        public Consumables(XY initPos, int radius = GameData.PropRadius) :
            base(initPos, radius, GameObjType.Consumables)
        {
            this.canMove = false;
            this.MoveSpeed = GameData.PropMoveSpeed;
        }
    }


    ///// <summary>
    ///// 坑人地雷
    ///// </summary>
    // public abstract class DebuffMine : Consumables
    //{
    //     public DebuffMine(XYPosition initPos) : base(initPos) { }
    // }

    #region 所有增益道具
    /// <summary>
    /// 增加速度
    /// </summary>
    public sealed class AddSpeed : Consumables
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
    public sealed class AddLifeOrClairaudience : Consumables
    {
        public AddLifeOrClairaudience(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.AddLifeOrClairaudience;
    }
    public sealed class AddHpOrAp : Consumables
    {
        public AddHpOrAp(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.AddHpOrAp;
    }
    public sealed class RecoveryFromDizziness : Consumables
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
    public sealed class ShieldOrSpear : Consumables
    {
        public ShieldOrSpear(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.ShieldOrSpear;
    }
    public sealed class Key3 : Consumables
    {
        public Key3(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key3;
    }
    public sealed class Key5 : Consumables
    {
        public Key5(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key5;
    }
    public sealed class Key6 : Consumables
    {
        public Key6(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key6;
    }
    public sealed class NullProp : Consumables
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
        public static Consumables GetConsumables(PropType propType, XY pos)
        {
            switch (propType)
            {
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