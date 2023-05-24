using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    public abstract class Item : ObjOfCharacter
    {
        public override bool IsRigid => true;

        public override bool IgnoreCollideExecutor(IGameObj targetObj) => false;

        public override ShapeType Shape => ShapeType.Square;

        public abstract PropType GetPropType();

        public Item(XY initPos, int radius = GameData.propRadius) :
            base(initPos, radius, GameObjType.Item)
        {
            this.ReSetCanMove(false);
            this.MoveSpeed = 0;
        }
    }


    ///// <summary>
    ///// 坑人地雷
    ///// </summary>
    // public abstract class DebuffMine : Gadget
    //{
    //     public DebuffMine(XYPosition initPos) : base(initPos) { }
    // }

    public sealed class CraftingBench : Item
    {
        public CraftingBench(XY initPos, Character character, int num) :
            base(initPos)
        {
            Parent = character;
            this.num = num;
        }
        private readonly int num;
        private long parentStateNum;
        public long ParentStateNum
        {
            get => Interlocked.Read(ref parentStateNum);
            set => Interlocked.Exchange(ref parentStateNum, value);
        }
        public void StopSkill()
        {
            ((SummonGolem)Parent!.FindActiveSkill(ActiveSkillType.SummonGolem)).DeleteGolem((int)num);
        }
        public void TryStopSkill()
        {
            Parent!.ResetPlayerState(parentStateNum);
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