using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore.Query;
using System;
using System.Linq;
using Microsoft.EntityFrameworkCore;

namespace Festival.BL.Facades
{
    public class StageFacade : CrudeFacadeBase<StageEntity, StageListModel, StageDetailModel>
    {
        public StageFacade(UnitOfWork unitOfWork,
            RepositoryBase<StageEntity> repository,
            IMapper<StageEntity, StageListModel, StageDetailModel> mapper,
            IEntityFactory entityFactory)
            : base(unitOfWork, repository, mapper, entityFactory)
        {
        }

        protected override Func<IQueryable<StageEntity>, IIncludableQueryable<StageEntity, object>>[] Includes { get; } =
            new Func<IQueryable<StageEntity>, IIncludableQueryable<StageEntity, object>>[]
            {
                entities => entities.Include(entity => entity.Performances)
            };
    }
}
