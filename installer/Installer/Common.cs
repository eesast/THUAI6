using System.ComponentModel;
using System;
using System.Windows.Input;
using System.Globalization;
using System.Windows.Data;

namespace starter.viewmodel.common
{
    public abstract class NotificationObject : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        ///< summary>
        /// announce notification
        ///  </summary>
        ///< param name="propertyName">property name</param>
        public void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
    ///< summary>
    /// BaseCommand
    ///  </summary>
    public class BaseCommand : ICommand
    {
        private Func<object, bool> _canExecute;
        private Action<object> _execute;

        public BaseCommand(Func<object, bool> canExecute, Action<object> execute)
        {
            _canExecute = canExecute;
            _execute = execute;
        }

        public BaseCommand(Action<object> execute) :
            this(null, execute)
        {
        }

        public event EventHandler CanExecuteChanged
        {
            add
            {
                if (_canExecute != null)
                {
                    CommandManager.RequerySuggested += value;
                }
            }
            remove
            {
                if (_canExecute != null)
                {
                    CommandManager.RequerySuggested -= value;
                }
            }
        }

        public bool CanExecute(object parameter)
        {
            return _canExecute == null ? true : _canExecute(parameter);
        }

        public void Execute(object parameter)
        {
            if (_execute != null && CanExecute(parameter))
            {
                _execute(parameter);
            }
        }
    }

    public class RadioConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
            {
                return false;
            }
            string checkvalue = value.ToString();
            string targetvalue = parameter.ToString();
            bool r = checkvalue.Equals(targetvalue);
            return r;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
            {
                return null;
            }

            if ((bool)value)
            {
                return parameter.ToString();
            }
            return null;
        }
    }

}
