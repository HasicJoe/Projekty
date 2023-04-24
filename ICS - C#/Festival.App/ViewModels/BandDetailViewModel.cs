using System;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.DetailModels;
using Nemesis.Essentials.Design;

namespace Festival.App.ViewModels
{
    public class BandDetailViewModel : ViewModelBase
    {
        public ObservableCollection<string> StageNames { get; set; }
        public Guid BandId { get; set; }
        public ICommand ReturnCommand { get; set; }
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly BandFacade _bandFacade;
        private readonly StageFacade _stageFacade;
        public BandDetailModel Band { get; set; }
        public BandDetailViewModel(IMediator mediator, INavigationService navigationService,
            BandFacade bandFacade,StageFacade stageFacade)
        {
            StageNames = new ObservableCollection<string>();
            _mediator = mediator;
            _mediator.Register<SelectedMessage<BandDetailModel>>(Loaded);
            _navigationService = navigationService;
            _bandFacade = bandFacade;
            _stageFacade = stageFacade;
            ReturnCommand = new RelayCommand(BackToProgramView, () => true);
        }

        public void NullFieldsToDefaultValues()
        {
            if (Band.ImageUrl.IsNullOrEmpty())
            {
                Band.ImageUrl = "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";
            }
            if (Band.ProgramDescription.IsNullOrEmpty())
            {
                Band.ProgramDescription = "No program description.";
            }
            if (Band.BandDescription.IsNullOrEmpty())
            {
                Band.BandDescription = "No band description.";
            }
            if (Band.Genre.IsNullOrEmpty())
            {
                Band.Genre = "Unknown";
            }
            if (Band.CountryOfOrigin.IsNullOrEmpty())
            {
                Band.CountryOfOrigin = "Unknown";
            }
        }

        public void LoadNames()
        {
            foreach (var performance in Band.Performances)
            {
                var stage = _stageFacade.GetById(performance.StageId);
                if(stage.Name.IsNullOrEmpty())
                {
                    StageNames.Add("Unknown Stage");
                }
                else
                {
                    StageNames.Add(stage.Name);
                }
            }
        }

        private void Loaded(SelectedMessage<BandDetailModel> action)
        {
            Band = _bandFacade.GetById(action.Id);
            LoadNames();
            NullFieldsToDefaultValues();
        }

        private void BackToProgramView()
        {
            _navigationService.NavigateTo<ProgramListView>();
        }
    }
}
