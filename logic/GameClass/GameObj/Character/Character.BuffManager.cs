using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using Preparation.Utility;

namespace GameClass.GameObj
{
    public partial class Character
    {
        private readonly BuffManager buffManager;
        /// <summary>
        /// 角色携带的buff管理器
        /// </summary>
        private class BuffManager
        {
            [StructLayout(LayoutKind.Explicit, Size = 8)]
            private struct BuffValue  // buff参数联合体类型，可能是int或double
            {
                [FieldOffset(0)]
                public int iValue;
                [FieldOffset(0)]
                public double lfValue;

                public BuffValue(int intValue)
                {
                    this.lfValue = 0.0;
                    this.iValue = intValue;
                }
                public BuffValue(double longFloatValue)
                {
                    this.iValue = 0;
                    this.lfValue = longFloatValue;
                }
            }

            /// <summary>
            /// buff列表
            /// </summary>
            private readonly LinkedList<BuffValue>[] buffList;
            private readonly object[] buffListLock;

            private void AddBuff(BuffValue bf, int buffTime, BuffType buffType, Action ReCalculateFunc)
            {
                LinkedListNode<BuffValue> buffNode;
                lock (buffListLock[(int)buffType])
                {
                    buffNode = buffList[(int)buffType].AddLast(bf);
                }
                ReCalculateFunc();
                if (buffTime != -1)
                {
                    new Thread
                        (
                            () =>
                            {

                                Thread.Sleep(buffTime);
                                try
                                {
                                    lock (buffListLock[(int)buffType])
                                    {
                                        buffList[(int)buffType].Remove(buffNode);
                                    }
                                }
                                catch
                                {
                                }
                                ReCalculateFunc();

                            }
                        )
                    { IsBackground = true }.Start();
                }
            }

            public int ReCalculateFloatBuff(BuffType buffType, int orgVal, int maxVal, int minVal)
            {
                double times = 1.0;
                lock (buffListLock[(int)buffType])
                {
                    foreach (var add in buffList[(int)buffType])
                    {
                        times *= add.lfValue;
                    }
                }
                return Math.Max(Math.Min((int)Math.Round(orgVal * times), maxVal), minVal);
            }

            public void AddMoveSpeed(double add, int buffTime, Action<int> SetNewMoveSpeed, int orgMoveSpeed) => AddBuff(new BuffValue(add), buffTime, BuffType.AddSpeed, () => SetNewMoveSpeed(ReCalculateFloatBuff(BuffType.AddSpeed, orgMoveSpeed, GameData.MaxSpeed, GameData.MinSpeed)));
            public bool HasFasterSpeed
            {
                get
                {
                    lock (buffListLock[(int)BuffType.AddSpeed])
                    {
                        return buffList[(int)BuffType.AddSpeed].Count != 0;
                    }
                }
            }

            public void AddShield(int shieldTime) => AddBuff(new BuffValue(), shieldTime, BuffType.Shield, () =>
                                                                                                           { });
            public bool HasShield
            {
                get
                {
                    lock (buffListLock[(int)BuffType.Shield])
                    {
                        return buffList[(int)BuffType.Shield].Count != 0;
                    }
                }
            }
            public void AddAp() => AddBuff(new BuffValue(), -1, BuffType.AddAp, () => { });
            public bool HasAp
            {
                get
                {
                    lock (buffListLock[(int)BuffType.AddAp])
                    {
                        return buffList[(int)BuffType.AddAp].Count != 0;
                    }
                }
            }
            public bool TryAddAp()
            {
                if (HasAp)
                {
                    lock (buffListLock[(int)BuffType.AddAp])
                    {
                        buffList[(int)BuffType.AddAp].RemoveFirst();
                    }
                    return true;
                }
                return false;
            }

            public void AddLIFE(int totelTime) => AddBuff(new BuffValue(), totelTime, BuffType.AddLIFE, () =>
                                                                                                        { });
            public bool HasLIFE
            {
                get
                {
                    lock (buffListLock[(int)BuffType.AddLIFE])
                    {
                        return buffList[(int)BuffType.AddLIFE].Count != 0;
                    }
                }
            }
            public bool TryActivatingLIFE()
            {
                if (HasLIFE)
                {
                    lock (buffListLock[(int)BuffType.AddLIFE])
                    {
                        buffList[(int)BuffType.AddLIFE].RemoveFirst();
                    }
                    return true;
                }
                return false;
            }

            public void AddSpear(int spearTime) => AddBuff(new BuffValue(), spearTime, BuffType.Spear, () =>
                                                                                                       { });
            public bool HasSpear
            {
                get
                {
                    lock (buffListLock[(int)BuffType.Spear])
                    {
                        return buffList[(int)BuffType.Spear].Count != 0;
                    }
                }
            }

            /// <summary>
            /// 清除所有buff
            /// </summary>
            public void ClearAll()
            {
                for (int i = 0; i < buffList.Length; ++i)
                {
                    lock (buffListLock[i])
                    {
                        buffList[i].Clear();
                    }
                }
            }

            public BuffManager()
            {
                var buffTypeArray = Enum.GetValues(typeof(BuffType));
                buffList = new LinkedList<BuffValue>[buffTypeArray.Length];
                buffListLock = new object[buffList.Length];
                int i = 0;
                foreach (BuffType type in buffTypeArray)
                {
                    buffList[i] = new LinkedList<BuffValue>();
                    buffListLock[i++] = new object();
                }
            }
        }
        public int ReCalculateBuff(BuffType buffType, int orgVal, int maxVal, int minVal)
        {
            return buffManager.ReCalculateFloatBuff(buffType, orgVal, maxVal, minVal);
        }
    }
}
