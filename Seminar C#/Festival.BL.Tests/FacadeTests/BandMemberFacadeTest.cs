using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Festival.BL.Facades;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.DAL;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Internal;
using Microsoft.EntityFrameworkCore.Storage;
using Xunit;
using Xunit.Abstractions;

namespace Festival.BL.Tests.FacadeTests
{
    public class BandMemberFacadeTest
    {
        private readonly BandMemberFacade _facadeSUT;
        private readonly RepositoryBase<BandMemberEntity> _repository;
        private readonly BandMemberMapper _mapper;

        public BandMemberFacadeTest(ITestOutputHelper output)
        {
            var dbContextFactory = new DbContextInMemoryFactory(nameof(BandMemberFacadeTest));
            var dbx = dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();
            var unitOfWork = new UnitOfWork(dbx);
            _repository = new RepositoryBase<BandMemberEntity>(unitOfWork);
            _mapper = new BandMemberMapper();
            var entityFactory = new EntityFactory(dbx.ChangeTracker);
            _facadeSUT = new BandMemberFacade(unitOfWork, _repository, _mapper, entityFactory);
        }


        [Fact]
        public void NewBandMember_InsertOrUpdate_Persists()
        {

            // Arrange
            var bandMemberDetailModel = new BandMemberDetailModel()
            {
                Name = "George",
                NickName = "Jumbo Joe",
                BirthDate = new DateTime(1992, 7, 5),
                HeadMember = false,
                ImageUrl = "https://townsquare.media/site/295/files/2015/06/Dave-Grohl1-630x420.jpg?w=980&q=75"
            };

            // Act
            bandMemberDetailModel = _facadeSUT.Save(bandMemberDetailModel);

            // Assert
            Assert.NotEqual(Guid.Empty, bandMemberDetailModel.Id);

            var entityFromDb = _repository.GetById(bandMemberDetailModel.Id);
            Assert.Equal(bandMemberDetailModel, _mapper.Map(entityFromDb));

        }

        [Fact]
        public void DeleteBandMember_Persists()
        {
            // Arrange
            var bandMemberDetailModel = new BandMemberDetailModel()
            {
                Name = "George",
                NickName = "Jumbo Joe",
                BirthDate = new DateTime(1992, 7, 5),
                HeadMember = false,
                ImageUrl = "https://townsquare.media/site/295/files/2015/06/Dave-Grohl1-630x420.jpg?w=980&q=75"
            };

            
            bandMemberDetailModel = _facadeSUT.Save(bandMemberDetailModel);

            // Act
            _facadeSUT.Delete(bandMemberDetailModel.Id);

            // Assert
            Assert.Null(_facadeSUT.GetById(bandMemberDetailModel.Id));
        }
    }
}

