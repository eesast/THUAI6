using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Gadget : ObjOfCharacter
    {
        public override bool IsRigid => true;

        public override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Gadget || targetObj.Type == GameObjType.Bullet
                || targetObj.Type == GameObjType.Character || targetObj.Type == GameObjType.Chest)
                return true;
            return false;
        }

        public override ShapeType Shape => ShapeType.Square;

        public abstract bool IsUsable();
        public abstract PropType GetPropType();

        public Gadget(XY initPos, int radius = GameData.propRadius) :
            base(initPos, radius, GameObjType.Gadget)
        {
            this.CanMove.Set(false);
            this.MoveSpeed = GameData.propMoveSpeed;
        }
    }
    public abstract class Tool : Gadget
    {
        private bool isUsed = false;
        public bool IsUsed
        {
            get
            {
                lock (gameObjLock)
                    return isUsed;
            }
            set
            {
                lock (gameObjLock)
                {
                    isUsed = value;
                }
            }
        }
        public override bool IsUsable() => !IsUsed;
        public Tool(XY initPos) : base(initPos) { }
    }
    public abstract class Consumables : Gadget
    {
        public override bool IsUsable() => true;
        public Consumables(XY initPos) : base(initPos) { }
    }

    ///// <summary>
    ///// 坑人地雷
    ///// </summary>
    // public abstract class DebuffMine : Gadget
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
    #endregion
    public sealed class Key3 : Tool
    {
        public Key3(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key3;
    }
    public sealed class Key5 : Tool
    {
        public Key5(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key5;
    }
    public sealed class Key6 : Tool
    {
        public Key6(XY initPos) : base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.Key6;
    }
    public sealed class NullProp : Gadget
    {
        public override bool IsUsable() => false;
        public NullProp() : base(new XY(1, 1))
        {
        }
        public override PropType GetPropType() => PropType.Null;
    }
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
        public static Gadget GetConsumables(PropType propType, XY pos)
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