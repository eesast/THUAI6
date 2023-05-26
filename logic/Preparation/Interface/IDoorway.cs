using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IDoorway : IGameObj
    {
        public long OpenStartTime { get; }
        public int OpenDegree { get; }
        public bool StopOpenning();
        public bool TryToOpen();
    }
}