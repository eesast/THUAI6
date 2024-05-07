﻿using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值

    public class InTheVariableRange
    {
        protected readonly object vLock = new();
        public object VLock => vLock;
        private static int numOfClass = 0;
        public static int NumOfClass => numOfClass;
        public readonly int idInClass;
        public int IdInClass => idInClass;
        public InTheVariableRange()
        {
            idInClass = Interlocked.Increment(ref numOfClass);
        }
    }

    /// <summary>
    /// 一个保证在[0,maxValue]的可变int，支持可变的maxValue（请确保大于0）
    /// </summary>
    public class IntInTheVariableRange : InTheVariableRange
    {
        protected int v;
        protected int maxV;
        #region 构造与读取
        public IntInTheVariableRange(int value, int maxValue) : base()
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
        public IntInTheVariableRange(int maxValue) : base()
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set IntInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public IntInTheVariableRange() : base()
        {
            v = this.maxV = int.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " , maxValue:" + maxV.ToString();
            }
        }
        public int GetValue() { lock (vLock) return v; }
        public static implicit operator int(IntInTheVariableRange aint) => aint.GetValue();
        public int GetMaxV() { lock (vLock) return maxV; }
        public (int, int) GetValueAndMaxV() { lock (vLock) return (v, maxV); }
        public bool IsMaxV() { lock (vLock) return v == maxV; }
        #endregion

        #region 内嵌读取（在锁的情况下读取内容同时读取其他更基本的外部数据）
        public (int, long) GetValue(StartTime startTime)
        {
            lock (vLock)
            {
                return (v, startTime.Get());
            }
        }
        public (int, int, long) GetValueAndMaxValue(StartTime startTime)
        {
            lock (vLock)
            {
                return (v, maxV, startTime.Get());
            }
        }
        #endregion

        #region 普通设置MaxV与Value的值的方法
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
        #endregion

        #region 特殊条件的设置MaxV与Value的值的方法
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
        public void SetVToMaxV()
        {
            lock (vLock)
            {
                v = maxV;
            }
        }
        public bool Set0IfNotMaxor0()
        {
            lock (vLock)
            {
                if (v < maxV && v > 0)
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
        #endregion

        #region 普通运算
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
                else v = (int)(v * mulV);
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
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(double mulPositiveV)
        {
            lock (vLock)
            {
                if (v > maxV / mulPositiveV) v = maxV; //避免溢出
                else v = (int)(v * mulPositiveV);
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
        #endregion

        #region 特殊条件的运算
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
        #endregion

        #region 与InTheVariableRange类的运算，运算会影响该对象的值
        public int AddV(IntInTheVariableRange a)
        {
            if (this.idInClass == a.idInClass) return -1;
            bool thisLock = false;
            bool thatLock = false;
            try
            {
                if (this.idInClass < a.idInClass)
                {
                    Monitor.Enter(vLock, ref thisLock);
                    Monitor.Enter(a.VLock, ref thatLock);
                }
                else
                {
                    Monitor.Enter(a.VLock, ref thatLock);
                    Monitor.Enter(vLock, ref thisLock);
                }
                int previousV = v;
                v += a.GetValue();
                if (v > maxV) v = maxV;
                a.SubPositiveV(v - previousV);
                return v - previousV;
            }
            finally
            {
                if (thisLock) Monitor.Exit(vLock);
                if (thatLock) Monitor.Exit(a.VLock);
            }
        }
        public int SubV(IntInTheVariableRange a)
        {
            if (this.idInClass == a.idInClass) return -1;
            bool thisLock = false;
            bool thatLock = false;
            try
            {
                if (this.idInClass < a.idInClass)
                {
                    Monitor.Enter(vLock, ref thisLock);
                    Monitor.Enter(a.VLock, ref thatLock);
                }
                else
                {
                    Monitor.Enter(a.VLock, ref thatLock);
                    Monitor.Enter(vLock, ref thisLock);
                }
                int previousV = v;
                v -= a.GetValue();
                if (v < 0) v = 0;
                a.SubPositiveV(previousV - v);
                return previousV - v;
            }
            finally
            {
                if (thisLock) Monitor.Exit(vLock);
                if (thatLock) Monitor.Exit(a.VLock);
            }
        }
        #endregion

        #region 与StartTime类的特殊条件的运算，运算会影响StartTime类的值
        /// <summary>
        /// 试图加到满，如果加上时间差*速度可以达到MaxV，则加上并使startTime变为long.MaxValue
        /// 如果无法加到maxValue则不加
        /// </summary>
        /// <returns>返回试图加到的值与最大值</returns>
        public (int, int, long) TryAddToMaxV(StartTime startTime, double speed = 1.0)
        {
            lock (vLock)
            {
                long addV = (long)(startTime.StopIfPassing(maxV - v) * speed);
                if (addV < 0) return (v, maxV, startTime.Get());
                if (maxV - v < addV) return (v = maxV, maxV, startTime.Get());
                return ((int)(v + addV), maxV, startTime.Get());
            }
        }
        /// <summary>
        /// 增加量为时间差*速度，并将startTime变为long.MaxValue
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public int AddV(StartTime startTime, double speed = 1.0)
        {
            lock (vLock)
            {
                int previousV = v;
                int addV = (int)((Environment.TickCount64 - startTime.Stop()) * speed);
                if (addV < 0) v += addV;
                else return 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }

        /// <summary>
        /// 试图加到满，如果加上时间差*速度可以达到MaxV，则加上并使startTime变为long.MaxValue
        /// 如果无法加到maxValue则清零
        /// </summary>
        /// <returns>返回是否清零</returns>
        public bool Set0IfNotAddToMaxV(StartTime startTime, double speed = 1.0)
        {
            lock (vLock)
            {
                if (v == maxV) return false;
                int addV = (int)(startTime.StopIfPassing(maxV - v) * speed);
                if (addV < 0)
                {
                    v = 0;
                    return true;
                }
                if (maxV - v < addV)
                {
                    v = maxV;
                    return false;
                }
                v = 0;
                return false;
            }
        }
        #endregion
    }

    /// <summary>
    /// 一个保证在[0,maxValue]的可变long，支持可变的maxValue(请确保大于0)
    /// </summary>
    public class LongInTheVariableRange : InTheVariableRange
    {
        protected long v;
        protected long maxV;
        #region 构造与读取
        public LongInTheVariableRange(long value, long maxValue) : base()
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
        public LongInTheVariableRange(long maxValue) : base()
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set SafaValues.LongInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public LongInTheVariableRange() : base()
        {
            v = this.maxV = long.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " , maxValue:" + maxV.ToString();
            }
        }
        public long GetValue() { lock (vLock) return v; }
        public static implicit operator long(LongInTheVariableRange aint) => aint.GetValue();
        public long GetMaxV() { lock (vLock) return maxV; }
        public (long, long) GetValueAndMaxV() { lock (vLock) return (v, maxV); }
        public bool IsMaxV()
        {
            lock (vLock)
            {
                return v == maxV;
            }
        }
        #endregion

        #region 普通设置MaxV与Value的值的方法
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
        #endregion

        #region 普通运算
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
                else v = (long)(v * mulV);
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
        /// <summary>
        /// 应当保证乘数大于0
        /// </summary>
        public void MulPositiveV(double mulPositiveV)
        {
            lock (vLock)
            {
                if (v > maxV / mulPositiveV) v = maxV; //避免溢出
                else v = (long)(v * mulPositiveV);
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
        #endregion

        #region 特殊条件的设置MaxV与Value的值的方法
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
        public void SetVToMaxV()
        {
            lock (vLock)
            {
                v = maxV;
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
        #endregion

        #region 特殊条件的运算
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
        #endregion

        #region 与StartTime类的特殊条件的运算，运算会影响StartTime类的值
        /// <summary>
        /// 试图加到满，如果加上时间差*速度可以达到MaxV，则加上并使startTime变为long.MaxValue
        /// 如果无法加到maxValue则不加
        /// </summary>
        /// <returns>返回试图加到的值与最大值</returns>
        public (long, long, long) TryAddToMaxV(StartTime startTime, double speed = 1.0)
        {
            lock (vLock)
            {
                long addV = (long)(startTime.StopIfPassing(maxV - v) * speed);
                if (addV < 0) return (v, maxV, startTime.Get());
                if (maxV - v < addV) return (v = maxV, maxV, startTime.Get());
                return (v + addV, maxV, startTime.Get());
            }
        }

        #endregion
    }

    /// <summary>
    /// 一个保证在[0,maxValue]的可变double，支持可变的maxValue（请确保大于0）
    /// </summary>
    public class DoubleInTheVariableRange : InTheVariableRange
    {
        protected double v;
        protected double maxV;
        #region 构造与读取
        public DoubleInTheVariableRange(double value, double maxValue) : base()
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
        public DoubleInTheVariableRange(double maxValue) : base()
        {
            if (maxValue < 0)
            {
                Debugger.Output("Warning:Try to set DoubleInTheVariableRange.maxValue to " + maxValue.ToString() + ".");
                maxValue = 0;
            }
            v = this.maxV = maxValue;
        }
        public DoubleInTheVariableRange() : base()
        {
            v = this.maxV = double.MaxValue;
        }

        public override string ToString()
        {
            lock (vLock)
            {
                return "value:" + v.ToString() + " , maxValue:" + maxV.ToString();
            }
        }
        public double GetValue() { lock (vLock) return v; }
        public static implicit operator double(DoubleInTheVariableRange adouble) => adouble.GetValue();
        public double GetMaxV() { lock (vLock) return maxV; }
        public (double, double) GetValueAndMaxV() { lock (vLock) return (v, maxV); }
        public bool IsMaxV()
        {
            lock (vLock)
            {
                return v == maxV;
            }
        }
        #endregion

        #region 普通设置MaxV与Value的值的方法
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
        #endregion

        #region 普通运算
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
        #endregion

        #region 特殊条件的设置MaxV与Value的值的方法
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
        public void SetVToMaxV()
        {
            lock (vLock)
            {
                v = maxV;
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
        #endregion

        #region 特殊条件的运算
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
        #endregion
    }
}