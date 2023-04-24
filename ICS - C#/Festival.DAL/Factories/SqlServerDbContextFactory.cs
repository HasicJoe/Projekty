
using Microsoft.EntityFrameworkCore;

namespace Festival.DAL.Factories
{
    public class SqlServerDbContextFactory : INamedDbContextFactory<FestivalDbContext>
    {

        private readonly string _connectionString;

        public SqlServerDbContextFactory(string connectionString)
        {
            _connectionString = connectionString;
        }

        public FestivalDbContext CreateDbContext()
        {
            var optionsBuilder = new DbContextOptionsBuilder<FestivalDbContext>();
            optionsBuilder.UseSqlServer(_connectionString);
            return new FestivalDbContext(optionsBuilder.Options);
        }
    }
}
