# MX-5 Smart Gauge – Projekt specifikáció

## 1. Projekt célja

Egy ESP32-alapú, univerzális, konfigurálható autós digitális műszer fejlesztése.

A műszer különböző analóg szenzorok mérési adatait jeleníti meg egy kör alakú TFT kijelzőn. A rendszer saját Wi-Fi-hálózatot és webes konfigurációs felületet biztosít.

A webes felület mobiltelefonról és számítógépről is használható, internetkapcsolat nélkül.

A projektet modulárisan kell megvalósítani, hogy később új szenzorokkal, kijelzőtípusokkal és CAN-kommunikációval bővíthető legyen.

## 2. Fejlesztői környezet

- IDE: Visual Studio Code
- Build rendszer: PlatformIO
- Framework: Arduino
- Programozási nyelv: C++
- Webes felület: HTML, CSS, JavaScript
- Kommunikáció: HTTP REST API és WebSocket
- Konfiguráció: JSON
- Fájlrendszer: LittleFS
- Beállítások tárolása: Preferences / NVS és LittleFS

A kód legyen moduláris, áttekinthető és dokumentált.

## 3. Hardver

### Végleges hardver

- Mikrokontroller: ESP32-WROOM-32U
- Kijelző: 1,53 hüvelykes, 360×360 pixel felbontású TFT
- Kijelzővezérlő: ST77916
- Kommunikáció: a kijelző pontos interfésze később ellenőrizendő
- Első valódi szenzor: Bosch 0 261 230 340 kombinált olajnyomás- és hőmérséklet-érzékelő

A Bosch szenzor pontos bekötését és karakterisztikáját adatlap alapján kell ellenőrizni.

### Ideiglenes hardver

A fejlesztés kezdetben egy másik ESP32 fejlesztőpanelen történik.

A pontos paneltípus még nincs meghatározva.

Az első verziónak kijelző és fizikai szenzorok nélkül is teljesen működnie kell.

## 4. Wi-Fi és webes felület

Az ESP32 saját, jelszóval védett Wi-Fi Access Pointot hoz létre.

Alapértelmezett hálózatnév: MX5-Gauge

Alapértelmezett IP-cím: 192.168.4.1

A webes felület legyen:
- Mobilbarát, reszponzív
- Modern, sötét témájú
- Gyors és könnyen kezelhető
- Internetkapcsolat nélkül használható
- iPhone Safari és asztali böngészőkkel kompatibilis

Minden konfigurációs funkció a webes felületről legyen elérhető.

## 5. Szenzorkezelés

A rendszer tetszőleges számú, a rendelkezésre álló memóriával és hardverrel összeegyeztethető logikai szenzort kezeljen.

Minden szenzor rendelkezzen:
- Egyedi azonosítóval
- Névvel
- Típussal
- Mértékegységgel
- Minimum és maximum értékkel
- Figyelmeztetési határértékekkel
- Engedélyezett/letiltott állapottal
- Adatforrással

### Támogatott adatforrások

1. Analóg ADC-bemenet
2. Virtuális demó szenzor
3. Később CAN-busz

A szenzor adatforrása és a kijelzőn történő megjelenítése egymástól független legyen.

## 6. Szenzorkarakterisztikák

A webes felületen lehessen egyedi szenzorprofilokat létrehozni.

Támogatott karakterisztikák:

### Lineáris

Két kalibrációs pont alapján történő átszámítás.

Példa:
- 0,5 V = 0 bar
- 4,5 V = 10 bar

### Többpontos

Tetszőleges számú kalibrációs pont.

A köztes értékek lineáris interpolációval számítandók.

### NTC

Ellenállás-hőmérséklet karakterisztika, táblázattal vagy Steinhart–Hart paraméterekkel.

A felhasznált felhúzó ellenállás értéke külön konfigurálható legyen.

A karakterisztikák a webes felületen grafikusan is megjeleníthetők legyenek.

## 7. Demó szenzorok

A rendszer támogasson virtuális szenzorokat, amelyekhez nem szükséges fizikai érzékelő.

A demó szenzorok ugyanolyan módon használhatók a műszerlapokon, mint a valódi szenzorok.

Konfigurálható paraméterek:
- Szenzor neve
- Mértékegység
- Minimum érték
- Maximum érték
- Változási sebesség
- Jelalak
- Engedélyezett állapot

Támogatott jelalakok:
- Szinuszos változás
- Háromszögjel
- Véletlenszerű ingadozás
- Állandó érték

A demóadatok automatikusan frissüljenek.

A webes előnézet és a későbbi fizikai kijelző ugyanazokat az aktuális értékeket használja.

## 8. Műszerlapok

A rendszer több különálló műszerlapot kezeljen.

Minden műszerlap konfigurálható legyen.

### Műszerlaptípusok

1. Köríves digitális
2. Hagyományos analóg mutatós
3. Kétértékes digitális
4. Minimalista digitális

### Konfigurálható tulajdonságok

- Műszerlap neve
- Megjelenítési stílus
- Elsődleges szenzor
- Másodlagos szenzor
- Színek
- Skálatartomány
- Figyelmeztetési színek
- Mértékegységek megjelenítése

A műszerlapok létrehozhatók, módosíthatók, törölhetők és átrendezhetők legyenek.

Az aktív műszerlap a webes felületről legyen kiválasztható.

## 9. Élő kijelző-előnézet

A webes felületen jelenjen meg egy kör alakú, 360×360 pixeles műszerlap-előnézet.

Két üzemmód szükséges.

### Élő előnézet

Az ESP32 aktuális megjelenítési állapotát mutatja.

A szenzorértékek automatikusan frissüljenek.

### Szerkesztési előnézet

A felhasználó a még nem mentett beállításokat is megtekintheti.

A módosítások mentésig ne változtassák meg az aktív műszerlapot.

Az előnézetet HTML Canvas vagy SVG segítségével kell megvalósítani.

A fizikai TFT és a böngészős előnézet azonos konfigurációt és mérési adatokat használjon.

## 10. Konfigurációk mentése

Minden konfiguráció maradjon meg újraindítás után.

Tárolandó adatok:
- Szenzorok
- Szenzorprofilok
- Kalibrációs adatok
- Demóadat-generátorok
- Műszerlapok
- Aktív műszerlap
- Kijelzőbeállítások
- Figyelmeztetési határértékek

A konfigurációk legyenek JSON-formátumban exportálhatók és importálhatók.

Mentés előtt történjen adatformátum- és értéktartomány-ellenőrzés.

## 11. Szoftverarchitektúra

A következő logikai modulokat különítsük el:

- WiFiManager: hálózatkezelés
- WebServer: webes felület és API
- SensorManager: szenzorok nyilvántartása
- SensorProfile: karakterisztikák
- DemoSensor: szimulált mérési értékek
- GaugeManager: műszerlapok kezelése
- DisplayManager: fizikai kijelző kezelése
- ConfigManager: konfigurációk mentése és betöltése

A DisplayManager kijelző nélküli üzemmódot is támogasson.

Az első fejlesztési fázisban a kijelzőkezelés nem szükséges.

## 12. Fejlesztési ütemezés

### 1. fázis: Alaprendszer

- PlatformIO projekt létrehozása
- ESP32 Wi-Fi Access Point
- HTTP webszerver
- LittleFS
- Alap webes kezelőfelület

### 2. fázis: Szenzorkezelés

- Szenzorok létrehozása és szerkesztése
- Demó szenzorok
- Demóadat-generálás
- Szenzorprofilok

### 3. fázis: Műszerlapok

- Műszerlap-szerkesztő
- Megjelenített értékek kiválasztása
- Műszerlapok közötti váltás
- Élő böngészős előnézet

### 4. fázis: Konfiguráció

- Tartós mentés
- Import/export
- Hibakezelés
- Beállítások visszaállítása

### 5. fázis: Fizikai kijelző

- ST77916 inicializálása
- Grafikus megjelenítés
- Szinkronizáció a webes előnézettel

### 6. fázis: Valódi szenzorok

- Bosch szenzor bekötése
- ADC-mérés
- Kalibráció
- Szűrés és hibadetektálás

## 13. Fejlesztési szabályok

- Ne készítsünk monolitikus programot.
- Minden funkció külön modulban legyen.
- A kijelző nélküli működés mindig maradjon támogatott.
- A webes felület és a firmware közötti kommunikáció dokumentált API-n keresztül történjen.
- Az új szenzortípusok hozzáadása ne igényelje a műszerlapok átírását.
- Kerüljük a felesleges külső függőségeket.
- Figyeljünk az ESP32 memória- és flashkorlátaira.
- A hibás konfiguráció ne akadályozza meg a rendszer indulását.
- A konfigurációs hálózatot védjük megfelelő hitelesítéssel.
- Az elsődleges cél a stabil, bővíthető rendszer, nem a funkciók egyszerre történő megvalósítása.

## 14. Aktuális feladat

Elsőként kizárólag az 1. fejlesztési fázist valósítsuk meg.

A konkrét ESP32 panel típusának ismeretében készítsük el a PlatformIO konfigurációt, a Wi-Fi Access Pointot, a webszervert és egy alap webes kezelőfelületet.

A későbbi funkciókat csak a szükséges interfészek és modulstruktúra szintjén készítsük elő.

A fejlesztést kis, külön tesztelhető lépésekben végezzük.