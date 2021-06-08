using System;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;

namespace Festival.App.Wrappers
{
    public class PerformanceWrapper : ModelWrapper<PerformanceDetailModel>
    {
        public PerformanceWrapper(PerformanceDetailModel model) : base(model)
        {
        }

        public StageListModel Stage
        {
            get => GetValue<StageListModel>();
            set => SetValue(value);
        }

        public BandListModel Band
        {
            get => GetValue<BandListModel>();
            set => SetValue(value);
        }

        public DateTime TimeStart
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }

        public DateTime TimeEnd
        {
            get => GetValue<DateTime>();
            set => SetValue(value);
        }

        public static implicit operator PerformanceWrapper(PerformanceDetailModel detailModel) => new PerformanceWrapper(detailModel);
        public static implicit operator PerformanceDetailModel(PerformanceWrapper wrapper) => wrapper.Model;
    }
}
