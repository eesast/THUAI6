using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;

namespace Server
{
    public class Game
    {
        private const int gameTime = 3000;
        public int GameTime => gameTime;

        private MessageToClient gameInfo = new();
        private object gameInfoLock = new();
        private int isGaming = 0;
        public bool IsGaming
        {
            get => Interlocked.CompareExchange(ref isGaming, 0, 0) != 0;
            set => Interlocked.Exchange(ref isGaming, value ? 1 : 0);
        }

        public MessageToClient GetCopiedGameInfo()
        {
            lock (gameInfoLock)
            {
                return gameInfo.Clone();
            }
        }
        public void AddPlayer(PlayerMsg player)
        {
            lock (gameInfoLock)
            {
                if (player.PlayerType == PlayerType.NullPlayerType)
                    return;
                if (player.PlayerType == PlayerType.HumanPlayer)
                {
                    gameInfo.HumanMessage.Add(new MessageOfHuman()
                    {
                        PlayerId = player.PlayerId
                    });
                    return;
                }
                if (player.PlayerType == PlayerType.ButcherPlayer)
                {
                    gameInfo.ButcherMessage.Add(new MessageOfButcher()
                    {
                        PlayerID = player.PlayerId
                    });
                    return;
                }
            }
        }

        public SemaphoreSlim StartGame()
        {
            IsGaming = true;
            var waitHandle = new SemaphoreSlim(0);

            new Thread
            (
                () =>
                {
                    new FrameRateTaskExecutor<int>
                    (
                        () => IsGaming,
                        () =>
                        {
                            lock (gameInfo)
                            {
                                for (int i = 0; i < gameInfo.HumanMessage.Count; i++)
                                {
                                    if (gameInfo.HumanMessage[i] != null)
                                    {
                                        gameInfo.HumanMessage[i].X++;
                                        gameInfo.HumanMessage[i].Y--;
                                    }
                                }
                                for (int i = 0; i < gameInfo.ButcherMessage.Count; i++)
                                {
                                    if (gameInfo.ButcherMessage[i] != null)
                                    {
                                        gameInfo.ButcherMessage[i].X--;
                                        gameInfo.ButcherMessage[i].Y++;
                                    }
                                }
                            }
                        },
                        100,
                        () =>
                        {
                            IsGaming = false;
                            waitHandle.Release();
                            return 0;
                        },
                        gameTime
                    ).Start();
                }
            )
            { IsBackground = true }.Start();
            return waitHandle;
        }
    }
}
