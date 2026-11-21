# Telegram-gebruikers beheren

Beschikbaar vanaf thermostaatfirmware **3.0.0-alpha.7**. Upload deze eenmalig uit
de `thermostat`-map van deze werkmap. De remote hoeft hiervoor niet bijgewerkt te
worden. Gebruik een normale firmware-upload, zonder de flash/NVS te wissen.

Het bestaande ingestelde privé-chat-ID blijft de vaste beheerder. Alleen die
persoon kan gebruikers beheren. Alle commando's werken in een privégesprek met
de thermostaatbot; groepen worden niet toegelaten.

## Iemand toevoegen

1. Laat de persoon `/start` sturen naar de thermostaatbot. Die geeft het eigen
   numerieke ID terug, zonder toegang tot de thermostaat te geven.
2. Stuur als beheerder bijvoorbeeld `/toestaan 123456789 Anna`.
3. Wacht op **Toegang opgeslagen voor ...**. Deze bevestiging volgt pas als de
   thermostaat de lijst in zijn permanente geheugen heeft opgeslagen.
4. Laat Anna opnieuw `/start` sturen. Het thermostaatmenu is nu beschikbaar.

Een externe ID-bot of het doorsturen van een bericht is niet nodig.

## Commando's

| Commando | Werking |
| --- | --- |
| `/start` | Thermostaatmenu openen; onbekende gebruikers krijgen hun eigen ID. |
| `/help` | Uitleg; de beheerder krijgt ook alle beheercommando's en voorbeelden. |
| `/toestaan 123456789 Anna` | Toegang geven en een herkenbare naam bewaren. |
| `/toestaan 123456789 Anna Maria` | De naam van een bestaande gebruiker wijzigen. |
| `/gebruikers` | Beheerder en alle toegevoegde gebruikers tonen. |
| `/verwijderen 123456789` | Toegang intrekken. |

Er is ruimte voor acht extra gebruikers naast de beheerder. Namen zijn optioneel
en mogen maximaal 48 UTF-8-bytes lang zijn. Gebruik positieve, numerieke privé-ID's,
geen `@gebruikersnaam`. De vaste beheerder kan niet via deze commando's worden
verwijderd of gewijzigd.

De lijst blijft na stroomuitval en een normale firmware-update bestaan. Een
volledige flash/NVS-wisactie verwijdert de lijst; de geconfigureerde beheerder kan
daarna opnieuw gebruikers toevoegen. Bestaande thermostaatinstellingen hebben een
eigen opslag en worden door deze commando's niet gewijzigd.

## Afhandeling en controles

Een wijziging wordt pas actief nadat opslaan is geslaagd. Bij een fout blijft de
oude lijst actief en krijgt de beheerder een foutmelding. Wacht op de bevestiging
voordat je een volgende wijziging stuurt. Toevoegen van een identieke gebruiker
schrijft niet opnieuw naar flash. Intrekken van toegang blokkeert ook eerdere
Telegram-menuknoppen en verwijdert de bijbehorende gespreksstatus. Een opdracht die
al vóór intrekking door de thermostaat was aangenomen wordt niet teruggedraaid.

Onbekende gebruikers kunnen alleen met `/start` of `/help` hun eigen ID opvragen.
Deze antwoorden worden globaal begrensd tot één per drie seconden; bij gelijktijdige
aanvragen kan iemand het commando na een paar seconden opnieuw sturen.

De netwerkkern verwerkt Telegram en de actuele gebruikerslijst. Een begrensde
wachtrij draagt een wijziging over naar de besturingskern, die alleen in het
bestaande rustige OpenTherm-venster naar een eigen NVS-namespace schrijft.

Hosttests: `tools/check_telegram_native.cmd` controleert de werkelijke parser,
autorisatie, berichtafhandeling en opslagadapter met gesimuleerde Telegram/NVS/
wachtrijen. Dit vervangt geen controle op het apparaat: test na upload toevoegen,
beide gebruikers, intrekken en behoud na herstart.
