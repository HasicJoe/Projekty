using hw04.Car;

namespace hw04.Race;

public class Lap
{
    public RaceCar RaceCar { get; }
    public int Number { get; }

    public TimeSpan LapTime {get; set; }

    public TimeSpan OverHeadTime { get; set; }

    public List<TrackPointPass> TrackPoints { get; }

    public Lap(RaceCar car, int number, TimeSpan lapTime, List<TrackPointPass> trackPoints)
    {
        RaceCar = car;
        Number = number;
        TrackPoints = trackPoints;
        OverHeadTime = TimeSpan.Zero;
        LapTime = lapTime;
    }

    public void AddOverhead(TimeSpan overhead)
    {
        OverHeadTime =+ overhead;
        LapTime += overhead;
    }

}