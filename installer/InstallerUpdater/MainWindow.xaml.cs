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
using Program;
using System.Diagnostics;
using System.IO;
using System.ComponentModel;

namespace InstallerUpdater
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        BackgroundWorker asyncDownloader = new BackgroundWorker();
        public MainWindow()
        {
            InitializeComponent();
            MessageBox.Show("这是旧版");
            asyncDownloader.DoWork += AsyncDownloader_DoWork;
            asyncDownloader.RunWorkerCompleted += AsyncDownloader_RunWorkerCompleted;
            if (asyncDownloader.IsBusy)
            {
                MessageBox.Show("更新失败，请汇报");
                Process.Start(System.IO.Path.Combine(Updater.Dir, "Installer.exe"));
                Application.Current.Shutdown();
            }
            else
                asyncDownloader.RunWorkerAsync();
        }

        private void AsyncDownloader_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Result == null)
            {
                MessageBox.Show("更新失败，请汇报");
            }
            else if ((bool)e.Result)
            {
                MessageBox.Show("已成功更新下载器，即将启动下载器");
            }
            else
            {
                MessageBox.Show("更新失败，请汇报");
            }
            Process.Start(System.IO.Path.Combine(Updater.Dir, "Installer.exe"));
            Application.Current.Shutdown();
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
                if (Updater.UpdateInstaller())
                    e.Result = true;
                else
                    e.Result = false;
            }
        }
    }
}
