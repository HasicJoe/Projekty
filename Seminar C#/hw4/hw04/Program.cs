using hw04;

var simulationSilverstone = new Simulation(CurrentF1.Tracks.Silverstone);

//Console.WriteLine("========================= STRATEGY TESTING START  =========================");
//CurrentF1.Cars.All.First().SetMediumHardStrategy();
//var lapsMediumHard = await simulationSilverstone.SimulateLapsAsync(CurrentF1.Cars.All.First(), 52);
//Console.WriteLine("SIMULATION RESULTS:");
//foreach (var lap in lapsMediumHard)
//{
//    Console.WriteLine($"{lap.RaceCar.Driver}, lap: {lap.Number}, laptime: {lap.LapTime:mm\\:ss\\.ff}");
//}
//Console.WriteLine("========================= STRATEGY TESTING END  ===========================");




// Tady můžete vyhodnotit na základě jednotlivých simulací, která je nejlepší
// a zvolit ji pro všechny nebo některé formule.
// Stejně můžete testovat nastavení formule - turnSpeed a straightSpeed;
// skuste zachovat součet turnSpeed a straightSpeed, ale optimalizovat
// rychlost na kolo. (není součástí zadání = není nutno řešit)

// Závod

/*
 * Pre zavod som vlozil vlastnu strategiu, ktora zacina na stredne tvrdych pneumatikach a obsahuje 2 pitstopy a aby
 * to nebolo take 'dummy' tak som pre zmenu pneumatik zvolil random interval cim sa potencionalne vyhneme pripadu
 * kedy by obaja kolegovia jazdiaci za rovnaky tim isli do boxov naraz - toto mi prislo nelogicke.
 */

Console.WriteLine("========================= RACE SIMULATION START  ==========================");
CurrentF1.Cars.All.ForEach(c => c.SetMediumSoftMediumStrategy());
var race = await new Simulation(CurrentF1.Tracks.Silverstone).SimulateRaceAsync(CurrentF1.Cars.All, 52);
Console.WriteLine("========================= RACE SIMULATION END  ===========================");


// analyticka metoda pre vypis finalneho poradia jazdcov
Console.WriteLine("========================= FINAL ORDER START ==============================");
foreach (var (car, totalTime) in race.GetFinalOrder())
{
    Console.WriteLine($"{car}: {totalTime.Minutes} min {totalTime.Seconds} s {totalTime.Milliseconds} ms");
}
Console.WriteLine("========================= FINAL ORDER END ===============================");

// analyticka metoda pre vypis poradia po zvolenom kole
Console.WriteLine("========================= ORDER AFTER 10TH LAP START ==============================");
foreach (var (car, totalTime) in race.GetOrder(10))
{
    Console.WriteLine($"{car}: {totalTime.Minutes} min {totalTime.Seconds} s {totalTime.Milliseconds} ms");
}
Console.WriteLine("========================= ORDER AFTER 10TH LAP END ===============================");

// analyticka metoda pre vypis najrychlejsieho kola kazdeho jazdca - najrychlejsie kola by mali byt
// v poslednych kolach, ked maju formule softy (najrychlejsiu zmes)
Console.WriteLine("========================= FASTEST LAPS START ==============================");
foreach (var (car, fastestLapTime, lapIndex) in race.GetFastestLapForEachDriver())
{
    Console.WriteLine($"{car}: {fastestLapTime:mm\\:ss\\.ff} [LAP {lapIndex}]");
}
Console.WriteLine("========================= FASTEST LAPS END ==============================");


Console.WriteLine("==================== TRACKPOINT DETAIL INFO START ==== =====================");
foreach (var trackPointDetail in race.GetTrackPointsInfo())
{
    trackPointDetail.PrintPointDetails();
}
Console.WriteLine("==================== TRACKPOINT DETAIL INFO END ===========================");