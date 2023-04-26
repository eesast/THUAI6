﻿using System;
using System.Threading;
using GameClass.GameObj;
using Preparation.Utility;
using Preparation.Interface;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private readonly CharacterManager characterManager;
        private class CharacterManager
        {
            readonly Map gameMap;
            public CharacterManager(Map gameMap)
            {
                this.gameMap = gameMap;
            }

            public void SetPlayerState(Character player, PlayerStateType value = PlayerStateType.Null, GameObj? gameObj = null)
            {
                switch (player.PlayerState)
                {
                    case PlayerStateType.OpeningTheChest:
                        ((Chest)player.WhatInteractingWith).StopOpen();
                        break;
                    case PlayerStateType.OpeningTheDoorway:
                        Doorway doorway = (Doorway)player.WhatInteractingWith;
                        doorway.OpenDegree += gameMap.Timer.nowTime() - doorway.OpenStartTime;
                        doorway.OpenStartTime = 0;
                        break;
                    default:
                        break;
                }
                player.ChangePlayerState(value, gameObj);
            }

            public Character? AddPlayer(XY pos, int teamID, int playerID, CharacterType characterType, Character? parent = null)
            {
                Character newPlayer;

                if (characterType == CharacterType.Robot)
                {
                    newPlayer = new Golem(pos, GameData.characterRadius, parent);
                }
                else newPlayer = (GameData.IsGhost(characterType)) ? new Ghost(pos, GameData.characterRadius, characterType) : new Student(pos, GameData.characterRadius, characterType);
                gameMap.Add(newPlayer);

                newPlayer.TeamID = teamID;
                newPlayer.PlayerID = playerID;
                #region 人物装弹
                new Thread
                (
                    () =>
                    {
                        while (!gameMap.Timer.IsGaming)
                            Thread.Sleep(Math.Max(newPlayer.CD, GameData.checkInterval));
                        long lastTime = Environment.TickCount64;
                        new FrameRateTaskExecutor<int>(
                            loopCondition: () => gameMap.Timer.IsGaming && !newPlayer.IsResetting,
                            loopToDo: () =>
                            {
                                long nowTime = Environment.TickCount64;
                                if (newPlayer.BulletNum == newPlayer.MaxBulletNum)
                                    lastTime = nowTime;
                                else if (nowTime - lastTime >= newPlayer.CD)
                                {
                                    _ = newPlayer.TryAddBulletNum();
                                    lastTime = nowTime;
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
                #endregion
                #region BGM,牵制得分更新
                new Thread
                (
                    () =>
                    {
                        while (!gameMap.Timer.IsGaming)
                            Thread.Sleep(GameData.checkInterval);
                        int TimePinningDown = 0, ScoreAdded = 0;

                        bool noise = false;
                        if (!newPlayer.IsGhost())
                        {
                            gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                            try
                            {
                                foreach (Character person in gameMap.GameObjDict[GameObjType.Character])
                                {
                                    if (person.IsGhost())
                                    {
                                        if (person.CharacterType == CharacterType.ANoisyPerson)
                                        {
                                            noise = true;
                                            newPlayer.AddBgm(BgmType.GhostIsComing, 1411180);
                                            newPlayer.AddBgm(BgmType.GeneratorIsBeingFixed, 154991);
                                        }
                                    }
                                }
                            }
                            finally
                            {
                                gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                            }
                        }
                        new FrameRateTaskExecutor<int>(
                        loopCondition: () => gameMap.Timer.IsGaming && !newPlayer.IsResetting,
                        loopToDo: () =>
                        {
                            gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                            try
                            {
                                if (newPlayer.IsGhost())
                                {
                                    double bgmVolume = 0;
                                    foreach (Character person in gameMap.GameObjDict[GameObjType.Character])
                                    {
                                        if (!person.IsGhost() && XY.DistanceFloor3(newPlayer.Position, person.Position) <= (newPlayer.AlertnessRadius / person.Concealment))
                                        {
                                            if ((double)newPlayer.AlertnessRadius / XY.DistanceFloor3(newPlayer.Position, person.Position) > bgmVolume)
                                                bgmVolume = newPlayer.AlertnessRadius / XY.DistanceFloor3(newPlayer.Position, person.Position);
                                        }
                                    }
                                    newPlayer.AddBgm(BgmType.StudentIsApproaching, bgmVolume);
                                }
                                else
                                {
                                    foreach (Character person in gameMap.GameObjDict[GameObjType.Character])
                                    {
                                        if (person.IsGhost())
                                        {
                                            if (!noise)
                                            {
                                                if (XY.DistanceFloor3(newPlayer.Position, person.Position) <= (newPlayer.AlertnessRadius / person.Concealment))
                                                    newPlayer.AddBgm(BgmType.GhostIsComing, (double)newPlayer.AlertnessRadius / XY.DistanceFloor3(newPlayer.Position, person.Position));
                                                else newPlayer.AddBgm(BgmType.GhostIsComing, 0);
                                            }
                                            if (newPlayer.CharacterType != CharacterType.Teacher && newPlayer.CharacterType != CharacterType.Robot && !newPlayer.NoHp() && newPlayer.PlayerState != PlayerStateType.Stunned && XY.DistanceFloor3(newPlayer.Position, person.Position) <= GameData.PinningDownRange)
                                            {
                                                TimePinningDown += GameData.checkInterval;
                                                newPlayer.AddScore(GameData.StudentScorePinDown(TimePinningDown) - ScoreAdded);
                                                ScoreAdded = GameData.StudentScorePinDown(TimePinningDown);
                                            }
                                            else TimePinningDown = ScoreAdded = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                            finally
                            {
                                gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                            }

                            if (!noise)
                            {
                                gameMap.GameObjLockDict[GameObjType.Generator].EnterReadLock();
                                try
                                {
                                    double bgmVolume = 0;
                                    foreach (Generator generator in gameMap.GameObjDict[GameObjType.Generator])
                                    {
                                        if (XY.DistanceFloor3(newPlayer.Position, generator.Position) <= newPlayer.AlertnessRadius)
                                        {
                                            if (generator.NumOfFixing > 0 && (double)newPlayer.AlertnessRadius * generator.DegreeOfRepair / GameData.degreeOfFixedGenerator / XY.DistanceFloor3(newPlayer.Position, generator.Position) > bgmVolume)
                                                bgmVolume = (double)newPlayer.AlertnessRadius * generator.DegreeOfRepair / GameData.degreeOfFixedGenerator / XY.DistanceFloor3(newPlayer.Position, generator.Position);
                                        }
                                    }
                                    newPlayer.AddBgm(BgmType.GeneratorIsBeingFixed, bgmVolume);
                                }
                                finally
                                {
                                    gameMap.GameObjLockDict[GameObjType.Generator].ExitReadLock();
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
                #endregion

                return newPlayer;
            }

            public void BeAddictedToGame(Student player, Ghost ghost)
            {
                if (player.CharacterType == CharacterType.Robot)
                {
                    ghost.AddScore(GameData.TrickerScoreDestroyRobot);
                    Die(player);
                    return;
                }
                ghost.AddScore(GameData.TrickerScoreStudentBeAddicted);
                if (player.GamingAddiction > 0)
                {
                    if (player.GamingAddiction < GameData.BeginGamingAddiction)
                        player.GamingAddiction = GameData.BeginGamingAddiction;
                    else if (player.GamingAddiction < GameData.MidGamingAddiction)
                        player.GamingAddiction = GameData.MidGamingAddiction;
                    else
                    {
                        ghost.AddScore(GameData.TrickerScoreStudentDie);
                        Die(player);
                        return;
                    }
                }
                SetPlayerState(player, PlayerStateType.Addicted);
                new Thread
                    (() =>
                    {
#if DEBUG
                        Debugger.Output(player, " is addicted ");
#endif
                        new FrameRateTaskExecutor<int>(
                            () => (player.PlayerState == PlayerStateType.Addicted || player.PlayerState == PlayerStateType.Rescued) && player.GamingAddiction < player.MaxGamingAddiction && gameMap.Timer.IsGaming,
                            () =>
                            {
                                player.GamingAddiction += (player.PlayerState == PlayerStateType.Addicted) ? GameData.frameDuration : 0;
                            },
                            timeInterval: GameData.frameDuration,
                            () =>
                            {
                                if (player.GamingAddiction == player.MaxGamingAddiction && gameMap.Timer.IsGaming)
                                {
                                    ghost.AddScore(GameData.TrickerScoreStudentDie);
                                    Die(player);
                                }
                                return 0;
                            }
                        )
                            .Start();
                    }
                    )
                { IsBackground = true }.Start();
            }

            public bool BeStunned(Character player, int time)
            {
                if (player.PlayerState == PlayerStateType.Stunned || player.NoHp() || player.CharacterType == CharacterType.Robot) return false;
                new Thread
                    (() =>
                    {
                        SetPlayerState(player, PlayerStateType.Stunned);
                        Thread.Sleep(time);
                        if (player.PlayerState == PlayerStateType.Stunned)
                            SetPlayerState(player);
                    }
                    )
                { IsBackground = true }.Start();
                return true;
            }

            public bool TryBeAwed(Student character, Bullet bullet)
            {
                if (character.CanBeAwed())
                {
                    if (BeStunned(character, GameData.basicStunnedTimeOfStudent))
                        bullet.Parent.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.basicStunnedTimeOfStudent));
                    return true;
                }
                return false;
            }

            /// <summary>
            /// 遭受攻击
            /// </summary>
            /// <param name="subHP"></param>
            /// <param name="hasSpear"></param>
            /// <param name="attacker">伤害来源</param>
            /// <returns>人物在受到攻击后死了吗</returns>
            public void BeAttacked(Student student, Bullet bullet)
            {
#if DEBUG
                Debugger.Output(student, "is being shot!");
#endif
                if (student.NoHp()) return;  // 原来已经死了
                if (!bullet.Parent.IsGhost()) return;

                if (student.CharacterType == CharacterType.StraightAStudent)
                {
                    ((WriteAnswers)student.FindIActiveSkill(ActiveSkillType.WriteAnswers)).DegreeOfMeditation = 0;
                }
                student.SetDegreeOfTreatment0();
#if DEBUG
                Debugger.Output(bullet, " 's AP is " + bullet.AP.ToString());
#endif
                if (student.TryUseShield())
                {
                    if (bullet.HasSpear)
                    {
                        int subHp = student.TrySubHp(bullet.AP);
#if DEBUG
                        Debugger.Output(this, "is being shot! Now his hp is" + student.HP.ToString());
#endif
                        bullet.Parent.AddScore(GameData.TrickerScoreAttackStudent(subHp) + GameData.ScorePropUseSpear);
                        bullet.Parent.HP = (int)(bullet.Parent.HP + (bullet.Parent.Vampire * subHp));
                    }
                    else return;
                }
                else
                {
                    int subHp;
                    if (bullet.HasSpear)
                    {
                        subHp = student.TrySubHp(bullet.AP + GameData.ApSpearAdd);
#if DEBUG
                        Debugger.Output(this, "is being shot with Spear! Now his hp is" + student.HP.ToString());
#endif
                    }
                    else
                    {
                        subHp = student.TrySubHp(bullet.AP);
#if DEBUG
                        Debugger.Output(this, "is being shot! Now his hp is" + student.HP.ToString());
#endif
                    }
                    bullet.Parent.AddScore(GameData.TrickerScoreAttackStudent(subHp));
                    bullet.Parent.HP = (int)(bullet.Parent.HP + (bullet.Parent.Vampire * subHp));
                }
                if (student.HP <= 0)
                    student.TryActivatingLIFE();  // 如果有复活甲

                if (student.HP <= 0)
                    BeAddictedToGame(student, (Ghost)bullet.Parent);
                else TryBeAwed(student, bullet);
            }

            public bool BackSwing(Character player, int time)
            {
                if (time <= 0) return false;
                if (player.PlayerState == PlayerStateType.Swinging || (!player.Commandable() && player.PlayerState != PlayerStateType.TryingToAttack)) return false;
                SetPlayerState(player, PlayerStateType.Swinging);

                new Thread
                        (() =>
                        {
                            Thread.Sleep(time);

                            if (player.PlayerState == PlayerStateType.Swinging)
                            {
                                SetPlayerState(player);
                            }
                        }
                        )
                { IsBackground = true }.Start();
                return true;
            }

            public void Die(Student player)
            {
#if DEBUG
                Debugger.Output(player, "die.");
#endif
                if (player.PlayerState == PlayerStateType.Deceased) return;
                player.RemoveFromGame(PlayerStateType.Deceased);

                for (int i = 0; i < GameData.maxNumOfPropInPropInventory; i++)
                {
                    Prop? prop = player.UseProp(i);
                    if (prop != null)
                    {
                        prop.ReSetPos(player.Position, gameMap.GetPlaceType(player.Position));
                        gameMap.Add(prop);
                    }
                }
                if (player.CharacterType == CharacterType.Robot)
                {
                    if (((Golem)player).Parent != null && ((Golem)player).Parent.CharacterType == CharacterType.TechOtaku)
                    {
                        ((SummonGolem)(((Golem)player).Parent.FindIActiveSkill(ActiveSkillType.SummonGolem))).GolemSummoned = null;
                        player.FindIActiveSkill(ActiveSkillType.UseRobot).IsBeingUsed = false;
                    }
                    return;
                }
                ++gameMap.NumOfDeceasedStudent;
            }

        }
    }
}