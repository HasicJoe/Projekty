using HW02.BussinessContext.FileDatabase;

namespace HW02.BussinessContext.Services;

public class CategoryService
{
    private Dictionary<int, Category> _storedCategories = new Dictionary<int, Category>();
    private CategoryDBContext _dbContext = new CategoryDBContext();

    public void Add(Category category)
    {
        if (_storedCategories.ContainsKey(category.Id))
        {
            throw new EntityWithSameIdAlreadyExistException<Category>(category);
        }
        _storedCategories.Add(category.Id, category);
        _dbContext.SaveCategories(_storedCategories.Values);
    }

    public void Remove(int categoryId)
    {
        if (!_storedCategories.ContainsKey(categoryId))
        {
            throw new InvalidOperationException();
        }
        _storedCategories.Remove(categoryId);
        _dbContext.SaveCategories(_storedCategories.Values);
    }

    public List<Category> List()
    {
        return _dbContext.ReadCategories();
    }

    public Entity? GetCategoryEntity(int categoryId)
    {
        foreach (var category in List())
        {
            if (category.Id == categoryId)
            {
                return new Entity(category.Id, category.Name, category.Id);
            }
        }
        return null;
    }

    public bool Find(int categoryId)
    {
        return _storedCategories.ContainsKey(categoryId);
    }

    public CategoryDBContext GetCategoryContext()
    {
        return _dbContext;
    }
}