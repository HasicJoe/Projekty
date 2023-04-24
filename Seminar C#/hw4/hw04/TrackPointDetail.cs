using hw04.TrackPoints;

namespace hw04;

public class TrackPointDetail
{
    public ITrackPoint TrackPoint { get; set; }
    public string FastestDriver { get; set; }
    public TimeSpan FastestTime { get; set; }
    public int FastestLap { get; set; }
    public string LongestWaitingDriver {get; set; }
    public TimeSpan LongestWaitingTime { get; set; }
    public int LongestWaitingLap { get; set; }

    public TrackPointDetail(ITrackPoint trackPoint, string fastestDriver, TimeSpan fastestTime, int fastestLap,
        string
            longestWaitingDriver, TimeSpan longestWaitingTime, int longestWaitingLap)
    {
        TrackPoint = trackPoint;
        FastestDriver = fastestDriver;
        FastestTime = fastestTime;
        FastestLap = fastestLap;
        LongestWaitingTime = longestWaitingTime;
        LongestWaitingDriver = longestWaitingDriver;
        LongestWaitingLap = longestWaitingLap;
    }

    public void PrintPointDetails()
    {
        Console.WriteLine($"=== Trackpoint: {TrackPoint.Description}");
        Console.WriteLine($"\t The fastest pass: ({FastestDriver}, {FastestTime.Milliseconds} ms, lap: {FastestLap}),"); 
        Console.WriteLine($"\t The longest waiting time: ({LongestWaitingDriver}, {LongestWaitingTime.Milliseconds} ms, lap:{LongestWaitingLap})");
    }
}