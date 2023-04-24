using System;
using System.Collections.Generic;

namespace Festival.DAL.Entities
{
    public record PerformanceEntity : EntityBase
    {
        public Guid BandId { get; set; }
        public BandEntity Band { get; set; }
       
        public Guid StageId { get; set; }
        public StageEntity Stage { get; set; }

        public DateTime TimeStart { get; set; }
        public DateTime TimeEnd { get; set; }

        private sealed class PerformanceEntityEqualityComparer : IEqualityComparer<PerformanceEntity>
        {
            public bool Equals(PerformanceEntity x, PerformanceEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.TimeStart.Equals(y.TimeStart) &&
                       x.TimeEnd.Equals(y.TimeEnd) &&
                       StageEntity.WithoutPerformancesComparer.Equals(x.Stage, y.Stage) &&
                       BandEntity.WithoutPerformancesComparer.Equals(x.Band, y.Band);
            }

            public int GetHashCode(PerformanceEntity obj)
            {
                return HashCode.Combine(obj.BandId, obj.Band, obj.StageId, obj.Stage, obj.TimeStart, obj.TimeEnd);
            }
        }

        public static IEqualityComparer<PerformanceEntity> PerformanceEntityComparer { get; } = new PerformanceEntityEqualityComparer();
    }
}
