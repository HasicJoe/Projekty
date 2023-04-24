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
using Xunit;

namespace Festival.BL.Tests.MapperTests
{
    public class PerformanceMapperTests
    {
        private readonly PerformanceMapper _mapper;
        private readonly EntityFactory _entityFactory;
        private static readonly ChangeTracker _changeTracker;
        private readonly CultureInfo _cultureInfo;

        public PerformanceMapperTests()
        {
            _mapper = new PerformanceMapper();
            _entityFactory = new EntityFactory(_changeTracker);
            _cultureInfo = new CultureInfo("de-DE");

        }

        [Fact]
        public void PerformanceEntityToListModel()
        {
            // Arange
            var band = new BandEntity()
            {
                Name = "BandName",
                Genre = "genre",
                CountryOfOrigin = "origin",
                BandDescription = "desc",
            };

            var stage = new StageEntity()
            {
                Name = "stage",
                StageDescription = "desc"

            };

            var performanceEntity = new PerformanceEntity()
            {
                Band = band,
                BandId = band.Id,
                Stage = stage,
                StageId = stage.Id,
                TimeStart = new DateTime(2021, 5, 1, 8, 0,0),
                TimeEnd = new DateTime(2021, 5, 1, 8, 30, 0)

            };

            // Act
            var performanceListModel = PerformanceMapper.MapListModel(performanceEntity);

            // Assert
            Assert.Equal(performanceEntity.Id, performanceListModel.Id);
            Assert.Equal(performanceEntity.BandId, performanceListModel.BandId);
            Assert.Equal(performanceEntity.StageId, performanceListModel.StageId);
            Assert.Equal(performanceEntity.TimeStart, performanceListModel.TimeStart);
            Assert.Equal(performanceEntity.TimeEnd, performanceListModel.TimeEnd);


        }

        [Fact]
        public void PerformanceEntityToDetailModel()
        {
            // Arange
            var band = new BandEntity()
            {
                Name = "BandName",

            };

            var stage = new StageEntity()
            {
                Name = "stage",
                StageDescription = "desc",
            };

            var performanceEntity = new PerformanceEntity()
            {
                Band = band,
                BandId = band.Id,
                Stage = stage,
                StageId = stage.Id,
                TimeStart = new DateTime(2021, 5, 1, 8, 0, 0),
                TimeEnd = new DateTime(2021, 5, 1, 8, 30, 0)

            };

            // Act
            var performanceDetailModel = _mapper.Map(performanceEntity);

            // Assert
            Assert.Equal(performanceEntity.Id, performanceDetailModel.Id);
            Assert.Equal(performanceEntity.BandId, performanceDetailModel.Band.Id);
            Assert.Equal(performanceEntity.StageId, performanceDetailModel.Stage.Id);
            Assert.Equal(performanceEntity.Band.Id,performanceDetailModel.Band.Id);
            Assert.Equal(performanceEntity.Band.Name, performanceDetailModel.Band.Name);
            Assert.Equal(performanceEntity.Stage.Id, performanceDetailModel.Stage.Id);
            Assert.Equal(performanceEntity.Stage.Name, performanceDetailModel.Stage.Name);
            Assert.Equal(performanceEntity.TimeStart, performanceDetailModel.TimeStart);
            Assert.Equal(performanceEntity.TimeEnd, performanceDetailModel.TimeEnd);


        }

        [Fact]
        public void PerformanceDetailModelToEntity()
        {
            // Arange
           
            var bandDetailModel = new BandListModel()
            {
                Id = new Guid(),
                Name = "BandName",
            };

            var stageDetailModelModel = new StageListModel()
            {
                Id = new Guid(),
                Name = "stage",
            };

            var performanceDetailModel= new PerformanceDetailModel()
            {

                Band = bandDetailModel,
                Stage = stageDetailModelModel,

                TimeStart = new DateTime(2021, 5, 1, 8, 0, 0),
                TimeEnd = new DateTime(2021, 5, 1, 8, 30, 0)

            };

            // Act
            var performanceEntity = _mapper.Map(performanceDetailModel, _entityFactory);

            // Assert
            Assert.Equal(performanceDetailModel.Id, performanceEntity.Id);
            Assert.Equal(performanceDetailModel.Band.Id, performanceEntity.BandId);
            Assert.Equal(performanceDetailModel.Stage.Id, performanceEntity.StageId);
            Assert.Equal(performanceDetailModel.TimeStart, performanceEntity.TimeStart);
            Assert.Equal(performanceDetailModel.TimeEnd, performanceEntity.TimeEnd);


        }


    }
}
