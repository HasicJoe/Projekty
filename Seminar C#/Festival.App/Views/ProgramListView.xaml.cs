using System.Windows.Controls;
using Festival.App.ViewModels;
using Syncfusion.UI.Xaml.Scheduler;

namespace Festival.App.Views
{
    /// <summary>
    /// Interaction logic for ProgramListView.xaml
    /// </summary>
    public partial class ProgramListView :UserControl
    {
        public ProgramListView(ProgramViewModel programViewModel)
        {
            InitializeComponent();
            DataContext = programViewModel;
        }

        private void Schedule_OnAppointmentTapped(object sender, AppointmentTappedArgs e)
        {
            var programViewModel = (ProgramViewModel) ((SfScheduler) sender).DataContext;
            programViewModel.ViewBandDetail(sender, e);
        }
    }
}
