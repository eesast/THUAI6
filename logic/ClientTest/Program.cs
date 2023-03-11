using Grpc.Core;
using Protobuf;

namespace ClientTest
{
    public class Program
    {
        public static async Task Main(string[] args)
        {
            Thread.Sleep(3000);
            Channel channel = new Channel("127.0.0.1:8888", ChannelCredentials.Insecure);
            var client = new AvailableService.AvailableServiceClient(channel);
            PlayerMsg playerInfo = new();
            playerInfo.PlayerId = 0;
            playerInfo.PlayerType = PlayerType.StudentPlayer;
            playerInfo.StudentType = StudentType.NullStudentType;
            var call = client.AddPlayer(playerInfo);
            MoveMsg moveMsg = new();
            moveMsg.PlayerId = 0;
            moveMsg.PlayerType = PlayerType.StudentPlayer;
            moveMsg.TimeInMilliseconds = 100;
            moveMsg.Angle = 0;
            while (true)
            {
                Console.ReadLine();
                client.Move(moveMsg);
                Console.WriteLine("Move!");
            }

            while (await call.ResponseStream.MoveNext())
            {
                var currentGameInfo = call.ResponseStream.Current;
                if (playerInfo.PlayerType == PlayerType.StudentPlayer)
                {
                    for (int i = 0; i < currentGameInfo.ObjMessage.Count; i++)
                    {
                        //Console.WriteLine($"Human is at ({currentGameInfo.StudentMessage[i].X}, {currentGameInfo.StudentMessage[i].Y})");
                    }
                }
            }
        }
    }
}