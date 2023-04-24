using System.Security.Cryptography.X509Certificates;
using HW02.AnalyticalDataContext;
using HW02.BussinessContext;

namespace HW02
{
    public class Program
    {
        public static void Main(string[] Args)
        {
            // TODO: Initialize all clases here, when some dependency needed, insert object through constrcutor
            ConsoleHandler consoleHandler = new ConsoleHandler();
            OperationManager operationManager = new OperationManager();
            LoggerListener loggerListener = new LoggerListener(operationManager);
            AnalyticalDataListener analyticalListener = new AnalyticalDataListener(operationManager);
            Seed seed = new Seed(operationManager, consoleHandler);
            seed.InsertData();


            while (true)
            { 
                Operation operation = consoleHandler.ManageUserRequest(Console.ReadLine().Split(" "));
                operationManager.SendOperaton(operation);
            }
        }
    }
}
