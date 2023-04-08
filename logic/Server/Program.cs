using Grpc.Core;
using Protobuf;
using System.Threading;
using Timothy.FrameRateTask;
using System;
using System.Net.Http.Headers;
using CommandLine;

namespace Server
{
    public class Program
    {
        static int Main(string[] args)
        {
            foreach (var arg in args)
            {
                Console.Write($"{arg} ");
            }
            Console.WriteLine();

            ArgumentOptions? options = null;
            _ = Parser.Default.ParseArguments<ArgumentOptions>(args).WithParsed(o => { options = o; });
            if (options == null)
            {
                Console.WriteLine("Argument parsing failed!");
                return 1;
            }

            Console.WriteLine("Server begins to run: " + options.ServerPort.ToString());

            try
            {
                GameServer? gameServer = new(options);
                Grpc.Core.Server server = new Grpc.Core.Server(new[] { new ChannelOption(ChannelOptions.SoReuseport, 0) })
                {
                    Services = { AvailableService.BindService(gameServer) },
                    Ports = { new ServerPort(options.ServerIP, options.ServerPort, ServerCredentials.Insecure) }
                };
                server.Start();

                Console.WriteLine("Server begins to listen!");
                gameServer.WaitForEnd();
                Console.WriteLine("Server end!");
                server.ShutdownAsync().Wait();

                Thread.Sleep(50);
                Console.WriteLine("");
                Console.WriteLine("===================  Final Score   ====================");
                Console.WriteLine($"Studnet: {gameServer.GetScore()[0]}");
                Console.WriteLine($"Tricker: {gameServer.GetScore()[1]}");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return 0;
        }
    }
}