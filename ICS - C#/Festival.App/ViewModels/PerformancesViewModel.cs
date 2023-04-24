using System;
using System.Collections.ObjectModel;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.DetailModels;

namespace Festival.App.ViewModels
{
    public class PerformancesViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly PerformanceFacade _performanceFacade;

        public RelayCommand AddPerformanceViewCommand { get; set; }

        public RelayCommand<Guid> EditPerformanceCommand { get; set; }
        public RelayCommand<Guid> DeletePerformanceCommand { get; set; }


        public ObservableCollection<PerformanceDetailModel> Performances { get; set; }

        public PerformancesViewModel(IMediator mediator, INavigationService navigationService,
            PerformanceFacade performanceFacade, BandFacade bandFacade, StageFacade stageFacade)

        {
            Performances = new ObservableCollection<PerformanceDetailModel>();

            _mediator = mediator;
            _navigationService = navigationService;
            _performanceFacade = performanceFacade;

            bandFacade.GetAllList();
            stageFacade.GetAllList();

            foreach (var performance in _performanceFacade.GetAllList())
            {
                Performances.Add(_performanceFacade.GetById(performance.Id));
            }

            AddPerformanceViewCommand = new RelayCommand(Add);
            EditPerformanceCommand = new RelayCommand<Guid>(Edit);
        }

        public void Add()
        {
            _navigationService.NavigateTo<AddPerformanceView>();
            _mediator.Send(new NewMessage<PerformanceDetailModel>());
        }

        public void Edit(Guid modelId)
        {
            _navigationService.NavigateTo<AddPerformanceView>();
            _mediator.Send(new SelectedMessage<PerformanceDetailModel> { Id = modelId });
        }
    }
}