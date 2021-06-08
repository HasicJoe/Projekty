using System;
using System.Collections.ObjectModel;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.ListModels;

namespace Festival.App.ViewModels
{
    public class BandsViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly BandFacade _bandFacade;

        public ObservableCollection<BandListModel> Bands { get; set; } = new ObservableCollection<BandListModel>();

        public RelayCommand AddBandCommand { get; set; }
        public RelayCommand<Guid> EditBandCommand { get; set; }

        public BandsViewModel(IMediator mediator, INavigationService navigationService, BandFacade bandFacade)
        {
            Bands = new ObservableCollection<BandListModel>();

            _mediator = mediator;
            _navigationService = navigationService;
            _bandFacade = bandFacade;

            
            foreach (var band in _bandFacade.GetAllList())
            {
                Bands.Add(band);
            }

            AddBandCommand = new RelayCommand(Add);

            EditBandCommand = new RelayCommand<Guid>(Edit);
        }

        public void Edit(Guid bandId)
        {
            _navigationService.NavigateTo<AddEditBandView>();
            _mediator.Send(new SelectedMessage<BandListModel> { Id = bandId });
        }

        public void Add()
        {
            _navigationService.NavigateTo<AddEditBandView>();
            _mediator.Send(new NewMessage<BandListModel> { Id = Guid.Empty });
        }

    }
}
