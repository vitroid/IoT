"""Scan for iBeacons.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
from uuid import UUID

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
from construct import Array, Byte, Const, Int8sl, Int16ub, Int16ul, Struct
from construct.core import ConstError

ibeacon_format = Struct(
    # "type_length" / Const(b"\x02\x15"),
    # "uuid" / Array(16, Byte),
    "seq" / Int8sl,
    "temperature" / Int16ul,
)


def device_found(
    device: BLEDevice, advertisement_data: AdvertisementData
):
    """Decode iBeacon."""
    try:
        payload = advertisement_data.manufacturer_data[0xffff] # fake company ID
        # print(advertisement_data)
        ibeacon = ibeacon_format.parse(payload)
        print(f"Seq      : {ibeacon.seq}")
        print(f"Temp / C : {ibeacon.temperature}")
        # # uuid = UUID(bytes=bytes(ibeacon.uuid))
        # # print(f"UUID     : {uuid}")
        # print(f"Major    : {ibeacon.major}")
        # print(f"Minor    : {ibeacon.minor}")
        # print(f"TX power : {ibeacon.power} dBm")
        # print(f"RSSI     : {device.rssi} dBm")
        # print(47 * "-")
    except KeyError:
        # Apple company ID (0x004c) not found
        pass
    except ConstError:
        # No iBeacon (type 0x02 and length 0x15)
        pass


async def main():
    """Scan for devices."""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    while True:
        await scanner.start()
        await asyncio.sleep(1.0)
        await scanner.stop()


asyncio.run(main())
