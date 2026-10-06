#!/usr/bin/env python3
"""Descarga dependencias web embebidas (QR offline)."""
from __future__ import annotations

import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "web" / "js" / "qrcode.min.js"
URL = "https://cdn.jsdelivr.net/npm/qrcodejs@1.0.0/qrcode.min.js"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    print(f"Descargando {URL} …")
    urllib.request.urlretrieve(URL, OUT)
    size = OUT.stat().st_size
    print(f"OK → {OUT} ({size} bytes)")


if __name__ == "__main__":
    main()
