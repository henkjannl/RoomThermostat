"""Verify PNG selection, stable firmware names, and transparent white pixels."""
from pathlib import Path
import sys,tempfile,unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from display_asset_sources import selected_assets,opaque_image
from PIL import Image
class AssetSources(unittest.TestCase):
    def test_png_overrides_jpg_but_keeps_lookup_name(self):
        with tempfile.TemporaryDirectory() as tmp:
            folder=Path(tmp)/'shared/assets/icons';folder.mkdir(parents=True)
            Image.new('RGB',(32,32),'red').save(folder/'iconClock.jpg')
            Image.new('RGBA',(32,32),(0,0,255,255)).save(folder/'iconClock.png')
            files=dict(selected_assets(tmp))
            self.assertEqual(files['iconClock.jpg'],folder/'iconClock.png')
            self.assertEqual(opaque_image(files['iconClock.jpg']).getpixel((0,0)),(0,0,255))
    def test_transparent_and_partial_alpha_on_white(self):
        with tempfile.TemporaryDirectory() as tmp:
            p=Path(tmp)/'icon.png';im=Image.new('RGBA',(3,1),(0,0,0,0))
            im.putpixel((1,0),(0,0,0,128));im.putpixel((2,0),(12,34,56,255));im.save(p)
            result=opaque_image(p)
            self.assertEqual(result.getpixel((0,0)),(255,255,255))
            self.assertEqual(result.getpixel((1,0)),(127,127,127))
            self.assertEqual(result.getpixel((2,0)),(12,34,56))
if __name__=='__main__':unittest.main(verbosity=2)
