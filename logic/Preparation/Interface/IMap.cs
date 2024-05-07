using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IMap
    {
        ITimer Timer { get; }

        // the two dicts must have same keys
        Dictionary<GameObjType, LockedClassList<IGameObj>> GameObjDict { get; }

        public uint[,] ProtoGameMap { get; }
        public PlaceType GetPlaceType(IGameObj obj);
        public bool IsOutOfBound(IGameObj obj);
        public IOutOfBound GetOutOfBound(XY pos);  // 返回新建的一个OutOfBound对象
    }
}
