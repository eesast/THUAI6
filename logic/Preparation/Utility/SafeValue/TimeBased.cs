﻿using System;
using System.Threading;

namespace Preparation.Utility
{
    //其对应属性不应当有set访问器，避免不安全的=赋值

    /// <summary>
    /// 记录上次Start的时间，尚未Start则为long.MaxValue
    /// 当前不为long.MaxValue则不能Start
    /// </summary>
    public class StartTime
    {
        private long _time;
        public StartTime(long time)
        {
            _time = time;
        }
        public StartTime() { _time = long.MaxValue; }
        public long Get() => Interlocked.CompareExchange(ref _time, -2, -2);
        public override string ToString() => Get().ToString();
        /// <returns>返回操作前的值</returns>
        public long Start() => Interlocked.CompareExchange(ref _time, Environment.TickCount64, long.MaxValue);
        /// <returns>返回操作前的值</returns>
        public long Stop() => Interlocked.Exchange(ref _time, long.MaxValue);
        /// <returns>返回时间差,<0意味着未开始</returns>
        public long StopIfPassing(long passedTime)
        {
            long ans = Environment.TickCount64 - Interlocked.CompareExchange(ref _time, -2, -2);
            if (ans > passedTime)
            {
                Interlocked.Exchange(ref _time, long.MaxValue);
            }
            return ans;
        }
    }

    public class LongInTheVariableRangeWithStartTime : LongInTheVariableRange
    {
        public StartTime startTime = new();
        public LongInTheVariableRangeWithStartTime(long value, long maxValue) : base(value, maxValue) { }
        /// <summary>
        /// 默认使Value=maxValue
        /// </summary>
        public LongInTheVariableRangeWithStartTime(long maxValue) : base(maxValue) { }
        public LongInTheVariableRangeWithStartTime() : base() { }

        #region 读取
        public (long, long) GetValueWithStartTime()
        {
            lock (vLock)
            {
                return (v, startTime.Get());
            }
        }
        public (long, long, long) GetValueAndMaxVWithStartTime()
        {
            lock (vLock)
            {
                return (v, maxV, startTime.Get());
            }
        }
        #endregion

        /// <summary>
        /// 试图加到满，如果加上时间差*速度可以达到MaxV，则加上并使startTime变为long.MaxValue
        /// 如果无法加到maxValue则不加
        /// </summary>
        /// <returns>返回试图加到的值与最大值</returns>
        public (long, long, long) AddStartTimeToMaxV(double speed = 1.0)
        {
            lock (vLock)
            {
                long addV = (long)(startTime.StopIfPassing(maxV - v) * speed);
                if (addV < 0) return (v, maxV, startTime.Get());
                if (maxV - v < addV) return (v = maxV, maxV, startTime.Get());
                return (v + addV, maxV, startTime.Get());
            }
        }

        /// <summary>
        /// 增加量为时间差*速度，并将startTime变为long.MaxValue
        /// </summary>
        /// <returns>返回实际改变量</returns>
        public long AddStartTime(double speed = 1.0)
        {
            lock (vLock)
            {
                long previousV = v;
                long addV = (Environment.TickCount64 - startTime.Stop());
                if (addV < 0) v += (long)(addV * speed);
                else return 0;
                if (v > maxV) v = maxV;
                return v - previousV;
            }
        }

        /// <summary>
        /// 试图加到满，如果加上时间差*速度可以达到MaxV，则加上
        /// 如果无法加到maxValue则清零
        /// 无论如何startTime变为long.MaxValue
        /// </summary>
        /// <returns>返回是否清零</returns>
        public bool Set0IfNotAddStartTimeToMaxV(double speed = 1.0)
        {
            lock (vLock)
            {
                if (v == maxV) return false;
                long addV = (long)(startTime.Stop() * speed);
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

        public void SetAndStop(long value = 0)
        {
            lock (vLock)
            {
                this.v = value;
                startTime.Stop();
            }
        }
    }

    public class TimeBasedProgressAtVariableSpeed
    {
        private LongInTheVariableRangeWithStartTime progress;
        public AtomicDouble speed;

        #region 构造 
        public TimeBasedProgressAtVariableSpeed(long needProgress, double speed = 1.0)
        {
            progress = new LongInTheVariableRangeWithStartTime(0, needProgress);
            if (needProgress <= 0) Debugger.Output("Bug:TimeBasedProgressAtVariableSpeed.needProgress (" + needProgress.ToString() + ") is less than 0.");
            this.speed = new(speed);
        }
        public TimeBasedProgressAtVariableSpeed()
        {
            progress = new LongInTheVariableRangeWithStartTime(0, 0);
            this.speed = new(1.0);
        }
        #endregion

        #region 读取
        public override string ToString()
        {
            long progressStored, lastStartTime;
            (progressStored, lastStartTime) = progress.GetValueWithStartTime();
            return "ProgressStored: " + progressStored.ToString()
                        + " ; LastStartTime: " + lastStartTime.ToString() + "ms"
                        + " ; Speed: " + speed.ToString();
        }
        public long GetProgressNow() => progress.AddStartTimeToMaxV((double)speed).Item1;
        public (long, long, long) GetProgressNowAndNeedTimeAndLastStartTime() => progress.AddStartTimeToMaxV((double)speed);
        public long GetProgressStored() => progress.GetValue();
        public (long, long) GetProgressStoredAndNeedTime() => progress.GetValueAndMaxV();
        public (long, long, long) GetProgressStoredAndNeedTimeAndLastStartTime() => progress.GetValueAndMaxVWithStartTime();

        public bool IsFinished()
        {
            long progressNow, needTime;
            (progressNow, needTime, _) = progress.AddStartTimeToMaxV((double)speed);
            return progressNow == needTime;
        }
        public bool IsProgressing()
        {
            long progressNow, needTime, startT;
            (progressNow, needTime, startT) = progress.AddStartTimeToMaxV((double)speed);
            return (startT != long.MaxValue && progressNow != needTime);
        }
        #endregion

        public bool Start(long needTime)
        {
            if (needTime <= 2)
            {
                Debugger.Output("Warning:Start TimeBasedProgressAtVariableSpeed with the needProgress (" + needTime.ToString() + ") which is less than 0.");
                return false;
            }
            if (progress.startTime.Start() != long.MaxValue) return false;
            progress.SetMaxV(needTime);
            return true;
        }
        public bool Start()
        {
            return progress.startTime.Start() == long.MaxValue;
        }
        /// <summary>
        /// 使进度条强制终止清零
        /// </summary>
        public void Set0()
        {
            progress.SetAndStop();
        }
        /// <summary>
        /// 如果进度条加上时间差不能为满，使进度条强制终止清零
        /// </summary>
        public void TryStop()
        {
            progress.Set0IfNotAddStartTimeToMaxV(speed);
        }
        /// <summary>
        /// 使进度条暂停
        /// </summary>
        public bool Pause()
        {
            return progress.AddStartTime((double)speed) != 0;
        }
        /// <summary>
        /// 使进度条进度为满
        /// </summary>
        public void Finish()
        {
            progress.SetVToMaxV();
            progress.startTime.Stop();
        }
    }

    /// <summary>
    /// 根据时间推算Start后完成多少进度的进度条（long）。
    /// 只允许Start（清零状态的进度条才可以Start）时修改needTime（请确保大于0）；
    /// 支持InterruptToSet0使未完成的进度条终止清零；支持Set0使进度条强制终止清零；
    /// 通过原子操作实现。
    /// </summary>
    public class TimeBasedProgressOptimizedForInterrupting
    {
        private long endT = long.MaxValue;
        private long needT;

        public TimeBasedProgressOptimizedForInterrupting(long needTime)
        {
            if (needTime <= 0) Debugger.Output("Bug:TimeBasedProgressOptimizedForInterrupting.needProgress (" + needTime.ToString() + ") is less than 0.");
            this.needT = needTime;
        }
        public TimeBasedProgressOptimizedForInterrupting()
        {
            this.needT = 0;
        }
        public long GetEndTime() => Interlocked.CompareExchange(ref endT, -2, -2);
        public long GetNeedTime() => Interlocked.CompareExchange(ref needT, -2, -2);
        public override string ToString() => "EndTime:" + Interlocked.CompareExchange(ref endT, -2, -2).ToString() + " ms, NeedTime:" + Interlocked.CompareExchange(ref needT, -2, -2).ToString() + " ms";
        public bool IsFinished()
        {
            return Interlocked.CompareExchange(ref endT, -2, -2) <= Environment.TickCount64;
        }
        public bool IsStarted() => Interlocked.Read(ref endT) != long.MaxValue;
        /// <summary>
        /// GetProgress<0则表明未开始
        /// </summary>
        public long GetProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
        public long GetNonNegativeProgress()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            long progress = Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
            return progress < 0 ? 0 : progress;
        }
        /// <summary>
        /// GetProgress<0则表明未开始
        /// </summary>
        public long GetProgress(long time)
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - time;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            return Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
        }
        public long GetNonNegativeProgress(long time)
        {
            long cutime = Interlocked.Read(ref endT) - time;
            if (cutime <= 0) return Interlocked.CompareExchange(ref needT, -2, -2);
            long progress = Interlocked.CompareExchange(ref needT, -2, -2) - cutime;
            return progress < 0 ? 0 : progress;
        }
        /// <summary>
        /// <0则表明未开始
        /// </summary>
        public static implicit operator long(TimeBasedProgressOptimizedForInterrupting pLong) => pLong.GetProgress();

        /// <summary>
        /// GetProgressDouble<0则表明未开始
        /// </summary>
        public double GetProgressDouble()
        {
            long cutime = Interlocked.CompareExchange(ref endT, -2, -2) - Environment.TickCount64;
            if (cutime <= 0) return 1;
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (needTime == 0) return 0;
            return 1.0 - ((double)cutime / needTime);
        }
        public double GetNonNegativeProgressDouble(long time)
        {
            long cutime = Interlocked.Read(ref endT) - time;
            if (cutime <= 0) return 1;
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (needTime <= cutime) return 0;
            return 1.0 - ((double)cutime / needTime);
        }

        public bool Start(long needTime)
        {
            if (needTime <= 0)
            {
                Debugger.Output("Warning:Start TimeBasedProgressOptimizedForInterrupting with the needProgress (" + needTime.ToString() + ") which is less than 0.");
                return false;
            }
            //规定只有Start可以修改needT，且需要先访问endTime，从而避免锁（某种程度上endTime可以认为是needTime的锁）
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            if (needTime <= 2) Debugger.Output("Warning:the field of TimeBasedProgressOptimizedForInterrupting is " + needTime.ToString() + ",which is too small.");
            Interlocked.Exchange(ref needT, needTime);
            return true;
        }
        public bool Start()
        {
            long needTime = Interlocked.CompareExchange(ref needT, -2, -2);
            if (Interlocked.CompareExchange(ref endT, Environment.TickCount64 + needTime, long.MaxValue) != long.MaxValue) return false;
            return true;
        }
        /// <summary>
        /// 使进度条强制终止清零
        /// </summary>
        public void Set0() => Interlocked.Exchange(ref endT, long.MaxValue);
        /// <summary>
        /// 使未完成的进度条终止清零
        /// </summary>
        public bool InterruptToSet0()
        {
            if (Environment.TickCount64 < Interlocked.CompareExchange(ref endT, -2, -2))
            {
                Interlocked.Exchange(ref endT, long.MaxValue);
                return true;
            }
            return false;
        }
        //增加其他新的写操作可能导致不安全
    }

    /// <summary>
    /// 冷却时间为可变的CDms的bool，不支持查看当前进度，初始为True
    /// </summary>
    public class BoolUpdateEachCD
    {
        private long cd;
        private long nextUpdateTime = 0;
        public BoolUpdateEachCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:BoolUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public BoolUpdateEachCD(long cd)
        {
            if (cd <= 1) Debugger.Output("Bug:BoolUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public BoolUpdateEachCD(long cd, long startTime)
        {
            if (cd <= 1) Debugger.Output("Bug:BoolUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
            this.nextUpdateTime = startTime;
        }

        public long GetCD() => Interlocked.Read(ref cd);

        public bool TryUse()
        {
            long needTime = Interlocked.Exchange(ref nextUpdateTime, long.MaxValue);
            if (needTime <= Environment.TickCount64)
            {
                Interlocked.Exchange(ref nextUpdateTime, Environment.TickCount64 + Interlocked.Read(ref cd));
                return true;
            }
            Interlocked.Exchange(ref nextUpdateTime, needTime);
            return false;
        }
        public void SetCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:BoolUpdateEachCD.cd to " + cd.ToString() + ".");
            Interlocked.Exchange(ref this.cd, cd);
        }
    }

    /// <summary>
    /// 冷却时间为可变的CDms的进度条，初始为满
    /// </summary>
    public class LongProgressUpdateEachCD
    {
        private int isusing = 0;
        private long cd;
        private long nextUpdateTime = 0;
        public LongProgressUpdateEachCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:LongProgressUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public LongProgressUpdateEachCD(long cd)
        {
            if (cd <= 1) Debugger.Output("Bug:LongProgressUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
        }
        public LongProgressUpdateEachCD(long cd, long startTime)
        {
            if (cd <= 1) Debugger.Output("Bug:LongProgressUpdateEachCD.cd (" + cd.ToString() + ") is less than 1.");
            this.cd = cd;
            this.nextUpdateTime = startTime;
        }

        public long GetRemainingTime()
        {
            long v = Interlocked.Read(ref nextUpdateTime) - Environment.TickCount64;
            return v < 0 ? 0 : v;
        }
        public long GetCD() => Interlocked.Read(ref cd);

        public bool TryUse()
        {
            if (Interlocked.Exchange(ref isusing, 1) == 1) return false;
            long needTime = Interlocked.Read(ref nextUpdateTime);
            if (needTime <= Environment.TickCount64)
            {
                Interlocked.Exchange(ref nextUpdateTime, Environment.TickCount64 + Interlocked.Read(ref cd));
                Interlocked.Exchange(ref isusing, 0);
                return true;
            }
            Interlocked.Exchange(ref isusing, 0);
            return false;
        }
        public void SetCD(int cd)
        {
            if (cd <= 1) Debugger.Output("Bug:Set LongProgressUpdateEachCD.cd to " + cd.ToString() + ".");
            Interlocked.Exchange(ref this.cd, cd);
        }
    }

    /// <summary>
    /// 一个保证在[0,maxNum],每CDms自动+1的int，支持可变的CD、maxNum(请确保大于0)
    /// </summary>
    public class IntNumUpdateEachCD
    {
        private int num;
        private int maxNum;
        private int cd;
        private long updateTime = 0;
        private readonly object numLock = new();
        public IntNumUpdateEachCD(int num, int maxNum, int cd)
        {
            if (num < 0) Debugger.Output("Bug:IntNumUpdateEachCD.num (" + num.ToString() + ") is less than 0.");
            if (maxNum < 0) Debugger.Output("Bug:IntNumUpdateEachCD.maxNum (" + maxNum.ToString() + ") is less than 0.");
            if (cd <= 0) Debugger.Output("Bug:IntNumUpdateEachCD.cd (" + cd.ToString() + ") is less than 0.");
            this.num = (num < maxNum) ? num : maxNum;
            this.maxNum = maxNum;
            this.cd = cd;
            this.updateTime = Environment.TickCount64;
        }
        /// <summary>
        /// 默认使num=maxNum
        /// </summary>
        public IntNumUpdateEachCD(int maxNum, int cd)
        {
            if (maxNum < 0) Debugger.Output("Bug:IntNumUpdateEachCD.maxNum (" + maxNum.ToString() + ") is less than 0.");
            if (cd <= 0) Debugger.Output("Bug:IntNumUpdateEachCD.cd (" + cd.ToString() + ") is less than 0.");
            this.num = this.maxNum = maxNum;
            this.cd = cd;
        }
        public IntNumUpdateEachCD()
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
        public static implicit operator int(IntNumUpdateEachCD aint) => aint.GetNum(Environment.TickCount64);

        /// <summary>
        /// 应当保证该subV>=0
        /// </summary>
        public int TrySub(int subV)
        {
            if (subV < 0) Debugger.Output("Bug:IntNumUpdateEachCD Try to sub " + subV.ToString() + ", which is less than 0.");
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
            if (addV < 0) Debugger.Output("Bug:IntNumUpdateEachCD Try to add " + addV.ToString() + ", which is less than 0.");
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
                if (cd <= 0) Debugger.Output("Bug:Set IntNumUpdateEachCD.cd to " + cd.ToString() + ".");
                this.cd = cd;
            }
        }
    }
}