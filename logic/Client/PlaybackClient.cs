using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Protobuf;
using Playback;
using System.Threading;
using Timothy.FrameRateTask;
using System.Windows;

namespace Client
{
    public class PlaybackClient
    {
        private readonly string fileName;
        private readonly double playbackSpeed;
        private readonly int frameTimeInMilliseconds;
        public MessageReader? Reader;
        private SemaphoreSlim sema;
        public SemaphoreSlim Sema => sema;
        public PlaybackClient(string fileName, double playbackSpeed = 1.0, int frameTimeInMilliseconds = 50)
        {
            this.fileName = fileName;
            this.playbackSpeed = playbackSpeed;
            this.frameTimeInMilliseconds = frameTimeInMilliseconds;
            this.sema = new SemaphoreSlim(1, 1);
            try
            {
                Reader = new MessageReader(this.fileName);
            }
            catch (Exception ex)
            {
                Reader = null;
                Console.WriteLine(ex.Message);
                return;
            }
        }

        public int[,]? ReadDataFromFile(List<MessageOfProp> listOfProp, List<MessageOfStudent> listOfHuman, List<MessageOfTricker> listOfButcher, List<MessageOfBullet> listOfBullet,
        List<MessageOfBombedBullet> listOfBombedBullet, List<MessageOfAll> listOfAll, List<MessageOfChest> listOfChest, List<MessageOfClassroom> listOfClassroom,
        List<MessageOfDoor> listOfDoor, List<MessageOfHiddenGate> listOfHiddenGate, List<MessageOfGate> listOfGate, object lockOfProp, object lockOfHuman, object lockOfButcher,
        object lockOfBullet, object lockOfBombedBullet, object lockOfAll, object lockOfChest, object lockOfClassroom, object lockOfDoor, object lockOfHiddenGate, object lockOfGate)
        {
            if (Reader == null)
                return null;
            Sema.Wait();
            bool endFile = false;
            bool mapFlag = false;  // 是否获取了地图
            int[,] map = new int[50, 50];
            long frame = (long)(this.frameTimeInMilliseconds / this.playbackSpeed);
            var mapCollecter = new MessageReader(this.fileName);
            while (!mapFlag)
            {
                var msg = mapCollecter.ReadOne();
                if (msg == null)
                    throw new Exception("Map message is not in the playback file!");
                foreach (var obj in msg.ObjMessage)
                {
                    if (obj.MessageOfObjCase == MessageOfObj.MessageOfObjOneofCase.MapMessage)
                    {
                        try
                        {
                            for (int i = 0; i < 50; i++)
                            {
                                for (int j = 0; j < 50; j++)
                                {
                                    map[i, j] = Convert.ToInt32(obj.MapMessage.Row[i].Col[j]) + 4;
                                }
                            }
                        }
                        catch
                        {
                            mapFlag = false;
                        }
                        finally
                        {
                            mapFlag = true;
                        }
                        break;
                    }
                }
            };
            new Thread(() =>
            {
                try
                {
                    new FrameRateTaskExecutor<int>
                    (
                        () => !endFile,
                        () =>
                        {
                            var content = Reader.ReadOne();
                            if (content == null)
                            {
                                endFile = true;
                            }
                            else
                            {
                                // 加锁是必要的，画图操作和接收信息操作不能同时进行，否则画图时foreach会有bug
                                lock (lockOfHuman)
                                {
                                    listOfHuman.Clear();
                                }
                                lock (lockOfButcher)
                                {
                                    listOfButcher.Clear();
                                }
                                lock (lockOfProp)
                                {
                                    listOfProp.Clear();
                                }
                                lock (lockOfBombedBullet)
                                {
                                    listOfBombedBullet.Clear();
                                }
                                lock (lockOfBullet)
                                {
                                    listOfBullet.Clear();
                                }
                                lock (lockOfAll)
                                {
                                    listOfAll.Clear();
                                }
                                lock (lockOfChest)
                                {
                                    listOfChest.Clear();
                                }
                                lock (lockOfClassroom)
                                {
                                    listOfClassroom.Clear();
                                }
                                lock (lockOfDoor)
                                {
                                    listOfDoor.Clear();
                                }
                                lock (lockOfHiddenGate)
                                {
                                    listOfHiddenGate.Clear();
                                }
                                lock (lockOfGate)
                                {
                                    listOfGate.Clear();
                                }
                                switch (content.GameState)
                                {
                                    case GameState.GameStart:
                                        foreach (var obj in content.ObjMessage)
                                        {
                                            switch (obj.MessageOfObjCase)
                                            {
                                                case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                    lock (lockOfHuman)
                                                    {
                                                        listOfHuman.Add(obj.StudentMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                    lock (lockOfButcher)
                                                    {
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                    lock (lockOfProp)
                                                    {
                                                        listOfProp.Add(obj.PropMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                    lock (lockOfBombedBullet)
                                                    {
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                    lock (lockOfBullet)
                                                    {
                                                        listOfBullet.Add(obj.BulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                    lock (lockOfChest)
                                                    {
                                                        listOfChest.Add(obj.ChestMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                    lock (lockOfClassroom)
                                                    {
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                    lock (lockOfDoor)
                                                    {
                                                        listOfDoor.Add(obj.DoorMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                    lock (lockOfGate)
                                                    {
                                                        listOfGate.Add(obj.GateMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.MapMessage:
                                                    break;
                                            }
                                        }
                                        lock (lockOfAll)
                                        {
                                            listOfAll.Add(content.AllMessage);
                                        }
                                        break;
                                    case GameState.GameRunning:
                                        foreach (var obj in content.ObjMessage)
                                        {
                                            switch (obj.MessageOfObjCase)
                                            {
                                                case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                    lock (lockOfHuman)
                                                    {
                                                        listOfHuman.Add(obj.StudentMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                    lock (lockOfButcher)
                                                    {
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                    lock (lockOfProp)
                                                    {
                                                        listOfProp.Add(obj.PropMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                    lock (lockOfBombedBullet)
                                                    {
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                    lock (lockOfBullet)
                                                    {
                                                        listOfBullet.Add(obj.BulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                    lock (lockOfChest)
                                                    {
                                                        listOfChest.Add(obj.ChestMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                    lock (lockOfClassroom)
                                                    {
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                    lock (lockOfDoor)
                                                    {
                                                        listOfDoor.Add(obj.DoorMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                    lock (lockOfGate)
                                                    {
                                                        listOfGate.Add(obj.GateMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.HiddenGateMessage:
                                                    lock (lockOfHiddenGate)
                                                    {
                                                        listOfHiddenGate.Add(obj.HiddenGateMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.MapMessage:
                                                    break;
                                            }
                                        }
                                        lock (lockOfAll)
                                        {
                                            listOfAll.Add(content.AllMessage);
                                        }
                                        break;
                                    case GameState.GameEnd:
                                        MessageBox.Show("Game Over!");
                                        foreach (var obj in content.ObjMessage)
                                        {
                                            switch (obj.MessageOfObjCase)
                                            {
                                                case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                    lock (lockOfHuman)
                                                    {
                                                        listOfHuman.Add(obj.StudentMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                    lock (lockOfButcher)
                                                    {
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                    lock (lockOfProp)
                                                    {
                                                        listOfProp.Add(obj.PropMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                    lock (lockOfBombedBullet)
                                                    {
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                    lock (lockOfBullet)
                                                    {
                                                        listOfBullet.Add(obj.BulletMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                    lock (lockOfChest)
                                                    {
                                                        listOfChest.Add(obj.ChestMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                    lock (lockOfClassroom)
                                                    {
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                    lock (lockOfDoor)
                                                    {
                                                        listOfDoor.Add(obj.DoorMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                    lock (lockOfGate)
                                                    {
                                                        listOfGate.Add(obj.GateMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.HiddenGateMessage:
                                                    lock (lockOfHiddenGate)
                                                    {
                                                        listOfHiddenGate.Add(obj.HiddenGateMessage);
                                                    }
                                                    break;
                                                case MessageOfObj.MessageOfObjOneofCase.MapMessage:
                                                    break;
                                            }
                                        }
                                        lock (lockOfAll)
                                        {
                                            listOfAll.Add(content.AllMessage);
                                        }
                                        break;
                                }
                            }
                        },
                    frame,
                    () =>
                    {
                        Sema.Release();
                        return 1;
                    }
                    )
                    { AllowTimeExceed = true }.Start();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            })
            { IsBackground = true }.Start();
            return map;
        }
    }
}
