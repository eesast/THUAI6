using Preparation.Utility;

namespace GameClass.GameObj
{
    // 为方便界面组做子弹爆炸特效，现引入“爆炸中的子弹”，在每帧发送给界面组
    public sealed class BombedBullet : Immovable
    {
        public override ShapeType Shape => ShapeType.Circle;
        public override bool IsRigid => false;
        public long MappingID { get; }
        public readonly Bullet bulletHasBombed;
        public readonly XY facingDirection;

        public BombedBullet(Bullet bullet) :
            base(bullet.Position, bullet.Radius, GameObjType.BombedBullet)
        {
            this.bulletHasBombed = bullet;
            this.MappingID = bullet.ID;
            this.facingDirection = bullet.FacingDirection;
        }
    }
}
