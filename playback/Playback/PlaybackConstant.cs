namespace Playback
{
    public static class PlayBackConstant
    {
        public static string ExtendedName = ".thuaipb";
        public static byte[] Prefix = { (byte)'P', (byte)'B', 5, 0 };   // 文件前缀，用于标识文件类型，版本号为6
    }
}
