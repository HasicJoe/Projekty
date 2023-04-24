using Festival.BL.Models.ListModels;
using System;

namespace Festival.BL.Models.DetailModels
{
    public record PerformanceDetailModel : ModelBase
    {
        public StageListModel Stage { get; set; }
        public BandListModel Band { get; set; }

        public DateTime TimeStart { get; set; }
        public DateTime TimeEnd { get; set; }
    }
}
