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

        private uint numOfRepairedGenerators = 0;
        public uint NumOfRepairedGenerators
        {
            get => Interlocked.CompareExchange(ref numOfRepairedGenerators, 0, 0);
        }
        public void AddNumOfRepairedGenerators()
        {
            uint value = Interlocked.Increment(ref numOfRepairedGenerators);
            if (value == GameData.numOfGeneratorRequiredForEmergencyExit)
            {
                GameObjLockDict[GameObjType.EmergencyExit].EnterWriteLock();
                try
                {
                    Random r = new Random(Environment.TickCount);
                    EmergencyExit emergencyExit = (EmergencyExit)(GameObjDict[GameObjType.EmergencyExit][r.Next(0, GameObjDict[GameObjType.EmergencyExit].Count)]);
                    emergencyExit.CanOpen = true;
                    Preparation.Utility.Debugger.Output(emergencyExit, emergencyExit.Position.ToString());
                }
                finally
                {
                    GameObjLockDict[GameObjType.EmergencyExit].ExitWriteLock();
                }
            }
            else
                  if (value == GameData.numOfGeneratorRequiredForRepair)
            {
                GameObjLockDict[GameObjType.Doorway].EnterWriteLock();
                try
                {
                    foreach (Doorway doorway in GameObjDict[GameObjType.Doorway])
                        doorway.PowerSupply = true;
                }
                finally
                {
                    GameObjLockDict[GameObjType.Doorway].ExitWriteLock();
                }
            }
        }

        private uint numOfDeceasedStudent = 0;
        public uint NumOfDeceasedStudent
        {
            get => Interlocked.CompareExchange(ref numOfDeceasedStudent, 0, 0);
        }
        private uint numOfEscapedStudent = 0;
        public uint NumOfEscapedStudent
        {
            get => Interlocked.CompareExchange(ref numOfEscapedStudent, 0, 0);
        }
        private uint numOfNoHpStudent = 0;
        public uint NumOfNoHpStudent
        {
            get => Interlocked.CompareExchange(ref numOfNoHpStudent, 0, 0);
        }
        private uint numOfRemovedStudent = 0;
        public uint NumOfRemovedStudent
        {
            get => Interlocked.CompareExchange(ref numOfRemovedStudent, 0, 0);
        }

        public void MapEscapeStudent()
        {
            if (Interlocked.Increment(ref numOfRemovedStudent) == GameData.numOfStudent - 1)
                OpenEmergencyExit();
            if (Interlocked.Increment(ref numOfNoHpStudent) == GameData.numOfStudent)
            {
                AddScoreFromAddict();
                Timer.IsGaming = false;
                return;
            }
            Interlocked.Increment(ref numOfEscapedStudent);
        }
        public void MapDieStudent()
        {
            if (Interlocked.Increment(ref numOfRemovedStudent) == GameData.numOfStudent - 1)
                OpenEmergencyExit();
            uint noHp = Interlocked.Increment(ref numOfNoHpStudent);
            ghost!.AddScore(GameData.TrickerScoreStudentDie);
            if (noHp == GameData.numOfStudent)
            {
                AddScoreFromAddict();
                Timer.IsGaming = false;
                return;
            }
            Interlocked.Increment(ref numOfDeceasedStudent);
        }
        public void MapAddictStudent()
        {
            if (Interlocked.Increment(ref numOfNoHpStudent) == GameData.numOfStudent)
            {
                AddScoreFromAddict();
                Timer.IsGaming = false;
            }
        }
        public void MapRescueStudent()
        {
            if (Timer.IsGaming)
                Interlocked.Decrement(ref numOfNoHpStudent);
        }

        private void OpenEmergencyExit()
        {
            GameObjLockDict[GameObjType.EmergencyExit].EnterReadLock();
            try
            {
                foreach (EmergencyExit emergencyExit in GameObjDict[GameObjType.EmergencyExit])
                    if (emergencyExit.CanOpen)
                    {
                        emergencyExit.IsOpen = true;
                        break;
                    }
            }
            finally
            {
                GameObjLockDict[GameObjType.EmergencyExit].ExitReadLock();
            }
        }
        private void AddScoreFromAddict()
        {
            ghost!.AddScore(GameData.TrickerScoreStudentDie * (GameData.numOfStudent - NumOfRemovedStudent));
        }


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
        public Ghost? ghost = null;
        public Character? FindPlayerToAction(long playerID)
        {
            Character? player = null;
            gameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character person in gameObjDict[GameObjType.Character])
                {
                    if (playerID == person.ID)
                    {
                        if (person.CharacterType == CharacterType.TechOtaku)
                        {
                            Debugger.Output(person, person.PlayerID.ToString());
                            foreach (Character character in gameObjDict[GameObjType.Character])
                            {
                                if (((UseRobot)person.FindActiveSkill(ActiveSkillType.UseRobot)).NowPlayerID == character.PlayerID)
                                {
                                    player = character;
                                    break;
                                }
                            }
                        }
                        else player = person;
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

        public GameObj? OneForInteract(XY Pos, GameObjType gameObjType)
        {
            GameObj? GameObjForInteract = null;
            GameObjLockDict[gameObjType].EnterReadLock();
            try
            {
                foreach (GameObj gameObj in GameObjDict[gameObjType])
                {
                    if (GameData.ApproachToInteract(gameObj.Position, Pos))
                    {
                        GameObjForInteract = gameObj;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[gameObjType].ExitReadLock();
            }
            return GameObjForInteract;
        }
        public Student? StudentForInteract(Student AStudent)
        {
            GameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character character in GameObjDict[GameObjType.Character])
                {
                    if (!character.IsGhost() && character != AStudent && GameData.ApproachToInteract(character.Position, AStudent.Position))
                    {
                        return (Student)character;
                    }
                }
            }
            finally
            {
                GameObjLockDict[GameObjType.Character].ExitReadLock();
            }
            return null;
        }
        public GameObj? OneInTheSameCell(XY Pos, GameObjType gameObjType)
        {
            GameObj? GameObjForInteract = null;
            GameObjLockDict[gameObjType].EnterReadLock();
            try
            {
                foreach (GameObj gameObj in GameObjDict[gameObjType])
                {
                    if (GameData.IsInTheSameCell(gameObj.Position, Pos))
                    {
                        GameObjForInteract = gameObj;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[gameObjType].ExitReadLock();
            }
            return GameObjForInteract;
        }
        public GameObj? PartInTheSameCell(XY Pos, GameObjType gameObjType)
        {
            GameObj? GameObjForInteract = null;
            GameObjLockDict[gameObjType].EnterReadLock();
            try
            {
                foreach (GameObj gameObj in GameObjDict[gameObjType])
                {
                    if (GameData.PartInTheSameCell(gameObj.Position, Pos))
                    {
                        GameObjForInteract = gameObj;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[gameObjType].ExitReadLock();
            }
            return GameObjForInteract;
        }
        public GameObj? OneForInteractInACross(XY Pos, GameObjType gameObjType)
        {
            GameObj? GameObjForInteract = null;
            GameObjLockDict[gameObjType].EnterReadLock();
            try
            {
                foreach (GameObj gameObj in GameObjDict[gameObjType])
                {
                    if (GameData.ApproachToInteractInACross(gameObj.Position, Pos))
                    {
                        GameObjForInteract = gameObj;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[gameObjType].ExitReadLock();
            }
            return GameObjForInteract;
        }

        public bool CanSee(Character player, GameObj gameObj)
        {
            if ((gameObj.Type == GameObjType.Character) && ((Character)gameObj).HasInvisible) return false;
            XY pos1 = player.Position;
            XY pos2 = gameObj.Position;
            XY del = pos1 - pos2;
            if (del * del > player.ViewRange * player.ViewRange) return false;
            if (del.x > del.y)
            {
                if (GetPlaceType(pos1) == PlaceType.Grass && GetPlaceType(pos2) == PlaceType.Grass)
                {
                    for (int x = GameData.PosGridToCellX(pos1) + GameData.numOfPosGridPerCell; x < GameData.PosGridToCellX(pos2); x += GameData.numOfPosGridPerCell)
                    {
                        if (GetPlaceType(pos1 + del * (x / del.x)) != PlaceType.Grass)
                            return false;
                    }
                }
                else
                {
                    for (int x = GameData.PosGridToCellX(pos1) + GameData.numOfPosGridPerCell; x < GameData.PosGridToCellX(pos2); x += GameData.numOfPosGridPerCell)
                    {
                        if (GetPlaceType(pos1 + del * (x / del.x)) == PlaceType.Wall)
                            return false;
                    }
                }
            }
            else
            {
                if (GetPlaceType(pos1) == PlaceType.Grass && GetPlaceType(pos2) == PlaceType.Grass)
                {
                    for (int y = GameData.PosGridToCellY(pos1) + GameData.numOfPosGridPerCell; y < GameData.PosGridToCellY(pos2); y += GameData.numOfPosGridPerCell)
                    {
                        if (GetPlaceType(pos1 + del * (y / del.y)) != PlaceType.Grass)
                            return false;
                    }
                }
                else
                {
                    for (int y = GameData.PosGridToCellY(pos1) + GameData.numOfPosGridPerCell; y < GameData.PosGridToCellY(pos2); y += GameData.numOfPosGridPerCell)
                    {
                        if (GetPlaceType(pos1 + del * (y / del.y)) == PlaceType.Wall)
                            return false;
                    }
                }
            }
            return true;
        }

        public bool Remove(GameObj gameObj)
        {
            GameObj? ToDel = null;
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            try
            {
                foreach (GameObj obj in GameObjDict[gameObj.Type])
                {
                    if (gameObj.ID == obj.ID)
                    {
                        ToDel = obj;
                        break;
                    }
                }
                if (ToDel != null)
                {
                    GameObjDict[gameObj.Type].Remove(ToDel);
                    ToDel.TryToRemove();
                }
            }
            finally
            {
                GameObjLockDict[gameObj.Type].ExitWriteLock();
            }
            return ToDel != null;
        }
        public bool RemoveJustFromMap(GameObj gameObj)
        {
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            try
            {
                if (GameObjDict[gameObj.Type].Remove(gameObj))
                {
                    gameObj.TryToRemove();
                    return true;
                }
                return false;
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
                        case (uint)PlaceType.Chest:
                            {
                                Add(new Chest(GameData.GetCellCenterPos(i, j)));
                                break;
                            }
                        case (uint)PlaceType.Door3:
                            {
                                Add(new Door(GameData.GetCellCenterPos(i, j), PlaceType.Door3));
                                break;
                            }
                        case (uint)PlaceType.Door5:
                            {
                                Add(new Door(GameData.GetCellCenterPos(i, j), PlaceType.Door5));
                                break;
                            }
                        case (uint)PlaceType.Door6:
                            {
                                Add(new Door(GameData.GetCellCenterPos(i, j), PlaceType.Door6));
                                break;
                            }
                        case (uint)PlaceType.Window:
                            {
                                Add(new Window(GameData.GetCellCenterPos(i, j)));
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
