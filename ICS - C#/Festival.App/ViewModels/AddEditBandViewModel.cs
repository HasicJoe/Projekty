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
using MaterialDesignThemes.Wpf;
using Festival.App.Wrappers;
using Festival.App.Services.MessageDialog;
using Nemesis.Essentials.Design;

namespace Festival.App.ViewModels
{
    public class AddEditBandViewModel : ViewModelBase
    {
        private readonly IMediator _mediator;
        private readonly INavigationService _navigationService;
        private readonly IMessageDialogService _messageDialogService;
        private readonly BandFacade _bandFacade;
        private readonly BandMemberFacade _memberFacade;
        private readonly PerformanceFacade _performanceFacade;

        public ICommand SaveBandCommand { get; set; }
        public ICommand DeleteBandCommand { get; set; }

        public ICommand AddBandMemberCommand { get; set; }
        public ICommand DeleteBandMemberCommand { get; set; }

        public ICommand NewBandMemberCommand { get; set; }
        public ICommand EditBandMemberCommand { get; set; }

        public string BandName { get; set; }
        public string BandDesc { get; set; }
        public string BandImageUrl { get; set; }
        public string Genre { get; set; }
        public string CountryOfOrigin { get; set; }
        public string ProgramDescription { get; set; }

        public BandMemberWrapper Member { get; set; } = new(new BandMemberDetailModel());

        public ObservableCollection<BandMemberListModel> BandMembers { get; set; } =
            new();

        public AddEditBandViewModel(IMediator mediator, INavigationService navigationService, BandFacade bandFacade,
            BandMemberFacade memberFacade, PerformanceFacade performanceFacade,
            IMessageDialogService messageDialogService)
        {
            _mediator = mediator;
            _navigationService = navigationService;
            _bandFacade = bandFacade;
            _memberFacade = memberFacade;
            _performanceFacade = performanceFacade;
            _messageDialogService = messageDialogService;

            DeleteBandCommand = new RelayCommand(Delete, CanDelete);

            mediator.Register<NewMessage<BandListModel>>(NewBand);
            mediator.Register<SelectedMessage<BandListModel>>(SetBand);

            AddBandMemberCommand = new RelayCommand(AddMember);

            DeleteBandMemberCommand = new RelayCommand<Guid>(DeleteMember);

            NewBandMemberCommand = new RelayCommand<DialogHost>(NewBandMember);
            EditBandMemberCommand = new RelayCommand<object>(EditBandMember);
        }

        private void NewBandMember(DialogHost parameter)
        {
            Member.Id = Guid.Empty;
            Member.Name = "";
            Member.NickName = "";
            Member.HeadMember = false;
            Member.BirthDate = DateTime.Now;
            Member.ImageUrl = "";
            DialogHost.OpenDialogCommand.Execute(null, parameter);
        }

        private void EditBandMember(object parameter)
        {
            var parameterList = (List<object>) parameter;

            var model = _memberFacade.GetById((Guid) parameterList[0]);
            if (model != null)
            {
                Member.Id = model.Id;
                Member.Name = model.Name;
                Member.NickName = model.NickName;
                Member.HeadMember = model.HeadMember;
                Member.BirthDate = model.BirthDate;
                Member.ImageUrl = model.ImageUrl;
            }

            DialogHost.OpenDialogCommand.Execute(null, (DialogHost) parameterList[1]);
        }

        private void DeleteMember(Guid id)
        {
            var confirmation = _messageDialogService.Show("Confirmation",
                "Are you sure, you want to delete this band member?", MessageDialogButtonConfiguration.YesNo,
                MessageDialogResult.No);
            if (confirmation == MessageDialogResult.Yes)
                try
                {
                    _memberFacade.Delete(id);
                    var member = BandMembers.First(m => m.Id == id);
                    if (member != null) BandMembers.Remove(member);
                }
                catch
                {
                }
        }

        private void AddMember()
        {
            var detailModel = new BandMemberDetailModel()
            {
                Id = Member.Id,
                Name = Member.Name,
                BirthDate = Member.BirthDate,
                ImageUrl = Member.ImageUrl,
                NickName = Member.NickName,
                HeadMember = Member.HeadMember
            };

            if (detailModel.ImageUrl.IsNullOrEmpty())
                detailModel.ImageUrl =
                    "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";

            detailModel = _memberFacade.Save(detailModel);

            var listModel = new BandMemberListModel()
            {
                Id = detailModel.Id,
                Name = detailModel.Name,
                ImageUrl = detailModel.ImageUrl
            };

            if (Member.Id == Guid.Empty)
            {
                BandMembers.Add(listModel);
            }
            else
            {
                var member = BandMembers.Single(member => member.Id == Member.Id);
                BandMembers.Remove(member);
                member.Name = Member.Name;
                member.ImageUrl = Member.ImageUrl;
                BandMembers.Add(member);
            }


            DialogHost.CloseDialogCommand.Execute(null, null);
        }


        private void NewBand(NewMessage<BandListModel> obj)
        {
            BandName = "";
            BandDesc = "";
            BandImageUrl = "";
            Genre = "";
            CountryOfOrigin = "";
            ProgramDescription = "";
            BandMembers = new ObservableCollection<BandMemberListModel>();

            SaveBandCommand = new RelayCommand(Save, CanSave);
        }

        private void SetBand(SelectedMessage<BandListModel> obj)
        {
            var band = _bandFacade.GetById(obj.Id);
            BandName = band.Name;
            BandDesc = band.BandDescription;
            BandImageUrl = band.ImageUrl;
            Genre = band.Genre;
            CountryOfOrigin = band.CountryOfOrigin;
            ProgramDescription = band.ProgramDescription;
            BandMembers = new ObservableCollection<BandMemberListModel>();
            foreach (var member in band.BandMembers) BandMembers.Add(member);

            SaveBandCommand = new RelayCommand(() => Edit(obj.Id), CanSave);
        }

        private void Edit(Guid id)
        {
            var pDetailModels =
                _bandFacade.GetById(id).Performances.Select(performance => _performanceFacade.GetById(performance.Id))
                    .ToList();

            _bandFacade.Delete(id);

            var band = new BandDetailModel()
            {
                Name = BandName,
                BandDescription = BandDesc,
                ImageUrl = BandImageUrl,
                Genre = Genre,
                CountryOfOrigin = CountryOfOrigin,
                ProgramDescription = ProgramDescription,
                BandMembers = BandMembers,
                Performances = new List<PerformanceListModel>()
            };

            if (band.ImageUrl.IsNullOrEmpty())
                band.ImageUrl = "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";

            var newBand = _bandFacade.Save(band);

            foreach (var performance in pDetailModels)
            {
                performance.Band = new BandListModel()
                {
                    Id = newBand.Id,
                    ImageUrl = newBand.ImageUrl,
                    Name = newBand.Name
                };
                performance.Id = new Guid();

                _performanceFacade.Save(performance);
            }

            _navigationService.NavigateTo<BandsListView>();
        }

        private bool CanSave()
        {
            return BandName != null && !Equals(BandName.Trim(), "");
        }

        private void Save()
        {
            var band = new BandDetailModel()
            {
                Name = BandName,
                BandDescription = BandDesc,
                ImageUrl = BandImageUrl,
                Genre = Genre,
                CountryOfOrigin = CountryOfOrigin,
                ProgramDescription = ProgramDescription,
                BandMembers = BandMembers,
                Performances = new List<PerformanceListModel>()
            };
            if (band.ImageUrl.IsNullOrEmpty())
                band.ImageUrl = "https://cdn.iconscout.com/icon/premium/png-512-thumb/page-not-found-1-503918.png";
            _bandFacade.Save(band);

            _navigationService.NavigateTo<BandsListView>();
        }

        private bool CanDelete()
        {
            var bandList = _bandFacade.GetAllList();
            return bandList != null && bandList.Any(i => i.Name == BandName);
        }

        private void Delete()
        {
            var band = _bandFacade.GetAllList().Single(i => i.Name == BandName);
            var bandId = band.Id;
            _bandFacade.Delete(bandId);
            _navigationService.NavigateTo<BandsListView>();
        }
    }
}