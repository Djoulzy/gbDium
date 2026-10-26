#!/usr/bin/env python3
"""Validate the ROM-only DMG cartridge produced by this project (no emulator)."""

import argparse
from pathlib import Path


NINTENDO_LOGO = bytes.fromhex(
    "CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D "
    "00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99 "
    "BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E"
)


def validate(rom: bytes) -> list[str]:
    errors = []
    if len(rom) != 32 * 1024:
        errors.append(f"taille incorrecte : {len(rom)} octets, attendu : 32768")
    if len(rom) < 0x150:
        return errors + ["en-tête de cartouche incomplet"]
    if rom[0x104:0x134] != NINTENDO_LOGO:
        errors.append("logo Nintendo invalide")
    if rom[0x134:0x13A] != b"GBDIUM":
        errors.append("titre de cartouche incorrect")
    for address, expected, label in (
        (0x143, 0, "compatibilité DMG"),
        (0x147, 0, "type de cartouche ROM ONLY"),
        (0x148, 0, "taille ROM déclarée (32 Kio)"),
        (0x149, 0, "taille RAM déclarée (sans RAM cartouche)"),
    ):
        if rom[address] != expected:
            errors.append(f"{label} : 0x{rom[address]:02X}, attendu : 0x{expected:02X}")
    header_checksum = (-sum(rom[0x134:0x14D]) - 25) & 0xFF
    if rom[0x14D] != header_checksum:
        errors.append("checksum de l'en-tête invalide")
    global_checksum = (sum(rom) - sum(rom[0x14E:0x150])) & 0xFFFF
    if int.from_bytes(rom[0x14E:0x150], "big") != global_checksum:
        errors.append("checksum global invalide")
    return errors


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("rom", type=Path)
    args = parser.parse_args()
    try:
        errors = validate(args.rom.read_bytes())
    except OSError as error:
        parser.exit(1, f"Lecture impossible : {error}\n")
    if errors:
        parser.exit(1, "ROM invalide :\n- " + "\n- ".join(errors) + "\n")
    print(f"ROM valide : {args.rom} — 32 Kio, DMG, ROM ONLY, checksums corrects")


if __name__ == "__main__":
    main()
