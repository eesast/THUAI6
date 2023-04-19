using System.Collections.Generic;
using System.Threading;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IMap
    {
        ITimer Timer { get; }

        // the two dicts must have same keys
        Dictionary<GameObjType, IList<IGameObj>> GameObjDict { get; }
        Dictionary<GameObjType, ReaderWriterLockSlim> GameObjLockDict { get; }

        public uint[,] ProtoGameMap { get; }
        public PlaceType GetPlaceType(IGameObj obj);
        public bool IsOutOfBound(IGameObj obj);
        public IOutOfBound GetOutOfBound(XY pos);  // 返回新建的一个OutOfBound对象
    }
}
