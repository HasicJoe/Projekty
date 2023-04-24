using HW02.BussinessContext.FileDatabase;

namespace HW02.BussinessContext;

public class ProductService
{
    private Dictionary<int, Product> _storedProducts;
    private ProductDBContext _dbContext;

    public ProductService(CategoryDBContext dbContext)
    {
        _storedProducts = new Dictionary<int, Product>();
        _dbContext = new ProductDBContext(dbContext);
    }

    public void Add(Product product)
    {
        if (_storedProducts.ContainsKey(product.Id))
        {
            throw new EntityWithSameIdAlreadyExistException<Product>(product);
        }
        _storedProducts.Add(product.Id, product);
        _dbContext.SaveProducts(_storedProducts.Values);
    }

    public void Remove(int productId)
    {
        if (!_storedProducts.ContainsKey(productId))
        {
            throw new InvalidInputArgumentException(
                $"Unable to delete product with productId={productId}. This product id does not exist.");
        }
        _storedProducts.Remove(productId);
        _dbContext.SaveProducts(_storedProducts.Values);
    }

    public void RemoveCategory(int categoryId)
    {
        foreach (var product in _storedProducts)
        {
            if (product.Value.CategoryId == categoryId)
            {
                Remove(product.Key);
            }
        }
    }

    public List<Product> List()
    {
        return _dbContext.ReadProducts();
    }

    public Entity? GetProductEntity(int productId)
    {
        foreach(var product in _dbContext.ReadProducts())
        {
            if (product.Id == productId)
            {
                return new Entity(product.Id, product.Name, product.CategoryId);

            }
        }
        return null;
    }


    public List<Product> ListCategory(int categoryId)
    {
        List<Product> products = _dbContext.ReadProducts();
        List<Product> categoryProducts = new List<Product>();
        foreach (var product in products)
        {
            if (product.CategoryId == categoryId)
            {
                categoryProducts.Add(product);
            }
        }
        return categoryProducts;
    }

}