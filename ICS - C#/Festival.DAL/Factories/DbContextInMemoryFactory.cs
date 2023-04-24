using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore;

namespace Festival.DAL.Factories
{
    public class DbContextInMemoryFactory : INamedDbContextFactory<FestivalDbContext>
    {
        private readonly string _databaseName;

        public DbContextInMemoryFactory(string databaseName)
        {
            _databaseName = databaseName;
        }

        public FestivalDbContext CreateDbContext()
        {
            var contextOptionsBuilder = new DbContextOptionsBuilder<FestivalDbContext>();
            contextOptionsBuilder.UseInMemoryDatabase(_databaseName);

            return new FestivalDbContext(contextOptionsBuilder.Options);
        }

    }
}