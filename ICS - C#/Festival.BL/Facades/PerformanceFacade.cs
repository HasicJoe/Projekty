using System.Diagnostics;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;

namespace Festival.BL.Facades
{
    public class PerformanceFacade : CrudeFacadeBase<PerformanceEntity, PerformanceListModel, PerformanceDetailModel>
    {
        public PerformanceFacade(UnitOfWork unitOfWork,
            RepositoryBase<PerformanceEntity> repository,
            IMapper<PerformanceEntity, PerformanceListModel, PerformanceDetailModel> mapper,
            IEntityFactory entityFactory)
            : base(unitOfWork, repository, mapper, entityFactory)
        {
            Mapper = mapper;
            EntityFactory = entityFactory;
            Repository = repository;
            UnitOfWork = unitOfWork;
        }

        private bool CheckCollision(PerformanceDetailModel model)
        {
            // Check for time collisions
            var isCollision = false;
            var performances = Repository.GetAll();
            var newStart = model.TimeStart;
            var newEnd = model.TimeEnd;
            foreach (var performance in performances)
            {
                if(performance.Id == model.Id)
                {
                    continue;
                }

                if(performance.BandId != model.Band.Id)
                {
                    continue;
                }

                if(newStart <= performance.TimeEnd && newEnd >= performance.TimeStart)
                {
                    // Time collision
                    isCollision = true;
                    break;
                }
            }

            return isCollision;
        }

        public override PerformanceDetailModel? Save(PerformanceDetailModel model)
        {
            var _ = GetById(model.Id);

            var entity = Mapper.Map(model, EntityFactory);

            if (!CheckCollision(model))
            {
                entity = Repository.InsertOrUpdate(entity);

                UnitOfWork.Commit();

                return GetById(entity.Id);
            }
            else
            {
                Debug.WriteLine("Time collision is not allowed. Performance not saved.");
                return null;
            }
        }
    }
}

