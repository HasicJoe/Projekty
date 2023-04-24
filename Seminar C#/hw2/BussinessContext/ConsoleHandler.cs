using System.ComponentModel;
using HW02.BussinessContext.Services;
using System.Xml.Linq;

namespace HW02.BussinessContext;

public class ConsoleHandler
{
    private static CategoryService categoryService = new();
    private static ProductService productService = new ProductService(categoryService.GetCategoryContext());
    private static Generator _generator = new();
    private static OutputHandler _outputHandler = new OutputHandler();
    private Entity entity;
    private OperationType _lastOperation;
    private bool _lastOperationSuccess = true;

    public void CheckArgumentLength(string[] args, int length)
    {
        if (args.Length != length)
        {
            throw new InvalidInputArgumentException($"Input arguments have invalid length. Expected length: {length}");
        }
    }

    public void TryParseAddProduct(string[] request)
    {
        CheckArgumentLength(request, 4);
        string name = request[1];
        int categoryId;
        double price;
        _lastOperation = OperationType.AddProduct;
        if (int.TryParse(request[2], out categoryId) && double.TryParse(request[3], out price))
        {
            if (!categoryService.Find(categoryId))
            {
                throw new InvalidOperationException($"Invalid add-product operation. Category with categoryId {categoryId} does not exists.");
            }
            int productId = _generator.GenerateProductId();
            if (price <= 0.0)
            {
                throw new InvalidEnumArgumentException("Invalid price. Price should be positive.");
            }
            productService.Add(new Product(productId, name, categoryId, price));
            entity = new Entity(productId, name, categoryId);
        }
        else
            throw new InvalidInputArgumentException("Unable to resolve command add-product [name] [categoryId] [price]. Invalid arguments");
    }

    public void TryParseDeleteProduct(string[] request)
    {
        CheckArgumentLength(request, 2);
        int productId;
        _lastOperation = OperationType.DeleteProduct;
        if (int.TryParse(request[1], out productId))
        {
            entity = productService.GetProductEntity(productId);
            productService.Remove(productId);
        }
        else
        {
            throw new InvalidInputArgumentException("Unable to resolve command delete-product [productId]. Invalid arguments.");
        }
    }

    private void TryParseListProducts(string[] request)
    {
        CheckArgumentLength(request, 1);
        _outputHandler.PrintProducts(productService.List());
        _lastOperation = OperationType.ListProducts;
    }

    private void TryParseAddCategory(string[] request)
    {
        CheckArgumentLength(request, 2);
        int categoryId = _generator.GenerateCategoryId();
        categoryService.Add(new Category(categoryId, request[1], categoryId));
        entity = categoryService.GetCategoryEntity(categoryId);
        _lastOperation = OperationType.AddCategory;
        
    }

    private void TryParseDeleteCategory(string[] request)
    {
        CheckArgumentLength(request, 2);
        int categoryId;
        _lastOperation = OperationType.DeleteCategory;
        if (int.TryParse(request[1], out categoryId))
        {
            entity = categoryService.GetCategoryEntity(categoryId);
            productService.RemoveCategory(categoryId);
            categoryService.Remove(categoryId);
        }
        else
        {
            throw new InvalidInputArgumentException("Unable to resolve command delete-category [categoryId]. Invalid arguments.");
        }

    }

    private void TryParseListCategories(string[] request)
    {
        CheckArgumentLength(request, 1);
        _outputHandler.PrintCategories(categoryService.List());
        _lastOperation = OperationType.ListCategories;
    }


    private void TryParseGetProductsByCategory(string[] request)
    {
        CheckArgumentLength(request, 2);
        int categoryId;
        if (int.TryParse(request[1], out categoryId))
        {
            _outputHandler.PrintProducts(productService.ListCategory(categoryId));
        }
        else
        {
            throw new InvalidInputArgumentException("Unable to resolve command get-products-by-category [categoryId]. Invalid arguments.");
        }
        _lastOperation = OperationType.GetProductByCategory;
    }

    public OperationType GetLastOperationType()
    {
        return _lastOperation;
    }

    public Operation ManageUserRequest(string[] request)
    {
        string message = "";
        try
        {
            switch (request[0])
            {
                case "add-product":
                    TryParseAddProduct(request);
                    break;
                case "delete-product":
                    TryParseDeleteProduct(request);
                    break;
                case "list-products":
                    TryParseListProducts(request);
                    break;
                case "add-category":
                    TryParseAddCategory(request);
                    break;
                case "delete-category":
                    TryParseDeleteCategory(request);
                    break;
                case "list-categories":
                    TryParseListCategories(request);
                    break;
                case "get-products-by-category":
                    TryParseGetProductsByCategory(request);
                    break;
                default:
                    throw new InvalidInputArgumentException();
            }
            _lastOperationSuccess = true;
        }
        catch (Exception ex)
        {
            message = ex.Message;
            _lastOperationSuccess = false;
            Console.WriteLine($"{ex.GetType().Name}: {ex.Message}");
        }
        Operation operation = new Operation(_lastOperation, _lastOperationSuccess, entity);
        if (!_lastOperationSuccess)
        {
            operation.AddExceptionInfo(message);
        }
        return operation;
    }
}


