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
            serial.FontSize = scores.FontSize = star.FontSize = status.FontSize = prop.FontSize = fontsize;
        }

        private void SetStaticValue(MessageOfHuman obj)
        {
            switch (obj.HumanType)  // 参数未设定
            {
                case HumanType._1:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:HumanType1";
                    break;
                case HumanType._2:
                    coolTime = 20000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:HumanType2";
                    break;
                case HumanType._3:
                    coolTime = 30000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:HumanType3";
                    break;
                case HumanType._4:
                    coolTime = 40000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:HumanType4";
                    break;
                case HumanType.NullHumanType:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:NullHumanType";
                    break;
            }
            initialized = true;
        }
        private void SetDynamicValue(MessageOfHuman obj)
        {
            skillprogress.Value = 100 - obj.TimeUntilSkillAvailable / coolTime * 100;
            if (obj.State == HumanState.Dead)
            {
                skillprogress.Value = 0;
                skillprogress.Background = Brushes.Gray;
            }
            else
                skillprogress.Background = Brushes.White;
            Func<MessageOfHuman, int> life =
                (obj) =>
            {
                if (obj.State == HumanState.Dead || obj.State == HumanState.OnChair || obj.State == HumanState.Dying)
                    return 0;
                else
                    return obj.Life;
            };
            // star.Text = "⭐：";准备放剩余被挂次数
            status.Text = "🔧：" + Convert.ToString(1) + "\n🏃：" + Convert.ToString(obj.Speed) + "\n♥：" + Convert.ToString(life(obj)) + "\n🛡：" + Convert.ToString(0);
            scores.Text = "Scores:" + Convert.ToString(0);
            switch (obj.Prop)
            {
                case PropType.Ptype1:
                    prop.Text = "🔧";
                    break;
                case PropType.Ptype2:
                    prop.Text = "🛡";
                    break;
                case PropType.Ptype3:
                    prop.Text = "♥";
                    break;
                case PropType.Ptype4:
                    prop.Text = "⛸";
                    break;
                default:
                    prop.Text = "  ";
                    break;
            }
        }
        public void SetValue(MessageOfHuman obj)
        {
            if (!initialized)
                SetStaticValue(obj);
            SetDynamicValue(obj);
        }
        private int coolTime;
        private bool initialized;
    }
}
