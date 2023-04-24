using Festival.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace Festival.DAL
{
    public class FestivalDbContext : DbContext
    {
        public FestivalDbContext(DbContextOptions options) : base(options)
        {
        }

        public DbSet<BandMemberEntity> BandMembers { get; set; }
        public DbSet<BandEntity> Bands { get; set; }
        public DbSet<PerformanceEntity> Performances { get; set; }
        public DbSet<StageEntity> Stages { get; set; }
    }
}