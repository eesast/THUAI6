
using starter.viewmodel.common;
using System;
using System.Windows;
using System.Windows.Forms;
using Downloader;
using MessageBox = System.Windows.MessageBox;
using System.Configuration;

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
                this.RaisePropertyChanged("CompleteVis");
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
            get
            {
                return obj.Username;
            }
            set
            {
                obj.Username = value;
                this.RaisePropertyChanged("Username");
            }
        }
        public string Password
        {
            get { return obj.Password; }
            set
            {
                obj.Password = value;
                this.RaisePropertyChanged("Password");
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

        public string UpdateBtnCont
        {
            get
            {
                return obj.UpdatePlanned ? "Update" : "Check Updates";
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

        private BaseCommand clickBrowseCommand;
        public BaseCommand ClickBrowseCommand
        {
            get
            {
                if (clickBrowseCommand == null)
                {
                    clickBrowseCommand = new BaseCommand(new Action<object>(o =>
                    {
                        using (FolderBrowserDialog dialog = new FolderBrowserDialog())
                        {
                            _ = dialog.ShowDialog();
                            if (dialog.SelectedPath != String.Empty)
                                Route = dialog.SelectedPath;
                        }
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
                            if (obj.install())
                            {
                                Status = SettingsModel.Status.menu;
                            }

                        }
                        else if (Status == SettingsModel.Status.move)
                        {
                            Status = SettingsModel.Status.working;
                            switch (obj.move())
                            {
                                case -1:
                                    MessageBox.Show("文件已打开或者目标路径下有同名文件！", "", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                                    break;
                                case -2:
                                    Status = SettingsModel.Status.error;
                                    break;
                            }
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
                            if (obj.Update())
                            {

                                Status = SettingsModel.Status.successful;
                                this.RaisePropertyChanged("UpdateButtonCont");

                            }
                            else
                                Status = SettingsModel.Status.error;
                        }
                        else
                        {
                            Status = SettingsModel.Status.working;
                            Status = obj.checkUpdate();
                            this.RaisePropertyChanged("UpdateButtonCont");
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
                    clickLoginCommand = new BaseCommand(new Action<object>(o =>
                    {
                        obj.Login();
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
                        if (!obj.Launch())
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
        private BaseCommand clickLogoutCommand;
        public BaseCommand ClickLogoutCommand
        {
            get
            {
                if (clickLogoutCommand == null)
                {
                    clickLogoutCommand = new BaseCommand(new Action<object>(o =>
                    {
                        Status = SettingsModel.Status.login;
                    }));
                }
                return clickLogoutCommand;
            }
        }
    }
}