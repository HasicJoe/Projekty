using System.Collections.Generic;
using System.Linq;
using Festival.BL.Factories;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;

namespace Festival.BL.Mappers
{
    public class StageMapper : IMapper<StageEntity, StageListModel, StageDetailModel>
    {
        public IEnumerable<StageListModel> Map(IQueryable<StageEntity> entities)
    => entities?.Select(entity => MapListModel(entity)).ToValueCollection();

        public static StageListModel MapListModel(StageEntity entity)
            => new StageListModel
            {
                Id = entity.Id,
                Name = entity.Name,
                ImageUrl = entity.ImageUrl
            };

        public StageDetailModel Map(StageEntity entity)
            => entity == null ? null : new StageDetailModel
            {
                Id = entity.Id,
                Name = entity.Name,
                ImageUrl = entity.ImageUrl,
                StageDescription = entity.StageDescription,
                
                Performances = entity.Performances.Select(
                    performanceListModel => new PerformanceListModel()
                    {
                        Id = performanceListModel.Id,
                        BandId = performanceListModel.BandId,
                        StageId = performanceListModel.StageId,
                        TimeStart = performanceListModel.TimeStart,
                        TimeEnd = performanceListModel.TimeEnd,
                        BandName = performanceListModel.Band != null ? performanceListModel.Band.Name : ""

                    }).ToValueCollection()
            };

        public StageEntity Map(StageDetailModel detailModel, IEntityFactory entityFactory)
        {
            var entity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<StageEntity>(detailModel.Id);

            entity.Id = detailModel.Id;
            entity.Name = detailModel.Name;
            entity.StageDescription = detailModel.StageDescription;
            entity.ImageUrl = detailModel.ImageUrl;
            entity.Performances = detailModel.Performances.Select(model =>
            {
                var performanceEntity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<PerformanceEntity>(model.Id);
                performanceEntity.BandId = model.BandId;
                performanceEntity.StageId = model.StageId;
                return performanceEntity;
            }).ToValueCollection();

            return entity;
        }
    }
}
