using System;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using Microsoft.EntityFrameworkCore;
using Xunit;

namespace Festival.DAL.Tests
{
    public sealed class EntityStatesTest : IDisposable
    {

    private readonly FestivalDbContext _festivalDbContextSUT;

    public EntityStatesTest()
    {
        var dbContextFactory = new DbContextInMemoryFactory(nameof(EntityStatesTest));
        _festivalDbContextSUT = dbContextFactory.CreateDbContext();
        _festivalDbContextSUT.Database.EnsureCreated();
    }


    private readonly BandMemberEntity _bandMemberEntity = new()
    {
        Name = "Peter Hrivnak",
        NickName = "Kuko",
        HeadMember = true,
        BirthDate = new DateTime(1972,11,15),

    };

    [Fact]
    public void AddedStateTest()
    {
        _festivalDbContextSUT.Add(_bandMemberEntity);
        Assert.Equal(EntityState.Added, _festivalDbContextSUT.Entry(_bandMemberEntity).State);
    }

    [Fact]
    public void UnchangedStateTest()
    {
        _festivalDbContextSUT.BandMembers.Add(_bandMemberEntity);
        _festivalDbContextSUT.SaveChanges();
        Assert.Equal(EntityState.Unchanged, _festivalDbContextSUT.Entry(_bandMemberEntity).State);
    }

    [Fact]
    public void ModifiedStateTest()
    {
        var entityEntry = _festivalDbContextSUT.BandMembers.Add(_bandMemberEntity);
        _festivalDbContextSUT.SaveChanges();
        entityEntry.Entity.Name = "Jozo";
        Assert.Equal(EntityState.Modified, _festivalDbContextSUT.Entry(_bandMemberEntity).State);
    }

    [Fact]
    public void DeletedStateTest()
    {
        _festivalDbContextSUT.BandMembers.Add(_bandMemberEntity);
        _festivalDbContextSUT.SaveChanges();
        _festivalDbContextSUT.Remove(_bandMemberEntity);
        Assert.Equal(EntityState.Deleted, _festivalDbContextSUT.Entry(_bandMemberEntity).State);
    }

    [Fact]
    public void DetachedStateTest()
    {
        Assert.Equal(EntityState.Detached, _festivalDbContextSUT.Entry(_bandMemberEntity).State);
    }

        public void Dispose() => _festivalDbContextSUT?.Dispose();


    }
}
