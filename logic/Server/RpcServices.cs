﻿using Grpc.Core;
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
        public override Task<BoolRes> TryConnection(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"TryConnection ID: {request.PlayerId}");
#endif 
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
                    var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));
                    lock (semaDictLock)
                    {
                        semaDict.Add(request.PlayerId, temp);
                    }
                }
                do
                {
                    semaDict[request.PlayerId].Item1.Wait();
                    try
                    {
                        if (currentGameInfo != null)
                        {
                            await responseStream.WriteAsync(currentGameInfo);
                            //Console.WriteLine("Send!");
                        }
                    }
                    finally
                    {
                        semaDict[request.PlayerId].Item2.Release();
                    }
                } while (game.GameMap.Timer.IsGaming);
                return;
            }

            if (game.GameMap.Timer.IsGaming)
                return;
            if (!ValidPlayerID(request.PlayerId))  //玩家id是否正确
                return;
            if (communicationToGameID[request.PlayerId] != GameObj.invalidID)  //是否已经添加了该玩家
                return;

            Preparation.Utility.CharacterType characterType = Preparation.Utility.CharacterType.Null;
            if (request.PlayerType == PlayerType.StudentPlayer)
                characterType = CopyInfo.ToStudentType(request.StudentType);
            else if (request.PlayerType == PlayerType.TrickerPlayer)
                characterType = CopyInfo.ToTrickerType(request.TrickerType);

            lock (addPlayerLock)
            {
                Game.PlayerInitInfo playerInitInfo = new(GetBirthPointIdx(request.PlayerId), PlayerTypeToTeamID(request.PlayerType), (int)request.PlayerId, characterType);
                long newPlayerID = game.AddPlayer(playerInitInfo);
                if (newPlayerID == GameObj.invalidID)
                    return;
                communicationToGameID[request.PlayerId] = newPlayerID;
                // 内容待修改
                var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));
                bool start = false;
                Console.WriteLine($"Id: {request.PlayerId} joins.");
                lock (semaDictLock)
                {
                    semaDict.Add(request.PlayerId, temp);
                    start = (semaDict.Count - spectatorList.Count) == playerNum;
                }
                if (start) StartGame();
            }

            do
            {
                semaDict[request.PlayerId].Item1.Wait();
                try
                {
                    if (currentGameInfo != null)
                    {
                        await responseStream.WriteAsync(currentGameInfo);
                        //Console.WriteLine("Send!");
                    }
                }
                finally
                {
                    semaDict[request.PlayerId].Item2.Release();
                }
            } while (game.GameMap.Timer.IsGaming);
        }

        public override Task<BoolRes> Attack(AttackMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"Attack ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            if (request.Angle == double.NaN)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            game.Attack(gameID, request.Angle);
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }


        public override Task<MoveRes> Move(MoveMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"Move ID: {request.PlayerId}, TimeInMilliseconds: {request.TimeInMilliseconds}");
#endif            
            MoveRes moveRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                moveRes.ActSuccess = false;
                return Task.FromResult(moveRes);
            }
            if (request.Angle == double.NaN)
            {
                moveRes.ActSuccess = false;
                return Task.FromResult(moveRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            game.MovePlayer(gameID, (int)request.TimeInMilliseconds, request.Angle);
            // 之后game.MovePlayer可能改为bool类
            moveRes.ActSuccess = true;
            if (!game.GameMap.Timer.IsGaming) moveRes.ActSuccess = false;
            return Task.FromResult(moveRes);
        }

        public override Task<BoolRes> SendMessage(SendMsg request, ServerCallContext context)
        {
            var boolRes = new BoolRes();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
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
            }
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> PickProp(PropMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"PickProp ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.PickProp(gameID, CopyInfo.ToPropType(request.PropType));
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> UseProp(PropMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"UseProp ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            game.UseProp(gameID, CopyInfo.ToPropType(request.PropType));
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> ThrowProp(PropMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"ThrowProp ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            game.ThrowProp(gameID, CopyInfo.ToPropType(request.PropType));
            boolRes.ActSuccess = true;
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> UseSkill(SkillMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"UseSkill ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.UseActiveSkill(gameID, request.SkillId);
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> Graduate(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"Graduate ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.Escape(gameID);
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> StartRescueMate(TreatAndRescueMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartRescueMate ID: {request.PlayerId}");
#endif 
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            if (request.ToPlayerId >= 0 && request.ToPlayerId < options.MaxStudentCount)
            {
                var toGameID = communicationToGameID[request.ToPlayerId];
                boolRes.ActSuccess = game.Rescue(gameID, toGameID);
            }
            else
            {
                boolRes.ActSuccess = game.Rescue(gameID);
            }

            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> StartTreatMate(TreatAndRescueMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartTreatMate ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            if (request.ToPlayerId >= 0 && request.ToPlayerId < options.MaxStudentCount)
            {
                var toGameID = communicationToGameID[request.ToPlayerId];
                boolRes.ActSuccess = game.Treat(gameID, toGameID);
            }
            else
            {
                boolRes.ActSuccess = game.Treat(gameID);
            }

            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> StartLearning(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartLearning ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.Fix(gameID);
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> StartOpenChest(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartOpenChest ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.OpenChest(gameID);
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> StartOpenGate(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"StartOpenGate ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.OpenDoorway(gameID);
            return Task.FromResult(boolRes);
        }
        public override Task<BoolRes> OpenDoor(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"OpenDoor ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.LockOrOpenDoor(gameID);
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> CloseDoor(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"CloseDoor ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.LockOrOpenDoor(gameID);
            return Task.FromResult(boolRes);
        }

        public override Task<BoolRes> EndAllAction(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"EndAllAction ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.Stop(gameID);
            return Task.FromResult(boolRes);
        }


        public override Task<BoolRes> SkipWindow(IDMsg request, ServerCallContext context)
        {
#if DEBUG
            Console.WriteLine($"SkipWindow ID: {request.PlayerId}");
#endif     
            BoolRes boolRes = new();
            if (request.PlayerId >= spectatorMinPlayerID)
            {
                boolRes.ActSuccess = false;
                return Task.FromResult(boolRes);
            }
            var gameID = communicationToGameID[request.PlayerId];
            boolRes.ActSuccess = game.ClimbingThroughWindow(gameID);
            return Task.FromResult(boolRes);
        }
    }
}
