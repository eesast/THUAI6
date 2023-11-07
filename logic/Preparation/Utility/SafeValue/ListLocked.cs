using System;
using System.Collections.Generic;
using System.Threading;

namespace Preparation.Utility
{
    public class LockedClassList<T> where T : class
    {
        private readonly ReaderWriterLockSlim listLock = new();
        private List<T> list;

        #region 构造
        public LockedList()
        {
            list = new List<T>();
        }
        public LockedList(int capacity)
        {
            list = new List<T>(capacity);
        }
        public LockedList(IEnumerable<T> collection)
        {
            list = new List<T>(collection);
        }
        #endregion

        #region 修改
        public TResult WriteLock<TResult>(Func<TResult> func)
        {
            listLock.EnterWriteLock();
            try
            {
                return func();
            }
            finally
            {
                listLock.ExitWriteLock();
            }

        }
        public void WriteLock(Action func)
        {
            listLock.EnterWriteLock();
            try
            {
                func();
            }
            finally
            {
                listLock.ExitWriteLock();
            }

        }

        public void Add(T item)
        {
            WriteLock(() => { list.Add(item); });
        }

        public void Insert(int index, T item)
        {
            WriteLock(() => { list.Insert(index, item); });
        }

        public void Clear()
        {
            WriteLock(() => { list.Clear(); });
        }

        public bool Remove(T item)
        {
            return WriteLock<bool>(() => { return list.Remove(item); });
        }

        public int RemoveAll(T item) => WriteLock(() => { return list.RemoveAll((t) => { return t == item; }); });

        public bool RemoveOne(Predicate<T> match) =>
            WriteLock(() =>
            {
                int index = list.FindIndex(match);
                if (index == -1) return false;
                list.RemoveAt(index);
                return true;
            });

        public int RemoveAll(Predicate<T> match) => WriteLock(() => { return list.RemoveAll(match); });

        public bool RemoveAt(int index)
        {
            return WriteLock(() =>
            {
                if (index > list.Count) return false;
                list.RemoveAt(index);
                return true;
            });
        }
        #endregion

        #region 读取与对类操作
        public TResult ReadLock<TResult>(Func<TResult> func)
        {
            listLock.EnterReadLock();
            try
            {
                return func();
            }
            finally
            {
                listLock.ExitReadLock();
            }
        }
        public void ReadLock(Action func)
        {
            listLock.EnterReadLock();
            try
            {
                func();
            }
            finally
            {
                listLock.ExitReadLock();
            }

        }

        public T this[int index]
        {
            get
            {
                return ReadLock<T>(() => { return list[index]; });
            }
            set
            {
                ReadLock(() => { list[index] = value!; });
            }
        }
        public int Count => ReadLock(() => { return list.Count; });

        public int IndexOf(T item)
        {
            return ReadLock(() => { return list.IndexOf(item); });
        }

        public bool Contains(T item)
        {
            return ReadLock(() => { return list.Contains(item); });
        }

        public T? Find(Predicate<T> match)
        {
            return ReadLock(() => { return list.Find(match); });
        }

        public List<T> FindAll(Predicate<T> match)
        {
            return ReadLock(() => { return list.FindAll(match); });
        }

        public int FindIndex(Predicate<T> match) => ReadLock(() => { return list.FindIndex(match); });

        public void ForEach(Action<T> action) => ReadLock(() => { list.ForEach(action); });
        #endregion
    }
}