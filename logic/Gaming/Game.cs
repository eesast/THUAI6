﻿using System;
using System.Threading;
using System.Collections.Generic;
using Preparation.Utility;
using Preparation.Interface;
using GameClass.GameObj;

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
            Character? newPlayer = characterManager.AddPlayer(pos, playerInitInfo.teamID, playerInitInfo.playerID, playerInitInfo.characterType);
            if (newPlayer == null) return GameObj.invalidID;
            // Console.WriteLine($"x,y: {pos.x},{pos.y}");

            // Console.WriteLine($"GameObjDict[GameObjType.Character] length:{gameMap.GameObjDict[GameObjType.Character].Count}");
            teamList[(int)playerInitInfo.teamID].AddPlayer(newPlayer);
            return newPlayer.ID;
        }
        public bool StartGame(int milliSeconds)
        {
            if (gameMap.Timer.IsGaming)
                return false;

            propManager.StartProducing();

            // 开始游戏
            new Thread
            (
                () =>
                {
                    if (!gameMap.Timer.StartGame(milliSeconds))
                        return;

                    EndGame();  // 游戏结束时要做的事
                }
            )
            { IsBackground = true }.Start();

            return true;
        }

        public void EndGame()
        {
        }
        public bool MovePlayer(long playerID, int moveTimeInMilliseconds, double angle)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return actionManager.MovePlayer(player, moveTimeInMilliseconds, angle);
            }
            else
            {
#if DEBUG
                Console.WriteLine($"PlayerID:{playerID} player does not exists!");
#endif
                return false;

            }
        }
        public bool Treat(long playerID, long playerTreatedID = -1)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayerToAction(playerID);
            if (playerTreatedID == -1)
            {
                if (player != null && !player.IsGhost())
                    return actionManager.Treat((Student)player);
            }
            else
            {
                ICharacter? playerTreated = gameMap.FindPlayer(playerTreatedID);
                if (player != null && playerTreated != null)
                {
                    if (!playerTreated.IsGhost() && !player.IsGhost())
                        return actionManager.Treat((Student)player, (Student)playerTreated);
                }
            }
            return false;
        }
        public bool Rescue(long playerID, long playerRescuedID = -1)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayerToAction(playerID);
            if (playerRescuedID == -1)
            {
                if (player != null && !player.IsGhost())
                    return actionManager.Rescue((Student)player);
            }
            else
            {
                ICharacter? playerRescued = gameMap.FindPlayer(playerRescuedID);
                if (player != null && playerRescued != null)
                {
                    if (!playerRescued.IsGhost() && !player.IsGhost())
                        return actionManager.Rescue((Student)player, (Student)playerRescued);
                }
            }

            return false;
        }
        public bool Fix(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayerToAction(playerID);
            if (player != null && !player.IsGhost())
                return actionManager.Fix((Student)player);
            return false;
        }
        public bool Escape(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            ICharacter? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                if (!player.IsGhost())
                    return actionManager.Escape((Student)player);
            }
            return false;
        }
        public bool Stop(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return ActionManager.Stop(player);
            }
            return false;
        }
        public bool OpenDoorway(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null && !player.IsGhost())
            {
                return actionManager.OpenDoorway((Student)player);
            }
            return false;
        }
        public bool OpenChest(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return actionManager.OpenChest(player);
            }
            return false;
        }
        public bool ClimbingThroughWindow(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return actionManager.ClimbingThroughWindow(player);
            }
            return false;
        }
        public bool LockDoor(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return actionManager.LockDoor(player);
            }
            return false;
        }
        public bool OpenDoor(long playerID)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return actionManager.OpenDoor(player);
            }
            return false;
        }
        public bool Attack(long playerID, double angle)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return attackManager.Attack(player, angle);
            }
            return false;
        }
        public void UseProp(long playerID, PropType propType = PropType.Null)
        {
            if (!gameMap.Timer.IsGaming)
                return;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                propManager.UseProp(player, propType);
            }
        }
        public void ThrowProp(long playerID, PropType propType = PropType.Null)
        {
            if (!gameMap.Timer.IsGaming)
                return;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                propManager.ThrowProp(player, propType);
            }
        }
        public bool PickProp(long playerID, PropType propType = PropType.Null)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayerToAction(playerID);
            if (player != null)
            {
                return propManager.PickProp(player, propType);
            }
            return false;
        }

        public bool UseActiveSkill(long playerID, int skillNum, int parameter)
        {
            if (!gameMap.Timer.IsGaming)
                return false;
            Character? player = gameMap.FindPlayer(playerID);
            if (player != null)
            {
                if (player.Occupation.ListOfIActiveSkill.Count <= skillNum) return false;
                return skillManager.UseActiveSkill(player, player.Occupation.ListOfIActiveSkill[skillNum], parameter);
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

        /*public void ClearLists(GameObjType[] objIdxes)
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
        }*/
        public void ClearAllLists()
        {
            foreach (var keyValuePair in gameMap.GameObjDict)
            {
                if (!GameData.NeedCopy(keyValuePair.Key))
                {
                    gameMap.GameObjLockDict[keyValuePair.Key].EnterWriteLock();
                    try
                    {
                        if (keyValuePair.Key == GameObjType.Character)
                        {
                            foreach (Character player in gameMap.GameObjDict[GameObjType.Character])
                            {
                                player.ReSetCanMove(false);
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
                if (GameData.NeedCopy(keyValuePair.Key))
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

            characterManager = new CharacterManager(gameMap);
            attackManager = new AttackManager(gameMap, characterManager);
            actionManager = new ActionManager(gameMap, characterManager);
            propManager = new PropManager(gameMap, characterManager);
            skillManager = new SkillManager(gameMap, actionManager, attackManager, propManager, characterManager);
        }
    }
}
