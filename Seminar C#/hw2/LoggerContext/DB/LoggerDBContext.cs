using HW02.BussinessContext;
using HW02.Helpers;

namespace HW02.LoggerContext.DB
{
    public class LoggerDBContext
    {

        private readonly string[] _paths = { "..", "..", "..", "LoggerContext", "DB", "Storage", "Log.txt" };
        private readonly string _filePath;

        public LoggerDBContext()
        {
            _filePath = Path.Combine(_paths);
            FileHelper.CreateFile(_filePath);
        }

        public void WriteLog(Operation o)
        {
            string timestamp = DateTime.Now.ToString("dd/MM/yyyy HH:mm:ss");
            using (StreamWriter sw = new StreamWriter(_filePath, true))
            {
                if (o.operationSuccess)
                {
                    const string success = "Success";
                    if ((o.operationName == "Add") || (o.operationName == "Delete"))
                    {
                        if (o.entity != null)
                        {
                            sw.WriteLine($"[{timestamp}] {o.operationName}; {o.entityType}; {success}; {o.entity.Id}; {o.entity.Name}; {o.entity.CategoryId}");
                        }
                        else
                        {
                            sw.WriteLine($"[{timestamp}] {o.operationName}; {o.entityType}; {success};");
                        }
                    }
                    else if (o.operationName == "Get")
                    {
                        sw.WriteLine($"[{timestamp}] {o.operationName}; {o.entityType}; {success};");
                    }
                }
                else
                {
                    const string failure = "Failure";
                    sw.WriteLine($"[{timestamp}] {o.operationName}; {o.entityType}; {failure}; {o.exceptionInfo}");
                }
            }
        }
    }
}
