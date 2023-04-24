using System.Linq;

namespace HW02.BussinessContext;

public class OutputHandler
{
    private int GetMaxLength(string header, int length)
    {
        if (header.Length > length)
        {
            return header.Length;
        }
        return length;
    }
    public void PrintCategories(List<Category> categories)
    {
       // handle categories list
        int nameLength = GetMaxLength("Name", categories.OrderByDescending(c => c.Name.Length).First().Name.Length);
        int idLength = GetMaxLength("Id", categories.OrderByDescending(c => c.Id.ToString().Length).First().Id.ToString().Length);
        string idHeader = "Id".PadRight(idLength);
        string nameHeader = "Name".PadRight(nameLength);
        Console.WriteLine($"{idHeader} | {nameHeader}");
        Console.WriteLine(new string('_', $"{idHeader} | {nameHeader}".Length));

        foreach (Category category in categories)
        {
            Console.WriteLine(category.CategoryInfo(idLength, nameLength));
        }
    }

    public void PrintProducts(List<Product> products)
    {
        // handle products list
        int idLength = GetMaxLength("Id", products.OrderByDescending(c => c.Id.ToString().Length).First().Id.ToString().Length);
        int nameLength = GetMaxLength("Name", products.OrderByDescending(c => c.Name.Length).First().Name.Length);
        int categoryIdLength = GetMaxLength("CategoryId", products.OrderByDescending(c => c.CategoryId.ToString().Length).First().CategoryId.ToString().Length);
        int priceLength = GetMaxLength("Price", products.OrderByDescending(c => c.Price.ToString().Length).First().Price.ToString().Length);
        string idHeader = "Id".PadRight(idLength);
        string nameHeader = "Name".PadRight(nameLength);
        string categoryIdHeader = "CategoryId".PadRight(categoryIdLength);
        string priceHeader = "Price".PadRight(priceLength);
        Console.WriteLine($"{idHeader} | {nameHeader} | {categoryIdHeader} | {priceHeader}");
        Console.WriteLine(new string('_', $"{idHeader} | {nameHeader} | {categoryIdHeader} | {priceHeader}".Length));

        foreach (Product product in products)
        {
            Console.WriteLine(product.ProductInfo(idLength, nameLength, categoryIdLength, priceLength));
        }
    }
}