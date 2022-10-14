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

//留意初始化
//目前MainWindow还未复现的功能：
//左侧信息private void SetStatusBar()，
//private void ReactToCommandline()，
//private void Playback(string fileName, double pbSpeed = 2.0)
//绘图函数private void DrawLaser(Point source, double theta, double range,double Width)//三个参数分别为攻击者的位置，攻击方位角（窗口坐标）和攻击半径
//       private void DrawProp(MessageToClient.Types.GameObjMessage data, string text)，
//       private void Attack(object sender,RoutedEventArgs e)
//地图相关private void ZoomMap()，private void DrawMap()

//交互：private void ClickToSetMode(object sender, RoutedEventArgs e)
//最近要解决private void ConnectToServer(string[] comInfo)
//private void KeyBoardControl(object sender, KeyEventArgs e)
//private void GetMap(MessageToClient.Types.GameObjMessage obj)
//private void OnReceive()
//private bool CanSee(MessageOfCharacter msg) (以及两个重载函数)
//private void Refresh(object? sender, EventArgs e)
//private void Bonus()

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            isClientStocked = true;
            isPlaybackMode = false;
            WindowStartupLocation = WindowStartupLocation.CenterScreen;
        }

        //之后需要修改，现在只具有修改按钮形状的功能，并不能实现暂停/继续
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
        //未复现
        private void ClickToConnect(object sender, RoutedEventArgs e)
        {
            
        }

        //窗口最大化、关闭、最小化、拖拽
        private void ClickToMaxmize(object sender, RoutedEventArgs e)
        {
            if (WindowState != WindowState.Maximized)
                WindowState = WindowState.Maximized;
            else WindowState = WindowState.Normal;
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

        //寻求帮助、访问EESAST（部分功能未复原）
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

        //配置连接（未复原）、我的AI（THUAI5未实现）、获取更新、天梯信息（可能需要网站协助）
        private void ClickToSetConnect(object sender, RoutedEventArgs e)
        {
 //           ConnectRegister crg = new();
 //           crg.Show();
        }
        private void ClickToEnterVS(object sender, RoutedEventArgs e)
        {
            //try
            //{
            //    if (!File.Exists("VSRoute.txt"))
            //    {
            //        File.Create("VSRoute.txt");
            //        Exception ex = new("没有路径存储文件，已为您创建。请将VS路径输入该文件，并重新操作。");
            //        throw ex;
            //    }//创建路径文件 
            //    using StreamReader sr = new("VSRoute.txt");
            //    _ = Process.Start(sr.ReadLine());
            //}
            //catch (Exception exc)
            //{
            //    ErrorDisplayer error = new("发生错误。以下是系统报告:\n" + exc.ToString());
            //    error.Show();
            //}
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

        //敬请期待函数
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
        private bool isClientStocked;
        private bool isPlaybackMode;
    }
}
