using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Festival.BL.Facades;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Xunit;

namespace Festival.BL.Tests.FacadeTests
{
   public class PerformanceFacadeTest
   {

       private readonly PerformanceFacade _facadeSUT;
       private readonly RepositoryBase<PerformanceEntity> _repository;
       private readonly PerformanceMapper _mapper;


       public PerformanceFacadeTest()
       {
           var dbContextFactory = new DbContextInMemoryFactory(nameof(PerformanceFacadeTest));
           var dbx = dbContextFactory.CreateDbContext();
           dbx.Database.EnsureCreated();
           var unitOfWork = new UnitOfWork(dbx);
           _repository = new RepositoryBase<PerformanceEntity>(unitOfWork);
           _mapper = new PerformanceMapper();
           var entityFactory = new EntityFactory(dbx.ChangeTracker);
           _facadeSUT = new PerformanceFacade(unitOfWork, _repository, _mapper, entityFactory);
       }

       [Fact]
       public void NewPerformance_InsertOrUpdateWithBandAndStage_Persists()
       {
           // Arrange
          var band = new BandEntity()
           {
               Name = "Meat Loafers",
               Genre = "Chill Rock",
               CountryOfOrigin = "Ireland",
               BandDescription = "Esist nice wettr innit?",
               ProgramDescription = "What?",
               ImageUrl = "https://resize.indiatvnews.com/en/resize/newbucket/715_-/2016/07/musocal-1467633431.jpg",
               BandMembers =
               {
                   new()
                   {
                       Name = "Member A"
                   },
                   new()
                   {
                       Name = "Member B"
                   }
               }
           };

          var stage =  new StageEntity()
           {
               Name = "Stage1",
               StageDescription = "big one"
           };


           var performanceDetailModel = new PerformanceDetailModel()
           {
                Stage = new()
                {
                    Id = stage.Id,
                    Name = stage.Name
                },
                Band = new()
                {
                    Id = band.Id,
                    Name = band.Name,
                    ImageUrl = band.ImageUrl
                },
                TimeStart = new DateTime(2021,12,21,14,0,0),
                TimeEnd = new DateTime(2021, 12, 21, 14, 30, 0)

           };

           // Act
           performanceDetailModel = _facadeSUT.Save(performanceDetailModel);

           // Assert
           Assert.NotEqual(Guid.Empty, performanceDetailModel.Id);

           var entityFromDb = _repository.GetById(performanceDetailModel.Id);
           Assert.Equal(performanceDetailModel, _mapper.Map(entityFromDb));

       }

       [Fact]
       public void NewPerformance_InsertOrUpdateWithoutBandAndStage_Persists()
       {
           // Arrange

           var performanceDetailModel = new PerformanceDetailModel()
           {
               Stage = new StageListModel(),
               Band = new BandListModel(),
               TimeStart = new DateTime(2021, 7, 2, 20, 0, 0),
               TimeEnd = new DateTime(2021, 7, 2, 22, 0, 0)

           };

           // Act
           performanceDetailModel = _facadeSUT.Save(performanceDetailModel);

           // Assert
           Assert.NotEqual(Guid.Empty, performanceDetailModel.Id);
           var entityFromDb = _repository.GetById(performanceDetailModel.Id);
           Assert.Equal(performanceDetailModel, _mapper.Map(entityFromDb));
       }

        [Fact]
       public void NewPerformance_InsertTimeCollision()
       {

           // Arrange
           var band = new BandEntity()
           {
               Name = "Meat Loafers",
               Genre = "Chill Rock",
               CountryOfOrigin = "Ireland",
               BandDescription = "Esist nice wettr innit?",
               ProgramDescription = "What?",
               ImageUrl = "https://resize.indiatvnews.com/en/resize/newbucket/715_-/2016/07/musocal-1467633431.jpg",
               BandMembers =
               {
                   new()
                   {
                       Name = "Member A"
                   },
                   new()
                   {
                       Name = "Member B"
                   }
               }
           };

           var stage = new StageEntity()
           {
               Name = "Stage1",
               StageDescription = "big one"
           };


           var performanceDetailModel1 = new PerformanceDetailModel()
           {
               Stage = new()
               {
                   Id = stage.Id,
                   Name = stage.Name
               },
               Band = new()
               {
                   Id = band.Id,
                   Name = band.Name,
                   ImageUrl = band.ImageUrl
               },
               TimeStart = new DateTime(2021, 5, 21, 14, 0, 0),
               TimeEnd = new DateTime(2021, 5, 21, 14, 30, 0)

           };

           var performanceDetailModel2 = new PerformanceDetailModel()
           {
               Stage = new()
               {
                   Id = stage.Id,
                   Name = stage.Name
               },
               Band = new()
               {
                   Id = band.Id,
                   Name = band.Name,
                   ImageUrl = band.ImageUrl
               },
               TimeStart = new DateTime(2021, 5, 21, 14, 00, 0),
               TimeEnd = new DateTime(2021, 5, 21, 14, 45, 0)

           };

            // Act
            performanceDetailModel1 = _facadeSUT.Save(performanceDetailModel1);
            performanceDetailModel2 = _facadeSUT.Save(performanceDetailModel2);

            // Assert
            Assert.NotEqual(Guid.Empty, performanceDetailModel1.Id);
            Assert.NotNull(performanceDetailModel2);
            var entityFromDb = _repository.GetById(performanceDetailModel1.Id);
            Assert.Equal(performanceDetailModel1, _mapper.Map(entityFromDb));
            var performances = _repository.GetAll();
            Assert.Equal(2, performances.Count());

       }

       [Fact]
       public void DeletePerformance_Persists()
       {

           // Arrange
           var band = new BandEntity()
           {
               Name = "Meat Loafers",
               Genre = "Chill Rock",
               CountryOfOrigin = "Ireland",
               BandDescription = "Esist nice wettr innit?",
               ProgramDescription = "What?",
               ImageUrl = "https://resize.indiatvnews.com/en/resize/newbucket/715_-/2016/07/musocal-1467633431.jpg",
               BandMembers =
               {
                   new()
                   {
                       Name = "Member A"
                   },
                   new()
                   {
                       Name = "Member B"
                   }
               }
           };

           var stage = new StageEntity()
           {
               Name = "Stage1",
               StageDescription = "big one"
           };


           var performanceDetailModel = new PerformanceDetailModel()
           {
               Stage = new()
               {
                   Id = stage.Id,
                   Name = stage.Name
               },
               Band = new()
               {
                   Id = band.Id,
                   Name = band.Name,
                   ImageUrl = band.ImageUrl
               },
               TimeStart = new DateTime(2022, 5, 21, 14, 0, 0),
               TimeEnd = new DateTime(2022, 5, 21, 14, 30, 0)

           };

            
            performanceDetailModel = _facadeSUT.Save(performanceDetailModel);

            // Act
            _facadeSUT.Delete(performanceDetailModel.Id);
            
            // Assert
            var deleted_entity = _facadeSUT.GetById(performanceDetailModel.Id);
            Assert.Null(deleted_entity);
       }
   }
}
