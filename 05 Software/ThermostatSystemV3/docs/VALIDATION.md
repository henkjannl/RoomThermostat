# Validatie — 3.0.0-alpha.1

Datum: 23 oktober 2026.

## Builds geslaagd

| Firmware | Board / omgeving | Statische RAM | Applicatieflash |
|---|---|---:|---:|
| Thermostaat | mhetesp32minikit | 74.228 bytes (22,7%) | 1.738.057 bytes (82,9% van 2 MiB) |
| CYD USB-test | esp32-s3-devkitc-1, usb-test | 44.724 bytes (13,6%) | circa 790 kB (10,8% van 7 MiB) |
| CYD batterij | esp32-s3-devkitc-1, battery | 44.724 bytes (13,6%) | 790.277 bytes (10,8% van 7 MiB) |

Statische RAM sluit dynamische radio-, queue-, TLS- en displayallocaties uit. ESP-NOW-runtimegeheugen en invloed op OpenTherm zijn nog niet op het apparaat gemeten. De bestaande OneWire preprocessorwaarschuwingen uit de thermostaatdependencies blijven aanwezig.

## Uitgevoerde controles

- Tien bestaande architectuurcontroles uit V2.1.4, aangepast aan de nieuwe directorystructuur: geslaagd.
- Vijf host-contracttests: 56-byte berichtlayout, afwijzen van gewijzigde bytes, verkeerde sessie/request/lengte, batterij-CRC bij een beschadigd record en ringbufferomloop: geslaagd. Deze testen een onafhankelijk Python-formaatcontract; zij voeren geen ESP-NOW-firmware uit.
- 56 Amsterdam-tijdzonegevallen voor 2024–2030: geslaagd.
- 51 RGB565-assets gecontroleerd tegen originele JPEGs op afmetingen, pixelwaarden en rijbuffergrenzen: geslaagd.
- Vier gedeelde fonts byte-voor-byte gelijk aan originele VLW-bestanden; glyph-/bitmapgrenzen gecontroleerd: geslaagd.
- C++ static assertions op beide targets bevestigen 56-byte wire-packets en 32-byte batterijrecords.
- Handmatige codecontrole: callbacks doen uitsluitend vaste kopieën naar een queue en een atomische zendstatus-update; berichtauthenticatie en peermanagement lopen buiten callbacks. Er zijn geen schrijf-/instelcommando's in protocol v1.
- Bronconfiguratie van het thermostaatdisplay is naar een projectheader verplaatst. De build gebruikt die header voor alle TFT-eSPI-compilatie; de oude projectdependencies hoeven niet aangepast te worden.
- De nieuwe bronnen zijn apart aangemaakt. De oude projectmappen zijn niet overschreven of gemigreerd. Lokale pairing- en wifi/Telegram-configuratiebestanden zijn van Git uitgesloten.

## Niet uitgevoerd

Geen upload, radio-uitwisseling op echte hardware, scherm-/touchhoektest, spanningsmeting, slaapstroommeting, NVM-power-cut-test of batterijduurtest. Een succesvolle build bewijst geen kanaalherstel, ontvangstlatentie, standbyverbruik of USB-onderaan-oriëntatie. De fysieke rotation/touchmapping moet als eerste gecontroleerd worden.

De eerste CYD-UI gebruikt het ingebouwde Adafruit GFX-font. De gedeelde originele VLW-fonts en iconen zijn centraal beschikbaar en worden al door de thermostaat gebruikt; een gemeenschappelijke grafische renderer voor beide libraries is bewust geen onderdeel van deze eerste radioproef.

De batterijfile is 64 KiB bij volle ring, met filesystemoverhead erbuiten. Niet-geflushede RTC-records kunnen verloren gaan bij spanningsuitval. Het filesystem wordt alleen automatisch geformatteerd als de complete gereserveerde partitie gewist is; onbekende inhoud wordt behouden. CSV-export is tijdens een actieve USB-sessie beschikbaar.

Zie ACCEPTANCE.md voor de volgorde van hardwaretests. De beelden in artifacts/ zijn lokale, gekoppelde builds met privésleutels; niet publiceren. manifest.json bevat de bestandsgroottes en SHA-256-hashes.

## CYD uploadadrescorrectie — 23 oktober 2026

COM4-readback na eerdere uploads bevatte nog exact dezelfde ongeldige appheader op 0x20000 en applicatiedata in OTA-metadata op 0x19000. VS Code gebruikte deze GitHub-projectkopie; eerdere correcties stonden uitsluitend in de afzonderlijke ChatGPT-workspace.

Nu in deze projectkopie aangepast: remote-cyd/platformio.ini gebruikt board_upload.offset_address = 0x20000 en post:../tools/cyd_upload_layout.py. Het post-script verplaatst de Arduino OTA-metadataseed van 0xe000 naar 0x19000, zowel in FLASH_EXTRA_IMAGES als in de uiteindelijke esptool-uploadflags. Normaal USB-uploaden initialiseert daarmee ook de OTA-metadata; geen volledige flashwisactie nodig.

Beide CYD-builds geslaagd. Beide daadwerkelijke envdump-uitvoeren bevestigen app=0x20000 en OTA metadata=0x19000; oude offset 0xe000 ontbreekt. Vijf contracttests, tien architectuurtests en 56 tijdzonegevallen geslaagd. Dit is build-/hostvalidatie: de gecorrigeerde firmware is nog niet door deze chat geüpload en een succesvolle hardwarestart is nog niet bevestigd. Eerdere uploads op 0x10000 kunnen NVS hebben beschadigd; het filesystem wordt door deze correctie niet gewist.

## Wake-versiebanner — 23 oktober 2026

CYD remote 3.0.0-alpha.1 staat nu direct op het wakescherm en bovenaan iedere statusrefresh. Serieel wordt de banner na USB-verbinding na 500 ms en 2500 ms gemeld; geen blokkerende wachtlus of verlenging van de normale actieve periode. Het commando version toont versie en buildomgeving. Een ontbrekend batterijlog wordt bij eerste start gecontroleerd met exists voordat het voor lezen wordt geopend. Beide CYD-builds en 71 hostcontroles geslaagd in de staging-workspace; bronbestanden hash-gecontroleerd overgenomen. Hardwaregedrag na upload nog te bevestigen.


## LCD DC/CS-correctie — 23 oktober 2026

Foto toont de oude batterijmonitorinterface terwijl COM4-readback en ELF-identiteit de nieuwe usb-testfirmware bevestigen. OTA-record op 0x19000 selecteert app0 (sequence 1). Concrete bronfout gevonden: Adafruit_ILI9341(SPIClass*, dc, cs, rst) kreeg LCD_CS en LCD_DC omgekeerd. Pinvolgorde gecorrigeerd naar LCD_DC, LCD_CS. Daardoor kon de nieuwe firmware de LCD niet correct beschrijven; achtergebleven paneelinhoud is de waarschijnlijke verklaring voor het oude scherm. Beide CYD-builds, thermostaatbuild en 71 hostcontroles geslaagd in de staging-workspace. Correctie in deze GitHub-projectkopie toegepast; hardwarebevestiging na upload nog nodig.


## ESP-NOW ontvangstcorrectie — 23 oktober 2026

Gebruiker bevestigt V3 op thermostaat, ketel/temperatuur actief en beide temperaturen op remote, maar trage en soms mislukte verbinding. Thermostaat gebruikte standaard Wi-Fi-modemslaap (WIFI_PS_MIN_MODEM). Espressif waarschuwt voor gemiste ESP-NOW-ontvangst met modem-sleep bij een AP-verbonden station: https://github.com/espressif/esp-idf/blob/master/examples/wifi/espnow/README.md . WiFi.setSleep(false) toegevoegd in networkWorker na setupWifi, met foutmelding bij mislukken. Dit is een plausibele oorzaak; radiometing moet de verbetering bevestigen. Extra verbruik van de thermostaat is niet gemeten. Remote, diepe slaap, actieve tijd, protocol en OpenThermcyclus blijven gelijk.

Alle drie builds en 71 hostcontroles geslaagd in staging-workspace; aangepaste netwerkheader hash-gecontroleerd overgenomen. Thermostaatapp 1.738.417 bytes. Alleen thermostaat opnieuw uploaden. Daarna twintig wakecycli testen en tijd tot verse status/mislukkingen registreren. Eerste kanaalzoekactie kan nog seconden duren. Nog geen hardwarebevestiging van deze ontvangstcorrectie.


## Remote-UI in thermostaatstijl — 23 oktober 2026

Gebruiker bevestigt vier wakecycli met vlotte verbinding na uitschakelen van modemslaap op thermostaat; twintigcyclustest nog niet voltooid.

Remote-UI vernieuwd: wit, originele gedeelde Noway Light18/Medium24/Medium60-fonts en temperatuur-/setpoint-/vlamiconen. Grote kamertemperatuur, apart setpointvak, compacte verbinding/batterij en blijvend versienummer. Oude gegevens zijn grijs en expliciet oud; geen oude verwarmingsstatus als actueel. Alleen-lezen: geen nieuwe instelknoppen of protocolwijzigingen. Overal tikken houdt wakker, geen hoekknoppen. Bereikbare hoekmetingen gebruiker: TL (38,26), TR (208,29), BL (36,288), BR (213,297). Reserveer grote toekomstige touchvlakken binnen dit bereik.

Directe VLW-renderer uit originele flashbytes, zonder heapfonttabellen of framebuffer. Graadteken als cirkel omdat originele fonts dit missen. Alleen gewijzigde panelen worden opnieuw getekend. Geen wijziging in radio of slaaptiming; werkelijk verbruik/rendertijd nog niet gemeten.

Stagingvalidatie: drie builds, 71 hostcontroles, 51 artworkchecks en vier fontarraychecks geslaagd. CYD RAM 44.748 bytes, appflash circa 1.203 kB (16,4% van 7 MiB). tools/preview_remote_ui.py rendert layoutvoorbeelden met originele VLW-fonts en controleert glyphgrenzen, inclusief extreme waarden; geen hardwaretest. Vier bron-/toolbestanden hash-gecontroleerd naar deze GitHub-kopie overgenomen. Alleen remote opnieuw uploaden, geen filesystemupload. Visuele controle op echte LCD na upload nog nodig.

## IPS-kleurinversie — 23 oktober 2026

Gebruiker meldt zwarte achtergrond en omgekeerde kleuren bij de witte UI. screen.invertDisplay(true) toegevoegd direct na screen.begin, zodat de paneelinversie bij elke start en deep-sleep-wake expliciet wordt ingesteld. RGB565-kleuren en originele assets blijven gelijk. 71 hostchecks geslaagd; hardwarecontrole van wit, tekst- en icoonkleuren na upload blijft nodig.



## Bedienbare remote alpha.2

Zie VALIDATION-ALPHA2.md en UPLOAD-ALPHA2.md voor bouwresultaten, hardwarebeperkingen en uploadvolgorde.


## Thermostat alpha.3

24 October 2026: see NETWORK-ALPHA3.md for the Telegram timeout fix, diagnostics, build/host checks and hardware follow-up. Remote alpha.2 remains compatible.


## Remote alpha.3

See REMOTE-ALPHA3.md for the domestic-hot-water icon correction, native pixel tests and upload instructions.


## Matching flame icons alpha.4

See FLAME-ALPHA4.md. Both devices require alpha.4 / protocol v3. Host pixel tests cover all nine levels. Earlier alpha.3 network and shower fixes are retained.


## Updated icons alpha.5

See ICONS-ALPHA5.md for PNG override/alpha handling, regenerated shared assets and build/host checks. Both devices report alpha.5. User artwork exports are preserved.


## Remote wake alpha.6

See WAKE-ALPHA6.md: waiting screen remains until first accepted snapshot. Host regression covers startup/failure/retry/first-status transitions. Thermostat alpha.5 and custom icons are unchanged.
