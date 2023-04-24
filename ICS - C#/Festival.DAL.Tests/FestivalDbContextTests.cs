using System;
using System.Linq;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace Festival.DAL.Tests
{
    public class FestivalDbContextTests : IDisposable
    {
        private readonly DbContextInMemoryFactory _dbContextFactory;
        private readonly FestivalDbContext _dbContextSUT;

        public FestivalDbContextTests()
        {
            _dbContextFactory = new DbContextInMemoryFactory(nameof(FestivalDbContextTests));
            _dbContextSUT = _dbContextFactory.CreateDbContext();
            _dbContextSUT.Database.EnsureCreated();
        }

        [Fact]
        public void AddBandMember()
        {
            var bandMember = new BandMemberEntity()
            {
                Name = "Pete Smith",
                NickName = "Jumbo Joe",
                BirthDate = new DateTime(1982, 1, 5),
                HeadMember = false,
                ImageUrl = "https://pbs.twimg.com/profile_images/948606462187761670/JxE_rHL3.jpg"
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.BandMembers.Add(bandMember);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandMemberFromDb = dbContext.BandMembers.FirstOrDefault(x => x.Id == bandMember.Id);
                Assert.Equal(bandMember, bandMemberFromDb);
            }
        }

        [Fact]
        public void UpdateBandMember()
        {
            var bandMember = new BandMemberEntity()
            {
                Name = "Pete Smith",
                NickName = "Jumbo Joe",
                HeadMember = true,
                BirthDate = new DateTime(1982, 1, 5),
                ImageUrl = "https://pbs.twimg.com/profile_images/948606462187761670/JxE_rHL3.jpg"
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.BandMembers.Add(bandMember);
                dbContext.SaveChanges();
            }
            bandMember.HeadMember = false;
            bandMember.BirthDate = new DateTime(1982, 2, 5);
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.BandMembers.Update(bandMember);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandMemberFromDb = dbContext.BandMembers.FirstOrDefault(x => x.Id == bandMember.Id);
                Assert.Equal(bandMember, bandMemberFromDb);
            }
        }

        [Fact]
        public void DeleteBandMember()
        {
            var bandMember = new BandMemberEntity()
            {
                Name = "George Eddie",
                NickName = "Jumbo Joe",
                HeadMember = true,
                BirthDate = new DateTime(1982, 1, 5),
            };
            //Act
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.BandMembers.Add(bandMember);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.BandMembers.Remove(bandMember);
                dbContext.SaveChanges();
            }
            // Assert
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandMemberFromDb = dbContext.BandMembers.FirstOrDefault(x => x.Id == bandMember.Id);
                Assert.Null(bandMemberFromDb);
            }
        }


        [Fact]
        public void AddBand()
        {
            var band = new BandEntity()
            {
                BandDescription = "American hard rock band from Los Angeles, California, formed in 1985",
                Name = "Guns N' Roses",
                Genre = "Hard rock",
                CountryOfOrigin = "U.S.",
                ProgramDescription = "The most successful hard rock band  is ready to shine at our festival after their American tour!",
                ImageUrl = "https://cdn.ontourmedia.io/gunsnroses/site_v2/animations/gnr_loop_logo_01.jpg",
                BandMembers =
                {
                    new BandMemberEntity()
                    {
                        HeadMember = true,
                        Name = "Axl Rose",
                        NickName = "Bill Rose",
                        BirthDate = new DateTime(1962, 7, 5)
                    },
                    new BandMemberEntity()
                    {
                        HeadMember = false,
                        Name = "Dizzy Reed",
                        NickName = "Dizzy",
                        BirthDate = new DateTime(1978, 2, 25)
                    }
                }
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Bands.Add(band);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandFromDatabase = dbContext.Bands
                    .Include(bandEntity => bandEntity.BandMembers)
                    .FirstOrDefault(x => x.Id == band.Id);
                Assert.Equal(band, bandFromDatabase);
            }
        }


        [Fact]
        public void EditBand()
        {
            var band = new BandEntity()
            {
                BandDescription = "American hard rock band from Los Angeles, California, formed in 1985",
                Name = "Guns N' Roses",
                Genre = "Hard rock",
                CountryOfOrigin = "U.S.",
                ProgramDescription = "The most successful hard rock band  is ready to shine at our festival after their American tour!",
                ImageUrl = "https://cdn.ontourmedia.io/gunsnroses/site_v2/animations/gnr_loop_logo_01.jpg",
                BandMembers =
                {
                    new BandMemberEntity()
                    {
                        HeadMember = true,
                        Name = "Axl Rose",
                        NickName = "Bill Rose",
                        BirthDate = new DateTime(1992, 7, 5)
                    },
                    new BandMemberEntity()
                    {
                        HeadMember = false,
                        Name = "Dizzy Reed",
                        NickName = "Dizzy",
                        BirthDate = new DateTime(1952, 7, 15)
                    }
                }
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Bands.Add(band);
                dbContext.SaveChanges();
            }
            band.BandDescription = "In September 2020, the band's Greatest Hits album was re-released";
            band.Genre = "heavy metal";

            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Bands.Update(band);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandFromDatabase = dbContext.Bands
                    .Include(bandEntity => bandEntity.BandMembers)
                    .FirstOrDefault(x => x.Id == band.Id);
                Assert.Equal(band, bandFromDatabase);
            }
        }


        [Fact]
        public void DeleteBand()
        {
            var band = new BandEntity()
            {
                BandDescription = "American hard rock band from Los Angeles, California, formed in 1985",
                Name = "Guns N' Roses",
                Genre = "Hard rock",
                ProgramDescription = "The most successful hard rock band  is ready to shine at our festival after their American tour!",
                ImageUrl = "https://cdn.ontourmedia.io/gunsnroses/site_v2/animations/gnr_loop_logo_01.jpg",
                CountryOfOrigin = "U.S.",
                BandMembers =
                {
                    new BandMemberEntity()
                    {
                        HeadMember = true,
                        Name = "Axl Rose",
                        NickName = "Bill Rose",
                        BirthDate = new DateTime(1992, 7, 5)
                    },
                    new BandMemberEntity()
                    {
                        HeadMember = false,
                        Name = "Dizzy Reed",
                        NickName = "Dizzy",
                        BirthDate = new DateTime(1972, 7, 15)
                    }
                }
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Bands.Add(band);
                dbContext.SaveChanges();
            }

            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Bands.Remove(band);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var bandFromDatabase = dbContext.Bands
                    .Include(bandEntity => bandEntity.BandMembers)
                    .FirstOrDefault(x => x.Id == band.Id);
                Assert.Null(bandFromDatabase);
            }
        }

        [Fact]
        public void AddPerformance()
        {
            var startTime = new DateTime(2021, 3, 21, 16, 40, 0);
            var endTime = new DateTime(2021, 3, 21, 19, 30, 0);
            var performance = new PerformanceEntity()
            {
                TimeStart = startTime,
                TimeEnd = endTime,
                Band = new BandEntity()
                {
                    BandDescription = "American hard rock band from Los Angeles, California, formed in 1985",
                    Name = "Guns N' Roses",
                    Genre = "Hard rock",
                    ProgramDescription = "The most successful hard rock band  is ready to shine at our festival after their American tour!",
                    ImageUrl = "https://cdn.ontourmedia.io/gunsnroses/site_v2/animations/gnr_loop_logo_01.jpg",
                    CountryOfOrigin = "U.S.",
                    BandMembers =
                    {
                        new BandMemberEntity()
                        {
                            HeadMember = true,
                            Name = "Axl Rose",
                            NickName = "Bill Rose",
                            BirthDate = new DateTime(1952, 7, 5)
                        },
                        new BandMemberEntity()
                        {
                            HeadMember = false,
                            Name = "Dizzy Reed",
                            NickName = "Dizzy",
                            BirthDate = new DateTime(1992, 7, 15)
                        }
                    }
                },
                Stage = new StageEntity()
                {
                    Name = "Stage C",
                    StageDescription = "Small Stage for new bands!",
                },
            };
            performance.Band.Performances.Add(performance);
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Performances.Add(performance);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                  var performanceFromDb = dbContext.Performances
                      .Include(x => x.Stage)
                      .Include(x => x.Band)
                      .ThenInclude(x => x.BandMembers)
                      .FirstOrDefault(x => x.Id == performance.Id);
                  
                Assert.Equal(performance, performanceFromDb);
            }
        }


        [Fact]
        public void UpdatePerformance()
        {
            var startTime = new DateTime(2021, 3, 21, 16, 40, 0);
            var endTime = new DateTime(2021, 3, 21, 19, 40, 0);
            var performance = new PerformanceEntity()
            {
                TimeStart = startTime,
                TimeEnd = endTime,
                Band = new BandEntity()
                {
                    BandDescription = "American hard rock band from Los Angeles, California, formed in 1985",
                    Name = "Guns N' Roses",
                    Genre = "Hard rock",
                    ProgramDescription = "The most successful hard rock band  is ready to shine at our festival after their American tour!",
                    ImageUrl = "https://cdn.ontourmedia.io/gunsnroses/site_v2/animations/gnr_loop_logo_01.jpg",
                    CountryOfOrigin = "U.S.",
                    BandMembers =
                    {
                        new BandMemberEntity()
                        {
                            HeadMember = true,
                            Name = "Axl Rose",
                            NickName = "Bill Rose",
                            BirthDate = new DateTime(1972, 1, 5)
                        },
                        new BandMemberEntity()
                        {
                            HeadMember = false,
                            Name = "Dizzy Reed",
                            NickName = "Dizzy",
                            BirthDate = new DateTime(1992, 2, 1)
                        }
                    }
                },
                Stage = new StageEntity()
                {
                    Name = "Stage C",
                    StageDescription = "Small Stage for new bands!",
                },
            };
            performance.Band.Performances.Add(performance);
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Performances.Add(performance);
                dbContext.SaveChanges();
            }
            // Changing stage and time of performance
            performance.Stage.Name = "Stage B";
            performance.Stage.StageDescription = "Backup Stage for headliners!";
            performance.TimeStart = new DateTime(2021, 3, 22, 12, 40, 0);
            performance.TimeStart = new DateTime(2021, 3, 22, 14, 10, 30);

            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Performances.Update(performance);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var performanceFromDb = dbContext.Performances
                    .Include(x => x.Stage)
                    .Include(x => x.Band)
                    .ThenInclude(x => x.BandMembers)
                    .FirstOrDefault(x => x.Id == performance.Id);
                Assert.Equal(performance, performanceFromDb);
            }
        }

        [Fact]
        public void DeletePerformance()
        {
            // Arrange
            var startTime = new DateTime(2021, 3, 21, 16, 40, 0);
            var endTime = new DateTime(2021, 3, 21, 19, 40, 0);

            var performance = new PerformanceEntity()
            {
                TimeStart = startTime,
                TimeEnd = endTime,
                Band = new BandEntity()
                {
                    BandDescription = "Slovak pop-rock music band which was founded in 1985!",
                    Name = "Elan",
                    ImageUrl = "https://www.elan.cz/img/elan.jpg",
                    Genre = "Pop rock",
                    CountryOfOrigin = "Slovakia",
                    BandMembers =
                    {
                        new BandMemberEntity()
                        {
                            HeadMember = true,
                            Name = "Jozef Raz",
                            NickName = "Big Joseph",
                            BirthDate = new DateTime(1977, 7, 13)
                        },
                        new BandMemberEntity()
                        {
                            HeadMember = false,
                            Name = "Jan Balaz",
                            NickName = "Guitarhero",
                            BirthDate = new DateTime(1947, 7, 7)
                        }
                    }
                },
                Stage = new StageEntity()
                {
                    Name = "Stage D",
                    StageDescription = "Friendly stage for popular national groups!",
                },
            };
            performance.Band.Performances.Add(performance);
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Performances.Add(performance);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Performances.Remove(performance);
                dbContext.SaveChanges();
            }
            using (var context = _dbContextFactory.CreateDbContext())
            {
                var performanceFromDb = context.Performances
                    .Include(x => x.Stage)
                    .Include(x => x.Band)
                    .ThenInclude(x => x.BandMembers)
                    .FirstOrDefault(x => x.Id == performance.Id);
                Assert.Null(performanceFromDb);
            }
        }


        [Fact]
        public void AddStage()
        { 
            var stage = new StageEntity()
            {
                Name = "Main Stage A1",
                StageDescription = "Main stage for the biggest shows!",
                ImageUrl = "https://www.exitfest.org/wp-content/uploads/2018/03/04_Explosive-Stage.jpg"
            };
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                dbContext.Stages.Add(stage);
                dbContext.SaveChanges();
            }
            using (var dbContext = _dbContextFactory.CreateDbContext())
            {
                var stageFromDb = dbContext.Stages.FirstOrDefault(x => x.Id == stage.Id);
                Assert.Equal(stage, stageFromDb);
            }
        }

        [Fact]
        public void UpdateStage()
        {
            var stage = new StageEntity()
            {
                Name = "Main Stage A1",
                StageDescription = "Main stage for the biggest shows!",
                ImageUrl = "https://www.exitfest.org/wp-content/uploads/2018/03/04_Explosive-Stage.jpg"
            };

            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                dbcContext.Stages.Add(stage);
                dbcContext.SaveChanges();
            }
            stage.StageDescription = "Main stage is currently cancelled due to weather conditions!";

            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                dbcContext.Stages.Update(stage);
                dbcContext.SaveChanges();
            }

            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                var stageFromDb = dbcContext.Stages.FirstOrDefault(x => x.Id == stage.Id);
                Assert.Equal(stage, stageFromDb);
            }
        }

        [Fact]
        void DeleteStage()
        {
            var stage = new StageEntity()
            {
                Name = "Energy tent X",
                StageDescription = "Best place for after parties!",
                ImageUrl = "https://www.exitfest.org/wp-content/uploads/2018/03/04_Explosive-Stage.jpg"
            };

            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                dbcContext.Stages.Add(stage);
                dbcContext.SaveChanges();
            }
            
            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                dbcContext.Stages.Remove(stage);
                dbcContext.SaveChanges();
            }

            using (var dbcContext = _dbContextFactory.CreateDbContext())
            {
                var stageFromDb = dbcContext.Stages.FirstOrDefault(x => x.Id == stage.Id);
                Assert.Null(stageFromDb);
            }
        }

        public void Dispose() => _dbContextSUT.Dispose();
    }
}