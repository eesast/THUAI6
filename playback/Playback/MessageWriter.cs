using Google.Protobuf;
using Protobuf;
using System;
using System.IO;
using System.IO.Compression;

namespace Playback
{
    public class MessageWriter : IDisposable
    {
        private FileStream fs;
        private CodedOutputStream cos;
        private MemoryStream ms;
        private GZipStream gzs;
        private const int memoryCapacity = 1024 * 1024;    // 1M

        private static void ClearMemoryStream(MemoryStream msToClear)
        {
            msToClear.Position = 0;
            msToClear.SetLength(0);
        }

        public MessageWriter(string fileName, uint teamCount, uint playerCount)
        {
            if (!fileName.EndsWith(PlayBackConstant.ExtendedName))
            {
                fileName += PlayBackConstant.ExtendedName;
            }

            fs = new FileStream(fileName, FileMode.Create, FileAccess.Write);
            fs.Write(PlayBackConstant.Prefix);      // 写入前缀

            fs.Write(BitConverter.GetBytes((UInt32)teamCount));    // 写入队伍数
            fs.Write(BitConverter.GetBytes((UInt32)playerCount));    // 写入每队的玩家人数
            ms = new MemoryStream(memoryCapacity);
            cos = new CodedOutputStream(ms);
            gzs = new GZipStream(fs, CompressionMode.Compress);
        }

        public void WriteOne(MessageToClient msg)
        {
            cos.WriteMessage(msg);
            if (ms.Length > memoryCapacity)
                Flush();
        }

        public void Flush()
        {
            if (fs.CanWrite)
            {
                cos.Flush();
                gzs.Write(ms.GetBuffer(), 0, (int)ms.Length);
                gzs.Flush();
                ClearMemoryStream(ms);
                fs.Flush();
            }
        }

        public void Dispose()
        {
            if (fs.CanWrite)
            {
                Flush();
                cos.Dispose();
                gzs.Dispose();
                fs.Dispose();
            }
        }

        ~MessageWriter()
        {
            Dispose();
        }
    }
}
