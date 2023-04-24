using System.Diagnostics;

public static class TaskTimeTracker
{
    private static Stopwatch _stopwatch;

    static TaskTimeTracker()
    {
        _stopwatch = new Stopwatch();
    }

    public static void Start()
    {
        _stopwatch.Reset();
        _stopwatch.Start();
    }

    public static TimeSpan Stop()
    {
        _stopwatch.Stop();
        return _stopwatch.Elapsed;
    }
}