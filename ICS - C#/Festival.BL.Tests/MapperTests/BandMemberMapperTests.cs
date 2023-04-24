using System;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore.ChangeTracking;
using Xunit;

namespace Festival.BL.Tests.MapperTests
{
    public class BandMemberMapperTests
    {
        private readonly BandMemberMapper _bandMemberMapper;
        private readonly EntityFactory _entityFactory;
        private static readonly ChangeTracker _changeTracker;

        public BandMemberMapperTests()
        {
            _bandMemberMapper = new BandMemberMapper();
            _entityFactory = new EntityFactory(_changeTracker);
        }

        [Fact]
        public void MapBandMember_EntityToListModel()
        {
            // Arrange
            var bandMemberEntity = new BandMemberEntity()
            {
                Name = "Peter Hrivnak"
            };

            // Act
            var bandMemberListModel = BandMemberMapper.MapListModel(bandMemberEntity);

            // Assert
            Assert.Equal(bandMemberListModel.Id,bandMemberEntity.Id);
            Assert.Equal(bandMemberListModel.Name, bandMemberEntity.Name);
        }

        [Fact]
        public void MapBandMember_EntityToDetailModel()
        {
            // Arrange
            var bandMemberEntity = new BandMemberEntity()
            {
                Name = "Peter Hrivnak",
                NickName = "Kuko",
                HeadMember = true,
                BirthDate = new DateTime(1992, 7, 5)
            };

            // Act
            var bandMemberDetailModel = _bandMemberMapper.Map(bandMemberEntity);

            // Assert
            Assert.Equal(bandMemberEntity.Id,bandMemberDetailModel.Id);
            Assert.Equal(bandMemberEntity.Name, bandMemberDetailModel.Name);
            Assert.Equal(bandMemberEntity.NickName, bandMemberDetailModel.NickName);
            Assert.Equal(bandMemberEntity.HeadMember, bandMemberDetailModel.HeadMember);
            Assert.Equal(bandMemberEntity.BirthDate, bandMemberDetailModel.BirthDate);
        }

        [Fact]
        public void MapBandMember_DetailModelToEntity()
        {
            // Arrange
            var bandMemberDetailModel = new BandMemberDetailModel()
            {
                Name = "Peter Hrivnak",
                NickName = "Kuko",
                HeadMember = true,
                BirthDate = new DateTime(1992, 7, 5),
                ImageUrl = "www.image.com"
            };

            // Act
            var bandMemberEntity = _bandMemberMapper.Map(bandMemberDetailModel, _entityFactory);

            // Assert
            Assert.Equal(bandMemberDetailModel.Id, bandMemberEntity.Id);
            Assert.Equal(bandMemberDetailModel.Name, bandMemberEntity.Name);
            Assert.Equal(bandMemberDetailModel.NickName, bandMemberEntity.NickName);
            Assert.Equal(bandMemberDetailModel.HeadMember, bandMemberEntity.HeadMember);
            Assert.Equal(bandMemberDetailModel.BirthDate, bandMemberEntity.BirthDate);
            Assert.Equal(bandMemberDetailModel.ImageUrl, bandMemberEntity.ImageUrl);
        }
    }
}
