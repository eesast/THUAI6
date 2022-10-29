using Preparation.Interface;
using Preparation.Utility;

namespace GameClass.GameObj
{
    /// <summary>
    /// 所有物，具有主人（Parent）（特定玩家）属性的对象
    /// </summary>
    public abstract class ObjOfCharacter : GameObj, IObjOfCharacter
    {
        private ICharacter? parent = null;  // 主人
        public ICharacter? Parent
        {
            get => parent;
            set {
                lock (gameObjLock)
                {
                    parent = value;
                }
            }
        }
        // LHR注：本来考虑在构造函数里设置parent属性，见THUAI4在游戏引擎中才设置该属性，作罢。——2021/9/24
        public ObjOfCharacter(XY initPos, int initRadius, PlaceType initPlace, GameObjType initType) :
            base(initPos, initRadius, initPlace, initType)
        {
        }
    }
}
