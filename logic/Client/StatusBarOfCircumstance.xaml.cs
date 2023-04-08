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
using Preparation.Utility;

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
            if (fontsize != 0)
            {
                status.FontSize = 13 * fontsize / 12;
                time.FontSize = 14 * fontsize / 12;
                name.FontSize = 14 * fontsize / 12;
                scoresOfStudents.FontSize = scoresOfTrickers.FontSize = fontsize;
            }
        }

        public void SetValue(MessageOfAll obj, bool gateOpened, bool hiddenGateRefreshed, bool hiddenGateOpened, long playerId)
        {
            int min, sec;
            sec = obj.GameTime / 1000;
            min = sec / 60;
            sec = sec % 60;
            time.Text = "Time⏳: " + Convert.ToString(min) + ": ";
            if (sec / 10 == 0)
            {
                time.Text += "0";
                time.Text += Convert.ToString(sec);
            }
            else
            {
                time.Text += Convert.ToString(sec);
            }
            if (playerId == GameData.numOfStudent)
            {
                name.Text = "🚀 Tricker's";
            }
            else if (playerId < GameData.numOfStudent)
            {
                name.Text = "🚀 Student" + Convert.ToString(playerId) + "'s";
            }
            else
            {
                name.Text = "🚀 Spectator's";
            }
            if (obj.SubjectFinished < Preparation.Utility.GameData.numOfGeneratorRequiredForRepair)
            {
                status.Text = "📱: " + Convert.ToString(obj.SubjectFinished) + "\n🚪: ";
            }
            else
            {
                status.Text = "📱: Gate can be opened" + "\n🚪: ";
            }
            if (gateOpened)
            {
                status.Text += "Open\n🆘: ";
            }
            else
            {
                status.Text += "Close\n🆘: ";
            }
            //if (obj.SubjectFinished >= Preparation.Utility.GameData.numOfGeneratorRequiredForEmergencyExit)
            //{
            //    hiddenGateRefreshed = true;
            //}
            //if (Preparation.Utility.GameData.numOfStudent == 1 + obj.StudentQuited + obj.StudentGraduated)
            //{
            //    hiddenGateOpened = true;
            //}
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
