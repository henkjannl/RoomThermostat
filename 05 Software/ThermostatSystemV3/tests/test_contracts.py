"""Host contract tests: wire layout/authentication and battery-record recovery.

These are independent format fixtures, not an ESP-NOW or ESP32 simulation.
"""
import hashlib
import hmac
import struct
import unittest
import zlib

PACKET=struct.Struct('<IBBBB6s6sIIIIhhIIIHBBhBB38s7sBBB16s')
RECORD=struct.Struct('<IIIqIHBBI')
KEY=bytes(range(32)) # Test-only key; never device provisioning material.

def message():
    data=PACKET.pack(0x33544852,3,4,6,15,b'\x01'*6,b'\x02'*6,123,7,9,1792771200,1985,2050,42,43,900,1,0,0,0,0,0,bytes(38),bytes(7),0,0,6,b'\0'*16)
    return data[:-16]+hmac.digest(KEY,data[:-16],'sha256')[:16]

def accepted(data,session=123,request=7):
    if len(data)!=PACKET.size: return False
    fields=PACKET.unpack(data)
    return (fields[0]==0x33544852 and fields[1]==3 and fields[2]==4 and
            fields[7]==session and fields[8]==request and
            hmac.compare_digest(data[-16:],hmac.digest(KEY,data[:-16],'sha256')[:16]))

class Contracts(unittest.TestCase):
    def test_packet_layout(self):
        self.assertEqual(PACKET.size,124)
        self.assertEqual(PACKET.unpack(message())[-2],6)
        self.assertEqual(PACKET.unpack(message())[11:13],(1985,2050))
        self.assertTrue(accepted(message()))
    def test_all_single_byte_changes_rejected(self):
        original=message()
        for i in range(len(original)):
            bad=bytearray(original);bad[i]^=1
            self.assertFalse(accepted(bad),i)
    def test_wrong_session_request_and_lengths(self):
        self.assertFalse(accepted(message(),session=124))
        self.assertFalse(accepted(message(),request=8))
        self.assertFalse(accepted(message()[:-1]))
        self.assertFalse(accepted(message()+b'0'))
    def test_crc_rejects_torn_record(self):
        self.assertEqual(RECORD.size,32)
        body=RECORD.pack(0x33425431,2049,12,900,15000,3980,88,7,0)[:-4]
        record=body+struct.pack('<I',zlib.crc32(body))
        self.assertEqual(zlib.crc32(record[:-4]),struct.unpack('<I',record[-4:])[0])
        damaged=bytearray(record);damaged[19]^=1
        self.assertNotEqual(zlib.crc32(damaged[:-4]),struct.unpack('<I',damaged[-4:])[0])
    def test_bounded_ring_wrap(self):
        slots={}
        for sequence in range(1,4097): slots[(sequence-1)%2048]=sequence
        self.assertEqual(len(slots),2048)
        self.assertEqual(sorted(slots.values()),list(range(2049,4097)))

if __name__=='__main__': unittest.main(verbosity=2)
