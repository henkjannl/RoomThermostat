"""Collect local app images and hashes; images contain private pairing keys."""
from pathlib import Path
import hashlib,json,shutil
root=Path(__file__).resolve().parents[1]
builds=[('thermostat','mhetesp32minikit','thermostat'),('remote-cyd','usb-test','cyd-usb-test'),('remote-cyd','battery','cyd-battery')]
manifest={'version':'3.0.0-alpha.2','warning':'Local paired images; do not publish','images':{}}
for project,env,label in builds:
    folder=root/'artifacts'/label
    folder.mkdir(parents=True,exist_ok=True)
    for name in ('firmware.bin','bootloader.bin','partitions.bin','firmware.elf'):
        src=root/project/'.pio/build'/env/name
        assert src.exists(),src
        dest=folder/name;shutil.copy2(src,dest)
        manifest['images'][f'{label}/{name}']={'bytes':dest.stat().st_size,'sha256':hashlib.sha256(dest.read_bytes()).hexdigest()}
(root/'artifacts/manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
print('Collected three local builds with SHA-256 manifest; no uploads performed')
