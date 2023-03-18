using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;
using Gaming;
using GameClass.GameObj;
using Preparation.Utility;
using Playback;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace Server
{
    public class GameServer : AvailableService.AvailableServiceBase
    {
        private Dictionary<long, (SemaphoreSlim, SemaphoreSlim)> semaDict = new();
        protected readonly ArgumentOptions options;
        private HttpSender? httpSender;
        private object gameLock = new();
        private const int playerNum = 1;  // 注意修改
        private MessageToClient currentGameInfo = new();
        private MessageOfObj currentMapMsg = new();
        private object newsLock = new();
        private List<MessageOfNews> currentNews = new();
        private SemaphoreSlim endGameSem = new(0);
        protected readonly Game game;
        private uint spectatorMinPlayerID = 2022;
        private List<uint> spectatorList = new List<uint>();
        public int TeamCount => options.TeamCount;
        protected long[] communicationToGameID;  // 通信用的ID映射到游戏内的ID，通信中0-3为Student，4为Tricker
        private readonly object messageToAllClientsLock = new();
        public static readonly long SendMessageToClientIntervalInMilliseconds = 50;
        private MessageWriter? mwr = null;

        public void StartGame()
        {
            if (game.GameMap.Timer.IsGaming) return;
            /*foreach (var id in communicationToGameID)
            {
                if (id == GameObj.invalidID) return;     //如果有未初始化的玩家，不开始游戏
            }*/ //测试时人数不够
            Console.WriteLine("Game starts!");
            game.StartGame((int)options.GameTimeInSecond * 1000);
            Thread.Sleep(1);
            new Thread(() =>
            {
                bool flag = true;
                new FrameRateTaskExecutor<int>
                (
                    () => game.GameMap.Timer.IsGaming,
                    () =>
                    {
                        if (flag == true)
                        {
                            ReportGame(GameState.GameStart);
                            flag = false;
                        }
                        else ReportGame(GameState.GameRunning);
                    },
                    SendMessageToClientIntervalInMilliseconds,
                    () =>
                    {
                        ReportGame(GameState.GameEnd);  // 最后发一次消息，唤醒发消息的线程，防止发消息的线程由于有概率处在 Wait 状态而卡住
                        OnGameEnd();
                        return 0;
                    }
                ).Start();
            })
            { IsBackground = true }.Start();
        }
        public void WaitForEnd()
        {
            this.endGameSem.Wait();
            mwr?.Dispose();
        }

        private void SaveGameResult(string path)
        {
            Dictionary<string, int> result = new Dictionary<string, int>();
            for (int i = 0; i < TeamCount; i++)
            {
                result.Add("Team" + i.ToString(), GetTeamScore(i)); //Team待修改
            }
            JsonSerializer serializer = new JsonSerializer();
            using (StreamWriter sw = new StreamWriter(path))
            {
                using (JsonWriter writer = new JsonTextWriter(sw))
                {
                    serializer.Serialize(writer, result);
                }
            }
        }
        protected virtual void SendGameResult()		// 天梯的 Server 给网站发消息记录比赛结果
        {
            var scores = new JObject[options.TeamCount];
            for (ushort i = 0; i < options.TeamCount; ++i)
            {
                scores[i] = new JObject { ["team_id"] = i.ToString(), ["score"] = GetTeamScore(i) };
            } // Team待修改
            httpSender?.SendHttpRequest
                (
                    new JObject
                    {
                        ["result"] = new JArray(scores)
                    }
                );
        }

        private void OnGameEnd()
        {
            game.ClearAllLists();
            mwr?.Flush();
            if (options.ResultFileName != DefaultArgumentOptions.FileName)
                SaveGameResult(options.ResultFileName + ".json");
            //SendGameResult();
            this.endGameSem.Release();
        }

        public void ReportGame(GameState gameState, bool requiredGaming = true)
        {
            var gameObjList = game.GetGameObj();
            currentGameInfo = new();
            lock (messageToAllClientsLock)
            {
                switch (gameState)
                {
                    case GameState.GameRunning:
                    case GameState.GameEnd:
                        foreach (GameObj gameObj in gameObjList)
                        {
                            currentGameInfo.ObjMessage.Add(CopyInfo.Auto(gameObj));
                        }
                        lock (newsLock)
                        {
                            foreach (var news in currentNews)
                            {
                                currentGameInfo.ObjMessage.Add(CopyInfo.Auto(news));
                            }
                            currentNews.Clear();
                        }
                        currentGameInfo.GameState = gameState;
                        currentGameInfo.AllMessage = new(); // 还没写
                        mwr?.WriteOne(currentGameInfo);
                        break;
                    case GameState.GameStart:
                        currentGameInfo.ObjMessage.Add(currentMapMsg);
                        foreach (GameObj gameObj in gameObjList)
                        {
                            currentGameInfo.ObjMessage.Add(CopyInfo.Auto(gameObj));
                        }
                        lock (newsLock)
                        {
                            foreach (var news in currentNews)
                            {
                                currentGameInfo.ObjMessage.Add(CopyInfo.Auto(news));
                            }
                            currentNews.Clear();
                        }
                        currentGameInfo.GameState = gameState;
                        currentGameInfo.AllMessage = new(); // 还没写
                        mwr?.WriteOne(currentGameInfo);
                        break;
                    default:
                        break;
                }
            }

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item1.Release();
            }

            foreach (var kvp in semaDict)
            {
                kvp.Value.Item2.Wait();
            }
        }
        public int GetTeamScore(long teamID)
        {
            return game.GetTeamScore(teamID);
        }

        private int PlayerIDToTeamID(long playerID)
        {
            if (0 <= playerID && playerID < options.PlayerCountPerTeam) return 0;
            if (playerID == options.PlayerCountPerTeam) return 1;
            return -1;
        }
        private uint GetBirthPointIdx(long playerID)  // 获取出生点位置
        {
            return (uint)playerID + 1; // ID从0-4,出生点从1-5
        }
        private bool ValidPlayerID(long playerID)
        {
            if (0 <= playerID && playerID < options.PlayerCountPerTeam + 1)
                return true;
            return false;
        }

        private Protobuf.PlaceType IntToPlaceType(uint n)
        {
            switch (n)
            {
                case 0: return Protobuf.PlaceType.Land;
                case 6: return Protobuf.PlaceType.Wall;
                case 7: return Protobuf.PlaceType.Grass;
                case 8: return Protobuf.PlaceType.Classroom;
                case 9: return Protobuf.PlaceType.Gate;
                case 10: return Protobuf.PlaceType.HiddenGate;
                case 11: return Protobuf.PlaceType.Window;
                case 12: return Protobuf.PlaceType.Door;
                case 13: return Protobuf.PlaceType.Chest;
                default: return Protobuf.PlaceType.NullPlaceType;
            }
        }
        private MessageOfObj MapMsg(uint[,] map)
        {
            MessageOfObj msgOfMap = new();
            msgOfMap.MapMessage = new();
            for (int i = 0; i < GameData.rows; i++)
            {
                msgOfMap.MapMessage.Row.Add(new MessageOfMap.Types.Row());
                for (int j = 0; j < GameData.cols; j++)
                {
                    msgOfMap.MapMessage.Row[i].Col.Add(IntToPlaceType(map[i, j]));
                }
            }
            return msgOfMap;
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
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                // 观战模式
                uint tp = (uint)request.PlayerId;
                if (!spectatorList.Contains(tp))
                {
                    spectatorList.Add(tp);
                    Console.WriteLine("A new spectator comes to watch this game.");
                }
                return;
            }

            if (game.GameMap.Timer.IsGaming)
                return;
            if (!ValidPlayerID(request.PlayerId))  //玩家id是否正确
                return;
            if (communicationToGameID[request.PlayerId] != GameObj.invalidID)  //是否已经添加了该玩家
                return;

            Preparation.Utility.CharacterType characterType = Preparation.Utility.CharacterType.Null; // 待修改
            if (request.PlayerType == PlayerType.StudentPlayer)
            {
                switch (request.StudentType)
                {
                    default:
                        characterType = Preparation.Utility.CharacterType.Athlete;
                        break;
                }
            }
            else if (request.PlayerType == PlayerType.TrickerPlayer)
            {
                switch (request.TrickerType)
                {
                    default:
                        characterType = Preparation.Utility.CharacterType.Assassin;
                        break;
                }
            }

            lock (addPlayerLock)
            {
                Game.PlayerInitInfo playerInitInfo = new(GetBirthPointIdx(request.PlayerId), PlayerIDToTeamID(request.PlayerId), request.PlayerId, characterType);
                long newPlayerID = game.AddPlayer(playerInitInfo);
                if (newPlayerID == GameObj.invalidID)
                    return;
                communicationToGameID[request.PlayerId] = newPlayerID;
                // 内容待修改
                var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));
                bool start = false;
                Console.WriteLine($"Id: {request.PlayerId} joins.");
                lock (semaDict)
                {
                    semaDict.Add(request.PlayerId, temp);
                    start = semaDict.Count == playerNum;
                }
                if (start) StartGame();
            }

            do
            {
                semaDict[request.PlayerId].Item1.Wait();
                if (currentGameInfo != null)
                {
                    await responseStream.WriteAsync(currentGameInfo);
                    //Console.WriteLine("Send!");
                }
                semaDict[request.PlayerId].Item2.Release();
            } while (game.GameMap.Timer.IsGaming);
        }

        public override Task<BoolRes> Attack(AttackMsg request, ServerCallContext context)
        {
            var gameID = communicationToGameID[request.PlayerId];
            game.Attack(gameID, request.Angle);
            BoolRes boolRes = new();
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }


        public override Task<MoveRes> Move(MoveMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"Move ID: {request.PlayerId}, TimeInMilliseconds: {request.TimeInMilliseconds}");
#endif            
            var gameID = communicationToGameID[request.PlayerId];
            MoveRes moveRes = new();
            game.MovePlayer(gameID, (int)request.TimeInMilliseconds, request.Angle);
            // 之后game.MovePlayer可能改为bool类
            moveRes.ActSuccess = true;
            if (!game.GameMap.Timer.IsGaming) moveRes.ActSuccess = false;
            return Task.FromResult(moveRes);
        }

        public override Task<BoolRes> PickProp(PropMsg request, ServerCallContext context)
        {
            BoolRes boolRes = new();
            var gameID = communicationToGameID[request.PlayerId];
            if (request.PropType == Protobuf.PropType.NullPropType)
                boolRes.ActSuccess = game.PickProp(gameID, Preparation.Utility.PropType.Null);
            // 待修改
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> SendMessage(SendMsg request, ServerCallContext context)
        {
            var boolRes = new BoolRes();
            if (!ValidPlayerID(request.PlayerId) || !ValidPlayerID(request.ToPlayerId)
                || PlayerIDToTeamID(request.PlayerId) != PlayerIDToTeamID(request.ToPlayerId) || request.PlayerId == request.ToPlayerId)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            if (request.Message.Length > 256)
            {
#if DEBUG
                Console.WriteLine("Message string is too long!");
#endif
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            else
            {
                MessageOfNews news = new();
                news.News = request.Message;
                news.FromId = request.PlayerId;
                news.ToId = request.ToPlayerId;
                lock (newsLock)
                {
                    currentNews.Add(news);
                }
#if DEBUG
                Console.WriteLine(news.News);
#endif
                //teamCommunicatonMsg[request.ToPlayerId].Enqueue(msg);
            }
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> UseProp(PropMsg request, ServerCallContext context)
        {
            return base.UseProp(request, context);
        }
        public override Task<BoolRes> UseSkill(SkillMsg request, ServerCallContext context)
        {
            return base.UseSkill(request, context);
        }

        public override Task<BoolRes> Graduate(IDMsg request, ServerCallContext context)
        {
            return base.Graduate(request, context);
        }
        public override Task<BoolRes> StartRescueMate(IDMsg request, ServerCallContext context)
        {
            return base.StartRescueMate(request, context);
        }
        public override Task<BoolRes> StartTreatMate(IDMsg request, ServerCallContext context)
        {
            return base.StartTreatMate(request, context);
        }
        public override Task<BoolRes> StartLearning(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartLearning ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.Fix(gameID);
            return Task.FromResult(boolRes);
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
            currentMapMsg = MapMsg(game.GameMap.ProtoGameMap);
            communicationToGameID = new long[options.PlayerCountPerTeam + 1];
            //创建server时先设定待加入人物都是invalid
            for (int i = 0; i < communicationToGameID.GetLength(0); i++)
            {
                communicationToGameID[i] = GameObj.invalidID;
            }

            if (options.FileName != DefaultArgumentOptions.FileName)
            {
                try
                {
                    mwr = new MessageWriter(options.FileName, options.TeamCount, options.PlayerCountPerTeam);
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
