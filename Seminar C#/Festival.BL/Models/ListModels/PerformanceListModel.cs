using System;
using System.Collections.ObjectModel;

namespace Festival.BL.Models.ListModels
{
    public record PerformanceListModel : IModel
    {
        public Guid Id { get; set; }
        public Guid BandId { get; set; }
        public Guid StageId { get; set; }
        public DateTime TimeStart { get; set; }
        public DateTime TimeEnd { get; set; }
        
        // SfScheduler dependencies
        public string BandName { get; set; }
        public ObservableCollection<object> ResourceIdCollection { get; set; }
    }
}
