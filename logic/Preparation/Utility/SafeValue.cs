using System;
using System.Threading;

namespace Preparation.Utility
{
    //理论上结构体最好不可变，这里采用了可变结构。
    //其对应属性不应当有set访问器，避免不安全的=赋值
    public struct AtomicInt
    {
        private int v;
        public AtomicInt(int x)
        {
            v = x;
        }
        public override string ToString() => Interlocked.CompareExchange(ref v, -1, -1).ToString();
        public int Get() => Interlocked.CompareExchange(ref v, -1, -1);
        public static implicit operator int(AtomicInt aint) => Interlocked.CompareExchange(ref aint.v, -1, -1);
        public int Set(int value) => Interlocked.Exchange(ref v, value);

        public int Add(int x) => Interlocked.Add(ref v, x);
        public int Sub(int x) => Interlocked.Add(ref v, -x);
        public int Inc() => Interlocked.Increment(ref v);
        public int Dec() => Interlocked.Decrement(ref v);

        public void CompareExchange(int newV, int compareTo) => Interlocked.CompareExchange(ref v, newV, compareTo);
        /// <returns>返回操作前的值</returns>
        public int CompareExReturnOri(int newV, int compareTo) => Interlocked.CompareExchange(ref v, newV, compareTo);
    }
    public struct AtomicBool
    {
        private int v;//v==0为false,v!=0(v==1或v==-1)为true
        public AtomicBool(bool x)
        {
            v = x ? 1 : 0;
        }
        public override string ToString() => (Interlocked.CompareExchange(ref v, -2, -2) == 0) ? "false" : "true";
        public bool Get() => (Interlocked.CompareExchange(ref v, -1, -1) != 0);
        public static implicit operator bool(AtomicBool abool) => (Interlocked.CompareExchange(ref abool.v, -1, -1) != 0);

        public bool Set(bool value) => (Interlocked.Exchange(ref v, value ? 1 : 0) != 0);

        /// <returns>赋值前的值是否与将赋予的值不相同</returns>
        public bool TrySet(bool value)
        {
            return (Interlocked.CompareExchange(ref v, value ? 1 : 0, value ? 0 : 1) ^ (value ? 1 : 0)) != 0;
        }

        public bool Invert() => Interlocked.Add(ref v, -1) != 0;
        public bool And(bool x) => Interlocked.And(ref v, x ? 1 : 0) != 0;
        public bool Or(bool x) => Interlocked.Or(ref v, x ? 1 : 0) != 0;
    }
    /// <summary>
    /// 一个能记录Start后完成多少进度的进度条（int），
    /// 只允许Start时修改needTime，支持TryStop使未完成的进度条清零，支持Set0使进度条强制清零
    /// </summary>
    public struct IntProgressContinuously
    {
        private long endT = long.MaxValue;
        private long needT;

        public IntProgressContinuously(long needTime)
        {
            this.needT = needTime;
        }
        public long GetEndTime() => Interlocked.CompareExchange(ref endT, -2, -2);
        public long GetNeedTime() => Interlocked.CompareExchange(ref needT, -2, -2);
        public override string ToString() => "EndTime:" + Interlocked.CompareExchange(ref endT, -2, -2).ToString() + " ms, NeedTime:" + Interlocked.CompareExchange(ref needT, -2, -2).ToString() + " ms";
        public bool IsFinished()
        {
            return Interlocked.CompareExchange(ref endT, -2, -2) <= Environment.TickCount64;
        }
        /// <summary>
        /// GetProgress<0则表明未开始
        /// </summary>
        public long GetProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
        /// <summary>
        /// GetProgressDouble<0则表明未开始
        /// </summary>
        public double GetProgressDouble()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return 1;
            return 1.0 - ((double)cutime / Interlocked.CompareExchange(ref needT, -2, -2));
        }

        public bool Start(long needTime)
        {
            //规定只有Start可以修改needT，且需要先访问endTime，从而避免锁（某种程度上endTime可以认为是needTime的锁）
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            if (needTime <= 2) Debugger.Output("Warning:the field of IntProgressContinuously is " + needTime.ToString() + ",which is too small.");
            Interlocked.Exchange(ref this.needT, needTime);
            return true;
        }
        public bool Start()
        {
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            return true;
        }
        public void Set0() => Interlocked.Exchange(ref endT, long.MaxValue);
        public void TryStop()
        {
            if (Environment.TickCount64 < Interlocked.CompareExchange(ref endT, -2, -2))
                Interlocked.Exchange(ref endT, long.MaxValue);
        }
        //增加其他新的写操作可能导致不安全
    }
    /// <summary>
    /// 一个保证在[0,maxValue]的可变int，支持可变的maxValue(请确保大于0)
    /// </summary>
    public struct IntWithVariableRange
    {
        private int v;
        private int maxV;
        private readonly object vLock = new();
        public IntWithVariableRange(int value, int maxValue)
        {
            if (maxValue <= 0) Debugger.Output("Bug:IntWithVariableRange.maxValue (" + maxValue.ToString() + ") is less than 0.");
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " ,maxValue:" + maxV.ToString();
            }
        }
        public int GetValue() { lock (vLock) return v; }
        public int GetMaxV() { lock (vLock) return maxV; }

        public void SetMaxV(int maxValue)
        {
            lock (vLock)
            {
                if (maxValue <= 0) Debugger.Output("Bug:IntWithVariableRange.maxValue (" + maxValue.ToString() + ") is less than 0.");
                maxV = maxValue;
                if (v > maxValue) v = maxValue;
            }
        }
        /// <summary>
        /// 应当保证该value>=0
        /// </summary>
        public int SetPositiveV(int value)
        {
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        public int SetV(int value)
        {
            lock (vLock)
            {
                if (value <= 0) return v = 0;
                return v = (value > maxV) ? maxV : value;
            }
        }
        public int AddV(int addV)
        {
            lock (vLock)
            {
                v += addV;
                if (v < 0) return v = 0;
                if (v > maxV) return v = maxV;
                return v;
            }
        }
        /// <summary>
        /// 应当保证该增加值大于0
        /// </summary>
        public int AddPositiveV(int addPositiveV)
        {
            lock (vLock)
            {
                v += addPositiveV;
                if (v > maxV) return v = maxV;
                return v;
            }
        }
        /// <summary>
        /// 应当保证该减少值大于0
        /// </summary>
        public int SubPositiveV(int subPositiveV)
        {
            lock (vLock)
            {
                v += subPositiveV;
                if (v < 0) return v = 0;
                return v;
            }
        }
    }
}
