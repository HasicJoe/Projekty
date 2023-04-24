namespace HW02.BussinessContext;

public class Category : Entity
{

    public Category(int id, string name, int categoryId)
    {
        Id = id;
        Name = name;
        CategoryId = categoryId;
    }

    public string CategoryInfo(int idLength, int nameLength)
    {
        return $"{Id.ToString().PadRight(idLength)} | {Name.PadRight(nameLength)}";
    }

}