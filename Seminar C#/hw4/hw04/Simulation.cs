using System.Diagnostics;
using hw04.Car;
using hw04.Race;
using hw04.TrackPoints;

namespace hw04;

public class Simulation
{
    public Track Track {get; set; }
    public Simulation(Track track)
    {
        Track = track;
    }

    public async Task<Race.Race> SimulateRaceAsync(List<RaceCar> cars, int numberOfLaps)
    {
        Race.Race race = new Race.Race(cars, Track, numberOfLaps, this);
        await race.StartAsync();
        return race;
    }

    public async Task<List<Lap>> SimulateLapsAsync(RaceCar car, int numberOfLaps)
    {
        Console.WriteLine($"{car.Driver} started simulation of selected compound... [this simulation takes some time]");
        var lapResults = new List<Lap>();
        for (int lap = 1; lap < numberOfLaps + 1; lap++)
        {
            lapResults.Add(await Task.Run(async () =>
            {
                TaskTimeTracker.Start();
                var completedLap = await SimulateLap(car, lap);
                completedLap.AddOverhead(TaskTimeTracker.Stop());
                return completedLap;
            }));
        }
        return await Task.FromResult(lapResults);
    }

    public async Task<Lap> SimulateLap(RaceCar car, int currentLapIndex)
    {
        var trackPoints = Track.GetLap(car);
        List<TrackPointPass> passResults = new List<TrackPointPass>();
        TimeSpan lapElapsedTime = TimeSpan.Zero;
        foreach (var p in trackPoints)
        {
            var pointPass = await p.PassAsync(car, currentLapIndex);
            passResults.Add(pointPass.UpdateExecution(TaskTimeTracker.Stop()));
            lapElapsedTime += pointPass.ExecutionTime + pointPass.DrivingTime + pointPass.WaitingTime;
            TaskTimeTracker.Start();
        }
        var lap = new Lap(car, currentLapIndex, lapElapsedTime, passResults);
        car.AddTireDegradationAfterLap();
        return lap;
    }

}