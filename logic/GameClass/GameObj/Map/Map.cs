using System.Collections.Generic;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using System.Collections.Concurrent;

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
                Random r = new(Environment.TickCount);
                EmergencyExit emergencyExit = (EmergencyExit)(GameObjDict[GameObjType.EmergencyExit][r.Next(0, GameObjDict[GameObjType.EmergencyExit].Count)]);
                emergencyExit.CanOpen.SetReturnOri(true);
                Preparation.Utility.Debugger.Output(emergencyExit, emergencyExit.Position.ToString());
            }
            else
                  if (value == GameData.numOfGeneratorRequiredForRepair)
            {
                GameObjDict[GameObjType.Doorway].ForEach(delegate (IGameObj doorway)
                {
                    ((Doorway)doorway).PowerSupply.SetReturnOri(true);
                });
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
            EmergencyExit? emergencyExit =
            (EmergencyExit?)GameObjDict[GameObjType.EmergencyExit].Find(gameObj => ((EmergencyExit)gameObj).CanOpen);
            if (emergencyExit != null)
                emergencyExit.IsOpen = true;
        }
        private void AddScoreFromAddict()
        {
            ghost!.AddScore(GameData.TrickerScoreStudentDie * (GameData.numOfStudent - NumOfRemovedStudent));
        }


        private Dictionary<GameObjType, LockedClassList<IGameObj>> gameObjDict;
        public Dictionary<GameObjType, LockedClassList<IGameObj>> GameObjDict => gameObjDict;

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

        public Character? FindPlayerInID(long playerID)
        {
            return (Character?)GameObjDict[GameObjType.Character].Find(gameObj => (playerID == ((Character)gameObj).ID));
        }
        public Character? FindPlayerInPlayerID(long playerID)
        {
            return (Character?)GameObjDict[GameObjType.Character].Find(gameObj => (playerID == ((Character)gameObj).PlayerID));
        }
        public Ghost? ghost = null;
        public Character? FindPlayerToAction(long playerID)
        {

            Character? player = (Character?)GameObjDict[GameObjType.Character].Find(gameObj => (playerID == ((Character)gameObj).ID));
            if (player == null) return null;
            if (player.CharacterType == CharacterType.TechOtaku)
                player = (Character?)GameObjDict[GameObjType.Character].Find(
                    gameObj => (
                                    ((UseRobot)player.FindActiveSkill(ActiveSkillType.UseRobot)).NowPlayerID == ((Character)gameObj).PlayerID
                                    )
                    );
            return player;
        }

        public GameObj? OneForInteract(XY Pos, GameObjType gameObjType)
        {
            return (GameObj?)GameObjDict[gameObjType].Find(gameObj => GameData.ApproachToInteract(gameObj.Position, Pos));
        }
        public Student? StudentForInteract(Student AStudent)
        {
            return (Student?)GameObjDict[GameObjType.Character].Find(gameObj =>
            {
                Character character = (Character)gameObj;
                return !character.IsGhost() && character != AStudent && GameData.ApproachToInteract(character.Position, AStudent.Position);
            });
        }
        public GameObj? OneInTheSameCell(XY Pos, GameObjType gameObjType)
        {
            return (GameObj?)GameObjDict[gameObjType].Find(gameObj =>
                GameData.IsInTheSameCell(gameObj.Position, Pos)
            );
        }
        public GameObj? PartInTheSameCell(XY Pos, GameObjType gameObjType)
        {
            return (GameObj?)GameObjDict[gameObjType].Find(gameObj =>
    GameData.PartInTheSameCell(gameObj.Position, Pos)
);
        }
        public GameObj? OneForInteractInACross(XY Pos, GameObjType gameObjType)
        {
            return (GameObj?)GameObjDict[gameObjType].Find(gameObj =>
                GameData.ApproachToInteractInACross(gameObj.Position, Pos));
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
            if (GameObjDict[gameObj.Type].RemoveOne(obj => gameObj.ID == obj.ID))
            {
                gameObj.TryToRemove();
                return true;
            }
            return false;
        }
        public bool RemoveJustFromMap(GameObj gameObj)
        {
            if (GameObjDict[gameObj.Type].Remove(gameObj))
            {
                gameObj.TryToRemove();
                return true;
            }
            return false;
        }
        public void Add(GameObj gameObj)
        {
            GameObjDict[gameObj.Type].Add(gameObj);
        }

        public Map(uint[,] mapResource)
        {
            gameObjDict = new Dictionary<GameObjType, LockedClassList<IGameObj>>();
            foreach (GameObjType idx in Enum.GetValues(typeof(GameObjType)))
            {
                if (idx != GameObjType.Null)
                {
                    gameObjDict.TryAdd(idx, new LockedClassList<IGameObj>());
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
