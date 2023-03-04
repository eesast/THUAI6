using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;
using Gaming;
using GameClass.GameObj;
using Preparation.Utility;


namespace Server
{
    public class GameServer : AvailableService.AvailableServiceBase
    {
        private Dictionary<long, (SemaphoreSlim, SemaphoreSlim)> semaDict = new();
        private object gameLock = new();
        private const int playerNum = 1;  // 注意修改
        private MessageToClient currentGameInfo = new();
        private SemaphoreSlim endGameSem = new(0);

        object gameInfo = new();
        private int isGaming = 0;
        public bool IsGaming
        {
            get => Interlocked.CompareExchange(ref isGaming, 0, 0) != 0;
            set => Interlocked.Exchange(ref isGaming, value ? 1 : 0);
        }
        // 以上是测试时用到的定义

        protected readonly ArgumentOptions options;
        protected readonly Game game;
        private uint spectatorMinPlayerID = 2022;
        private List<Tuple<PlayerType, uint>> spectatorList = new List<Tuple<PlayerType, uint>>();
        public int TeamCount => options.TeamCount;
        protected long[,] communicationToGameID;  // 通信用的ID映射到游戏内的ID,[i,j]表示team：i，player：j的id。
        private readonly object messageToAllClientsLock = new();
        public static readonly long SendMessageToClientIntervalInMilliseconds = 50;
        private readonly Semaphore endGameInfoSema = new(0, 1);
        // private MessageWriter? mwr = null;

        public SemaphoreSlim StartGameTest()
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
                                /*for (int i = 0; i < gameInfo.GameObjs.Count; i++)
                                {
                                    if (gameInfo.GameObjs[i].Character != null)
                                    {
                                        gameInfo.GameObjs[i].Character.X++;
                                        gameInfo.GameObjs[i].Character.Y--;
                                    }
                                }*/
                            }
                        },
                        100,
                        () =>
                        {
                            IsGaming = false;
                            waitHandle.Release();
                            return 0;
                        },
                        3000//gameTime
                    ).Start();
                }
            )
            { IsBackground = true }.Start();
            return waitHandle;
        }
        public void StartGame()
        {
            bool gameState = game.StartGame((int)options.GameTimeInSecond * 1000);
            var waitHandle = new SemaphoreSlim(gameState == true ? 1 : 0);  // 注意修改
            new Thread(() =>
            {
                new FrameRateTaskExecutor<int>
                (
                    () => game.GameMap.Timer.IsGaming,
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
            //currentGameInfo = null;

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item1.Release();
            }

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item2.Wait();
            }
        }

        private int PlayerTypeToTeamID(PlayerType playerType)
        {
            if (playerType == PlayerType.StudentPlayer) return 0;
            if (playerType == PlayerType.TrickerPlayer) return 1;
            return -1;
        }
        private uint GetBirthPointIdx(PlayerType playerType, long playerID)  // 获取出生点位置
        {
            return (uint)((PlayerTypeToTeamID(playerType) * options.PlayerCountPerTeam) + playerID + 1);
        }
        private bool ValidPlayerTypeAndPlayerID(PlayerType playerType, long playerID)
        {
            if (playerType == PlayerType.StudentPlayer && 0 <= playerID && playerID < options.PlayerCountPerTeam)
                return true; // 人数待修改
            if (playerType == PlayerType.TrickerPlayer && 0 <= playerID && playerID < options.PlayerCountPerTeam)
                return true;
            return false;
        }

        public override Task<BoolRes> TryConnection(IDMsg request, ServerCallContext context)
        {
            Console.WriteLine($"TryConnection ID: {request.PlayerId}");
            var onConnection = new BoolRes();
            lock (gameLock)
            {
                if (0 <= request.PlayerId && request.PlayerId < playerNum)  // 注意修改
                {
                    onConnection.ActSuccess = true;
                    Console.WriteLine(onConnection.ActSuccess);
                    return Task.FromResult(onConnection);
                }
            }
            onConnection.ActSuccess = false;
            return Task.FromResult(onConnection);
        }

        protected readonly object addPlayerLock = new();
        public override async Task AddPlayer(PlayerMsg request, IServerStreamWriter<MessageToClient> responseStream, ServerCallContext context)
        {
            Console.WriteLine($"AddPlayer: {request.PlayerId}");
            if (request.PlayerId >= spectatorMinPlayerID && request.PlayerType == PlayerType.NullPlayerType)
            {
                // 观战模式
                Tuple<PlayerType, uint> tp = new Tuple<PlayerType, uint>(request.PlayerType, (uint)request.PlayerId);
                if (!spectatorList.Contains(tp))
                {
                    spectatorList.Add(tp);
                    Console.WriteLine("A new spectator comes to watch this game.");
                }
                return;
            }

            if (game.GameMap.Timer.IsGaming)
                return;
            if (!ValidPlayerTypeAndPlayerID(request.PlayerType, request.PlayerId))  //玩家id是否正确
                return;
            //if (communicationToGameID[PlayerTypeToTeamID(request.PlayerType), request.PlayerId] != GameObj.invalidID)  //是否已经添加了该玩家
            //return;

            Preparation.Utility.CharacterType characterType = Preparation.Utility.CharacterType.Athlete; // 待修改

            lock (addPlayerLock)
            {
                Game.PlayerInitInfo playerInitInfo = new(GetBirthPointIdx(request.PlayerType, request.PlayerId), PlayerTypeToTeamID(request.PlayerType), request.PlayerId, characterType);
                long newPlayerID = game.AddPlayer(playerInitInfo);
                //if (newPlayerID == GameObj.invalidID)
                //return;
                // 内容待修改
                var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));
                bool start = false;
                Console.WriteLine($"PlayerType: {request.PlayerType} Id: {request.PlayerId} joins.");
                lock (semaDict)
                {
                    semaDict.Add(request.PlayerId, temp);
                    start = semaDict.Count == playerNum;  // 之后补上CheckStart函数
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
            } while (game.GameMap.Timer.IsGaming);
        }

        public override Task<BoolRes> Trick(TrickMsg request, ServerCallContext context)
        {
            game.Attack(request.PlayerId, request.Angle);
            BoolRes boolRes = new();
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }

        public override Task GetMessage(IDMsg request, IServerStreamWriter<MsgRes> responseStream, ServerCallContext context)
        {
            return base.GetMessage(request, responseStream, context);
        }

        public override Task<MoveRes> Move(MoveMsg request, ServerCallContext context)
        {
            Console.WriteLine($"Move ID: {request.PlayerId}, TimeInMilliseconds: {request.TimeInMilliseconds}");
            game.MovePlayer(request.PlayerId, (int)request.TimeInMilliseconds, request.Angle);
            // 之后game.MovePlayer可能改为bool类型
            MoveRes moveRes = new();
            moveRes.ActSuccess = true;
            return Task.FromResult(moveRes);
        }

        public override Task<BoolRes> PickProp(PropMsg request, ServerCallContext context)
        {
            BoolRes boolRes = new();
            if (request.PropType == Protobuf.PropType.NullPropType)
                boolRes.ActSuccess = game.PickProp(request.PlayerId, Preparation.Utility.PropType.Null);
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> SendMessage(SendMsg request, ServerCallContext context)
        {
            return base.SendMessage(request, context);
        }

        public override Task<BoolRes> UseProp(IDMsg request, ServerCallContext context)
        {
            return base.UseProp(request, context);
        }

        public override Task<BoolRes> UseSkill(IDMsg request, ServerCallContext context)
        {
            return base.UseSkill(request, context);
        }

        public override Task<BoolRes> Graduate(IDMsg request, ServerCallContext context)
        {
            return base.Graduate(request, context);
        }
        public override Task<BoolRes> StartHealMate(IDMsg request, ServerCallContext context)
        {
            return base.StartHealMate(request, context);
        }
        public override Task<BoolRes> StartHelpMate(IDMsg request, ServerCallContext context)
        {
            return base.StartHelpMate(request, context);
        }
        public override Task<BoolRes> StartLearning(IDMsg request, ServerCallContext context)
        {
            return base.StartLearning(request, context);
        }

        public GameServer(ArgumentOptions options)
        {
            this.options = options;
            //if (options.mapResource == DefaultArgumentOptions.MapResource)
            //    this.game = new Game(MapInfo.defaultMap, options.TeamCount);
            //else
            {
                uint[,] map = new uint[GameData.rows, GameData.cols];
                try
                {
                    string? line;
                    int i = 0, j = 0;
                    using (StreamReader sr = new StreamReader(options.mapResource))
                    {
                        while (!sr.EndOfStream && i < GameData.rows)
                        {
                            if ((line = sr.ReadLine()) != null)
                            {
                                string[] nums = line.Split(' ');
                                foreach (string item in nums)
                                {
                                    if (item.Length > 1)//以兼容原方案
                                    {
                                        map[i, j] = (uint)int.Parse(item);
                                    }
                                    else
                                    {
                                        //2022-04-22 by LHR 十六进制编码地图方案（防止地图编辑员瞎眼x
                                        map[i, j] = (uint)Preparation.Utility.MapEncoder.Hex2Dec(char.Parse(item));
                                    }
                                    j++;
                                    if (j >= GameData.cols)
                                    {
                                        j = 0;
                                        break;
                                    }
                                }
                                i++;
                            }
                        }
                    }
                }
                catch
                {
                    map = MapInfo.defaultMap;
                }
                finally { this.game = new Game(map, options.TeamCount); }
            }
            communicationToGameID = new long[options.TeamCount, options.PlayerCountPerTeam];
            //创建server时先设定待加入人物都是invalid
            for (int i = 0; i < communicationToGameID.GetLength(0); i++)
            {
                for (int j = 0; j < communicationToGameID.GetLength(1); j++)
                {
                    communicationToGameID[i, j] = GameObj.invalidID;
                }
            }

            if (options.FileName != DefaultArgumentOptions.FileName)
            {
                try
                {
                    //mwr = new MessageWriter(options.FileName, options.TeamCount, options.PlayerCountPerTeam);
                }
                catch
                {
                    Console.WriteLine($"Error: Cannot create the playback file: {options.FileName}!");
                }
            }

            if (options.Url != DefaultArgumentOptions.Url && options.Token != DefaultArgumentOptions.Token)
            {
                //this.httpSender = new HttpSender(options.Url, options.Token, "PUT");
            }
        }
    }
}
