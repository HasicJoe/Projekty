using System;
using System.Linq;
using Festival.DAL.Entities;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace Festival.DAL.Factories
{
    public class EntityFactory : IEntityFactory
    {

        private readonly ChangeTracker _changeTracker;
        public EntityFactory(ChangeTracker changeTracker) => _changeTracker = changeTracker;


        public TEntity CreateOrGet<TEntity>(Guid id) where TEntity : class, IEntity, new()
        {
            TEntity entity = null;
            if (id != Guid.Empty)
            {
                entity = _changeTracker?.Entries<TEntity>().SingleOrDefault(i => i.Entity.Id == id)
                    ?.Entity;
                if (entity == null)
                {
                    entity = new TEntity { Id = id };
                    _changeTracker?.Context.Attach(entity);
                }
            }
            else
            {
                entity = new TEntity();
            }
            return entity;

        }
    }
}
