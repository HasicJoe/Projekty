namespace HW02
{
    public class InvalidInputArgumentException : Exception
    {
        public InvalidInputArgumentException(string msg) : base($"Unable to match user request - invalid arguments. {msg}") { }
        public InvalidInputArgumentException() : base("Unable to match user request - invalid arguments.") { }
    }
}