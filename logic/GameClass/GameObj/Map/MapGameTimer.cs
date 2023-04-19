using System;
using System.Threading;
using Preparation.Interface;

namespace GameClass.GameObj
{
    public partial class Map
    {
        // xfgg说：爱因斯坦说，每个坐标系都有与之绑定的时钟，(x, y, z, ict) 构成四维时空坐标，在洛伦兹变换下满足矢量性（狗头）
        private readonly GameTimer timer = new();
        public ITimer Timer => timer;

        public class GameTimer : ITimer
        {
            private int startTime;
            public int nowTime() => Environment.TickCount - startTime;

            private bool isGaming = false;
            public bool IsGaming
            {
                get => isGaming;
                set
                {
                    lock (isGamingLock)
                        isGaming = value;
                }
            }

            readonly object isGamingLock = new();

            public bool StartGame(int timeInMilliseconds)
            {
                lock (isGamingLock)
                {
                    if (isGaming)
                        return false;
                    isGaming = true;
                    startTime = Environment.TickCount;
                }
                Thread.Sleep(timeInMilliseconds);
                isGaming = false;
                return true;
            }
        }
    }
}
