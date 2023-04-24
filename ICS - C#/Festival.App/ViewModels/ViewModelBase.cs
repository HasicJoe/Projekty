using System.ComponentModel;
using Festival.App.ViewModels.Interfaces;
using System.Runtime.CompilerServices;
using System.Windows;

namespace Festival.App.ViewModels
{
    public abstract class ViewModelBase : IViewModel, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler? PropertyChanged;

        protected ViewModelBase()
        {
            if (DesignerProperties.GetIsInDesignMode(new DependencyObject()))
            {
                LoadInDesignMode();
            }
        }

        protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public virtual void LoadInDesignMode()
        {

        }
    }
}
