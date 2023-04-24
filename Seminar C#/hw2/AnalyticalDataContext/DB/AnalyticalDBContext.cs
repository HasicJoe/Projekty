using HW02.Helpers;
using System.Text.Json;

namespace HW02.AnalyticalDataContext.DB
{
    public class AnalyticalDBContext
    {
        private readonly string[] _paths = { "..", "..", "..", "AnalyticalDataContext", "DB", "Storage", "AnalyticalData.json" };
        private readonly string _filePath;

        public AnalyticalDBContext()
        {
            _filePath = Path.Combine(_paths);
            FileHelper.CreateFile(_filePath);
        }

        // TODO: replace type List<object> in functions headers to the appropriate data model -> List<YourDataModel>
        public void SaveAnalyticalData(List<AnalyticalDataModel> log)
        {
            string jsonString = JsonSerializer.Serialize(log);
            using (StreamWriter outputFile = new StreamWriter(_filePath))
            {
                int itemCount = log.Count();
                int loopIndex = 0;
                outputFile.WriteLine("[");
                foreach (AnalyticalDataModel item in log)
                {
                    loopIndex++;
                    string data = "";
                    if (loopIndex < itemCount)
                    {
                        data = $"\t{{\r\n    \"CategoryId\": {item.CategoryId},\r\n    \"CategoryName\": \"{item.CategoryName}\",\r\n    \"ProductCount\": {item.ProductCount}\r\n\t}},";
                    }
                    else
                    {
                        data = $"\t{{\r\n    \"CategoryId\": {item.CategoryId},\r\n    \"CategoryName\": \"{item.CategoryName}\",\r\n    \"ProductCount\": {item.ProductCount}\r\n\t}}";
                    }
                    outputFile.WriteLine(data);
                }
                outputFile.WriteLine("]");
            }
        }

        public List<AnalyticalDataModel> ReadAnalyticalData()
        {
            string? line;
            using (StreamReader inputFile = new StreamReader(_filePath))
            {
                line = inputFile.ReadLine();
            }

            if (line == null)
            {
                return new List<AnalyticalDataModel>();
            }

            var model = JsonSerializer.Deserialize<List<AnalyticalDataModel>>(line);
            return model;
        }
    }
}
