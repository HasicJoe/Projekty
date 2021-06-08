using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using System;
using System.Windows;
using Festival.App.Services;
using Festival.App.ViewModels;
using Festival.App.Views;
using Festival.App.Services.MessageDialog;
using Festival.DAL.Factories;
using Festival.BL.Facades;
using Festival.BL.Mappers;
using Festival.DAL.UnitOfWork;
using Festival.DAL.Repositories;
using Festival.DAL.Entities;
using Microsoft.EntityFrameworkCore;


namespace Festival.App
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public static IHost host;
        public App()
        {
            host = Host.CreateDefaultBuilder()
            .ConfigureAppConfiguration(ConfigureAppConfiguration)
                .ConfigureServices((context, services) => { ConfigureServices(context.Configuration, services); })
            .Build();
        }

        private static void ConfigureAppConfiguration(HostBuilderContext context, IConfigurationBuilder builder) 
        {
            builder.AddJsonFile(@"appsettings.json", false, true);
        }

        private static void ConfigureServices(IConfiguration configuration, 
            IServiceCollection services)
        {
            /// Views
            services.AddSingleton<MainWindow>();
            services.AddTransient<ProgramListView>();
            services.AddTransient<PerformancesListView>();
            services.AddTransient<BandsListView>();
            services.AddTransient<AddEditBandView>();
            services.AddTransient<HomeView>();
            services.AddTransient<AddPerformanceView>();
            services.AddTransient<StageListView>();
            services.AddTransient<AddEditStageView>();
            services.AddTransient<BandDetailView>();

            /// Navigations
            services.AddSingleton<INavigationService, NavigationService>();
            services.AddSingleton<IMessageDialogService, MessageDialogService>();
            
            /// Mediator
            services.AddSingleton<IMediator, Mediator>();

            /// View models
            services.AddSingleton<AddEditBandViewModel>();
            services.AddTransient<AddPerformanceViewModel>();
            services.AddTransient<BandDetailViewModel>();
            services.AddTransient<BandsViewModel>();
            services.AddTransient<HomeViewModel>();
            services.AddTransient<MainViewModel>();
            services.AddTransient<PerformancesViewModel>();
            services.AddTransient<ProgramViewModel>();
            services.AddTransient<StageViewModel>();
            services.AddSingleton<AddEditStageViewModel>();

            /// Facades, creating dbcontext
            var dbContextFactory = new SqlServerDbContextFactory(configuration.GetConnectionString("DefaultConnection"));
            var dbx = dbContextFactory.CreateDbContext();
            dbx.Database.EnsureCreated();

            var unitOfWork = new UnitOfWork(dbx);

            var stageRepo = new RepositoryBase<StageEntity>(unitOfWork);
            services.AddSingleton<StageFacade>(provider => new StageFacade(unitOfWork, stageRepo, new StageMapper(), new EntityFactory(dbx.ChangeTracker)));
            
            var bandMemberRepo = new RepositoryBase<BandMemberEntity>(unitOfWork);
            services.AddSingleton<BandMemberFacade>(provider => new BandMemberFacade(unitOfWork, bandMemberRepo, new BandMemberMapper(), new EntityFactory(dbx.ChangeTracker)));

            var bandRepo = new RepositoryBase<BandEntity>(unitOfWork);
            services.AddSingleton<BandFacade>(provider => new BandFacade(unitOfWork, bandRepo, new BandMapper(), new EntityFactory(dbx.ChangeTracker)));

            var performanceRepo = new RepositoryBase<PerformanceEntity>(unitOfWork);
            services.AddSingleton<PerformanceFacade>(provider => new PerformanceFacade(unitOfWork, performanceRepo, new PerformanceMapper(), new EntityFactory(dbx.ChangeTracker)));
        }
        protected override async void OnStartup(StartupEventArgs e)
        {
            await host.StartAsync();

            var mainWindow = host.Services.GetRequiredService<MainWindow>();
            
            var navigationService = host.Services.GetRequiredService<INavigationService>();
            navigationService.ContentPanel = mainWindow.ContentPanel;

            mainWindow.Show();
            base.OnStartup(e);
        }

        protected override async void OnExit(ExitEventArgs e)
        {
            using (host)
            {
                await host.StopAsync(TimeSpan.FromSeconds(5));
            }
            base.OnExit(e);
        }
    }
}