using System;

namespace Festival.DAL.Entities
{
    public abstract record EntityBase() : IEntity
    {
        public Guid Id { get; set; }
    }
}
