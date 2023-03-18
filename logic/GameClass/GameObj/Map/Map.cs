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

        public readonly uint[,] protoGameMap;
        public uint[,] ProtoGameMap => protoGameMap;
        public PlaceType GetPlaceType(IGameObj obj)
        {
            try
            {
                return (PlaceType)protoGameMap[obj.Position.x / GameData.numOfPosGridPerCell, obj.Position.y / GameData.numOfPosGridPerCell];
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
                return (PlaceType)protoGameMap[pos.x / GameData.numOfPosGridPerCell, pos.y / GameData.numOfPosGridPerCell];
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
        public bool RemoveJustFromMap(GameObj gameObj)
        {
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            try
            {
                return GameObjDict[gameObj.Type].Remove(gameObj);
            }
            finally
            {
                GameObjLockDict[gameObj.Type].ExitWriteLock();
            }
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

            protoGameMap = new uint[mapResource.GetLength(0), mapResource.GetLength(1)];
            Array.Copy(mapResource, protoGameMap, mapResource.Length);

            birthPointList = new Dictionary<uint, XY>(GameData.numOfBirthPoint);

            for (int i = 0; i < GameData.rows; ++i)
            {
                for (int j = 0; j < GameData.cols; ++j)
                {
                    switch (mapResource[i, j])
                    {
                        case (uint)PlaceType.Wall:
                            {
                                Add(new Wall(GameData.GetCellCenterPos(i, j)));
                                break;
                            }
                        case (uint)PlaceType.Doorway:
                            {
                                Add(new Doorway(GameData.GetCellCenterPos(i, j)));
                                break;
                            }
                        case (uint)PlaceType.EmergencyExit:
                            {
                                Add(new EmergencyExit(GameData.GetCellCenterPos(i, j)));
                                break;
                            }
                        case (uint)PlaceType.Generator:
                            {
                                Add(new Generator(GameData.GetCellCenterPos(i, j)));
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
