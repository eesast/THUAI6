using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IDoorway : IGameObj
    {
        public TimeBasedProgressAtVariableSpeed ProgressOfDoorway { get; }
    }
}