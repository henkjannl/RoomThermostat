# Werken aan dit project

- Twee PlatformIO-projecten: thermostat en remote-cyd. Behoud de gedeelde protocolcontracten en bouw beide bij wijzigingen aan shared/.
- De V2.1.4-thermostaat en batterijmonitor v0.1.0 buiten deze map zijn referenties; wijzig die niet vanuit dit project.
- Geen credentials, pairing_secret.h of gebouwde firmware in Git. Firmware bevat de lokale sleutels. Maak geen externe repository/publicatie zonder opdracht.
- OpenTherm blijft op vijf seconden en heeft één eigenaar op Core 1. ESP-NOW-callbacks mogen alleen begrensde queue-operaties doen. Geen UI, regeling of flashwrites vanuit callbacks.
- Alpha.1 is alleen-lezen. Voeg geen instelopdrachten toe zonder versie/validatie/bevestiging/deduplicatie in het protocol.
- Gedeelde assets komen uit shared/assets; gegenereerde headers hebben generators in tools/. Bewaar de originele artwork.
- Bouw thermostat, remote-cyd usb-test en remote-cyd battery; voer tests/test_contracts.py, tests/verify_architecture.py en tests/verify_timezone_rules.py uit. Controleer gewijzigde assets met de bijbehorende tools.
- Rapportage moet build-/hosttests onderscheiden van hardwarevalidatie. Houd versie, beperkingen en testresultaten bij in docs/VALIDATION.md.
