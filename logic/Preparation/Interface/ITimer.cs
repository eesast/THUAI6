
namespace Preparation.Interface
{
    public interface ITimer
    {
        bool IsGaming { get; set; }
        public int nowTime();
        public bool StartGame(int timeInMilliseconds);
    }
}
