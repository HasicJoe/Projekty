
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace BudgetTracker.Models
{
    public class ExpenseModel
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public int Id { get; set; }
        [Required]
        public DateTime Date { get; set; }
        [Required]
        public decimal Cost { get; set; }
        [Required]
        public int ExpenseCategoryId { get; set; }
        [ForeignKey("ExpenseCategoryId")]
        public virtual ExpenseCategoryModel? ExpenseCategory { get; set; }
        public string? UserId { get; set; }
        public string? ExpenseNote { get; set; }
    }
}
