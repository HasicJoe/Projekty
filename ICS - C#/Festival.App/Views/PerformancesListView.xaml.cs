using System.Windows.Controls;
using Festival.App.ViewModels;

namespace Festival.App.Views
{
    /// <summary>
    /// Interaction logic for PerformancesListView.xaml
    /// </summary>
    public partial class PerformancesListView : UserControl
    {
        public PerformancesListView( PerformancesViewModel performancesViewModel)
        {
            InitializeComponent();
            DataContext = performancesViewModel;
        }

    }
}
