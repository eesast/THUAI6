using System;
using System.Threading;

namespace Preparation.Utility
{

    //其对应属性不应当有set访问器，避免不安全的=赋值
    /// <summary>
    /// 一个保证在[0,maxValue]的可变int，支持可变的maxValue（请确保大于0）
    /// </summary>
    public class IntWithVariableRange
    {
        private int v;
        private int maxV;
        private readonly object vLock = new();
        public IntWithVariableRange(int value, int maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set IntWithVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public IntWithVariableRange(int maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set IntWithVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public IntWithVariableRange()
        {
            v = this.maxV = int.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " ,maxValue:" + maxV.ToString();
            }
        }
        public int GetValue() { lock (vLock) return v; }
        public static implicit operator int(IntWithVariableRange aint) => aint.GetValue();
        public int GetMaxV() { lock (vLock) return maxV; }

        /// <summary>
        /// 若maxValue<=0则maxValue设为0并返回False
        /// </summary>
        public bool SetMaxV(int maxValue)
        {
            if (maxValue <= 0)
            {
                lock (vLock)
                {
                    v = maxV = 0;
                    return false;
                }
            }
            lock (vLock)
            {
                maxV = maxValue;
                if (v > maxValue) v = maxValue;
            }
            return true;
        }
        /// <summary>
        /// 应当保证该maxValue>=0
        /// </summary>
        public void SetPositiveMaxV(int maxValue)
        {
            lock (vLock)
            {
                maxV = maxValue;
                if (v > maxValue) v = maxValue;
            }
        }
        /// <summary>
        /// 如果当前值大于maxValue,则更新maxValue失败
        /// </summary>
        public bool TrySetMaxV(int maxValue)
        {
            lock (vLock)
            {
                if (v > maxValue) return false;
                maxV = maxValue;
                return true;
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
            if (value <= 0)
            {
                lock (vLock)
                {
                    return v = 0;
                }
            }
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        /// <returns>返回实际改变量</returns>
        public int AddV(int addV)
        {
            lock (vLock)
            {
                int previousV = v;
                v += addV;
                if (v < 0) v = 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }
        /// <summary>
        /// 应当保证增加值大于0
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public int AddPositiveV(int addPositiveV)
        {
            lock (vLock)
            {
                addPositiveV = Math.Min(addPositiveV, maxV - v);
                v += addPositiveV;
            }
            return addPositiveV;
        }
        public void MulV(int mulV)
        {
            if (mulV <= 0)
            {
                lock (vLock) v = 0;
                return;
            }
            lock (vLock)
            {
                v *= mulV;
                if (v > maxV) v = maxV;
            }
        }
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(int mulPositiveV)
        {
            lock (vLock)
            {
                v *= mulPositiveV;
                if (v > maxV) v = maxV;
            }
        }
        /// <returns>返回实际改变量</returns>
        public int SubV(int subV)
        {

            lock (vLock)
            {
                subPositiveV = Math.Min(subPositiveV, v);
                v -= subPositiveV;
            }
            return subPositiveV;
        }
        /// <summary>
        /// 应当保证该减少值大于0
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public int SubPositiveV(int subPositiveV)
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
        /// <returns>返回实际改变量</returns>
        public int TryAddAll(int addV)
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