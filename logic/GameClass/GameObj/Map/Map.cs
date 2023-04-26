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

        private object lockForNum = new();
        private void WhenStudentNumChange()
        {
            if (numOfDeceasedStudent + numOfEscapedStudent == GameData.numOfStudent)
            {
                Timer.IsGaming = false;
            }

            if (GameData.numOfStudent - NumOfDeceasedStudent - NumOfEscapedStudent == 1)
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
        }
        private uint numOfRepairedGenerators = 0;
        public uint NumOfRepairedGenerators
        {
            get => numOfRepairedGenerators;
            set
            {
                lock (lockForNum)
                {
                    numOfRepairedGenerators = value;
                    if (NumOfRepairedGenerators == GameData.numOfGeneratorRequiredForEmergencyExit)
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
                      if (NumOfRepairedGenerators == GameData.numOfGeneratorRequiredForRepair)
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
            }
        }
        private uint numOfDeceasedStudent = 0;
        public uint NumOfDeceasedStudent
        {
            get => numOfDeceasedStudent;
            set
            {
                lock (lockForNum)
                {
                    numOfDeceasedStudent = value;
                    WhenStudentNumChange();
                }
            }
        }
        private uint numOfEscapedStudent = 0;
        public uint NumOfEscapedStudent
        {
            get => numOfEscapedStudent;
            set
            {
                lock (lockForNum)
                {
                    numOfEscapedStudent = value;
                    WhenStudentNumChange();
                }
            }
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
                        if (person.CharacterType == CharacterType.TechOtaku && person.FindIActiveSkill(ActiveSkillType.UseRobot).IsBeingUsed)
                        {
                            Debugger.Output(person, person.PlayerID.ToString());
                            foreach (Character character in gameObjDict[GameObjType.Character])
                            {
                                Debugger.Output(character, character.PlayerID.ToString());
                                if (person.PlayerID + GameData.numOfPeople == character.PlayerID)
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
            }
            finally
            {
                GameObjLockDict[gameObj.Type].ExitWriteLock();
            }
            if (ToDel == null) return false;
            GameObjDict[gameObj.Type].Remove(ToDel);
            return true;
        }
        public bool RemoveJustFromMap(GameObj gameObj)
        {
            GameObjLockDict[gameObj.Type].EnterWriteLock();
            bool flag;
            try
            {
                flag = GameObjDict[gameObj.Type].Remove(gameObj);
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
        public Student? StudentForInteract(XY Pos)
        {
            Student? GameObjForInteract = null;
            GameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character character in GameObjDict[GameObjType.Character])
                {
                    if (!character.IsGhost() && GameData.ApproachToInteract(character.Position, Pos))
                    {
                        GameObjForInteract = (Student)character;
                        break;
                    }
                }
            }
            finally
            {
                GameObjLockDict[GameObjType.Character].ExitReadLock();
            }
            return GameObjForInteract;
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
