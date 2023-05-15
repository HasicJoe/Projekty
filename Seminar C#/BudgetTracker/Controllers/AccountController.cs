using BudgetTracker.Models;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;

namespace BudgetTracker.Controllers
{
    public class AccountController : Controller
    {
        private readonly UserManager<UserModel> _userManager;
        private readonly SignInManager<UserModel> _signInManager;

        public AccountController(
            UserManager<UserModel> userManager,
            SignInManager<UserModel> signInManager)
        {
            _userManager = userManager;
            _signInManager = signInManager;
        }

        [HttpGet]
        public IActionResult Register()
        {
            if (User.Identity != null && User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> Register(UserRegistrationModel model)
        {
            if (ModelState.IsValid)
            {
                var user = new UserModel { UserName = model.UserName, Email = model.Email };
                var result = await _userManager.CreateAsync(user, model.Password);
                if (result.Succeeded)
                {
                    TempData["Message"] = $"Registration was successful. Please login as {model.UserName}.";
                    return RedirectToAction("Login", "Account");
                }
                foreach (var error in result.Errors)
                {
                    ModelState.AddModelError(string.Empty, error.Description);
                }
            }
            return View(model);
        }

        [HttpGet]
        public IActionResult Login()
        {
            // ak je uzivatel prihlaseny je presmerovany na home page
            if (User.Identity != null && User.Identity.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            return View();
        }

        [HttpPost]
        public async Task<IActionResult> Login(UserLoginModel model)
        {
            if (ModelState.IsValid)
            {
                var result = await _signInManager.PasswordSignInAsync(model.UserName, model.Password, isPersistent: false, lockoutOnFailure: false);
                if (result.Succeeded)
                {
                    TempData["Message"] = $"Login was successful. Welcome {model.UserName}.";
                    return RedirectToAction("Index", "Home");
                }
                ModelState.AddModelError(string.Empty, "Invalid login attempt.");
                TempData["Message"] = $"Invalid login attempt.";
            }
            return View(model);
        }

        [HttpPost]
        public async Task<IActionResult> Logout()
        {
            if (!User.Identity!.IsAuthenticated)
            {
                return RedirectToAction("Index", "Home");
            }
            await _signInManager.SignOutAsync();
            TempData["Message"] = $"Logout was successful.";
            return RedirectToAction("Login", "Account");
        }
    }
}