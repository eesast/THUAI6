using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Prop : ObjOfCharacter
    {
        public override bool IsRigid => true;

        protected override bool IgnoreCollideExecutor(IGameObj targetObj)
        {
            if (targetObj.Type == GameObjType.Prop || targetObj.Type == GameObjType.Bullet
                || targetObj.Type == GameObjType.Character || targetObj.Type == GameObjType.Chest)
                return true;
            return false;
        }

        public override ShapeType Shape => ShapeType.Square;

        public abstract PropType GetPropType();

        public Prop(XY initPos, PlaceType place, int radius = GameData.PropRadius) :
            base(initPos, radius, GameObjType.Prop)
        {
            this.place = place;
            this.CanMove = false;
            this.moveSpeed = GameData.PropMoveSpeed;
        }
    }



    ///// <summary>
    ///// 坑人地雷
    ///// </summary>
    // public abstract class DebuffMine : Prop
    //{
    //     public DebuffMine(XYPosition initPos) : base(initPos) { }
    // }
    #region 所有增益道具
    /// <summary>
    /// 增加速度
    /// </summary>
    public sealed class AddSpeed : Prop
    {
        public AddSpeed(XY initPos, PlaceType placeType) :
            base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.addSpeed;
    }
    /// <summary>
    /// 复活甲
    /// </summary>
    public sealed class AddLIFE : Prop
    {
        public AddLIFE(XY initPos, PlaceType placeType) :
            base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.addLIFE;
    }
    /// <summary>
    /// 护盾
    /// </summary>
    public sealed class Shield : Prop
    {
        public Shield(XY initPos, PlaceType placeType) : base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.Shield;
    }
    /// <summary>
    /// 矛
    /// </summary>
    public sealed class Spear : Prop
    {
        public Spear(XY initPos, PlaceType placeType) : base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.Spear;
    }
    public sealed class Key3 : Prop
    {
        public Key3(XY initPos, PlaceType placeType) : base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.Key3;
    }
    public sealed class Key5 : Prop
    {
        public Key5(XY initPos, PlaceType placeType) : base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.Key5;
    }
    public sealed class Key6 : Prop
    {
        public Key6(XY initPos, PlaceType placeType) : base(initPos, placeType)
        {
        }
        public override PropType GetPropType() => PropType.Key6;
    }
    public sealed class Null : Prop
    {
        public Null(XY initPos, PlaceType placeType) : base(initPos, placeType)
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
}
