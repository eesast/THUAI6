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
    /// StatusBarOfHunter.xaml 的交互逻辑
    /// </summary>
    public partial class StatusBarOfHunter : UserControl
    {
        public StatusBarOfHunter(Grid parent, int Row, int Column)
        {
            InitializeComponent();
            parent.Children.Add(this);
            Grid.SetColumn(this, Column);
            Grid.SetColumnSpan(this, 2);
            Grid.SetRow(this, Row);
            initialized = false;
        }
        public void SetFontSize(double fontsize)
        {
            if (fontsize != 0)
                serial.FontSize = scores.FontSize = state.FontSize = status.FontSize = activeSkill0.FontSize = activeSkill1.FontSize = activeSkill2.FontSize = prop0.FontSize = prop1.FontSize = prop2.FontSize = fontsize;
        }

        private void SetStaticValue(MessageOfTricker obj)
        {
            switch (obj.TrickerType)  // 参数未设定
            {
                case TrickerType.Assassin:
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\nAssassin";
                    break;
                case TrickerType.Klee:
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\nKlee";
                    break;
                case TrickerType.ANoisyPerson:
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\nANoisyPerson";
                    break;
                case TrickerType._4:
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\nTrickerType4";
                    break;
                case TrickerType.NullTrickerType:
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\nNullTrickerType";
                    break;
            }
            activeSkill0.Text = "Skill0";
            activeSkill1.Text = "Skill1";
            activeSkill2.Text = "Skill2";
            initialized = true;
        }
        private void SetDynamicValue(MessageOfTricker obj)
        {
            status.Text = "🏃🏿‍: " + Convert.ToString(obj.Speed);
            switch (obj.PlayerState)
            {
                case PlayerState.Idle:
                    state.Text = "Idle";
                    break;
                case PlayerState.Learning:
                    state.Text = "Learning";
                    break;
                case PlayerState.Addicted:
                    state.Text = "Addicted";
                    break;
                case PlayerState.Graduated:
                    state.Text = "Graduated";
                    break;
                case PlayerState.Quit:
                    state.Text = "Quit";
                    break;
                case PlayerState.Treated:
                    state.Text = "Treated";
                    break;
                case PlayerState.Rescued:
                    state.Text = "Rescued";
                    break;
                case PlayerState.Stunned:
                    state.Text = "Stunned";
                    break;
                case PlayerState.Treating:
                    state.Text = "Treating";
                    break;
                case PlayerState.Rescuing:
                    state.Text = "Rescuing";
                    break;
                case PlayerState.Swinging:
                    state.Text = "Swinging";
                    break;
                case PlayerState.Attacking:
                    state.Text = "Attacking";
                    break;
                case PlayerState.Locking:
                    state.Text = "Locking";
                    break;
                case PlayerState.Rummaging:
                    state.Text = "Rummaging";
                    break;
                case PlayerState.Climbing:
                    state.Text = "Climbing";
                    break;
                case PlayerState.OpeningAChest:
                    state.Text = "OpeningAChest";
                    break;
                case PlayerState.UsingSpecialSkill:
                    state.Text = "UsingSpecialSkill";
                    break;
                case PlayerState.OpeningAGate:
                    state.Text = "OpeningAGate";
                    break;
                default:
                    break;
            }
            scores.Text = "Scores: " + Convert.ToString(obj.Score);
            if (obj.TimeUntilSkillAvailable[0] >= 0)
                skillprogress0.Value = 100 - 100.0 * obj.TimeUntilSkillAvailable[0] / coolTime0[4];
            if (obj.TimeUntilSkillAvailable[1] >= 0)
                skillprogress1.Value = 100 - 100.0 * obj.TimeUntilSkillAvailable[1] / coolTime1[4];
            if (obj.TimeUntilSkillAvailable[2] >= 0)
                skillprogress2.Value = 100 - 100.0 * obj.TimeUntilSkillAvailable[2] / coolTime2[4];
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

        public void SetValue(MessageOfTricker obj)
        {
            if (!initialized)
                SetStaticValue(obj);
            SetDynamicValue(obj);
        }
        public void NewData(int[] life, int[] death, int[,] coolTime)
        {
            for (int i = 0; i < GameData.numOfStudent; ++i)
            {
                totalLife[i] = life[i];
                totalDeath[i] = death[i];
                coolTime0[i] = coolTime[0, i];
                coolTime1[i] = coolTime[1, i];
                coolTime2[i] = coolTime[2, i];
            }
            coolTime0[4] = coolTime[0, 4];
            coolTime1[4] = coolTime[1, 4];
            coolTime2[4] = coolTime[2, 4];
        }
        private int[] totalLife = new int[4] { 100, 100, 100, 100 }, totalDeath = new int[4] { 100, 100, 100, 100 };
        private int[] coolTime0 = new int[5] { 100, 100, 100, 100, 100 }, coolTime1 = new int[5] { 100, 100, 100, 100, 100 }, coolTime2 = new int[5] { 100, 100, 100, 100, 100 };
        private bool initialized;
    }
}
