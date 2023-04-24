using System;
using System.Collections.Generic;
using System.Linq;
using Festival.BL.Mappers;
using Festival.BL.Models.DetailModels;
using Festival.BL.Models.ListModels;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore.ChangeTracking;
using Xunit;

namespace Festival.BL.Tests.MapperTests
{
    public class BandMapperTests
    {
        private readonly BandMapper _bandMapper;
        private static readonly ChangeTracker changeTracker;
        private readonly IEntityFactory _entityFactory;

        public BandMapperTests()
        {
            _bandMapper = new BandMapper();
            _entityFactory = new EntityFactory(changeTracker);
        }

        [Fact]
        public void MapBand_EntityToListModel()
        {
            // Arrange
            var bandEntity = new BandEntity {Id = new Guid(), Name = "Horkyze Slize"};

            // Act
            var bandListModel = BandMapper.MapListModel(bandEntity);

            // Assert
            Assert.Equal(bandEntity.Id, bandListModel.Id);
            Assert.Equal(bandEntity.Name, bandListModel.Name);
        }

        [Fact]
        public void MapBand_EntityToDetailModel()
        {
            // Arrange
            var bandEntity = new BandEntity
            {
                Id = new Guid(),
                Name = "Kabat",
                Genre = "Rock",
                CountryOfOrigin = "Czech Republic",
                BandDescription = "Kdyz se u nas chlapi poperou",
                ProgramDescription = "They'll play for 1 hour.",
                BandMembers =
                {
                    new BandMemberEntity
                    {
                        HeadMember = true,
                        Name = "Josef Vojtek",
                        BirthDate = new DateTime(1972, 11, 5)
                    },
                    new BandMemberEntity
                    {
                        HeadMember = false,
                        Name = "Tomas Krulich",
                        BirthDate = new DateTime(1978, 11, 15)
                    }
                },
                Performances =
                {
                    new PerformanceEntity
                    {
                        TimeStart = new DateTime(2021, 4, 7, 16, 30, 0),
                        TimeEnd = new DateTime(2021, 4, 7, 17, 25, 0)
                    }
                }

            };

            // Act
            var bandDetailModel = _bandMapper.Map(bandEntity);

            // Assert
            Assert.Equal(bandEntity.Id, bandDetailModel.Id);
            Assert.Equal(bandEntity.Name, bandDetailModel.Name);
            Assert.Equal(bandEntity.Genre, bandDetailModel.Genre);
            Assert.Equal(bandEntity.CountryOfOrigin, bandDetailModel.CountryOfOrigin);
            Assert.Equal(bandEntity.BandDescription, bandDetailModel.BandDescription);
            Assert.Equal(bandEntity.ProgramDescription, bandDetailModel.ProgramDescription);
            foreach (var merged in bandDetailModel.BandMembers.Zip(bandEntity.BandMembers, Tuple.Create))
            {
                Assert.Equal(merged.Item1.Id, merged.Item2.Id);
                Assert.Equal(merged.Item1.Name, merged.Item2.Name);
            }

            foreach (var merged in bandDetailModel.Performances.Zip(bandEntity.Performances, Tuple.Create))
            {
                Assert.Equal(merged.Item1.TimeStart, merged.Item2.TimeStart);
                Assert.Equal(merged.Item1.TimeEnd, merged.Item2.TimeEnd);
            }
        }

        [Fact]
        public void MapBand_DetailModelToEntity()
        { 
            // Arrange
            var bandDetailModel = new BandDetailModel
            {
                Name = "Konflikt",
                Genre = "Punk-Rock",
                CountryOfOrigin = "Slovakia",
                BandDescription = "They have catchy songs.",
                ProgramDescription = "Support band for Metallica",
                BandMembers = new List<BandMemberListModel>()
                {
                    new()
                    {
                        Name = "Robert Juranyi"
                    },
                    new()
                    {
                        Name = "Kristian Kadlecik"
                    },
                    new()
                    {
                        Name = "Pavel Prokop"
                    }
                },
                Performances = new List<PerformanceListModel>()
                {
                    new()
                    {
                        TimeStart = new DateTime(2021, 5, 1, 8, 0, 0),
                        TimeEnd = new DateTime(2021, 5, 1, 8, 30, 0)
                    }
                }
            };

            // Act
            var bandEntity = _bandMapper.Map(bandDetailModel, _entityFactory);

            // Assert
            Assert.Equal(bandDetailModel.Id,bandEntity.Id);
            Assert.Equal(bandDetailModel.Name,bandEntity.Name);
            Assert.Equal(bandDetailModel.Genre, bandEntity.Genre);
            Assert.Equal(bandDetailModel.CountryOfOrigin, bandEntity.CountryOfOrigin);
            Assert.Equal(bandDetailModel.BandDescription, bandEntity.BandDescription);
            Assert.Equal(bandDetailModel.ProgramDescription, bandEntity.ProgramDescription);
            foreach (var merged in bandDetailModel.BandMembers.Zip(bandEntity.BandMembers, Tuple.Create))
            {
                Assert.Equal(merged.Item1.Id, merged.Item2.Id);
                Assert.Equal(merged.Item1.Name, merged.Item2.Name);
            }

            foreach (var merged in bandDetailModel.Performances.Zip(bandEntity.Performances, Tuple.Create))
            {
                Assert.Equal(merged.Item1.Id, merged.Item2.Id);
                Assert.Equal(merged.Item1.TimeStart, merged.Item2.TimeStart);
                Assert.Equal(merged.Item1.TimeEnd, merged.Item2.TimeEnd);
            }
        }
    }
}
