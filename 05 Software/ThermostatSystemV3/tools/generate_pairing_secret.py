"""Provision one local pair; never overwrite existing keys or commit them."""
from pathlib import Path
import secrets
path=Path(__file__).resolve().parents[1]/'shared/protocol/include/pairing_secret.h'
if path.exists():
    print('Existing pairing keys retained')
else:
    text='#pragma once\n// Local pairing keys. Keep private and back up with both firmwares.\n'
    for name,n in [('PAIR_AUTH_KEY',32),('PAIR_PMK',16),('PAIR_LMK',16)]:
        text+=f'static const uint8_t {name}[{n}] = {{'+','.join(str(b) for b in secrets.token_bytes(n))+'};\n'
    path.write_text(text)
    print('Pairing keys generated; values deliberately not printed')
