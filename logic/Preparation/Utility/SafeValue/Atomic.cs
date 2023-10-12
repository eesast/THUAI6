using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值
    public class AtomicInt
    {
        private int v;
        public AtomicInt(int x)
        {
            v = x;
        }
        public override string ToString() => Interlocked.CompareExchange(ref v, -1, -1).ToString();
        public int Get() => Interlocked.CompareExchange(ref v, -1, -1);
        public static implicit operator int(AtomicInt aint) => Interlocked.CompareExchange(ref aint.v, -1, -1);
        /// <returns>返回操作前的值</returns>
        public int SetReturnOri(int value) => Interlocked.Exchange(ref v, value);
        public int Add(int x) => Interlocked.Add(ref v, x);
        public int Sub(int x) => Interlocked.Add(ref v, -x);
        public int Inc() => Interlocked.Increment(ref v);
        public int Dec() => Interlocked.Decrement(ref v);
        /// <returns>返回操作前的值</returns>
        public int CompareExReturnOri(int newV, int compareTo) => Interlocked.CompareExchange(ref v, newV, compareTo);
    }

    public class AtomicLong
    {
        private long v;
        public AtomicLong(long x)
        {
            v = x;
        }
        public override string ToString() => Interlocked.Read(ref v).ToString();
        public long Get() => Interlocked.Read(ref v);
        public static implicit operator long(AtomicLong aint) => Interlocked.Read(ref aint.v);
        /// <returns>返回操作前的值</returns>
        public long SetReturnOri(long value) => Interlocked.Exchange(ref v, value);
        public long Add(long x) => Interlocked.Add(ref v, x);
        public long Sub(long x) => Interlocked.Add(ref v, -x);
        public long Inc() => Interlocked.Increment(ref v);
        public long Dec() => Interlocked.Decrement(ref v);
        /// <returns>返回操作前的值</returns>
        public long CompareExReturnOri(long newV, long compareTo) => Interlocked.CompareExchange(ref v, newV, compareTo);
    }

    public class AtomicBool
    {
        private int v;//v==0为false,v==1为true
        public AtomicBool(bool x)
        {
            v = x ? 1 : 0;
        }
        public override string ToString() => (Interlocked.CompareExchange(ref v, -2, -2) == 0) ? "false" : "true";
        public bool Get() => (Interlocked.CompareExchange(ref v, -1, -1) != 0);
        public static implicit operator bool(AtomicBool abool) => (Interlocked.CompareExchange(ref abool.v, -1, -1) != 0);
        /// <returns>返回操作前的值</returns>
        public bool SetReturnOri(bool value) => (Interlocked.Exchange(ref v, value ? 1 : 0) != 0);
        /// <returns>赋值前的值是否与将赋予的值不相同</returns>
        public bool TrySet(bool value)
        {
            return (Interlocked.CompareExchange(ref v, value ? 1 : 0, value ? 0 : 1) ^ (value ? 1 : 0)) != 0;
        }
        public bool And(bool x) => Interlocked.And(ref v, x ? 1 : 0) != 0;
        public bool Or(bool x) => Interlocked.Or(ref v, x ? 1 : 0) != 0;
    }
}