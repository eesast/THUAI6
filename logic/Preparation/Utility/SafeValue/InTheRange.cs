using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值

    /// <summary>
    /// 一个保证在[0,maxValue]的可变int，支持可变的maxValue（请确保大于0）
    /// </summary>
    public class IntInTheVariableRange
    {
        private int v;
        private int maxV;
        private readonly object vLock = new();
        public IntInTheVariableRange(int value, int maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set IntInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public IntInTheVariableRange(int maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set IntInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public IntInTheVariableRange()
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
        public static implicit operator int(IntInTheVariableRange aint) => aint.GetValue();
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
                if (v > maxV / mulV) v = maxV; //避免溢出
                else v *= mulV;
            }
        }
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(int mulPositiveV)
        {
            lock (vLock)
            {
                if (v > maxV / mulPositiveV) v = maxV; //避免溢出
                else v *= mulPositiveV;
            }
        }
        /// <returns>返回实际改变量</returns>
        public int SubV(int subV)
        {
            lock (vLock)
            {
                int previousV = v;
                v -= subV;
                if (v < 0) v = 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
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
        public int TryAddToMaxV(int addV)
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

        public bool Set0IfNotMax()
        {
            lock (vLock)
            {
                if (v < maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }
        public bool Set0IfMax()
        {
            lock (vLock)
            {
                if (v == maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }

        public bool IsMaxV()
        {
            lock (vLock)
            {
                return v == maxV;
            }
        }
    }

    /// <summary>
    /// 一个保证在[0,maxValue]的可变long，支持可变的maxValue(请确保大于0)
    /// </summary>
    public class LongInTheVariableRange
    {
        private long v;
        private long maxV;
        private readonly object vLock = new();
        public LongInTheVariableRange(long value, long maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set SafaValues.LongInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public LongInTheVariableRange(long maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set SafaValues.LongInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public LongInTheVariableRange()
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
        public static implicit operator long(LongInTheVariableRange aint) => aint.GetValue();
        public long GetMaxV() { lock (vLock) return maxV; }

        /// <summary>
        /// 若maxValue<=0则maxValue设为0并返回False
        /// </summary>
        public bool SetMaxV(long maxValue)
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
        public void SetPositiveMaxV(long maxValue)
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
        public bool TrySetMaxV(long maxValue)
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
        public long SetPositiveV(long value)
        {
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        public long SetV(long value)
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
        /// 应当保证增加值大于0
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public long AddPositiveV(long addPositiveV)
        {
            lock (vLock)
            {
                addPositiveV = Math.Min(addPositiveV, maxV - v);
                v += addPositiveV;
            }
            return addPositiveV;
        }
        public void MulV(long mulV)
        {
            if (mulV <= 0)
            {
                lock (vLock) v = 0;
                return;
            }
            lock (vLock)
            {
                if (v > maxV / mulV) v = maxV; //避免溢出
                else v *= mulV;
            }
        }
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(long mulPositiveV)
        {
            lock (vLock)
            {
                if (v > maxV / mulPositiveV) v = maxV; //避免溢出
                else v *= mulPositiveV;
            }
        }
        /// <returns>返回实际改变量</returns>
        public long SubV(long subV)
        {
            lock (vLock)
            {
                long previousV = v;
                v -= subV;
                if (v < 0) v = 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }
        /// <summary>
        /// 应当保证该减少值大于0
        /// </summary>
        /// <returns>返回实际改变量</returns>
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
        /// <returns>返回实际改变量</returns>
        public long TryAddToMaxV(long addV)
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

        public bool Set0IfNotMax()
        {
            lock (vLock)
            {
                if (v < maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }
        public bool Set0IfMax()
        {
            lock (vLock)
            {
                if (v == maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }

        public bool IsMaxV()
        {
            lock (vLock)
            {
                return v == maxV;
            }
        }
    }

    /// <summary>
    /// 一个保证在[0,maxValue]的可变double，支持可变的maxValue（请确保大于0）
    /// </summary>
    public class DoubleInTheVariableRange
    {
        private double v;
        private double maxV;
        private readonly object vLock = new();
        public DoubleInTheVariableRange(double value, double maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set DoubleInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = value < maxValue ? value : maxValue;
            this.maxV = maxValue;
        }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public DoubleInTheVariableRange(double maxValue)
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set DoubleInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public DoubleInTheVariableRange()
        {
            v = this.maxV = double.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " ,maxValue:" + maxV.ToString();
            }
        }
        public double GetValue() { lock (vLock) return v; }
        public static implicit operator double(DoubleInTheVariableRange adouble) => adouble.GetValue();
        public double GetMaxV() { lock (vLock) return maxV; }

        /// <summary>
        /// 若maxValue<=0则maxValue设为0并返回False
        /// </summary>
        public bool SetMaxV(double maxValue)
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
        public void SetPositiveMaxV(double maxValue)
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
        public bool TrySetMaxV(double maxValue)
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
        public double SetPositiveV(double value)
        {
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        public double SetV(double value)
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
        public double AddV(double addV)
        {
            lock (vLock)
            {
                double previousV = v;
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
        public double AddPositiveV(double addPositiveV)
        {
            lock (vLock)
            {
                addPositiveV = Math.Min(addPositiveV, maxV - v);
                v += addPositiveV;
            }
            return addPositiveV;
        }
        public void MulV(double mulV)
        {
            if (mulV <= 0)
            {
                lock (vLock) v = 0;
                return;
            }
            lock (vLock)
            {
                if (v > maxV / mulV) v = maxV; //避免溢出
                else v *= mulV;
            }
        }
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(double mulPositiveV)
        {
            lock (vLock)
            {
                if (v > maxV / mulPositiveV) v = maxV; //避免溢出
                else v *= mulPositiveV;
            }
        }
        /// <returns>返回实际改变量</returns>
        public double SubV(double subV)
        {
            lock (vLock)
            {
                double previousV = v;
                v -= subV;
                if (v < 0) v = 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }
        /// <summary>
        /// 应当保证该减少值大于0
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public double SubPositiveV(double subPositiveV)
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
        public double TryAddToMaxV(double addV)
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

        public bool Set0IfNotMax()
        {
            lock (vLock)
            {
                if (v < maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }
        public bool Set0IfMax()
        {
            lock (vLock)
            {
                if (v == maxV)
                {
                    v = 0;
                    return true;
                }
            }
            return false;
        }

        public bool IsMaxV()
        {
            lock (vLock)
            {
                return v == maxV;
            }
        }
    }
}