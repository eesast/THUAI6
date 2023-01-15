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
using System.Windows.Shapes;

namespace Client
{
    /// <summary>
    /// StatusBarOfCircumstance.xaml 的交互逻辑
    /// </summary>
    public partial class StatusBarOfCircumstance : UserControl
    {
        public StatusBarOfCircumstance(Grid parent, int Row, int Column)
        {
            InitializeComponent();
            parent.Children.Add(this);
            Grid.SetColumn(this, Column);
            Grid.SetColumnSpan(this, 2);
            Grid.SetRow(this, Row);
            Grid.SetRowSpan(this, 2);
            initialized = false;
        }
        public void SetFontSize(double fontsize)
        {
            serial.FontSize = scores.FontSize = star.FontSize = status.FontSize = prop.FontSize = fontsize;
        }
        private bool initialized;
    }
}
