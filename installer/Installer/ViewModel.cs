
using starter.viewmodel.common;
using System;
using System.Windows;
using System.Windows.Forms;
using Downloader;
using MessageBox = System.Windows.MessageBox;
using System.Configuration;
using System.Drawing.Design;

namespace starter.viewmodel.settings
{
    public class SettingsViewModel : NotificationObject
    {
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
            //WebConnect.Web.WriteUserEmail("wangsk21@mails.tsinghua.edu.cn");
            if (Downloader.Program.Tencent_cos_download.CheckAlreadyDownload())
            {
                obj.checkUpdate();
                Status = SettingsModel.Status.login;
                this.RaisePropertyChanged("WindowWidth");
                //TODO:在启动时立刻检查更新，确保选手启动最新版选手包
                //TODO:若有更新，将启动键改为更新键；
                //TODO:相应地，使用login界面启动；
                //TODO:结构：上方为登录框架，下方有“修改选手包”按钮
            }
            else
            {
                Route = Environment.GetEnvironmentVariable("USERPROFILE") + "\\THUAI6";
                Status = SettingsModel.Status.newUser;
                this.RaisePropertyChanged("WindowWidth");
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
                        return "将主体程序安装在：";
                    case SettingsModel.Status.move:
                        return "将主体程序移动到：";
                    default:
                        return "";
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
            get => obj.Route;
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
                    return "";
            }
        }
        public string LaunchBtnCont
        {
            get
            {
                return obj.UpdatePlanned ? "更新" : "启动";
            }
        }
        public string UploadBtnCont
        {
            get
            {
                return obj.UploadReady ? "上传代码" : "选择代码上传";
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
                var openFileDialog = new Microsoft.Win32.OpenFileDialog()
                {
                    Filter = "c++ Source Files (.cpp)|*.cpp|c++ Header File (.h)|*.h|python Source File (.py)|*.py"
                };
                var result = openFileDialog.ShowDialog();
                if (result == true)
                {
                    return openFileDialog.FileName;
                }
            }
            return "";
        }

        private BaseCommand clickBrowseCommand;
        public BaseCommand ClickBrowseCommand
        {
            get
            {
                if (clickBrowseCommand == null)
                {
                    clickBrowseCommand = new BaseCommand(new Action<object>(o =>
                    {
                        Route = RouteSelectWindow("Folder");
                    }));
                }
                return clickBrowseCommand;
            }
        }
        private BaseCommand clickConfirmCommand;
        public BaseCommand ClickConfirmCommand
        {
            get
            {
                if (clickConfirmCommand == null)
                {
                    clickConfirmCommand = new BaseCommand(new Action<object>(o =>
                    {
                        if (Status == SettingsModel.Status.newUser)
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            if (obj.install())
                            {
                                Status = SettingsModel.Status.successful;
                            }

                        }
                        else if (Status == SettingsModel.Status.move)
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
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
                    }));
                }
                return clickConfirmCommand;
            }
        }
        private BaseCommand clickUpdateCommand;
        public BaseCommand ClickUpdateCommand
        {
            get
            {
                if (clickUpdateCommand == null)
                {
                    clickUpdateCommand = new BaseCommand(new Action<object>(o =>
                    {
                        if (obj.UpdatePlanned)
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            if (obj.Update())
                            {

                                Status = SettingsModel.Status.successful;
                                this.RaisePropertyChanged("UpdateBtnCont");
                                this.RaisePropertyChanged("UpdateInfo");

                            }
                            else
                                Status = SettingsModel.Status.error;
                        }
                        else
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            Status = obj.checkUpdate();
                            this.RaisePropertyChanged("UpdateBtnCont");
                            this.RaisePropertyChanged("UpdateInfo");
                        }
                    }));
                }
                return clickUpdateCommand;
            }
        }
        private BaseCommand clickMoveCommand;
        public BaseCommand ClickMoveCommand
        {
            get
            {
                if (clickMoveCommand == null)
                {
                    clickMoveCommand = new BaseCommand(new Action<object>(o =>
                    {
                        Status = SettingsModel.Status.move;
                    }));
                }
                return clickMoveCommand;
            }
        }
        private BaseCommand clickUninstCommand;
        public BaseCommand ClickUninstCommand
        {
            get
            {
                if (clickUninstCommand == null)
                {
                    clickUninstCommand = new BaseCommand(new Action<object>(o =>
                    {
                        Status = SettingsModel.Status.working;
                        this.RaisePropertyChanged("ProgressVis");
                        switch (obj.Uninst())
                        {
                            case -1:
                                Status = SettingsModel.Status.menu;
                                MessageBox.Show("文件已经打开，请关闭后再删除", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                break;
                            case 0:
                                Status = SettingsModel.Status.successful;
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

        private BaseCommand clickLoginCommand;
        public BaseCommand ClickLoginCommand
        {
            get
            {
                if (clickLoginCommand == null)
                {
                    clickLoginCommand = new BaseCommand(new Action<object>(async o =>
                    {
                        switch (await obj.Login())
                        {
                            case -1:
                                obj.LoginFailed = true;
                                break;
                            case 0:
                                obj.LoginFailed = false;
                                Status = SettingsModel.Status.web;
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

        private BaseCommand clickLaunchCommand;
        public BaseCommand ClickLaunchCommand
        {
            get
            {
                if (clickLaunchCommand == null)
                {
                    clickLaunchCommand = new BaseCommand(new Action<object>(o =>
                    {
                        if (obj.UpdatePlanned)
                        {
                            Status = SettingsModel.Status.working;
                            this.RaisePropertyChanged("ProgressVis");
                            if (obj.Update())
                            {
                                this.RaisePropertyChanged("UpdateBtnCont");
                                this.RaisePropertyChanged("LaunchBtnCont");
                                Status = SettingsModel.Status.login;
                                this.RaisePropertyChanged("UpdateInfo");
                            }
                            else
                                Status = SettingsModel.Status.error;
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
        private BaseCommand clickEditCommand;
        public BaseCommand ClickEditCommand
        {
            get
            {
                if (clickEditCommand == null)
                {
                    clickEditCommand = new BaseCommand(new Action<object>(o =>
                    {
                        Status = SettingsModel.Status.menu;
                    }));
                }
                return clickEditCommand;
            }
        }
        private BaseCommand clickBackCommand;
        public BaseCommand ClickBackCommand
        {
            get
            {
                if (clickBackCommand == null)
                {
                    clickBackCommand = new BaseCommand(new Action<object>(o =>
                    {
                        if (Downloader.Program.Tencent_cos_download.CheckAlreadyDownload())
                            Status = SettingsModel.Status.login;
                        else
                            Status = SettingsModel.Status.newUser;
                    }));
                }
                return clickBackCommand;
            }
        }
        private BaseCommand clickUploadCommand;
        public BaseCommand ClickUploadCommand
        {
            get
            {
                if (clickUploadCommand == null)
                {
                    clickUploadCommand = new BaseCommand(new Action<object>(async o =>
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
        private BaseCommand clickReselectCommand;
        public BaseCommand ClickReselectCommand
        {
            get
            {
                if (clickReselectCommand == null)
                {
                    clickReselectCommand = new BaseCommand(new Action<object>(o =>
                    {
                        obj.CodeRoute = "";
                        obj.UploadReady = false;
                        this.RaisePropertyChanged("UploadBtnCont");
                        this.RaisePropertyChanged("UploadReadyVis");
                        this.RaisePropertyChanged("CodeName");
                        this.RaisePropertyChanged("CoverVis");
                    }));
                }
                return clickReselectCommand;
            }
        }
        private BaseCommand clickExitCommand;
        public BaseCommand ClickExitCommand
        {
            get
            {
                if (clickExitCommand == null)
                {
                    clickExitCommand = new BaseCommand(new Action<object>(o =>
                    {
                        System.Windows.Application.Current.Shutdown();
                    }));
                }
                return clickExitCommand;
            }
        }
    }

}