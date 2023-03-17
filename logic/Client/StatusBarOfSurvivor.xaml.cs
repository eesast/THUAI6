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

        private void SetStaticValue(MessageOfStudent obj)
        {
            switch (obj.StudentType)  // 参数未设定
            {
                case StudentType._1:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:StudentType1";
                    break;
                case StudentType._2:
                    coolTime = 20000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:StudentType2";
                    break;
                case StudentType._3:
                    coolTime = 30000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:StudentType3";
                    break;
                case StudentType._4:
                    coolTime = 40000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:StudentType4";
                    break;
                case StudentType.NullStudentType:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\nSkill:NullStudentType";
                    break;
            }
            initialized = true;
        }
        private void SetDynamicValue(MessageOfStudent obj)
        {
            skillprogress.Value = 100 - obj.TimeUntilSkillAvailable[0] / coolTime * 100;
            if (obj.State == PlayerState.Quit)
            {
                skillprogress.Value = 0;
                skillprogress.Background = Brushes.Gray;
            }
            else
                skillprogress.Background = Brushes.White;
            Func<MessageOfStudent, int> life =
                (obj) =>
            {
                if (obj.State == PlayerState.Quit || obj.State == PlayerState.Addicted)
                    return 0;
                else
                    return obj.Determination;
            };
            // star.Text = "⭐：";准备放剩余被挂次数
            status.Text = "🔧：" + Convert.ToString(0) + "\n🏃：" + Convert.ToString(obj.Speed) + "\n♥：" + Convert.ToString(life(obj)) + "\n🛡：" + Convert.ToString(0);
            scores.Text = "Scores:" +obj.Score;
            foreach (var icon in obj.Prop)
            {
                switch (icon)
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
        }
        public void SetValue(MessageOfStudent obj)
        {
            if (!initialized)
                SetStaticValue(obj);
            SetDynamicValue(obj);
        }
        private int coolTime;
        private bool initialized;
    }
}
