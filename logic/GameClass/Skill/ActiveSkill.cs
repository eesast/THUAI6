using GameClass.GameObj;
using System.Threading;
using Preparation.Interface;
using Preparation.Utility;
using System;
using Timothy.FrameRateTask;

namespace GameClass.Skill
{
    public class BecomeVampire : IActiveSkill  // 化身吸血鬼
    {
        private const int moveSpeed = GameData.basicMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = (int)(GameData.basicHp / 6 * 9.5);
        public int MaxHp => maxHp;

        // 以上参数以后再改
        public int SkillCD => GameData.commonSkillCD / 3 * 4;
        public int DurationTime => GameData.commonSkillTime;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;

        public bool SkillEffect(Character player)
        {
            return ActiveSkillFactory.SkillEffect(this, player, () =>
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
    public class BecomeInvisible : IActiveSkill
    {
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 6;

        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
        public bool SkillEffect(Character player)
        {
            return ActiveSkillFactory.SkillEffect(this, player, () =>
                                                                {
                                                                    player.IsInvisible = true;
                                                                    Debugger.Output(player, "uses atombomb!");
                                                                },
                                                  () =>
                                                  { player.IsInvisible = false; });
        }
    }
    public class NuclearWeapon : IActiveSkill  // 核武器
    {
        private const int moveSpeed = GameData.basicMoveSpeed / 3 * 4;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        private const int maxBulletNum = GameData.basicBulletNum * 2 / 3;
        public int MaxBulletNum => maxBulletNum;
        // 以上参数以后再改
        public int SkillCD => GameData.commonSkillCD / 3 * 7;
        public int DurationTime => GameData.commonSkillTime / 10;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
        public bool SkillEffect(Character player)
        {
            return ActiveSkillFactory.SkillEffect(this, player, () =>
                                                                {
                                                                    player.BulletOfPlayer = BulletType.AtomBomb;
                                                                    Debugger.Output(player, "uses atombomb!");
                                                                },
                                                  () =>
                                                  { player.BulletOfPlayer = player.OriBulletOfPlayer; });
        }
    }
    public class SuperFast : IActiveSkill  // 3倍速
    {
        private const int moveSpeed = GameData.basicMoveSpeed * 4 / 3;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp / 6 * 4;
        public int MaxHp => maxHp;

        private const int maxBulletNum = GameData.basicBulletNum * 4 / 3;
        public int MaxBulletNum => maxBulletNum;
        // 以上参数以后再改
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime / 10 * 4;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
        public bool SkillEffect(Character player)
        {
            return ActiveSkillFactory.SkillEffect(this, player, () =>
                                                                {
                                                                    player.AddMoveSpeed(this.DurationTime, 3.0);
                                                                    Debugger.Output(player, "moves very fast!");
                                                                },
                                                  () =>
                                                  { });
        }
    }
    public class NoCommonSkill : IActiveSkill  // 这种情况不该发生，定义着以防意外
    {
        private const int moveSpeed = GameData.basicMoveSpeed;
        public int MoveSpeed => moveSpeed;

        private const int maxHp = GameData.basicHp;
        public int MaxHp => maxHp;

        private const int maxBulletNum = GameData.basicBulletNum;
        public int MaxBulletNum => maxBulletNum;
        // 以上参数以后再改
        public int SkillCD => GameData.commonSkillCD;
        public int DurationTime => GameData.commonSkillTime;
        private readonly object commonSkillLock = new object();
        public object ActiveSkillLock => commonSkillLock;
        public bool SkillEffect(Character player)
        {
            return false;
        }
    }

    public static class ActiveSkillFactory
    {
        public static bool SkillEffect(IActiveSkill activeSkill, Character player, Action startSkill, Action endSkill)
        {
            lock (activeSkill.ActiveSkillLock)
            {
                if (player.TimeUntilCommonSkillAvailable == 0)
                {
                    player.TimeUntilCommonSkillAvailable = activeSkill.SkillCD;
                    new Thread
                    (() =>
                    {
                        startSkill();
                        new FrameRateTaskExecutor<int>(
                            () => !player.IsResetting,
                            () =>
                            {
                                player.TimeUntilCommonSkillAvailable -= (int)GameData.frameDuration;
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
                            () => player.TimeUntilCommonSkillAvailable > 0 && !player.IsResetting,
                            () =>
                            {
                                player.TimeUntilCommonSkillAvailable -= (int)GameData.frameDuration;
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

                        player.TimeUntilCommonSkillAvailable = 0;
                        Debugger.Output(player, "CommonSkill is ready.");
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
    public static IActiveSkill FindIActiveSkill(ActiveSkillType activeSkillType) 
    {
            switch (activeSkillType)
            {
                case ActiveSkillType.BecomeInvisible:
                    return new BecomeInvisible();
                default:
                    return null;
            }
    }
    }

}
