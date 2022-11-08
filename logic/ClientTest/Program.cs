using Grpc.Core;
using Protobuf;

namespace ClientTest
{
    public class Program
    {
        public static async Task Main(string[] args)
        {
            Channel channel = new Channel("127.0.0.1:8888", ChannelCredentials.Insecure);
            var client = new AvailableService.AvailableServiceClient(channel);
            PlayerMsg playerInfo = new();
            playerInfo.PlayerId = Convert.ToInt32(args[0]);
            playerInfo.PlayerType = (PlayerType)Convert.ToInt32(args[1]);
            var call = client.AddPlayer(playerInfo);
            while (await call.ResponseStream.MoveNext())
            {
                var currentGameInfo = call.ResponseStream.Current;
                if (playerInfo.PlayerType == PlayerType.HumanPlayer)
                {
                    for (int i = 0; i < currentGameInfo.HumanMessage.Count; i++)
                    {
                        Console.WriteLine($"Human is at ({currentGameInfo.HumanMessage[i].X}, {currentGameInfo.HumanMessage[i].Y})");
                    }
                }
                if (playerInfo.PlayerType == PlayerType.ButcherPlayer)
                {
                    for (int i = 0; i < currentGameInfo.ButcherMessage.Count; i++)
                    {
                        Console.WriteLine($"Butcher is at ({currentGameInfo.ButcherMessage[i].X}, {currentGameInfo.ButcherMessage[i].Y})");
                    }
                }
            }
        }
    }
}