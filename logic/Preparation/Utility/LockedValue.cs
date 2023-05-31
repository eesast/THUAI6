using System.Threading;

namespace Preparation.Utility
{
    //理论上结构体最好不可变，这里采用了可变结构。
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

        public void CompareExchange(int b, int c) => Interlocked.CompareExchange(ref v, b, c);
        /// <returns>返回操作前的值</returns>
        public int CompareExReturnOri(int b, int c) => Interlocked.CompareExchange(ref v, b, c);
    }
    public struct AtomicBool
    {
        private int v;//v==0为false,v!=0(v==1或v==-1)为true
        public AtomicBool(bool x)
        {
            v = x ? 1 : 0;
        }
        public override string ToString() => (Interlocked.CompareExchange(ref v, -1, -1) == 0) ? "false" : "true";
        public bool Get() => (Interlocked.CompareExchange(ref v, -1, -1) != 0);
        public static implicit operator bool(AtomicBool abool) => (Interlocked.CompareExchange(ref abool.v, -1, -1) != 0);

        public bool Set(bool value) => (Interlocked.Exchange(ref v, value ? 1 : 0) != 0);

        /// <returns>赋值前的值是否与将赋予的值不相同</returns>
        public bool TrySet(bool value)
        {
            int ori = Interlocked.CompareExchange(ref v, value ? 1 : 0, value ? 1 : 0);
            return value ? (ori == 0) : (ori != 0);
        }

        public bool Invert() => Interlocked.Add(ref v, -1) != 0;
        public bool And(bool x) => Interlocked.And(ref v, x ? 1 : 0) != 0;
        public bool Or(bool x) => Interlocked.Or(ref v, x ? 1 : 0) != 0;
    }
}
