# Manuál k spusteniu a inštalácii bakalárskej práce - Získavanie znalostí z webových logov (Samuel Valaštín, 2022)
Tento manuál slúži ako návod na lokálne spustenie implementovanej aplikácie pre získavania znalostí z webových prístupových logov. Táto aplikácia, ktorá bola vyvinutá ako praktická časť bakalárskej práce bola vyvíjaná na **Linuxovom operačnom systéme**. Vzhľadom na fakt, že aplikácia po predspracovaní ukladá spracované dátové rámce a grafické objekty získaných štatistík na **Google Disk**, tak sa odporúča pre prácu s touto aplikáciou využívať prehliadač **Google Chrome**. Aplikácia však umožňuje beh a spustenie aj na iných dostupných prehliadačoch.

# Návod a prerekvizity pred lokálnym spustením aplikácie
Táto podsekcia definuje potrebné prerekvizity a návod, ktorý informuje o spôsobe, akým je možné implementovanú aplikáciu spojazdniť.

## Prerekvizity a technické informácie
**1).** V podzložke **/app** je k dispozícii textový súbor **requirements.txt** so všetkými požiadavkami a externými python balíčkami, ktoré implementovaná aplikácia vyžaduje pre svoju činnosť. V prípade využitia na webovom serveri je nutné odkomentovať posledný riadok tohto priloženého súboru, ktorý reprezentuje požiadavku na webový server. Všetky externé **Javascriptové** knižnice sú prilinkované prostredníctom CDN a tak nie je nutné ich priloženie, či prípadná inštalácia.

  
**2).** Pre nainštalovanie potrebných externých balíčkov je potrebné mať nainštalovaný interpret jazyku *Python*, pričom aplikácia bola implementovaná a odtestovaná vo verzii **Python 3.9.2**. Rovnako je potrebné mať nainštalovaný *Python Package Manager (pip)*, pričom pre vývoj aplikácie som využíval package manager vo verzii **pip 20.2.4**.

## Návod pre lokálne spustenie aplikácie (Linux OS)
Tento návod obsahuje informácie k lokálnemu spusteniu aplikácie po splnení vyššie popísaných prerekvizít.

**1).** Po nainštalovaní balíku Python a Package Manageru pre tento jazyk je najskôr potrebné nainštalovať externé balíky, ktoré aplikácia vyžaduje pre svoju činnosť : **python -m pip install -r requirements.txt**
Ideálne je využit python environment, v prípade ak je nainštalovaný niektorá zo závislostí je vhodné nainštalovať potrebné závislosti v odtestovaných verziách prostredníctvom: **python -m pip install -U -r requirements.txt**
V textovom súbore *requirements.txt* sú špecifikované verzie balíčkov, ktorých činnosť v spoluprácii s aplikáciou bola riadne odtestovaná. 

**2).** Po nainštalovaní je pripravený jednoduchý skript, ktorý umožnuje naštartovanie jednoduchého HTTP serveru. Spustenie tohto skriptu je možné vykonať z terminálu prostredníctvom kombinácie príkazov:

**chmod +x run.sh** -- slúži pre polovenie spustenia skriptu run.sh

**./run.sh** -- pre naštartovanie HTTP serveru

**3).** V prípade úspešného naštartovania HTTP serveru a po splnení prerekvizít je možné k aplikácii pristúpiť z prostredia webového prehliadača vložením nasledujúcej adresy: **http://localhost:5000/** alebo prípadne adresy **http://127.0.0.1:5000/**

**4).** Na hlavnej stránke implementovanej aplikácie je k dispozícii rozcestník, ktorý obsahuje odkazy na všetky implementované časti aplikácie a rovnako sú k dispozícii menšie datasety, ktoré kliknutím na dátovu sadu zo zoznamu pripravených dátových sád umožňuje lokálne stiahnutie týchto datasetov, ktorými možno vykonanať predspracovanie a po nastavení ukladania je možné vykonať aj následnú modelovaciu časť určenú pre získavanie znalostí z týchto predspracovaných dát.
  
## Aplikácia je dostupná aj na webovom hostingu
Implementovaná aplikácia je rovnako dostupná aj na webovom hostingu na adrese: **https://logmine.herokuapp.com/**
Nasadená aplikácia umožnuje demonštráciu činnosti aplikácie, avšak vzhľadom na minimálne pamäťové a výpočetné možnosti voľného hostingu sa odporúča, vzhľadom na povahu aplikácie lokálny beh.
  
## Správa ukladania prostredníctvom Google Drive API
Implementovaná aplikácia vyžaduje prihlásenie Google účtom a nastavenie aktívneho priečinku, do ktorého sú ukladané predspracované dátové rámce a vizualizované štatistiky. Toto nastavenie je možné vykonať prostredníctvom výberu položky **Správa ukladania spracovaných rámcov/štatistík** z rozcestníku dostupného na hlavnej stránke aplikácie. Bližšie informácie sú k dispozícii v technickej dokumentácii k bakalárskej práci

# Upozornenie !!!
Aplikácia umožňuje zmenu prihláseného užívateľa pre ukladanie dátových rámcov a štatistík do Google priečinku špecifikovaného užívatela. Pri využívaní aplikácii na webovom hostingu na adrese **https://logmine.herokuapp.com/** tento hosting nepodporuje úpravu portov, ktoré knižnica **pyDrive2** a  server pre autentifikáciu vyžaduje a tak nie je možné vykonať autentifikáciu. Pre lokálne spustenie všetko pracuje správne a aplikácia umožňuje zmeniť aktívneho užívateľa. Na hostingu je prihlásený užívateľ a aplikácia zároveň poskytuje všetkú ďaľej implementovanú funkcionalitu k správe ukladania.

# Kontakt
V prípade nejasností alebo problémov ohľadne nastavenia aplikácie či jej činnosti ma môžte kontaktovať prostredníctvom e-mailu: **xvalas10@stud.fit.vutbr.cz** respektíve osobný e-mail: **samuel.valastinnn@gmail.com**. Rád vam zodpoviem na vaše otázky.