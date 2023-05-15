using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public abstract class Prop : ObjOfCharacter
    {
        public override bool IsRigid => true;

        public override bool IgnoreCollideExecutor(IGameObj targetObj) => false;

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
    // public abstract class DebuffMine : Consumables
    //{
    //     public DebuffMine(XYPosition initPos) : base(initPos) { }
    // }

    public sealed class CraftingBench : Prop
    {
        public CraftingBench(XY initPos) :
            base(initPos)
        {
        }
        public override PropType GetPropType() => PropType.CraftingBench;
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
}