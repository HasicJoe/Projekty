using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace BudgetTracker.Models
{
    public class ExpenseCategoryModel
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }
        [Required]
        public string Name { get; set; }
        [Required]
        public decimal Limit { get; set; }
        public string? UserId { get; set; }
        public virtual ICollection<ExpenseModel>? Expenses { get; set; }
    }
}
