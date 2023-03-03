using System.Collections.Generic;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;

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
                        case (uint)MapInfoObjType.Wall:
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
                        case (uint)MapInfoObjType.Doorway:
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

                        case (uint)MapInfoObjType.EmergencyExit:
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
                        case (uint)MapInfoObjType.Generator:
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
                        case (uint)MapInfoObjType.BirthPoint1:
                        case (uint)MapInfoObjType.BirthPoint2:
                        case (uint)MapInfoObjType.BirthPoint3:
                        case (uint)MapInfoObjType.BirthPoint4:
                        case (uint)MapInfoObjType.BirthPoint5:
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
