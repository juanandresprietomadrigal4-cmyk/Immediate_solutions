#!/usr/bin/env python3
"""Genera web_bundle/index.html (PlatformIO embed) y index.html en la raíz."""
from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import quote

ROOT = Path(__file__).resolve().parents[1]
WEB = ROOT / "web"
OUT_BUNDLE = ROOT / "firmware" / "embed" / "index.html"
OUT_ROOT = ROOT / "index.html"
QRCODE = WEB / "js" / "qrcode.min.js"


def inline_assets(html: str, *, include_qr: bool) -> str:
    css = (WEB / "css" / "app.css").read_text(encoding="utf-8")
    html = re.sub(
        r'<link rel="stylesheet" href="css/app\.css">\s*',
        f"<style>\n{css}\n</style>\n",
        html,
        count=1,
    )
    scripts = ["app.js"]
    if include_qr:
        scripts.insert(0, "qrcode.min.js")
    for name in scripts:
        path = WEB / "js" / name
        if not path.is_file():
            raise FileNotFoundError(f"Falta {path}")
        js = path.read_text(encoding="utf-8")
        html = html.replace(
            f'<script src="js/{name}"></script>',
            f"<script>\n{js}\n</script>",
        )
    if not include_qr:
        html = html.replace('<script src="js/qrcode.min.js"></script>\n', "")

    icon = WEB / "img" / "qr-icon.svg"
    if icon.is_file():
        svg = icon.read_text(encoding="utf-8").strip()
        data_uri = "data:image/svg+xml," + quote(svg, safe="")
        html = html.replace('src="img/qr-icon.svg"', f'src="{data_uri}"')
    return html


def main() -> int:
    if not QRCODE.is_file():
        print("Aviso: qrcode.min.js no encontrado; el QR usará texto.", file=sys.stderr)

    template = (WEB / "index.html").read_text(encoding="utf-8")
    bundled = inline_assets(template, include_qr=QRCODE.is_file())

    OUT_BUNDLE.parent.mkdir(parents=True, exist_ok=True)
    OUT_BUNDLE.write_text(bundled, encoding="utf-8")
    OUT_ROOT.write_text(bundled, encoding="utf-8")

    print(f"OK {OUT_BUNDLE} ({OUT_BUNDLE.stat().st_size} bytes)")
    print(f"OK {OUT_ROOT}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
