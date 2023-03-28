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
            status.FontSize = 13 * fontsize / 12;
            time.FontSize = 13 * fontsize / 12;
            scoresOfStudents.FontSize = scoresOfTrickers.FontSize = fontsize;
        }

        public void SetValue(MessageOfAll obj, bool gateOpened)
        {
            bool hiddenGateRefreshed = false, hiddenGateOpened = false;
            time.Text = "Time⏳: " + Convert.ToString(obj.GameTime);
            status.Text = "📱: " + Convert.ToString(obj.SubjectFinished) + "\n🚪: ";
            if (gateOpened)
            {
                status.Text += "Open\n🆘: ";
            }
            else
            {
                status.Text += "Close\n🆘: ";
            }
            if (obj.SubjectFinished >= Preparation.Utility.GameData.numOfGeneratorRequiredForEmergencyExit)
            {
                hiddenGateRefreshed = true;
            }
            if (Preparation.Utility.GameData.numOfStudent == 1 + obj.StudentQuited + obj.StudentGraduated)
            {
                hiddenGateOpened = true;
            }
            if (hiddenGateRefreshed)
            {
                if (hiddenGateOpened)
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
            status.Text += Convert.ToString(obj.StudentGraduated) + "\n⚰️: ";
            status.Text += Convert.ToString(obj.StudentQuited);
            scoresOfStudents.Text = "Scores of Students: " + Convert.ToString(obj.StudentScore);
            scoresOfTrickers.Text = "Scores of Tricker: " + Convert.ToString(obj.TrickerScore);
        }
    }
}
