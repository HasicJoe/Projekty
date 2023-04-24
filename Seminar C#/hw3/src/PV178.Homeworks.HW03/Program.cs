using PV178.Homeworks.HW03;

Console.WriteLine("Welcome to HW03, you can use this file as a playground for manually testing your solution.");


Queries q = new Queries();
var data = q.SafeCountriesQuery();
Console.WriteLine(data);