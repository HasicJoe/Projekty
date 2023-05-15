using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace BudgetTracker.Models
{
    public class AppDbContext : DbContext
    {
        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options)
        {
        }
        protected override void OnModelCreating(ModelBuilder builder)
        {
            base.OnModelCreating(builder);
            // sprava Identity
            builder.Entity<IdentityUserClaim<string>>().ToTable("UserClaims");
            builder.Entity<IdentityUserRole<string>>()
                .HasKey(ur => new { ur.UserId, ur.RoleId });
            builder.Entity<IdentityRole>().ToTable("Roles");
            // odstranenie warningu pri aktualizacii databazy a praci s decimal
            builder.Entity<ExpenseCategoryModel>()
                .Property(e => e.Limit)
                .HasColumnType("decimal(18,4)");
            // kombinacia mena limitu a uzivatela musi byt unikatna
            builder.Entity<ExpenseCategoryModel>()
                .HasIndex(eCM => new { eCM.Name, eCM.Limit, eCM.UserId })
                .IsUnique();
            // odstranenie warningu pri aktualizacii databazy a praci s decimal
            builder.Entity<ExpenseModel>()
                .Property(e => e.Cost)
                .HasColumnType("decimal(18,4)");

            //vazba medzi kategoriami vydavkov a vydavkami
            builder.Entity<ExpenseModel>()
                .HasOne<ExpenseCategoryModel>()
                .WithMany(category => category.Expenses)
                .HasForeignKey(expense => expense.ExpenseCategoryId);

            builder.Entity<ExpenseCategoryModel>()
                .HasMany(category => category.Expenses)
                .WithOne(expense => expense.ExpenseCategory)
                .HasForeignKey(expense => expense.ExpenseCategoryId);
        }
        public DbSet<UserModel> Users { get; set; }
        public DbSet<IdentityUserRole<string>> UserRoles { get; set; }
        public DbSet<ExpenseCategoryModel> ExpenseCategories { get; set; }
        public DbSet<ExpenseModel> Expenses { get; set; }
    }
}
