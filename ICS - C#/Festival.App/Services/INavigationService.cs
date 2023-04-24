using System.Windows.Controls;

namespace Festival.App.Services
{
    public interface INavigationService
    {
        void NavigateTo<TView>()
            where TView: UserControl;

        Panel ContentPanel { get; set; }
    }
}
