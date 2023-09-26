using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值
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

    /// <summary>
    /// 一个保证在[0,maxValue]的可变long，支持可变的maxValue(请确保大于0)
    /// </summary>
    public class LongWithVariableRange
    {
        private long v;
        private long maxV;
        private readonly object vLock = new();
        public LongWithVariableRange(long value, long maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set SafaValues.LongWithVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public LongWithVariableRange(long maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set SafaValues.LongWithVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public LongWithVariableRange()
        {
            v = this.maxV = long.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " ,maxValue:" + maxV.ToString();
            }
        }
        public long GetValue() { lock (vLock) return v; }
        public static implicit operator long(LongWithVariableRange aint) => aint.GetValue();
        public long GetMaxV() { lock (vLock) return maxV; }

        /// <summary>
        /// 若maxValue<=0则maxValue设为0并返回False
        /// </summary>
        public bool SetMaxV(long maxValue)
        {
            if (maxValue < 0) maxValue = 0;
            lock (vLock)
            {
                maxV = maxValue;
                if (v > maxValue) v = maxValue;
            }
            return maxValue > 0;
        }
        /// <summary>
        /// 应当保证该maxValue>=0
        /// </summary>
        public void SetPositiveMaxV(long maxValue)
        {
            lock (vLock)
            {
                maxV = maxValue;
                if (v > maxValue) v = maxValue;
            }
        }
        /// <summary>
        /// 应当保证该value>=0
        /// </summary>
        public long SetPositiveV(long value)
        {
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        public long SetV(long value)
        {
            if (value < 0) value = 0;
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        /// <summary>
        /// 返回实际改变量
        /// </summary>
        public long AddV(long addV)
        {
            lock (vLock)
            {
                long previousV = v;
                v += addV;
                if (v < 0) v = 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }
        /// <summary>
        /// 应当保证该增加值大于0,返回实际改变量
        /// </summary>
        public long AddPositiveV(long addPositiveV)
        {
            lock (vLock)
            {
                addPositiveV = Math.Min(addPositiveV, maxV - v);
                v += addPositiveV;
            }
            return addPositiveV;
        }
        /// <summary>
        /// 应当保证该减少值大于0,返回实际改变量
        /// </summary>
        public long SubPositiveV(long subPositiveV)
        {
            lock (vLock)
            {
                subPositiveV = Math.Min(subPositiveV, v);
                v -= subPositiveV;
            }
            return subPositiveV;
        }

        /// <summary>
        /// 试图加到满，如果无法加到maxValue则不加并返回-1
        /// </summary>
        public long TryAddAll(long addV)
        {
            lock (vLock)
            {
                if (maxV - v <= addV)
                {
                    addV = maxV - v;
                    v = maxV;
                    return addV;
                }
                return -1;
            }
        }
    }
}