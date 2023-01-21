
using starter.viewmodel.common;
using System;
using System.Windows;
using System.Windows.Forms;

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
                Installed = true;
            }
            else
            {
                Route = Environment.GetEnvironmentVariable("USERPROFILE") + "\\THUAI6";
                EditingRoute = true;
            }
        }

        public string Route
        {
            get {
                return obj.Route;
            }
            set {
                obj.Route = value;
                this.RaisePropertyChanged("Route");
            }
        }
        public bool Installed
        {
            get {
                return obj.installed;
            }
            set {
                obj.installed = value;
                this.RaisePropertyChanged("Installed");
                this.RaisePropertyChanged("InstIntroVis");
                this.RaisePropertyChanged("EditIntroVis");
                this.RaisePropertyChanged("MoveIntroVis");
            }
        }
        public bool EditingRoute
        {
            get {
                return obj.EditingRoute;
            }
            set {
                obj.EditingRoute = value;
                this.RaisePropertyChanged("EditingRoute");
                this.RaisePropertyChanged("MoveIntroVis");
                this.RaisePropertyChanged("RouteBoxVis");
            }
        }
        public Visibility RouteBoxVis  // if the route editing textbox is visible
        {
            get {
                return obj.EditingRoute ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        /// <summary>
        /// if the install/edit instruction can be seen
        /// </summary>
        public Visibility InstIntroVis
        {
            get {
                return obj.installed ? Visibility.Collapsed : Visibility.Visible;
            }
        }
        public Visibility EditIntroVis
        {
            get {
                return obj.installed ? Visibility.Visible : Visibility.Collapsed;
            }
        }
        public Visibility MoveIntroVis
        {
            get {
                if (obj.installed == true && obj.EditingRoute == true)
                    return Visibility.Visible;
                else
                    return Visibility.Collapsed;
            }
        }

        private BaseCommand clickBrowseCommand;
        public BaseCommand ClickBrowseCommand
        {
            get {
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
    get {
        if (clickConfirmCommand == null)
        {
                    clickConfirmCommand = new BaseCommand(new Action<object>(o =>
                    {
                        if (obj.install())
                        {
                            EditingRoute = false;
                    Installed = true;
        }
    }));
                }
return clickConfirmCommand;
}
}
}
}