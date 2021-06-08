using Festival.BL.Models.ListModels;
using System.Collections.Generic;

namespace Festival.BL.Models.DetailModels
{
    public record BandDetailModel : ModelBase
    {
        public string Name { get; set; }
        public string ImageUrl { get; set; }
        public string Genre { get; set; }
        public string CountryOfOrigin { get; set; }
        public string BandDescription { get; set; }
        public string ProgramDescription { get; set; }

        public ICollection<BandMemberListModel> BandMembers { get; set; }
        public ICollection<PerformanceListModel> Performances { get; set; }
    }
}
