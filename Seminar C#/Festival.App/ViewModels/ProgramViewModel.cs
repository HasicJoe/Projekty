using System;
using System.Collections.ObjectModel;
using System.Windows.Media;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Syncfusion.UI.Xaml.Scheduler;

namespace Festival.App.ViewModels
{

    public class ProgramViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        public ObservableCollection<PerformanceListModel> Events { get; set; }
        public ObservableCollection<SchedulerResource> ResourceCollection { get; set; }

        public ProgramViewModel(IMediator mediator, INavigationService navigationService,
            PerformanceFacade performanceFacade, StageFacade stageFacade, BandFacade bandFacade)
        {
            Events = new ObservableCollection<PerformanceListModel>();
            ResourceCollection = new ObservableCollection<SchedulerResource>();

            _mediator = mediator;
            _navigationService = navigationService;

            Random rnd = new Random();

            bandFacade.GetAllList();

            // create Resource list for scheduler
            foreach (var stage in stageFacade.GetAllList())
            {
                ResourceCollection.Add(new SchedulerResource()
                {
                    Background = new SolidColorBrush(Color.FromRgb(
                        (byte)rnd.Next(150),
                        (byte)rnd.Next(150),
                        (byte)rnd.Next(150))),
                    Foreground = new SolidColorBrush(Color.FromRgb(0xff, 0xff, 0xff)),
                    Name = stage.Name,
                    Id = stage.Id
                });
            }
            
            // fill Event list for scheduler
            foreach (var performance in performanceFacade.GetAllList())
            {
                Events.Add(performance);
            }
        }

        public void ViewBandDetail(object sender, AppointmentTappedArgs e)
        {
            if (e.Appointment == null)
            {
                return;
            }
            Guid bandId = ((PerformanceListModel) (e.Appointment.Data)).BandId;
            _navigationService.NavigateTo<BandDetailView>();
            _mediator.Send(new SelectedMessage<BandDetailModel> { Id = bandId});
        }
    }
}