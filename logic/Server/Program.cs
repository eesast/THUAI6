using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;

namespace Server
{
    public class Program
    {
        public static void Main()
        {
            try
            {
                GameServer gameServer = new();
                Grpc.Core.Server server = new Grpc.Core.Server(new[] { new ChannelOption(ChannelOptions.SoReuseport, 0) })
                {
                    Services = { AvailableService.BindService(gameServer) },
                    Ports = { new ServerPort("0.0.0.0", 8888, ServerCredentials.Insecure) }
                };
                server.Start();

                Console.WriteLine("Server begins to listen!");
                gameServer.WaitForEnd();
                Console.WriteLine("Server end!");
                server.ShutdownAsync().Wait();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }
    }
}