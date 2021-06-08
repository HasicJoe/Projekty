using System;
using Festival.BL.Models.DetailModels;

namespace Festival.App.Wrappers
{
    public class BandMemberWrapper : ModelWrapper<BandMemberDetailModel>
    {
        public BandMemberWrapper(BandMemberDetailModel model) : base(model)
        {

        }

        public string Name
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public string NickName
        {
            get => GetValue<string>();
            set => SetValue(value);
        }
        public bool HeadMember
        {
            get => GetValue<bool>();
            set => SetValue(value);
        }
        public DateTime BirthDate
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }
        public string ImageUrl
        {
            get => GetValue<string>();
            set => SetValue(value);
        }

        public static implicit operator BandMemberWrapper(BandMemberDetailModel detailModel) => new BandMemberWrapper(detailModel);
        public static implicit operator BandMemberDetailModel(BandMemberWrapper wrapper) => wrapper.Model;

    }
}
