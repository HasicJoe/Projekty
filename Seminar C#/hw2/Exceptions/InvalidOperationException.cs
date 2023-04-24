namespace HW02;

public class InvalidOperationException : Exception
{
    public InvalidOperationException() : base("Invalid operation - unable to perform requested operation.") { }
    public InvalidOperationException(string msg) : base($"{msg}") { }
}