using Google.Protobuf.WellKnownTypes;
using Preparation.Interface;
using Preparation.Utility;
using System.Threading;

namespace GameClass.GameObj
{
    /// <summary>
    /// 所有物，具有主人（Parent）（特定玩家）属性的对象
    /// </summary>
    public abstract class ObjOfCharacter : Moveable, IObjOfCharacter
    {
        private ReaderWriterLockSlim objOfCharacterReaderWriterLock = new();
        public ReaderWriterLockSlim ObjOfCharacterReaderWriterLock => objOfCharacterReaderWriterLock;
        private ICharacter? parent = null;  // 主人
        public ICharacter? Parent
        {
            get
            {
                lock (objOfCharacterReaderWriterLock)
                {
                    return parent;
                }
            }
            set
            {
                lock (objOfCharacterReaderWriterLock)
                {
                    parent = value;
                }
            }
        }
        // LHR注：本来考虑在构造函数里设置parent属性，见THUAI4在游戏引擎中才设置该属性，作罢。——2021/9/24
        public ObjOfCharacter(XY initPos, int initRadius, GameObjType initType) :
            base(initPos, initRadius, initType)
        {
        }
    }
}
