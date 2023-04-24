namespace HW02.BussinessContext;

public class Entity
{
    public int Id { get; set; }
    public string Name { get; set; }
    public int CategoryId { get; set; }

    public Entity(int id, string name, int categoryId)
    {
        Id = id;
        Name = name;
        CategoryId = categoryId;
    }

    public Entity()
    {
    }

    public void Update(int id, string name, int categoryId)
    {
        Id = Id;
        Name = name;
        CategoryId = categoryId;
    }
}