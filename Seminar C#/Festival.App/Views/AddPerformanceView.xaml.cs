using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Controls;
using Festival.App.ViewModels;

namespace Festival.App.Views
{
    /// <summary>
    /// Interaction logic for AddPerformanceView.xaml
    /// </summary>
    public partial class AddPerformanceView : UserControl
    {
        public AddPerformanceView(AddPerformanceViewModel addPerformanceViewModel)
        {
            InitializeComponent();
            DataContext = addPerformanceViewModel;
        }
    }
}
