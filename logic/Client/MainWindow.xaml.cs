﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using System.Windows.Threading;
using Grpc.Core;
using Protobuf;
using Playback;
using CommandLine;
using Preparation.Utility;
using Preparation.Interface;
using System.Diagnostics.CodeAnalysis;

// 目前MainWindow还未复现的功能：
// private void ClickToSetMode(object sender, RoutedEventArgs e)
// private void Bonus()

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            timer = new DispatcherTimer
            {
                Interval = new TimeSpan(50000)  // 每50ms刷新一次
            };
            timer.Tick += new EventHandler(Refresh);  // 定时器初始化
            InitializeComponent();
            timer.Start();
            SetStatusBar();
            isClientStocked = true;
            isPlaybackMode = false;
            isSpectatorMode = false;
            drawPicLock = new();
            listOfProp = new List<MessageOfProp>();
            listOfHuman = new List<MessageOfStudent>();
            listOfButcher = new List<MessageOfTricker>();
            listOfBullet = new List<MessageOfBullet>();
            listOfBombedBullet = new List<MessageOfBombedBullet>();
            listOfAll = new List<MessageOfAll>();
            listOfChest = new List<MessageOfChest>();
            listOfClassroom = new List<MessageOfClassroom>();
            listOfDoor = new List<MessageOfDoor>();
            listOfGate = new List<MessageOfGate>();
            listOfHiddenGate = new List<MessageOfHiddenGate>();
            countList = new List<int>();
            WindowStartupLocation = WindowStartupLocation.CenterScreen;
            unit = Math.Sqrt(UpperLayerOfMap.ActualHeight * UpperLayerOfMap.ActualWidth) / 50;
            unitFontsize = unit / 13;
            unitHeight = UpperLayerOfMap.ActualHeight / 50;
            unitWidth = UpperLayerOfMap.ActualWidth / 50;
            ReactToCommandline();
        }

        [MemberNotNull(nameof(StatusBarsOfSurvivor), nameof(StatusBarsOfHunter), nameof(StatusBarsOfCircumstance))]
        private void SetStatusBar()
        {
            StatusBarsOfSurvivor = new StatusBarOfSurvivor[4];
            StatusBarsOfHunter = new StatusBarOfHunter(MainGrid, 3, 0);
            StatusBarsOfCircumstance = new StatusBarOfCircumstance(MainGrid, 1, 0);
            for (int i = 4; i < 8; i++)
            {
                StatusBarsOfSurvivor[i - 4] = new(MainGrid, i / 2 + 2, i % 2);
            }
        }

        // 获得地图信息,未更新数值
        private void GetMap(MessageOfMap obj)
        {
            int[,] map = new int[50, 50];
            try
            {
                for (int i = 0; i < 50; i++)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        map[i, j] = Convert.ToInt32(obj.Row[i].Col[j]) + 4;//与proto一致
                    }
                }
            }
            catch
            {
                mapFlag = false;
            }
            finally
            {
                defaultMap = map;
                mapFlag = true;
            }
        }

        private void ReactToCommandline()
        {
            string[] args = Environment.GetCommandLineArgs();
            if (args.Length == 2)
            {
                Playback(args[1]);
                return;
            }
            _ = Parser.Default.ParseArguments<ArgumentOptions>(args).WithParsed(o =>
            { options = o; });
            if (options != null && Convert.ToInt64(options.PlayerID) > 2023)
            {
                isSpectatorMode = true;
                string[] comInfo = new string[3];
                comInfo[0] = options.Ip;
                comInfo[1] = options.Port;
                comInfo[2] = options.PlayerID;
                ConnectToServer(comInfo);
                OnReceive();
                return;
            }
            if (options == null || options.cl == false)
            {
                OnReceive();
            }
            else
            {
                if (options.PlaybackFile == DefaultArgumentOptions.FileName)
                {
                    try
                    {
                        string[] comInfo = new string[5];
                        comInfo[0] = options.Ip;
                        comInfo[1] = options.Port;
                        comInfo[2] = options.PlayerID;
                        comInfo[3] = options.PlayerType;
                        comInfo[4] = options.Occupation;
                        ConnectToServer(comInfo);
                        OnReceive();
                    }
                    catch
                    {
                        OnReceive();
                    }
                }
                else
                {
                    Playback(options.PlaybackFile, options.PlaybackSpeed);
                }
            }
        }

        private void Playback(string fileName, double pbSpeed = 2.0)
        {
            var pbClient = new PlaybackClient(fileName, pbSpeed);
            int[,]? map;
            if ((map = pbClient.ReadDataFromFile(listOfProp, listOfHuman, listOfButcher, listOfBullet, listOfBombedBullet, listOfAll, listOfChest, listOfClassroom, listOfDoor, listOfHiddenGate, listOfGate, drawPicLock, countList)) != null)
            {
                isClientStocked = false;
                PorC.Content = "⏸";
                isPlaybackMode = true;
                defaultMap = map;
                mapFlag = true;
            }
            else
            {
                MessageBox.Show("Failed to read the playback file!");
                isClientStocked = true;
                PorC.Content = "▶";
            }
        }

        // 连接Server,comInfo[]的格式：0-ip 1- port 2-playerID 3-human/TrickerType 4-occupation
        private void ConnectToServer(string[] comInfo)
        {
            if (!isPlaybackMode)
            {
                if (!isSpectatorMode && comInfo.Length != 5 || isSpectatorMode && comInfo.Length != 3)
                    throw new Exception("注册信息有误！");
                playerID = Convert.ToInt64(comInfo[2]);
                Connect.Background = Brushes.Gray;
                string connect = new string(comInfo[0]);
                connect += ':';
                connect += comInfo[1];
                Channel channel = new Channel(connect, ChannelCredentials.Insecure);
                client = new AvailableService.AvailableServiceClient(channel);
                PlayerMsg playerMsg = new PlayerMsg();
                playerMsg.PlayerId = playerID;
                if (!isSpectatorMode)
                {
                    playerType = Convert.ToInt64(comInfo[3]) switch
                    {
                        0 => PlayerType.NullPlayerType,
                        1 => PlayerType.StudentPlayer,
                        2 => PlayerType.TrickerPlayer,
                        _ => PlayerType.NullPlayerType
                    };
                    playerMsg.PlayerType = playerType;
                    if (Convert.ToInt64(comInfo[3]) == 1)
                    {
                        humanOrButcher = true;
                    }
                    else if (Convert.ToInt64(comInfo[3]) == 2)
                    {
                        humanOrButcher = false;
                    }
                    if (playerType == PlayerType.StudentPlayer)
                    {
                        switch (Convert.ToInt64(comInfo[4]))
                        {
                            case 1:
                                playerMsg.StudentType = StudentType.Athlete;
                                break;
                            case 2:
                                playerMsg.StudentType = StudentType.Teacher;
                                break;
                            case 3:
                                playerMsg.StudentType = StudentType.StraightAStudent;
                                break;
                            case 4:
                                playerMsg.StudentType = StudentType.Robot;
                                break;
                            case 5:
                                playerMsg.StudentType = StudentType.TechOtaku;
                                break;
                            case 6:
                                playerMsg.StudentType = StudentType.Sunshine;
                                break;
                            case 0:
                            default:
                                playerMsg.StudentType = StudentType.NullStudentType;
                                break;
                        }
                    }
                    else if (playerType == PlayerType.TrickerPlayer)
                    {
                        switch (Convert.ToInt64(comInfo[4]))
                        {
                            case 1:
                                playerMsg.TrickerType = TrickerType.Assassin;
                                break;
                            case 2:
                                playerMsg.TrickerType = TrickerType.Klee;
                                break;
                            case 3:
                                playerMsg.TrickerType = TrickerType.ANoisyPerson;
                                break;
                            case 4:
                                playerMsg.TrickerType = TrickerType.Idol;
                                break;
                            case 0:
                            default:
                                playerMsg.TrickerType = TrickerType.NullTrickerType;
                                break;
                        }
                    }
                }
                responseStream = client.AddPlayer(playerMsg);
                Connect.Background = Brushes.Transparent;
                isClientStocked = false;
                PorC.Content = "⏸";
                // 建立连接的同时加入人物
            }
        }

        // 绘制道具
        private void DrawProp(MessageOfProp data, string text)
        {
            TextBox icon = new()
            {
                FontSize = 7 * unitFontsize,
                Width = unitWidth,
                Height = unitHeight,
                Text = text,
                HorizontalAlignment = HorizontalAlignment.Left,
                VerticalAlignment = VerticalAlignment.Top,
                Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                Background = Brushes.Transparent,
                BorderBrush = Brushes.Transparent,
                IsReadOnly = true
            };
            UpperLayerOfMap.Children.Add(icon);
        }

        private void ZoomMap(object sender, SizeChangedEventArgs e)
        {
            unit = Math.Sqrt(UpperLayerOfMap.ActualHeight * UpperLayerOfMap.ActualWidth) / 50;
            unitFontsize = unit / 13;
            unitHeight = UpperLayerOfMap.ActualHeight / 50;
            unitWidth = UpperLayerOfMap.ActualWidth / 50;
            for (int i = 0; i < 50; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    if (mapPatches[i, j] != null)
                    {
                        mapPatches[i, j].Width = unitWidth;
                        mapPatches[i, j].Height = unitHeight;
                        mapPatches[i, j].HorizontalAlignment = HorizontalAlignment.Left;
                        mapPatches[i, j].VerticalAlignment = VerticalAlignment.Top;
                        mapPatches[i, j].Margin = new Thickness(unitWidth * j, unitHeight * i, 0, 0);
                    }
                }
            }
        }

        private void DrawMap()
        {
            classroomArray = new TextBlock[countList[0]];
            doorArray = new TextBlock[countList[1]];
            chestArray = new TextBlock[countList[2]];
            gateArray = new TextBlock[countList[3]];
            classroomPositionIndex = new int[countList[0]];
            doorPositionIndex = new int[countList[1]];
            chestPositionIndex = new int[countList[2]];
            gatePositionIndex = new int[countList[3]];
            int cntOfClassroom = 0, cntOfDoor = 0, cntOfGate = 0, cntOfChest = 0;
            for (int i = 0; i < defaultMap.GetLength(0); i++)
            {
                for (int j = 0; j < defaultMap.GetLength(1); j++)
                {
                    mapPatches[i, j] = new()
                    {
                        Width = unitWidth,
                        Height = unitHeight,
                        HorizontalAlignment = HorizontalAlignment.Left,
                        VerticalAlignment = VerticalAlignment.Top,
                        Margin = new Thickness(unitWidth * j, unitHeight * i, 0, 0)//unitWidth cannot be replaced by Width
                    };
                    switch (defaultMap[i, j])
                    {
                        case 6:
                            mapPatches[i, j].Fill = Brushes.Brown;
                            mapPatches[i, j].Stroke = Brushes.Brown;
                            break;//wall
                        case 7:
                            mapPatches[i, j].Fill = Brushes.Green;
                            mapPatches[i, j].Stroke = Brushes.Green;
                            break;//grass
                        case 8:
                            mapPatches[i, j].Fill = Brushes.LightPink;
                            mapPatches[i, j].Stroke = Brushes.LightPink;
                            classroomPositionIndex[cntOfClassroom] = 50 * i + j;
                            classroomArray[cntOfClassroom] = new TextBlock()
                            {
                                FontSize = 8 * unitFontsize,//
                                Width = unitWidth,//
                                Height = unitHeight,//
                                Text = Convert.ToString(-1),//
                                TextAlignment = TextAlignment.Center,
                                HorizontalAlignment = HorizontalAlignment.Left,
                                VerticalAlignment = VerticalAlignment.Top,
                                Margin = new Thickness(j * unitWidth / 1000.0 - unitWidth / 2, i * unitHeight / 1000.0 - unitHeight / 2, 0, 0),//
                                Background = Brushes.Transparent,
                            };
                            ++cntOfClassroom;
                            break;//classroom
                        case 9:
                            mapPatches[i, j].Fill = Brushes.LightSkyBlue;
                            mapPatches[i, j].Stroke = Brushes.LightSkyBlue;
                            gatePositionIndex[cntOfGate] = 50 * i + j;
                            gateArray[cntOfGate] = new TextBlock()
                            {
                                FontSize = 8 * unitFontsize,
                                Width = unitWidth,
                                Height = unitHeight,
                                Text = Convert.ToString(-1),
                                TextAlignment = TextAlignment.Center,
                                HorizontalAlignment = HorizontalAlignment.Left,
                                VerticalAlignment = VerticalAlignment.Top,
                                Margin = new Thickness(j * unitWidth / 1000.0 - unitWidth / 2, i * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                Background = Brushes.Transparent,
                            };
                            ++cntOfGate;
                            break;//gate
                        case 10:
                            break;//emergency
                        case 11:
                            mapPatches[i, j].Fill = Brushes.Gray;
                            mapPatches[i, j].Stroke = Brushes.Gray;
                            break;//window
                        case 12:
                        case 13:
                        case 14:
                            mapPatches[i, j].Fill = Brushes.Khaki;
                            mapPatches[i, j].Stroke = Brushes.Khaki;
                            doorPositionIndex[cntOfDoor] = 50 * i + j;
                            doorArray[cntOfDoor] = new TextBlock()
                            {
                                FontSize = 9 * unitFontsize,
                                Width = unitWidth,
                                Height = unitHeight,
                                Text = Convert.ToString(-1),
                                TextAlignment = TextAlignment.Center,
                                HorizontalAlignment = HorizontalAlignment.Left,
                                VerticalAlignment = VerticalAlignment.Top,
                                Margin = new Thickness(j * unitWidth / 1000.0 - unitWidth / 2, i * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                Background = Brushes.Transparent,
                            };
                            ++cntOfDoor;
                            break;//door
                        case 15:
                            mapPatches[i, j].Fill = Brushes.Orange;
                            mapPatches[i, j].Stroke = Brushes.Orange;
                            chestPositionIndex[cntOfChest] = 50 * i + j;
                            chestArray[cntOfChest] = new TextBlock()
                            {
                                FontSize = 8 * unitFontsize,
                                Width = unitWidth,
                                Height = unitHeight,
                                Text = Convert.ToString(-1),
                                TextAlignment = TextAlignment.Center,
                                HorizontalAlignment = HorizontalAlignment.Left,
                                VerticalAlignment = VerticalAlignment.Top,
                                Margin = new Thickness(j * unitWidth / 1000.0 - unitWidth / 2, i * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                Background = Brushes.Transparent,
                            };
                            ++cntOfChest;
                            break;//chest
                        default:
                            break;
                    }
                    UnderLayerOfMap.Children.Add(mapPatches[i, j]);
                }
            }
            hasDrawed = true;
        }

        //三个参数分别为攻击者的位置，攻击方位角（窗口坐标）和攻击半径
        private void DrawLaser(Point source, double theta, double range, double Width)  // 三个参数分别为攻击者的位置，攻击方位角（窗口坐标）和攻击半径
        {
            Point[] endPoint = new Point[4];
            Point target = new();
            target.X = source.X + range * Math.Cos(theta);
            target.Y = source.Y + range * Math.Sin(theta);
            endPoint[0].X = source.X + Width * Math.Cos(theta - Math.PI / 2);
            endPoint[0].Y = source.Y + Width * Math.Sin(theta - Math.PI / 2);
            endPoint[1].X = target.X + Width * Math.Cos(theta - Math.PI / 2);
            endPoint[1].Y = target.Y + Width * Math.Sin(theta - Math.PI / 2);
            endPoint[2].X = target.X + Width * Math.Cos(theta + Math.PI / 2);
            endPoint[2].Y = target.Y + Width * Math.Sin(theta + Math.PI / 2);
            endPoint[3].X = source.X + Width * Math.Cos(theta + Math.PI / 2);
            endPoint[3].Y = source.Y + Width * Math.Sin(theta + Math.PI / 2);
            Polygon laserIcon = new();
            laserIcon.Stroke = System.Windows.Media.Brushes.Red;
            laserIcon.Fill = System.Windows.Media.Brushes.Red;
            laserIcon.StrokeThickness = 2;
            laserIcon.HorizontalAlignment = HorizontalAlignment.Left;
            laserIcon.VerticalAlignment = VerticalAlignment.Top;
            PointCollection laserEndPoints = new();
            for (int i = 0; i < 4; i++)
            {
                laserEndPoints.Add(endPoint[i]);
            }
            laserIcon.Points = laserEndPoints;
            UpperLayerOfMap.Children.Add(laserIcon);
        }

        private async void OnReceive()  // 已按照3.5版proto更新信息，但是左侧信息栏还未填充。log未更新,switch1,2,3更新log
        {
            try
            {
                while (responseStream != null && await responseStream.ResponseStream.MoveNext())
                {
                    lock (drawPicLock)  // 加锁是必要的，画图操作和接收信息操作不能同时进行，否则画图时foreach会有bug
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
                        MessageToClient content = responseStream.ResponseStream.Current;
                        MessageOfMap mapMessage = new MessageOfMap();
                        bool mapMessageExist = false;
                        switch (content.GameState)
                        {
                            case GameState.GameStart:
                                foreach (var obj in content.ObjMessage)
                                {
                                    switch (obj.MessageOfObjCase)
                                    {
                                        case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                            if (humanOrButcher && obj.StudentMessage.PlayerId == playerID)
                                            {
                                                human = obj.StudentMessage;
                                            }
                                            listOfHuman.Add(obj.StudentMessage);
                                            break;
                                        case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                            if (!humanOrButcher && obj.TrickerMessage.PlayerId == playerID)
                                            {
                                                butcher = obj.TrickerMessage;
                                            }
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
                                        case MessageOfObj.MessageOfObjOneofCase.MapMessage:
                                            mapMessage = obj.MapMessage;
                                            break;
                                    }
                                }
                                listOfAll.Add(content.AllMessage);
                                countList.Clear();
                                countList.Add(listOfClassroom.Count);
                                countList.Add(listOfDoor.Count);
                                countList.Add(listOfChest.Count);
                                countList.Add(listOfGate.Count);
                                GetMap(mapMessage);
                                break;
                            case GameState.GameRunning:
                                foreach (var obj in content.ObjMessage)
                                {
                                    switch (obj.MessageOfObjCase)
                                    {
                                        case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                            if (humanOrButcher && obj.StudentMessage.PlayerId == playerID)
                                            {
                                                human = obj.StudentMessage;
                                            }
                                            listOfHuman.Add(obj.StudentMessage);
                                            break;
                                        case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                            if (!humanOrButcher && obj.TrickerMessage.PlayerId == playerID)
                                            {
                                                butcher = obj.TrickerMessage;
                                            }
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
                                            mapMessage = obj.MapMessage;
                                            mapMessageExist = true;//只有中间加入游戏的旁观者着一种可能，使得在这里收到地图
                                            break;
                                    }
                                }
                                listOfAll.Add(content.AllMessage);
                                if (mapMessageExist)
                                {
                                    countList.Clear();
                                    countList.Add(listOfClassroom.Count);
                                    countList.Add(listOfDoor.Count);
                                    countList.Add(listOfChest.Count);
                                    countList.Add(listOfGate.Count);
                                    GetMap(mapMessage);
                                    mapMessageExist = false;
                                }
                                break;
                            case GameState.GameEnd:
                                MessageBox.Show("Game Over!");
                                foreach (var obj in content.ObjMessage)
                                {
                                    switch (obj.MessageOfObjCase)
                                    {
                                        case MessageOfObj.MessageOfObjOneofCase.StudentMessage:
                                            if (humanOrButcher && obj.StudentMessage.PlayerId == playerID)
                                            {
                                                human = obj.StudentMessage;
                                            }
                                            listOfHuman.Add(obj.StudentMessage);
                                            break;
                                        case MessageOfObj.MessageOfObjOneofCase.TrickerMessage:
                                            if (!humanOrButcher && obj.TrickerMessage.PlayerId == playerID)
                                            {
                                                butcher = obj.TrickerMessage;
                                            }
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
                                    }
                                }
                                listOfAll.Add(content.AllMessage);
                                break;
                        }
                    }
                    if (responseStream == null)
                    {
                        throw new Exception("Unconnected");
                    }
                }
            }
            catch (Exception ex)
            {
                ErrorDisplayer error = new("Error: " + ex.ToString());
                error.Show();
            }
        }

        private bool CanSee(MessageOfStudent msg)
        {
            if (msg.PlayerState == PlayerState.Quit || msg.PlayerState == PlayerState.Graduated)
                return false;
            //if (isSpectatorMode || isPlaybackMode)
            //    return true;
            //if (humanOrButcher && human != null)
            //{
            //    if (msg.Place == human.Place)
            //        return true;
            //    if (msg.PlayerId == playerID + Preparation.Utility.GameData.numOfPeople)//robot and its owner
            //        return true;
            //}
            //else if (!humanOrButcher && butcher != null)
            //{
            //    if (msg.Place == butcher.Place)
            //        return true;
            //}
            //if (msg.Place == Protobuf.PlaceType.Grass)
            //    return false;
            return true;
        }

        private bool CanSee(MessageOfTricker msg)
        {
            //if (isSpectatorMode || isPlaybackMode)
            //    return true;
            //if (!humanOrButcher && butcher != null)
            //{
            //    if (butcher.Guid == msg.Guid)  // 自己能看见自己
            //        return true;
            //}
            //if (humanOrButcher && human != null)
            //{
            //    if (msg.TrickerType == Protobuf.TrickerType.Assassin)
            //    {
            //        foreach (var buff in msg.Buff)
            //        {
            //            if (buff == Protobuf.TrickerBuffType.TrickerInvisible)
            //                return false;
            //        }
            //    }
            //    if (msg.Place == human.Place)
            //        return true;
            //}
            //if (msg.Place == Protobuf.PlaceType.Grass)
            //    return false;
            return true;
        }

        private bool CanSee(MessageOfProp msg)
        {
            //if (isSpectatorMode || isPlaybackMode)
            //    return true;
            //if (humanOrButcher && human != null)
            //{
            //    if (msg.Place == human.Place)
            //        return true;
            //}
            //else if (!humanOrButcher && butcher != null)
            //{
            //    if (msg.Place == butcher.Place)
            //        return true;
            //}
            //if (msg.Place == Protobuf.PlaceType.Grass)
            //    return false;
            return true;
        }

        private bool CanSee(MessageOfBullet msg)
        {
            //if (isSpectatorMode || isPlaybackMode)
            //    return true;
            //if (humanOrButcher && human != null)
            //{
            //    if (msg.Place == human.Place)
            //        return true;
            //}
            //else if (!humanOrButcher && butcher != null)
            //{
            //    if (msg.Place == butcher.Place)
            //        return true;
            //}
            //if (msg.Place == Protobuf.PlaceType.Grass)
            //    return false;
            return true;
        }

        private bool CanSee(MessageOfBombedBullet msg)
        {
            //if (isSpectatorMode || isPlaybackMode)
            //    return true;
            //if (humanOrButcher && human != null)
            //{
            //    if (msg.Place == human.Place)
            //        return true;
            //}
            //else if (!humanOrButcher && butcher != null)
            //{
            //    if (msg.Place == butcher.Place)
            //        return true;
            //}
            //if (msg.Place == Protobuf.PlaceType.Grass)
            //    return false;
            return true;
        }

        private int FindIndexOfClassroom(MessageOfClassroom msg)
        {
            for (int i = 0; i < classroomPositionIndex.Length; ++i)
            {
                int k = msg.X / 1000 * 50 + msg.Y / 1000;
                if (k == classroomPositionIndex[i])
                    return i;
            }
            return -1;
        }

        private int FindIndexOfGate(MessageOfGate msg)
        {
            for (int i = 0; i < gatePositionIndex.Length; ++i)
            {
                int k = msg.X / 1000 * 50 + msg.Y / 1000;
                if (k == gatePositionIndex[i])
                    return i;
            }
            return -1;
        }

        private int FindIndexOfDoor(MessageOfDoor msg)
        {
            for (int i = 0; i < doorPositionIndex.Length; ++i)
            {
                int k = msg.X / 1000 * 50 + msg.Y / 1000;
                if (k == doorPositionIndex[i])
                    return i;
            }
            return -1;
        }

        private int FindIndexOfChest(MessageOfChest msg)
        {
            for (int i = 0; i < chestPositionIndex.Length; ++i)
            {
                int k = msg.X / 1000 * 50 + msg.Y / 1000;
                if (k == chestPositionIndex[i])
                    return i;
            }
            return -1;
        }

        private void Refresh(object? sender, EventArgs e) //log未更新
        {
            try
            {
                lock (drawPicLock)  // 加锁是必要的，画图操作和接收信息操作不能同时进行
                {
                    // Bonus();
                    if (WindowState == WindowState.Maximized)
                        MaxButton.Content = "❐";
                    else
                        MaxButton.Content = "🗖";
                    foreach (var obj in listOfHuman)
                    {
                        if (obj.PlayerId < GameData.numOfStudent && !isDataFixed[obj.PlayerId])
                        {
                            IStudentType occupation = (IStudentType)OccupationFactory.FindIOccupation(Transformation.ToStudentType(obj.StudentType));
                            totalLife[obj.PlayerId] = occupation.MaxHp;
                            totalDeath[obj.PlayerId] = occupation.MaxGamingAddiction;
                            int i = 0;
                            foreach (var skill in occupation.ListOfIActiveSkill)
                            {
                                var iActiveSkill = SkillFactory.FindActiveSkill(skill);
                                coolTime[i, obj.PlayerId] = (int)(iActiveSkill.SkillCD.GetCD());
                                ++i;
                            }
                            isDataFixed[obj.PlayerId] = true;
                        }
                    }
                    foreach (var obj in listOfButcher)
                    {
                        if (obj.PlayerId < GameData.numOfPeople && !isDataFixed[obj.PlayerId])
                        {
                            IGhostType occupation1 = (IGhostType)OccupationFactory.FindIOccupation(Transformation.ToTrickerType(obj.TrickerType));
                            int j = 0;
                            foreach (var skill in occupation1.ListOfIActiveSkill)
                            {
                                var iActiveSkill = SkillFactory.FindActiveSkill(skill);
                                coolTime[j, GameData.numOfStudent] = (int)(iActiveSkill.SkillCD.GetCD());
                                ++j;
                            }
                            isDataFixed[obj.PlayerId] = true;
                        }
                    }

                    for (int i = 0; i < GameData.numOfStudent; i++)
                    {
                        StatusBarsOfSurvivor[i].NewData(totalLife, totalDeath, coolTime);
                    }

                    StatusBarsOfHunter.NewData(totalLife, totalDeath, coolTime);

                    for (int i = 0; i < GameData.numOfStudent; i++)
                    {
                        StatusBarsOfSurvivor[i].SetFontSize(12 * unitFontsize);
                    }

                    StatusBarsOfHunter.SetFontSize(12 * unitFontsize);
                    StatusBarsOfCircumstance.SetFontSize(12 * unitFontsize);
                    if (!isClientStocked)
                    {
                        UpperLayerOfMap.Children.Clear();
                        foreach (var data in listOfAll)
                        {
                            StatusBarsOfCircumstance.SetValue(data, gateOpened, isEmergencyDrawed, isEmergencyOpened, playerID, isPlaybackMode);
                        }
                        if (!hasDrawed && mapFlag)
                        {
                            DrawMap();
                        }
                        foreach (var data in listOfHuman)
                        {
                            if (data.PlayerId < GameData.numOfStudent)
                                StatusBarsOfSurvivor[data.PlayerId].SetValue(data, data.PlayerId);
                            if (CanSee(data))
                            {
                                Ellipse icon = new()
                                {
                                    Width = 2 * radiusTimes * unitWidth,
                                    Height = 2 * radiusTimes * unitHeight,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * radiusTimes, data.X * unitHeight / 1000.0 - unitHeight * radiusTimes, 0, 0),
                                    Fill = Brushes.BlueViolet,
                                };
                                if (data.StudentType == StudentType.Robot)
                                    icon.Fill = Brushes.Gray;
                                TextBlock num = new()
                                {
                                    FontSize = 7 * unitFontsize,
                                    Width = 2 * radiusTimes * unitWidth,
                                    Height = 2 * radiusTimes * unitHeight,
                                    Text = Convert.ToString(data.PlayerId),
                                    TextAlignment = TextAlignment.Center,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * radiusTimes, data.X * unitHeight / 1000.0 - unitHeight * radiusTimes, 0, 0),
                                    Background = Brushes.Transparent,
                                    Foreground = Brushes.White,
                                };
                                //if (data.StudentType == StudentType.Robot)
                                //    num.Text = Convert.ToString(data.PlayerId - Preparation.Utility.GameData.numOfPeople);
                                UpperLayerOfMap.Children.Add(icon);
                                UpperLayerOfMap.Children.Add(num);
                            }
                        }
                        foreach (var data in listOfButcher)
                        {
                            StatusBarsOfHunter.SetValue(data);
                            if (CanSee(data))
                            {
                                Ellipse icon = new()
                                {
                                    Width = 2 * radiusTimes * unitWidth,
                                    Height = 2 * radiusTimes * unitHeight,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * radiusTimes, data.X * unitHeight / 1000.0 - unitHeight * radiusTimes, 0, 0),
                                    Fill = Brushes.Chocolate,
                                };
                                UpperLayerOfMap.Children.Add(icon);
                            }
                        }
                        foreach (var data in listOfProp)
                        {
                            if (CanSee(data))
                            {
                                switch (data.Type)
                                {
                                    case Protobuf.PropType.Key3:
                                        DrawProp(data, "🔑");
                                        break;
                                    case Protobuf.PropType.Key5:
                                        DrawProp(data, "🔑");
                                        break;
                                    case Protobuf.PropType.Key6:
                                        DrawProp(data, "🔑");
                                        break;
                                    case Protobuf.PropType.AddSpeed:
                                        DrawProp(data, "⛸");
                                        break;
                                    case Protobuf.PropType.AddHpOrAp:
                                        DrawProp(data, "♥");
                                        break;
                                    case Protobuf.PropType.AddLifeOrClairaudience:
                                        DrawProp(data, "🏅");
                                        break;
                                    case Protobuf.PropType.ShieldOrSpear:
                                        DrawProp(data, "🛡");
                                        break;
                                    case Protobuf.PropType.RecoveryFromDizziness:
                                        DrawProp(data, "🕶");
                                        break;
                                    case Protobuf.PropType.CraftingBench:
                                        DrawProp(data, "🎰");
                                        break;
                                    default:
                                        DrawProp(data, "");
                                        break;
                                }
                            }
                        }
                        foreach (var data in listOfBullet)
                        {
                            if (CanSee(data))
                            {
                                Ellipse icon = new()
                                {
                                    Width = 2 * bulletRadiusTimes * unitWidth,
                                    Height = 2 * bulletRadiusTimes * unitHeight,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * bulletRadiusTimes, data.X * unitHeight / 1000.0 - unitHeight * bulletRadiusTimes, 0, 0),
                                    Fill = Brushes.Red,
                                };
                                switch (data.Type)
                                {
                                    case Protobuf.BulletType.FlyingKnife:
                                        icon.Fill = Brushes.Blue;
                                        break;
                                    case Protobuf.BulletType.CommonAttackOfTricker:
                                    case Protobuf.BulletType.BombBomb:
                                    case Protobuf.BulletType.JumpyDumpty:
                                    case Protobuf.BulletType.Strike:
                                        icon.Fill = Brushes.Red;
                                        break;
                                    default:
                                        break;
                                }
                                UpperLayerOfMap.Children.Add(icon);
                            }
                        }
                        foreach (var data in listOfBombedBullet)
                        {
                            if (CanSee(data))
                            {
                                switch (data.Type)
                                {
                                    case Protobuf.BulletType.BombBomb:
                                        {
                                            double bombRange = 1.0 * data.BombRange / Preparation.Utility.GameData.numOfPosGridPerCell;
                                            Ellipse icon = new()
                                            {
                                                Width = 2 * bombRange * unitWidth,
                                                Height = 2 * bombRange * unitHeight,
                                                HorizontalAlignment = HorizontalAlignment.Left,
                                                VerticalAlignment = VerticalAlignment.Top,
                                                Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * bombRange, data.X * unitHeight / 1000.0 - unitHeight * bombRange, 0, 0),
                                                Fill = Brushes.DarkRed,
                                            };
                                            UpperLayerOfMap.Children.Add(icon);
                                            break;
                                        }
                                    case Protobuf.BulletType.JumpyDumpty:
                                        {
                                            double bombRange = 1.0 * data.BombRange / Preparation.Utility.GameData.numOfPosGridPerCell;
                                            Ellipse icon = new()
                                            {
                                                Width = 2 * bombRange * unitWidth,
                                                Height = 2 * bombRange * unitHeight,
                                                HorizontalAlignment = HorizontalAlignment.Left,
                                                VerticalAlignment = VerticalAlignment.Top,
                                                Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth * bombRange, data.X * unitHeight / 1000.0 - unitHeight * bombRange, 0, 0),
                                                Fill = Brushes.DarkRed,
                                            };
                                            UpperLayerOfMap.Children.Add(icon);
                                            break;
                                        }
                                    default:
                                        break;
                                }
                            }
                        }
                        foreach (var data in listOfClassroom)
                        {
                            int deg = (int)(100.0 * data.Progress / Preparation.Utility.GameData.degreeOfFixedGenerator);
                            int idx = FindIndexOfClassroom(data);
                            classroomArray[idx].FontSize = 8 * unitFontsize;
                            classroomArray[idx].Width = unitWidth;
                            classroomArray[idx].Height = unitHeight;
                            classroomArray[idx].Text = Convert.ToString(deg);
                            classroomArray[idx].Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0);
                            if (deg == 100)
                            {
                                classroomArray[idx].Text = "A+";
                            }
                            UpperLayerOfMap.Children.Add(classroomArray[idx]);
                        }
                        foreach (var data in listOfChest)
                        {
                            int deg = (int)(100.0 * data.Progress / Preparation.Utility.GameData.degreeOfOpenedChest);
                            int idx = FindIndexOfChest(data);
                            chestArray[idx].FontSize = 8 * unitFontsize;
                            chestArray[idx].Width = unitWidth;
                            chestArray[idx].Height = unitHeight;
                            chestArray[idx].Text = Convert.ToString(deg);
                            chestArray[idx].Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0);
                            if (deg == 100)
                            {
                                chestArray[idx].Text = "Ø";
                            }
                            UpperLayerOfMap.Children.Add(chestArray[idx]);
                        }
                        foreach (var data in listOfGate)
                        {
                            int deg = (int)(100.0 * data.Progress / Preparation.Utility.GameData.degreeOfOpenedDoorway);
                            int idx = FindIndexOfGate(data);
                            gateArray[idx].FontSize = 8 * unitFontsize;
                            gateArray[idx].Width = unitWidth;
                            gateArray[idx].Height = unitHeight;
                            gateArray[idx].Text = Convert.ToString(deg);
                            gateArray[idx].Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0);
                            if (deg == 100)
                            {
                                gateOpened = true;
                                gateArray[idx].Text = "🔓";
                            }
                            UpperLayerOfMap.Children.Add(gateArray[idx]);
                        }
                        foreach (var data in listOfDoor)
                        {
                            int idx = FindIndexOfDoor(data);
                            doorArray[idx].FontSize = 9 * unitFontsize;
                            doorArray[idx].Width = unitWidth;
                            doorArray[idx].Height = unitHeight;
                            doorArray[idx].Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0);
                            if (data.IsOpen)
                            {
                                doorArray[idx].Text = Convert.ToString("🔓");
                            }
                            else
                            {
                                doorArray[idx].Text = Convert.ToString("🔒");
                            }
                            UpperLayerOfMap.Children.Add(doorArray[idx]);
                        }
                        foreach (var data in listOfHiddenGate)
                        {
                            if (!isEmergencyDrawed)
                            {
                                mapPatches[data.X / Preparation.Utility.GameData.numOfPosGridPerCell, data.Y / Preparation.Utility.GameData.numOfPosGridPerCell].Fill = Brushes.LightSalmon;
                                mapPatches[data.X / Preparation.Utility.GameData.numOfPosGridPerCell, data.Y / Preparation.Utility.GameData.numOfPosGridPerCell].Stroke = Brushes.LightSalmon;
                                isEmergencyDrawed = true;
                            }
                            if (data.Opened)
                            {
                                isEmergencyOpened = true;
                                hiddenGateArray = new TextBlock()
                                {
                                    FontSize = 9 * unitFontsize,
                                    Width = unitWidth,
                                    Height = unitHeight,
                                    Text = Convert.ToString("🔓"),
                                    TextAlignment = TextAlignment.Center,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                    Background = Brushes.Transparent,
                                };
                                UpperLayerOfMap.Children.Add(hiddenGateArray);
                            }
                        }
                    }
                    counter++;
                }
            }
            catch (Exception exc)
            {
                ErrorDisplayer error = new("Error: " + exc.ToString());
                error.Show();
                isClientStocked = true;
                PorC.Content = "▶";
            }
        }

        // 键盘控制
        private void KeyBoardControl(object sender, KeyEventArgs e)
        {
            if (!isPlaybackMode && !isSpectatorMode)
            {
                if (client is null)
                {
                    return;
                }

                switch (e.Key)
                {
                    case Key.W:
                    case Key.NumPad8:
                        MoveMsg msgW = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 25,
                            Angle = Math.PI
                        };
                        client.Move(msgW);
                        break;
                    case Key.S:
                    case Key.NumPad2:
                        MoveMsg msgS = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 25,
                            Angle = 0
                        };
                        client.Move(msgS);
                        break;
                    case Key.D:
                    case Key.NumPad6:
                        MoveMsg msgD = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 25,
                            Angle = Math.PI / 2
                        };
                        client.Move(msgD);
                        break;
                    case Key.A:
                    case Key.NumPad4:
                        MoveMsg msgA = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 25,
                            Angle = 3 * Math.PI / 2
                        };
                        client.Move(msgA);
                        break;
                    case Key.J:
                        AttackMsg msgJ = new()
                        {
                            PlayerId = playerID,
                            Angle = Math.PI
                        };
                        client.Attack(msgJ);
                        break;
                    case Key.K:
                        IDMsg msgK = new()
                        {
                            PlayerId = playerID,
                        };
                        client.StartLearning(msgK);
                        break;
                    case Key.R:
                        TreatAndRescueMsg msgR = new()
                        {
                            PlayerId = playerID,
                            ToPlayerId = -1,
                        };
                        client.StartRescueMate(msgR);
                        break;
                    case Key.T:
                        TreatAndRescueMsg msgT = new()
                        {
                            PlayerId = playerID,
                            ToPlayerId = -1,
                        };
                        client.StartTreatMate(msgT);
                        break;
                    case Key.G:
                        IDMsg msgG = new()
                        {
                            PlayerId = playerID,
                        };
                        client.Graduate(msgG);
                        break;
                    case Key.H:
                        IDMsg msgH = new()
                        {
                            PlayerId = playerID,
                        };
                        client.StartOpenGate(msgH);
                        break;
                    case Key.O:
                        IDMsg msgO = new()
                        {
                            PlayerId = playerID,
                        };
                        client.OpenDoor(msgO);
                        break;
                    case Key.P:
                        IDMsg msgP = new()
                        {
                            PlayerId = playerID,
                        };
                        client.CloseDoor(msgP);
                        break;
                    case Key.U:
                        IDMsg msgU = new()
                        {
                            PlayerId = playerID,
                        };
                        client.SkipWindow(msgU);
                        break;
                    case Key.I:
                        IDMsg msgI = new()
                        {
                            PlayerId = playerID,
                        };
                        client.StartOpenChest(msgI);
                        break;
                    case Key.E:
                        IDMsg msgE = new()
                        {
                            PlayerId = playerID,
                        };
                        client.EndAllAction(msgE);
                        break;
                    case Key.F:
                        PropMsg msgF = new()
                        {
                            PlayerId = playerID,
                            PropType = Protobuf.PropType.NullPropType,
                        };
                        client.PickProp(msgF);
                        break;
                    case Key.C:
                        PropMsg msgC = new()
                        {
                            PlayerId = playerID,
                            PropType = Protobuf.PropType.NullPropType,
                        };
                        client.ThrowProp(msgC);
                        break;
                    case Key.V:
                        PropMsg msgV = new()
                        {
                            PlayerId = playerID,
                            PropType = Protobuf.PropType.NullPropType,
                        };
                        client.UseProp(msgV);
                        break;
                    case Key.B:
                        SkillMsg msgB = new()
                        {
                            PlayerId = playerID,
                            SkillId = 0,
                            SkillParam = 0,
                        };
                        client.UseSkill(msgB);
                        break;
                    case Key.N:
                        SkillMsg msgN = new()
                        {
                            PlayerId = playerID,
                            SkillId = 1,
                        };
                        client.UseSkill(msgN);
                        break;
                    case Key.M:
                        SkillMsg msgM = new()
                        {
                            PlayerId = playerID,
                            SkillId = 2,
                        };
                        client.UseSkill(msgM);
                        break;
                    default:
                        break;
                }
            }
        }

        //鼠标双击
        private void Attack(object sender, RoutedEventArgs e)
        {
            if (!isPlaybackMode && !isSpectatorMode)
            {
                if (client is null)
                {
                    return;
                }
                if (humanOrButcher && human != null)
                {
                    AttackMsg msgJ = new()
                    {
                        PlayerId = playerID
                    };
                    double mouseY = Mouse.GetPosition(UpperLayerOfMap).X * 1000 / unitWidth;
                    double mouseX = Mouse.GetPosition(UpperLayerOfMap).Y * 1000 / unitHeight;
                    msgJ.Angle = Math.Atan2(mouseY - human.Y, mouseX - human.X);
                    client.Attack(msgJ);
                }
                if (!humanOrButcher && butcher != null)
                {
                    AttackMsg msgJ = new()
                    {
                        PlayerId = playerID
                    };
                    double mouseY = Mouse.GetPosition(UpperLayerOfMap).X * 1000 / unitWidth;
                    double mouseX = Mouse.GetPosition(UpperLayerOfMap).Y * 1000 / unitHeight;
                    msgJ.Angle = Math.Atan2(mouseY - butcher.Y, mouseX - butcher.X);
                    client.Attack(msgJ);
                }
            }
        }

        private void ClickToPauseOrContinue(object sender, RoutedEventArgs e)
        {
            if (!isClientStocked)
            {
                isClientStocked = true;
                PorC.Content = "▶";
            }
            else
            {
                try
                {
                    isClientStocked = false;
                    PorC.Content = "⏸";
                }
                catch (Exception ex)
                {
                    ErrorDisplayer error = new("发生错误。以下是系统报告:\n" + ex.ToString());
                    error.Show();
                }
            }
        }

        // 未复现
        private void ClickToConnect(object sender, RoutedEventArgs e)
        {
        }

        // 窗口最大化、关闭、最小化、拖拽
        private void ClickToMaxmize(object sender, RoutedEventArgs e)
        {
            if (WindowState != WindowState.Maximized)
                WindowState = WindowState.Maximized;
            else
                WindowState = WindowState.Normal;
        }

        private void ClickToClose(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void ClickToMinimize(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        private void DragWindow(object sender, RoutedEventArgs e)
        {
            DragMove();
        }

        // 寻求帮助、访问EESAST（部分功能未复现）
        private void ClickForHelp(object sender, RoutedEventArgs e)
        {
            PleaseWait();
        }

        private void ClickToVisitEESAST(object sender, RoutedEventArgs e)
        {
            try
            {
                _ = Process.Start("C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe", "https://eesast.com");
            }
            catch (Exception ex)
            {
                ErrorDisplayer error = new("发生错误。以下是系统报告\n" + ex.ToString());
                error.Show();
            }
        }

        // 配置连接（未复现）、我的AI（THUAI5未实现）、获取更新、天梯信息（可能需要网站协助）
        private void ClickToSetConnect(object sender, RoutedEventArgs e)
        {
            //           ConnectRegister crg = new();
            //           crg.Show();
        }

        private void ClickToEnterVS(object sender, RoutedEventArgs e)
        {
            // try
            //{
            //     if (!File.Exists("VSRoute.txt"))
            //     {
            //         File.Create("VSRoute.txt");
            //         Exception ex = new("没有路径存储文件，已为您创建。请将VS路径输入该文件，并重新操作。");
            //         throw ex;
            //     }//创建路径文件
            //     using StreamReader sr = new("VSRoute.txt");
            //     _ = Process.Start(sr.ReadLine());
            // }
            // catch (Exception exc)
            //{
            //     ErrorDisplayer error = new("发生错误。以下是系统报告:\n" + exc.ToString());
            //     error.Show();
            // }
            PleaseWait();
        }

        private void ClickForUpdate(object sender, RoutedEventArgs e)
        {
            PleaseWait();
        }

        private void ClickToCheckLadder(object sender, RoutedEventArgs e)
        {
            PleaseWait();
        }

        // 敬请期待函数
        private void PleaseWait()
        {
            try
            {
                throw new Exception("敬请期待");
            }
            catch (Exception ex)
            {
                ErrorDisplayer error = new(ex.Message);
                error.Show();
            }
        }

        // 以下为Mainwindow自定义属性
        private readonly DispatcherTimer timer;  // 定时器
        private long counter;                    // 预留的取时间变量
        AvailableService.AvailableServiceClient? client;
        AsyncServerStreamingCall<MessageToClient>? responseStream;
        private StatusBarOfSurvivor[] StatusBarsOfSurvivor;
        private StatusBarOfHunter StatusBarsOfHunter;
        private StatusBarOfCircumstance StatusBarsOfCircumstance;

        private bool isClientStocked;
        private bool isPlaybackMode;

        private long playerID;
        private PlayerType playerType;

        private double unit;
        private double unitHeight;
        private double unitWidth;
        private readonly Rectangle[,] mapPatches = new Rectangle[50, 50];

        private List<MessageOfProp> listOfProp;
        private List<MessageOfStudent> listOfHuman;
        private List<MessageOfTricker> listOfButcher;
        private List<MessageOfBullet> listOfBullet;
        private List<MessageOfBombedBullet> listOfBombedBullet;
        private List<MessageOfAll> listOfAll;
        private List<MessageOfChest> listOfChest;
        private List<MessageOfClassroom> listOfClassroom;
        private List<MessageOfDoor> listOfDoor;
        private List<MessageOfGate> listOfGate;
        private List<MessageOfHiddenGate> listOfHiddenGate;

        private TextBlock[] classroomArray;
        private int[] classroomPositionIndex;
        private TextBlock[] chestArray;
        private int[] chestPositionIndex;
        private TextBlock[] doorArray;
        private int[] doorPositionIndex;
        private TextBlock[] gateArray;
        private int[] gatePositionIndex;
        private TextBlock hiddenGateArray;//make a map from the position of icons to the index
        private List<int> countList;

        private object drawPicLock = new object();
        private MessageOfStudent? human = null;
        private MessageOfTricker? butcher = null;
        private bool humanOrButcher;//true for human

        private bool mapFlag = false;
        private bool hasDrawed = false;
        public int[,] defaultMap = new int[,] {
            { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },//6墙,1-5出生点
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },//7草
            { 6, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6 },//8机
            { 6, 0, 0, 0, 0, 6, 0, 6, 7, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 0, 0, 6 },//9大门
            { 6, 0, 0, 0, 0, 6, 6, 6, 6, 7, 0, 0, 0, 0, 0, 15, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 15, 0, 0, 0, 6 },//10紧急出口
            { 6, 6, 0, 0, 0, 0, 9, 6, 6, 7, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 6, 6, 7, 7, 6, 6, 6, 6, 6, 6, 11, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },//11窗
            { 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 7, 7, 6, 6, 7, 7, 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 13, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6 },//12-14门
            { 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 7, 7, 7, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6 },//15箱
            { 6, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 6 },
            { 6, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 7, 7, 6, 0, 6 },
            { 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 7, 6, 0, 6 },
            { 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 6, 12, 6, 6, 6, 6, 6, 6, 11, 6, 6, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 6, 0, 6 },
            { 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 6 },
            { 6, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 7, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 6 },
            { 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6, 6, 7, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 11, 6, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 6 },
            { 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 6, 7, 0, 0, 6 },
            { 6, 7, 7, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 7, 7, 0, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 6 },
            { 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 0, 0, 5, 0, 7, 7, 6, 0, 0, 0, 0, 0, 0, 7, 6, 6, 6, 6, 15, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 6, 7, 7, 0, 0, 0, 0, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 7, 6, 0, 0, 0, 6 },
            { 6, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 7, 0, 0, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 6, 6, 0, 10, 0, 6 },
            { 6, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 6, 6, 6, 6, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 6, 7, 0, 2, 0, 0, 6 },
            { 6, 0, 6, 0, 0, 0, 0, 0, 0, 6, 11, 6, 6, 6, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 11, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 11, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 12, 6, 6, 6, 0, 0, 0, 0, 6, 6, 6, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 6, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 7, 7, 0, 0, 0, 0, 6 },
            { 6, 0, 6, 7, 0, 0, 0, 8, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 0, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 6, 6, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 7, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,6, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 7, 7, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 6, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 6, 6, 6, 6, 6, 7, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 6 },
            { 6, 6, 0, 0, 7, 7, 6, 7, 7, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 0, 0, 0, 0, 6 },
            { 6, 6, 15, 0, 0, 0, 7, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 11, 6, 0, 0, 0, 0, 0, 6 },
            { 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6,6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 15, 0, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0,8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 6, 7, 7, 0, 0, 0, 6, 6, 6, 11, 6, 0, 0, 6, 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 6, 0, 6, 7, 7, 6, 7, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 14, 6, 6, 6, 0, 0, 0, 0, 0, 7, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 0, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 7, 6, 0, 6, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 7, 6, 6, 6, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 6, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 6, 6, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 11, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 6, 6, 6, 6, 6, 7, 0, 0, 0, 10, 0, 0, 0, 0, 6, 6, 7, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 6, 0, 0, 0, 0, 7, 6, 6, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 6, 0, 0, 0, 7, 7, 6, 6, 0, 0, 0, 6 },
            { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 }
            };

        private string[] comInfo = new string[5];
        ArgumentOptions? options = null;
        bool gateOpened = false;
        bool isSpectatorMode = false;
        bool isEmergencyOpened = false;
        bool isEmergencyDrawed = false;
        bool[] isDataFixed = new bool[5] { false, false, false, false, false };
        double unitFontsize = 10;
        const double radiusTimes = 1.0 * Preparation.Utility.GameData.characterRadius / Preparation.Utility.GameData.numOfPosGridPerCell;
        const double bulletRadiusTimes = 1.0 * GameData.bulletRadius / Preparation.Utility.GameData.numOfPosGridPerCell;
        private int[] totalLife = new int[4] { 100, 100, 100, 100 }, totalDeath = new int[4] { 100, 100, 100, 100 };
        private int[,] coolTime = new int[3, 5] { { 100, 100, 100, 100, 100 }, { 100, 100, 100, 100, 100 }, { 100, 100, 100, 100, 100 } };
    }
}