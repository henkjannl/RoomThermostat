# Bedienbare remote alpha.2 — 23 oktober 2026

Protocol v2, versie 3.0.0-alpha.2 op beide apparaten. Dit vervangt de eerdere
alleen-lezen radioproef. Uploadvolgorde en hardwarecontrole: UPLOAD-ALPHA2.md.

## Uitgevoerd op de ontwikkelcomputer

- PlatformIO: thermostat/mhetesp32minikit, remote-cyd/usb-test en remote-cyd/battery.
- C++-hosttest van de echte `remote_protocol.h` en `remote_menu.h`, met een minimale
  raster-/LCD-testadapter: alle menuschermen, grote aanraakvlakken zonder overlap,
  één vaste terugknop, terugkeer Meer dagen na dagtype/duur/Automatisch, huidige waarden,
  lokale drafts, opslaan en annuleren, offline blokkering en onzekere bevestiging.
- Dezelfde C++-test controleert grenswaarden, tijdvolgorde, weekindex/dagtype,
  comfort/nachtrelatie, sensorcorrectie, Onbeperkt/Automatisch en opdrachtvalidatie:
  verkeerde boot-ID, verlopen lease, oude revisie/ID, identieke herhaling en gewijzigde
  inhoud bij dezelfde ID. Een identieke herhaling krijgt het gecachte antwoord.
- Vijf onafhankelijke draadformaat-/HMAC-/batterijringtests: v2 heeft 123 bytes,
  inclusief 16-byte HMAC-tag; iedere eenbytewijziging in het testbericht wordt afgewezen.
- Twaalf architectuurchecks: Core 1 blijft eigenaar, callbacks kopiëren alleen naar
  de begrensde queue, veilige NVS-opslag, netwerkworkerisolatie en OpenTherm-interval.
- 56 tijdzonegevallen voor Europe/Amsterdam; originele fonts en 65 RGB565-assets
  gecontroleerd op broninhoud, afmetingen en buffers. Firmware-schermen gerenderd
  vanuit de echte C++-code en visueel nagekeken, onder `artifacts/remote-ui/firmware-*`.
- Browserregressie van het klikmodel, inclusief terugkeer Meer dagen.

## Gedrag en grenzen

Status bevat actuele thermostaatinstellingen, temperaturen, regelreden, effectieve
zeven dagtypen met overridevlaggen, klok, ketel- en warmwaterstatus. De remote toont
geen voorbeeldwaarden. De eerste wektik wordt geblokkeerd tot minimaal 200 ms loslaten;
één actie per aanraking, geen herhaling bij vasthouden.

Opdrachten bevatten boot-ID, sessie, ID, revisie, hosttijd/lease en begrensde parameters.
De thermostaat voert ze op Core 1 uit, bevestigt met de resulterende snapshot en bewaart
het laatste antwoord per sessie. Een sessiewissel krijgt een nieuwe lease. Herhalingen
gebruiken exact dezelfde inhoud en ID; na 4,5 seconden wordt onzekerheid getoond en
status opnieuw opgevraagd. Geen automatische herhaling onder een nieuwe ID of na reboot.
Wijzigingen door Telegram, fysieke bediening of het programma veranderen de revisie.
De meerdaagse override vervalt nu direct na de inclusieve laatste dag, zonder een
extra dag waarop de interne override nog actief lijkt maar de regeling al regulier is.

Menuwijzigingen gaan pas bij OK/bevestigen naar de host. Kouder/warmer stuurt één
absolute stap van 0,5 °C vanaf de getoonde waarde met revisiecontrole. Bij timeouts wordt
niets als bevestigd opgeslagen getoond. Rapporten gaan uitsluitend via de bestaande
Telegram-worker, naar het geconfigureerde chatadres. De remote bevestigt de aanvraag,
niet aflevering. Herstart wacht op een veilige ketelpauze en succesvolle NVS-opslag.

NVS volgt voor normale wijzigingen de bestaande uitgestelde opslagcyclus. De radio
blijft uit in deep sleep; er zijn geen periodieke wakes toegevoegd. Het scherm gebruikt
een framebuffer als allocatie lukt, anders de directe renderer. De framebuffer wordt
alleen op de remote gebruikt; geen grote extra framebuffer op de thermostaat.

## Nog te valideren op hardware

Er is in deze wijzigingsronde niets naar de apparaten geüpload. Tests hierboven zijn
build-/hosttests; ze bevestigen geen werkelijke ESP-NOW-aflevering, fysieke touch,
OpenTherm-timing onder bediening, Telegram-aflevering, LCD-snelheid of energieverbruik.
Gebruik de hardwarestappen in UPLOAD-ALPHA2.md. De vorige succesvolle alpha.1-tests
zijn geen hardwarebewijs voor alpha.2. Firmwarebeelden bevatten privésleutels en blijven
in de lokale uitgesloten artifacts-map.
