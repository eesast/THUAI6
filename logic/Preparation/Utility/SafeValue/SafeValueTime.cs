using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值

    /// <summary>
    /// 根据时间推算Start后完成多少进度的进度条（long）。
    /// 只允许Start时修改needTime（请确保大于0）；
    /// 支持TrySet0使未完成的进度条终止清零；支持Set0使进度条强制终止清零；
    /// 通过原子操作实现。
    /// </summary>
    public class LongProgressByTime
    {
        private long endT = long.MaxValue;
        private long needT;

        public LongProgressByTime(long needTime)
        {
            if (needTime <= 0) Debugger.Output("Bug:LongProgressByTime.needTime (" + needTime.ToString() + ") is less than 0.");
            this.needT = needTime;
        }
        public LongProgressByTime()
        {
            this.needT = 0;
        }
        public long GetEndTime() => Interlocked.CompareExchange(ref endT, -2, -2);
        public long GetNeedTime() => Interlocked.CompareExchange(ref needT, -2, -2);
        public override string ToString() => "EndTime:" + Interlocked.CompareExchange(ref endT, -2, -2).ToString() + " ms, NeedTime:" + Interlocked.CompareExchange(ref needT, -2, -2).ToString() + " ms";
        public bool IsFinished()
        {
            return Interlocked.CompareExchange(ref endT, -2, -2) <= Environment.TickCount64;
        }
        public bool IsOpened() => Interlocked.Read(ref endT) != long.MaxValue;
        /// <summary>
        /// GetProgress<0则表明未开始
        /// </summary>
        public long GetProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
        public long GetNonNegativeProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            long progress = Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
            return progress < 0 ? 0 : progress;
        }
        /// <summary>
        /// GetProgress<0则表明未开始
        /// </summary>
        public long GetProgress(long time)
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - time;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
        public long GetNonNegativeProgress(long time)
        {
            long cutime = Interlocked.Read(ref endT) - time;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            long progress = Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
            return progress < 0 ? 0 : progress;
        }
        /// <summary>
        /// <0则表明未开始
        /// </summary>
        public static implicit operator long(LongProgressByTime pLong) => pLong.GetProgress();

        /// <summary>
        /// GetProgressDouble<0则表明未开始
        /// </summary>
        public double GetProgressDouble()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return 1;
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (needTime == 0) return 0;
            return 1.0 - ((double)cutime / needTime);
        }
        public double GetNonNegativeProgressDouble(long time)
        {
            long cutime = Interlocked.Read(ref endT) - time;
            if (cutime <= 0) return 1;
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (needTime <= cutime) return 0;
            return 1.0 - ((double)cutime / needTime);
        }

        public bool Start(long needTime)
        {
            if (needTime <= 0)
            {
                Debugger.Output("Warning:Start LongProgressByTime with the needTime (" + needTime.ToString() + ") which is less than 0.");
                return false;
            }
            //规定只有Start可以修改needT，且需要先访问endTime，从而避免锁（某种程度上endTime可以认为是needTime的锁）
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            if (needTime <= 2) Debugger.Output("Warning:the field of LongProgressByTime is " + needTime.ToString() + ",which is too small.");
            Interlocked.Exchange(ref needT, needTime);
            return true;
        }
        public bool Start()
        {
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            return true;
        }
        /// <summary>
        /// 使进度条强制终止清零
        /// </summary>
        public void Set0() => Interlocked.Exchange(ref endT, long.MaxValue);
        /// <summary>
        /// 使未完成的进度条终止清零
        /// </summary>
        public bool TrySet0()
        {
            if (Environment.TickCount64 < Interlocked.CompareExchange(ref endT, -2, -2))
            {
                Interlocked.Exchange(ref endT, long.MaxValue);
                return true;
            }
            return false;
        }
        //增加其他新的写操作可能导致不安全
    }

    /// <summary>
    /// 一个每CDms自动更新冷却的bool，支持可变的无锁CD，不支持查看当前进度，初始为True
    /// </summary>
    public class BoolCoolingDownByCD
    {
        private long cd;
        private long nextUpdateTime = 0;
        public BoolCoolingDownByCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public BoolCoolingDownByCD(long cd)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public BoolCoolingDownByCD(long cd, long startTime)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
            this.nextUpdateTime = startTime;
        }

        public long GetCD() => Interlocked.Read(ref cd);

        public bool TryUse()
        {
            long needTime = Interlocked.Exchange(ref nextUpdateTime, long.MaxValue);
            if (needTime <= Environment.TickCount64)
            {
                Interlocked.Exchange(ref nextUpdateTime, Environment.TickCount64 + Interlocked.Read(ref cd));
                return true;
            }
            Interlocked.Exchange(ref nextUpdateTime, needTime);
            return false;
        }
        public void SetCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:SetReturnOri IntNumUpdateByCD.cd to " + cd.ToString() + ".");
            Interlocked.Exchange(ref this.cd, cd);
        }
    }

    /// <summary>
    /// 一个每CDms自动更新的进度条，支持可变的CD，初始为满
    /// </summary>
    public class LongProgressCoolingDownByCD
    {
        private int isusing = 0;
        private long cd;
        private long nextUpdateTime = 0;
        public LongProgressCoolingDownByCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public LongProgressCoolingDownByCD(long cd)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public LongProgressCoolingDownByCD(long cd, long startTime)
        {
            if (cd <= 1) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
            this.nextUpdateTime = startTime;
        }

        public long GetRemainingTime()
        {
            long v = Interlocked.Read(ref nextUpdateTime) - Environment.TickCount64;
            return v < 0 ? 0 : v;
        }
        public long GetCD() => Interlocked.Read(ref cd);

        public bool TryUse()
        {
            if (Interlocked.Exchange(ref isusing, 1) == 1) return false;
            long needTime = Interlocked.Read(ref nextUpdateTime);
            if (needTime <= Environment.TickCount64)
            {
                Interlocked.Exchange(ref nextUpdateTime, Environment.TickCount64 + Interlocked.Read(ref cd));
                Interlocked.Exchange(ref isusing, 0);
                return true;
            }
            Interlocked.Exchange(ref isusing, 0);
            return false;
        }
        public void SetCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:SetReturnOri IntNumUpdateByCD.cd to " + cd.ToString() + ".");
            Interlocked.Exchange(ref this.cd, cd);
        }
    }
}