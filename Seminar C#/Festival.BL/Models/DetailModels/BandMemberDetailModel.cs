using System;

namespace Festival.BL.Models.DetailModels
{
    public record BandMemberDetailModel : ModelBase
    {
        public string Name { get; set; }
        public string NickName { get; set; }
        public bool HeadMember { get; set; }
        public DateTime BirthDate { get; set; }
        public string ImageUrl { get; set; }
    }
}
