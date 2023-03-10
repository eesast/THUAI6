using Preparation.Utility;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameClass.GameObj
{
    // 为方便界面组做道具拾起特效，现引入“被捡起的道具”，在每帧发送给界面组
    public class PickedProp : GameObj
    {
        public override ShapeType Shape => ShapeType.Circle;
        public override bool IsRigid => false;
        public long MappingID { get; }
        public Prop PropHasPicked;
        public PickedProp(Prop prop) :
            base(prop.Position, prop.Radius, GameObjType.PickedProp)
        {
            this.PropHasPicked = prop;
            this.MappingID = prop.ID;
        }
    }
}
