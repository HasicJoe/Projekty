using BudgetTracker.Models;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.TestUtilities;

namespace BudgetTracker.Controllers
{
    public class ExpenseCategoryController : Controller
    {
        private readonly UserManager<UserModel> _userManager;
        private readonly AppDbContext _appDbContext;
        public ExpenseCategoryController(
            UserManager<UserModel> userManager, AppDbContext appDbContext)
        {
            _userManager = userManager;
            _appDbContext = appDbContext;
        }

        [HttpGet]
        public IActionResult Category()
        {
            // presmerovanie neprihlaseneho uzivatela
            if (User.Identity != null && !User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> AddCategory(ExpenseCategoryModel model)
        {
            if (!User.Identity!.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }

            if (!ModelState.IsValid)
            {
                return View("Category");
            }

            var user = await _userManager.GetUserAsync(User);
            if (user == null)
            {
                return BadRequest("Logged user not found.");
            }

            model.UserId = user.Id;
            var existingCategory = await _appDbContext.ExpenseCategories
                .FirstOrDefaultAsync(c => c.Name == model.Name && c.UserId == user.Id);

            if (existingCategory != null)
            {
                return BadRequest($"An expense category with the name '{model.Name}' already exists.");
            }

            try
            {
                _appDbContext.ExpenseCategories.Add(model);
                await _appDbContext.SaveChangesAsync();
                return Ok();
            }
            catch (DbUpdateException ex)
            {
                return BadRequest($"Unable to add new expense category. Error: {ex.Message}");
            }
        }

        [HttpGet]
        public async Task<IActionResult> GetUserExpensesCategory()
        {
            try
            {
                var user = await _userManager.GetUserAsync(User).ConfigureAwait(false);
                var categories = await _appDbContext.ExpenseCategories
                    .Where(ec => ec.UserId == user!.Id)
                    .ToListAsync<ExpenseCategoryModel>().ConfigureAwait(false);
                return Json(categories);
            }
            catch (Exception ex)
            {
                return BadRequest("Unable to fetch user expense categories. Error: " + ex.Message);
            }
        }

        [HttpGet]
        public async Task<IActionResult> GetExpenseCategory(int id)
        {
            try
            {
                var category = await _appDbContext.ExpenseCategories.FindAsync(id);
                return Json(category);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to fetch selected expense category with id = {id}. Error: " + ex.Message);
            }
        }

        [HttpPost]
        public async Task<IActionResult> UpdateExpenseCategory()
        {
            try
            {
                var expenseCategory = await _appDbContext.ExpenseCategories
                    .FindAsync(int.Parse(Request.Form["edit-id"]!));
                if (expenseCategory == null) return NotFound();
                // aktualizacia atributov a ulozenie zmien
                expenseCategory.Name = Request.Form["edit-name"]!;
                expenseCategory.Limit = Convert.ToDecimal(Request.Form["edit-limit"]);
                await _appDbContext.SaveChangesAsync();
                return new StatusCodeResult(200);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to update existing expense category. Error: " + ex.Message);
            }
        }

        [HttpGet]
        public async Task<IActionResult> DeleteExpenseCategory(int id)
        {
            try
            {
                var expenseCategory = await _appDbContext.ExpenseCategories.FindAsync(id);
                if(expenseCategory == null) return NotFound();

                //vymazanie vydavkov pred vymazanim expense category
                var expensesToDelete = await _appDbContext.Expenses
                    .Where(e => e.ExpenseCategoryId == id)
                    .ToListAsync();
                foreach (var expense in expensesToDelete)
                {
                    _appDbContext.Expenses.Remove(expense);
                }
                // vymazanie kategorie vydavkov
                _appDbContext.ExpenseCategories.Remove(expenseCategory);
                await _appDbContext.SaveChangesAsync();
                return Ok();
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to delete existing expense category. Error: " + ex.Message);
            }
        }
    }
}
