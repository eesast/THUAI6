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
            public bool BecomeVampire(Character player)
            {
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.BecomeVampire), player, () =>
                {
                    player.Vampire += 0.5;
                    Debugger.Output(player, "becomes vampire!");
                },
                                                      () =>
                                                      {
                                                          double tempVam = player.Vampire - 0.5;
                                                          player.Vampire = tempVam < player.OriVampire ? player.OriVampire : tempVam;
                                                      });
            }

            public bool CanBeginToCharge(Character player)
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
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.BecomeInvisible);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    player.AddInvisible(activeSkill.DurationTime);
                    Debugger.Output(player, "become invisible!");
                },
                                                      () =>
                                                      { });
            }

            public bool JumpyBomb(Character player)
            {
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
                IActiveSkill activeSkill = player.FindIActiveSkill(ActiveSkillType.WriteAnswers);
                return ActiveSkillEffect(activeSkill, player, () =>
                {
                    Generator? generator = (Generator?)gameMap.OneForInteract(player.Position, GameObjType.Generator);
                    if (generator != null)
                    {
                        generator.Repair(((WriteAnswers)activeSkill).DegreeOfMeditation);
                        Debugger.Output(player, "uses WriteAnswers in" + generator.ToString() + "with " + (((WriteAnswers)activeSkill).DegreeOfMeditation).ToString());
                        ((WriteAnswers)activeSkill).DegreeOfMeditation = 0;
                    }
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
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Howl), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (!character.IsGhost() && XY.Distance(character.Position, player.Position) <= player.ViewRange)
                            {
                                if (AttackManager.BeStunned(character, GameData.TimeOfStudentFaintingWhenHowl))
                                    player.AddScore(GameData.TrickerScoreStudentBeStunned(GameData.TimeOfStudentFaintingWhenHowl));
                                break;
                            }
                        }
                    }
                    finally
                    {
                        gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                    }
                    AttackManager.BackSwing(player, GameData.TimeOfGhostSwingingAfterHowl);
                    Debugger.Output(player, "howled!");
                },
                                                      () =>
                                                      { });
            }

            public bool Punish(Character player)
            {
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.Punish), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (character.IsGhost() &&
                                (character.PlayerState == PlayerStateType.TryingToAttack || character.PlayerState == PlayerStateType.Swinging
                                || character.PlayerState == PlayerStateType.UsingSkill || character.PlayerState == PlayerStateType.LockingOrOpeningTheDoor || character.PlayerState == PlayerStateType.OpeningTheChest)
                                && gameMap.CanSee(player, character))
                            {
                                if (AttackManager.BeStunned(character, GameData.TimeOfGhostFaintingWhenPunish + (player.MaxHp - player.HP) / GameData.timeFactorOfGhostFainting))
                                    player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.TimeOfGhostFaintingWhenPunish + (player.MaxHp - player.HP) / GameData.timeFactorOfGhostFainting));
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

            public bool SuperFast(Character player)
            {
                return ActiveSkillEffect(player.FindIActiveSkill(ActiveSkillType.SuperFast), player, () =>
                {
                    player.AddMoveSpeed(player.FindIActiveSkill(ActiveSkillType.SuperFast).DurationTime, 3.0);
                    Debugger.Output(player, "moves very fast!");
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