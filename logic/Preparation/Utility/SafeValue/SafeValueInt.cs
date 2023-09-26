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

    /*
    /// <summary>
    /// 记录（不是根据时间）完成多少进度的进度条（long）。
    /// </summary>
    public struct IntProgressByAdding
    {
        private int completedProgress = -1;
        private int requiredProgress;
        public IntProgressByAdding(int completedProgress, int requiredProgress)
        {
            this.completedProgress = completedProgress;
            this.requiredProgress = requiredProgress;
        }
        public IntProgressByAdding(int requiredProgress)
        {
            this.requiredProgress = requiredProgress;
        }
        public IntProgressByAdding() 
        {
            this.requiredProgress=int.MaxValue;
        }
    }
    */

    /// <summary>
    /// 一个保证在[0,maxValue]的可变int，支持可变的maxValue(请确保大于0)
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
        public void SetPositiveMaxV(int maxValue)
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
        public int SetPositiveV(int value)
        {
            lock (vLock)
            {
                return v = (value > maxV) ? maxV : value;
            }
        }
        public int SetV(int value)
        {
            if (value < 0) value = 0;
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
        /// <summary>
        /// 应当保证该减少值大于0,返回实际改变量
        /// </summary>
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
                return 0;
            }
        }
    }

    /// <summary>
    /// 一个保证在[0,maxNum],每CDms自动+1的int，支持可变的CD、maxNum(请确保大于0)
    /// </summary>
    public class IntNumUpdateByCD
    {
        private int num;
        private int maxNum;
        private int cd;
        private long updateTime = 0;
        private readonly object numLock = new();
        public IntNumUpdateByCD(int num, int maxNum, int cd)
        {
            if (num < 0) Debugger.Output("Bug:IntNumUpdateByCD.num (" + num.ToString() + ") is less than 0.");
            if (maxNum < 0) Debugger.Output("Bug:IntNumUpdateByCD.maxNum (" + maxNum.ToString() + ") is less than 0.");
            if (cd <= 0) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 0.");
            this.num = (num < maxNum) ? num : maxNum;
            this.maxNum = maxNum;
            this.cd = cd;
            this.updateTime = Environment.TickCount64;
        }
        /// <summary>
        /// 默认使num=maxNum
        /// </summary>
        public IntNumUpdateByCD(int maxNum, int cd)
        {
            if (maxNum < 0) Debugger.Output("Bug:IntNumUpdateByCD.maxNum (" + maxNum.ToString() + ") is less than 0.");
            if (cd <= 0) Debugger.Output("Bug:IntNumUpdateByCD.cd (" + cd.ToString() + ") is less than 0.");
            this.num = this.maxNum = maxNum;
            this.cd = cd;
        }
        public IntNumUpdateByCD()
        {
            this.num = this.maxNum = 0;
            this.cd = int.MaxValue;
        }

        public int GetMaxNum() { lock (numLock) return maxNum; }
        public int GetCD() { lock (numLock) return cd; }
        public int GetNum(long time)
        {
            lock (numLock)
            {
                if (num < maxNum && time - updateTime >= cd)
                {
                    int add = (int)Math.Min(maxNum - num, (time - updateTime) / cd);
                    updateTime += add * cd;
                    return (num += add);
                }
                return num;
            }
        }
        public static implicit operator int(IntNumUpdateByCD aint) => aint.GetNum(Environment.TickCount64);

        /// <summary>
        /// 应当保证该subV>=0
        /// </summary>
        public int TrySub(int subV)
        {
            if (subV < 0) Debugger.Output("Bug:IntNumUpdateByCD Try to sub " + subV.ToString() + ", which is less than 0.");
            long time = Environment.TickCount64;
            lock (numLock)
            {
                if (num < maxNum && time - updateTime >= cd)
                {
                    int add = (int)Math.Min(maxNum - num, (time - updateTime) / cd);
                    updateTime += add * cd;
                    num += add;
                }
                if (num == maxNum) updateTime = time;
                num -= subV = Math.Min(subV, num);
            }
            return subV;
        }
        /// <summary>
        /// 应当保证该addV>=0
        /// </summary>
        public void TryAdd(int addV)
        {
            if (addV < 0) Debugger.Output("Bug:IntNumUpdateByCD Try to add " + addV.ToString() + ", which is less than 0.");
            lock (numLock)
            {
                num += Math.Min(addV, maxNum - num);
            }
        }
        /// <summary>
        /// 若maxNum<=0则maxNum及Num设为0并返回False
        /// </summary>
        public bool SetMaxNumAndNum(int maxNum)
        {
            if (maxNum < 0) maxNum = 0;
            lock (numLock)
            {
                this.num = this.maxNum = maxNum;
            }
            return maxNum > 0;
        }
        /// <summary>
        /// 应当保证该maxnum>=0
        /// </summary>
        public void SetPositiveMaxNumAndNum(int maxNum)
        {
            lock (numLock)
            {
                this.num = this.maxNum = maxNum;
            }
        }
        /// <summary>
        /// 应当保证该maxnum>=0
        /// </summary>
        public void SetPositiveMaxNum(int maxNum)
        {
            lock (numLock)
            {
                if ((this.maxNum = maxNum) < num)
                    num = maxNum;
            }
        }
        /// <summary>
        /// 若maxNum<=0则maxNum及Num设为0并返回False
        /// </summary>
        public bool SetMaxNum(int maxNum)
        {
            if (maxNum < 0) maxNum = 0;
            lock (numLock)
            {
                if ((this.maxNum = maxNum) < num)
                    num = maxNum;
            }
            return maxNum > 0;
        }
        /// <summary>
        /// 若num<0则num设为0并返回False
        /// </summary>
        public bool SetNum(int num)
        {
            lock (numLock)
            {
                if (num < 0)
                {
                    this.num = 0;
                    updateTime = Environment.TickCount64;
                    return false;
                }
                if (num < maxNum)
                {
                    if (this.num == maxNum) updateTime = Environment.TickCount64;
                    this.num = num;
                }
                else this.num = maxNum;
                return true;
            }
        }
        /// <summary>
        /// 应当保证该num>=0
        /// </summary>
        public void SetPositiveNum(int num)
        {
            lock (numLock)
            {
                if (num < maxNum)
                {
                    if (this.num == maxNum) updateTime = Environment.TickCount64;
                    this.num = num;
                }
                else this.num = maxNum;
            }
        }
        public void SetCD(int cd)
        {
            lock (numLock)
            {
                if (cd <= 0) Debugger.Output("Bug:SetReturnOri IntNumUpdateByCD.cd to " + cd.ToString() + ".");
                this.cd = cd;
            }
        }
    }
}