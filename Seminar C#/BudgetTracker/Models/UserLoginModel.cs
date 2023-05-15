using System.ComponentModel.DataAnnotations;

namespace BudgetTracker.Models
{
    public class UserLoginModel
    {
        [Required]
        public string UserName { get; set; }

        [Required]
        [DataType(DataType.Password)]
        public string Password { get; set; }
    }
}