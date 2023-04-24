namespace HW02.BussinessContext;

public class Product : Entity
{
  
    public double Price { get; set; }

    public Product(int id, string name, int categoryId, double price)
    {
        Id = id;
        Name = name;
        CategoryId = categoryId;
        Price = price;
    }

    public string ProductInfo(int idLength, int nameLength, int categoryIdLength, int priceLength)
    {
        return
            $"{Id.ToString().PadRight(idLength)} | {Name.PadRight(nameLength)} | {CategoryId.ToString().PadRight(categoryIdLength)} | {Price.ToString().PadRight(priceLength)}";
    }
}