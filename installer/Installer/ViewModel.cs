
using starter.viewmodel.common;
using System;
using System.Windows;
using System.Windows.Forms;
using Downloader;
using MessageBox = System.Windows.MessageBox;
using System.Configuration;
using System.Drawing.Design;
using Application = System.Windows.Application;
using System.ComponentModel;
using Installer;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.IO;
using System.Windows.Automation.Provider;
using System.Diagnostics;

namespace starter.viewmodel.settings
{
    public class SettingsViewModel : NotificationObject
    {
        //定义BackgroundWorker
        BackgroundWorker asyncDownloader;
        BackgroundWorker asyncUpdater;
        /// <summary>
        /// Model object
        /// </summary>
        private SettingsModel obj = new SettingsModel();
        /// <summary>
        /// initializer
        /// </summary>

        public SettingsViewModel()
        {

            //Program.Tencent_cos_download.UpdateHash();

            Status = SettingsModel.Status.working;
            string currentDirectory = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule?.FileName)
                ?? throw new Exception("Fail to get current directory");
            //MessageBox.Show("更新器工作正常");
            //if (!Program.Tencent_cos_download.SelfUpdateDismissed())
            //{
            //    switch (Program.Tencent_cos_download.CheckSelfVersion())
            //    {
            //        case 1:
            //            Process.Start(System.IO.Path.Combine(currentDirectory, "InstallerUpdater.exe"));
            //            Environment.Exit(0);
            //            break;
            //        case -1:
            //            MessageBox.Show("下载器更新检查出错，将继续启动现有下载器");
            //            break;
            //    }
            //}

            //实例化BackgroundWorker
            asyncDownloader = new BackgroundWorker();
            asyncUpdater = new BackgroundWorker();
            //指示BackgroundWorker是否可以报告进度更新
            //当该属性值为True是，将可以成功调用ReportProgress方法，否则将引发InvalidOperationException异常。
            asyncDownloader.WorkerReportsProgress = true;
            asyncUpdater.WorkerReportsProgress = true;
            //挂载方法：
            asyncDownloader.DoWork += AsyncDownloader_DoWork;
            asyncUpdater.DoWork += AsyncUpdater_DoWork;
            //完成通知器：
            asyncDownloader.RunWorkerCompleted += AsyncDownloader_RunWorkerCompleted;
            asyncUpdater.RunWorkerCompleted += AsyncUpdater_RunWorkerCompleted;

            UpdateInfoVis = Visibility.Collapsed;

            if (Downloader.Program.Tencent_cos_download.CheckAlreadyDownload())
            {
                obj.checkUpdate();
                Status = SettingsModel.Status.login;
                this.RaisePropertyChanged("WindowWidth");
                this.RaisePropertyChanged("LaunchVis");
                if (obj.RecallUser())
                    RememberMe = true;
                else
                    RememberMe = false;
                this.RaisePropertyChanged("RememberMe");
                this.RaisePropertyChanged("SwitchOSBtnCont");
                //在启动时立刻检查更新，确保选手启动最新版选手包
                //若有更新，将启动键改为更新键；
                //相应地，使用login界面启动；
                //结构：上方为登录框架，下方有“修改选手包”按钮
            }
            else
            {
                Route = Environment.GetEnvironmentVariable("USERPROFILE") + "\\THUAI6";
                Status = SettingsModel.Status.newUser;
                this.RaisePropertyChanged("WindowWidth");
            }
        }

        private void AsyncDownloader_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Result == null)
            {
                Status = SettingsModel.Status.error;
            }
            else if ((bool)e.Result)
            {
                Status = SettingsModel.Status.successful;
            }
            else
            {
                Status = SettingsModel.Status.newUser;
            }
        }

        private void AsyncUpdater_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Result == null)
            {
                Status = SettingsModel.Status.error;
            }
            else
            {
                this.RaisePropertyChanged("LaunchVis");
                if ((int)e.Result == 1)
                {
                    Status = SettingsModel.Status.successful;
                    this.RaisePropertyChanged("UpdateBtnCont");
                    this.RaisePropertyChanged("UpdateInfo");
                    this.RaisePropertyChanged("LaunchBtnCont");
                }
                else if ((int)e.Result == 2)
                {
                    Status = SettingsModel.Status.login;
                    this.RaisePropertyChanged("UpdateBtnCont");
                    this.RaisePropertyChanged("LaunchBtnCont");
                    this.RaisePropertyChanged("UpdateInfo");
                }
                else
                {
                    string updateFailList = "";
                    foreach (var Filename in Program.UpdateFailed)
                    {
                        updateFailList += Filename + "\n";
                    }
                    MessageBox.Show($"以下文件因被占用而未能成功更新：\n{updateFailList}请关闭它们，并再试一次");
                    Program.ResetUpdateFailedInfo();
                    obj.checkUpdate();
                    Status = SettingsModel.Status.successful;
                    this.RaisePropertyChanged("UpdateBtnCont");
                    this.RaisePropertyChanged("UpdateInfo");
                    this.RaisePropertyChanged("LaunchBtnCont");
                }
            }
        }

        private void AsyncUpdater_DoWork(object? sender, DoWorkEventArgs e)
        {
            if (asyncUpdater.CancellationPending)
            {
                e.Cancel = true;
                MessageBox.Show("下载取消");
                if (e.Argument?.ToString()?.Equals("Manual") ?? false)
                {
                    Status = SettingsModel.Status.menu;
                }
                else
                    Status = SettingsModel.Status.login;
                return;
            }
            else
            {
                if (obj.Update())
                    if (e.Argument?.ToString()?.Equals("Manual") ?? false)
                    {
                        e.Result = 1;
                    }
                    else
                        e.Result = 2;
                else
                    e.Result = -1;
            }
        }

        private void AsyncDownloader_DoWork(object? sender, DoWorkEventArgs e)
        {
            if (asyncDownloader.CancellationPending)
            {
                e.Cancel = true;
                return;
            }
            else
            {
                if (obj.install())
                    e.Result = true;
                else
                    e.Result = false;
            }
        }

        //TODO:参赛界面：包括上传参赛代码、申请对战
        //TODO:界面中应包含上次对战完成提示及下载回放按钮

        public int ExtraColumn
        {
            get
            {
                if (Status == SettingsModel.Status.newUser || Status == SettingsModel.Status.move)
                    return 75;
                else
                    return 0;
            }
        }
        public int WindowWidth
        {
            get
            {
                switch (Status)
                {
                    case SettingsModel.Status.newUser:
                        return 505;
                    case SettingsModel.Status.move:
                        return 505;
                    case SettingsModel.Status.working:
                        return 435;
                    case SettingsModel.Status.successful:
                        return 435;
                    default:
                        return 355;
                }
            }
        }

        public SettingsModel.Status Status
        {
            get
            {
                return obj.status;
            }
            set
            {
                obj.status = value;
                this.RaisePropertyChanged("ExtraColumn");
                this.RaisePropertyChanged("Intro");
                this.RaisePropertyChanged("RouteBoxIntro");
                this.RaisePropertyChanged("LoginVis");
                this.RaisePropertyChanged("MenuVis");
                this.RaisePropertyChanged("RouteBoxVis");
                this.RaisePropertyChanged("ProgressVis");
                //TODO: Thread will be taken by process working and window will not refresh.
                this.RaisePropertyChanged("CompleteVis");
                this.RaisePropertyChanged("WindowWidth");
                this.RaisePropertyChanged("WebVis");
                this.RaisePropertyChanged("CoverVis");
                this.RaisePropertyChanged("LaunchVis");
                this.RaisePropertyChanged("NewUserVis");
                this.RaisePropertyChanged("ConfirmBtnCont");
            }
        }
        public string Intro
        {
            get
            {
                switch (Status)
                {
                    case SettingsModel.Status.newUser:
                        return "欢迎使用选手包，请选择你想要安装选手包的位置：";
                    case SettingsModel.Status.menu:
                        return "你已经安装了选手包，请选择想要进行的操作：";
                    case SettingsModel.Status.login:
                        return "使用EESAST账号登录";
                    case SettingsModel.Status.web:
                        return "THUAI6 赛场：";
                    case SettingsModel.Status.disconnected:
                        return "你可能没有连接到网络，无法下载/更新选手包";
                    case SettingsModel.Status.error:
                        return "我们遇到了一些问题，请向[]反馈";
                    default:
                        return "";
                }
            }
        }
        public string RouteBoxIntro
        {
            get
            {
                switch (Status)
                {

                    case SettingsModel.Status.newUser:
                        return "将选手包安装在（将创建THUAI6文件夹）：";
                    case SettingsModel.Status.move:
                        return "将选手包移动到（THUAI6文件夹将会被整体移动）：";
                    default:
                        return "";
                }
            }
        }
        public string AbortOrSelLanguage
        {
            get
            {
                string ans = "";
                if (obj.UploadReady)
                    ans = "放弃上传";
                else if (obj.launchLanguage == SettingsModel.LaunchLanguage.cpp)
                    ans = "语言:c++";
                else if (obj.launchLanguage == SettingsModel.LaunchLanguage.python)
                    ans = "语言:python";
                return ans;
            }
        }
        public string SwitchOSBtnCont
        {
            get
            {
                switch (obj.usingOS)
                {
                    case SettingsModel.UsingOS.Win:
                        return "当前系统:Windows";
                    case SettingsModel.UsingOS.Linux:
                        return "当前系统:Linux";
                    case SettingsModel.UsingOS.OSX:
                        return "当前系统:macOS";
                    default:
                        return "当前系统:无效的名称";
                }
            }
        }
        public int PlayerNum
        {
            get
            {
                int ans;
                if (obj.PlayerNum.Equals("player_1"))
                    ans = 1;
                else if (obj.PlayerNum.Equals("player_2"))
                    ans = 2;
                else if (obj.PlayerNum.Equals("player_3"))
                    ans = 3;
                else if (obj.PlayerNum.Equals("player_4"))
                    ans = 4;
                else
                    ans = 0;
                return ans;
            }
            set
            {
                switch (value)
                {
                    case 1:
                        obj.PlayerNum = "player_1";
                        break;
                    case 2:
                        obj.PlayerNum = "player_2";
                        break;
                    case 3:
                        obj.PlayerNum = "player_3";
                        break;
                    case 4:
                        obj.PlayerNum = "player_4";
                        break;
                    default:
                        obj.PlayerNum = "nSelect";
                        break;
                }
                this.RaisePropertyChanged("PlayerNum");
            }
        }

        public string Route
        {
            get
            {
                return obj.Route;
            }
            set
            {
                obj.Route = value;
                this.RaisePropertyChanged("Route");
            }
        }
        public string Username
        {
            get => obj.Username;
            set
            {
                obj.Username = value;
                this.RaisePropertyChanged("Username");
            }
        }
        public string Password
        {
            get => obj.Password;
            set
            {
                obj.Password = value;
                this.RaisePropertyChanged("Password");
            }
        }
        public string CodeName
        {
            get
            {
                return obj.CodeRoute.Substring(obj.CodeRoute.LastIndexOf('/') == -1 ? obj.CodeRoute.LastIndexOf('\\') + 1 : obj.CodeRoute.LastIndexOf('/') + 1);
            }
        }

        public bool RememberMe
        {
            get
            {
                return obj.RememberMe;
            }
            set
            {
                obj.RememberMe = value;
                this.RaisePropertyChanged("RememberMe");
            }
        }

        public Visibility NewUserVis
        {
            get
            {
                return Status == SettingsModel.Status.newUser ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility MenuVis
        {
            get
            {
                return Status == SettingsModel.Status.menu ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility RouteBoxVis
        {
            get
            {
                return (Status == SettingsModel.Status.newUser || Status == SettingsModel.Status.move) ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility LoginVis
        {
            get
            {
                return Status == SettingsModel.Status.login ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility ProgressVis
        {
            get
            {
                return Status == SettingsModel.Status.working ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility CompleteVis
        {
            get
            {
                return Status == SettingsModel.Status.successful ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility WebVis
        {
            get
            {
                return Status == SettingsModel.Status.web ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility CoverVis
        {
            get
            {
                return Status == SettingsModel.Status.web && !obj.UploadReady ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility LoginFailVis
        {
            get
            {
                return obj.LoginFailed ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility MatchFinishedVis
        {
            get
            {
                return obj.CombatCompleted ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility UploadReadyVis
        {
            get { return obj.UploadReady ? Visibility.Visible : Visibility.Collapsed; }
        }

        public Visibility UpdateInfoVis
        {
            get; set;
        }

        public Visibility LaunchVis
        {
            get
            {
                return obj.status == SettingsModel.Status.login && (!obj.UpdatePlanned) ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        public string UpdateBtnCont
        {
            get
            {
                return obj.UpdatePlanned ? "更新" : "检查更新";
            }
        }
        public string UpdateInfo
        {
            get
            {
                if (obj.UpdatePlanned)
                    return obj.Updates;
                else
                    return "已是最新版本";
            }
        }
        public string LaunchBtnCont
        {
            get
            {
                string ans;
                if (obj.UpdatePlanned)
                    ans = "更新";
                else if (obj.launchLanguage == SettingsModel.LaunchLanguage.cpp)
                    ans = "启动c++包";
                else
                    ans = "启动python包";
                return ans;
            }
        }
        public string UploadBtnCont
        {
            get
            {
                return obj.UploadReady ? "上传代码" : "选择代码上传";
            }
        }
        public string ShiftLanguageBtnCont
        {
            get
            {
                return obj.launchLanguage == SettingsModel.LaunchLanguage.cpp ? "改为python" : "改为c++";
            }
        }
        public string ConfirmBtnCont
        {
            get
            {
                switch (Status)
                {
                    case SettingsModel.Status.newUser:
                        return "确认并安装";
                    case SettingsModel.Status.move:
                        return "确认并移动";
                    default:
                        return "";
                }
            }
        }

        public string RouteSelectWindow(string type)
        {
            if (type == "Folder")
            {
                using (FolderBrowserDialog dialog = new FolderBrowserDialog())
                {
                    _ = dialog.ShowDialog();
                    if (dialog.SelectedPath != String.Empty)
                        return dialog.SelectedPath;
                }
            }
            else if (type == "File")
            {

                var openFileDialog = new Microsoft.Win32.OpenFileDialog();
                if (obj.launchLanguage == SettingsModel.LaunchLanguage.cpp)
                {
                    openFileDialog.InitialDirectory = (Route + "/THUAI6/win/CAPI/cpp/API/src/").Replace("/", "\\");
                    openFileDialog.Filter = "c++ Source Files (.cpp)|*.cpp|c++ Header File (.h)|*.h|python Source File (.py)|*.py";
                }
                else if (obj.launchLanguage == SettingsModel.LaunchLanguage.python)
                {
                    openFileDialog.InitialDirectory = (Route + "/THUAI6/win/CAPI/python/PyAPI/").Replace("/", "\\");
                    openFileDialog.Filter = "python Source File (.py)|*.py|c++ Source Files (.cpp)|*.cpp|c++ Header File (.h)|*.h";
                }
                var result = openFileDialog.ShowDialog();
                if (result == true)
                {
                    return openFileDialog.FileName;
                }
            }
            return "";
        }

        private BaseCommand? clickBrowseCommand;
        public BaseCommand ClickBrowseCommand
        {
            get
            {
                if (clickBrowseCommand == null)
                {
                    clickBrowseCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        Route = RouteSelectWindow("Folder");
                    }));
                }
                return clickBrowseCommand;
            }
        }
        private BaseCommand? clickConfirmCommand;
        public BaseCommand ClickConfirmCommand
        {
            get
            {
                if (clickConfirmCommand == null)
                {
                    clickConfirmCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        if (Status == SettingsModel.Status.newUser)
                        {
                            if (Directory.Exists(Route))
                            {
                                Status = SettingsModel.Status.working;
                                this.RaisePropertyChanged("ProgressVis");
                                /*if (obj.install())
                                {
                                    Status = SettingsModel.Status.successful;
                                }*/
                                if (asyncDownloader.IsBusy)
                                    return;
                                else
                                {
                                    asyncDownloader.RunWorkerAsync();
                                }
                            }
                            else
                            {
                                MessageBox.Show("所选的路径不存在，请重新选择", "路径不存在", MessageBoxButton.OK, MessageBoxImage.Warning);
                            }

                        }
                        else if (Status == SettingsModel.Status.move)
                        {
                            //Status = SettingsModel.Status.working;
                            //this.RaisePropertyChanged("ProgressVis");
                            if (Directory.Exists(Route))
                            {
                                switch (obj.move())
                                {
                                    case -1:
                                        MessageBox.Show("文件已打开或者目标路径下有同名文件！", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        break;
                                    case -2:
                                        Status = SettingsModel.Status.error;
                                        break;
                                }
                                Status = SettingsModel.Status.successful;
                            }
                            else
                            {
                                MessageBox.Show("所选的路径不存在，请重新选择", "路径不存在", MessageBoxButton.OK, MessageBoxImage.Warning);
                            }
                        }
                    }));
                }
                return clickConfirmCommand;
            }
        }
        private BaseCommand? clickUpdateCommand;
        public BaseCommand ClickUpdateCommand
        {
            get
            {
                if (clickUpdateCommand == null)
                {
                    clickUpdateCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        this.RaisePropertyChanged("UpdateInfoVis");
                        if (obj.UpdatePlanned)
                        {
                            UpdateInfoVis = Visibility.Collapsed;
                            this.RaisePropertyChanged("UpdateInfoVis");
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            /*if (obj.Update())
                            {

                                Status = SettingsModel.Status.successful;
                                this.RaisePropertyChanged("UpdateBtnCont");
                                this.RaisePropertyChanged("UpdateInfo");

                            }
                            else
                                Status = SettingsModel.Status.error;*/
                            if (asyncUpdater.IsBusy)
                                return;
                            else
                                asyncUpdater.RunWorkerAsync("Manual");
                        }
                        else
                        {
                            UpdateInfoVis = Visibility.Visible;
                            this.RaisePropertyChanged("UpdateInfoVis");
                            //Status = SettingsModel.Status.working;
                            //this.RaisePropertyChanged("ProgressVis");
                            Status = obj.checkUpdate();
                            this.RaisePropertyChanged("UpdateBtnCont");
                            this.RaisePropertyChanged("UpdateInfo");
                            this.RaisePropertyChanged("LaunchVis");
                        }
                    }));
                }
                return clickUpdateCommand;
            }
        }
        private BaseCommand? clickMoveCommand;
        public BaseCommand ClickMoveCommand
        {
            get
            {
                if (clickMoveCommand == null)
                {
                    clickMoveCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        Status = SettingsModel.Status.move;
                    }));
                }
                return clickMoveCommand;
            }
        }
        private BaseCommand? clickUninstCommand;
        public BaseCommand ClickUninstCommand
        {
            get
            {
                if (clickUninstCommand == null)
                {
                    clickUninstCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        UpdateInfoVis = Visibility.Collapsed;
                        this.RaisePropertyChanged("UpdateInfoVis");
                        switch (obj.Uninst())
                        {
                            case -1:
                                Status = SettingsModel.Status.menu;
                                MessageBox.Show("文件已经打开，请关闭后再删除", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                break;
                            case 0:
                                Status = SettingsModel.Status.newUser;
                                MessageBox.Show($"删除成功！player文件夹中的文件已经放在{Downloader.Program.Data.FilePath}/{Downloader.Program.ProgramName}的根目录下", "", MessageBoxButton.OK, MessageBoxImage.Information, MessageBoxResult.OK);
                                break;
                            default:
                                Status = SettingsModel.Status.error;
                                break;

                        }

                    }));
                }
                return clickUninstCommand;
            }
        }

        private BaseCommand? clickLoginCommand;
        public BaseCommand ClickLoginCommand
        {
            get
            {
                if (clickLoginCommand == null)
                {
                    clickLoginCommand = new BaseCommand(new Action<object?>(async o =>
                    {
                        switch (await obj.Login())
                        {
                            case -1:
                                obj.LoginFailed = true;
                                break;
                            case 0:
                                obj.LoginFailed = false;
                                Status = SettingsModel.Status.web;
                                if (obj.RememberMe)
                                {
                                    obj.RememberUser();
                                    RememberMe = true;
                                    this.RaisePropertyChanged("RememberMe");
                                }
                                else
                                {
                                    obj.ForgetUser();
                                    RememberMe = false;
                                    this.RaisePropertyChanged("RememberMe");
                                    Username = "";
                                    Password = "";
                                    this.RaisePropertyChanged("Username");
                                    this.RaisePropertyChanged("Password");
                                }
                                this.RaisePropertyChanged("CoverVis");
                                break;
                            case -2:
                                MessageBox.Show("无法连接服务器，请检查网络情况", "网络错误", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                break;
                        }
                        this.RaisePropertyChanged("LoginFailVis");
                    }));
                }
                return clickLoginCommand;
            }
        }

        private BaseCommand? clickLaunchCommand;
        public BaseCommand ClickLaunchCommand
        {
            get
            {
                if (clickLaunchCommand == null)
                {
                    clickLaunchCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        if (obj.UpdatePlanned)
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            if (asyncUpdater.IsBusy)
                                return;
                            else
                                asyncUpdater.RunWorkerAsync("Auto");
                        }
                        else if (!obj.Launch())
                        {
                            Status = SettingsModel.Status.menu;
                        }
                    }));
                }
                return clickLaunchCommand;
            }
        }
        private BaseCommand? clickEditCommand;
        public BaseCommand ClickEditCommand
        {
            get
            {
                if (clickEditCommand == null)
                {
                    clickEditCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        Status = SettingsModel.Status.menu;
                        if (obj.UpdatePlanned)
                            UpdateInfoVis = Visibility.Visible;
                        this.RaisePropertyChanged("UpdateInfoVis");
                    }));
                }
                return clickEditCommand;
            }
        }
        private BaseCommand? clickBackCommand;
        public BaseCommand ClickBackCommand
        {
            get
            {
                if (clickBackCommand == null)
                {
                    clickBackCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        UpdateInfoVis = Visibility.Collapsed;
                        this.RaisePropertyChanged("UpdateInfoVis");
                        if (Downloader.Program.Tencent_cos_download.CheckAlreadyDownload())
                            Status = SettingsModel.Status.login;
                        else
                            Status = SettingsModel.Status.newUser;
                    }));
                }
                return clickBackCommand;
            }
        }
        private BaseCommand? clickUploadCommand;
        public BaseCommand ClickUploadCommand
        {
            get
            {
                if (clickUploadCommand == null)
                {
                    clickUploadCommand = new BaseCommand(new Action<object?>(async o =>
                    {
                        if (obj.UploadReady)
                        {
                            if (obj.PlayerNum.Equals("nSelect"))
                            {
                                MessageBox.Show("您还没有选择要上传的玩家身份", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                            }
                            else
                            {
                                switch (await obj.Upload())
                                {
                                    case -1:
                                        MessageBox.Show("Token失效！", "", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.OK);
                                        break;
                                    case -2:
                                        MessageBox.Show("目标路径不存在！", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        break;
                                    case -3:
                                        MessageBox.Show("服务器错误", "", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.OK);
                                        break;
                                    case -4:
                                        MessageBox.Show("您未登录或登录失效", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        Status = SettingsModel.Status.login;
                                        break;
                                    case -5:
                                        MessageBox.Show("您未报名THUAI!", "", MessageBoxButton.OK, MessageBoxImage.Error, MessageBoxResult.OK);
                                        break;
                                    case -6:
                                        MessageBox.Show("读取文件失败，请确认文件是否被占用", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        break;
                                    case -7:
                                        MessageBox.Show("网络错误，请检查你的网络", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        break;
                                    case -8:
                                        MessageBox.Show("不是c++或python源文件", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                        break;
                                }
                                obj.CodeRoute = "";
                                obj.UploadReady = false;
                                this.RaisePropertyChanged("UploadBtnCont");
                                this.RaisePropertyChanged("UploadReadyVis");
                                this.RaisePropertyChanged("CoverVis");
                                this.RaisePropertyChanged("AbortOrSelLanguage");
                            }
                        }
                        else
                        {
                            obj.CodeRoute = RouteSelectWindow("File");
                            if (obj.CodeRoute != "")
                            {
                                obj.UploadReady = true;
                                this.RaisePropertyChanged("UploadBtnCont");
                                this.RaisePropertyChanged("UploadReadyVis");
                                this.RaisePropertyChanged("CodeName");
                                this.RaisePropertyChanged("CoverVis");
                                this.RaisePropertyChanged("AbortOrSelLanguage");
                            }
                            else
                            {
                                MessageBox.Show("未选择代码，请重新选择！", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                            }
                        }
                    }));
                }
                return clickUploadCommand;
            }
        }
        private BaseCommand? clickAboutUploadCommand;
        public BaseCommand ClickAboutUploadCommand
        {
            get
            {
                if (clickAboutUploadCommand == null)
                {
                    clickAboutUploadCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        if (obj.UploadReady)
                        {
                            obj.CodeRoute = "";
                            obj.UploadReady = false;
                            this.RaisePropertyChanged("UploadBtnCont");
                            this.RaisePropertyChanged("UploadReadyVis");
                            this.RaisePropertyChanged("CodeName");
                            this.RaisePropertyChanged("CoverVis");
                            this.RaisePropertyChanged("AbortOrSelLanguage");
                        }
                        else
                        {
                            if (obj.launchLanguage == SettingsModel.LaunchLanguage.cpp)
                                obj.launchLanguage = SettingsModel.LaunchLanguage.python;
                            else
                                obj.launchLanguage = SettingsModel.LaunchLanguage.cpp;
                            this.RaisePropertyChanged("AbortOrSelLanguage");
                            this.RaisePropertyChanged("ShiftLanguageBtnCont");
                            this.RaisePropertyChanged("LaunchBtnCont");
                        }
                    }));
                }
                return clickAboutUploadCommand;
            }
        }
        private BaseCommand? clickExitCommand;
        public BaseCommand ClickExitCommand
        {
            get
            {
                if (clickExitCommand == null)
                {
                    clickExitCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        Application.Current.Shutdown();
                    }));
                }
                return clickExitCommand;
            }
        }
        private BaseCommand? clickShiftLanguageCommand;
        public BaseCommand ClickShiftLanguageCommand
        {
            get
            {
                if (clickShiftLanguageCommand == null)
                {
                    clickShiftLanguageCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        if (obj.launchLanguage == SettingsModel.LaunchLanguage.cpp)
                            obj.launchLanguage = SettingsModel.LaunchLanguage.python;
                        else
                            obj.launchLanguage = SettingsModel.LaunchLanguage.cpp;
                        this.RaisePropertyChanged("ShiftLanguageBtnCont");
                        this.RaisePropertyChanged("LaunchBtnCont");
                        this.RaisePropertyChanged("AbortOrSelLanguage");
                    }));
                }
                return clickShiftLanguageCommand;
            }
        }
        private BaseCommand? clickReadCommand;
        public BaseCommand ClickReadCommand
        {
            get
            {
                if (clickReadCommand == null)
                {
                    clickReadCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        if (!Directory.Exists(Route + "/THUAI6/win"))
                        {
                            if (Directory.Exists(Route.Substring(0, Route.Length - 7) + "/THUAI6/win"))
                                Route = Route.Substring(0, Route.Length - 7);
                            else
                            {
                                MessageBox.Show("请选择名为THUAI6的完整选手包文件夹，这可能意味着你需要对选手包文件夹进行重命名");
                                return;
                            }
                        }
                        Program.Data.ResetFilepath(Route);
                        if (Program.Tencent_cos_download.CheckAlreadyDownload())
                            Status = SettingsModel.Status.login;
                    }));
                }
                return clickReadCommand;
            }
        }
        private BaseCommand? clickSwitchOSCommand;
        public BaseCommand ClickSwitchOSCommand
        {
            get
            {
                if (clickSwitchOSCommand == null)
                {
                    clickSwitchOSCommand = new BaseCommand(new Action<object?>(o =>
                    {
                        switch (obj.usingOS)
                        {
                            case SettingsModel.UsingOS.Win:
                                obj.usingOS = SettingsModel.UsingOS.Linux;
                                break;
                            case SettingsModel.UsingOS.Linux:
                                obj.usingOS = SettingsModel.UsingOS.OSX;
                                break;
                            case SettingsModel.UsingOS.OSX:
                                obj.usingOS = SettingsModel.UsingOS.Win;
                                break;
                        }
                        this.RaisePropertyChanged("SwitchOSBtnCont");
                        obj.WriteUsingOS();
                        obj.checkUpdate();
                        UpdateInfoVis = Visibility.Visible;
                        this.RaisePropertyChanged("UpdateBtnCont");
                        this.RaisePropertyChanged("UpdateInfo");
                        this.RaisePropertyChanged("UpdateInfoVis");
                    }));
                }
                return clickSwitchOSCommand;
            }
        }

    }

}