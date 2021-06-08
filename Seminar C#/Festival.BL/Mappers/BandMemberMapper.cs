using System.Collections.Generic;
using System.Linq;
using Festival.BL.Factories;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;

namespace Festival.BL.Mappers
{
    public class BandMemberMapper : IMapper<BandMemberEntity, BandMemberListModel, BandMemberDetailModel>
    {
        public IEnumerable<BandMemberListModel> Map(IQueryable<BandMemberEntity> entities)
            => entities?.Select(entity => MapListModel(entity)).ToValueCollection();

        public static BandMemberListModel MapListModel(BandMemberEntity entity) => new BandMemberListModel()
        {
            Id = entity.Id,
            Name = entity.Name,
            ImageUrl = entity.ImageUrl
        };

        public BandMemberDetailModel Map(BandMemberEntity entity) => entity == null
            ? null
            : new BandMemberDetailModel
            {
                Id = entity.Id,
                Name = entity.Name,
                NickName = entity.NickName,
                HeadMember = entity.HeadMember,
                BirthDate = entity.BirthDate,
                ImageUrl = entity.ImageUrl
            };

        public BandMemberEntity Map(BandMemberDetailModel detailModel, IEntityFactory entityFactory)
        {
            var entity = (entityFactory ??= new CreateNewEntityFactory()).CreateOrGet<BandMemberEntity>(detailModel.Id);

            entity.Id = detailModel.Id;
            entity.Name = detailModel.Name;
            entity.NickName = detailModel.NickName;
            entity.HeadMember = detailModel.HeadMember;
            entity.BirthDate = detailModel.BirthDate;
            entity.ImageUrl = detailModel.ImageUrl;

            return entity;
        }
    }
}
