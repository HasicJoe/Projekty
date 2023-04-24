namespace Festival.App.ViewModels.Interfaces
{
    public interface IViewModel
    {
        void LoadInDesignMode();
    }

    public interface IViewModel<TViewModelParameter> : IViewModel
    {
    }
}
