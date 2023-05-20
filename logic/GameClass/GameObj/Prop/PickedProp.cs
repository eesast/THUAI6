using Preparation.Utility;
using System;
namespace GameClass.GameObj
{
    // 为方便界面组做道具拾起特效，现引入“被捡起的道具”，在每帧发送给界面组
    /*
    public class Item : Immovable
    {
        public override ShapeType Shape => ShapeType.Circle;
        public override bool IsRigid => false;
        public long MappingID { get; }
        public readonly Gadget propHasPicked;
        public Item(Gadget prop) :
            base(prop.Position, prop.Radius, GameObjType.Item)
        {
            this.propHasPicked = prop;
            this.MappingID = prop.ID;
        }
    }*/
}
