using System;
using System.Threading;
using System.Collections.Generic;
using GameClass.GameObj;
using Preparation.Utility;
using Timothy.FrameRateTask;
using Preparation.Interface;

namespace Gaming
{
    public partial class Game
    {
        public struct PlayerInitInfo
        {
            public uint birthPointIndex;
            public long teamID;
            public long playerID;
            public CharacterType characterType;
            public PlayerInitInfo(uint birthPointIndex, long teamID, long playerID, CharacterType characterType)
            {
                this.birthPointIndex = birthPointIndex;
                this.teamID = teamID;
                this.characterType = characterType;
                this.playerID = playerID;
            }
        }

        private readonly List<Team> teamList;
        public List<Team> TeamList => teamList;
        private readonly Map gameMap;
        public Map GameMap => gameMap;
        //       private readonly int numOfTeam;
        public long AddPlayer(PlayerInitInfo playerInitInfo)
        {
            if (!Team.teamExists(playerInitInfo.teamID))
                /*  || !MapInfo.ValidBirthPointIdx(playerInitInfo.birthPointIdx)
                  || gameMap.BirthPointList[playerInitInfo.birthPointIdx].Parent != null)*/
                return GameObj.invalidID;

            XY pos = gameMap.BirthPointList[playerInitInfo.birthPointIndex];
            // Console.WriteLine($"x,y: {pos.x},{pos.y}");
            
            Character newPlayer = (GameData.IsGhost(playerInitInfo.characterType)) ? new Ghost(pos, GameData.characterRadius, gameMap.GetPlaceType(pos), playerInitInfo.characterType) : new Student(pos, GameData.characterRadius, gameMap.GetPlaceType(pos), playerInitInfo.characterType);
            gameMap.GameObjLockDict[GameObjType.Character].EnterWriteLock();
            try
            {
                gameMap.GameObjDict[GameObjType.Character].Add(newPlayer);
            }
            finally
            {
                gameMap.GameObjLockDict[GameObjType.Character].ExitWriteLock();
            }
            // Console.WriteLine($"GameObjDict[GameObjType.Character] length:{gameMap.GameObjDict[GameObjType.Character].Count}");
            teamList[(int)playerInitInfo.teamID].AddPlayer(newPlayer);
            newPlayer.TeamID = playerInitInfo.teamID;
            newPlayer.PlayerID = playerInitInfo.playerID;

            new Thread  //人物装弹
            (
                () =>
                {
                    while (!gameMap.Timer.IsGaming)
                        Thread.Sleep(newPlayer.CD);
                    long lastTime = Environment.TickCount64;
                    new FrameRateTaskExecutor<int>(
                        loopCondition: () => gameMap.Timer.IsGaming,
                        loopToDo: () =>
                        {
                            if (!newPlayer.IsResetting)
                            {
                                long nowTime = Environment.TickCount64;
                                if (newPlayer.BulletNum == newPlayer.MaxBulletNum)
                                    lastTime = nowTime;
                                if (nowTime - lastTime >= newPlayer.CD)
                                {
                                    _ = newPlayer.TryAddBulletNum();
                                    lastTime = nowTime;
                                }
                            }
                        },
                        timeInterval: GameData.checkInterval,
                        finallyReturn: () => 0
                    )
                    {
                        AllowTimeExceed = true/*,
                        MaxTolerantTimeExceedCount = 5,
                        TimeExceedAction = exceedTooMuch =>
                        {
                            if (exceedTooMuch) Console.WriteLine("The computer runs too slow that it cannot check the color below the player in time!");
                        }*/
                    }
                        .Start();
                }
            )
            { IsBackground = true }.Start();

            return newPlayer.ID;
        }
        public bool StartGame(int milliSeconds)
        {
            if (gameMap.Timer.IsGaming)
                return false;
            gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character player in gameMap.GameObjDict[GameObjType.Character])
                {
                    player.CanMove = true;

                    player.AddShield(GameData.shieldTimeAtBirth);
                }
            }
            finally
            {
                gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
            }

            propManager.StartProducing();
            new Thread
            (
                () =>
                {
                    new FrameRateTaskExecutor<int>
                    (
                        loopCondition: () => gameMap.Timer.IsGaming,
                        loopToDo: () =>
                        {
                            foreach (var kvp in gameMap.GameObjDict)  // 检查物体位置
                            {
                                if (kvp.Key == GameObjType.Bullet || kvp.Key == GameObjType.Character || kvp.Key == GameObjType.Prop)
                                {
                                    gameMap.GameObjLockDict[kvp.Key].EnterWriteLock();
                                    try
                                    {
                                        foreach (var item in gameMap.GameObjDict[kvp.Key])
                                        {
                                            item.Place = gameMap.GetPlaceType(item.Position);
                                        }
                                    }
                                    finally
                                    {
                                        gameMap.GameObjLockDict[kvp.Key].ExitWriteLock();
                                    }
                                }
                            }
                        },
                        timeInterval: GameData.checkInterval,
                        finallyReturn: () => 0
                    )
                    {
                        AllowTimeExceed = true
                    }.Start();
                }
            )
            { IsBackground = true }.Start();
            // 开始游戏
            if (!gameMap.Timer.StartGame(milliSeconds))
                return false;

            EndGame();  // 游戏结束时要做的事

            return true;
        }

        public void EndGame()
        {
            gameMap.GameObjLockDict[GameObjType.Character].EnterWriteLock();
            /*try
            {
            }
            finally
            {
            }*/
            gameMap.GameObjLockDict[GameObjType.Character].ExitWriteLock();
        }
        public bool MovePlayer(long playerID, int moveTimeInMilliseconds, double angle)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                return actionManager.MovePlayer(player, moveTimeInMilliseconds, angle);
#if DEBUG
                Console.WriteLine($"PlayerID:{playerID} move to ({player.Position.x},{player.Position.y})!");
#endif
            }
            else
            {
                return false;
#if DEBUG
                Console.WriteLine($"PlayerID:{playerID} player does not exists!");
#endif
            }
        }
        public bool Treat(long playerID, long playerTreatedID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayer(playerID);
            ICharacter? playerTreated = gameMap.FindPlayer(playerTreatedID);
            if (player != null && playerTreated != null)
            {
                if (!playerTreated.IsGhost() && !player.IsGhost())
                    return actionManager.Treat((Student)player, (Student)playerTreated);
            }
            return false;
        }
        public bool Rescue(long playerID, long playerRescuedID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayer(playerID);
            ICharacter? playerRescued = gameMap.FindPlayer(playerRescuedID);
            if (player != null && playerRescued != null)
            {
                if (!playerRescued.IsGhost() && !player.IsGhost())
                    return actionManager.Treat((Student)player, (Student)playerRescued);
            }
            return false;
        }
        public bool Fix(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                if (!player.IsGhost())
                    return actionManager.Fix((Student)player);
            }
            return false;
        }
        public bool Escape(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                if (!player.IsGhost())
                    return actionManager.Escape((Student)player);
            }
            return false;
        }
        public void Attack(long playerID, double angle)
        {
            if (!gameMap.Timer.IsGaming)
                return;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                _ = attackManager.Attack(player, angle);
            }
        }
        public void UseProp(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                propManager.UseProp(player);
            }
        }
        public void ThrowProp(long playerID, int timeInmillionSeconds, double angle)
        {
            if (!gameMap.Timer.IsGaming)
                return;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                propManager.ThrowProp(player, timeInmillionSeconds, angle);
            }
        }
        public bool PickProp(long playerID, PropType propType = PropType.Null)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                return propManager.PickProp(player, propType);
            }
            return false;
        }

        public bool UseActiveSkill(long playerID, ActiveSkillType activeSkillType)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                return skillManager.UseActiveSkill(player, activeSkillType);
            }
            else
                return false;
        }

        public void AllPlayerUsePassiveSkill()
        {
            if (!gameMap.Timer.IsGaming)
                return;
            gameMap.GameObjLockDict[GameObjType.Character].EnterWriteLock();
            try
            {
                foreach (Character player in gameMap.GameObjDict[GameObjType.Character])
                {
                    skillManager.UseAllPassiveSkill(player);
                }
            }
            finally
            {
                gameMap.GameObjLockDict[GameObjType.Character].ExitWriteLock();
            }
        }

        public void ClearLists(GameObjType[] objIdxes)
        {
            foreach (var idx in objIdxes)
            {
                if (idx != GameObjType.Null)
                {
                    gameMap.GameObjLockDict[idx].EnterWriteLock();
                    try
                    {
                        gameMap.GameObjDict[idx].Clear();
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[idx].ExitWriteLock();
                    }
                }
            }
        }
        public void ClearAllLists()
        {
            foreach (var keyValuePair in gameMap.GameObjDict)
            {
                if (!GameData.IsMap(keyValuePair.Key))
                {
                    gameMap.GameObjLockDict[keyValuePair.Key].EnterWriteLock();
                    try
                    {
                        if (keyValuePair.Key == GameObjType.Character)
                        {
                            foreach (Character player in gameMap.GameObjDict[GameObjType.Character])
                            {
                                player.CanMove = false;
                            }
                        }
                        gameMap.GameObjDict[keyValuePair.Key].Clear();
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[keyValuePair.Key].ExitWriteLock();
                    }
                }
            }
        }

        public int GetTeamScore(long teamID)
        {
            return teamList[(int)teamID].Score;
        }
        public List<IGameObj> GetGameObj()
        {
            var gameObjList = new List<IGameObj>();
            foreach (var keyValuePair in gameMap.GameObjDict)
            {
                if (!GameData.IsMap(keyValuePair.Key))
                {
                    gameMap.GameObjLockDict[keyValuePair.Key].EnterReadLock();
                    try
                    {
                        gameObjList.AddRange(gameMap.GameObjDict[keyValuePair.Key]);
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[keyValuePair.Key].ExitReadLock();
                    }
                }
            }
            return gameObjList;
        }
        public Game(uint[,] mapResource, int numOfTeam)
        {
            // if (numOfTeam > maxTeamNum) throw new TeamNumOverFlowException();

            gameMap = new Map(mapResource);

            // 加入队伍
            //    this.numOfTeam = numOfTeam;
            teamList = new List<Team>();
            for (int i = 0; i < numOfTeam; ++i)
            {
                teamList.Add(new Team());
            }

            skillManager = new SkillManager();
            attackManager = new AttackManager(gameMap);
            actionManager = new ActionManager(gameMap);
            propManager = new PropManager(gameMap);
        }
    }
}
