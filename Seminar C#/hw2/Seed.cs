using HW02.BussinessContext;

namespace HW02;

public class Seed
{
    public OperationManager operationManager;
    public ConsoleHandler consoleHandler;
    public Seed(OperationManager oManager, ConsoleHandler cHandler)
    {
        operationManager = oManager;
        consoleHandler = cHandler;
    }


    private void AddCategories()
    {
        List<string> categories = new List<string>()
        {
            "add-category Sport",
            "add-category Phones",
            "add-category Books"
        };
        foreach (string category in categories)
        {
            Operation operation = consoleHandler.ManageUserRequest(category.Split(" "));
            operationManager.SendOperaton(operation);
        }
    }

    private void AddProducts()
    {
        List<string> products = new List<string>()
        {
            "add-product Ball 1 12.50",
            "add-product Stick 1 140.2",
            "add-product myPhone15 2 440.99",
            "add-product myPhone16 2 819.99",
            "add-product Brno-history 3 14.50",
            "add-product Moon 3 12.99",
        };
        foreach (string product in products)
        {
            Operation operation = consoleHandler.ManageUserRequest(product.Split(" "));
            operationManager.SendOperaton(operation);
        }
    }

    public void InsertData()
    {
        AddCategories();
        AddProducts();
    }
    

}