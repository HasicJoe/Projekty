namespace HW02
{
    public class EntityWithSameIdAlreadyExistException<T> : Exception
    {
        public EntityWithSameIdAlreadyExistException(T entity) : base($"{entity} already exists") { }
    }
}
