using hw04.TrackPoints;

namespace hw04.Race;

public record TrackPointPass(ITrackPoint TrackPoint, TimeSpan WaitingTime, TimeSpan DrivingTime, string Driver, int Lap, TimeSpan ExecutionTime)
{
    public TrackPointPass UpdateExecution(TimeSpan taskTime) => this with { ExecutionTime = taskTime };
}