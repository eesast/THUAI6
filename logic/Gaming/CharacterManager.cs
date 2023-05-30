using System.Threading;
using GameClass.GameObj;
using Preparation.Utility;
using Preparation.Interface;
using Timothy.FrameRateTask;
using static Gaming.Game;

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

            private readonly object numTeacherLock = new();
            private int factorTeacher = 1;
            public int FactorTeacher
            {
                get
                {
                    lock (numTeacherLock)
                    {
                        return factorTeacher;
                    }
                }
            }
            public void DoubleFactorTeacher()
            {
                lock (numTeacherLock)
                    factorTeacher *= 2;
            }

            public Character? AddPlayer(XY pos, long teamID, long playerID, CharacterType characterType, Character? parent = null)
            {
                Character newPlayer;

                if (characterType == CharacterType.Robot)
                {
                    newPlayer = new Golem(pos, GameData.characterRadius, parent);
                }
                else
                {
                    if (GameData.IsGhost(characterType))
                        newPlayer = gameMap.ghost = new Ghost(pos, GameData.characterRadius, characterType);
                    else
                    {
                        newPlayer = new Student(pos, GameData.characterRadius, characterType);
                        if (characterType == CharacterType.Teacher)
                            DoubleFactorTeacher();
                    }
                }
                gameMap.Add(newPlayer);

                newPlayer.TeamID = teamID;
                newPlayer.PlayerID = playerID;
                /* #region 人物装弹
                 new Thread
                 (
                     () =>
                     {
                         while (!gameMap.Timer.IsGaming)
                             Thread.Sleep(Math.Max(newPlayer.CD, GameData.checkInterval));
                         long lastTime = Environment.TickCount64;
                         new FrameRateTaskExecutor<int>(
                             loopCondition: () => gameMap.Timer.IsGaming && !newPlayer.IsRemoved,
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
                             AllowTimeExceed = true,
                         }
                             .Start();
                     }
                 )
                 { IsBackground = true }.Start();
                 #endregion
     */
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
                        loopCondition: () => gameMap.Timer.IsGaming && !newPlayer.IsRemoved,
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
                                            if (newPlayer.CharacterType != CharacterType.Teacher && newPlayer.CharacterType != CharacterType.Robot && newPlayer.CanPinDown() && XY.DistanceFloor3(newPlayer.Position, person.Position) <= GameData.PinningDownRange)
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
                long stateNum = player.SetPlayerState(RunningStateType.RunningForcibly, PlayerStateType.Addicted);
                if (stateNum == -1) return;

                if (player.CharacterType == CharacterType.Robot)
                {
                    Die(player);
                    return;
                }
                if (player.GamingAddiction > 0)
                {
                    if (player.GamingAddiction < GameData.BeginGamingAddiction)
                        player.GamingAddiction = GameData.BeginGamingAddiction;
                    else if (player.GamingAddiction < GameData.MidGamingAddiction)
                        player.GamingAddiction = GameData.MidGamingAddiction;
                    else
                    {
                        Die(player);
                        return;
                    }
                }
                ghost.AddScore(GameData.TrickerScoreStudentBeAddicted);

                gameMap.MapAddictStudent();
                new Thread
                    (() =>
                    {
                        Debugger.Output(player, " is addicted ");

                        new FrameRateTaskExecutor<int>(
                            () => stateNum == player.StateNum && player.GamingAddiction < player.MaxGamingAddiction && gameMap.Timer.IsGaming,
                            () =>
                            {
                                player.GamingAddiction += (player.PlayerState == PlayerStateType.Addicted) ? GameData.frameDuration : 0;
                            },
                            timeInterval: GameData.frameDuration,
                            () =>
                            {
                                gameMap.MapRescueStudent();
                                if (player.GamingAddiction == player.MaxGamingAddiction && gameMap.Timer.IsGaming)
                                {
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

            public static long BeStunned(Character player, int time)
            {
                if (player.CharacterType == CharacterType.Robot) return -1;
                long stateNum = player.SetPlayerState(RunningStateType.RunningForcibly, PlayerStateType.Stunned);
                if (stateNum == -1) return -1;
                new Thread
                    (() =>
                    {
                        Debugger.Output(player, " is stunned for " + time.ToString());
                        Thread.Sleep(time);
                        player.ResetPlayerState(stateNum);
                    }
                    )
                { IsBackground = true }.Start();
                return stateNum;
            }

            public bool TryBeAwed(Student character, Bullet bullet)
            {
                if (character.CanBeAwed())
                {
                    if (BeStunned(character, GameData.basicStunnedTimeOfStudent) > 0)
                        bullet.Parent!.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.basicStunnedTimeOfStudent));
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
                if (!bullet.Parent!.IsGhost()) return;

                if (student.CharacterType == CharacterType.StraightAStudent)
                {
                    ((WriteAnswers)student.FindActiveSkill(ActiveSkillType.WriteAnswers)).DegreeOfMeditation = 0;
                }
                student.SetDegreeOfTreatment0();

                if (student.NoHp()) return;  // 原来已经死了

#if DEBUG
                Debugger.Output(bullet, " 's AP is " + bullet.AP.ToString());
#endif
                if (student.TryUseShield())
                {
                    if (bullet.HasSpear)
                    {
                        long subHp = student.SubHp(bullet.AP);
#if DEBUG
                        Debugger.Output(this, "is being shot! Now his hp is" + student.HP.ToString());
#endif
                        bullet.Parent.AddScore(GameData.TrickerScoreAttackStudent(subHp) + GameData.ScorePropUseSpear);
                        bullet.Parent.AddHP((long)bullet.Parent.Vampire * subHp);
                    }
                    else return;
                }
                else
                {
                    long subHp;
                    if (bullet.HasSpear)
                    {
                        subHp = student.SubHp(bullet.AP + GameData.ApSpearAdd);
#if DEBUG
                        Debugger.Output(this, "is being shot with Spear! Now his hp is" + student.HP.ToString());
#endif
                    }
                    else
                    {
                        subHp = student.SubHp(bullet.AP);
#if DEBUG
                        Debugger.Output(this, "is being shot! Now his hp is" + student.HP.ToString());
#endif
                    }
                    bullet.Parent.AddScore(GameData.TrickerScoreAttackStudent(subHp));
                    if (student.CharacterType == CharacterType.Teacher)
                    {
                        student.AddScore(subHp * GameData.factorOfScoreWhenTeacherAttacked / GameData.basicApOfGhost / FactorTeacher);
                    }

                    bullet.Parent.AddHP((long)(bullet.Parent.Vampire * subHp));
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
                long stateNum = player.SetPlayerState(RunningStateType.RunningForcibly, PlayerStateType.Swinging);
                if (stateNum == -1) return false;

                new Thread
                        (() =>
                        {
                            Thread.Sleep(time);
                            player.ResetPlayerState(stateNum);
                        }
                        )
                { IsBackground = true }.Start();
                return true;
            }

            public void Die(Student player)
            {
                Debugger.Output(player, "die.");

                if (!player.TryToRemoveFromGame(PlayerStateType.Deceased)) return;

                for (int i = 0; i < GameData.maxNumOfPropInPropInventory; i++)
                {
                    Gadget? prop = player.ConsumeProp(i);
                    if (prop != null)
                    {
                        prop.ReSetPos(player.Position);
                        gameMap.Add(prop);
                    }
                }
                if (player.CharacterType == CharacterType.Robot)
                {
                    var parent = ((Golem)player).Parent;
                    if (parent != null && parent.CharacterType == CharacterType.TechOtaku)
                    {
                        ((SummonGolem)(parent.FindActiveSkill(ActiveSkillType.SummonGolem))).DeleteGolem((int)(player.PlayerID - parent.PlayerID) / GameData.numOfPeople - 1);
                        UseRobot useRobot = (UseRobot)parent.FindActiveSkill(ActiveSkillType.UseRobot);
                        if (useRobot.TryResetNowPlayerID((int)player.PlayerID))
                        {
                            lock (parent.ActionLock)
                            {
                                if (parent.PlayerState == PlayerStateType.UsingSkill)
                                    parent.SetPlayerStateNaturally();
                            }
                        }
                        gameMap.ghost.AddScore(GameData.TrickerScoreDestroyRobot);
                        return;
                    }
                }
                gameMap.MapDieStudent();
            }
        }
    }
}