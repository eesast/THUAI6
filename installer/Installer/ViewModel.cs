
using starter.viewmodel.common;
using System;
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
            Route = Environment.GetEnvironmentVariable("USERPROFILE") + "\\THUAI6";
            CanEditRoute = true;
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

        public bool CanEditRoute  // if the user can still edit install route
        {
            get
            {
                return !obj.HaveRoute;
            }
            set
            {
                obj.HaveRoute = !value;
                obj.EditingRoute = value;
                this.RaisePropertyChanged("CanEditRoute");
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
                        CanEditRoute = false;
                        obj.install();
                    }));
                }
                return clickConfirmCommand;
            }
        }
    }
}