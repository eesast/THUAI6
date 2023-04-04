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
                IActiveSkill skill = player.FindIActiveSkill(ActiveSkillType.CanBeginToCharge);
                Debugger.Output(player, "can begin to charge!");


                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 3.0);
                    //See SkillWhenColliding in ActionManager
                },
                                                      () =>
                                                      { });
            }

            public static bool BecomeInvisible(Character player)
            {
                if ((!player.Commandable())) return false;
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.BecomeInvisible);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    player.AddInvisible(activeSkill.DurationTime);
                    Debugger.Output(player, "become invisible!");
                },
                                                      () =>
                                                      { });
            }

            public static bool UseRobot(Character player)
            {
                if ((!player.Commandable()) || ((SummonGolem)player.FindIActiveSkill(ActiveSkillType.SummonGolem)).GolemSummoned == null) return false;
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.UseRobot);
                activeSkill.IsBeingUsed = (activeSkill.IsBeingUsed) ? false : true;
                return true;
            }

            public static bool JumpyBomb(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.JumpyBomb), player, () =>
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
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.WriteAnswers);
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
                if ((!player.Commandable())) return false;
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.SummonGolem);
                if (((SummonGolem)activeSkill).GolemSummoned != null) return false;
                XY res = player.Position + new XY(player.FacingDirection, player.Radius * 2);
                if (actionManager.moveEngine.CheckCollision(player, res) != null)
                    return false;

                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    characterManager.AddPlayer(res, player.TeamID, player.PlayerID + GameData.numOfPeople, CharacterType.Robot, player);
                },
                                                      () =>
                                                      { });
            }

            public static bool UseKnife(Character player)
            {
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.UseKnife), player, () =>
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
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Howl), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (!character.IsGhost() && XY.Distance(character.Position, player.Position) <= player.ViewRange)
                            {
                                if (CharacterManager.BeStunned(character, GameData.TimeOfStudentFaintingWhenHowl))
                                    player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.TimeOfStudentFaintingWhenHowl));
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    CharacterManager.BackSwing(player, GameData.TimeOfGhostSwingingAfterHowl);
                    Debugger.Output(player, "howled!");
                },
                                                      () =>
                                                      { });
            }

            public bool Punish(Character player)
            {
                if ((!player.Commandable())) return false;
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Punish), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (character.IsGhost() &&
                                (character.PlayerState == PlayerStateType.TryingToAttack || character.PlayerState == PlayerStateType.Swinging
                                || character.PlayerState == PlayerStateType.UsingSkill)
                                && gameMap.CanSee(player, character))
                            {
                                if (CharacterManager.BeStunned(character, GameData.TimeOfGhostFaintingWhenPunish))
                                    player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.TimeOfGhostFaintingWhenPunish));
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
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Rouse), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if ((character.PlayerState == PlayerStateType.Addicted) && gameMap.CanSee(player, character))
                            {
                                character.PlayerState = PlayerStateType.Null;
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
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Encourage), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if ((character.HP < character.MaxHp) && gameMap.CanSee(player, character))
                            {
                                player.AddScore(GameData.StudentScoreTreat(character.MaxHp - character.HP));
                                character.HP = character.MaxHp;
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
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Inspire), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (gameMap.CanSee(player, character))
                            {
                                player.AddScore(GameData.StudentScoreTreat(character.MaxHp - character.HP));
                                character.AddMoveSpeed(GameData.PropDuration);
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

            public static bool ActiveSkillEffect(IActiveSkill activeSkill, Character player, Action startSkill, Action endSkill)
            {
                lock (activeSkill.ActiveSkillLock)
                {
                    ActiveSkillType activeSkillType = SkillFactory.FindActiveSkillType(activeSkill);
                    if (player.TimeUntilActiveSkillAvailable[activeSkillType] == 0)
                    {
                        player.SetTimeUntilActiveSkillAvailable(activeSkillType, activeSkill.SkillCD);
                        new Thread
                        (() =>
                        {
                            startSkill();
                            activeSkill.IsBeingUsed = true;
                            new FrameRateTaskExecutor<int>(
                                () => !player.IsResetting,
                                () =>
                                {
                                    player.AddTimeUntilActiveSkillAvailable(activeSkillType, -(int)GameData.frameDuration);
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
                            activeSkill.IsBeingUsed = false;
                            Debugger.Output(player, "return to normal.");

                            new FrameRateTaskExecutor<int>(
                                loopCondition: () => player.TimeUntilActiveSkillAvailable[activeSkillType] > 0 && !player.IsResetting,
                                loopToDo: () =>
                                {
                                    player.AddTimeUntilActiveSkillAvailable(activeSkillType, -(int)GameData.frameDuration);
                                },
                                timeInterval: GameData.frameDuration,
                                finallyReturn: () => 0
                            )
                            {
                                AllowTimeExceed = true,
                                MaxTolerantTimeExceedCount = ulong.MaxValue,
                            }
                                .Start();

                            player.SetTimeUntilActiveSkillAvailable(activeSkillType, 0);
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