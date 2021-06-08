using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query;
using System;
using System.Linq;

namespace Festival.BL.Facades
{
    public class BandFacade : CrudeFacadeBase<BandEntity, BandListModel, BandDetailModel>
    {
        public BandFacade(
            UnitOfWork unitOfWork,
            RepositoryBase<BandEntity> repository,
            IMapper<BandEntity, BandListModel, BandDetailModel> mapper,
            IEntityFactory entityFactory)
            : base(unitOfWork, repository, mapper, entityFactory)
        {
        }

        protected override Func<IQueryable<BandEntity>, IIncludableQueryable<BandEntity, object>>[] Includes { get; } =
            new Func<IQueryable<BandEntity>, IIncludableQueryable<BandEntity, object>>[]
            {
                entities => entities.Include(entity => entity.BandMembers),
                entities => entities.Include(entity => entity.Performances)
            };
    }
}
