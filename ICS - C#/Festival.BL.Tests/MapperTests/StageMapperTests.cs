using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore.ChangeTracking;
using Nemesis.Essentials.Design;
using Xunit;

namespace Festival.BL.Tests.MapperTests
{
    public class StageMapperTests
    {
        private readonly StageMapper _mapper;
        private readonly EntityFactory _entityFactory;
        private static readonly ChangeTracker _changeTracker;
        private readonly CultureInfo _cultureInfo;

        public StageMapperTests()
        {
            _mapper = new StageMapper();
            _entityFactory = new EntityFactory(_changeTracker);
            _cultureInfo = new CultureInfo("de-DE");
        }


        [Fact]
        public void StageEntityToListModel()
        {
            // Arrange
            var stageEntity = new StageEntity()
            {
                Name = "stage",
                StageDescription = null
            };

            // Act
            var stageListModel = StageMapper.MapListModel(stageEntity);

            // Assert
            Assert.Equal(stageEntity.Id, stageListModel.Id);
            Assert.Equal(stageEntity.Name, stageListModel.Name);
        }

        [Fact]
        public void StageEntityToDetailModel()
        {

            // Arrange

            var bandEntity = new BandEntity()
            {
                Name = "Kabat",
                Genre = "Rock",
                CountryOfOrigin = "Czech Republic",
                BandDescription = "Kdyz se u nas chlapi poperou",
                ProgramDescription = "They'll play for 1 hour.",
                BandMembers =
                {
                    new BandMemberEntity()
                    {
                        HeadMember = true,
                        Name = "Josef Vojtek",
                        BirthDate = new DateTime(1972, 11, 5)
                    },
                    new BandMemberEntity()
                    {
                        HeadMember = false,
                        Name = "Tomas Krulich",
                        BirthDate = new DateTime(1974, 11, 5)
                    }
                }
            };

            var stageEntity = new StageEntity()
            {
                Name = "stage",
                StageDescription = "description",
                ImageUrl = "https://upload.wikimedia.org/wikipedia/commons/8/8d/Green_Stage_at_Southside_Festival_2019.jpg",
            };


            // Act
            var stageDetailModel = _mapper.Map(stageEntity);


            // Assert
            Assert.Equal(stageEntity.Id, stageDetailModel.Id);
            Assert.Equal(stageEntity.Name, stageDetailModel.Name);
            Assert.Equal(stageEntity.StageDescription, stageDetailModel.StageDescription);
            
        }

        [Fact]
        public void DetailModelToStageEntity()
        {

            // Arrange

            var bandEntity = new BandEntity()
            {
                Name = "Kabat",
                Genre = "Rock",
                CountryOfOrigin = "Czech Republic",
                BandDescription = "Kdyz se u nas chlapi poperou",
                ProgramDescription = "They'll play for 1 hour.",
                BandMembers =
                {
                    new BandMemberEntity()
                    {
                        HeadMember = true,
                        Name = "Josef Vojtek",
                        BirthDate = new DateTime(1972, 11, 5)
                    },
                    new BandMemberEntity()
                    {
                        HeadMember = false,
                        Name = "Tomas Krulich",
                        BirthDate = new DateTime(1977, 1, 5)
                    }
                }
            };

            var stageDetailModel = new StageDetailModel()
            {
                Name = "name",
                StageDescription = "description",
                ImageUrl = "https://upload.wikimedia.org/wikipedia/commons/8/8d/Green_Stage_at_Southside_Festival_2019.jpg",
                Performances = new List<PerformanceListModel>()
                {
                    new ()
                    {
                        TimeStart = new DateTime(2021,05,21,14,0,0),
                        TimeEnd = new DateTime(2021,05,21,14,30,0),
                        BandId = bandEntity.Id
                    },
                    new ()
                    {
                        TimeStart = new DateTime(2021,05,22,14,0,0),
                        TimeEnd = new DateTime(2021,05,22,14,30,0),
                        BandId = bandEntity.Id
                    },
                }
            };


            // Act
            var stageEntity = _mapper.Map(stageDetailModel, _entityFactory);


            Assert.Equal(stageDetailModel.Id,stageEntity.Id);
            Assert.Equal(stageDetailModel.Name, stageEntity.Name);
            Assert.Equal(stageDetailModel.StageDescription, stageEntity.StageDescription);

        }

    }
}