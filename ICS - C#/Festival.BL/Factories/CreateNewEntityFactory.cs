using System;
using Festival.DAL.Entities;
using Festival.DAL.Factories;

namespace Festival.BL.Factories
{
    public class CreateNewEntityFactory : IEntityFactory
    {
        public TEntity CreateOrGet<TEntity>(Guid id) where TEntity : class, IEntity, new() => new TEntity();
        
    }
}
