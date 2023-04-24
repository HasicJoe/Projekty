using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;

namespace Festival.App.Services
{
    class NavigationService : INavigationService
    {
        public Panel ContentPanel { get; set; }

        public void NavigateTo<TView>()
            where TView: UserControl
        {
            ContentPanel.Children.Clear();
            var view = App.host.Services.GetRequiredService<TView>();
            ContentPanel.Children.Add(view);
        }

    }
}
