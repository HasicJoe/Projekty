using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows.Data;
using System.Globalization;

namespace Festival.App.Converters
{
    public class MultiDateConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            StringBuilder output = new StringBuilder();

            output.Append(((DateTime)values[0]).ToString("d", CultureInfo.CreateSpecificCulture("en-US")));
            output.Append(" ");
            output.Append(((DateTime)values[1]).ToString("t", CultureInfo.CreateSpecificCulture("en-US")));

            return output.ToString();
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
