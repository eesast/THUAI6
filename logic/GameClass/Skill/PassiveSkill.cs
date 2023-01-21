using System;
using System.Threading;
using GameClass.GameObj;
using Preparation.GameData;
using Preparation.Interface;
using Preparation.Utility;
using Timothy.FrameRateTask;

namespace GameClass.Skill  // 被动技能开局时就释放，持续到游戏结束
{
    public class RecoverAfterBattle : IPassiveSkill  // 脱战回血，普通子弹
    {
        private readonly BulletType initBullet = BulletType.OrdinaryBullet;
        public BulletType InitBullet => initBullet;
        // 以上参数以后再改
        public void SkillEffect(Character player)
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
    }
    public class SpeedUpWhenLeavingGrass : IPassiveSkill  // 3倍速
    {
        private readonly BulletType initBullet = BulletType.FastBullet;
        public BulletType InitBullet => initBullet;
        // 以上参数以后再改
        public void SkillEffect(Character player)
        {
            PlaceType nowPlace = player.Place;
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
                            nowPlace = player.Place;
                            if ((lastPlace == PlaceType.Grass1 || lastPlace == PlaceType.Grass2 || lastPlace == PlaceType.Grass3) && nowPlace == PlaceType.Land)
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
    }
    public class Vampire : IPassiveSkill  // 被动就是吸血，普通子弹
    {
        private readonly BulletType initBullet = BulletType.LineBullet;
        public BulletType InitBullet => initBullet;
        // 以上参数以后再改
        public void SkillEffect(Character player)
        {
            player.OriVampire = 0.5;
            player.Vampire = player.OriVampire;
        }
    }

    public class NoPassiveSkill : IPassiveSkill  // 没技能，这种情况不应该发生，先定义着以防意外
    {
        private readonly BulletType initBullet = BulletType.OrdinaryBullet;
        public BulletType InitBullet => initBullet;
        // 以上参数以后再改
        public void SkillEffect(Character player)
        {
        }
    }
}
