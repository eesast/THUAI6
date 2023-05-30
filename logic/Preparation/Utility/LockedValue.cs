using System.Threading;

namespace Preparation.Utility
{
    public struct AtomicInt
    {
        private int v;
        public AtomicInt(int x)
        {
            v = x;
        }
        public override string ToString() => Interlocked.CompareExchange(ref v, -1, -1).ToString();
        public int Get() => Interlocked.CompareExchange(ref v, -1, -1);
        public int Set(int value) => Interlocked.Exchange(ref v, value);

        public int Add(int x) => Interlocked.Add(ref v, x);
        public int Sub(int x) => Interlocked.Add(ref v, -x);
        public int Inc() => Interlocked.Increment(ref v);
        public int Dec() => Interlocked.Decrement(ref v);

        public int CompareExchange(int b, int c) => Interlocked.CompareExchange(ref v, b, c);
    }
}
