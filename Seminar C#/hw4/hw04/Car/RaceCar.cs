using hw04.Car.Tires;

namespace hw04.Car;

public class RaceCar
{
    
    /// <summary>
    /// Seznam pneumatik v pořadí, v jakém je bude auto při závodu měnit. 
    /// </summary>
    public List<Tire> TireStrategy { get; set; }

    public string Driver { get; set; }
    public Team Team { get; set; }
    public double TurnSpeed { get; set; }
    public double StraightSpeed { get; set; }

    public int ActiveTireIndex { get; set; }

    public bool BoxCall { get; set; }
    

    /// <param name="driver">Jméno řidiče formule</param>
    /// <param name="team">Tým, pod který formule patří</param>
    /// <param name="turnSpeed">Rychlost auta v zatáčce</param>
    /// <param name="straightSpeed">Rychlost auta na rovince</param>
    public RaceCar(string driver, Team team, double turnSpeed, double straightSpeed)
    {
        Driver = driver;
        Team = team;
        TurnSpeed = turnSpeed;
        StraightSpeed = straightSpeed;
        ActiveTireIndex = 0;
        BoxCall = false;
    }

    public void SetNextTireIndex()
    {
        if (ActiveTireIndex + 1 < TireStrategy.Count)
        {
            ActiveTireIndex++;
        }
    }

    public void AddTireDegradationAfterLap()
    {
        TireStrategy[ActiveTireIndex].AddLap();
    }

    public async Task SimulateTireChange()
    {
        var swapTime = TimeSpan.FromMilliseconds(new Random().Next(50, 100));
        await Task.Delay(swapTime);
    }
}