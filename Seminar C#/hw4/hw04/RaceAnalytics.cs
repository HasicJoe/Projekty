using System.Collections.Generic;
using System.Security.Cryptography;
using hw04.Car;
using hw04.TrackPoints;

namespace hw04;

public static class RaceAnalytics
{
    public static List<(string driver, TimeSpan TotalTime)> GetFinalOrder(this Race.Race race)
    {
        Dictionary<string, TimeSpan> driversTimes = new Dictionary<string, TimeSpan>();
        foreach (var p in race.GetLapsData())
        {
            driversTimes.Add(p.Key, p.Value.Aggregate(TimeSpan.Zero, (total, lap) => total + lap.LapTime));
        }
        
        var finalOrder = driversTimes
            .Select(x => (driver: x.Key, TotalTime: x.Value))
            .OrderBy(result => result.TotalTime)
            .ToList();
        return finalOrder;
    }

    public static List<(string driver, TimeSpan LapTime, int lapIndex)> GetFastestLapForEachDriver(this Race.Race race)
    {
        var fastestLapRaceData = race.GetLapsData()
            .Select(x => (
                driver: x.Key,
                LapTime: x.Value.OrderBy(l => l.LapTime).First().LapTime,
                lapIndex: x.Value.OrderBy(l => l.LapTime).First().Number
            ))
            .ToList();
        return fastestLapRaceData;
    }

    public static List<(string driver, TimeSpan TotalTime)> GetOrder(this Race.Race race, int afterLap)
    {
        Dictionary<string, TimeSpan> driversTimes = new Dictionary<string, TimeSpan>();
        var driverLapTimes = race.GetLapsData()
            .ToDictionary(x => x.Key, x => x.Value.Where(x => x.Number <= afterLap).ToList());
        foreach (var p in driverLapTimes)
        {
            driversTimes.Add(p.Key, p.Value.Aggregate(TimeSpan.Zero, (total, lap) => total + lap.LapTime));
        }
        var orderAfterSpecificLap = driversTimes
            .Select(x => (driver: x.Key, TotalTime: x.Value))
            .OrderBy(result => result.TotalTime)
            .ToList();
        return orderAfterSpecificLap;
    }

    public static List<TrackPointDetail> GetTrackPointsInfo(this Race.Race race)
    {
        var raceData = race.GetLapsData();

        var uniqueTrackPoints = raceData
            .Values.SelectMany(laps => laps.SelectMany(lap => lap.TrackPoints))
            .GroupBy(tP => tP.TrackPoint.Description)
            .Distinct()
            .ToList();

        var pointsDetails = new List<TrackPointDetail>();
        foreach (var uniqueTP in uniqueTrackPoints)
        {
            var pointPasses = raceData.Values.SelectMany(laps =>
                laps.SelectMany(lap => lap.TrackPoints).Where(lap => lap.TrackPoint.Description == uniqueTP.Key)).ToList();

            var fastestPointPass = pointPasses.OrderBy(p => p.DrivingTime + p.WaitingTime).First();
            var longestWaitingTime = pointPasses.OrderByDescending(p => p.WaitingTime).First();
            pointsDetails.Add(new TrackPointDetail(
                fastestPointPass.TrackPoint, fastestPointPass.Driver,
                fastestPointPass.DrivingTime + fastestPointPass.WaitingTime, fastestPointPass.Lap,
                longestWaitingTime.Driver, longestWaitingTime.WaitingTime, longestWaitingTime.Lap));
        }
        return pointsDetails;
    }






}