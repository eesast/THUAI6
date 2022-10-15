using System.Collections.Generic;
using System.Threading;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IMap
    {
        ITimer Timer { get; }

        // the two dicts must have same keys
        Dictionary<GameObjIdx, IList<IGameObj>> GameObjDict { get; }
        Dictionary<GameObjIdx, ReaderWriterLockSlim> GameObjLockDict { get; }

        public bool IsOutOfBound(IGameObj obj);
        public IOutOfBound GetOutOfBound(XYPosition pos);  // 返回新建的一个OutOfBound对象
    }
}
