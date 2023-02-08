using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;

namespace Server
{
    public class GameServer : AvailableService.AvailableServiceBase
    {
        private Dictionary<long, (SemaphoreSlim, SemaphoreSlim)> semaDict = new();
        private object gameLock = new();
        private const int playerNum = 1;
        private MessageToClient currentGameInfo = new();
        private Game game = new();
        public int GameTime => game.GameTime;
        private SemaphoreSlim endGameSem = new(0);

        public override Task<BoolRes> TryConnection(IDMsg request, ServerCallContext context)
        {
            var onConnection = new BoolRes();
            lock (gameLock)
            {
                // if (0 <= request.PlayerId && request.PlayerId < playerNum)
                {
                    onConnection.ActSuccess = true;
                    Console.WriteLine(onConnection.ActSuccess);
                    return Task.FromResult(onConnection);
                }
            }
            onConnection.ActSuccess = false;
            return Task.FromResult(onConnection);
        }

        public override async Task AddPlayer(PlayerMsg request, IServerStreamWriter<MessageToClient> responseStream, ServerCallContext context)
        {
            lock (gameLock)
            {
                if (game.IsGaming)
                    return;
                game.AddPlayer(request);
                var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));

                bool start = false;
                Console.WriteLine($"Id: {request.PlayerId} joins.");
                lock (semaDict)
                {
                    semaDict.Add(request.PlayerId, temp);
                    start = semaDict.Count == playerNum;
                }

                if (start)
                {
                    Console.WriteLine("Game starts!");
                    StartGame();
                }
            }

            do
            {
                semaDict[request.PlayerId].Item1.Wait();
                if (currentGameInfo != null)
                {
                    await responseStream.WriteAsync(currentGameInfo);
                    Console.WriteLine("Send!");
                }
                semaDict[request.PlayerId].Item2.Release();
            } while (game.IsGaming);
        }

        public override Task<BoolRes> Attack(AttackMsg request, ServerCallContext context)
        {
            return base.Attack(request, context);
        }

        public override Task<BoolRes> CarryHuman(IDMsg request, ServerCallContext context)
        {
            return base.CarryHuman(request, context);
        }

        public override Task<BoolRes> EndFixMachine(IDMsg request, ServerCallContext context)
        {
            return base.EndFixMachine(request, context);
        }

        public override Task<BoolRes> EndSaveHuman(IDMsg request, ServerCallContext context)
        {
            return base.EndSaveHuman(request, context);
        }

        public override Task<BoolRes> Escape(IDMsg request, ServerCallContext context)
        {
            return base.Escape(request, context);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override Task GetMessage(IDMsg request, IServerStreamWriter<MsgRes> responseStream, ServerCallContext context)
        {
            return base.GetMessage(request, responseStream, context);
        }

        public override Task<BoolRes> HangHuman(IDMsg request, ServerCallContext context)
        {
            return base.HangHuman(request, context);
        }

        public override Task<MoveRes> Move(MoveMsg request, ServerCallContext context)
        {
            return base.Move(request, context);
        }

        public override Task<BoolRes> PickProp(PickMsg request, ServerCallContext context)
        {
            return base.PickProp(request, context);
        }

        public override Task<BoolRes> ReleaseHuman(IDMsg request, ServerCallContext context)
        {
            return base.ReleaseHuman(request, context);
        }

        public override Task<BoolRes> SendMessage(SendMsg request, ServerCallContext context)
        {
            return base.SendMessage(request, context);
        }

        public override Task<BoolRes> StartFixMachine(IDMsg request, ServerCallContext context)
        {
            return base.StartFixMachine(request, context);
        }

        public override Task<BoolRes> StartSaveHuman(IDMsg request, ServerCallContext context)
        {
            return base.StartSaveHuman(request, context);
        }

        public override Task<BoolRes> UseProp(IDMsg request, ServerCallContext context)
        {
            return base.UseProp(request, context);
        }

        public override Task<BoolRes> UseSkill(IDMsg request, ServerCallContext context)
        {
            return base.UseSkill(request, context);
        }
        public void StartGame()
        {
            var waitHandle = game.StartGame();
            new Thread(() =>
            {
                new FrameRateTaskExecutor<int>
                (
                    () => game.IsGaming,
                    ReportGame,
                    1000,
                    () =>
                    {
                        ReportGame();  // 最后发一次消息，唤醒发消息的线程，防止发消息的线程由于有概率处在 Wait 状态而卡住
                        return 0;
                    }
                ).Start();
            })
            { IsBackground = true }.Start();
            new Thread(() =>
            {
                waitHandle.Wait();
                this.endGameSem.Release();
            })
            { IsBackground = true }.Start();

        }
        public void WaitForEnd()
        {
            this.endGameSem.Wait();
        }

        public void ReportGame()
        {
            currentGameInfo = game.GetCopiedGameInfo();

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item1.Release();
            }

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item2.Wait();
            }
        }

        public GameServer()
        {
        }
    }
}