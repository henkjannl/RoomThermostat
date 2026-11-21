# Kamerthermostaat in Grafana

Dashboard: [Kamerthermostaat op AsusHome](http://192.168.178.63:3000/d/room-thermostat-v3/kamerthermostaat).

Negen panelen: actuele kamertemperatuur, ingestelde temperatuur, warmtevraag,
leeftijd van het laatste meetpunt, kamertemperatuur/instelling als grafiek,
warmtevraag als grafiek, gevraagde keteltemperatuur, warmwateractiviteit/vrijgave,
en datakwaliteit/CV-vrijgave. Automatische verversing: 30 seconden. Standaard:
laatste uur, Europe/Amsterdam. Bovenaan kan de gebruiker een andere periode kiezen.

Het dashboard gebruikt de bestaande Flux-datasource `influxdb`, UID
`dg2wdkyp71lvkc`, bucket `Zonstraat35`, measurement `room_thermostat` en
device `room-thermostat`. De JSON bevat geen wachtwoorden of tokens.

Warmtevraag is de berekende PID-uitgang in procenten, geen gemeten vermogen in
kW. Gevraagde keteltemperatuur is een instelling, geen gemeten aanvoertemperatuur.
De warmwatertijdlijn toont activiteit en vrijgave apart; een ongeldig ketelantwoord
wordt oranje ‘Onbekend’, geen ‘Uit’. De leeftijdsindicator wordt oranje vanaf
60 seconden en rood vanaf 180 seconden. De statvelden kunnen oudere laatst
bekende waarden tonen: beoordeel deze samen met de leeftijd en datakwaliteit.

Herstellen/importeren: Grafana → New → Import dashboard → plak de inhoud van
`kamerthermostaat.json` → Load → Import. Op een andere Grafana-installatie moet
de datasource-UID worden aangepast. Genereer de JSON met
`python tools/generate_grafana_dashboard.py`; pas daarvoor zo nodig `DS` aan.

Een dashboard verandert de thermostaatregeling niet en schrijft geen meetdata.
