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
using Festival.DAL.Repositories;
using Festival.DAL.UnitOfWork;
using Festival.DAL.Factories;
using Xunit;

namespace Festival.BL.Tests.FacadeTests
{
    public class BandFacadeTests
    {
        private readonly BandFacade _facadeSUT;
        private readonly RepositoryBase<BandEntity> _repository;
        private readonly BandMapper _mapper;

        public BandFacadeTests()
        {
            var dbContextFactory = new DbContextInMemoryFactory(nameof(BandFacadeTests));
            var dbx = dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();
            var unitOfWork = new UnitOfWork(dbx);
            _repository = new RepositoryBase<BandEntity>(unitOfWork);
            _mapper = new BandMapper();
            var entityFactory = new EntityFactory(dbx.ChangeTracker);
            _facadeSUT = new BandFacade(unitOfWork, _repository, _mapper, entityFactory);
        }

        [Fact]
        public void NewBand_InsertOrUpdateWithBandMembersAndPerformances_Persisted()
        {
            // Arrange
            var bandDetailModel = new BandDetailModel
            {
                Name = "Meat Loafers",
                Genre = "Chill Rock",
                CountryOfOrigin = "Ireland",
                BandDescription = "Esist nice wettr innit?",
                ProgramDescription = "What?",
                BandMembers = new List<BandMemberListModel>
                {
                    new BandMemberListModel
                    {
                        Name = "Member A"
                    },
                    new BandMemberListModel
                    {
                        Name = "Member B"
                    }
                },
                Performances = new List<PerformanceListModel>
                {
                    new PerformanceListModel
                    {
                        TimeStart = new DateTime(2021,9,6,14,30,0),
                        TimeEnd = new DateTime(2021,9,6,15,30,0)
                    }
                }
            };
            
            // Act
            bandDetailModel = _facadeSUT.Save(bandDetailModel);
            var entityFromDb = _repository.GetById(bandDetailModel.Id);

            // Assert
            Assert.NotEqual(Guid.Empty, bandDetailModel.Id);
            Assert.Equal(bandDetailModel, _mapper.Map(entityFromDb));

        }

        [Fact]
        public void NewBand_InsertOrUpdateWithoutBandMembersAndPerformances_Throws()
        {
            // Arrange
            var bandDetailModel = new BandDetailModel
            {
                Name = "Deez",
                Genre = "Jazz",
                CountryOfOrigin = "Germany",
                BandDescription = "M",
                ProgramDescription = "None",
                BandMembers = {},
                Performances = {}
            };

            // Act, Assert
            Assert.Throws<ArgumentNullException>(() => _facadeSUT.Save(bandDetailModel));
        }

        [Fact]
        public void DeleteBand_Persists()
        {
            // Arrange
            var bandDetailModel = new BandDetailModel
            {
                Name = "Big Boys",
                Genre = "Soft Rock",
                CountryOfOrigin = "Iceland",
                BandDescription = "Hot",
                ProgramDescription = "Something",
                BandMembers = new List<BandMemberListModel>
                {
                    new BandMemberListModel
                    {
                        Name = "Member A"
                    },
                    new BandMemberListModel
                    {
                        Name = "Member B"
                    }
                },
                Performances = new List<PerformanceListModel>
                {
                    new PerformanceListModel
                    {
                        TimeStart = new DateTime(2021,9,6,14,30,0),
                        TimeEnd = new DateTime(2021,9,6,15,30,0)
                    }
                }
            };

            
            bandDetailModel = _facadeSUT.Save(bandDetailModel);

            // Act
            _facadeSUT.Delete(bandDetailModel.Id);

            // Assert
            Assert.Null(_facadeSUT.GetById(bandDetailModel.Id));
        }
    }
}