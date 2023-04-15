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
using Preparation.Interface;


namespace Server
{
    public partial class GameServer : AvailableService.AvailableServiceBase
    {
        private Dictionary<long, (SemaphoreSlim, SemaphoreSlim)> semaDict = new();
        private object semaDictLock = new();
        protected readonly ArgumentOptions options;
        private HttpSender? httpSender;
        private object gameLock = new();
        private MessageToClient currentGameInfo = new();
        private MessageOfObj currentMapMsg = new();
        private object newsLock = new();
        private List<MessageOfNews> currentNews = new();
        private SemaphoreSlim endGameSem = new(0);
        protected readonly Game game;
        private uint spectatorMinPlayerID = 2023;
        private List<uint> spectatorList = new List<uint>();
        public int playerNum;
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
                            game.AllPlayerUsePassiveSkill();
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
            int[] score = GetScore();
            result.Add("Student", score[0]);
            result.Add("Tricker", score[1]);
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
            int[] score = GetScore();
            scores[0] = new JObject { ["team_name"] = "Student", ["score"] = score[0] };
            scores[1] = new JObject { ["team_name"] = "Tricker", ["score"] = score[1] };
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
            SendGameResult();
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
                    case GameState.GameStart:
                        if (gameState == GameState.GameStart || IsSpectatorJoin)
                        {
                            currentGameInfo.ObjMessage.Add(currentMapMsg);
                            IsSpectatorJoin = false;
                        }
                        int time = game.GameMap.Timer.nowTime();
                        foreach (GameObj gameObj in gameObjList)
                        {
                            MessageOfObj? msg = CopyInfo.Auto(gameObj, time);
                            if (msg != null) currentGameInfo.ObjMessage.Add(msg);
                        }
                        lock (newsLock)
                        {
                            foreach (var news in currentNews)
                            {
                                MessageOfObj? msg = CopyInfo.Auto(news);
                                if (msg != null) currentGameInfo.ObjMessage.Add(msg);
                            }
                            currentNews.Clear();
                        }
                        currentGameInfo.GameState = gameState;
                        currentGameInfo.AllMessage = GetMessageOfAll(time);
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
        public int[] GetScore()
        {
            int[] score = new int[2]; // 0代表Student，1代表Tricker
            game.GameMap.GameObjLockDict[GameObjType.Character].EnterReadLock();
            try
            {
                foreach (Character character in game.GameMap.GameObjDict[GameObjType.Character])
                {
                    if (!character.IsGhost()) score[0] += character.Score;
                    else score[1] += character.Score;
                }

            }
            finally
            {
                game.GameMap.GameObjLockDict[GameObjType.Character].ExitReadLock();
            }
            return score;
        }

        private int PlayerIDToTeamID(long playerID)
        {
            if (0 <= playerID && playerID < options.StudentCount) return 0;
            if (options.MaxStudentCount <= playerID && playerID < options.MaxStudentCount + options.TrickerCount) return 1;
            return -1;
        }

        private int PlayerTypeToTeamID(Protobuf.PlayerType playerType)
        {
            if (playerType == PlayerType.StudentPlayer) return 0;
            if (playerType == PlayerType.TrickerPlayer) return 1;
            return -1;
        }
        private uint GetBirthPointIdx(long playerID)  // 获取出生点位置
        {
            return (uint)playerID + 1; // ID从0-4,出生点从1-5
        }
        private bool ValidPlayerID(long playerID)
        {
            if ((0 <= playerID && playerID < options.StudentCount) || (options.MaxStudentCount <= playerID && playerID < options.MaxStudentCount + options.TrickerCount))
                return true;
            return false;
        }

        private MessageOfAll GetMessageOfAll(int time)
        {
            MessageOfAll msg = new MessageOfAll();
            msg.GameTime = time;
            msg.SubjectFinished = (int)game.GameMap.NumOfRepairedGenerators;
            msg.StudentGraduated = (int)game.GameMap.NumOfEscapedStudent;
            msg.StudentQuited = (int)game.GameMap.NumOfDeceasedStudent;
            int[] score = GetScore();
            msg.StudentScore = score[0];
            msg.TrickerScore = score[1];
            //msg.GateOpened
            //msg.HiddenGateRefreshed
            //msg.HiddenGateOpened
            return msg;
        }

        private Protobuf.PlaceType IntToPlaceType(uint n)
        {
            switch (n)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    return Protobuf.PlaceType.Land;
                case 6: return Protobuf.PlaceType.Wall;
                case 7: return Protobuf.PlaceType.Grass;
                case 8: return Protobuf.PlaceType.Classroom;
                case 9: return Protobuf.PlaceType.Gate;
                case 10: return Protobuf.PlaceType.HiddenGate;
                case 11: return Protobuf.PlaceType.Window;
                case 12: return Protobuf.PlaceType.Door3;
                case 13: return Protobuf.PlaceType.Door5;
                case 14: return Protobuf.PlaceType.Door6;
                case 15: return Protobuf.PlaceType.Chest;
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

        public GameServer(ArgumentOptions options)
        {
            this.options = options;
            if (options.mapResource == DefaultArgumentOptions.MapResource)
                this.game = new Game(MapInfo.defaultMap, options.TeamCount);
            else
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
            playerNum = options.StudentCount + options.TrickerCount;
            currentMapMsg = MapMsg(game.GameMap.ProtoGameMap);
            communicationToGameID = new long[options.MaxStudentCount + options.TrickerCount];
            //创建server时先设定待加入人物都是invalid
            for (int i = 0; i < communicationToGameID.GetLength(0); i++)
            {
                communicationToGameID[i] = GameObj.invalidID;
            }

            if (options.FileName != DefaultArgumentOptions.FileName)
            {
                try
                {
                    mwr = new MessageWriter(options.FileName, options.TeamCount, options.StudentCount);
                }
                catch
                {
                    Console.WriteLine($"Error: Cannot create the playback file: {options.FileName}!");
                }
            }

            if (options.Url != DefaultArgumentOptions.Url && options.Token != DefaultArgumentOptions.Token)
            {
                this.httpSender = new HttpSender(options.Url, options.Token);
            }
        }
    }
}
