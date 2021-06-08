using System;
using System.Collections.Generic;
using System.Linq;
using Festival.DAL.Entities;
using Microsoft.EntityFrameworkCore;

namespace Festival.DAL.Repositories
{
    public class RepositoryBase<TEntity> where TEntity : class, IEntity, new()
    {
        public UnitOfWork.UnitOfWork _unitOfWork;

        public RepositoryBase(UnitOfWork.UnitOfWork unitOfWork)
        {
            _unitOfWork = unitOfWork ?? throw new ArgumentNullException(nameof(unitOfWork));
        }

        public void Delete(TEntity entity)
        {
            _unitOfWork.DbContext.Set<TEntity>().Remove(entity);
        }

        public void DeleteById(Guid entityGuid)
        {
            var entity = _unitOfWork.DbContext.Set<TEntity>().SingleOrDefault(i => i.Id.Equals(entityGuid));
            Delete(entity);
        }

        public TEntity GetById(Guid entityGuid)
        {
            return _unitOfWork.DbContext.Set<TEntity>().SingleOrDefault(entity => entity.Id.Equals(entityGuid));
        }

        public IQueryable<TEntity> GetAll()
        {
            return _unitOfWork.DbContext
                .Set<TEntity>();
        }

        public TEntity InsertOrUpdate(TEntity entity)
        {
            _unitOfWork.DbContext.Update<TEntity>(entity);
            SynchronizeCollections(entity);
            return entity;
        }

        private void SynchronizeCollections(TEntity entity)
        {
            var collectionsToBeSynchronized = typeof(TEntity).GetProperties().Where(i =>
                i.PropertyType.IsGenericType && i.PropertyType.GetGenericTypeDefinition() == typeof(ICollection<>));

            if (!collectionsToBeSynchronized?.Any() ?? false) return;


            var entityInDb = GetById(entity.Id);
            if (entityInDb == null) return;


            foreach (var collectionSelector in collectionsToBeSynchronized)
            {
                var updatedCollection = (collectionSelector.GetValue(entity) as ICollection<IEntity>).ToArray();
                var collectionInDb = (collectionSelector.GetValue(entityInDb) as ICollection<IEntity>).ToArray();


                foreach (var item in collectionInDb)
                    if (!updatedCollection.Contains(item, PrimaryKeyComparers.IdComparer))
                        DeleteById(item.Id);
            }
        }
    }
}