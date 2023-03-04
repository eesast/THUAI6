using System.Collections.Generic;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using GameClass.GameObj;

namespace GameClass.GameObj
{
    public partial class Map : IMap
    {

        private readonly Dictionary<uint, XY> birthPointList;  // 出生点列表
        public Dictionary<uint, XY> BirthPointList => birthPointList;

        private Dictionary<GameObjType, IList<IGameObj>> gameObjDict;
        public Dictionary<GameObjType, IList<IGameObj>> GameObjDict => gameObjDict;
        private Dictionary<GameObjType, ReaderWriterLockSlim> gameObjLockDict;
        public Dictionary<GameObjType, ReaderWriterLockSlim> GameObjLockDict => gameObjLockDict;

        public readonly uint[,] ProtoGameMap;
        public PlaceType GetPlaceType(GameObj obj)
        {
            try
            {
                return (PlaceType)ProtoGameMap[obj.Position.x / GameData.numOfPosGridPerCell, obj.Position.y / GameData.numOfPosGridPerCell];
            }
            catch
            {
                return PlaceType.Null;
            }
        }

        public PlaceType GetPlaceType(XY pos)
        {
            try
            {
                return (PlaceType)ProtoGameMap[pos.x / GameData.numOfPosGridPerCell, pos.y / GameData.numOfPosGridPerCell];
            }
            catch
            {
                return PlaceType.Null;
            }
        }

        public bool IsOutOfBound(IGameObj obj)
        {
            return obj.Position.x >= GameData.lengthOfMap - obj.Radius || obj.Position.x <= obj.Radius || obj.Position.y >= GameData.lengthOfMap - obj.Radius || obj.Position.y <= obj.Radius;
        }
        public IOutOfBound GetOutOfBound(XY pos)
        {
            return new OutOfBoundBlock(pos);
        }

        public Character? FindPlayer(long playerID)
        {
            Character? player = null;
            gameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character person in gameObjDict[GameObjType.Character])
                {
                    if (playerID == person.ID)
                    {
                        player = person;
                        break;
                    }
                }
            }
            finally
            {
                gameObjLockDict[GameObjType.Character].ExitReadLock();
            }
            return player;
        }
        public bool Remove(GameObj gameObj)
        {
            bool flag = false;
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            try
            {
                foreach (GameObj obj in GameObjDict[gameObj.Type])
                {
                    if (gameObj.ID == obj.ID)
                    {
                        GameObjDict[gameObj.Type].Remove(obj);
                        flag = true;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[gameObj.Type].ExitWriteLock();
            }
            return flag;
        }
        public void Add(GameObj gameObj)
        {
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            try
            {
                GameObjDict[gameObj.Type].Add(gameObj);
            }
            finally
            {
                GameObjLockDict[gameObj.Type].ExitWriteLock();
            }
        }
        public Map(uint[,] mapResource)
        {
            gameObjDict = new Dictionary<GameObjType, IList<IGameObj>>();
            gameObjLockDict = new Dictionary<GameObjType, ReaderWriterLockSlim>();
            foreach (GameObjType idx in Enum.GetValues(typeof(GameObjType)))
            {
                if (idx != GameObjType.Null)
                {
                    gameObjDict.Add(idx, new List<IGameObj>());
                    gameObjLockDict.Add(idx, new ReaderWriterLockSlim());
                }
            }

            ProtoGameMap = new uint[mapResource.GetLength(0), mapResource.GetLength(1)];
            Array.Copy(mapResource, ProtoGameMap, mapResource.Length);

            birthPointList = new Dictionary<uint, XY>(GameData.numOfBirthPoint);

            for (int i = 0; i < GameData.rows; ++i)
            {
                for (int j = 0; j < GameData.cols; ++j)
                {
                    switch (mapResource[i, j])
                    {
                        case (uint)PlaceType.Wall:
                            {
                                GameObjLockDict[GameObjType.Wall].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjType.Wall].Add(new Wall(GameData.GetCellCenterPos(i, j)));
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjType.Wall].ExitWriteLock();
                                }
                                break;
                            }
                        case (uint)PlaceType.Doorway:
                            {
                                GameObjLockDict[GameObjType.Doorway].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjType.Doorway].Add(new Doorway(GameData.GetCellCenterPos(i, j)));
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjType.Doorway].ExitWriteLock();
                                }
                                break;
                            }

                        case (uint)PlaceType.EmergencyExit:
                            {
                                GameObjLockDict[GameObjType.EmergencyExit].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjType.EmergencyExit].Add(new EmergencyExit(GameData.GetCellCenterPos(i, j)));
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjType.EmergencyExit].ExitWriteLock();
                                }
                                break;
                            }
                        case (uint)PlaceType.Generator:
                            {
                                GameObjLockDict[GameObjType.Generator].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjType.Generator].Add(new Generator(GameData.GetCellCenterPos(i, j)));
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjType.Generator].ExitWriteLock();
                                }
                                break;
                            }
                        case (uint)PlaceType.BirthPoint1:
                        case (uint)PlaceType.BirthPoint2:
                        case (uint)PlaceType.BirthPoint3:
                        case (uint)PlaceType.BirthPoint4:
                        case (uint)PlaceType.BirthPoint5:
                            {
                                birthPointList.Add(mapResource[i, j], GameData.GetCellCenterPos(i, j));
                                break;
                            }
                    }
                }
            }
        }
    }
}
