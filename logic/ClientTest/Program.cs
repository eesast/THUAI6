using Grpc.Core;
using Protobuf;

namespace ClientTest
{
    public class Program
    {
        public static Task Main(string[] args)
        {
            Thread.Sleep(3000);
            Channel channel = new Channel("127.0.0.1:8888", ChannelCredentials.Insecure);
            var client = new AvailableService.AvailableServiceClient(channel);
            PlayerMsg playerInfo = new();
            playerInfo.PlayerId = 0;
            playerInfo.PlayerType = PlayerType.StudentPlayer;
            playerInfo.StudentType = StudentType.Athlete;
            var call = client.AddPlayer(playerInfo);
            MoveMsg moveMsg = new();
            moveMsg.PlayerId = 0;
            moveMsg.TimeInMilliseconds = 100;
            moveMsg.Angle = 0;
            int tot = 0;
            /*while (await call.ResponseStream.MoveNext())
            {
                var currentGameInfo = call.ResponseStream.Current;
                if (currentGameInfo.GameState == GameState.GameStart) break;
            }*/
            while (true)
            {
                Thread.Sleep(50);
                MoveRes boolRes = client.Move(moveMsg);
                if (boolRes.ActSuccess == false) break;
                tot++;
                if (tot % 10 == 0) moveMsg.Angle += 1;

                Console.WriteLine("Move!");
            }

            return Task.CompletedTask;
        }
    }
}