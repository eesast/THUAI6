using System;
using System.Collections.Generic;
using CommandLine;

namespace Client
{
    static class DefaultArgumentOptions
    {
        public static string FileName = "CLGG!@#$%^&*()_+";  // An impossible name of the playback file to indicate -f is not sepcified.
    }

    public class ArgumentOptions
    {
        [Option('u', "cl", Required = false, HelpText = "Whether to use command line")]
        public bool cl { get; set; } = false;

        [Option('i', "ip", Required = false, HelpText = "Client connected ip")]
        public string Ip { get; set; } = "127.0.0.1";

        [Option('p', "port", Required = false, HelpText = "Client listening port")]
        public string Port { get; set; } = "8888";

        [Option('t', "teamID", Required = false, HelpText = "Client teamID")]
        public string TeamID { get; set; } = "0";

        [Option('c', "characterID", Required = false, HelpText = "Client playerID")]
        public string PlayerID { get; set; } = "0";

        [Option('y', "type", Required = false, HelpText = "Client playerType")]
        public string PlayerType { get; set; } = "0";

        [Option('o', "occupation", Required = false, HelpText = "Client occupation")]
        public string Occupation { get; set; } = "0";

        [Option('f', "playbackFile", Required = false, HelpText = "The playback file name.")]
        public string PlaybackFile { get; set; } = DefaultArgumentOptions.FileName;

        [Option("playbackSpeed", Required = false, HelpText = "The speed of the playback, between 0.25 and 4.0")]
        public double PlaybackSpeed { get; set; } = 2.0;

    }
}
