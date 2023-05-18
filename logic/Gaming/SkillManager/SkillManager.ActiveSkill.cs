using GameClass.GameObj;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using Timothy.FrameRateTask;

namespace Gaming
{
    public partial class Game
    {
        private partial class SkillManager
        {
            public static bool CanBeginToCharge(Character player)
            {

                if ((!player.Commandable())) return false;
                ActiveSkill skill = player.FindActiveSkill(ActiveSkillType.CanBeginToCharge);
                Debugger.Output(player, "can begin to charge!");


                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 3.0);
                    //See SkillWhenColliding in ActionManager
                },
                                                      () =>
                                                      { });
            }

            public bool ShowTime(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill skill = player.FindActiveSkill(ActiveSkillType.ShowTime);

                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 0.8);
                    new Thread
                    (
                   () =>
                   {
                       new FrameRateTaskExecutor<int>(
                       loopCondition: () => player.Commandable() && gameMap.Timer.IsGaming,
                             loopToDo: () =>
                             {
                                 gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                                 try
                                 {
                                     foreach (Character person in gameMap.GameObjDict[GameObjType.Character])
                                     {
                                         if (!person.IsGhost() && player.CharacterType != CharacterType.Robot && !person.NoHp())
                                         {
                                             double dis = XY.DistanceFloor3(person.Position, player.Position);
                                             if (dis >= player.AlertnessRadius)
                                             {
                                                 person.AddMoveSpeed(GameData.checkIntervalWhenShowTime, dis / player.AlertnessRadius);
                                                 actionManager.MovePlayerWhenStunned(person, GameData.checkIntervalWhenShowTime, (player.Position - person.Position).Angle());
                                             }
                                             else if (dis >= player.ViewRange)
                                             {
                                                 Student student = (Student)person;
                                                 student.GamingAddiction += GameData.checkIntervalWhenShowTime;
                                                 if (student.GamingAddiction == student.MaxGamingAddiction)
                                                 {
                                                     player.AddScore(GameData.TrickerScoreStudentDie);
                                                     characterManager.Die(student);
                                                 }
                                             }
                                         }
                                     }
                                 }
                                 finally
                                 {
                                     gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                                 }
                             },
                             timeInterval: GameData.checkIntervalWhenShowTime,
                             finallyReturn: () => 0
                         )

                             .Start();
                   }
                    )
                    { IsBackground = true }.Start();
                },
                                                      () =>
                                                      {
                                                      }
                                                      );
            }

            public static bool BecomeInvisible(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.BecomeInvisible);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    player.AddScore(GameData.ScoreBecomeInvisible);
                    player.AddInvisible(activeSkill.DurationTime);
                    Debugger.Output(player, "become invisible!");
                },
                                                      () =>
                                                      { });
            }

            public bool UseRobot(Character player)
            {
                /*
                IGolem? golem = (IGolem?)(((SummonGolem)player.FindActiveSkill(ActiveSkillType.SummonGolem)).GolemSummoned);
                if ((!player.Commandable()) || ((SummonGolem)player.FindActiveSkill(ActiveSkillType.SummonGolem)).GolemSummoned == null) return false;
                Debugger.Output(player, "use robot!");
                IActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.UseRobot);
                activeSkill.IsBeingUsed = (activeSkill.IsBeingUsed) ? false : true;
                if (activeSkill.IsBeingUsed) player.SetPlayerState(PlayerStateType.UsingSkill);
                else player.SetPlayerState();*/
                return true;
            }

            public static bool JumpyBomb(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.JumpyBomb), player, () =>
                {
                    player.BulletOfPlayer = BulletType.BombBomb;
                    Debugger.Output(player, "uses jumpybomb!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }

            public bool WriteAnswers(Character player)
            {
                if ((!player.Commandable())) return false;
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.WriteAnswers);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    Generator? generator = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);
                    if (generator != null)
                    {
                        if (generator.Repair(((WriteAnswers)activeSkill).DegreeOfMeditation, player))
                            gameMap.NumOfRepairedGenerators++;
                        Debugger.Output(player, "uses WriteAnswers in" + generator.ToString() + "with " + (((WriteAnswers)activeSkill).DegreeOfMeditation).ToString());
                        ((WriteAnswers)activeSkill).DegreeOfMeditation = 0;
                    }
                },
                                                      () =>
                                                      { });
            }

            public bool SummonGolem(Character player)
            {
                ActiveSkill activeSkill = player.FindActiveSkill(ActiveSkillType.SummonGolem);
                int num = ((SummonGolem)activeSkill).AddGolem();
                if (num >= GameData.maxSummonedGolemNum) return false;
                Debugger.Output(player, num.ToString());

                XY res = player.Position + new XY(player.FacingDirection, player.Radius * 2);
                CraftingBench craftingBench = new(res, player, num);

                long stateNum = player.SetPlayerState(PlayerStateType.UsingSkill, craftingBench);
                if (stateNum == -1)
                {
                    ((SummonGolem)activeSkill).DeleteGolem(num);
                    return false;
                }

                player.ThreadNum.WaitOne();
                if (stateNum != player.StateNum)
                {
                    ((SummonGolem)activeSkill).DeleteGolem(num);
                    player.ThreadNum.Release();
                    return false;
                }

                if (actionManager.moveEngine.CheckCollision(craftingBench, res) != null)
                {
                    ((SummonGolem)activeSkill).DeleteGolem(num);
                    player.ThreadNum.Release();
                    return false;
                }
                craftingBench.ParentStateNum = stateNum;
                gameMap.Add(craftingBench);
                /*
               */
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                },
                () =>
                {
                    lock (player.ActionLock)
                    {
                        if (stateNum == player.StateNum)
                        {
                            gameMap.RemoveJustFromMap(craftingBench);
                            Golem? golem = (Golem?)characterManager.AddPlayer(res, player.TeamID, (num + 1) * GameData.numOfPeople + player.PlayerID, CharacterType.Robot, player);
                            if (golem == null)
                            {
                                ((SummonGolem)activeSkill).DeleteGolem(num);
                            }
                            Debugger.Output(player, activeSkill.DurationTime.ToString());
                            player.SetPlayerStateNaturally();
                            Debugger.Output(player, player.StateNum.ToString());
                            player.ThreadNum.Release();
                        }
                    }
                }
                 );
            }

            public static bool UseKnife(Character player)
            {
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.UseKnife), player, () =>
                {
                    player.BulletOfPlayer = BulletType.FlyingKnife;
                    Debugger.Output(player, "uses flyingknife!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }

            public bool Howl(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Howl), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (!character.IsGhost() && !character.NoHp() && XY.DistanceFloor3(character.Position, player.Position) <= player.ViewRange)
                            {
                                if (characterManager.BeStunned(character, GameData.timeOfStudentStunnedWhenHowl) > 0)
                                    player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.timeOfStudentStunnedWhenHowl));
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    characterManager.BackSwing(player, GameData.timeOfGhostSwingingAfterHowl);
                    Debugger.Output(player, "howled!");
                },
                                                      () =>
                                                      { });
            }

            public bool Punish(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Punish), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (character.IsGhost() &&
                                (character.PlayerState == PlayerStateType.TryingToAttack || character.PlayerState == PlayerStateType.Swinging
                                || character.PlayerState == PlayerStateType.UsingSkill
                                || character.PlayerState == PlayerStateType.LockingTheDoor || character.PlayerState == PlayerStateType.OpeningTheDoor
                                || character.PlayerState == PlayerStateType.ClimbingThroughWindows)
                                && gameMap.CanSee(player, character))
                            {
                                if (characterManager.BeStunned(character, GameData.timeOfGhostStunnedWhenPunish + GameData.factorOfTimeStunnedWhenPunish * (player.MaxHp - player.HP)) > 0)
                                    player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.timeOfGhostStunnedWhenPunish + GameData.factorOfTimeStunnedWhenPunish * (player.MaxHp - player.HP)));
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "uses punishing!");
                },
                                                      () =>
                                                      { });
            }

            public bool Rouse(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Rouse), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if ((character.PlayerState == PlayerStateType.Addicted) && gameMap.CanSee(player, character))
                            {
                                character.SetPlayerState();
                                character.HP = GameData.RemainHpWhenAddLife;
                                ((Student)character).TimeOfRescue = 0;
                                player.AddScore(GameData.StudentScoreRescue);
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "rouse someone!");
                },
                                                      () =>
                                                      { });
            }

            public bool Encourage(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Encourage), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if ((character.HP < character.MaxHp) && gameMap.CanSee(player, character))
                            {
                                player.AddScore(GameData.StudentScoreTreat(GameData.addHpWhenEncourage));
                                character.HP += GameData.addHpWhenEncourage;
                                ((Student)character).SetDegreeOfTreatment0();
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "encourage someone!");
                },
                                                      () =>
                                                      { });
            }

            public bool Inspire(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindActiveSkill(ActiveSkillType.Inspire), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (gameMap.CanSee(player, character))
                            {
                                player.AddScore(GameData.ScoreInspire);
                                character.AddMoveSpeed(GameData.timeOfAddingSpeedWhenInspire, GameData.addedTimeOfSpeedWhenInspire);
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    Debugger.Output(player, "inspires!");
                },
                                                      () =>
                                                      { });
            }

            public static bool ActiveSkillEffect(ActiveSkill activeSkill, Character player, Action startSkill, Action endSkill)
            {
                lock (activeSkill.ActiveSkillUseLock)
                {
                    if (activeSkill.TimeUntilActiveSkillAvailable == 0)
                    {
                        activeSkill.TimeUntilActiveSkillAvailable = activeSkill.SkillCD;

                        new Thread
                        (() =>
                        {
                            startSkill();
                            activeSkill.IsBeingUsed = 1;
                            new FrameRateTaskExecutor<int>(
                                () => !player.IsRemoved,
                                () =>
                                {
                                    activeSkill.TimeUntilActiveSkillAvailable -= (int)GameData.frameDuration;
                                },
                                timeInterval: GameData.frameDuration,
                                () => 0,
                                maxTotalDuration: (long)(activeSkill.DurationTime)
                            )
                            {
                                AllowTimeExceed = true,
                                MaxTolerantTimeExceedCount = ulong.MaxValue,
                            }
                                .Start();

                            endSkill();
                            activeSkill.IsBeingUsed = 0;
                            Debugger.Output(player, "return to normal.");

                            new FrameRateTaskExecutor<int>(
                                loopCondition: () => activeSkill.TimeUntilActiveSkillAvailable > 0,
                                loopToDo: () =>
                                {
                                    activeSkill.TimeUntilActiveSkillAvailable -= (int)GameData.frameDuration;
                                },
                                timeInterval: GameData.frameDuration,
                                finallyReturn: () => 0
                            )
                            {
                                AllowTimeExceed = true,
                                MaxTolerantTimeExceedCount = ulong.MaxValue,
                            }
                                .Start();

                            activeSkill.TimeUntilActiveSkillAvailable = 0;
                            Debugger.Output(player, "ActiveSkill is ready.");
                        }
                        )
                        { IsBackground = true }.Start();

                        return true;
                    }
                    else
                    {
                        Debugger.Output(player, "CommonSkill is cooling down!");
                        return false;
                    }
                }
            }
        }
    }
}