using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows.Input;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Nemesis.Essentials.Design;

namespace Festival.App.ViewModels
{
    public class AddEditStageViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly StageFacade _stageFacade;
        private readonly PerformanceFacade _performanceFacade;

        public ObservableCollection<PerformanceListModel> Performers { get; set; }
        public ICommand SaveStageCommand { get; set; }
        public ICommand DeleteStageCommand { get; set; }

        public string StageName { get; set; }
        public string StageDesc { get; set; }
        public string StageImageUrl { get; set; }

        public AddEditStageViewModel(IMediator mediator, INavigationService navigationService, 
            StageFacade stageFacade, PerformanceFacade performanceFacade, BandFacade bandFacade)
        {
            _mediator = mediator;
            _navigationService = navigationService;
            _stageFacade = stageFacade;
            _performanceFacade = performanceFacade;

            Performers = new ObservableCollection<PerformanceListModel>();
            bandFacade.GetAllList();

            DeleteStageCommand = new RelayCommand(Delete, CanDelete);

            mediator.Register<SelectedMessage<StageListModel>>(SetStage);
            mediator.Register<NewMessage<StageListModel>>(NewStage);

        }

        private void NewStage(NewMessage<StageListModel> obj)
        {
            StageName = "";
            StageDesc = "";
            StageImageUrl = "";

            SaveStageCommand = new RelayCommand(Save, CanSaveEdit);
        }


        private void SetStage(SelectedMessage<StageListModel> obj)
        {
            var stage = _stageFacade.GetById(obj.Id);
            StageName = stage.Name;
            StageDesc = stage.StageDescription;
            StageImageUrl = stage.ImageUrl;

            if (StageImageUrl.IsNullOrEmpty())
            {
                StageImageUrl = "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";
            }
            SaveStageCommand = new RelayCommand(() => Edit(obj.Id), CanSaveEdit);
            foreach (var performance in stage.Performances)
            {
                Performers.Add(performance);
            }
        }

        private void Edit(Guid id)
        {
            var pDetailModels = 
                _stageFacade.GetById(id).Performances.Select(performance => _performanceFacade.GetById(performance.Id)).ToList();

            _stageFacade.Delete(id);

          
            var stage = new StageDetailModel()
            {
                Name = StageName,
                StageDescription = StageDesc,
                ImageUrl = StageImageUrl,
                Performances = new List<PerformanceListModel>()
            };


            var newStage = _stageFacade.Save(stage);

            foreach (var performance in pDetailModels)
            {
                performance.Stage = new StageListModel()
                {
                    Id = newStage.Id,
                    ImageUrl = newStage.ImageUrl,
                    Name = newStage.Name
                };
                performance.Id = new Guid();

                _performanceFacade.Save(performance);
            }
            
            _navigationService.NavigateTo<StageListView>();

        }

        private bool CanSaveEdit() => StageName != null && !Equals(StageName.Trim(), ""); // TODO: Check if name exists

        private void Save()
        {
            var stage = new StageDetailModel()
            {
                Name = StageName,
                StageDescription = StageDesc,
                ImageUrl = StageImageUrl,
                Performances = new List<PerformanceListModel>()
            };

            _stageFacade.Save(stage);

            _navigationService.NavigateTo<StageListView>();

        }

        private bool CanDelete()
        {
            var stageList = _stageFacade.GetAllList();
            return stageList != null && stageList.Any(i => i.Name == StageName);
        }

        private void Delete()
        {
            var stage = _stageFacade.GetAllList().First(i => i.Name == StageName);

            var stageId = stage.Id;
            _stageFacade.Delete(stageId);
            _navigationService.NavigateTo<StageListView>();
        }
    }
}
