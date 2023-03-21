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
            serial.FontSize = scores.FontSize = status.FontSize = activeSkill0.FontSize = activeSkill1.FontSize = activeSkill2.FontSize = prop0.FontSize = prop1.FontSize = prop2.FontSize = prop3.FontSize = fontsize;
        }

        private void SetStaticValue(MessageOfStudent obj)
        {
            switch (obj.StudentType)  // 参数未设定
            {
                case StudentType.Athlete:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\n职业：StudentType1";
                    break;
                case StudentType._2:
                    coolTime = 20000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\n职业：StudentType2";
                    break;
                case StudentType._3:
                    coolTime = 30000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\n职业：StudentType3";
                    break;
                case StudentType._4:
                    coolTime = 40000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\n职业：StudentType4";
                    break;
                case StudentType.NullStudentType:
                    coolTime = 10000;
                    serial.Text = "👥" + Convert.ToString(2) + "🧓" + Convert.ToString(obj.PlayerId) + "\n职业：NullStudentType";
                    break;
            }
            initialized = true;
        }
        private void SetDynamicValue(MessageOfStudent obj)
        {
            int life;
            switch(obj.PlayerState)
            {
                case PlayerState.Idle:
                    life = obj.Determination;
                    status.Text = "♥：" + Convert.ToString(life);
                    break;
                case PlayerState.Addicted:
                    life = obj.Addiction;
                    status.Text = "💀：" + Convert.ToString(life);
                    break;
                case PlayerState.Graduated:
                    status.Text = "Graduated";
                    break;
                case PlayerState.Quit:
                    status.Text = "Quit";
                    break;
                default:
                    break;
            }//不完全
            scores.Text = "Scores:" + obj.Score;
            if (obj.TimeUntilSkillAvailable[0]>=0)
                skillprogress0.Value = 100 - obj.TimeUntilSkillAvailable[0] / coolTime * 100;
            if(obj.TimeUntilSkillAvailable[1] >= 0)
                skillprogress1.Value = 100 - obj.TimeUntilSkillAvailable[1] / coolTime * 100;
            if(obj.TimeUntilSkillAvailable[2] >= 0)
                skillprogress2.Value = 100 - obj.TimeUntilSkillAvailable[2] / coolTime * 100;
            if (obj.PlayerState == PlayerState.Quit)
            {
                skillprogress0.Value = skillprogress1.Value = skillprogress2.Value = 0;
                skillprogress0.Background = skillprogress1.Background= skillprogress2.Background=Brushes.Gray;
            }
            else
                skillprogress0.Background = skillprogress1.Background= skillprogress2.Background=Brushes.White;
            int cnt = 0;
            foreach (var icon in obj.Prop)
            {
                switch(cnt)
                {
                    case 0:
                        switch (icon)
                        {
                            case PropType.Key3:
                                prop0.Text = "🔧";
                                break;
                            case PropType.Key5:
                                prop0.Text = "🛡";
                                break;
                            case PropType.Key6:
                                prop0.Text = "♥";
                                break;
                            case PropType.Ptype4:
                                prop0.Text = "⛸";
                                break;
                            default:
                                prop0.Text = "  ";
                                break;
                        }
                        cnt++;
                        break;
                    case 1:
                        switch (icon)
                        {
                            case PropType.Key3:
                                prop1.Text = "🔧";
                                break;
                            case PropType.Key5:
                                prop1.Text = "🛡";
                                break;
                            case PropType.Key6:
                                prop1.Text = "♥";
                                break;
                            case PropType.Ptype4:
                                prop1.Text = "⛸";
                                break;
                            default:
                                prop1.Text = "  ";
                                break;
                        }
                        cnt++;
                        break;
                    case 2:
                        switch (icon)
                        {
                            case PropType.Key3:
                                prop2.Text = "🔧";
                                break;
                            case PropType.Key5:
                                prop2.Text = "🛡";
                                break;
                            case PropType.Key6:
                                prop2.Text = "♥";
                                break;
                            case PropType.Ptype4:
                                prop2.Text = "⛸";
                                break;
                            default:
                                prop2.Text = "  ";
                                break;
                        }
                        cnt++;
                        break;
                    case 3:
                        switch (icon)
                        {
                            case PropType.Key3:
                                prop3.Text = "🔧";
                                break;
                            case PropType.Key5:
                                prop3.Text = "🛡";
                                break;
                            case PropType.Key6:
                                prop3.Text = "♥";
                                break;
                            case PropType.Ptype4:
                                prop3.Text = "⛸";
                                break;
                            default:
                                prop3.Text = "  ";
                                break;
                        }
                        cnt++;
                        break;
                    default:
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
