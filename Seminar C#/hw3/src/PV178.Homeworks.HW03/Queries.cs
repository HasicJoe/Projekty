using PV178.Homeworks.HW03.DataLoading.DataContext;
using PV178.Homeworks.HW03.DataLoading.Factory;
using PV178.Homeworks.HW03.Model;
using PV178.Homeworks.HW03.Model.Enums;

namespace PV178.Homeworks.HW03
{
    public class Queries
    {
        private IDataContext? _dataContext;
        public IDataContext DataContext => _dataContext ??= new DataContextFactory().CreateDataContext();

        /// <summary>
        /// Ukážkové query na pripomenutie základnej LINQ syntaxe a operátorov. Výsledky nie je nutné vracať
        /// pomocou jedného príkazu, pri zložitejších queries je vhodné si vytvoriť pomocné premenné cez `var`.
        /// Toto query nie je súčasťou hodnotenia.
        /// </summary>
        /// <returns>The query result</returns>
        public int SampleQuery()
        {
            return DataContext.Countries
                .Where(a => a.Name?[0] >= 'A' && a.Name?[0] <= 'G')
                .Join(DataContext.SharkAttacks,
                    country => country.Id,
                    attack => attack.CountryId,
                    (country, attack) => new { country, attack }
                )
                .Join(DataContext.AttackedPeople,
                    ca => ca.attack.AttackedPersonId,
                    person => person.Id,
                    (ca, person) => new { ca, person }
                )
                .Where(p => p.person.Sex == Sex.Male)
                .Count(a => a.person.Age >= 15 && a.person.Age <= 40);
        }

        /// <summary>
        /// Úloha č. 1
        ///
        /// Vráťte zoznam, v ktorom je textová informácia o každom človeku,
        /// na ktorého v štáte Bahamas zaútočil žralok s latinským názvom začínajúcim sa 
        /// na písmeno I alebo N.
        /// 
        /// Túto informáciu uveďte v tvare:
        /// "{meno človeka} was attacked in Bahamas by {latinský názov žraloka}"
        /// </summary>
        /// <returns>The query result</returns>
        public List<string> InfoAboutPeopleThatNamesStartsWithCAndWasInBahamasQuery()
        {
            var query = DataContext.AttackedPeople
                .Join(DataContext.SharkAttacks, aP => aP.Id,
                    sA => sA.AttackedPersonId,
                    (aP, sA) => new { AttackedPerson = aP, SharkAttack = sA })
                .Join(DataContext.Countries, aPsa => aPsa.SharkAttack.CountryId,
                    c => c.Id, (aPsa, c) => new { aPsa, Country = c })
                .Where(aPsaC => aPsaC.Country.Name == "Bahamas")
                .Join(DataContext.SharkSpecies, aPsaC => aPsaC.aPsa.SharkAttack.SharkSpeciesId,
                    sS => sS.Id, (aPsaC, sS) => new { aPsaC, sS })
                .Where(aPsaCsS => aPsaCsS.sS.LatinName?[0] == 'I' ||
                                  aPsaCsS.sS.LatinName?[0] == 'N');

            return query.Select(s =>
                    $"{s.aPsaC.aPsa.AttackedPerson.Name} was attacked in {s.aPsaC.Country.Name} by {s.sS.LatinName}")
                .ToList();

        }

        /// <summary>
        /// Úloha č. 2
        ///
        /// Firma by chcela expandovať do krajín s nedemokratickou formou vlády – monarchie alebo teritória.
        /// Pre účely propagačnej kampane by chcela ukázať, že žraloky v týchto krajinách na ľudí neútočia
        /// s úmyslom zabiť, ale chcú sa s nimi iba hrať.
        /// 
        /// Vráťte súhrnný počet žraločích utokov, pri ktorých nebolo preukázané, že skončili fatálne.
        /// 
        /// Požadovany súčet vykonajte iba pre krajiny s vládnou formou typu 'Monarchy' alebo 'Territory'.
        /// </summary>
        /// <returns>The query result</returns>
        public int FortunateSharkAttacksSumWithinMonarchyOrTerritoryQuery()
        {
            var query = DataContext.SharkAttacks
                .Where(sA => sA.AttackSeverenity != AttackSeverenity.Fatal)
                .Join(DataContext.Countries, sA => sA.CountryId, c => c.Id,
                    (sA, c) => new { SharkAttack = sA, Country = c }
                ).Where(sAc =>
                    sAc.Country.GovernmentForm == GovernmentForm.Monarchy ||
                    sAc.Country.GovernmentForm == GovernmentForm.Territory);
            return query.Count();
        }

        /// <summary>
        /// Úloha č. 3
        ///
        /// Marketingovému oddeleniu dochádzajú nápady ako pomenovávať nové produkty.
        /// 
        /// Inšpirovať sa chcú prezývkami žralokov, ktorí majú na svedomí najviac
        /// útokov v krajinách na kontinente 'South America'. Pre pochopenie potrebujú 
        /// tieto informácie vo formáte slovníku:
        /// 
        /// (názov krajiny) -> (prezývka žraloka s najviac útokmi v danej krajine)
        /// </summary>
        /// <returns>The query result</returns>
        public Dictionary<string, string> MostProlificNicknamesInCountriesQuery()
        {
            var sharksInSouthAmericaCount = DataContext.SharkAttacks
                .Join(DataContext.Countries, sA => sA.CountryId,
                    c => c.Id, (sA, c) => new
                    {
                        SharkAttack = sA, Country = c
                    }
                ).Where(sAc => sAc.Country.Continent == "South America")
                .Join(DataContext.SharkSpecies, sAc => sAc.SharkAttack.SharkSpeciesId,
                    sS => sS.Id, (sAc, sS) => new
                    {
                        sAc, SharkSpecies = sS
                    }).GroupBy(sAcsS => new
                {
                    CountryName = sAcsS.sAc.Country.Name, SharkName = sAcsS.SharkSpecies.AlsoKnownAs

                })
                .Select(g => new
                {
                    SN = g.Key.SharkName, CN = g.Key.CountryName, Count = g.Count()
                });
            // odstranenie prazdnych aliasov a vybranie maxima pre kazdu krajinu
            Dictionary<string, string> mostProfilicNicknamesInCountries = sharksInSouthAmericaCount
                .Where(g => !string.IsNullOrEmpty(g.SN))
                .GroupBy(g => g.CN)
                .Select(g => g.OrderByDescending(x => x.Count).First())
                .ToDictionary(g => g.CN!, g => g.SN!);
            return mostProfilicNicknamesInCountries;
        }

        /// <summary>
        /// Úloha č. 4
        ///
        /// Firma chce začať kompenzačnú kampaň a potrebuje k tomu dáta.
        ///
        /// Preto zistite, ktoré žraloky útočia najviac na mužov. 
        /// Vráťte ID prvých troch žralokov, zoradených zostupne podľa počtu útokov na mužoch.
        /// </summary>
        /// <returns>The query result</returns>
        public List<int> ThreeSharksOrderedByNumberOfAttacksOnMenQuery()
        {
            var attackOnMales = DataContext.SharkAttacks
                .Join(DataContext.AttackedPeople, sA => sA.AttackedPersonId,
                    aP => aP.Id, (sA, aP) => new
                    {
                        SharkAttack = sA, AttackedP = aP
                    })
                .Where(sAaP => sAaP.AttackedP.Sex == Sex.Male);

            var sharkAttacks = attackOnMales
                .Join(DataContext.SharkSpecies, sAaP => sAaP.SharkAttack.SharkSpeciesId,
                    sS => sS.Id, (sAaP, sS) => new
                    {
                        sAaP, SharkSpecies = sS
                    })
                .GroupBy(sAaPsS => new { SharkSpiecesGID = sAaPsS.SharkSpecies.Id }
                )
                .Select(g => new { SharkId = g.Key.SharkSpiecesGID, Count = g.Count() });

            var topThreeAttackers = sharkAttacks
                .OrderByDescending(sA => sA.Count)
                .Take(3)
                .Select(sA => sA.SharkId).ToList();
            return topThreeAttackers;
        }

        /// <summary>
        /// Úloha č. 5
        ///
        /// Oslovila nás medzinárodná plavecká organizácia. Chce svojich plavcov motivovať možnosťou
        /// úteku pred útokom žraloka.
        ///
        /// Potrebuje preto informácie o priemernej rýchlosti žralokov, ktorí
        /// útočili na plávajúcich ľudí (informácie o aktivite počas útoku obsahuje "Swimming" alebo "swimming").
        /// 
        /// Pozor, dáta požadajú oddeliť podľa jednotlivých kontinentov. Ignorujte útoky takých druhov žralokov,
        /// u ktorých nie je známa maximálná rýchlosť. Priemerné rýchlosti budú zaokrúhlené na dve desatinné miesta.
        /// </summary>
        /// <returns>The query result</returns>
        public Dictionary<string, double> SwimmerAttacksSharkAverageSpeedQuery()
        {
            
            var query = DataContext.SharkSpecies
                .Where(sS => sS.TopSpeed != null && sS.TopSpeed > 0 )
                .Join(DataContext.SharkAttacks, sS => sS.Id, sA => sA.SharkSpeciesId,
                    (sS, sA) => new { sS, sA })
                .Where(sSsA => sSsA.sA.Activity != null && (sSsA.sA.Activity.Contains("swimming") || sSsA.sA.Activity.Contains("Swimming")))
                .Join(DataContext.Countries, sSsA => sSsA.sA.CountryId, c => c.Id,
                    (sSsA, c) => new
                    {
                        sSsA, c
                    })
                .Select(sSsAc => new
                {
                    Speed = sSsAc.sSsA.sS.TopSpeed,
                    sSsAc.c.Continent

                });
            var averageSpeedOnContinents = query
                .GroupBy(x =>  x.Continent)
                .Select(g => new
                    { 
                        Continent = g.Key?.ToString(), 
                        AvgSpeed = Math.Round((double) g.Average(d => d.Speed), 2)

                    }
                );
            return averageSpeedOnContinents.ToDictionary(x => x.Continent!, x => x.AvgSpeed);
        }

        /// <summary>
        /// Úloha č. 6
        ///
        /// Zistite všetky nefatálne (AttackSeverenity.NonFatal) útoky spôsobené pri člnkovaní 
        /// (AttackType.Boating), ktoré mal na vine žralok s prezývkou "Zambesi shark".
        /// Do výsledku počítajte iba útoky z obdobia po 3. 3. 1960 (vrátane) a ľudí,
        /// ktorých meno začína na písmeno z intervalu <D, K> (tiež vrátane).
        /// 
        /// Výsledný zoznam mien zoraďte abecedne.
        /// </summary>
        /// <returns>The query result</returns>
        public List<string> NonFatalAttemptOfZambeziSharkOnPeopleBetweenDAndKQuery()
        {
            DateTime startSharkDate = new DateTime(1960, 3 , 2);
            var boatingNonFatalAttacks = DataContext.SharkAttacks
                .Where(
                sA => (sA.AttackSeverenity == AttackSeverenity.NonFatal) && 
                      (sA.Type == AttackType.Boating) && (sA.DateTime > startSharkDate));
            var boatingNonFatalAttacksFromDtoK = boatingNonFatalAttacks
                .Join(DataContext.AttackedPeople, sA => sA.AttackedPersonId, 
                    aP => aP.Id, (sA, aP) => new
                {
                    sA, aP
                })
                .Where(sAaP => sAaP.aP.Name?[0] >= 'D' && sAaP.aP.Name?[0] <= 'K');

            var query = boatingNonFatalAttacksFromDtoK
                .Join(DataContext.SharkSpecies, sAaP => sAaP.sA.SharkSpeciesId, 
                    sS => sS.Id, (sAaP, sS) => new
                {
                    sAaP, sS
                })
                .Where(sAaPsS => sAaPsS.sS.AlsoKnownAs == "Zambesi shark")
                .Select(x => x.sAaP.aP.Name);
            return query.ToList()!;
        }

        /// <summary>
        /// Úloha č. 7
        ///
        /// Zistilo sa, že desať najľahších žralokov sa správalo veľmi podozrivo počas útokov v štátoch Južnej Ameriky.
        /// 
        /// Vráťte preto zoznam dvojíc, kde pre každý štát z Južnej Ameriky bude uvedený zoznam žralokov,
        /// ktorí v tom štáte útočili. V tomto zozname môžu figurovať len vyššie spomínaných desať najľahších žralokov.
        /// 
        /// Pokiaľ v nejakom štáte neútočil žiaden z najľahších žralokov, zoznam žralokov u takého štátu bude prázdny.
        /// </summary>
        /// <returns>The query result</returns>
        public List<Tuple<string, List<SharkSpecies>>> LightestSharksInSouthAmericaQuery()
        {
            List<Tuple<string, List<SharkSpecies>>> sharkList = new List<Tuple<string, List<SharkSpecies>>>();

            var top10LightestSharkIds = DataContext.SharkSpecies
                .OrderBy(sS => sS.Weight)
                .Take(10)
                .Select(sS => sS.Id);

            DataContext.Countries
                .Where(c => c.Continent == "South America")
                .Select(c => new Tuple<int, string>(c.Id, c.Name!))
                .ToList()
                .ForEach(country =>
                {
                    sharkList.Add(new Tuple<string, List<SharkSpecies>>(country.Item2, new List<SharkSpecies>()));
                    var lightSharkIds = DataContext.SharkAttacks
                        .Where(sA => sA.CountryId == country.Item1)
                        .Where(sA => top10LightestSharkIds.Contains(sA.SharkSpeciesId))
                        .Select(sA => sA.SharkSpeciesId)
                        .Distinct();
                    lightSharkIds.ToList().ForEach(sharkId =>
                    {
                        sharkList.Last().Item2.Add(DataContext.SharkSpecies.First(sS => sS.Id == sharkId));
                    });
                });
            return sharkList;
        }

        /// <summary>
        /// Úloha č. 8
        ///
        /// Napísať hocijaký LINQ dotaz musí byť pre Vás už triviálne. Riaditeľ firmy vás preto chce
        /// využiť na testovanie svojich šialených hypotéz.
        /// 
        /// Zistite, či každý žralok, ktorý má maximálnu rýchlosť aspoň 56 km/h zaútočil aspoň raz na
        /// človeka, ktorý mal viac ako 56 rokov. Výsledok reprezentujte ako pravdivostnú hodnotu.
        /// </summary>
        /// <returns>The query result</returns>
        public bool FiftySixMaxSpeedAndAgeQuery()
        {
            var speedySharskIds = DataContext.SharkSpecies
                .Where(sS => sS.TopSpeed >= 56).Select(sS => sS.Id);
            var oldPeopleIds = DataContext.AttackedPeople
                .Where(aP => aP.Age > 56).Select(aP => aP.Id);

            return speedySharskIds
                             .All(id => DataContext.SharkAttacks.Any(sA => sA.SharkSpeciesId == id)) &&
                         oldPeopleIds.All(id => DataContext.SharkAttacks.Any(sA => sA.AttackedPersonId == id));

        }

        /// <summary>
        /// Úloha č. 9
        ///
        /// Ohromili ste svojim výkonom natoľko, že si od Vás objednali rovno textové výpisy.
        /// Samozrejme, že sa to dá zvladnúť pomocou LINQ.
        /// 
        /// Chcú, aby ste pre všetky fatálne útoky v štátoch začínajúcich na písmeno 'B' alebo 'R' urobili výpis v podobe: 
        /// "{Meno obete} was attacked in {názov štátu} by {latinský názov žraloka}"
        /// 
        /// Záznam, u ktorého obeť nemá meno
        /// (údaj neexistuje, nejde o vlastné meno začínajúce na veľké písmeno, či údaj začína číslovkou)
        /// do výsledku nezaraďujte. Získané pole zoraďte abecedne a vraťte prvých 5 viet.
        /// </summary>
        /// <returns>The query result</returns>
        public List<string> InfoAboutPeopleAndCountriesOnBorRAndFatalAttacksQuery()
        {
            var fattalAttacks = DataContext.SharkAttacks
                .Where(sA => sA.AttackSeverenity == AttackSeverenity.Fatal)
                .Join(DataContext.Countries, sA => sA.CountryId,
                    c => c.Id, (sA, c) => new
                    {
                        sA, c
                    }
                )
                .Where(sAc => sAc.c.Name?.Length > 0)
                .Where(sAc => sAc.c.Name != null && (sAc.c.Name[0] == 'B' || sAc.c.Name[0] == 'R'));
            var query = fattalAttacks
                .Join(DataContext.SharkSpecies, sAc => sAc.sA.SharkSpeciesId,
                    sS => sS.Id, (sAc, sS) => new
                    {
                        sAc, sS

                    })
                .Join(DataContext.AttackedPeople, sAcsS => sAcsS.sAc.sA.AttackedPersonId, aP => aP.Id,
                    (sAcsS, aP) => new
                    {
                        aP, sAcsS
                    })
                .Where(sAcsSaP => (sAcsSaP.aP.Name?.Length > 0 && sAcsSaP.aP.Name[0] >= 'A' && sAcsSaP.aP.Name[0] <= 'Z'))
                .Select(s => $"{s.aP.Name} was attacked in {s.sAcsS.sAc.c.Name} by {s.sAcsS.sS.LatinName}");
            return query.OrderBy(s => s).Take(5).ToList();
        }

        /// <summary>
        /// Úloha č. 10
        ///
        /// Nedávno vyšiel zákon, že každá krajina Európy začínajúca na písmeno A až L (vrátane)
        /// musí zaplatiť pokutu 250 jednotiek svojej meny za každý žraločí útok na ich území.
        /// Pokiaľ bol tento útok smrteľný, musia dokonca zaplatiť 300 jednotiek. Ak sa nezachovali
        /// údaje o tom, či bol daný útok smrteľný alebo nie, nemusia platiť nič.
        /// Áno, tento zákon je spravodlivý...
        /// 
        /// Vráťte informácie o výške pokuty európskych krajín začínajúcich na A až L (vrátane).
        /// Tieto informácie zoraďte zostupne podľa počtu peňazí, ktoré musia tieto krajiny zaplatiť.
        /// Vo finále vráťte 5 záznamov s najvyššou hodnotou pokuty.
        /// 
        /// V nasledujúcej sekcii môžete vidieť príklad výstupu v prípade, keby na Slovensku boli 2 smrteľné útoky,
        /// v Česku jeden fatálny + jeden nefatálny a v Maďarsku žiadny:
        /// <code>
        /// Slovakia: 600 EUR
        /// Czech Republic: 550 CZK
        /// Hungary: 0 HUF
        /// </code>
        /// 
        /// </summary>
        /// <returns>The query result</returns>
        public List<string> InfoAboutFinesInEuropeQuery()
        {
            var attacksInEuropeStates = DataContext.SharkAttacks
                .Where(sS => sS.AttackSeverenity != AttackSeverenity.Unknown)
                .Join(DataContext.Countries, sA => sA.CountryId,
                    c => c.Id, (sA, c) => new
                    {
                        sA, c
                    }
                )
                .Where(sAc =>
                    sAc.c.Continent == "Europe" && sAc.c.Name?.Length > 0 && sAc.c.Name[0] >= 'A' &&
                    sAc.c.Name[0] <= 'L')
                .GroupBy(x => new { x.c.Name, x.sA.AttackSeverenity, x.c.CurrencyCode})
                .Select(g => new
                {
                    g.Key.Name,
                    g.Key.AttackSeverenity,
                    g.Key.CurrencyCode,
                    Value = g.Key.AttackSeverenity == AttackSeverenity.Fatal ? 300 * g.Count() : 250 * g.Count()
                });

            var query = attacksInEuropeStates
                .GroupBy(x => new {x.Name, x.CurrencyCode})
                .Select(g => new
                {
                    g.Key.Name,
                    g.Key.CurrencyCode,
                    TotalValue = g.Sum(x => x.Value)
                })
                .OrderByDescending(x => x.TotalValue).Take(5);
            return query.Select(s => $"{s.Name}: {s.TotalValue} {s.CurrencyCode}").ToList();
        }

        /// <summary>
        /// Úloha č. 11
        ///
        /// Organizácia spojených žraločích národov výhlásila súťaž: 5 druhov žralokov, 
        /// ktoré sú najviac agresívne získa hodnotné ceny.
        /// 
        /// Nájdite 5 žraločích druhov, ktoré majú na svedomí najviac ľudských životov,
        /// druhy zoraďte podľa počtu obetí.
        ///
        /// Výsledok vráťte vo forme slovníku, kde
        /// kľúčom je meno žraločieho druhu a
        /// hodnotou je súhrnný počet obetí spôsobený daným druhom žraloka.
        /// </summary>
        /// <returns>The query result</returns>
        public Dictionary<string, int> FiveSharkNamesWithMostFatalitiesQuery()
        {
            var query = DataContext.SharkAttacks
                .Where(sA => sA.AttackSeverenity == AttackSeverenity.Fatal)
                .Join(DataContext.SharkSpecies, sA => sA.SharkSpeciesId,
                    sS => sS.Id, (sA, sS) => new
                    {
                        sA,
                        sS
                    })
                .GroupBy(x => new { x.sS.Name })
                .Select(g => new
                {
                    g.Key.Name,
                    Count = g.Count()
                })
                .OrderByDescending(x => x.Count).Take(5);
            return query.ToDictionary(s => s.Name!, s => s.Count);
        }

        /// <summary>
        /// Úloha č. 12
        ///
        /// Riaditeľ firmy chce si chce podmaňiť čo najviac krajín na svete. Chce preto zistiť,
        /// na aký druh vlády sa má zamerať, aby prebral čo najviac krajín.
        /// 
        /// Preto od Vás chce, aby ste mu pomohli zistiť, aké percentuálne zastúpenie majú jednotlivé typy vlád.
        /// Požaduje to ako jeden string:
        /// "{1. typ vlády}: {jej percentuálne zastúpenie}%, {2. typ vlády}: {jej percentuálne zastúpenie}%, ...".
        /// 
        /// Výstup je potrebný mať zoradený od najväčších percent po najmenšie,
        /// pričom percentá riaditeľ vyžaduje zaokrúhľovať na jedno desatinné miesto.
        /// Pre zlúčenie musíte podľa jeho slov použiť metódu `Aggregate`.
        /// </summary>
        /// <returns>The query result</returns>
        public string StatisticsAboutGovernmentsQuery()
        {
            int totalCountries = DataContext.Countries.Select(c => c.Id).Count();
            var query = DataContext.Countries
                .GroupBy(x => new { Goverment = x.GovernmentForm.ToString() })
                .Select(g => new
                {
                    g.Key.Goverment,
                    Percentage = Math.Round((double)(100.0f * g.Count() / totalCountries), 1)
                })
                .OrderByDescending(s => s.Percentage)
                .ToList();
            return string.Join(", ", query.Select(s => $"{s.Goverment}: {s.Percentage:0.0}%"));
        }

        /// <summary>
        /// Úloha č. 13
        ///
        /// Firma zistila, že výrobky s tigrovaným vzorom sú veľmi populárne. Chce to preto aplikovať
        /// na svoju doménu.
        ///
        /// Nájdite informácie o ľudoch, ktorí boli obeťami útoku žraloka s menom "Tiger shark"
        /// a útok sa odohral v roku 2001.
        /// Výpis majte vo formáte:
        /// "{meno obete} was tiggered in {krajina, kde sa útok odohral}".
        /// V prípade, že chýba informácia o krajine útoku, uveďte namiesto názvu krajiny reťazec "Unknown country".
        /// V prípade, že informácie o obete vôbec neexistuje, útok ignorujte.
        ///
        /// Ale pozor! Váš nový nadriadený má panický strach z operácie `Join` alebo `GroupJoin`.
        /// Informácie musíte zistiť bez spojenia hocijakých dvoch tabuliek. Skúste sa napríklad zamyslieť,
        /// či by vám pomohla metóda `Zip`.
        /// </summary>
        /// <returns>The query result</returns>
        public List<string> TigerSharkAttackZipQuery()
        {
            const string unknown = "Unknown country";
            var tigerSharkId = DataContext.SharkSpecies
                .Where(sharkSpecies => sharkSpecies.Name == "Tiger shark")
                .Select(s => s.Id).FirstOrDefault();

            var attacks = DataContext.SharkAttacks
                .Where(sA => (sA.DateTime!.Value.Year == 2001) && (sA.SharkSpeciesId == tigerSharkId));

            var ids = attacks
                .Select(s => new { PersonId = s.AttackedPersonId, CountryId = s.CountryId ?? -1 })
                .Where(s => s.PersonId != null);

            var attackedPeopleIds = ids.Select(s => s.PersonId).ToList();
            var countriesIds = ids.Select(s => s.CountryId).ToList();
            return attackedPeopleIds
                .Zip(countriesIds, (personId, countryId) => 
                    $"{DataContext.AttackedPeople.First(x => x.Id == personId).Name} was tiggered in {DataContext.Countries.FirstOrDefault(x => x.Id == countryId)?.Name ?? unknown}")
                .ToList();
        }

        /// <summary>
        /// Úloha č. 14
        ///
        /// Vedúci oddelenia prišiel s ďalšou zaujímavou hypotézou. Myslí si, že veľkosť žraloka nejako 
        /// súvisí s jeho apetítom na ľudí.
        ///
        /// Zistite pre neho údaj, koľko percent útokov má na svedomí najväčší a koľko najmenší žralok.
        /// Veľkosť v tomto prípade uvažujeme podľa dĺžky.
        /// 
        /// Výstup vytvorte vo formáte: "{percentuálne zastúpenie najväčšieho}% vs {percentuálne zastúpenie najmenšieho}%"
        /// Percentuálne zastúpenie zaokrúhlite na jedno desatinné miesto.
        /// </summary>
        /// <returns>The query result</returns>
        public string LongestVsShortestSharkQuery()
        {
            var sharkAttacks = DataContext.SharkAttacks
                .Join(DataContext.SharkSpecies, sA => sA.SharkSpeciesId,
                    sS => sS.Id, (sA, sS) => new
                    {
                        sA, sS
                    });

            var query = sharkAttacks
                .GroupBy(x => new {  x.sS.Length })
                .Select(g => new
                {
                    g.Key.Length,
                    Percentage = Math.Round((double)(100.0f * g.Count() / sharkAttacks.Count()), 1)
                });
            var minLengthShark = query
                .Where(x => x.Length == query.Min(x => x.Length))
                .Select(s => $"{s.Percentage:0.0}")
                .FirstOrDefault();
            var maxLengthShark = query
                .Where(x => x.Length == query.Max(x => x.Length))
                .Select(s => $"{s.Percentage:0.0}")
                .FirstOrDefault();
            return $"{maxLengthShark}% vs {minLengthShark}%";
        }

        /// <summary>
        /// Úloha č. 15
        ///
        /// Na koniec vašej kariéry Vám chceme všetci poďakovať a pripomenúť Vám vašu mlčanlivosť.
        /// 
        /// Ako výstup požadujeme počet krajín, v ktorých žralok nespôsobil smrť (útok nebol fatálny).
        /// Berte do úvahy aj tie krajiny, kde žralok vôbec neútočil.
        /// </summary>
        /// <returns>The query result</returns>
        public int SafeCountriesQuery()
        {
            return DataContext.Countries.Distinct().Count() - DataContext.SharkAttacks
                .Where(sA => sA.AttackSeverenity == AttackSeverenity.Fatal)
                .Join(DataContext.Countries, sA => sA.CountryId,
                    c => c.Id, (sA, c) => new
                    {
                        sA, c
                    })
                .DistinctBy(x => x.sA.CountryId).Count();
        }
    }
}
