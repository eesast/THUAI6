
namespace Preparation.Interface
{
    public interface ITimer
    {
        bool IsGaming { get; }
        public bool StartGame(int timeInMilliseconds);
    }
}
