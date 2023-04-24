namespace HW02.Helpers
{
    public static class FileHelper
    {
        public static void CreateFile(string path)
        {
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            using var fs = File.Create(path);
        }
    }
}
