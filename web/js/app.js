(function () {
  "use strict";

  var POLL_MS = 2000;
  var API = {
    sensors: "/api/sensors",
    status: "/api/status",
    wifi: "/api/wifi",
  };

  var CFG = {
    t: {
      name: "Temperatura",
      desc: "Temperatura ambiente del invernadero (DHT22)",
      unit: "°C",
      dec: 1,
      id: "GPIO 4 · DHT22",
      icon: "🌡️",
      ok: [18, 30],
      warn: [10, 35],
    },
    h: {
      name: "Humedad del aire",
      desc: "Humedad relativa del aire (DHT22)",
      unit: "%",
      dec: 1,
      id: "GPIO 4 · DHT22",
      icon: "💧",
      ok: [50, 80],
      warn: [35, 90],
    },
    g: {
      name: "Gas MQ135",
      desc: "Nivel estimado (requiere calibración; no es ppm absoluto)",
      unit: "ppm est.",
      dec: 0,
      id: "GPIO 34 · ADC",
      icon: "☁️",
      ok: [0, 800],
      warn: [0, 1500],
    },
  };

  var values = { t: null, h: null, g: null };
  var stats = { t: {}, h: {}, g: {} };
  var active = null;
  var apiOnline = false;
  var dataSimulated = false;

  function $(id) {
    return document.getElementById(id);
  }

  function showMenuOnly() {
    $("menuView").classList.remove("hidden");
    $("detailView").classList.add("hidden");
    $("projectView").classList.add("hidden");
    $("settingsView").classList.add("hidden");
  }

  function stateFor(k, v) {
    var c = CFG[k];
    if (v >= c.ok[0] && v <= c.ok[1]) return ["ok", "Óptimo"];
    if (v >= c.warn[0] && v <= c.warn[1]) return ["warn", "Atención"];
    return ["bad", "Fuera de rango"];
  }

  function updateVariable(k, v) {
    var c = CFG[k],
      s = stats[k],
      st = stateFor(k, v);
    values[k] = v;
    s.min = s.min === undefined ? v : Math.min(s.min, v);
    s.max = s.max === undefined ? v : Math.max(s.max, v);
    var menuId = k === "t" ? "menuT" : k === "h" ? "menuH" : "menuG";
    var stateId = k === "t" ? "stateT" : k === "h" ? "stateH" : "stateG";
    $(menuId).textContent = v.toFixed(c.dec);
    $(stateId).textContent = st[1];
    $(stateId).className = "state " + st[0];
    if (active === k) renderDetail(k);
  }

  function renderDetail(k) {
    var c = CFG[k],
      v = values[k],
      s = stats[k];
    if (v === null) return;
    var st = stateFor(k, v);
    $("detailName").textContent = c.name;
    $("detailDesc").textContent = c.desc;
    $("detailIcon").textContent = c.icon;
    $("detailValue").textContent = v.toFixed(c.dec);
    $("detailUnit").textContent = c.unit;
    $("detailUnitInfo").textContent = c.unit;
    $("detailState").textContent = st[1];
    $("detailState").className = "badge " + st[0];
    $("detailMin").textContent = s.min.toFixed(c.dec) + " " + c.unit;
    $("detailMax").textContent = s.max.toFixed(c.dec) + " " + c.unit;
    $("detailOptimal").textContent = c.ok[0] + " – " + c.ok[1] + " " + c.unit;
    $("detailWarn").textContent = c.warn[0] + " – " + c.warn[1] + " " + c.unit;
    $("detailId").textContent = c.id;
    $("detailTime").textContent = new Date().toLocaleTimeString("es", {
      hour12: false,
    });
    $("detailSysState").textContent = apiOnline
      ? dataSimulated
        ? "Sensores simulados"
        : "Sensores en vivo"
      : "Sin enlace al ESP32";
  }

  function openDetail(k) {
    active = k;
    $("menuView").classList.add("hidden");
    $("detailView").classList.remove("hidden");
    $("projectView").classList.add("hidden");
    $("settingsView").classList.add("hidden");
    renderDetail(k);
  }

  function backMenu() {
    active = null;
    showMenuOnly();
  }

  document.querySelectorAll(".variable-btn").forEach(function (btn) {
    btn.addEventListener("click", function () {
      openDetail(btn.getAttribute("data-key"));
    });
  });
  $("backBtn").addEventListener("click", backMenu);

  function resetStats() {
    stats = { t: {}, h: {}, g: {} };
    if (active) renderDetail(active);
  }
  $("resetStatsBtn").addEventListener("click", resetStats);

  function setConnectionUi(label, dotClass) {
    $("connLabel").textContent = label;
    $("connDot").className = "dot " + dotClass;
  }

  function updateFooterNote() {
    var el = $("footerNote");
    if (!apiOnline) {
      el.textContent =
        "Modo vista previa: abre con python tools/dev_server.py o conecta al ESP32 en la misma red.";
      return;
    }
    if (dataSimulated) {
      el.textContent =
        "Lecturas simuladas en el ESP32 (revisa cableado DHT22 / MQ135). Actualización cada 2 s.";
      return;
    }
    el.textContent = "Datos en vivo del ESP32. Actualización cada 2 segundos.";
  }

  function applySensorPayload(data) {
    if (typeof data.t === "number") updateVariable("t", data.t);
    if (typeof data.h === "number") updateVariable("h", data.h);
    if (typeof data.g === "number") updateVariable("g", data.g);
    dataSimulated = !!data.simulated;
    updateFooterNote();
    if (active) renderDetail(active);
  }

  function simulateLocal() {
    applySensorPayload({
      t: 20 + Math.random() * 14,
      h: 45 + Math.random() * 43,
      g: 300 + Math.random() * 1400,
      simulated: true,
    });
  }

  function fetchJson(url, options) {
    return fetch(url, options).then(function (res) {
      if (!res.ok) throw new Error("HTTP " + res.status);
      return res.json();
    });
  }

  function pollStatus() {
    return fetchJson(API.status)
      .then(function (st) {
        apiOnline = true;
        var mode = st.mode || "sta";
        if (st.simulated || dataSimulated) {
          setConnectionUi(
            mode === "ap" ? "AP · simulación" : "En línea · simulación",
            "demo"
          );
        } else {
          setConnectionUi(
            mode === "ap" ? "Modo configuración (AP)" : "En línea",
            "online"
          );
        }
        return st;
      })
      .catch(function () {
        apiOnline = false;
        dataSimulated = true;
        setConnectionUi("Vista previa", "offline");
        updateFooterNote();
      });
  }

  function pollSensors() {
    return fetchJson(API.sensors)
      .then(function (data) {
        apiOnline = true;
        applySensorPayload(data);
        return pollStatus();
      })
      .catch(function () {
        apiOnline = false;
        setConnectionUi("Vista previa", "offline");
        simulateLocal();
      });
  }

  function clock() {
    $("clock").textContent = new Date().toLocaleTimeString("es", {
      hour12: false,
    });
  }

  clock();
  pollSensors();
  setInterval(clock, 1000);
  setInterval(pollSensors, POLL_MS);

  function openProject() {
    $("menuView").classList.add("hidden");
    $("detailView").classList.add("hidden");
    $("settingsView").classList.add("hidden");
    $("projectView").classList.remove("hidden");
  }
  function closeProject() {
    showMenuOnly();
  }
  $("projectBtn").addEventListener("click", openProject);
  $("projectBackBtn").addEventListener("click", closeProject);

  function loadWifiConfig() {
    fetchJson(API.wifi)
      .then(function (cfg) {
        $("wifiSsid").value = cfg.ssid || "";
        $("wifiPass").value = "";
        $("currentSsid").textContent = cfg.ssid || "Sin configurar";
        if (cfg.configured && cfg.ssid) {
          $("wifiPass").placeholder = "Deja vacío para mantener la actual";
        }
      })
      .catch(function () {
        $("wifiSsid").value = "";
        $("wifiPass").value = "";
        $("currentSsid").textContent = "Sin configurar (solo en ESP32)";
      });
  }

  function openSettings() {
    $("menuView").classList.add("hidden");
    $("detailView").classList.add("hidden");
    $("projectView").classList.add("hidden");
    $("settingsView").classList.remove("hidden");
    loadWifiConfig();
  }
  function closeSettings() {
    showMenuOnly();
    $("wifiMsg").textContent = "";
  }
  $("settingsBtn").addEventListener("click", openSettings);
  $("settingsBackBtn").addEventListener("click", closeSettings);
  $("togglePass").addEventListener("click", function () {
    var input = $("wifiPass");
    var show = input.type === "password";
    input.type = show ? "text" : "password";
    $("togglePass").textContent = show ? "Ocultar" : "Mostrar";
  });

  $("saveWifiBtn").addEventListener("click", function () {
    var ssid = $("wifiSsid").value.trim();
    var pass = $("wifiPass").value;
    if (!ssid) {
      $("wifiMsg").style.color = "var(--red)";
      $("wifiMsg").textContent = "Debes ingresar el nombre de la red.";
      return;
    }
    fetchJson(API.wifi, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ ssid: ssid, pass: pass }),
    })
      .then(function (res) {
        $("currentSsid").textContent = res.ssid || ssid;
        $("wifiPass").value = "";
        $("wifiMsg").style.color = "var(--green)";
        $("wifiMsg").textContent =
          res.message || "Configuración guardada. El ESP32 se reconectará.";
      })
      .catch(function () {
        $("wifiMsg").style.color = "var(--red)";
        $("wifiMsg").textContent =
          "No se pudo guardar (¿estás conectado al ESP32 o al servidor de desarrollo?).";
      });
  });

  function getPageUrl() {
    if (location.protocol === "http:" || location.protocol === "https:") {
      return location.origin + location.pathname;
    }
    return location.href;
  }

  function openQR() {
    var url = getPageUrl();
    $("qrUrl").textContent = url;
    var box = $("qrBox");
    box.innerHTML = "";
    if (typeof QRCode !== "undefined") {
      box.innerHTML = "";
      new QRCode(box, {
        text: url,
        width: 220,
        height: 220,
        colorDark: "#000000",
        colorLight: "#ffffff",
        correctLevel: QRCode.CorrectLevel.M,
      });
    } else {
      box.innerHTML =
        '<div class="qr-error">Biblioteca QR no cargada.<br><br>' +
        url +
        "</div>";
    }
    $("qrModal").classList.remove("hidden");
  }
  function closeQR() {
    $("qrModal").classList.add("hidden");
  }
  $("qrBtn").addEventListener("click", openQR);
  $("qrCloseBtn").addEventListener("click", closeQR);
  $("qrModal").addEventListener("click", function (e) {
    if (e.target === $("qrModal")) closeQR();
  });
  document.addEventListener("keydown", function (e) {
    if (e.key === "Escape" && !$("qrModal").classList.contains("hidden"))
      closeQR();
  });
})();
