using Grpc.Core;
using Playback;
using Protobuf;
using System.Collections.Concurrent;
using Timothy.FrameRateTask;

namespace Server
{
    abstract class ServerBase : AvailableService.AvailableServiceBase
    {
        public abstract void WaitForEnd();
        public abstract int[] GetScore();
    }
}
