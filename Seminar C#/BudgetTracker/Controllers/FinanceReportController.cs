using BudgetTracker.Models;
using Google.Protobuf.Collections;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;


namespace BudgetTracker.Controllers
{
    public class FinanceReportController : Controller
    {
        private readonly UserManager<UserModel> _userManager;
        private readonly AppDbContext _appDbContext;

        public FinanceReportController(
            UserManager<UserModel> userManager, AppDbContext appDbContext)
        {
            _userManager = userManager;
            _appDbContext = appDbContext;
        }

        [HttpGet]
        public IActionResult Index()
        {
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> MakeReport()
        {
            if (User.Identity != null && !User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            if (ModelState.IsValid)
            {
                try
                {
                    var year = int.Parse(Request.Form["year"]!);
                    var month = int.Parse(Request.Form["month"]!);
                    var user = await _userManager.GetUserAsync(User);
                    var userCategories = _appDbContext.ExpenseCategories
                        .Where(ec => ec.UserId == user!.Id)
                        .ToList();

                    var categories = new List<string>();
                    var expenses = new List<decimal>();
                    var limits = new List<decimal>();
                    foreach (var category in userCategories)
                    {
                        categories.Add(category.Name);
                        if (_appDbContext.Expenses.Count(e => e.ExpenseCategoryId == category.Id) == 0)
                        {
                            expenses.Add(0);
                        }
                        else
                        {
                            expenses.Add(
                                _appDbContext.Expenses
                                    .Where(e => e.ExpenseCategoryId == category.Id && e.Date.Year == year &&
                                                e.Date.Month == month)
                                    .Sum(e => e.Cost)
                            );
                        }

                        limits.Add(category.Limit);
                    }
                    var data = new
                    {
                        categories,
                        expenses,
                        limits
                    };
                    return Json(data);
                }
                catch (Exception ex)
                {
                    return BadRequest($"Unable to make financial report. Error: {ex.Message}");
                }
            }
            return BadRequest("Unable to make financial report. Invalid model state.");
        }

        [HttpPost]
        public async Task<IActionResult> MakeTotalSpendingReport()
        {
            try
            {
                // total budget consumption across all categories
                var year = int.Parse(Request.Form["year"]!);
                var month = int.Parse(Request.Form["month"]!);
                var user = await _userManager.GetUserAsync(User);
                var userCategories = _appDbContext.ExpenseCategories
                    .Where(ec => ec.UserId == user!.Id)
                    .ToList();
                decimal totalLimit = userCategories.Sum(c => c.Limit);
                decimal totalExpenses = _appDbContext.Expenses
                    .Where(e => e.Date.Year == year && e.Date.Month == month &&
                                userCategories.Contains(e.ExpenseCategory!))
                    .Sum(e => e.Cost);
                var data = new
                {
                    totalLimit,
                    totalExpenses
                };
                return Json(data);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to make total finance report. Error: {ex.Message}");
            }
        }

        [HttpPost]
        public async Task<IActionResult> MakeDayByDayReport()
        {
            try
            {
                var year = int.Parse(Request.Form["year"]!);
                var month = int.Parse(Request.Form["month"]!);
                var user = await _userManager.GetUserAsync(User);
                var userCategories = _appDbContext.ExpenseCategories
                    .Where(ec => ec.UserId == user!.Id)
                    .ToList();
                var categories = new List<string>();
                var expenses = new List<List<decimal>>();
                var limits = new List<decimal>();
                var dates = Enumerable.Range(1, DateTime.DaysInMonth(year, month))
                    .Select(day => new DateTime(year, month, day))
                    .ToList();
                foreach (var category in userCategories)
                {
                    categories.Add(category.Name);
                    var dailyExpenses = new List<decimal>();
                    for (var index = 0; index < dates.Count; index++)
                    {
                        dailyExpenses.Add(0);
                    }
                    if (_appDbContext.Expenses.Count(e => e.ExpenseCategoryId == category.Id) > 0)
                    {
                        for (var index = 1; index <= dates.Count; index++)
                        {
                            dailyExpenses[index - 1] = _appDbContext.Expenses
                                .Where(e => e.ExpenseCategoryId == category.Id && e.Date.Year == year && e.Date.Month == month && e.Date.Day == index)
                                .Sum(e => e.Cost);
                        }
                    }
                    expenses.Add(dailyExpenses); 
                    limits.Add(category.Limit);
                }
                var data = new
                {
                    categories,
                    expenses,
                    limits,
                    dates
                };
                return Json(data);
            }
            catch (Exception ex)
            {
                return BadRequest($"Unable to make financial day-by-day report. Error: {ex.Message}");
            }
        }
    }
}