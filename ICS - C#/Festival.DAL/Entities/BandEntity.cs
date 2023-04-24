using System;
using System.Collections.Generic;
using System.Linq;
using Nemesis.Essentials.Design;

namespace Festival.DAL.Entities
{
    public record BandEntity : EntityBase
    { 
        public string Name { get; set; }
        public string ImageUrl { get; set; }
        public string Genre { get; set; }
        public string CountryOfOrigin { get; set; }
        public string BandDescription { get; set; }
        public string ProgramDescription { get; set; }

        public ICollection<BandMemberEntity> BandMembers { get; set; } = new ValueCollection<BandMemberEntity>(BandMemberEntity.BandMemberEntityComparer);
        public ICollection<PerformanceEntity> Performances { get; set; } = new ValueCollection<PerformanceEntity>(PerformanceEntity.PerformanceEntityComparer);

        private sealed class BandEntityEqualityComparer : IEqualityComparer<BandEntity>
        {
            public bool Equals(BandEntity x, BandEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.Name == y.Name &&
                       x.ImageUrl == y.ImageUrl &&
                       x.Genre == y.Genre &&
                       x.CountryOfOrigin == y.CountryOfOrigin &&
                       x.BandDescription == y.BandDescription &&
                       x.ProgramDescription == y.ProgramDescription &&
                       x.BandMembers.OrderBy(member => member.Id).SequenceEqual(
                           y.BandMembers.OrderBy(member => member.Id), BandMemberEntity.BandMemberEntityComparer) &&
                       x.Performances.OrderBy(perf => perf.Id).SequenceEqual(y.Performances.OrderBy(perf => perf.Id),
                                PerformanceEntity.PerformanceEntityComparer);
            }

            public int GetHashCode(BandEntity obj)
            {
                return HashCode.Combine(obj.Name, obj.ImageUrl, obj.Genre, obj.CountryOfOrigin, obj.BandDescription, obj.ProgramDescription, obj.BandMembers, obj.Performances);
            }
        }

        public static IEqualityComparer<BandEntity> BandEntityComparer { get; } = new BandEntityEqualityComparer();

        private sealed class WithoutPerformancesEqualityComparer : IEqualityComparer<BandEntity>
        {
            public bool Equals(BandEntity x, BandEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.Name == y.Name &&
                       x.ImageUrl == y.ImageUrl &&
                       x.Genre == y.Genre &&
                       x.CountryOfOrigin == y.CountryOfOrigin &&
                       x.BandDescription == y.BandDescription &&
                       x.ProgramDescription == y.ProgramDescription &&
                       x.BandMembers.OrderBy(member => member.Id).SequenceEqual(
                           y.BandMembers.OrderBy(member => member.Id), BandMemberEntity.BandMemberEntityComparer);
            }

            public int GetHashCode(BandEntity obj)
            {
                return HashCode.Combine(obj.Name, obj.ImageUrl, obj.Genre, obj.CountryOfOrigin, obj.BandDescription, obj.ProgramDescription, obj.BandMembers);
            }
        }

        public static IEqualityComparer<BandEntity> WithoutPerformancesComparer { get; } = 
            new WithoutPerformancesEqualityComparer();
    }
}
