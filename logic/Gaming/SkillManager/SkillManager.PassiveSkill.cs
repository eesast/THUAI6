using System;
using System.Reflection.Emit;
using System.Threading;
using GameClass.GameObj;
using Preparation.Interface;
using Preparation.Utility;
using Timothy.FrameRateTask;

namespace Gaming  // 被动技能开局时就释放，持续到游戏结束
{
    public partial class Game
    {
        private partial class SkillManager
        {
            public void Meditate(Character player)
            {
                const int learningDegree = GameData.basicFixSpeed / 4;
                WriteAnswers activeSkill = (WriteAnswers)player.FindIActiveSkill(ActiveSkillType.WriteAnswers);
                new Thread
                (
                    () =>
                    {
                        new FrameRateTaskExecutor<int>
                        (
                            () => gameMap.Timer.IsGaming && !player.IsResetting,
                            () =>
                            {
                                if (player.Commandable()) activeSkill.DegreeOfMeditation += learningDegree;
                                else activeSkill.DegreeOfMeditation = 0;
                                Debugger.Output(player, "with " + (((WriteAnswers)activeSkill).DegreeOfMeditation).ToString());

                            },
                            timeInterval: GameData.frameDuration,
                            () => 0,
                            maxTotalDuration: GameData.gameDuration
                        )
                        {
                            AllowTimeExceed = true,
                            MaxTolerantTimeExceedCount = ulong.MaxValue,
                            TimeExceedAction = b =>
                            {
                                if (b)
                                    Console.WriteLine("Fetal Error: The computer runs so slow that passive skill time exceeds!!!!!!");

#if DEBUG
                                else
                                {
                                    Console.WriteLine("Debug info: passive skill time exceeds for once.");
                                }
#endif
                            }
                        }.Start();
                    }
                )
                { IsBackground = true }.Start();
            }
            public void RecoverAfterBattle(Character player)
            {
                const int recoverDegree = 5;  // 每帧回复血量
                int nowHP = player.HP;
                int lastHP = nowHP;
                long waitTime = 0;
                const long interval = 10000;  // 每隔interval时间不受伤害，角色即开始回血
                new Thread
                (
                    () =>
                    {
                        new FrameRateTaskExecutor<int>
                        (
                            () => true,
                            () =>
                            {
                                lastHP = nowHP;      // lastHP等于上一帧的HP
                                nowHP = player.HP;   // nowHP更新为这一帧的HP
                                if (lastHP > nowHP)  // 这一帧扣血了
                                {
                                    waitTime = 0;
                                }
                                else if (waitTime < interval)
                                {
                                    waitTime += GameData.frameDuration;
                                }

                                if (waitTime >= interval)  // 回复时，每帧(50ms)回复5，即1s回复100。
                                    player.TryAddHp(recoverDegree);
                            },
                            timeInterval: GameData.frameDuration,
                            () => 0,
                            maxTotalDuration: GameData.gameDuration
                        )
                        {
                            AllowTimeExceed = true,
                            MaxTolerantTimeExceedCount = ulong.MaxValue,
                            TimeExceedAction = b =>
                            {
                                if (b)
                                    Console.WriteLine("Fetal Error: The computer runs so slow that passive skill time exceeds!!!!!!");

#if DEBUG
                                else
                                {
                                    Console.WriteLine("Debug info: passive skill time exceeds for once.");
                                }
#endif
                            }
                        }.Start();
                    }
                )
                { IsBackground = true }.Start();
            }
            public void SpeedUpWhenLeavingGrass(Character player)
            {
                PlaceType nowPlace = gameMap.GetPlaceType(player.Position);
                PlaceType lastPlace = nowPlace;
                bool speedup = false;
                const int SpeedUpTime = 2000;  // 加速时间：2s
                new Thread
                (
                    () =>
                    {
                        new FrameRateTaskExecutor<int>
                        (
                            () => true,
                            () =>
                            {
                                lastPlace = nowPlace;
                                nowPlace = gameMap.GetPlaceType(player.Position);
                                if ((lastPlace == PlaceType.Grass) && nowPlace == PlaceType.Null)
                                {
                                    if (!speedup)
                                    {
                                        new Thread(() =>
                                        {
                                            speedup = true;
                                            player.AddMoveSpeed(SpeedUpTime, 3.0);
                                            speedup = false;
                                        })
                                        { IsBackground = true }.Start();
                                    }
                                }
                            },
                            timeInterval: GameData.frameDuration,
                            () => 0,
                            maxTotalDuration: GameData.gameDuration
                        )
                        {
                            AllowTimeExceed = true,
                            MaxTolerantTimeExceedCount = ulong.MaxValue,
                            TimeExceedAction = b =>
                            {
                                if (b)
                                    Console.WriteLine("Fetal Error: The computer runs so slow that passive skill time exceeds!!!!!!");

#if DEBUG
                                else
                                {
                                    Console.WriteLine("Debug info: passive skill time exceeds for once.");
                                }
#endif
                            }
                        }.Start();
                    }
                )
                { IsBackground = true }.Start();
            }

            public void Vampire(Character player)
            {
                player.OriVampire = 0.5;
                player.Vampire = player.OriVampire;
            }
        }
    }
}