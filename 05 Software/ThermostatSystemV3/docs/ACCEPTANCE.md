# Eerste hardwareproef

Versies op beide apparaten: `3.0.0-alpha.1`. Test eerst met toezicht en behoud de werkende V2.1.4-build.

1. Upload alleen de V3-thermostaatfirmware. Controleer lokale bediening, normale temperatuurregeling, Telegram, minimum heap en geldige OpenTherm-statusgaten voordat de remote wordt toegevoegd.
2. Upload CYD `usb-test`. Controleer dat het startscherm verschijnt vóór de netwerkstatus en dat USB fysiek onderaan zit. Test de vier hoeken: circa (0,0), (239,0), (0,319), (239,319). Seriële touchcoördinaten helpen de expliciete mapping in config.h te controleren. Deze versie heeft nog geen touchknoppen die instellingen wijzigen.
3. Controleer kamertemperatuur en setpoint naast de thermostaat, en verander het setpoint op de thermostaat zelf. De remote vraagt tijdens actief gebruik ongeveer iedere 1,5 seconde status; controleer de update.
4. Laat de CYD liggen: circa 15 seconden na de laatste aanraking gaat hij slapen. Wek minstens twintig keer door touch, ook na enkele uren slaap. Een vastgehouden aanraking moet geen resetlus veroorzaken. Test ontwaken na een eerdere niet-bereikbare thermostaat.
5. Zet de thermostaat uit: de CYD moet stoppen met zoeken, 'Niet bereikbaar' of 'Oude status' tonen en weer slapen. Vervang geen ontbrekende waarden door nul. Zet de thermostaat weer aan en test herstel.
6. Test terwijl Telegram bezig is met een rapport en terwijl wifi reconnect. De thermostaat mag voor de remote nooit handmatig naar een ander kanaal schakelen. De remote moet een gewijzigd accesspointkanaal opnieuw kunnen vinden. Discovery is getest voor de Nederlandse kanaalset 1–13 in code, niet op radiohardware.
7. Test CYD `battery`, verwijder USB en reset. Controleer batterijpercentage tegen gemeten spanning en meet slaap-/actiefstroom aan de batterijconnector. De bestaande tien dagen zijn geen verwachting voor deze nieuwe radiobelasting.
8. Stuur via USB tijdens de actieve periode `history` gevolgd door Enter. Controleer CSV, actieve duur, millivolts en flags. USB-testrecords hebben bit0 uit. Controleer dat na meer dan 15 minuten een nieuw gewoon record wordt opgenomen, dat de data na reset behouden blijft en dat RTC-bufferverlies bij spanningsuitval geen opgeslagen records corrupt maakt.
9. Controleer tegen het einde van de proef opnieuw de thermostaatstatus: heap, touch-respons en maximale geldige boiler-statusgaten. Compiletests bewijzen geen radio- of realtimegedrag.

## Bij problemen

- Geen verbinding: beide nieuwe firmwares en dezelfde lokale pairing_secret.h nodig; V2.1.4 ondersteunt de link nog niet. Let op afstand, AP-kanaal en reconnects.
- Verkeerde oriëntatie: pas PORTRAIT_ROTATION en touchmapping samen aan en herhaal de hoektest. Geen hardwareconclusie trekken uit alleen de build.
- 'Log niet beschikbaar': de software weigert een onbekende bestaande filesysteminhoud te wissen. Inspecteer/backup eerst; de bestaande batterij-NVS is een andere partitie en wordt niet gewist.
- Lange wake-up: meet scherm-aan, touchinitialisatie, eventuele eerste filesysteminitialisatie en eerste antwoord apart. Een cold start kan langer duren dan een volgende deep-sleepwake.

Instellingen wijzigen via de remote is de volgende fase. Die vereist expliciete commandobevestigingen, bereikvalidatie, deduplicatie en tests van gelijktijdige bediening via CYD/Telegram/lokale knoppen; dit protocol accepteert die opdrachten nog niet.
