using System;
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

// 目前MainWindow还未复现的功能：
// 部分errordisplayer
// private void ReactToCommandline()，
// private void Playback(string fileName, double pbSpeed = 2.0)

// 交互：private void ClickToSetMode(object sender, RoutedEventArgs e)

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
            unitHeight = unitWidth = unit = 13;
            bonusflag = true;
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
            drawPicLock = new();
            listOfProp = new List<MessageOfProp>();
            listOfHuman = new List<MessageOfHuman>();
            listOfButcher = new List<MessageOfButcher>();
            WindowStartupLocation = WindowStartupLocation.CenterScreen;
            comInfo[0] = "183.172.208.156";
            comInfo[1] = "8888";
            comInfo[2] = "0";
            comInfo[3] = "1";
            comInfo[4] = "1";

            //ConnectToServer(comInfo);
            //OnReceive();
            DrawMap();
            ZoomMap();
            MessageOfHuman kurei = new MessageOfHuman();
            kurei.X = 10000;
            kurei.Y = 20000;
            kurei.Speed = 1000;
            kurei.PlayerId = 0;
            listOfHuman.Add(kurei);
            // ReactToCommandline();
        }

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

        // 连接Server,comInfo[]的格式：0-ip 1- port 2-playerID 3-playerType 4-human/butcherType
        private void ConnectToServer(string[] comInfo)
        {
            if (!isPlaybackMode)
            {
                if (comInfo.Length != 5)
                    throw new Exception("注册信息有误！");
                playerID = Convert.ToInt64(comInfo[2]);
                Connect.Background = Brushes.Gray;
                string connect = new string(comInfo[0]);
                connect += ':';
                connect += comInfo[1];
                Channel channel = new Channel(connect, ChannelCredentials.Insecure);
                client = new AvailableService.AvailableServiceClient(channel);
                // 没判断连没连上

                PlayerMsg playerMsg = new PlayerMsg();
                playerMsg.PlayerId = playerID;
                playerType = Convert.ToInt64(comInfo[3]) switch
                {
                    0 => PlayerType.NullPlayerType,
                    1 => PlayerType.HumanPlayer,
                    2 => PlayerType.ButcherPlayer,
                };
                if (Convert.ToInt64(comInfo[3]) == 1)
                {
                    humanOrButcher = true;
                }
                else if (Convert.ToInt64(comInfo[3]) == 2)
                {
                    humanOrButcher = false;
                }
                playerMsg.PlayerType = playerType;
                if (playerType == PlayerType.HumanPlayer)
                {
                    switch (Convert.ToInt64(comInfo[4]))
                    {
                        case 0:
                            playerMsg.HumanType = HumanType.NullHumanType;
                            break;
                        case 1:
                            playerMsg.HumanType = HumanType._1;
                            break;
                        case 2:
                            playerMsg.HumanType = HumanType._2;
                            break;
                        case 3:
                            playerMsg.HumanType = HumanType._3;
                            break;
                        case 4:
                            playerMsg.HumanType = HumanType._4;
                            break;
                        default:
                            break;
                    }
                }
                else if (playerType == PlayerType.ButcherPlayer)
                {
                    switch (Convert.ToInt64(comInfo[4]))
                    {
                        case 0:
                            playerMsg.ButcherType = ButcherType.NullButcherType;
                            break;
                        case 1:
                            playerMsg.ButcherType = ButcherType._1;
                            break;
                        case 2:
                            playerMsg.ButcherType = ButcherType._2;
                            break;
                        case 3:
                            playerMsg.ButcherType = ButcherType._3;
                            break;
                        case 4:
                            playerMsg.ButcherType = ButcherType._4;
                            break;
                        default:
                            break;
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
                FontSize = 10,
                Width = 20,
                Height = 20,
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

        // 获得地图信息
        private void GetMap(MessageOfMap obj)
        {
            int[,] map = new int[50, 50];
            try
            {
                for (int i = 0; i < 50; i++)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        map[i, j] = Convert.ToInt32(obj.Row[i].Col[j]);
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
        private void ZoomMap()
        {
            for (int i = 0; i < 50; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    if (mapPatches[i, j] != null)
                    {
                        mapPatches[i, j].Width = UpperLayerOfMap.ActualWidth / 50;
                        mapPatches[i, j].Height = UpperLayerOfMap.ActualHeight / 50;
                        mapPatches[i, j].HorizontalAlignment = HorizontalAlignment.Left;
                        mapPatches[i, j].VerticalAlignment = VerticalAlignment.Top;
                        mapPatches[i, j].Margin = new Thickness(UpperLayerOfMap.ActualWidth / 50 * j, UpperLayerOfMap.ActualHeight / 50 * i, 0, 0);
                    }
                }
            }
        }
        private void DrawMap()
        {
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
                        Margin = new Thickness(Width * (j), Height * (i), 0, 0)
                    };
                    // mapPatches[i, j].SetValue(Canvas.LeftProperty, (double)(Width / 65.5 * j));
                    // mapPatches[i, j].SetValue(Canvas.TopProperty, (double)(Height / 56.5 * i));  // 用zoommap进行修改
                    switch (defaultMap[i, j])
                    {
                        case 6:
                            mapPatches[i, j].Fill = Brushes.Brown;
                            mapPatches[i, j].Stroke = Brushes.Brown;
                            break;
                        case 7:
                            mapPatches[i, j].Fill = Brushes.Green;
                            mapPatches[i, j].Stroke = Brushes.Green;
                            break;
                        case 8:
                            mapPatches[i, j].Fill = Brushes.LightPink;
                            mapPatches[i, j].Stroke = Brushes.LightPink;
                            break;//machine
                        case 9:
                            mapPatches[i, j].Fill = Brushes.LightSkyBlue;
                            mapPatches[i, j].Stroke = Brushes.LightSkyBlue;
                            break;//door
                        case 10:
                            mapPatches[i, j].Fill = Brushes.LightSalmon;
                            mapPatches[i, j].Stroke = Brushes.LightSalmon;
                            break;//emergency
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
        private async void OnReceive()  // log未更新,switch1,2更新log
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
                        MessageToClient content = responseStream.ResponseStream.Current;
                        switch (content.GameState)
                        {
                            case GameState.GameStart:
                                foreach (var obj in content.HumanMessage)
                                {
                                    if (humanOrButcher && obj.PlayerId == playerID)
                                    {
                                        human = obj;
                                    }
                                    listOfHuman.Add(obj);
                                }
                                foreach (var obj in content.ButcherMessage)
                                {
                                    if (!humanOrButcher && obj.PlayerId == playerID)
                                    {
                                        butcher = obj;
                                    }
                                    listOfButcher.Add(obj);
                                }
                                foreach (var obj in content.PropMessage)
                                {
                                    listOfProp.Add(obj);
                                }
                                GetMap(content.MapMessage);
                                break;
                            case GameState.GameRunning:
                                foreach (var obj in content.HumanMessage)
                                {
                                    if (humanOrButcher && obj.PlayerId == playerID)
                                    {
                                        human = obj;
                                    }
                                    listOfHuman.Add(obj);
                                }
                                foreach (var obj in content.ButcherMessage)
                                {
                                    if (!humanOrButcher && obj.PlayerId == playerID)
                                    {
                                        butcher = obj;
                                    }
                                    listOfButcher.Add(obj);
                                }
                                foreach (var obj in content.PropMessage)
                                {
                                    listOfProp.Add(obj);
                                }
                                if (!mapFlag)
                                    GetMap(content.MapMessage);
                                break;
                            case GameState.GameEnd:
                                foreach (var obj in content.HumanMessage)
                                {
                                    listOfHuman.Add(obj);
                                }
                                foreach (var obj in content.ButcherMessage)
                                {
                                    listOfButcher.Add(obj);
                                }
                                foreach (var obj in content.PropMessage)
                                {
                                    listOfProp.Add(obj);
                                }
                                break;
                        }
                    }
                }
                if (responseStream == null)
                {
                    throw new Exception("Unconnected");
                }
            }
            catch (Exception ex)
            {
                ErrorDisplayer error = new("Error: "+ex.ToString());
                error.Show();
            }   
        }

        private bool CanSee(MessageOfHuman msg)
        {
            if (msg.State == HumanState.Dead)
                return false;
            //if (playerID >= 2022 || teamID >= 2022)
            //   return true;
            if (humanOrButcher && human != null)
            {
                if (human.Guid == msg.Guid)  // 自己能看见自己
                    return true;
            }
            if (msg.Place == PlaceType.Grass || msg.Place == PlaceType.Gate || msg.Place == PlaceType.HiddenGate)
                return false;
            if (msg.Place == PlaceType.Land || msg.Place == PlaceType.Machine)
                return true;
            if (humanOrButcher && human != null)
            {
                if (msg.Place != human.Place)
                    return false;
            }
            else if (!humanOrButcher && butcher != null)
            {
                if (msg.Place != butcher.Place)
                    return false;
            }
            return true;
        }

        private bool CanSee(MessageOfButcher msg)
        {
            // if (playerID >= 2022 || teamID >= 2022)
            //     return true;
            if (!humanOrButcher && butcher != null)
            {
                if (butcher.Guid == msg.Guid)  // 自己能看见自己
                    return true;
            }
            if (msg.Place == PlaceType.Grass || msg.Place == PlaceType.Gate || msg.Place == PlaceType.HiddenGate)
                return false;
            if (msg.Place == PlaceType.Land || msg.Place == PlaceType.Machine)
                return true;
            if (humanOrButcher && human != null)
            {
                if (msg.Place != human.Place)
                    return false;
            }
            else if (!humanOrButcher && butcher != null)
            {
                if (msg.Place != butcher.Place)
                    return false;
            }
            return true;
        }

        private bool CanSee(MessageOfProp msg)
        {
            if (msg.Place == PlaceType.Land)
                return true;
            if (humanOrButcher && human != null)
            {
                if (msg.Place != human.Place)
                    return false;
            }
            else if (!humanOrButcher && butcher != null)
            {
                if (msg.Place != butcher.Place)
                    return false;
            }
            return true;
        }

        private void Refresh(object? sender, EventArgs e)
        {
            // Bonus();
            if (WindowState == WindowState.Maximized)
                MaxButton.Content = "❐";
            else
                MaxButton.Content = "🗖";
            if (StatusBarsOfSurvivor != null)
                for (int i = 4; i < 8; i++)
                {
                    StatusBarsOfSurvivor[i - 4].SetFontSize(12 * UpperLayerOfMap.ActualHeight / 650);
                }
            if (StatusBarsOfHunter != null)
                StatusBarsOfHunter.SetFontSize(12 * UpperLayerOfMap.ActualHeight / 650);
            if (StatusBarsOfCircumstance != null)
                StatusBarsOfCircumstance.SetFontSize(12 * UpperLayerOfMap.ActualHeight / 650);
            // 完成窗口信息更新
            if (!isClientStocked)
            {
                unit = Math.Sqrt(UpperLayerOfMap.ActualHeight * UpperLayerOfMap.ActualWidth) / 50;
                unitHeight = UpperLayerOfMap.ActualHeight / 50;
                unitWidth = UpperLayerOfMap.ActualWidth / 50;
                try
                {
                    // if (log != null)
                    //{
                    //     string temp = "";
                    //     for (int i = 0; i < dataDict[GameObjType.Character].Count; i++)
                    //     {
                    //         temp += Convert.ToString(dataDict[GameObjType.Character][i].MessageOfCharacter.TeamID) + "\n";
                    //     }
                    //     log.Content = temp;
                    // }
                    UpperLayerOfMap.Children.Clear();
                    // if ((communicator == null || !communicator.Client.IsConnected) && !isPlaybackMode)
                    //{
                    //     UnderLayerOfMap.Children.Clear();
                    //     throw new Exception("Client is unconnected.");
                    // }
                    // else
                    //{
                    lock (drawPicLock)  // 加锁是必要的，画图操作和接收信息操作不能同时进行
                    {
                        if (!hasDrawed && mapFlag)
                            DrawMap();
                        foreach (var data in listOfHuman)
                        {
                            StatusBarsOfSurvivor[data.PlayerId].SetValue(data);
                            if (CanSee(data))
                            {
                                Ellipse icon = new()
                                {
                                    Width = unitWidth,
                                    Height = unitHeight,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                    Fill = Brushes.BlueViolet,
                                };
                                UpperLayerOfMap.Children.Add(icon);
                            }
                        }
                        foreach (var data in listOfButcher)
                        {
                            if (CanSee(data))
                            {
                                Ellipse icon = new()
                                {
                                    Width = 10,
                                    Height = 10,
                                    HorizontalAlignment = HorizontalAlignment.Left,
                                    VerticalAlignment = VerticalAlignment.Top,
                                    Margin = new Thickness(data.Y * unitWidth / 1000.0 - unitWidth / 2, data.X * unitHeight / 1000.0 - unitHeight / 2, 0, 0),
                                    Fill = Brushes.Black,
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
                                    case PropType.Ptype1:
                                        DrawProp(data, "🔧");
                                        break;
                                    case PropType.Ptype2:
                                        DrawProp(data, "🛡");
                                        break;
                                    case PropType.Ptype3:
                                        DrawProp(data, "♥");
                                        break;
                                    case PropType.Ptype4:
                                        DrawProp(data, "⛸");
                                        break;
                                    default:
                                        DrawProp(data, "");
                                        break;
                                }
                            }
                        }

                        //}
                        ZoomMap();
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
            counter++;
        }

        // 键盘控制
        private void KeyBoardControl(object sender, KeyEventArgs e)
        {
            if (!isPlaybackMode)
            {
                switch (e.Key)
                {
                    case Key.W:
                    case Key.NumPad8:
                        MoveMsg msgW = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 50,
                            Angle = Math.PI
                        };
                        client.Move(msgW);
                        break;
                    case Key.NumPad2:
                    case Key.S:
                        MoveMsg msgS = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 50,
                            Angle = 0
                        };
                        client.Move(msgS);
                        break;
                    case Key.D:
                    case Key.NumPad6:
                        MoveMsg msgD = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 50,
                            Angle = Math.PI / 2
                        };
                        client.Move(msgD);
                        break;
                    case Key.A:
                    case Key.NumPad4:
                        MoveMsg msgA = new()
                        {
                            PlayerId = playerID,
                            TimeInMilliseconds = 50,
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
                    case Key.F:
                        PickMsg msgF = new()
                        {
                            PlayerId = playerID,
                        };
                        client.PickProp(msgF);
                        break;
                    case Key.E:
                        IDMsg msgE = new()
                        {
                            PlayerId = playerID,
                        };
                        client.UseProp(msgE);
                        break;
                    case Key.Q:
                        IDMsg msgQ = new()
                        {
                            PlayerId = playerID,
                        };
                        client.UseSkill(msgQ);
                        break;

                    default:
                        break;
                }
            }
        }

        //鼠标双击
        private void Attack(object sender, RoutedEventArgs e)
        {
            if (!isPlaybackMode)
            {
                if (humanOrButcher&&human != null)
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

        // 之后需要修改，现在只具有修改按钮形状的功能，并不能实现暂停/继续
        private void ClickToPauseOrContinue(object sender, RoutedEventArgs e)
        {
            if (!isClientStocked)
            {
                isClientStocked = true;
                PorC.Content = "▶";
            }
            else
            {
                isClientStocked = false;
                PorC.Content = "⏸";
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

        // 寻求帮助、访问EESAST（部分功能未复原）
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
            catch (Exception exc)
            {
                //               ErrorDisplayer error = new("发生错误。以下是系统报告\n" + exc.ToString());
                //               error.Show();
            }
        }

        // 配置连接（未复原）、我的AI（THUAI5未实现）、获取更新、天梯信息（可能需要网站协助）
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
            catch (Exception exc)
            {
                //               ErrorDisplayer error = new(exc.Message);
                //               error.Show();
            }
        }

        // 以下为Mainwindow自定义属性
        private readonly DispatcherTimer timer;  // 定时器
        private long counter;                    // 预留的取时间变量
        AvailableService.AvailableServiceClient client;
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
        private List<MessageOfHuman> listOfHuman;
        private List<MessageOfButcher> listOfButcher;
        private object drawPicLock = new object();
        private MessageOfHuman? human = null;
        private MessageOfButcher? butcher = null;
        private bool humanOrButcher;//true for human

        private bool bonusflag;
        private bool mapFlag = false;
        private bool hasDrawed = false;
        public int[,] defaultMap = new int[,] {{ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 9, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 7, 5, 7, 7, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 6, 6, 6, 0, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 6 },
            { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6 },
            { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 }};

        private string[] comInfo = new string[5];
    }
}
