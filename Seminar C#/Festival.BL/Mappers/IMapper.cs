using Festival.BL.Models;
using Festival.DAL.Entities;
using Festival.DAL.Factories;
using System.Collections.Generic;
using System.Linq;


namespace Festival.BL.Mappers
{
    public interface IMapper<TEntity, out TListModel, TDetailModel>
        where TEntity : class, IEntity, new()
        where TListModel : IModel, new()
        where TDetailModel : IModel, new()
    {
        IEnumerable<TListModel> Map(IQueryable<TEntity> entities);
        TDetailModel Map(TEntity entity);
        TEntity Map(TDetailModel detailModel, IEntityFactory entityFactory);
    }
}
