# MX-5 Smart Gauge

ESP32-alapú, kijelző nélkül is használható autós műszerprototípus. Saját Wi-Fi hozzáférési pontot és mobilbarát webes konfigurációs felületet biztosít szenzorok, kalibrációs profilok és műszerlapok kezeléséhez.

> A projekt jelenleg fejlesztési prototípus. Járműben vagy biztonságkritikus mérésre még nem használható.

## Jelenlegi funkciók

- `MX5-Gauge` Wi-Fi AP és webes felület a `http://192.168.4.1` címen
- virtuális demo- és analóg ADC-szenzorok
- lineáris, többpontos és NTC Steinhart–Hart profilok
- vezetett „Új szenzor” varázsló
- élő szenzorértékek és Canvas műszer-előnézet
- létrehozható, szerkeszthető, törölhető és átrendezhető műszerlapok
- köríves digitális, analóg mutatós, kétértékes digitális, minimalista, két félköríves, négyértékes és kétoldalas félkörös analóg műszer
- műszeren belül szenzoronként állítható tartomány, skálairány és tipográfia
- LittleFS-ről kiszolgált, internet nélkül működő webalkalmazás

## Hardver és környezet

A jelenlegi PlatformIO célpanel: `esp32cam` (AI Thinker ESP32-CAM). A végleges tervezett hardver ESP32-WROOM-32U és 360×360 pixeles ST77916 kijelző; a fizikai kijelző kezelése még nincs implementálva.

Szükséges eszközök:

- Visual Studio Code + PlatformIO, vagy PlatformIO Core
- ESP32-kompatibilis USB–soros átalakító
- stabil 5 V-os táp az ESP32-CAM modulhoz

## Fordítás és feltöltés

```powershell
pio run -e esp32cam
pio run -e esp32cam -t upload --upload-port COM3
pio run -e esp32cam -t buildfs
pio run -e esp32cam -t uploadfs --upload-port COM3
```

ESP32-CAM programozási mód:

1. IO0 összekötése GND-vel.
2. Modul újraindítása vagy rövid áramtalanítása.
3. Firmware feltöltése.
4. Szükség esetén ismételt reset programozási módban.
5. LittleFS feltöltése.
6. IO0 leválasztása a GND-ről, majd normál újraindítás.

A projektben az upload sebessége a stabilabb `115200` baud értékre van állítva.

## Alapértelmezett hálózat

- SSID: `MX5-Gauge`
- jelszó: `mx5gauge`
- cím: `http://192.168.4.1`

Az alapértelmezett jelszót éles használat előtt meg kell változtatni az `include/app_wifi_manager.h` fájlban. Nyilvános repóban ne tárolj valódi hozzáférési adatot.

## Projektstruktúra

```text
data/       LittleFS webalkalmazás
include/    firmware fejlécek és adatmodellek
src/        ESP32 firmware modulok
test/       későbbi automatizált tesztek helye
PROJECT_SPEC.md    eredeti részletes specifikáció
CURRENT_STATUS.md  átadási állapot és folytatási jegyzetek
```

## Fontos korlátozások

- A konfigurációk jelenleg RAM-ban élnek, újraindítás után az alapértékek töltődnek vissza. NVS/LittleFS perzisztencia még szükséges.
- Nincs konfiguráció import/export.
- Nincs WebSocket; az élő webes adatok HTTP lekérdezéssel frissülnek.
- A fizikai ST77916 kijelző és CAN-busz nincs implementálva.
- A valós Bosch szenzor bekötése, karakterisztikája, szűrése és hibadetektálása még ellenőrzendő.
- Az API jelenleg csak az AP jelszavával védett, külön HTTP-hitelesítés nincs.

A részletes aktuális állapotot és a folytatás pontos lépéseit lásd a [CURRENT_STATUS.md](CURRENT_STATUS.md) fájlban.

