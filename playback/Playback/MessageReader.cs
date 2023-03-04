using Communication.Proto;
using Google.Protobuf;
using System;
using System.IO;
using System.IO.Compression;

namespace Playback
{
    public class FileFormatNotLegalException : Exception
    {
        private readonly string fileName;
        public FileFormatNotLegalException(string fileName)
        {
            this.fileName = fileName;
        }
        public override string Message => $"The file: " + this.fileName + " is not a legal playback file for THUAI6.";
    }

    public class MessageReader : IDisposable
    {
        private FileStream? fs;
        private CodedInputStream cos;
        private GZipStream gzs;
        private byte[] buffer;
        public bool Finished { get; private set; } = false;

        public readonly uint teamCount;
        public readonly uint playerCount;

        const int bufferMaxSize = 1024 * 1024;       // 1M

        public MessageReader(string fileName)
        {
            if (!fileName.EndsWith(PlayBackConstant.ExtendedName))
            {
                fileName += PlayBackConstant.ExtendedName;
            }

            fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);

            try
            {
                var prefixLen = PlayBackConstant.Prefix.Length;
                byte[] bt = new byte[prefixLen + sizeof(UInt32) * 2];
                fs.Read(bt, 0, bt.Length);
                for (int i = 0; i < prefixLen; ++i)
                {
                    if (i == 2)
                    {
                        if (bt[i] == 0)  // 为了做一个兼容，0和5均视为5
                            continue;
                    }
                    if (bt[i] != PlayBackConstant.Prefix[i]) throw new FileFormatNotLegalException(fileName);
                }

                teamCount = BitConverter.ToUInt32(bt, prefixLen);
                playerCount = BitConverter.ToUInt32(bt, prefixLen + sizeof(UInt32));
            }
            catch
            {
                throw new FileFormatNotLegalException(fileName);
            }

            gzs = new GZipStream(fs, CompressionMode.Decompress);
            var tmpBuffer = new byte[bufferMaxSize];
            var bufferSize = gzs.Read(tmpBuffer);
            if (bufferSize == 0)
            {
                buffer = tmpBuffer;
                Finished = true;
            }
            else if (bufferSize != bufferMaxSize)       // 不留空位，防止 CodedInputStream 获取信息错误
            {
                if (bufferSize == 0)
                {
                    Finished = true;
                }
                buffer = new byte[bufferSize];
                Array.Copy(tmpBuffer, buffer, bufferSize);
            }
            else
            {
                buffer = tmpBuffer;
            }
            cos = new CodedInputStream(buffer);
        }

        public MessageToClient? ReadOne()
        {
        beginRead:
            if (Finished) return null;
            var pos = cos.Position;
            try
            {
                MessageToClient? msg = new MessageToClient();
                cos.ReadMessage(msg);
                return msg;
            }
            catch (InvalidProtocolBufferException)
            {
                var leftByte = buffer.Length - pos;     // 上次读取剩余的字节
                for (int i = 0; i < leftByte; ++i)
                {
                    buffer[i] = buffer[pos + i];
                }
                var bufferSize = gzs.Read(buffer, (int)leftByte, (int)(buffer.Length - leftByte)) + leftByte;
                if (bufferSize == leftByte)
                {
                    Finished = true;
                    return null;
                }
                if (bufferSize != buffer.Length)        // 不留空位，防止 CodedInputStream 获取信息错误
                {
                    var tmpBuffer = new byte[bufferSize];
                    Array.Copy(buffer, tmpBuffer, bufferSize);
                    buffer = tmpBuffer;
                }
                cos = new CodedInputStream(buffer);
                goto beginRead;
            }
        }

        public void Dispose()
        {
            Finished = true;
            if (fs == null)
                return;
            if (fs.CanRead)
            {
                fs.Close();
            }
        }

        ~MessageReader()
        {
            Dispose();
        }
    }
}
