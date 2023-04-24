using HW02.BussinessContext;

namespace HW02;

public class OperationManager
{
    public delegate void EntityOperationHandler(object sender, OperationEventArgs args);
    public event EntityOperationHandler entityOperationHandler;

    public void SendOperaton(Operation operation)
    {
        entityOperationHandler?.Invoke(this, new OperationEventArgs(operation));
    }

}