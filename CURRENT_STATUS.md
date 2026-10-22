# Fejlesztési állapot – 2026-10-22

## Hol tart a projekt?

Az 1–3. prototípusfázis működő kódja elkészült: Wi-Fi AP, LittleFS webalkalmazás, szenzor- és profilkezelés, demóadatok, műszerlap-kezelés és élő böngészős előnézet. Fizikai kijelző még nincs a rendszerben.

Legutóbb elkészült fejlesztések:

- szenzoronként lenyitható műszerszerkesztő;
- minden használt szenzorhoz külön minimum, maximum, skálakezdet és irány;
- szenzoronként külön név-, érték- és mértékegység-méret, valamint X/Y eltolás;
- két félköríves műszer rögzített bal és jobb oldali geometriával;
- négyértékes műszer szenzoronkénti tipográfiával;
- új „Kétoldalas analóg mutatós” típus, két külön félkörskálával;
- régi vagy hibás 0%-os tipográfiai értékek webes normalizálása 100%-ra;
- a régi közös tipográfiai mezők elrejtésének javítása.

Mind a firmware, mind a LittleFS-kép sikeresen lefordul:

```text
RAM:   16.0% (52360 / 327680 byte)
Flash: 36.8% (1156179 / 3145728 byte)
```

## A tesztmodul állapota

A firmware legutóbbi sikeres feltöltése megtörtént, és tartalmazza a `DualAnalog` firmware-típust. A legutóbbi javított webes LittleFS-képet viszont már nem sikerült feltölteni.

Diagnosztikai eredmények:

- az ESP32 ROM bootloader elérhető a COM3 porton;
- a chip típusa és MAC-címe kiolvasható;
- a flash strapping feszültsége 3,3 V;
- a flash azonosító `Manufacturer: ff`, `Device: ffff`;
- normál stubbal és `--no-stub` ROM módban sem válaszol a flash;
- a feltöltés `Failed to communicate with the flash chip` hibával áll le.

Következtetés: a tesztpanelen hardveres, tápellátási vagy belső flash-kapcsolati hiba valószínű. A LittleFS partíció a sikertelen törlési próbák miatt sérült vagy üres lehet. A fejlesztés az éleshez szánt új modul megérkezésekor folytatandó.

## Folytatás az új modulon

1. Ellenőrizni az új panel pontos típusát és flashméretét.
2. Szükség esetén módosítani a `platformio.ini` board-beállítását.
3. Csupasz panelen `flash-id` diagnosztika.
4. Firmware teljes feltöltése.
5. Reset programozási módban, majd LittleFS feltöltése.
6. IO0 leválasztása és normál újraindítás.
7. Webes smoke test minden műszertípussal.
8. Kiemelten ellenőrizni:
   - nincs dupla tipográfiai mező;
   - a két félkör bal és jobb íve is látható;
   - a 2–4. szenzor szövegmérete alapból 100%;
   - a két analóg mutató külön félkörön mozog;
   - minden szenzor minimuma, maximuma és iránya menthető.

## Következő fejlesztési prioritások

1. Tartós konfigurációmentés NVS/LittleFS használatával és verziózott migrációval.
2. Konfiguráció export/import.
3. Automatizált egység- és API-tesztek.
4. A webes rajzolás szélsőérték- és reszponzív tesztjei.
5. Végleges ESP32-WROOM-32U lábkiosztás.
6. ST77916 kijelzőillesztés.
7. Bosch 0 261 230 340 szenzor adatlap, illesztés, kalibráció és szűrés.
8. AP jelszó és további konfigurációs hitelesítés biztonságos kezelése.

## Ismert technikai megjegyzések

- A szenzorok, profilok és műszerek jelenleg fix méretű memóriatömbökben élnek.
- A `GaugeConfig` tartalmaz régi közös mezőket is a kompatibilitás miatt; az új felület a `slots[4]` szenzoronkénti adatokat használja.
- A webalkalmazás tömör, egysoros JavaScript függvényeket is tartalmaz; később érdemes modulokra bontani és formázni.
- A projekt jelenleg fejlesztési célra fix Wi-Fi hitelesítő adatot használ.

