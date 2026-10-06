# Sistema de monitoreo de invernadero

Interfaz web + firmware ESP32 para supervisar **temperatura**, **humedad** (DHT22) y **gas** (MQ135) en un invernadero.

## Arquitectura

```text
Sensores (DHT22, MQ135)
        ↓
    ESP32 (PlatformIO)
   · Lectura cada 2 s
   · NVS: credenciales Wi‑Fi
   · WebServer :80
        ↓
   REST JSON + HTML embebido (PROGMEM)
        ↓
   Navegador (web/ o index.html generado)
```

| Ruta | Método | Descripción |
|------|--------|-------------|
| `/` | GET | Interfaz web |
| `/api/sensors` | GET | `{ t, h, g, simulated }` |
| `/api/status` | GET | `{ online, mode, ip, ssid, simulated }` |
| `/api/wifi` | GET | `{ ssid, configured }` (sin contraseña) |
| `/api/wifi` | POST | `{ ssid, pass }` → guarda y reinicia |

**Modo configuración:** si no hay Wi‑Fi guardado, el ESP32 crea el AP `Invernadero-Setup` (contraseña `invernadero123`) y sirve la misma UI en `192.168.4.1`.

## Estructura del repositorio

```text
web/                 Fuente de la interfaz (HTML, CSS, JS)
tools/               build_web_bundle.py, dev_server.py, fetch_vendor.py
firmware/embed/      HTML embebido en el ESP32 (generado)
firmware/            Proyecto PlatformIO (ESP32)
index.html           Generado (no editar a mano)
```

## Desarrollo en PC (sin hardware)

1. Dependencias QR offline (una vez):

   ```bash
   python tools/fetch_vendor.py
   ```

2. Servidor con API de prueba:

   ```bash
   python tools/dev_server.py
   ```

3. Abre [http://127.0.0.1:8080/](http://127.0.0.1:8080/)

## Firmware ESP32

1. Instala [PlatformIO](https://platformio.org/).
2. Genera el bundle web (automático al compilar con PlatformIO, o manual):

   ```bash
   python tools/build_web_bundle.py
   ```

3. Compila y sube:

   ```bash
   cd firmware
   pio run -t upload
   pio device monitor
   ```

4. Cableado por defecto (`firmware/include/config_pins.h`):

   - DHT22 → **GPIO 4**
   - MQ135 (analógico) → **GPIO 34**

Si el DHT falla o el MQ135 no lee, el firmware devuelve lecturas **simuladas** (`simulated: true`) para que la demo siga funcionando.

## Flujo de trabajo al cambiar la UI

1. Edita archivos en `web/`.
2. Ejecuta `python tools/build_web_bundle.py` (PlatformIO lo hace solo vía `pio_prebuild.py`).
3. Vuelve a compilar y subir el firmware.

## Notas

- El valor de gas es **ppm estimado**; el MQ135 requiere calibración en campo.
- No subas contraseñas Wi‑Fi reales al repositorio.
