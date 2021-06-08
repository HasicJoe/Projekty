using System;
using System.Windows.Input;
using System.Collections.ObjectModel;
using Festival.App.Commands;
using Festival.App.Messages;
using Festival.App.Services;
using Festival.App.Services.MessageDialog;
using Festival.App.Views;
using Festival.BL.Facades;
using Festival.BL.Models.ListModels;
using Festival.BL.Models.DetailModels;
using Festival.App.Wrappers;

namespace Festival.App.ViewModels
{
    public class AddPerformanceViewModel : ViewModelBase
    {

        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly IMessageDialogService _messageDialogService;

        private readonly PerformanceFacade _performanceFacade;
        private readonly BandFacade _bandFacade;
        private readonly StageFacade _stageFacade;

        private DateTime _dateStart = new DateTime();
        private DateTime _dateEnd = new DateTime();

        public ObservableCollection<BandListModel> Bands { get; set; } = new ObservableCollection<BandListModel>();
        public ObservableCollection<StageListModel> Stages { get; set; } = new ObservableCollection<StageListModel>();
        public PerformanceWrapper Model { get; set; } = new PerformanceWrapper(new PerformanceDetailModel { Id = Guid.Empty });
        public DateTime DateStart
        {
            get { return _dateStart; }
            set
            {
                _dateStart = value;
                OnPropertyChanged("DateStart");
            }
        }
        public DateTime DateEnd 
        { 
            get { return _dateEnd; }
            set
            {
                _dateEnd = value;
                OnPropertyChanged("DateEnd");
            }
        }
        public ICommand SavePerformanceCommand { get; set; }
        public ICommand DeletePerformanceCommand { get; set; }
        private bool CanSave() =>
                Model != null &&
                Model.Band != null &&
                Model.Stage != null;

        private bool CanDelete() =>
            Model != null &&
            Model.Id != Guid.Empty;

        public void Load(SelectedMessage<PerformanceDetailModel> action)
        {
            if (Model.Id == Guid.Empty)
            {
                var fetched = _performanceFacade.GetById(action.Id);

                if (fetched != null)
                {
                    Model = fetched;
                    DateStart = Model.TimeStart.Date;
                    DateEnd = Model.TimeEnd.Date;
                }
                else
                {
                    New(new NewMessage<PerformanceDetailModel>());
                }
            }
        }

        public void New(NewMessage<PerformanceDetailModel> _)
        {
            Model.Band = null;
            Model.Stage = null;
            Model.TimeStart = DateTime.Now;
            Model.TimeEnd = DateTime.Now;
            DateStart = DateTime.Now.Date; 
            DateEnd = DateTime.Now.Date; 
        }

        public void Save()
        {
            Model.TimeStart = DateStart.Date.Add(Model.TimeStart.TimeOfDay);
            Model.TimeEnd   = DateEnd.Date.Add(Model.TimeEnd.TimeOfDay);

            if (Model.TimeStart > Model.TimeEnd)
            {
                _messageDialogService.Show("Error", "Performance cannot end before it starts!", MessageDialogButtonConfiguration.Ok, MessageDialogResult.Ok);
            }
            else
            {
                PerformanceDetailModel? savedModel = _performanceFacade.Save(Model);

                if (savedModel != null)
                {
                    _navigationService.NavigateTo<PerformancesListView>();
                }
                else
                {
                    System.Diagnostics.Debug.WriteLine("Collision of performances occured");
                    _messageDialogService.Show("Error", "Collision of performances occured! Please specify different date or time.", 
                        MessageDialogButtonConfiguration.Ok, MessageDialogResult.Ok);
                }
            }
        }

        public void Delete()
        {
            var confirmation = _messageDialogService.Show("Confirmation", "Are you sure, you want to delete this performance?", 
                MessageDialogButtonConfiguration.YesNo, MessageDialogResult.No);
            if(confirmation == MessageDialogResult.Yes)
            {
                try
                {
                    _performanceFacade.Delete(Model.Id);
                }
                catch
                {

                }
                finally
                {
                    _navigationService.NavigateTo<PerformancesListView>();
                }
            }
        }

        public AddPerformanceViewModel(IMediator mediator, INavigationService navigationService, IMessageDialogService messageDialogService, 
            StageFacade stageFacade, BandFacade bandFacade, PerformanceFacade performanceFacade)
        {
            _mediator = mediator;
            _navigationService = navigationService;
            _messageDialogService = messageDialogService;
            _performanceFacade = performanceFacade;
            _bandFacade = bandFacade;
            _stageFacade = stageFacade;

            SavePerformanceCommand = new RelayCommand(Save, CanSave);
            DeletePerformanceCommand = new RelayCommand(Delete, CanDelete);

            _mediator.Register<SelectedMessage<PerformanceDetailModel>>(Load);
            _mediator.Register<NewMessage<PerformanceDetailModel>>(New);
            
            foreach (var band in _bandFacade.GetAllList())
            {
                Bands.Add(band);
            }

            foreach(var stage in _stageFacade.GetAllList())
            {
                Stages.Add(stage);
            }
        }
    }
}
