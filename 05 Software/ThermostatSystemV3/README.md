# Thermostat System V3 — 3.0.0-alpha.2

Bedienbare remote met twee afzonderlijke PlatformIO-projecten. Alpha.2 gebruikt protocol v2 en moet op beide apparaten worden geüpload. Zie [uploadvolgorde](docs/UPLOAD-ALPHA2.md) en [testresultaten](docs/VALIDATION-ALPHA2.md). De bestaande thermostaat V2.1.4 en CYD-batterijmonitor v0.1.0 buiten dit project zijn niet gewijzigd. Alpha.2 is gebouwd en op de ontwikkelcomputer getest; hardwarevalidatie volgt na upload.

## Mappen

- `thermostat/`: V2.1.4-regelaar plus ESP-NOW-statusservice; OpenTherm blijft op vijf seconden.
- `remote-cyd/`: ES3C28P in portrait 240×320, wake op FT6336G/GPIO17, status en batterijpercentage, slaap na 15 seconden zonder touch.
- `shared/protocol/include/`: vast berichtformaat, authenticatie en ESP-NOW-transport.
- `shared/assets/icons/` en `fonts/`: originele gedeelde artwork/lettertypen; gegenereerde flashheaders staan in `shared/assets/include/`.
- `shared/ui/include/`: gedeelde labels en basiskleuren. De remote rendert de originele VLW-fonts en bitmapiconen rechtstreeks vanuit flash. Schermlayouts en drivers blijven apart.
- `tools/`: sleutels en assets genereren; `tests/`: contract- en tijdzonetests; `docs/`: ontwerp en acceptatiechecklist.

## Wat de proef doet

De CYD toont een verbindingsscherm voordat batterijmeting, flashlog of radio start. Hij probeert eerst het laatst gebruikte wifi-kanaal, zoekt vervolgens op Nederlandse kanalen 1–13 (maximaal acht seconden per zoekronde), en vraagt actuele temperaturen, instellingen, dagtypen, ketel- en warmwaterstatus op. Eerder ontvangen gegevens worden na vijf seconden als oud behandeld; wijzigen kan alleen bij actuele communicatie. Touch verlengt de actieve periode. De eerste wake-touch wordt genegeerd tot loslaten; min/menu/plus en de hiërarchische menu's bieden bediening.

De thermostaat blijft eigenaar van alle regeling. Radiocallbacks kopiëren alleen vaste berichten naar een korte queue. Core 1 verwerkt maximaal twee berichten per loop, onafhankelijk van blokkerende Telegram-aanroepen. Protocol v2 valideert en bevestigt wijzigingen met boot-/sessie-/opdracht-ID, revisie en vervaltijd; identieke herhalingen worden niet dubbel toegepast. De status wordt rechtstreeks uit Core-1-state opgebouwd; nooit uit de mogelijk oudere Telegram-snapshot.

## Koppeling en sleutels

Voor deze ene set zijn lokale privésleutels aangemaakt in `shared/protocol/include/pairing_secret.h`. Beide firmwares moeten met hetzelfde bestand worden gebouwd. Het bestand is uitgesloten van Git: maak zelf een veilige reservekopie. Deel ook gebouwde firmware niet publiekelijk: de sleutels zitten daarin. Een schone checkout heeft `python tools/generate_pairing_secret.py` nodig; nieuwe sleutels vereisen opnieuw bouwen/uploaden van beide apparaten. Het script overschrijft bestaande sleutels nooit.

Discovery bevat geen statusgegevens en is HMAC-geauthenticeerd. Statusverzoeken/-antwoorden gebruiken versleutelde unicast en een HMAC. Een willekeurige sessie per wake en oplopende requestnummers voorkomen dat de CYD oude antwoorden als nieuw toont. Geen publieke pairingmodus of beheerinterface; uitsluitend apparaten met de gedeelde sleutels kunnen deelnemen. Voor uitbreiding naar meerdere remotes zijn afzonderlijke peers/sleutels nodig.

ESP-NOW gebruikt op de thermostaat het bestaande wifi-kanaal. De CYD logt niet in op het accesspoint. Tijdens scans/reconnects van de thermostaat kan een antwoord wegvallen; de CYD probeert opnieuw. Dit kan niet uitsluitend met een compiletest worden gevalideerd.

## Bouwen

Voer vanuit deze hoofdmap uit, met PlatformIO in PATH:

```powershell
pio run --project-dir thermostat
pio run --project-dir remote-cyd -e usb-test
pio run --project-dir remote-cyd -e battery
```

De lokale installatie staat eventueel op `C:\Users\henkj\.platformio\penv\Scripts\platformio.exe`. Beide projecten pinnen Espressif32 6.10.0 / Arduino ESP32 2.0.17. De thermostaat heeft een project-eigen `include/tft_setup.h`; er hoeven geen dependencybestanden te worden aangepast.

## Uploaden en eerste test

1. Behoud een werkende V2.1.4-build voor terugkeer. Bouw/upload eerst de **V3-thermostaat**, daarna de CYD. De bestaande V2.1.4 kan nog niet met de remote communiceren.
2. Upload de thermostaatfirmware via PlatformIO; **geen filesystemupload**. De bestaande configuratie en instellingen blijven in het apparaat. `thermostat/data/config.jsn` is alleen een lokale kopie en is uitgesloten van Git.
3. Test de CYD eerst met `usb-test`, bijvoorbeeld `pio run --project-dir remote-cyd -e usb-test -t upload --upload-port COMx`. Selecteer de werkelijk aangesloten poort. De bestaande partitionering is behouden; wis niet de hele flash.
4. Controleer USB onderaan, vier touchhoeken via seriële `Touch portrait x=... y=...`-regels, actuele temperaturen, slapen en herhaald wekken. `PORTRAIT_ROTATION=0` en de expliciete touchmapping staan in `remote-cyd/include/config.h`; fysieke oriëntatie moet op dit paneel bevestigd worden. Bij 180° afwijking zijn rotatie 2 en beide touchspiegelingen nodig.
5. Bouw/upload `battery` voor de duurtest, verwijder USB en reset. Deze build markeert batterijrecords als batterijbedrijf; de USB-build niet. Percentage is een spanningsschatting, geen coulombmeting.

Er is niets naar de apparaten geüpload. De `.bin` onder `.pio/build/<omgeving>/` is een app-image; gebruik PlatformIO Upload voor de juiste bootloader/partitie-offsets, vooral bij de eerste CYD-upload.

CYD-uploadcorrectie (23 oktober 2026): `board_upload.offset_address = 0x20000` en `tools/cyd_upload_layout.py` zetten de app en Arduino OTA-metadataseed expliciet op de adressen uit de custom partitietabel (`0x20000` en `0x19000`). De Arduino-uploader van platform 6.10.0 gebruikt hiervoor anders `0x10000` en `0xe000`; een upload kan dan slagen terwijl de bootloader blijft resetten. Behoud het post-script bij uploads vanuit VS Code. Een normale USB-upload met deze correctie herstelt ook de OTA-metadata; er is geen volledige flashwisactie nodig. Controleer na upload de boot op COM4 (115200 baud). Eerdere uploads kunnen NVS al beschadigd hebben; de filesystempartitie wordt niet gewist.

## Batterijhistorie

De oude NVS-namespace blijft behouden. V3 gebruikt maximaal 2.048 records van 32 bytes in `/battery-v1.bin` (64 KiB file plus LittleFS-overhead). Elke record bevat versie/magic, volgnummer, sessie, verstreken RTC-tijd, actieve duur, millivolts, geschat percentage, flags en CRC32.

Normale logging gebeurt hoogstens eens per 15 minuten bij een gebruikers-wake; er zijn geen timer-wakes voor logging. Cold starts, een spanningsstijging van >50 mV ten opzichte van het laatste record en veranderingen in geldigheid/verbinding/USB-profiel zijn extra events. Een RTC-buffer bewaart maximaal acht nog niet geschreven records; schrijven gebeurt bij vier records, bij het eerstvolgende geschikte moment na een uur, of op cold start. Daardoor kunnen bij volledige stroomuitval nog niet opgeslagen metingen verloren gaan. Een gedeeltelijk geschreven record wordt via CRC verworpen. De opgeslagen tijd is **verstreken tijd per sessie**, geen kalenderdatum; de radiotijd wordt niet in deze klok gezet.

LittleFS wordt alleen automatisch geformatteerd als de hele daarvoor bestemde partitie aantoonbaar gewist is. Bij bestaande onbekende data of een mountfout blijft die behouden en meldt het scherm een logprobleem. Formatteer niet zomaar: exporteer/bewaar eerst gegevens. Met het seriële commando `history` worden pending records opgeslagen en geldige records als CSV geëxporteerd; sorteervolgorde is de fysieke ring, sorteer voor analyse op volgnummer. USB verbreekt bij deep sleep: stuur het commando tijdens de actieve periode.

De tien dagen uit de batterijmonitor blijven alleen een uitgangsmeting. Radioactiviteit, helderheid en gebruiksfrequentie zijn veranderd. Meet opnieuw aan de batterijaansluiting, zonder USB.

## Bronnen en behoud

Thermostaatbasis: `RoomThermostat/05 Software/Roomthermostat V2/Roomthermostat V2.1`, software 2.1.4. CYD-basis: `cyd-battery-monitor-v0.1.0` in de bestaande ES3C28P-componentenmap. De bestaande hardware-slaapcode en batterijspanningscurve zijn hergebruikt; voorspelde resterende dagen zijn bewust nog niet overgenomen naar deze andere belasting.

- https://www.lcdwiki.com/2.8inch_ESP32-S3_Display
- https://docs.espressif.com/projects/esp-idf/en/v4.4.7/esp32/api-reference/network/esp_now.html
- ESP-IDF/Arduino, TFT_eSPI, JPEGDecoder en Adafruit behouden hun eigen licenties. Gedeelde originele artwork/fonts zijn uit het bestaande lokale project overgenomen; deze repository is niet gepubliceerd.
