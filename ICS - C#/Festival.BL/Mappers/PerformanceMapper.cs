using Festival.BL.Factories;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace Festival.BL.Mappers
{
    public class PerformanceMapper : IMapper<PerformanceEntity, PerformanceListModel , PerformanceDetailModel > 
    {
        public IEnumerable<PerformanceListModel> Map(IQueryable<PerformanceEntity> entities)
          => entities?.Select(entity => MapListModel(entity)).ToValueCollection();

        public static PerformanceListModel MapListModel(PerformanceEntity? entity)
            => new PerformanceListModel()
            {
                Id = entity.Id,
                BandId = entity.BandId,
                StageId = entity.StageId,
                TimeStart = entity.TimeStart,
                TimeEnd = entity.TimeEnd,

                BandName = entity.Band.Name,
                ResourceIdCollection = new ObservableCollection<object>() { entity.StageId }
            };

        public PerformanceDetailModel Map(PerformanceEntity? entity)
            => entity == null ? null : new PerformanceDetailModel
            {
                Id = entity.Id,
                TimeStart = entity.TimeStart,
                TimeEnd = entity.TimeEnd,
                
                Band = new BandListModel()
                { 
                    Id = entity.Band.Id,
                    Name = entity.Band.Name
                },

                Stage = new StageListModel()
                {
                    Id = entity.Stage.Id,
                    Name = entity.Stage.Name
                }
            };

        public PerformanceEntity Map(PerformanceDetailModel detailModel, IEntityFactory entityFactory)
        {
            var entity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<PerformanceEntity>(detailModel.Id);

            entity.Id = detailModel.Id;
            entity.StageId = detailModel.Stage.Id;
            entity.BandId = detailModel.Band.Id;
            entity.TimeStart = detailModel.TimeStart;
            entity.TimeEnd = detailModel.TimeEnd;

            entity.Stage = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<StageEntity>(detailModel.Stage.Id);
            entity.Band = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<BandEntity>(detailModel.Band.Id);
            return entity;
        }
    }
}
