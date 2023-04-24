using System.Diagnostics;
using hw04.Car;
using hw04.Race;
using Microsoft.VisualBasic;
namespace hw04.TrackPoints;

public class Turn : ITrackPoint
{
    private static readonly TimeSpan DriveInTime = TimeSpan.FromMilliseconds(25);
    private readonly Stopwatch _stopwatch = new Stopwatch();

    public string Description { get; set; }
    public TimeSpan AverageTime { get; set; }
    private SemaphoreSlim _turnEntrySemaphore;

    public Turn(string description, TimeSpan averageTime, int carsAllowed)
    {
        Description = description;
        AverageTime = averageTime;
        _turnEntrySemaphore = new SemaphoreSlim(carsAllowed);
    }

    public async Task<TrackPointPass> PassAsync(RaceCar car, int lap)
    {
        _stopwatch.Reset();
        _stopwatch.Start();
        await _turnEntrySemaphore.WaitAsync();
        _stopwatch.Stop();
        var waitingTime = _stopwatch.Elapsed;
        var driveInTime = DriveInTime * car.TurnSpeed * car.TireStrategy[car.ActiveTireIndex].GetSpeed();
        await Task.Delay(driveInTime);
        _turnEntrySemaphore.Release();
        var turnTime = AverageTime * car.TurnSpeed * car.TireStrategy[car.ActiveTireIndex].GetSpeed();
        await Task.Delay(turnTime);
        return new TrackPointPass(this, waitingTime, driveInTime + turnTime, car.Driver, lap, TimeSpan.Zero);
    }
}