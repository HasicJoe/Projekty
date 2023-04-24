using HW02.Helpers;
using System.Text.Json;

namespace HW02.BussinessContext.FileDatabase
{
    public class CategoryDBContext
    {
        private readonly string[] _paths = { "..", "..", "..", "BussinessContext", "DB", "Storage", "Categories.json" };
        private readonly string _filePath;

        public CategoryDBContext()
        {
            _filePath = Path.Combine(_paths);
            FileHelper.CreateFile(_filePath);
        }

        public void SaveCategories(IEnumerable<Category> categories)
        {
            if (categories.Select(p => p.Id).Distinct().Count() != categories.Count())
            {
                var duplicitCategory = categories.GroupBy(c => c.Id).Select(g => new { id = g.Key, count = g.Count()}).Where(g => g.count > 1).First();
                var category = categories.Where(c => c.Id == duplicitCategory.id).First();
                throw new EntityWithSameIdAlreadyExistException<Category>(category);
            }

            string jsonString = JsonSerializer.Serialize(categories);
            File.Delete(_filePath);
            using StreamWriter outputFile = new StreamWriter(_filePath);
            outputFile.WriteLine(jsonString);
        }

        public List<Category> ReadCategories()
        {
            string? line;
            using (StreamReader inputFile = new StreamReader(_filePath))
            {
                line = inputFile.ReadLine();
            }

            if (line == null)
            {
                return new List<Category>();
            }

            var model = JsonSerializer.Deserialize<List<Category>>(line);
            return model;
        }
    }
}
