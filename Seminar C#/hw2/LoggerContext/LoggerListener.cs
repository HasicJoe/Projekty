using HW02.BussinessContext;
using HW02.Helpers;
using HW02.LoggerContext.DB;

namespace HW02
{
    public class LoggerListener
    {
        private LoggerDBContext _loggerContext;
        public LoggerListener(OperationManager om)
        {
            _loggerContext = new LoggerDBContext();
            om.entityOperationHandler += AddLine;
        }

        public void AddLine(object sender, OperationEventArgs args)
        {
            try
            {
                _loggerContext.WriteLog(args.Operation);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"{ex.GetType().Name}: {ex.Message}");
            }
        }
    }
}
