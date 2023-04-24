using System.Windows;
using Festival.App.Commands;
using Festival.App.Services;
using Festival.App.Views;

namespace Festival.App.ViewModels
{
    public class MainViewModel : ViewModelBase
    {

        private readonly INavigationService _navigationService;

        public RelayCommand HomeViewCommand { get; set; }
        public RelayCommand PerformancesViewCommand { get; set; }

        public RelayCommand BandsViewCommand { get; set; }
        public RelayCommand StagesViewCommand { get; set; }
        public RelayCommand ProgramViewCommand { get; set; }

        public RelayCommand AppExitCommand { get; set; }

        public RelayCommand AppMinimizeCommand { get; set; }

        public RelayCommand AppMaximizeCommand { get; set; }
        public RelayCommand AppMoveCommand { get; set; }

        public MainViewModel(INavigationService navigationService)
        {
            _navigationService = navigationService;

            HomeViewCommand =           new RelayCommand(() => _navigationService.NavigateTo<HomeView>());

            PerformancesViewCommand =   new RelayCommand(() => _navigationService.NavigateTo<PerformancesListView>());

            BandsViewCommand =          new RelayCommand(() => _navigationService.NavigateTo<BandsListView>());

            StagesViewCommand =         new RelayCommand(() => _navigationService.NavigateTo<StageListView>());

            ProgramViewCommand =        new RelayCommand(() => _navigationService.NavigateTo<ProgramListView>());

            AppExitCommand =            new RelayCommand(() => Application.Current.Shutdown());


            AppMinimizeCommand =        new RelayCommand(() => Application.Current.MainWindow.WindowState = WindowState.Minimized);

            AppMaximizeCommand =        new RelayCommand(() =>
            {
                if (Application.Current.MainWindow.WindowState == WindowState.Maximized)
                {
                    Application.Current.MainWindow.WindowState = WindowState.Normal;
                }
                else
                {
                    Application.Current.MainWindow.WindowState = WindowState.Maximized;
                }
            });

        }

        public void OnLoaded()
        {
            _navigationService.NavigateTo<HomeView>();
        }

    }
}
