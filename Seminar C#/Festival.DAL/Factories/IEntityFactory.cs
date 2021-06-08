using Festival.DAL.Entities;
using System;

namespace Festival.DAL.Factories
{
    public interface IEntityFactory
    {
        TEntity CreateOrGet<TEntity>(Guid id) where TEntity : class, IEntity, new();
    }
}
