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
using Protobuf;

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
        }
        public void SetFontSize(double fontsize)
        {
            time.FontSize = scoresofstudents.FontSize = scoresoftrickers.FontSize = status.FontSize = prop.FontSize = fontsize;
        }

        public void SetValue(MessageOfAll obj)
        {
            time.Text = "Time⏳: " + Convert.ToString(obj.GameTime);
            status.Text = "📱: " + Convert.ToString(obj.SubjectLeft) + "\n🚪: ";
            if(obj.GateOpened)
            {
                status.Text += "Open\n🆘: ";
            }
            else
            {
                status.Text += "Close\n🆘: ";
            }
            if(obj.HiddenGateRefreshed)
            {
                if(obj.HiddenGateOpened)
                {
                    status.Text += "Open\n🏃: ";
                }
                else
                {
                    status.Text += "Refreshed\n🏃: ";
                }
            }
            else
            {
                status.Text += "Unrefreshed\n🏃: ";
            }
            status.Text += Convert.ToString(obj.StudentGraduated)+ "\n⚰️: ";
            status.Text += Convert.ToString(obj.StudentQuited);
            scoresofstudents.Text = "Scores of Survivors: " + Convert.ToString(obj.StudentScore);
            scoresoftrickers.Text = "Scores of Hunters: " + Convert.ToString(obj.TrickerScore);
        }
    }
}
