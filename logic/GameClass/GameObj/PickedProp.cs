using Preparation.Utility;
using System;
namespace GameClass.GameObj
{
    // 为方便界面组做道具拾起特效，现引入“被捡起的道具”，在每帧发送给界面组
    public class PickedProp : Immovable
    {
        public override ShapeType Shape => ShapeType.Circle;
        public override bool IsRigid => false;
        public long MappingID { get; }
        public readonly Prop propHasPicked;
        public PickedProp(Prop prop) :
            base(prop.Position, prop.Radius, GameObjType.PickedProp)
        {
            this.propHasPicked = prop;
            this.MappingID = prop.ID;
        }
    }
}
