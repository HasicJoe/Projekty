using hw04.Car;
namespace hw04.TrackPoints;

public class Track
{
    private readonly List<ITrackPoint> _trackPoints;
    private Turn? _pitLaneEntry;
    private Turn? _pitLaneExit;
    private PitLane? _pitLane;

    public Track()
    {
        _trackPoints = new List<ITrackPoint>();
    }

    public Track AddTurn(string description, TimeSpan averageTime, int carsAllowed)
    {
        _trackPoints.Add(new Turn(description, averageTime, carsAllowed));
        return this;
    }

    public Track AddStraight(string description, TimeSpan averageTime)
    {
        _trackPoints.Add(new Straight(description, averageTime));
        return this;
    }

    public Track AddPitLane(TimeSpan entryTime, TimeSpan exitTime, List<Team> teams,
        int nextPoint)
    {
        _pitLane = new PitLane("PitLane", teams);
        _pitLaneEntry = new Turn("PitLane Entry", entryTime, 1);
        _pitLaneExit = new Turn("PitLane Exit", exitTime, 1);
        return this;
    }

    public IEnumerable<ITrackPoint> GetLapAfterPitstop()
    {
        List<ITrackPoint> trackPointsAfterPitstop = new List<ITrackPoint>
        {
            _pitLane!,
            _pitLaneExit!
        };
        // po vyjazde je prvym bodom Turn 2 - Farm curve
        for (int i = 2; i < _trackPoints.Count; i++)
        {
            trackPointsAfterPitstop.Add(_trackPoints[i]);
        }
        return trackPointsAfterPitstop;
    }

    public IEnumerable<ITrackPoint> GetPitstopLap()
    {
         List<ITrackPoint> trackPointsPitstop = new List<ITrackPoint>();
        for (int i = 0; i < _trackPoints.Count; i++)
        {
            // dokedy je description bez vynechania pitlane tak pridavame body
            if (_trackPoints[i].Description.StartsWith((i + 1).ToString()))
            {
                trackPointsPitstop.Add(_trackPoints[i]);
            }
            else
            {
                break;
            }
        }

        if (_pitLaneEntry != null) trackPointsPitstop.Add(_pitLaneEntry);
        return trackPointsPitstop;

    }

    /// <summary>
    /// Vrací seznam trackpoints s ohledem na to,
    /// zda má auto jít na konci kola vyměnit pneumatiky nebo ne
    /// </summary>
    /// <param name="car"></param>
    /// <returns></returns>
    public IEnumerable<ITrackPoint> GetLap(RaceCar car)
    {
        // prve kolo po vymene pneumatik 
        if (car.BoxCall)
        {
            car.BoxCall = false;
            return GetLapAfterPitstop();
        }
        // kolo pri navsteve pitstopu
        if (car.TireStrategy[car.ActiveTireIndex].NeedsChange())
        {
            car.BoxCall = true;
            return GetPitstopLap();
        }
        //klasicke kolo
        return _trackPoints;
    }

}