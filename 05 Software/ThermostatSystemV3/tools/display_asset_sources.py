"""Select exported icon sources while keeping existing firmware asset names."""
from pathlib import Path
from PIL import Image

def selected_assets(root):
    folder=Path(root)/'shared/assets/icons'
    # Existing code looks up /iconName.jpg; a new PNG overrides its pixels without
    # requiring changes to all renderers or replacing the original JPEG artwork.
    entries={p.name:p for p in folder.glob('icon*.jpg')}
    for p in folder.glob('icon*.png'):entries[p.with_suffix('.jpg').name]=p
    for pattern in ('remote-*.png','battery-*.png'):
        for p in folder.glob(pattern):entries[p.name]=p
    entries['HomeScreen.jpg']=folder/'HomeScreen.jpg'
    return sorted(entries.items())

def opaque_image(path):
    rgba=Image.open(path).convert('RGBA')
    white=Image.new('RGBA',rgba.size,(255,255,255,255))
    return Image.alpha_composite(white,rgba).convert('RGB')
