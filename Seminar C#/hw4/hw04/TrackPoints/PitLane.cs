using hw04.Car;
using hw04.Race;
using System.Diagnostics;

namespace hw04.TrackPoints;

public class PitLane : ITrackPoint
{
    public string Description { get; set; }
    // kazdy team ma svoj vlastny semafor pre kontrolu, ze iba jeden monopost moze byt v pitlane
    Dictionary<string, SemaphoreSlim> _pitStopSemaphore = new Dictionary<string, SemaphoreSlim>();
    private readonly Stopwatch _stopwatch = new Stopwatch();


    public PitLane(string description, List<Team> teams)
    {
        Description = description;
        foreach (var team in teams)
        {
            _pitStopSemaphore[team.Name] = new SemaphoreSlim(1);
        }
    }


    public async Task<TrackPointPass> PassAsync(RaceCar car, int lap)
    {
        _stopwatch.Reset();
        _stopwatch.Start();
        await _pitStopSemaphore[car.Team.Name].WaitAsync();
        _stopwatch.Stop();
        var waitingTime = _stopwatch.Elapsed;
        _stopwatch.Reset();
        _stopwatch.Start();
        var leftFrontTask = car.SimulateTireChange();
        var rightFrontTask = car.SimulateTireChange();
        var leftRearTask = car.SimulateTireChange();
        var rightRearTask = car.SimulateTireChange();
        await Task.WhenAll(leftFrontTask, rightFrontTask, leftRearTask, rightRearTask);
        _stopwatch.Stop();
        var pitStopTime = _stopwatch.Elapsed;
        _pitStopSemaphore[car.Team.Name].Release();
        car.SetNextTireIndex();
        return new TrackPointPass(this, waitingTime, pitStopTime, car.Driver, lap, TimeSpan.Zero);
    }
}