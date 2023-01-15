using System;
using System.Threading;
using System.Collections.Generic;
using GameClass.GameObj;
using Preparation.Utility;
using Preparation.GameData;
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
            public ActiveSkillType commonSkill;
            public PlayerInitInfo(uint birthPointIndex, long teamID, long playerID, CharacterType characterType, ActiveSkillType commonSkill)
            {
                this.birthPointIndex = birthPointIndex;
                this.teamID = teamID;
                this.characterType = characterType;
                this.commonSkill = commonSkill;
                this.playerID = playerID;
            }
        }

        private readonly List<Team> teamList;
        public List<Team> TeamList => teamList;
        private readonly Map gameMap;
        public Map GameMap => gameMap;
        private readonly int numOfTeam;
        public long AddPlayer(PlayerInitInfo playerInitInfo)
        {
            if (!Team.teamExists(playerInitInfo.teamID))
                /*  || !MapInfo.ValidBirthPointIdx(playerInitInfo.birthPointIdx)
                  || gameMap.BirthPointList[playerInitInfo.birthPointIdx].Parent != null)*/
                return GameObj.invalidID;

            XY pos = gameMap.BirthPointList[playerInitInfo.birthPointIndex].Position;
            // Console.WriteLine($"x,y: {pos.x},{pos.y}");
            Character newPlayer = new(pos, GameData.characterRadius, gameMap.GetPlaceType(pos), playerInitInfo.characterType, playerInitInfo.commonSkill);
            gameMap.BirthPointList[playerInitInfo.birthPointIndex].Parent = newPlayer;
            gameMap.GameObjLockDict[GameObjIdx.Player].EnterWriteLock();
            try
            {
                gameMap.GameObjDict[GameObjIdx.Player].Add(newPlayer);
            }
            finally
            {
                gameMap.GameObjLockDict[GameObjIdx.Player].ExitWriteLock();
            }
            // Console.WriteLine($"GameObjDict[GameObjIdx.Player] length:{gameMap.GameObjDict[GameObjIdx.Player].Count}");
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
            ) {
                AllowTimeExceed = true
                /*MaxTolerantTimeExceedCount = 5,
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
            gameMap.GameObjLockDict[GameObjIdx.Player].EnterReadLock();
            try
            {
            foreach (Character player in gameMap.GameObjDict[GameObjIdx.Player])
            {
                player.CanMove = true;

                player.AddShield(GameData.shieldTimeAtBirth);
            }
            }
            finally
            {
            gameMap.GameObjLockDict[GameObjIdx.Player].ExitReadLock();
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
                if (kvp.Key == GameObjIdx.Bullet || kvp.Key == GameObjIdx.Player || kvp.Key == GameObjIdx.Prop)
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
            gameMap.GameObjLockDict[GameObjIdx.Player].EnterWriteLock();
            /*try
            {
            }
            finally
            {
            }*/
            gameMap.GameObjLockDict[GameObjIdx.Player].ExitWriteLock();
}
public void MovePlayer(long playerID, int moveTimeInMilliseconds, double angle)
{
            if (!gameMap.Timer.IsGaming)
            return;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
            moveManager.MovePlayer(player, moveTimeInMilliseconds, angle);
#if DEBUG
            Console.WriteLine($"PlayerID:{playerID} move to ({player.Position.x},{player.Position.y})!");
#endif
            }
            else
            {
#if DEBUG
            Console.WriteLine($"PlayerID:{playerID} player does not exists!");
#endif
            }
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

public bool UseCommonSkill(long playerID)
{
            if (!gameMap.Timer.IsGaming)
            return false;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
            return skillManager.UseCommonSkill(player);
            }
            else
            return false;
}

public void AllPlayerUsePassiveSkill()
{
            if (!gameMap.Timer.IsGaming)
            return;
            gameMap.GameObjLockDict[GameObjIdx.Player].EnterWriteLock();
            try
            {
            foreach (Character player in gameMap.GameObjDict[GameObjIdx.Player])
            {
            skillManager.UsePassiveSkill(player);
            }
            }
            finally
            {
            gameMap.GameObjLockDict[GameObjIdx.Player].ExitWriteLock();
            }
}

public void ClearLists(GameObjIdx[] objIdxes)
{
            foreach (var idx in objIdxes)
            {
            if (idx != GameObjIdx.None)
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
            if (keyValuePair.Key != GameObjIdx.Map)
            {
            gameMap.GameObjLockDict[keyValuePair.Key].EnterWriteLock();
            try
            {
                if (keyValuePair.Key == GameObjIdx.Player)
                {
                    foreach (Character player in gameMap.GameObjDict[GameObjIdx.Player])
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
            if (keyValuePair.Key != GameObjIdx.Map)
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
            this.numOfTeam = numOfTeam;
            teamList = new List<Team>();
            for (int i = 0; i < numOfTeam; ++i)
            {
            teamList.Add(new Team());
            }

            skillManager = new SkillManager();
            attackManager = new AttackManager(gameMap);
            moveManager = new MoveManager(gameMap);
            propManager = new PropManager(gameMap);
}
}
}
