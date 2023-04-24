using hw04.Car;
using hw04.TrackPoints;
namespace hw04.Race;

public class Race
{
    private Track _track;
    private int _numberOfLaps;
    private IEnumerable<RaceCar> _raceCars;
    private Simulation _simulation;
    private Dictionary<string, List<Lap>> _lapsData = new Dictionary<string, List<Lap>>();
    private Dictionary<string, TimeSpan> _deficitsToLeader = new Dictionary<string, TimeSpan>();
    private int _leaderFinishedLap;
    private string _currentLeader;

    public Race(IEnumerable<RaceCar> cars, Track track, int numberOfLaps, Simulation simulation)
    {
        _track = track;
        _numberOfLaps = numberOfLaps;
        _raceCars = cars;
        _simulation = simulation;
        foreach (var car in cars)
        {
            _lapsData[car.Driver] = new List<Lap>();
        }
        _leaderFinishedLap = 0;
    }

    private TimeSpan CalculateTotalRacingTime(string driver)
    {
        var totalTime = _lapsData[driver].Aggregate(TimeSpan.Zero, (acc, lap) => acc + lap.LapTime);
        return totalTime;
    }

    private TimeSpan CalculateDeficitToLeader(string driver, int toLap)
    {
        var leaderTotalTime = _lapsData[_currentLeader].Where(x => x.Number <= toLap)
            .Aggregate(TimeSpan.Zero, (acc, lap) => acc + lap.LapTime);
        var driverTotalTime = _lapsData[driver].Aggregate(TimeSpan.Zero, (acc, lap) => acc + lap.LapTime);
        return driverTotalTime - leaderTotalTime;
    }

    private void ManageDataAfterLap(Lap finishedLap)
    {
        //ulozenie vysledku kola 
        _lapsData[finishedLap.RaceCar.Driver].Add(finishedLap);
        if (finishedLap.Number > _leaderFinishedLap)
        {
            _leaderFinishedLap = finishedLap.Number;
            _currentLeader = finishedLap.RaceCar.Driver;
            var totalTime = CalculateTotalRacingTime(finishedLap.RaceCar.Driver);
            Console.WriteLine($"Lap: {finishedLap.Number}");
            Console.WriteLine($"{finishedLap.RaceCar.Driver + ":",-10}  {totalTime:mm\\:ss\\.ff}");
            _deficitsToLeader[finishedLap.RaceCar.Driver] = TimeSpan.Zero;
        }
        else if (finishedLap.Number == _leaderFinishedLap)
        {
            _deficitsToLeader[finishedLap.RaceCar.Driver] = CalculateDeficitToLeader(finishedLap.RaceCar.Driver, _leaderFinishedLap);
            Console.WriteLine($"{finishedLap.RaceCar.Driver + ":",-11} +{_deficitsToLeader[finishedLap.RaceCar.Driver]:mm\\:ss\\.ff}");
        }
        else
        {
            // zavodnik o kolo a viac pomalsi
            _deficitsToLeader[finishedLap.RaceCar.Driver] = CalculateDeficitToLeader(finishedLap.RaceCar.Driver, finishedLap.Number);
            Console.WriteLine($"{finishedLap.RaceCar.Driver + ":",-11} +LAPPED");
        }
    }

    public async Task StartAsync()
    {
        var startLapBarrier = new Barrier(_lapsData.Count());
        var lapTasks = new Queue<Task<Lap>>();
        foreach (var raceCar in _raceCars)
        {
            _deficitsToLeader[raceCar.Driver] = TimeSpan.Zero;
            // úlohy pre prvé kolo - použitie bariéry na zaruèenie súèasného spustenia pre všetky formule
            var task = Task.Run(async () =>
            {
                startLapBarrier.SignalAndWait();
                TaskTimeTracker.Start();
                var lapResult = await _simulation.SimulateLap(raceCar, 1);
                lapResult.AddOverhead(TaskTimeTracker.Stop());
                return lapResult;
            });
            lapTasks.Enqueue(task);
        }

        while (lapTasks.Any())
        {
            // ziskanie vysledkov dokoncenia kola a vytvorenie novych taskov pre simulovanie dalsich kol
            var finishedTask = lapTasks.Dequeue();
            var finishedTaskResult = await finishedTask;
            ManageDataAfterLap(finishedTaskResult);
            if (_leaderFinishedLap < _numberOfLaps + 1 && finishedTaskResult.Number < _numberOfLaps)
            {
                var nextLapTask = Task.Run(async () =>
                {
                    TaskTimeTracker.Start();
                    var lapResult = await _simulation.SimulateLap(finishedTaskResult.RaceCar, finishedTaskResult.Number + 1);
                    lapResult.AddOverhead(TaskTimeTracker.Stop());
                    return lapResult;
                });
                lapTasks.Enqueue(nextLapTask);
            }
        }
    }

    public Dictionary<string, List<Lap>> GetLapsData()
    {
        return _lapsData;
    }
}