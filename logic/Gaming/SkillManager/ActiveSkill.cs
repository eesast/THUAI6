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
            public interface IActiveSkill
            {
                public int SkillCD { get; }
                public int DurationTime { get; } //技能持续时间
                public object ActiveSkillLock { get; }
                public bool SkillEffect(Character player);
            }
            public class BecomeVampire : IActiveSkill  // 化身吸血鬼
            {
                public int SkillCD => GameData.commonSkillCD / 3 * 4;
                public int DurationTime => GameData.commonSkillTime;

                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;

                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
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
            }
            private IActiveSkill becomeVampire = new BecomeVampire();

            public class BeginToCharge : IActiveSkill
            {
                public int SkillCD => GameData.commonSkillCD / 3 * 4;
                public int DurationTime => GameData.commonSkillTime;

                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;

                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
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
            }
            private IActiveSkill beginToCharge = new BeginToCharge();

            public class BecomeInvisible : IActiveSkill
            {
                public int SkillCD => GameData.commonSkillCD;
                public int DurationTime => GameData.commonSkillTime / 10 * 6;

                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;
                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
                    {
                        player.IsInvisible = true;
                        Debugger.Output(player, "become invisible!");
                    },
                                                          () =>
                                                          { player.IsInvisible = false; });
                }
            }
            private IActiveSkill becomeInvisible = new BecomeInvisible();

            public class NuclearWeapon : IActiveSkill  // 核武器
            {
                public int SkillCD => GameData.commonSkillCD / 3 * 7;
                public int DurationTime => GameData.commonSkillTime / 10;
                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;
                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
                    {
                        player.BulletOfPlayer = BulletType.AtomBomb;
                        Debugger.Output(player, "uses atombomb!");
                    },
                                                          () =>
                                                          { player.BulletOfPlayer = player.OriBulletOfPlayer; });
                }
            }
            private IActiveSkill nuclearWeapon = new NuclearWeapon();

            public class UseKnife : IActiveSkill
            {
                public int SkillCD => GameData.commonSkillCD / 3 * 2;
                public int DurationTime => GameData.commonSkillTime / 10;
                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;
                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
                    {
                        player.BulletOfPlayer = BulletType.FlyingKnife;
                        Debugger.Output(player, "uses flyingknife!");
                    },
                                                          () =>
                                                          { player.BulletOfPlayer = player.OriBulletOfPlayer; });
                }
            }
            private IActiveSkill useKnife = new UseKnife();

            public class SuperFast : IActiveSkill  // 3倍速
            {
                public int SkillCD => GameData.commonSkillCD;
                public int DurationTime => GameData.commonSkillTime / 10 * 4;
                private readonly object commonSkillLock = new object();
                public object ActiveSkillLock => commonSkillLock;
                public bool SkillEffect(Character player)
                {
                    return ActiveSkillEffect(this, player, () =>
                    {
                        player.AddMoveSpeed(this.DurationTime, 3.0);
                        Debugger.Output(player, "moves very fast!");
                    },
                                                          () =>
                                                          { });
                }
            }
            private IActiveSkill superFast = new SuperFast();

            public IActiveSkill? FindIActiveSkill(ActiveSkillType activeSkillType)
            {
                switch (activeSkillType)
                {
                    case ActiveSkillType.BecomeInvisible:
                        return this.becomeInvisible;
                    default:
                        return null;
                }
            }
            public static ActiveSkillType FindActiveSkillType(IActiveSkill ActiveSkill)
            {
                switch (ActiveSkill)
                {
                    case BecomeInvisible:
                        return ActiveSkillType.BecomeInvisible;
                    case UseKnife:
                        return ActiveSkillType.UseKnife;
                    case BeginToCharge:
                        return ActiveSkillType.BeginToCharge;
                    default:
                        return ActiveSkillType.Null;
                }
            }

            public static bool ActiveSkillEffect(IActiveSkill activeSkill, Character player, Action startSkill, Action endSkill)
            {
                lock (activeSkill.ActiveSkillLock)
                {
                    ActiveSkillType activeSkillType = FindActiveSkillType(activeSkill);
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
                                () => player.TimeUntilActiveSkillAvailable[activeSkillType] > 0 && !player.IsResetting,
                                () =>
                                {
                                    player.AddTimeUntilActiveSkillAvailable(activeSkillType, -(int)GameData.frameDuration);
                                },
                                timeInterval: GameData.frameDuration,
                                () => 0,
                                maxTotalDuration: (long)(activeSkill.SkillCD - activeSkill.DurationTime)
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
