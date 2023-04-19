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
                                if (player.Commandable() && player.PlayerState != PlayerStateType.Fixing) activeSkill.DegreeOfMeditation += learningDegree * GameData.frameDuration;
                                else activeSkill.DegreeOfMeditation = 0;
                                //Debugger.Output(player, "with " + (((WriteAnswers)activeSkill).DegreeOfMeditation).ToString());
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
        }
    }
}