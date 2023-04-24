using System.Collections.Generic;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Xunit;

namespace Festival.DAL.Tests
{
    public class RepositoryTests
    {
        private readonly RepositoryBase<StageEntity> _repositorySUT;
        private readonly UnitOfWork.UnitOfWork _unitOfWork;

        public RepositoryTests()
        {
            var dbxFactory = new DbContextInMemoryFactory(nameof(RepositoryTests));
            var dbx = dbxFactory.CreateDbContext();
            dbx.Database.EnsureCreated();
            _unitOfWork = new UnitOfWork.UnitOfWork(dbx);
            _repositorySUT = new RepositoryBase<StageEntity>(_unitOfWork);
        }

        [Fact]
        public void NewEntity_InsertOrUpdate_GetById()
        {
            // Arrange
            var stageEntity = new StageEntity()
            {
                Name = "stage1",
                StageDescription = "desc",
                Performances = new List<PerformanceEntity>()
            };

            // Act
            stageEntity = _repositorySUT.InsertOrUpdate(stageEntity);
            _unitOfWork.Commit();
            var retStage = _repositorySUT.GetById(stageEntity.Id);

            // Assert
            Assert.Equal(stageEntity, retStage);
        }

        [Fact]
        public void DeleteEntity_DeleteById()
        {
            // Arrange
            var stageEntity = new StageEntity()
            {
                Name = "stage1",
                StageDescription = "desc",
                Performances = new List<PerformanceEntity>()
            };

            // Act
            stageEntity = _repositorySUT.InsertOrUpdate(stageEntity);
            _unitOfWork.Commit();
            _repositorySUT.DeleteById(stageEntity.Id);
            _unitOfWork.Commit();

            // Assert
            Assert.Null(_repositorySUT.GetById(stageEntity.Id));
        }

        [Fact]
        public void GetEntities_GetAllList()
        {
            // Arrange
            var stage1 = new StageEntity()
            {
                Name = "stage1",
                StageDescription = "desc1",
                Performances = new List<PerformanceEntity>()
            };
            var stage2 = new StageEntity()
            {
                Name = "stage2",
                StageDescription = "desc2",
                Performances = new List<PerformanceEntity>()
            };

            // Act
            stage1 = _repositorySUT.InsertOrUpdate(stage1);
            stage2 = _repositorySUT.InsertOrUpdate(stage2);
            _unitOfWork.Commit();
            var stageList = _repositorySUT.GetAll();

            // Assert
            Assert.NotNull(stageList);
            Assert.NotEmpty(stageList);
        }
    }
}
