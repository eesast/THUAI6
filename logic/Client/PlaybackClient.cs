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
        List<MessageOfDoor> listOfDoor, List<MessageOfHiddenGate> listOfHiddenGate, List<MessageOfGate> listOfGate, object dataLock)
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
                int i = 0;
                try
                {
                    new FrameRateTaskExecutor<int>
                    (
                        () => !endFile,
                        () =>
                        {
                            var content = Reader.ReadOne();
                            i++;
                            if (content == null)
                            {
                                MessageBox.Show($"End! {i}");
                                endFile = true;
                            }
                            else
                            {
                                lock (dataLock)
                                {
                                    listOfHuman.Clear();
                                    listOfButcher.Clear();
                                    listOfProp.Clear();
                                    listOfBombedBullet.Clear();
                                    listOfBullet.Clear();
                                    listOfAll.Clear();
                                    listOfChest.Clear();
                                    listOfClassroom.Clear();
                                    listOfDoor.Clear();
                                    listOfHiddenGate.Clear();
                                    listOfGate.Clear();
                                    switch (content.GameState)
                                    {
                                        case GameState.GameStart:
                                            foreach (var obj in content.ObjMessage)
                                            {
                                                switch (obj.MessageOfObjCase)
                                                {
                                                    case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                        //if (humanOrButcher && obj.StudentMessage.PlayerId == playerID)
                                                        //{
                                                        //    human = obj.StudentMessage;
                                                        //}
                                                        listOfHuman.Add(obj.StudentMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                        //if (!humanOrButcher && obj.TrickerMessage.PlayerId == playerID)
                                                        //{
                                                        //    butcher = obj.TrickerMessage;
                                                        //}
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                        listOfProp.Add(obj.PropMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                        listOfBullet.Add(obj.BulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                        listOfChest.Add(obj.ChestMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                        listOfDoor.Add(obj.DoorMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                        listOfGate.Add(obj.GateMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.HiddenGateMessage:
                                                        listOfHiddenGate.Add(obj.HiddenGateMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.MapMessage:
                                                        break;
                                                }
                                            }
                                            listOfAll.Add(content.AllMessage);
                                            break;
                                        case GameState.GameRunning:
                                            foreach (var obj in content.ObjMessage)
                                            {
                                                switch (obj.MessageOfObjCase)
                                                {
                                                    case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                        //if (humanOrButcher && obj.StudentMessage.PlayerId == playerID)
                                                        //{
                                                        //    human = obj.StudentMessage;
                                                        //}
                                                        listOfHuman.Add(obj.StudentMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                        //if (!humanOrButcher && obj.TrickerMessage.PlayerId == playerID)
                                                        //{
                                                        //    butcher = obj.TrickerMessage;
                                                        //}
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                        listOfProp.Add(obj.PropMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                        listOfBullet.Add(obj.BulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                        listOfChest.Add(obj.ChestMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                        listOfDoor.Add(obj.DoorMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.HiddenGateMessage:
                                                        listOfHiddenGate.Add(obj.HiddenGateMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                        listOfGate.Add(obj.GateMessage);
                                                        break;
                                                }
                                            }
                                            listOfAll.Add(content.AllMessage);
                                            break;

                                        case GameState.GameEnd:
                                            MessageBox.Show("Game Over!");
                                            foreach (var obj in content.ObjMessage)
                                            {
                                                switch (obj.MessageOfObjCase)
                                                {
                                                    case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                                        listOfHuman.Add(obj.StudentMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                                        listOfButcher.Add(obj.TrickerMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.PropMessage:
                                                        listOfProp.Add(obj.PropMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BombedBulletMessage:
                                                        listOfBombedBullet.Add(obj.BombedBulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.BulletMessage:
                                                        listOfBullet.Add(obj.BulletMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ChestMessage:
                                                        listOfChest.Add(obj.ChestMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.ClassroomMessage:
                                                        listOfClassroom.Add(obj.ClassroomMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.DoorMessage:
                                                        listOfDoor.Add(obj.DoorMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.HiddenGateMessage:
                                                        listOfHiddenGate.Add(obj.HiddenGateMessage);
                                                        break;
                                                    case MessageOfObj.MessageOfObjOneofCase.GateMessage:
                                                        listOfGate.Add(obj.GateMessage);
                                                        break;
                                                }
                                            }
                                            listOfAll.Add(content.AllMessage);
                                            break;
                                    }
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
