#!/usr/bin/env python3
"""Servidor local: archivos estáticos de web/ + API mock del ESP32."""
from __future__ import annotations

import json
import random
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WEB = ROOT / "web"
WIFI = {"ssid": "RedDemo", "configured": True}


class DevHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB), **kwargs)

    def log_message(self, fmt, *args):
        print(f"[dev] {self.address_string()} {fmt % args}")

    def _send_json(self, payload, code=200):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.send_response(204)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        if self.path == "/api/sensors":
            self._send_json(
                {
                    "t": round(20 + random.random() * 14, 1),
                    "h": round(45 + random.random() * 43, 1),
                    "g": round(300 + random.random() * 1400),
                    "simulated": True,
                }
            )
            return
        if self.path == "/api/status":
            self._send_json(
                {
                    "online": True,
                    "mode": "demo",
                    "simulated": True,
                    "ip": "127.0.0.1",
                }
            )
            return
        if self.path == "/api/wifi":
            self._send_json(
                {
                    "ssid": WIFI["ssid"],
                    "configured": WIFI["configured"],
                }
            )
            return
        super().do_GET()

    def do_POST(self):
        if self.path == "/api/wifi":
            length = int(self.headers.get("Content-Length", "0"))
            raw = self.rfile.read(length).decode("utf-8") if length else "{}"
            try:
                data = json.loads(raw or "{}")
            except json.JSONDecodeError:
                self._send_json({"error": "JSON inválido"}, 400)
                return
            ssid = (data.get("ssid") or "").strip()
            if not ssid:
                self._send_json({"error": "SSID requerido"}, 400)
                return
            WIFI["ssid"] = ssid
            WIFI["configured"] = True
            self._send_json(
                {
                    "ok": True,
                    "ssid": ssid,
                    "message": "Guardado en servidor de demo (no persiste al cerrar).",
                }
            )
            return
        self.send_error(404)


def main():
    port = 8080
    httpd = ThreadingHTTPServer(("127.0.0.1", port), DevHandler)
    print(f"Servidor de desarrollo → http://127.0.0.1:{port}/")
    print("Ctrl+C para detener.")
    httpd.serve_forever()


if __name__ == "__main__":
    main()
