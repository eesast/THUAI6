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
            playerInfo.StudentType = StudentType.NullStudentType;
            var call = client.AddPlayer(playerInfo);
            MoveMsg moveMsg = new();
            moveMsg.PlayerId = 0;
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
                Console.WriteLine("hi");
                var currentGameInfo = call.ResponseStream.Current;
                for (int i = 0; i < currentGameInfo.ObjMessage.Count; i++)
                {
                    if (currentGameInfo.ObjMessage[i].MessageOfObjCase == MessageOfObj.MessageOfObjOneofCase.StudentMessage)
                        Console.WriteLine($"Human is at ({currentGameInfo.ObjMessage[i].StudentMessage.X}, {currentGameInfo.ObjMessage[i].StudentMessage.Y})");
                }
            }

        }
    }
}