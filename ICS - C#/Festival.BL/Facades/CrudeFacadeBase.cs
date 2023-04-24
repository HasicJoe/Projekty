using System;
using System.Collections.Generic;
using System.Linq;
using Festival.BL.Mappers;
using Festival.DAL.Entities;
using Festival.BL.Models;
using Festival.DAL.UnitOfWork;
using Festival.DAL.Repositories;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore.Query;

namespace Festival.BL.Facades
{
    public abstract class CrudeFacadeBase<TEntity, TListModel, TDetailModel>
        where TEntity : class, IEntity, new()
        where TListModel : IModel, new()
        where TDetailModel : IModel, new()
    {

        private protected IEntityFactory EntityFactory;
        private protected RepositoryBase<TEntity> Repository;
        private protected UnitOfWork UnitOfWork;
        private protected IMapper<TEntity, TListModel, TDetailModel> Mapper;

        protected CrudeFacadeBase(
            UnitOfWork unitOfWork,
            RepositoryBase<TEntity> repository,
            IMapper<TEntity, TListModel, TDetailModel> mapper,
            IEntityFactory entityFactory)
        {
            Mapper = mapper;
            EntityFactory = entityFactory;
            Repository = repository;
            UnitOfWork = unitOfWork;
        }

        protected virtual Func<IQueryable<TEntity>, IIncludableQueryable<TEntity, object>>[] Includes { get; } = 
            Array.Empty<Func<IQueryable<TEntity>, IIncludableQueryable<TEntity, object>>>();

        public IEnumerable<TListModel> GetAllList() => Mapper.Map(Repository.GetAll());

        public TDetailModel GetById(Guid id)
        {
            var query = Repository.GetAll();

            foreach (var include in Includes)
            {
                query = include(query);
            }

            return Mapper
                .Map(query.SingleOrDefault(i => i.Id.Equals(id)));
        }

        public void Delete(Guid id)
        {
            Repository.DeleteById(id);
            UnitOfWork.Commit();
        }

        public void DeleteEntity(TEntity entity)
        {
            Repository.Delete(entity);
            UnitOfWork.Commit();
        }

        public void Delete(TListModel model) => Delete(model.Id);

        public void Delete(TDetailModel model) => Delete(model.Id);

        public virtual TDetailModel Save(TDetailModel model)
        {
            var _ = GetById(model.Id); 

            var entity = Mapper.Map(model, EntityFactory);
            entity = Repository.InsertOrUpdate(entity);
            UnitOfWork.Commit();

            return GetById(entity.Id);
        }
        
    }
}
