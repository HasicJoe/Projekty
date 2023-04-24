using Microsoft.EntityFrameworkCore;

namespace Festival.DAL.Factories
{
    public interface INamedDbContextFactory<out TDbContext> where TDbContext : DbContext
    {
        TDbContext CreateDbContext();
    }
}