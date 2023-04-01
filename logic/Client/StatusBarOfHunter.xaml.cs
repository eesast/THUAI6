using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
            serial.FontSize = scores.FontSize = state.FontSize = status.FontSize = activeSkill0.FontSize = activeSkill1.FontSize = activeSkill2.FontSize = prop0.FontSize = prop1.FontSize = prop2.FontSize = prop3.FontSize = fontsize;
        }

        private void SetStaticValue(MessageOfTricker obj)
        {
            switch (obj.TrickerType)  // 参数未设定
            {
                case TrickerType.Assassin:
                    coolTime0 = coolTime1 = coolTime2 = 10000;
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\n职业: Assassin";
                    break;
                case TrickerType.Klee:
                    coolTime0 = coolTime1 = coolTime2 = 20000;
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\n职业: Klee";
                    break;
                case TrickerType.ANoisyPerson:
                    coolTime0 = coolTime1 = coolTime2 = 30000;
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\n职业: ANoisyPerson";
                    break;
                case TrickerType._4:
                    coolTime0 = coolTime1 = coolTime2 = 40000;
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\n职业: TrickerType4";
                    break;
                case TrickerType.NullTrickerType:
                    coolTime0 = coolTime1 = coolTime2 = -1;
                    serial.Text = "👥" + Convert.ToString(1) + "👻" + Convert.ToString(obj.PlayerId) + "\n职业: NullTrickerType";
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
                skillprogress0.Value = 100 - obj.TimeUntilSkillAvailable[0] / coolTime0 * 100;
            if (obj.TimeUntilSkillAvailable[1] >= 0)
                skillprogress1.Value = 100 - obj.TimeUntilSkillAvailable[1] / coolTime1 * 100;
            if (obj.TimeUntilSkillAvailable[2] >= 0)
                skillprogress2.Value = 100 - obj.TimeUntilSkillAvailable[2] / coolTime2 * 100;
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
                        SetPropValue(prop0, icon);
                        cnt++;
                        break;
                    case 1:
                        SetPropValue(prop0, icon);
                        cnt++;
                        break;
                    case 2:
                        SetPropValue(prop0, icon);
                        cnt++;
                        break;
                    case 3:
                        SetPropValue(prop0, icon);
                        cnt++;
                        break;
                    default:
                        break;
                }
            }
        }
        public void SetPropValue(TextBox textBox, Protobuf.PropType propType)
        {
            switch (propType)
            {
                case Protobuf.PropType.Key3:
                    textBox.Text = "🔑";
                    break;
                case Protobuf.PropType.Key5:
                    textBox.Text = "🔑";
                    break;
                case Protobuf.PropType.Key6:
                    textBox.Text = "🔑";
                    break;
                case Protobuf.PropType.AddSpeed:
                    textBox.Text = "⛸";
                    break;
                case Protobuf.PropType.AddLifeOrClairaudience:
                    textBox.Text = "🏅";
                    break;
                case Protobuf.PropType.AddHpOrAp:
                    textBox.Text = "♥";
                    break;
                case Protobuf.PropType.ShieldOrSpear:
                    textBox.Text = "🛡";
                    break;
                case Protobuf.PropType.RecoveryFromDizziness:
                    textBox.Text = "🕶";
                    break;
                default:
                    textBox.Text = "";
                    break;
            }
        }
        public void SetValue(MessageOfTricker obj)
        {
            if (!initialized)
                SetStaticValue(obj);
            SetDynamicValue(obj);
        }
        private int coolTime0, coolTime1, coolTime2;
        private bool initialized;
    }
}
