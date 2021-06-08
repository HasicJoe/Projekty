using System;
using System.Collections.ObjectModel;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.ListModels;
using Nemesis.Essentials.Design;

namespace Festival.App.ViewModels
{
    public class StageViewModel : ViewModelBase
    {

        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly StageFacade _stageFacade;

        public RelayCommand AddStageViewCommand { get; set; }
        public RelayCommand<Guid> EditStageViewCommand { get; set; }
        public ObservableCollection<StageListModel> Stages { get; set; }

        public StageViewModel(IMediator mediator, INavigationService navigationService, StageFacade stageFacade)

        {
            Stages = new ObservableCollection<StageListModel>();

            _mediator = mediator;
            _navigationService = navigationService;
            _stageFacade = stageFacade;

            foreach (var stage in _stageFacade.GetAllList())
            {
                if (stage.ImageUrl.IsNullOrEmpty())
                {
                    stage.ImageUrl = "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";
                }
                Stages.Add(stage);
            }
            
            AddStageViewCommand = new RelayCommand(Add);
            EditStageViewCommand = new RelayCommand<Guid>(Edit);
        }

        public void Edit(Guid stageId)
        {
            _navigationService.NavigateTo<AddEditStageView>();
            _mediator.Send(new SelectedMessage<StageListModel> { Id = stageId });
        }

        public void Add()
        {
            _navigationService.NavigateTo<AddEditStageView>();
            _mediator.Send(new NewMessage<StageListModel>{Id = Guid.Empty});
        }
    }
}
