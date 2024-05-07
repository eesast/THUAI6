using System;
using System.Collections.Generic;
using System.Linq;
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
            /// <summary>
            /// buff列表
            /// </summary>
            private readonly LinkedList<double>[] buffList;
            private readonly object[] buffListLock;

            private void AddBuff(double bf, int buffTime, BuffType buffType, Action ReCalculateFunc)
            {
                LinkedListNode<double> buffNode;
                lock (buffListLock[(int)buffType])
                {
                    buffNode = buffList[(int)buffType].AddLast(bf);
                }
                ReCalculateFunc();

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

            public int ReCalculateFloatBuff(BuffType buffType, int orgVal, int maxVal, int minVal)
            {
                double times = 1.0;
                lock (buffListLock[(int)buffType])
                {
                    foreach (var add in buffList[(int)buffType])
                    {
                        times *= add;
                    }
                }
                return Math.Max(Math.Min((int)Math.Round(orgVal * times), maxVal), minVal);
            }

            public void AddMoveSpeed(double add, int buffTime, Action<int> SetNewMoveSpeed, int orgMoveSpeed) => AddBuff(add, buffTime, BuffType.AddSpeed, () => SetNewMoveSpeed(ReCalculateFloatBuff(BuffType.AddSpeed, orgMoveSpeed, GameData.MaxSpeed, GameData.MinSpeed)));
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

            public void AddShield(int shieldTime) => AddBuff(0, shieldTime, BuffType.Shield, () =>
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
            public bool TryUseShield()
            {
                if (HasShield)
                {
                    lock (buffListLock[(int)BuffType.Shield])
                    {
                        buffList[(int)BuffType.Shield].RemoveFirst();
                    }
                    return true;
                }
                return false;
            }

            public void AddAp(int time) => AddBuff(0, time, BuffType.AddAp, () => { });
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

            public void AddLife(int totelTime) => AddBuff(0, totelTime, BuffType.AddLife, () =>
                                                                                                        { });
            public bool HasLIFE
            {
                get
                {
                    lock (buffListLock[(int)BuffType.AddLife])
                    {
                        return buffList[(int)BuffType.AddLife].Count != 0;
                    }
                }
            }
            public bool TryActivatingLIFE()
            {
                if (HasLIFE)
                {
                    lock (buffListLock[(int)BuffType.AddLife])
                    {
                        buffList[(int)BuffType.AddLife].RemoveFirst();
                    }
                    return true;
                }
                return false;
            }

            public void AddSpear(int spearTime) => AddBuff(0, spearTime, BuffType.Spear, () =>
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
            public bool TryUseSpear()
            {
                if (HasSpear)
                {
                    lock (buffListLock[(int)BuffType.Spear])
                    {
                        buffList[(int)BuffType.Spear].RemoveFirst();
                    }
                    return true;
                }
                return false;
            }

            public void AddClairaudience(int shieldTime) => AddBuff(0, shieldTime, BuffType.Clairaudience, () =>
            { });
            public bool HasClairaudience
            {
                get
                {
                    lock (buffListLock[(int)BuffType.Clairaudience])
                    {
                        return buffList[(int)BuffType.Clairaudience].Count != 0;
                    }
                }
            }

            public void AddInvisible(int shieldTime) => AddBuff(0, shieldTime, BuffType.Invisible, () =>
            { });
            public bool HasInvisible
            {
                get
                {
                    lock (buffListLock[(int)BuffType.Invisible])
                    {
                        return buffList[(int)BuffType.Invisible].Count != 0;
                    }
                }
            }
            public bool TryDeleteInvisible()
            {
                if (HasInvisible)
                {
                    lock (buffListLock[(int)BuffType.Invisible])
                    {
                        buffList[(int)BuffType.Invisible].RemoveFirst();
                    }
                    return true;
                }
                return false;
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
                buffList = new LinkedList<double>[buffTypeArray.Length];
                buffListLock = new object[buffList.Length];
                int i = 0;
                foreach (BuffType type in buffTypeArray)
                {
                    buffList[i] = new LinkedList<double>();
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
