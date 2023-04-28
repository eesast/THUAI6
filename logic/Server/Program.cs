using CommandLine;
using Grpc.Core;
using Protobuf;

namespace Server
{
    public class Program
    {
        static ServerBase CreateServer(ArgumentOptions options)
        {
            return options.Playback ? new PlaybackServer(options) : new GameServer(options);
        }

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
                var server = CreateServer(options);
                Grpc.Core.Server rpcServer = new Grpc.Core.Server(new[] { new ChannelOption(ChannelOptions.SoReuseport, 0) })
                {
                    Services = { AvailableService.BindService(server) },
                    Ports = { new ServerPort(options.ServerIP, options.ServerPort, ServerCredentials.Insecure) }
                };
                rpcServer.Start();

                Console.WriteLine("Server begins to listen!");
                server.WaitForEnd();
                Console.WriteLine("Server end!");
                rpcServer.ShutdownAsync().Wait();

                Thread.Sleep(50);
                Console.WriteLine("");
                Console.WriteLine("===================  Final Score  ====================");
                Console.WriteLine($"Studnet: {server.GetScore()[0]}");
                Console.WriteLine($"Tricker: {server.GetScore()[1]}");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return 0;
        }
    }
}