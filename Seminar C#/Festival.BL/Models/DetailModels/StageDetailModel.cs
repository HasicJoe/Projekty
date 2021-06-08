using Festival.BL.Models.ListModels;
using System.Collections.Generic;

namespace Festival.BL.Models.DetailModels
{
    public record StageDetailModel : ModelBase
    {
        public string Name { get; set; }
        public string StageDescription { get; set; }
        public string ImageUrl { get; set; }
        
        public ICollection<PerformanceListModel> Performances { get; set; }
    }
}
