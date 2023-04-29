using GameClass.GameObj;
using Gaming;
using Grpc.Core;
using Preparation.Utility;
using Protobuf;

namespace Server
{
    partial class GameServer : ServerBase
    {
        private int playerCountNow = 0;
        protected object spectatorLock = new object();
        protected bool isSpectatorJoin = false;
        protected bool IsSpectatorJoin
        {
            get
            {
                lock (spectatorLock)
                    return isSpectatorJoin;
            }

            set
            {
                lock (spectatorLock)
                    isSpectatorJoin = value;
            }
        }
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
            if (request.PlayerId >= spectatorMinPlayerID && options.NotAllowSpectator == false)
            {
                // 观战模式
                lock (spetatorJoinLock) // 具体原因见另一个上锁的地方
                {
                    if (semaDict.TryAdd(request.PlayerId, (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1))))
                    {
                        Console.WriteLine("A new spectator comes to watch this game.");
                        IsSpectatorJoin = true;
                    }
                    else
                    {
                        Console.WriteLine($"Duplicated Spectator ID {request.PlayerId}");
                        return;
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
                    catch (InvalidOperationException)
                    {
                        if (semaDict.TryRemove(request.PlayerId, out var semas))
                        {
                            try
                            {
                                semas.Item1.Release();
                                semas.Item2.Release();
                            }
                            catch { }
                            Console.WriteLine($"The spectator {request.PlayerId} exited");
                            return;
                        }
                    }
                    catch (Exception)
                    {
                        // Console.WriteLine(ex);
                    }
                    finally
                    {
                        try
                        {
                            semaDict[request.PlayerId].Item2.Release();
                        }
                        catch { }
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
                characterType = Transformation.ToStudentType(request.StudentType);
            else if (request.PlayerType == PlayerType.TrickerPlayer)
                characterType = Transformation.ToTrickerType(request.TrickerType);

            lock (addPlayerLock)
            {
                Game.PlayerInitInfo playerInitInfo = new(GetBirthPointIdx(request.PlayerId), PlayerTypeToTeamID(request.PlayerType), (int)request.PlayerId, characterType);
                long newPlayerID = game.AddPlayer(playerInitInfo);
                if (newPlayerID == GameObj.invalidID)
                    return;
                communicationToGameID[request.PlayerId] = newPlayerID;
                var temp = (new SemaphoreSlim(0, 1), new SemaphoreSlim(0, 1));
                bool start = false;
                Console.WriteLine($"Id: {request.PlayerId} joins.");
                lock (spetatorJoinLock)  // 为了保证绝对安全，还是加上这个锁吧
                {
                    if (semaDict.TryAdd(request.PlayerId, temp))
                    {
                        start = Interlocked.Increment(ref playerCountNow) == playerNum;
                    }
                }
                if (start) StartGame();
            }

            bool exitFlag = false;
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
                catch (Exception)
                {
                    if (!exitFlag)
                    {
                        Console.WriteLine($"The client {request.PlayerId} exited");
                        exitFlag = true;
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
            boolRes.ActSuccess = game.Attack(gameID, request.Angle);
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
            moveRes.ActSuccess = game.MovePlayer(gameID, (int)request.TimeInMilliseconds, request.Angle);
            if (!game.GameMap.Timer.IsGaming) moveRes.ActSuccess = false;
            return Task.FromResult(moveRes);
        }

        public override Task<BoolRes> SendMessage(SendMsg request, ServerCallContext context)
        {
            var boolRes = new BoolRes();
            if (request.PlayerId >= spectatorMinPlayerID || playerDeceased((int)request.PlayerId))
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
            
            switch (request.MessageCase)
            {
                case SendMsg.MessageOneofCase.TextMessage:
                    {
                        if (request.TextMessage.Length > 256)
                        {
#if DEBUG
                            Console.WriteLine("Text message string is too long!");
#endif
                            boolRes.ActSuccess = false;
                            return Task.FromResult(boolRes);
                        }
                        MessageOfNews news = new();
                        news.TextMessage = request.TextMessage;
                        news.FromId = request.PlayerId;
                        news.ToId = request.ToPlayerId;
                        lock (newsLock)
                        {
                            currentNews.Add(news);
                        }
#if DEBUG
                        Console.WriteLine(news.TextMessage);
#endif
                        boolRes.ActSuccess = true;
                        return Task.FromResult(boolRes);
                    }
                case SendMsg.MessageOneofCase.BinaryMessage:
                    {

                        if (request.BinaryMessage.Length > 256)
                        {
#if DEBUG
                            Console.WriteLine("Binary message string is too long!");
#endif
                        boolRes.ActSuccess = false;
                            return Task.FromResult(boolRes);
                        }
                        MessageOfNews news = new();
                        news.BinaryMessage = request.BinaryMessage;
                        news.FromId = request.PlayerId;
                        news.ToId = request.ToPlayerId;
                        lock (newsLock)
                        {
                            currentNews.Add(news);
                        }
#if DEBUG
                        Console.Write("BinaryMessageLength: ");
                        Console.WriteLine(news.BinaryMessage.Length);
#endif
                        boolRes.ActSuccess = true;
                        return Task.FromResult(boolRes);
                    }
                default:
                    {
                        boolRes.ActSuccess = false;
                        return Task.FromResult(boolRes);
                    }
            }

            
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
            boolRes.ActSuccess = game.PickProp(gameID, Transformation.ToPropType(request.PropType));
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
            game.UseProp(gameID, Transformation.ToPropType(request.PropType));
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
            game.ThrowProp(gameID, Transformation.ToPropType(request.PropType));
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
