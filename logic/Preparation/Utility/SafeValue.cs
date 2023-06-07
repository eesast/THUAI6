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
        public long GetProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
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
        //增加新的写操作可能导致不安全
    }
}
