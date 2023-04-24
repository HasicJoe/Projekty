using Nemesis.Essentials.Design;
using System.Collections.Generic;
using System.Linq;

namespace Festival.BL.Mappers
{
    public static class ValueCollectionExtensions
    {
        public static ValueCollection<T> ToValueCollection<T>(this IEnumerable<T> enumerable)
        {
            var collection = enumerable switch
            {
                IList<T> list => list,
                _ => enumerable.ToList()
            };
            return new ValueCollection<T>(collection);
        }
        public static ValueCollection<T> ToValueCollection<T>(this IEnumerable<T> enumerable, IEqualityComparer<T> comparer)
        {
            var collection = enumerable switch
            {
                IList<T> list => list,
                _ => enumerable.ToList()
            };
            return new ValueCollection<T>(collection, comparer);
        }
    }
}
