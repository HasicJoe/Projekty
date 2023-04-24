using HW02.BussinessContext;

namespace HW02;

public class OperationEventArgs : EventArgs
{
    public Operation Operation { get; set;}

    public OperationEventArgs(Operation operation)
    {
        Operation=operation;
    }
}

