using System.Collections.Generic;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using Preparation.GameData;
using System;

namespace GameClass.GameObj
{
    public partial class Map : IMap
    {

        private readonly Dictionary<uint, BirthPoint> birthPointList;  // 出生点列表
        public Dictionary<uint, BirthPoint> BirthPointList => birthPointList;

        private Dictionary<GameObjIdx, IList<IGameObj>> gameObjDict;
        public Dictionary<GameObjIdx, IList<IGameObj>> GameObjDict => gameObjDict;
        private Dictionary<GameObjIdx, ReaderWriterLockSlim> gameObjLockDict;
        public Dictionary<GameObjIdx, ReaderWriterLockSlim> GameObjLockDict => gameObjLockDict;

        public readonly uint[,] ProtoGameMap;
        public PlaceType GetPlaceType(GameObj obj)
        {
            try
            {
                uint type = ProtoGameMap[obj.Position.x / GameData.numOfPosGridPerCell, obj.Position.y / GameData.numOfPosGridPerCell];
                if (type == 2)
                    return PlaceType.Grass1;
                else if (type == 3)
                    return PlaceType.Grass2;
                else if (type == 4)
                    return PlaceType.Grass3;
                else
                    return PlaceType.Land;  // 其他情况均返回land
            }
            catch
            {
                return PlaceType.Land;
            }
        }

        public PlaceType GetPlaceType(XY pos)
        {
            try
            {
                switch (ProtoGameMap[pos.x / GameData.numOfPosGridPerCell, pos.y / GameData.numOfPosGridPerCell])
                {
                    case 2:
                        return PlaceType.Grass1;
                    case 3:
                        return PlaceType.Grass2;
                    case 4:
                        return PlaceType.Grass3;
                    default:
                        return PlaceType.Land;
                }
            }
            catch
            {
                return PlaceType.Land;
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
            gameObjLockDict[GameObjIdx.Player].EnterReadLock();
            try
            {
                foreach (Character person in gameObjDict[GameObjIdx.Player])
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
                gameObjLockDict[GameObjIdx.Player].ExitReadLock();
            }
            return player;
        }
        public Map(uint[,] mapResource)
        {
            gameObjDict = new Dictionary<GameObjIdx, IList<IGameObj>>();
            gameObjLockDict = new Dictionary<GameObjIdx, ReaderWriterLockSlim>();
            foreach (GameObjIdx idx in Enum.GetValues(typeof(GameObjIdx)))
            {
                if (idx != GameObjIdx.None)
                {
                    gameObjDict.Add(idx, new List<IGameObj>());
                    gameObjLockDict.Add(idx, new ReaderWriterLockSlim());
                }
            }

            ProtoGameMap = new uint[mapResource.GetLength(0), mapResource.GetLength(1)];
            Array.Copy(mapResource, ProtoGameMap, mapResource.Length);

            birthPointList = new Dictionary<uint, BirthPoint>(GameData.numOfBirthPoint);

            // 将出生点插入
            for (int i = 0; i < GameData.rows; ++i)
            {
                for (int j = 0; j < GameData.cols; ++j)
                {
                    switch (mapResource[i, j])
                    {
                        case (uint)MapInfoObjType.Wall:
                            {
                                GameObjLockDict[GameObjIdx.Map].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjIdx.Map].Add(new Wall(GameData.GetCellCenterPos(i, j)));
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjIdx.Map].ExitWriteLock();
                                }
                                break;
                            }
                        case (uint)MapInfoObjType.BirthPoint1:
                        case (uint)MapInfoObjType.BirthPoint2:
                        case (uint)MapInfoObjType.BirthPoint3:
                        case (uint)MapInfoObjType.BirthPoint4:
                        case (uint)MapInfoObjType.BirthPoint5:
                            {
                                BirthPoint newBirthPoint = new BirthPoint(GameData.GetCellCenterPos(i, j));
                                birthPointList.Add(MapInfo.BirthPointEnumToIdx((MapInfoObjType)mapResource[i, j]), newBirthPoint);
                                GameObjLockDict[GameObjIdx.Map].EnterWriteLock();
                                try
                                {
                                    GameObjDict[GameObjIdx.Map].Add(newBirthPoint);
                                }
                                finally
                                {
                                    GameObjLockDict[GameObjIdx.Map].ExitWriteLock();
                                }
                                break;
                            }
                    }
                }
            }
        }
    }
}
