using System;
using System.Collections.Generic;
using System.Linq;
using Nemesis.Essentials.Design;

namespace Festival.DAL.Entities
{
    public record StageEntity : EntityBase
    { 
        public string Name { get; set; }
        public string StageDescription { get; set; }
        public string ImageUrl { get; set; }

        public ICollection<PerformanceEntity> Performances { get; set; } =
            new ValueCollection<PerformanceEntity>(PerformanceEntity.PerformanceEntityComparer);

        private sealed class NameStageDescriptionEqualityComparer : IEqualityComparer<StageEntity>
        {
            public bool Equals(StageEntity x, StageEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.Name == y.Name && 
                       x.ImageUrl == y.ImageUrl &&
                       x.StageDescription == y.StageDescription &&
                       x.Performances.OrderBy(perf => perf.Id).SequenceEqual(y.Performances.OrderBy(perf => perf.Id));
            }

            public int GetHashCode(StageEntity obj)
            {
                return HashCode.Combine(obj.Name, obj.StageDescription, obj.ImageUrl,obj.Performances);
            }
        }

        public static IEqualityComparer<StageEntity> NameStageDescriptionComparer { get; } = new NameStageDescriptionEqualityComparer();

        private sealed class WithoutPerformancesEqualityComparer : IEqualityComparer<StageEntity>
        {
            public bool Equals(StageEntity x, StageEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.Name == y.Name &&
                       x.ImageUrl == y.ImageUrl &&
                       x.StageDescription == y.StageDescription;
            }

            public int GetHashCode(StageEntity obj)
            {
                return HashCode.Combine(obj.Name, obj.StageDescription, obj.ImageUrl, obj.Performances);
            }
        }

        public static IEqualityComparer<StageEntity> WithoutPerformancesComparer { get; } = new WithoutPerformancesEqualityComparer();
    }
}
