using hw04.Car;
using hw04.Race;

namespace hw04.TrackPoints;

public class Straight : ITrackPoint
{
    public string Description { get; set; }
    public TimeSpan AverageTime { get; set; }

    public Straight(string description, TimeSpan averageTime)
    {
        Description = description;
        AverageTime = averageTime;
    }
    
    public async Task<TrackPointPass> PassAsync(RaceCar car, int lap)
    {
        var waitingTime = TimeSpan.Zero;
        var straightTime = AverageTime * car.StraightSpeed * car.TireStrategy[car.ActiveTireIndex].GetSpeed();
        await Task.Delay(straightTime);
        return new TrackPointPass(this, waitingTime, straightTime, car.Driver, lap, TimeSpan.Zero);
    }
}