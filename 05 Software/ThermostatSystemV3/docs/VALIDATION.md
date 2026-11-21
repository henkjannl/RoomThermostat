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


## Remote layout update — 24 October 2026 (alpha.6)

Home status icons now precede the date/time. Updated original iconExit.png, iconOK.png and iconShowerOff.png copied from the artwork folder and compiled through the PNG override pipeline; idle hot water shows the dry shower head. Removed Automatic from More settings. Back is fixed at x=120,y=232 on all populated screens; editor row is minus, OK, back, plus. Home uses minus, menu, back, plus. Menu grids/lists and day selectors leave the bottom row clear.

Validation: thermostat, remote usb-test and battery builds passed. Native C++ navigation/command tests passed, including fixed back geometry, non-overlapping touch regions and visible idle shower artwork. Contract tests (5), architecture checks (14), timezone cases (56), PNG selection/alpha tests (2), and all 66 generated assets passed. Native framebuffer screenshots reviewed; overview at artifacts/remote-ui/updated-layout.png. No hardware upload or physical touch/LCD validation performed.


## Remote navigation and complete examples — 24 October 2026 (alpha.6)

Removed the home back button and all remote report navigation/actions; Telegram report support on the thermostat remains available. Home has three centered controls. Time, duration and sensor-offset editors use remote-previous/remote-next; comfort/night temperature controls retain minus/plus. All populated remote screens share the same status-icon renderer at the exact home coordinates. Titles and the unlimited-duration selector were repositioned to keep the status row clear. Removed the duplicate period subtitle that overlapped the relocated title.

Native C++ checks pass for navigation, fixed back placement, touch geometry, commands and identical status-row pixels across every page. Contract (5), architecture (14), timezone (56) and asset (66 images) checks passed. All three firmware builds passed. Exported and visually reviewed 29 examples from the actual firmware framebuffer, including every page, editor variants, confirmations and startup/error screens: artifacts/remote-ui/all-screens.html and all-screens-1.png through all-screens-3.png. Reproduce with tools/check_remote_native.cmd then tools/export_remote_gallery.py. No device upload or hardware validation performed.


## Separate back and cancel artwork — 24 October 2026 (alpha.6)

Imported the original iconBack.png and regenerated the shared flash assets. Menu navigation uses iconBack; edit/confirmation screens retain the red iconExit cross to discard/cancel, as agreed with the user (no separate iconCancel file was present). Both occupy the existing fixed bottom slot; home has neither. All 29 framebuffer examples regenerated; menu preview visually checked. Native controls, contract (5), architecture (14), timezone (56) and all 67 asset checks passed. No hardware upload or hardware validation.

Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Edit/cancel assets and radio-error recovery — 24 October 2026 (alpha.6)

Imported iconCancel.png and iconEdit.png from the user artwork. Edit/confirmation cancellation now uses iconCancel; Weekschema opens day editing with iconEdit. Added a clearly labelled Remote herstarten control on the radio-error screen, connected to a local ESP.restart callback in the application loop. It sends no thermostat command and needs no radio link. A radio error takes precedence over any populated snapshot. Preserved the recovery touch target across periodic unchanged repaints.

Native tests pass, including recovery-button hit testing, zero host commands during local restart, and preservation across unchanged repaint. Contract (5), architecture (14), timezone (56) and all 69 asset checks passed. Regenerated 29 examples; Weekschema and radio-error examples visually reviewed. Hardware restart/touch behavior has not been tested and no firmware was uploaded.
Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Startup status icons — 24 October 2026 (alpha.6)

All connecting, disconnected and radio-error screens now show the disconnected Wi-Fi/boiler icons and dry shower at the normal fixed header coordinates. Battery uses the same measured-voltage-derived status and existing 25% artwork steps as the main screen. Initial immediate wake feedback shows unknown battery (faded) until measurement completes, then redraws with the measured battery before logging/radio initialization. Battery changes/validity changes trigger startup-screen repaint without requiring a link change.

Native checks passed, including battery refreshes at 25%/75% on all three startup/error states and retention of the radio restart touch target. Contract (5), architecture (14) and timezone (56) checks passed. All 29 examples regenerated; connecting example visually reviewed. No hardware upload or hardware validation.
Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Unlimited button artwork — 24 October 2026 (alpha.6)

Imported the original iconUnlimited.png and replaced only the small unlimited-duration button artwork. Kept its 32-pixel render size, existing position and 50x48 touch target; all other button sizes unchanged. The large infinity value display remains a value indicator. Regenerated all 29 examples and reviewed the duration editor. Native controls, contract (5), architecture (14), timezone (56), and all 70 asset checks passed. No hardware upload or hardware validation.
Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Uniform office schedule font — 24 October 2026 (alpha.6)

Kantoor now uses the full-size original Noway Light18 font for its title, all four event labels and all four time values. Disabled per-label shrinking and small-value rendering for this page; existing icons, grid positions and touch geometry retained. Native navigation/controls checks, contract (5), architecture (14) and timezone (56) checks passed. Regenerated all 29 examples and visually checked example-office-times.png; all labels/times fit. No hardware upload or hardware validation.
Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Office time size correction — 24 October 2026 (alpha.6)

Kept all Kantoor event labels at full-size Noway Light18, and restored the time values to their earlier small rendering, as requested. Native controls, contract (5), architecture (14) and timezone (56) checks passed. Regenerated examples and visually checked the office screen. No hardware upload or hardware validation.
Firmware verification: thermostat, remote usb-test and remote battery builds all passed.


## Persistent Telegram users and /help — 24 October 2026

Thermostat version 3.0.0-alpha.7; remote remains alpha.6 and wire protocol is unchanged. Added /help, /toestaan CHAT_ID Naam, /gebruikers and /verwijderen CHAT_ID. The existing configured private chat remains the immutable owner; up to eight extra private users are persisted in a separate telegram_acl NVS namespace. Unknown private users receive only their own ID on /start or /help, with a global three-second response limit. Management requires both the private chat ID and sender ID to match the owner. All regular commands and callbacks pass authorization before conversation allocation. Removed users lose conversation state and cannot operate old keyboards or receive queued replies. Already accepted controller operations are not rolled back.

NVS drafts use bounded queues from the network worker to Core 1. Storage runs only while BoilerStep::Idle and opentherm.isReady(), and the live ACL and success reply change only after a successful save acknowledgement. Missing/corrupt records fall back to owner-only access; write errors retain the old live list. Names are bounded to 48 UTF-8 bytes; IDs are strictly positive decimal strings bounded to signed 64-bit range. Duplicate entries do not write flash. Owner credentials and thermostat settings records are unchanged.

Validation: thermostat (RAM 76,380 bytes; app flash 1,798,089 bytes), remote usb-test and remote battery builds passed. Native C++ tests exercise actual ACL policy/parser, command handler and NVS adapter against fake bot/queues/storage: owner/member separation, help, malformed/overflow IDs, capacity, rename/idempotence, reboot reload, corrupt records, failed writes, pending changes, removal and stale callbacks, and unknown-user response throttling passed. Contract tests (5), architecture checks (16) and timezone cases (56) passed. Build/host checks do not establish real Telegram delivery, physical NVS power-loss behavior or OpenTherm timing during access-list writes. No hardware upload performed. See docs/TELEGRAM-USERS.md for the one-time update and enrollment steps.


## Usual GitHub checkout synchronized - 24 October 2026

Installed the reviewed thermostat alpha.7 sources, Telegram users/help, remote alpha.6 UI changes, tests and documentation into the user's usual ThermostatSystemV3 checkout under Documents/GitHub/RoomThermostat/05 Software. Existing replaced files were backed up under the chat workspace artifacts/usual-project-backup/20261024-185613. Credentials, pairing keys, original artwork and existing checkout documentation were preserved. Assets regenerated from that checkout's current artwork (69 images); removed obsolete remote iconExit references from restart controls, which now use the circular restart symbol. Native test helper creates its output directory automatically.

All checks executed in the usual checkout: Telegram native integration tests, remote native UI tests, 5 contracts, 16 architecture checks, 56 timezone cases, 69 assets and all named remote UI assets passed. Exported all 29 screen examples. Thermostat, usb-test and battery builds passed. Both thermostat source and built firmware were verified to contain version 3.0.0-alpha.7. No device upload, commit or push performed.


## InfluxDB RAM logging — 1 November 2026 (thermostat alpha.8)

Added ten-second room/demand/setpoint sampling and direct OpenTherm DHW status/
validity transitions, plus DHW enable changes. Core 1 uses zero-wait bounded
queue operations; Core 0's existing worker performs batched HTTP writes and
backoff. No new flash writes, regulation changes, remote commands or wire format
changes. Missing configuration/token or queue allocation failure disables logging.

128 RAM records plus four pending points; oldest queued points are evicted and
counted on overflow. Pending bytes/timestamps survive HTTP failures until 204.
RAM does not survive a reset. About 21 minutes of periodic samples fit, fewer
with extra events. See INFLUX-LOGGING.md for timestamp/clock-correction behavior,
unknown-status semantics, retention failures and hardware acceptance checks.

Verification: enabled thermostat build passed (static RAM 78,484 bytes, app flash
1,802,929 bytes, dedicated token configured). Both CYD usb-test/battery
builds passed (protocol/UI unchanged). Native C++ executes the production capture,
overflow, formatter and retry worker against fake queues/HTTP/time: DHW repeated
status, valid-to-unknown-to-off recovery, stale status, queue eviction, offline/
unset-clock gating, failed/auth-rejected writes, exact retries across clock changes,
ACK advancement, backward time and invalid floats all passed. Architecture checks
(17), contract tests (5) and timezone cases (56) passed. The timezone suite used
the bundled runtime Python because PlatformIO Python lacks tzdata.

SSH to AsusHome as henkjan works with the locally generated key; InfluxDB health
reports pass, v2.9.1. The user lacks passwordless sudo/Docker access; no server
Docker/application configuration has been changed. A private token-entry helper
was placed under henkjan's home. The corrected dedicated token passed the real
write test: HTTP 204 for a separate room_thermostat_setup probe in Zonstraat35.
No fake room measurement was written. No commit or push performed.

Hardware follow-up, 1 November 2026: USB COM7 (CP210x serial 02048953) identified
the running alpha.7 thermostat with valid OpenTherm traffic. Uploaded alpha.8 via
PlatformIO; flash hash verification passed, no filesystem upload or full erase.
The running device reports alpha.8 and real Influx writes were acknowledged.
The user then stopped only the InfluxDB container for 60 seconds with automatic
restart. During the outage an Influx connection failure retained a pending batch
of four (observed retry interval 40 seconds); valid OpenTherm steps continued.
After restart acknowledgements rose 12 -> 16 -> 20 -> 21 and queued records
fell 8 -> 4 -> 1 -> 0. The observed OpenTherm maximum valid-status gap was
5,045 ms, maximum transmit gap 4,193 ms, late cycles zero; no reboot observed.
This validates a short database outage and backlog recovery on this device,
not all failure conditions. Router/Wi-Fi loss, cold offline boot, long buffer
overflow, power failure, real DHW transitions, and extended heap/stack behavior
remain separate acceptance checks. No electrical bus timing claim is made.

Grafana follow-up, 1 November 2026: created dashboard Kamerthermostaat, UID
room-thermostat-v3, using existing InfluxDB datasource dg2wdkyp71lvkc. Nine panels
cover latest values/age, temperature/setpoint, PID demand, requested boiler
temperature, DHW activity versus enable, and quality/CH enable. Default last hour,
Amsterdam timezone, 30-second refresh. Minimum ten-second graph interval and
nonempty aggregate windows preserve visible lines at the device's sample rate.
Verified in the browser with real values (room 20.4 C, setpoint 19.5 C, demand
0%, requested boiler temperature 5 C), DHW and quality timelines, and no panel
query errors. Reusable JSON and generator contain no credentials. No thermostat
firmware changes or uploads for this dashboard.
