using System;
using System.Collections.Generic;

namespace Festival.DAL.Entities
{
    public record BandMemberEntity : EntityBase
    {
        public string Name { get; set; }
        public string NickName { get; set; }
        public bool HeadMember { get; set; }
        public DateTime BirthDate { get; set; }
        public string ImageUrl { get; set; }

        private sealed class BandMemberEntityEqualityComparer : IEqualityComparer<BandMemberEntity>
        {
            public bool Equals(BandMemberEntity x, BandMemberEntity y)
            {
                if (ReferenceEquals(x, y)) return true;
                if (ReferenceEquals(x, null)) return false;
                if (ReferenceEquals(y, null)) return false;
                if (x.GetType() != y.GetType()) return false;
                return x.Name == y.Name && x.NickName == y.NickName && x.HeadMember == y.HeadMember && x.BirthDate == y.BirthDate && x.ImageUrl == y.ImageUrl;
            }

            public int GetHashCode(BandMemberEntity obj)
            {
                return HashCode.Combine(obj.Name, obj.NickName, obj.HeadMember, obj.BirthDate, obj.ImageUrl);
            }
        }

        public static IEqualityComparer<BandMemberEntity> BandMemberEntityComparer { get; } = new BandMemberEntityEqualityComparer();
    }
}