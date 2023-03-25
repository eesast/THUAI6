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
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.BecomeVampire), player, () =>
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
                IActiveSkill skill = player.UseIActiveSkill(ActiveSkillType.CanBeginToCharge);
                Debugger.Output(player, "can begin to charge!");
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
                            foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                            {
                                if (character.IsGhost() != player.IsGhost() && XY.Distance(player.Position + new XY(player.FacingDirection, player.Radius), character.Position) <= character.Radius)
                                {
                                    AttackManager.BeStunned(character, GameData.TimeOfGhostFaintingWhenCharge);
                                    player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.TimeOfGhostFaintingWhenCharge));
                                    AttackManager.BeStunned(player, GameData.TimeOfStudentFaintingWhenCharge);
                                    break;
                                }
                            }
                        }
                        finally
                        {
                            gameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
                        }
                    },
                     timeInterval: GameData.frameDuration,
                     finallyReturn: () => 0,
                     maxTotalDuration: skill.DurationTime
                      )

    .Start();
              }

          )
                { IsBackground = true }.Start();

                return ActiveSkillEffect(skill, player, () =>
                {
                    player.AddMoveSpeed(skill.DurationTime, 3.0);
                },
                                                      () =>
                                                      {
                                                          double tempVam = player.Vampire - 0.5;
                                                          player.Vampire = tempVam < player.OriVampire ? player.OriVampire : tempVam;
                                                      });
            }


            public static bool BecomeInvisible(Character player)
            {
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.BecomeInvisible), player, () =>
                {
                    player.IsInvisible = true;
                    Debugger.Output(player, "become invisible!");
                },
                                                      () =>
                                                      { player.IsInvisible = false; });
            }

            public bool NuclearWeapon(Character player)
            {
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.NuclearWeapon), player, () =>
                {
                    player.BulletOfPlayer = BulletType.AtomBomb;
                    Debugger.Output(player, "uses atombomb!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }


            public static bool UseKnife(Character player)
            {
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.UseKnife), player, () =>
                {
                    player.BulletOfPlayer = BulletType.FlyingKnife;
                    Debugger.Output(player, "uses flyingknife!");
                },
                                                      () =>
                                                      { player.BulletOfPlayer = player.OriBulletOfPlayer; });
            }

            public bool Punish(Character player)
            {
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.Punish), player, () =>
                {
                    gameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
                    try
                    {
                        foreach (Character character in gameMap.GameObjDict[GameObjType.Character])
                        {
                            if (player.IsGhost() && XY.Distance(player.Position, character.Position) <= player.ViewRange)
                            {
                                AttackManager.BeStunned(character, GameData.TimeOfGhostFaintingWhenPunish + (player.MaxHp - player.HP) / GameData.TimeFactorOfGhostFainting);
                                player.AddScore(GameData.StudentScoreTrickerBeStunned(GameData.TimeOfGhostFaintingWhenPunish + (player.MaxHp - player.HP) / GameData.TimeFactorOfGhostFainting));
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
                return ActiveSkillEffect(player.UseIActiveSkill(ActiveSkillType.SuperFast), player, () =>
                {
                    player.AddMoveSpeed(player.UseIActiveSkill(ActiveSkillType.SuperFast).DurationTime, 3.0);
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