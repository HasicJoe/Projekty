using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;

namespace Festival.DAL.Factories
{
    public class DesignTimeDbContextFactory : IDesignTimeDbContextFactory<FestivalDbContext>
    {
        public FestivalDbContext CreateDbContext(string[] args)
        {
            var builder = new DbContextOptionsBuilder<FestivalDbContext>();
            builder.UseSqlServer(@"
                            Server = (localdb)\MSSQLLocalDB;
                            Initial Catalog = Festival;
                            MultipleActiveResultSets = True;
                            Integrated Security = true");

            return new FestivalDbContext(builder.Options);
        }
    }
}
