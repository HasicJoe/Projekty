using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Festival.BL.Facades;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL;
using Festival.DAL.Entities;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace Festival.BL.Tests.FacadeTests
{
    public class StageFacadeTests
    {
        private readonly StageFacade _facadeSUT;
        private readonly RepositoryBase<StageEntity> _repository;
        private readonly StageMapper _mapper;

        public StageFacadeTests()
        {
            var dbContextFactory = new DbContextInMemoryFactory(nameof(StageFacadeTests));
            var dbx = dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();
            
            var unitOfWork = new UnitOfWork(dbx);
            _repository = new RepositoryBase<StageEntity>(unitOfWork);
            _mapper = new StageMapper();
           
            var entityFactory = new EntityFactory(dbx.ChangeTracker);
            _facadeSUT = new StageFacade(unitOfWork, _repository, _mapper, entityFactory);
        }

        [Fact]
        public void NewStage_InsertOrUpdateWithPerformances_Persisted()
        {
            // Arrange
            var stageDetailModel = new StageDetailModel()
            {
                Name = "Big Thick Stage",
                StageDescription = "It's Massive",
                Performances = new List<PerformanceListModel>
                {
                    new PerformanceListModel
                    {
                        TimeStart = new DateTime(2021,9,6,14,30,0),
                        TimeEnd = new DateTime(2021,9,6,15,30,0),
                    }
                }
            };

            // Act
            stageDetailModel = _facadeSUT.Save(stageDetailModel);
            var entityFromDb = _repository.GetById(stageDetailModel.Id);

            // Assert
            Assert.NotEqual(Guid.Empty, stageDetailModel.Id);
            Assert.Equal(stageDetailModel, _mapper.Map(entityFromDb));

        }

        [Fact]
        public void NewStage_InsertOrUpdateWithoutPerformances_Throws()
        {
            // Arrange
            var stageDetailModel = new StageDetailModel()
            {
                Name = "O2 Arena",
                StageDescription = "Nice",
                Performances = { },
            };

            // Act, Assert
            Assert.Throws<ArgumentNullException>(() => _facadeSUT.Save(stageDetailModel));
        }

        [Fact]
        public void DeleteStage_Persists()
        {
            // Arrange
            var stageDetailModel = new StageDetailModel()
            {
                Id = new Guid(),
                Name = "Small Little Stage",
                StageDescription = "It's Miniature",
                Performances = new List<PerformanceListModel>
                {
                    new PerformanceListModel
                    {
                        TimeStart = new DateTime(2021,9,6,14,30,0),
                        TimeEnd = new DateTime(2021,9,6,15,30,0)
                    }
                }
            };
            stageDetailModel = _facadeSUT.Save(stageDetailModel);

            // Act
            _facadeSUT.Delete(stageDetailModel.Id);

            // Assert
            Assert.Null(_facadeSUT.GetById(stageDetailModel.Id));
        }
    }
}