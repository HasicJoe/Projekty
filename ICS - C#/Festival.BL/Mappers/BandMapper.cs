using Festival.BL.Factories;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Festival.BL.Mappers
{
    public class BandMapper : IMapper<BandEntity, BandListModel, BandDetailModel>
    {
        public IEnumerable<BandListModel> Map(IQueryable<BandEntity> entities)
         => entities?.Select(entity => MapListModel(entity)).ToValueCollection();

        public static BandListModel MapListModel(BandEntity entity)
            => new BandListModel
            {
                Id = entity.Id,
                Name = entity.Name,
                ImageUrl = entity.ImageUrl
            };

        public BandDetailModel Map(BandEntity entity)
            => entity == null ? null : new BandDetailModel
            {
                Id = entity.Id,
                Name = entity.Name,
                Genre = entity.Genre,
                ImageUrl = entity.ImageUrl,
                CountryOfOrigin = entity.CountryOfOrigin,
                BandDescription = entity.BandDescription,
                ProgramDescription = entity.ProgramDescription,

                BandMembers = entity.BandMembers.Select(
                    bandListModel => new BandMemberListModel()
                    {
                        Id = bandListModel.Id,
                        Name = bandListModel.Name,
                        ImageUrl = bandListModel.ImageUrl
                    }).ToValueCollection(),

                Performances = entity.Performances.Select(
                    performanceListModel => new PerformanceListModel()
                    {
                        Id = performanceListModel.Id,
                        BandId = performanceListModel.BandId,
                        StageId = performanceListModel.StageId,
                        TimeStart = performanceListModel.TimeStart,
                        TimeEnd = performanceListModel.TimeEnd

                    }).ToValueCollection()
            };

        public BandEntity Map(BandDetailModel detailModel, IEntityFactory entityFactory)
        {
            var entity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<BandEntity>(detailModel.Id);

            entity.Id = detailModel.Id;
            entity.Name = detailModel.Name;
            entity.Genre = detailModel.Genre;
            entity.ImageUrl = detailModel.ImageUrl;
            entity.CountryOfOrigin = detailModel.CountryOfOrigin;
            entity.BandDescription = detailModel.BandDescription;
            entity.ProgramDescription = detailModel.ProgramDescription;

            entity.BandMembers = detailModel.BandMembers.Select(model =>
                {
                    var bandMemberEntity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<BandMemberEntity>(model.Id);
                    bandMemberEntity.Id = model.Id;
                    bandMemberEntity.Name = model.Name;
                    bandMemberEntity.ImageUrl = model.ImageUrl;
                    return bandMemberEntity;
                }).ToValueCollection();

            entity.Performances = detailModel.Performances.Select(model =>
                {
                    var performanceEntity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<PerformanceEntity>(model.Id);
                    performanceEntity.BandId = model.BandId;
                    performanceEntity.StageId = model.StageId;
                    performanceEntity.TimeStart = model.TimeStart;
                    performanceEntity.TimeEnd = model.TimeEnd;
                    return performanceEntity;
                }).ToValueCollection();

            return entity;
        }
    }
}
