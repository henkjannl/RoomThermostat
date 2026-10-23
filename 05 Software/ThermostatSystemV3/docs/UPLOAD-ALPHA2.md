# Upload 3.0.0-alpha.2

Dit is de eerste bedienbare remoteversie met het goedgekeurde menuontwerp.
Instellingen en status komen van de thermostaat; er staan geen voorbeeldmetingen in.
Na een dagtype- of duurwijziging binnen Meer dagen keer je terug naar Meer dagen.

## Uploadvolgorde in Visual Studio Code

1. Open `C:\Users\henkj\Documents\GitHub\ThermostatSystemV3\thermostat`.
   Voer PlatformIO **Upload** uit voor `mhetesp32minikit`.
2. Controleer dat de thermostaat `3.0.0-alpha.2` meldt en normaal werkt.
3. Open `C:\Users\henkj\Documents\GitHub\ThermostatSystemV3\remote-cyd`.
   Kies `usb-test` en voer PlatformIO **Upload** uit. In de uitvoer moet staan:
   `CYD upload layout: app=0x20000, OTA metadata=0x19000`.
4. Wek de remote en open Menu → Instellen → Meer → Status. Onderaan staat alpha.2.

Beide apparaten moeten alpha.2 hebben: protocol v2 is niet uitwisselbaar met alpha.1.
Gebruik gewoon Upload; geen Erase Flash en geen Upload Filesystem. Bestaande
thermostaatconfiguratie, instellingen en batterijhistorie blijven daarmee behouden.
Sluit een eventuele seriële monitor wanneer die de uploadpoort bezet houdt.

`usb-test` ondersteunt dezelfde slaapstand en bediening als `battery`. Het verschil
is de markering van batterijmetingen als extern gevoed of zelfstandig batterijgebruik.
Gebruik voor de eerste tests `usb-test`; kies `battery` voor daadwerkelijke
ontlaadmetingen zonder USB-voeding. Er zijn geen nieuwe periodieke wakes toegevoegd.

## Eerste controle op de echte apparaten

- Wek met één tik. Laat los; pas een volgende tik mag bedienen. Vasthouden mag geen
  herhaalde wijziging veroorzaken.
- Vergelijk kamer- en doeltemperatuur, zeven dagtypen en hun rode markeringen met
  de thermostaat. Wifi betekent een actuele remote-thermostaatverbinding.
- Wijzig de doeltemperatuur één stap en controleer de echte thermostaat. Herstel daarna
  desgewenst Automatisch via Instellen → Meer.
- Controleer bij Meer dagen dat dagtype, duur, Onbeperkt en Automatisch goed werken
  en terugkomen in Meer dagen. Terug tijdens bewerken moet niets opslaan.
- Controleer bij tijden/temperaturen de huidige waarde en of OK wordt bevestigd.
- Vergelijk douche- en ketelstatus tijdens werkelijk warmwatergebruik.
- Controleer na slaap opnieuw verbinding, kleuren en touch. Meet het batterijverbruik
  later opnieuw; de impact van het nieuwe scherm en radioberichten is nog niet gemeten.

Bij een verloren bevestiging staat **Onbevestigd; controleer instelling**. De remote
vraagt opnieuw status op en herhaalt die wijziging niet onder een nieuwe opdracht-ID.
Bij een gelijktijdige wijziging via Telegram of de fysieke thermostaat moet de editor
opnieuw worden geopend. Alleen OK/bevestigen verzendt een menu-instelling.

Rapporten worden na bevestiging door de thermostaat aangevraagd voor het bestaande
Telegram-chatadres. **Aangevraagd** bevestigt opname in de wachtrij, niet Telegram-aflevering.
Een herstart wacht op een veilige OpenTherm-pauze en bewaart gewijzigde instellingen.
Normale wijzigingen volgen de bestaande uitgestelde NVS-opslag, uiterlijk bij de
volgende veilige opslagcyclus (ongeveer één minuut); spanningsuitval kan tussentijdse
wijzigingen verliezen.

De lokale firmwarebestanden staan onder `artifacts`, met SHA-256 in `manifest.json`.
Die beelden bevatten de lokale koppelingssleutels en zijn niet voor publicatie.
