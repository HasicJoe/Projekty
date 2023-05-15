using BudgetTracker.Models;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace BudgetTracker.Controllers
{
    public class ExpenseController : Controller
    {
        private readonly UserManager<UserModel> _userManager;
        private readonly AppDbContext _appDbContext;

        public ExpenseController(UserManager<UserModel> userManager, AppDbContext appDbContext)
        {
            _userManager = userManager;
            _appDbContext = appDbContext;
        }


        [HttpGet]
        public IActionResult Index()
        {
            // presmerovanie neprihlaseneho uzivatela
            if (User.Identity != null && !User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            return View();
        }


        [HttpPost]
        public async Task<IActionResult> AddExpense(ExpenseModel model)
        {
            if (User.Identity != null && !User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            if (ModelState.IsValid)
            {
                var user = await _userManager.GetUserAsync(User);
                model.UserId = user!.Id;
                if (string.IsNullOrEmpty(model.ExpenseNote))
                {
                    model.ExpenseNote = string.Empty;
                }
                var categoryExists = await _appDbContext.ExpenseCategories.AnyAsync(ec => ec.Id == model.ExpenseCategoryId);
                if (!categoryExists)
                {
                    TempData["Message"] = "Expense category not found.";
                    return RedirectToAction("Index", "Expense");
                }
                try
                {
                    _appDbContext.Expenses.Add(model);
                    await _appDbContext.SaveChangesAsync();
                    var expenseCategory = await _appDbContext.ExpenseCategories.FindAsync(model.ExpenseCategoryId);
                    expenseCategory!.Expenses!.Add(model);
                    return Ok();
                }
                catch (Exception ex)
                {
                    TempData["Message"] = $"Unable to add new expense. Error: {ex}";
                    return RedirectToAction("Index", "Expense");
                }
            }
            return View("Index");
        }


        [HttpGet]
        public async Task<IActionResult> GetExpenses()
        {
            if (User.Identity != null && !User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            try
            {
                var user = await _userManager.GetUserAsync(User).ConfigureAwait(false);
                var expenses = await _appDbContext.Expenses
                    .Where(ec => ec.UserId == user!.Id)
                    .ToListAsync<ExpenseModel>().ConfigureAwait(false);
                var extendedExpenses = new List<ExtendedExpenseModel>();
                foreach (var expense in expenses)
                {
                    var extendedExpense = new ExtendedExpenseModel()
                    {
                        Date = expense.Date,
                        ExpenseCategoryId = expense.ExpenseCategoryId,
                        ExpenseCategoryName = _appDbContext.ExpenseCategories.First(ec => ec.Id == expense.ExpenseCategoryId).Name,
                        Cost = expense.Cost,
                        ExpenseNote = expense.ExpenseNote,
                        Id = expense.Id,
                        UserId = expense.UserId
                    };
                    extendedExpenses.Add(extendedExpense);
                }
                return Json(extendedExpenses);
            }
            catch (Exception ex)
            {
                return BadRequest("Unable to fetch user expense categories. Error: " + ex.Message);
            }
        }


        [HttpGet]
        public async Task<IActionResult> GetExpense(int id)
        {
            try
            {
                var expense = await _appDbContext.Expenses.FindAsync(id);
                var extendedExpense = new ExtendedExpenseModel()
                {
                    Cost = expense!.Cost,
                    Date = expense.Date,
                    ExpenseCategoryId = expense.ExpenseCategoryId,
                    ExpenseCategoryName = _appDbContext.ExpenseCategories
                        .First(ec => ec.Id == expense.ExpenseCategoryId).Name,
                    ExpenseNote = expense.ExpenseNote,
                    Id = expense.Id,
                    UserId = expense.UserId
                };
                return Json(extendedExpense);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to fetch selected expense category with id = {id}. Error: " + ex.Message);
            }
        }


        [HttpPost]
        public async Task<IActionResult> UpdateExpense()
        {
            try
            {
                var expense = await _appDbContext.Expenses.FindAsync(int.Parse(Request.Form["edit-id"]!));
                if (expense == null) return NotFound();
                //update
                expense.Cost = Convert.ToDecimal(Request.Form["edit-cost"]);
                expense.Date = DateTime.Parse(Request.Form["edit-date"]!);
                expense.ExpenseNote = Request.Form["edit-note"];
                await _appDbContext.SaveChangesAsync();
                return new StatusCodeResult(200);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to update existing expense. Error: " + ex.Message);
            }
        }


        [HttpGet]
        public async Task<IActionResult> DeleteExpense(int id)
        {
            try
            {
                var expense = await _appDbContext.Expenses.FindAsync(id);
                if (expense == null) return NotFound();
                _appDbContext.Expenses.Remove(expense);
                await _appDbContext.SaveChangesAsync();
                return Ok();
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to delete existing expense. Error: " + ex.Message);
            }
        }
    }
}
