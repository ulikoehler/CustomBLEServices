#!/usr/bin/env python3
import argparse
import uuid

def main():
    parser = argparse.ArgumentParser(description="Generate C++ BLE UUID macro line.")
    args = parser.parse_args()

    u = uuid.uuid4()
    bytes_le = u.bytes_le  # little-endian byte order

    # Format bytes as hex for C++ macro
    hex_bytes = ', '.join(f'0x{b:02X}' for b in bytes_le)
    # Split into two lines: 8 bytes per line
    hex_list = hex_bytes.split(', ')
    line1 = ', '.join(hex_list[:8])
    line2 = ', '.join(hex_list[8:])
    print(f"BLE_UUID128_INIT({line1},")
    print(f"                {line2});")

if __name__ == "__main__":
    main()