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
    /// StatusBarOfSurvivor.xaml 的交互逻辑
    /// </summary>
    public partial class StatusBarOfSurvivor : UserControl
    {
        public StatusBarOfSurvivor(Grid parent, int Row, int Column)
        {
            InitializeComponent();
            parent.Children.Add(this);
            Grid.SetColumn(this, Column);
            Grid.SetRow(this, Row);
            initialized = false;
        }
        public void SetFontSize(double fontsize)
        {
            serial.FontSize = scores.FontSize = status.FontSize = activeSkill0.FontSize = activeSkill1.FontSize = activeSkill2.FontSize = prop0.FontSize = prop1.FontSize = prop2.FontSize = fontsize;
        }

        private void SetStaticValue(MessageOfStudent obj)
        {
            switch (obj.StudentType)  // coolTime参数未设定，
            {
                case StudentType.Athlete:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nAthlete";
                    break;
                case StudentType.Teacher:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nTeacher";
                    break;
                case StudentType.StraightAStudent:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nStraightAStudent";
                    break;
                case StudentType.Robot:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nRobot";
                    break;
                case StudentType.TechOtaku:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nTechOtaku";
                    break;
                case StudentType.NullStudentType:
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nNullStudentType";
                    break;
            }
            activeSkill0.Text = "Skill0";
            activeSkill1.Text = "Skill1";
            activeSkill2.Text = "Skill2";
            initialized = true;
        }
        private void SetDynamicValue(MessageOfStudent obj, long i)
        {
            int life = obj.Determination, death = obj.Addiction;
            int perLife = (int)(100.0 * life / totalLife[i]), perDeath = (int)(100.0 * death / totalDeath[i]);
            if (perLife > 100)
                perLife = 0;
            if (perDeath > 100)
                perDeath = 0;
            switch (obj.PlayerState)
            {
                case PlayerState.Idle:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nIdle";
                    break;
                case PlayerState.Learning:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nLearning";
                    break;
                case PlayerState.Addicted:
                    status.Text = "💀: " + Convert.ToString(death) + "," + Convert.ToString(perDeath) + "%\nAddicted";
                    break;
                case PlayerState.Graduated:
                    status.Text = "♥" + "\nGraduated";
                    break;
                case PlayerState.Quit:
                    status.Text = "💀" + "\nQuit";
                    break;
                case PlayerState.Treated:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nTreated";
                    break;
                case PlayerState.Rescued:
                    status.Text = "💀: " + Convert.ToString(death) + "," + Convert.ToString(perDeath) + "%\nRescued";
                    break;
                case PlayerState.Stunned:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nStunned";
                    break;
                case PlayerState.Treating:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nTreating";
                    break;
                case PlayerState.Rescuing:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nRescuing";
                    break;
                case PlayerState.Swinging:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nSwinging";
                    break;
                case PlayerState.Attacking:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nAttacking";
                    break;
                case PlayerState.Locking:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nLocking";
                    break;
                case PlayerState.Rummaging:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nRummaging";
                    break;
                case PlayerState.Climbing:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nClimbing";
                    break;
                case PlayerState.OpeningAChest:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nOpeningAChest";
                    break;
                case PlayerState.UsingSpecialSkill:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nUsingSpecialSkill";
                    break;
                case PlayerState.OpeningAGate:
                    status.Text = "♥: " + Convert.ToString(life) + "," + Convert.ToString(perLife) + "%\nOpeningAGate";
                    break;
                default:
                    break;
            }
            scores.Text = "Scores: " + obj.Score;
            if (obj.TimeUntilSkillAvailable[0] >= 0)
                skillprogress0.Value = 100 - obj.TimeUntilSkillAvailable[0] / coolTime0[i] * 100;
            if (obj.TimeUntilSkillAvailable[1] >= 0)
                skillprogress1.Value = 100 - obj.TimeUntilSkillAvailable[1] / coolTime1[i] * 100;
            if (obj.TimeUntilSkillAvailable[2] >= 0)
                skillprogress2.Value = 100 - obj.TimeUntilSkillAvailable[2] / coolTime2[i] * 100;
            if (obj.PlayerState == PlayerState.Quit)
            {
                skillprogress0.Value = skillprogress1.Value = skillprogress2.Value = 0;
                skillprogress0.Background = skillprogress1.Background = skillprogress2.Background = Brushes.Gray;
            }
            else
                skillprogress0.Background = skillprogress1.Background = skillprogress2.Background = Brushes.White;
            int cnt = 0;
            foreach (var icon in obj.Prop)
            {
                switch (cnt)
                {
                    case 0:
                        switch (icon)
                        {
                            case Protobuf.PropType.Key3:
                                prop0.Text = "🔑3";
                                break;
                            case Protobuf.PropType.Key5:
                                prop0.Text = "🔑5";
                                break;
                            case Protobuf.PropType.Key6:
                                prop0.Text = "🔑6";
                                break;
                            case Protobuf.PropType.AddSpeed:
                                prop0.Text = "⛸";
                                break;
                            case Protobuf.PropType.AddLifeOrClairaudience:
                                prop0.Text = "🏅";
                                break;
                            case Protobuf.PropType.AddHpOrAp:
                                prop0.Text = "♥";
                                break;
                            case Protobuf.PropType.ShieldOrSpear:
                                prop0.Text = "🛡";
                                break;
                            case Protobuf.PropType.RecoveryFromDizziness:
                                prop0.Text = "🕶";
                                break;
                            default:
                                prop0.Text = "";
                                break;
                        }
                        cnt++;
                        break;
                    case 1:
                        switch (icon)
                        {
                            case Protobuf.PropType.Key3:
                                prop1.Text = "🔑3";
                                break;
                            case Protobuf.PropType.Key5:
                                prop1.Text = "🔑5";
                                break;
                            case Protobuf.PropType.Key6:
                                prop1.Text = "🔑6";
                                break;
                            case Protobuf.PropType.AddSpeed:
                                prop1.Text = "⛸";
                                break;
                            case Protobuf.PropType.AddLifeOrClairaudience:
                                prop1.Text = "🏅";
                                break;
                            case Protobuf.PropType.AddHpOrAp:
                                prop1.Text = "♥";
                                break;
                            case Protobuf.PropType.ShieldOrSpear:
                                prop1.Text = "🛡";
                                break;
                            case Protobuf.PropType.RecoveryFromDizziness:
                                prop1.Text = "🕶";
                                break;
                            default:
                                prop1.Text = "";
                                break;
                        }
                        cnt++;
                        break;
                    case 2:
                        switch (icon)
                        {
                            case Protobuf.PropType.Key3:
                                prop2.Text = "🔑3";
                                break;
                            case Protobuf.PropType.Key5:
                                prop2.Text = "🔑5";
                                break;
                            case Protobuf.PropType.Key6:
                                prop2.Text = "🔑6";
                                break;
                            case Protobuf.PropType.AddSpeed:
                                prop2.Text = "⛸";
                                break;
                            case Protobuf.PropType.AddLifeOrClairaudience:
                                prop2.Text = "🏅";
                                break;
                            case Protobuf.PropType.AddHpOrAp:
                                prop2.Text = "♥";
                                break;
                            case Protobuf.PropType.ShieldOrSpear:
                                prop2.Text = "🛡";
                                break;
                            case Protobuf.PropType.RecoveryFromDizziness:
                                prop2.Text = "🕶";
                                break;
                            default:
                                prop2.Text = "";
                                break;
                        }
                        cnt++;
                        break;
                    default:
                        break;
                }
            }
        }

        public void SetValue(MessageOfStudent obj, long i)
        {
            if (!initialized)
                SetStaticValue(obj);
            SetDynamicValue(obj, i);
        }
        private int[] totalLife = new int[4] { 100, 100, 100, 100 }, totalDeath = new int[4] { 100, 100, 100, 100 };
        private int[] coolTime0 = new int[5] { 100, 100, 100, 100, 100 }, coolTime1 = new int[5] { 100, 100, 100, 100, 100 }, coolTime2 = new int[5] { 100, 100, 100, 100, 100 };
        private bool initialized;
    }
}
