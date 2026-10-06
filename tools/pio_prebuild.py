# PlatformIO pre-build: genera web_bundle/index.html embebido en el firmware.
Import("env")
import subprocess
import sys
from pathlib import Path

root = Path(env["PROJECT_DIR"]).resolve().parent
script = root / "tools" / "build_web_bundle.py"
subprocess.check_call([sys.executable, str(script)], cwd=str(root))
