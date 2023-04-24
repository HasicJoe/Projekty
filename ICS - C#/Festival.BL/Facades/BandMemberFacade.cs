using System;
using System.Linq;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query;

namespace Festival.BL.Facades
{
    public class BandMemberFacade : CrudeFacadeBase<BandMemberEntity, BandMemberListModel, BandMemberDetailModel>
    {
        public BandMemberFacade(UnitOfWork unitOfWork,
            RepositoryBase<BandMemberEntity> repository,
            IMapper<BandMemberEntity, BandMemberListModel, BandMemberDetailModel> mapper,
            IEntityFactory entityFactory)
            : base(unitOfWork, repository, mapper, entityFactory)
        {
        }

    }
}
