#pragma once
#include <Arduino.h>

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Invernadero · Monitor</title>
<style>
:root{--bg:#0b0f14;--panel:#131922;--panel2:#19212c;--line:#29313b;--text:#edf2f7;--muted:#85909d;--green:#4ade80;--blue:#38bdf8;--amber:#f59e0b;--red:#f87171}
*{box-sizing:border-box;margin:0;padding:0}
body{min-height:100vh;background:var(--bg);color:var(--text);font-family:"Segoe UI",Arial,sans-serif}
button{font:inherit;cursor:pointer}
.hidden{display:none!important}
header{height:64px;border-bottom:1px solid var(--line);display:flex;align-items:center;justify-content:space-between;padding:0 20px;background:#0d1218}
.brand{display:flex;align-items:center;gap:10px}.brand-icon{width:38px;height:38px;border-radius:10px;background:rgba(74,222,128,.1);display:grid;place-items:center}
.brand h1{font-size:1rem}.brand p{font-size:.72rem;color:var(--muted)}
.status{display:flex;align-items:center;gap:8px;font-size:.8rem;color:var(--muted)}.dot{width:9px;height:9px;border-radius:50%;background:var(--green)}
main{max-width:1050px;margin:auto;padding:28px 18px}
.hero{margin-bottom:22px}.hero h2{font-size:1.5rem;margin-bottom:5px}.hero p{color:var(--muted);font-size:.88rem}
.menu-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:16px}
.variable-btn{border:1px solid var(--line);border-radius:16px;background:var(--panel);color:var(--text);padding:22px;text-align:left;transition:.18s ease;min-height:180px}
.variable-btn:hover{transform:translateY(-3px);border-color:#46515e;background:var(--panel2)}
.variable-btn .top{display:flex;justify-content:space-between;align-items:flex-start;gap:10px}.variable-btn .icon{font-size:2rem}
.variable-btn .name{font-size:1rem;font-weight:700}.variable-btn .reading{font-size:2.3rem;font-weight:700;margin-top:18px;font-variant-numeric:tabular-nums}.variable-btn small{font-size:.9rem;color:var(--muted);font-weight:400}
.variable-btn .state{display:inline-block;margin-top:12px;font-size:.7rem;padding:4px 9px;border:1px solid var(--line);border-radius:99px;color:var(--muted)}
.variable-btn.temp{border-top:3px solid var(--amber)}.variable-btn.hum{border-top:3px solid var(--blue)}.variable-btn.gas{border-top:3px solid var(--green)}
.detail{max-width:760px;margin:auto}.back{border:1px solid var(--line);background:transparent;color:var(--text);border-radius:8px;padding:9px 13px;margin-bottom:16px}
.detail-card{border:1px solid var(--line);border-radius:16px;background:var(--panel);padding:24px}.detail-head{display:flex;justify-content:space-between;align-items:center;gap:12px;border-bottom:1px solid var(--line);padding-bottom:16px;margin-bottom:20px}
.detail-head h2{font-size:1.35rem}.detail-head p{font-size:.78rem;color:var(--muted);margin-top:3px}.detail-icon{font-size:2.3rem}
.current{margin-bottom:22px}.current-label{font-size:.75rem;color:var(--muted);text-transform:uppercase;letter-spacing:.08em}.current-value{font-size:3.5rem;font-weight:800;line-height:1.1;margin:7px 0}.current-value small{font-size:1.1rem;color:var(--muted);font-weight:400}.badge{display:inline-block;padding:5px 10px;border-radius:99px;font-size:.75rem;border:1px solid var(--line)}
.badge.ok{color:var(--green);border-color:rgba(74,222,128,.45)}.badge.warn{color:var(--amber);border-color:rgba(245,158,11,.45)}.badge.bad{color:var(--red);border-color:rgba(248,113,113,.45)}
.info-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:12px}.info{background:var(--panel2);border:1px solid var(--line);border-radius:10px;padding:14px}.info span{display:block;font-size:.72rem;color:var(--muted);margin-bottom:5px}.info strong{font-size:1rem}
.footer-note{text-align:center;color:var(--muted);font-size:.73rem;margin-top:18px}
@media(max-width:760px){.menu-grid{grid-template-columns:1fr}.variable-btn{min-height:auto}.info-grid{grid-template-columns:1fr}.current-value{font-size:2.8rem}}

.settings-btn{border:1px solid var(--line);background:transparent;color:var(--text);border-radius:8px;padding:8px 11px;display:flex;align-items:center;gap:7px}
.settings-btn:hover{background:var(--panel2)}
.settings-panel{max-width:620px;margin:auto}
.settings-card{border:1px solid var(--line);border-radius:16px;background:var(--panel);padding:24px}
.settings-head{display:flex;justify-content:space-between;align-items:flex-start;gap:12px;border-bottom:1px solid var(--line);padding-bottom:16px;margin-bottom:20px}
.settings-head h2{font-size:1.35rem}.settings-head p{font-size:.8rem;color:var(--muted);margin-top:4px}
.form-group{margin-bottom:16px}.form-group label{display:block;font-size:.78rem;color:var(--muted);margin-bottom:7px}
.form-group input{width:100%;background:var(--bg);border:1px solid var(--line);border-radius:9px;color:var(--text);padding:12px;font:inherit}
.form-group input:focus{outline:none;border-color:var(--green)}
.pass-row{display:grid;grid-template-columns:1fr auto;gap:8px}
.pass-toggle{border:1px solid var(--line);background:var(--panel2);color:var(--text);border-radius:9px;padding:0 13px}
.save-btn{width:100%;border:0;border-radius:9px;background:var(--green);color:#07130a;font-weight:700;padding:12px;margin-top:4px}
.current-wifi{margin-top:18px;background:var(--panel2);border:1px solid var(--line);border-radius:10px;padding:14px}
.current-wifi span{display:block;color:var(--muted);font-size:.72rem;margin-bottom:5px}.current-wifi strong{font-size:.95rem}
.success{color:var(--green);font-size:.78rem;margin-top:10px;min-height:1.2em}


.project-btn{border:1px solid var(--line);background:transparent;color:var(--text);border-radius:8px;padding:8px 11px;display:flex;align-items:center;gap:7px}
.project-btn:hover{background:var(--panel2)}
.project-panel{max-width:760px;margin:auto}
.project-card{border:1px solid var(--line);border-radius:16px;background:var(--panel);padding:24px}
.project-head{display:flex;justify-content:space-between;align-items:flex-start;gap:12px;border-bottom:1px solid var(--line);padding-bottom:16px;margin-bottom:20px}
.project-head h2{font-size:1.35rem}.project-head p{font-size:.8rem;color:var(--muted);margin-top:4px}
.project-icon{font-size:2.2rem}
.project-intro{background:var(--panel2);border:1px solid var(--line);border-radius:10px;padding:15px;margin-bottom:16px;color:var(--text);font-size:.88rem;line-height:1.55}
.project-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:12px}
.project-info{background:var(--panel2);border:1px solid var(--line);border-radius:10px;padding:14px}
.project-info span{display:block;font-size:.72rem;color:var(--muted);margin-bottom:5px}
.project-info strong{font-size:.95rem}
.project-status{display:inline-flex;align-items:center;gap:7px;color:var(--green)}
.project-status i{width:8px;height:8px;border-radius:50%;background:var(--green)}
@media(max-width:760px){.project-grid{grid-template-columns:1fr}.project-btn span,.settings-btn span{display:none}}


.qr-back{position:fixed;inset:0;z-index:80;background:rgba(0,0,0,.72);display:flex;align-items:center;justify-content:center;padding:18px;backdrop-filter:blur(4px)}
.qr-modal{width:100%;max-width:390px;background:var(--panel);border:1px solid var(--line);border-radius:16px;padding:22px}
.qr-modal h3{font-size:1.2rem;margin-bottom:5px}.qr-modal p{color:var(--muted);font-size:.8rem;margin-bottom:14px}
.qr-box{width:244px;min-height:244px;margin:0 auto 12px;background:#fff;border-radius:12px;padding:12px;display:flex;align-items:center;justify-content:center}
.qr-box img{display:block;width:220px;height:220px}
.qr-url{font-size:.74rem;color:var(--blue);word-break:break-all;text-align:center;margin-bottom:12px}
.qr-error{color:#222;text-align:center;font-size:.8rem;line-height:1.45}
.qr-close{width:100%;border:0;border-radius:9px;background:var(--green);color:#07130a;font-weight:700;padding:12px}

</style>
</head>
<body>
<header>
<div class="brand"><div class="brand-icon">🌿</div><div><h1>Invernadero</h1><p id="clock">--:--:--</p></div></div>
<div style="display:flex;align-items:center;gap:8px">
<div class="status"><span>Simulación</span><i class="dot"></i></div>
<button id="qrBtn" class="project-btn" type="button">▣ <span>QR</span></button>
<button id="projectBtn" class="project-btn" type="button">ℹ <span>Proyecto</span></button>
<button id="settingsBtn" class="settings-btn" type="button">⚙ <span>Configuración</span></button>
</div>
</header>
<main>
<section id="menuView">
<div class="hero"><h2>Variables del invernadero</h2><p>Selecciona una variable para consultar toda su información.</p></div>
<div class="menu-grid">
<button class="variable-btn temp" data-key="t" type="button">
<div class="top"><div><div class="name">Temperatura</div><div class="reading"><span id="menuT">--.-</span> <small>°C</small></div></div><div class="icon">🌡️</div></div>
<span id="stateT" class="state">Sin datos</span>
</button>
<button class="variable-btn hum" data-key="h" type="button">
<div class="top"><div><div class="name">Humedad del aire</div><div class="reading"><span id="menuH">--.-</span> <small>%</small></div></div><div class="icon">💧</div></div>
<span id="stateH" class="state">Sin datos</span>
</button>
<button class="variable-btn gas" data-key="g" type="button">
<div class="top"><div><div class="name">Gas MQ135</div><div class="reading"><span id="menuG">---</span> <small>ppm</small></div></div><div class="icon">☁️</div></div>
<span id="stateG" class="state">Sin datos</span>
</button>
</div>
<div class="footer-note">Los valores simulados se actualizan cada 2 segundos.</div>
</section>

<section id="detailView" class="detail hidden">
<button id="backBtn" class="back" type="button">← Volver al menú</button>
<div class="detail-card">
<div class="detail-head"><div><h2 id="detailName">Variable</h2><p id="detailDesc">Información de la variable</p></div><div id="detailIcon" class="detail-icon">•</div></div>
<div class="current">
<div class="current-label">Valor actual</div>
<div class="current-value"><span id="detailValue">--</span> <small id="detailUnit"></small></div>
<span id="detailState" class="badge">Sin datos</span>
</div>
<div class="info-grid">
<div class="info"><span>Mínimo registrado</span><strong id="detailMin">--</strong></div>
<div class="info"><span>Máximo registrado</span><strong id="detailMax">--</strong></div>
<div class="info"><span>Rango óptimo</span><strong id="detailOptimal">--</strong></div>
<div class="info"><span>Rango de atención</span><strong id="detailWarn">--</strong></div>
<div class="info"><span>Unidad</span><strong id="detailUnitInfo">--</strong></div>
<div class="info"><span>ID del sensor</span><strong id="detailId">--</strong></div>
<div class="info"><span>Última actualización</span><strong id="detailTime">--:--:--</strong></div>
<div class="info"><span>Estado del sistema</span><strong>Activo</strong></div>
</div>
</div>
</section>


<section id="projectView" class="project-panel hidden">
<button id="projectBackBtn" class="back" type="button">← Volver al menú</button>
<div class="project-card">
<div class="project-head">
<div><h2>Información del proyecto</h2><p>Resumen general del sistema de monitoreo del invernadero.</p></div>
<div class="project-icon">🌿</div>
</div>
<div class="project-intro">
Este proyecto tiene como objetivo supervisar variables ambientales importantes dentro de un invernadero mediante sensores conectados a un ESP32. El sistema permite consultar la información desde una interfaz web sencilla y accesible desde dispositivos conectados a la misma red.
</div>
<div class="project-grid">
<div class="project-info"><span>Nombre del proyecto</span><strong>Sistema de Monitoreo de Invernadero</strong></div>
<div class="project-info"><span>Plataforma principal</span><strong>ESP32</strong></div>
<div class="project-info"><span>Variables monitoreadas</span><strong>Temperatura, Humedad y Gas</strong></div>
<div class="project-info"><span>Sensores</span><strong>DHT22 y MQ135</strong></div>
<div class="project-info"><span>Interfaz</span><strong>Servidor web local</strong></div>
<div class="project-info"><span>Actualización</span><strong>Cada 2 segundos</strong></div>
<div class="project-info"><span>Objetivo</span><strong>Monitoreo ambiental en tiempo real</strong></div>
<div class="project-info"><span>Estado del proyecto</span><strong class="project-status"><i></i>En desarrollo</strong></div>
</div>
</div>
</section>

<section id="settingsView" class="settings-panel hidden">
<button id="settingsBackBtn" class="back" type="button">← Volver al menú</button>
<div class="settings-card">
<div class="settings-head">
<div><h2>Configuración de Wi‑Fi</h2><p>Define la red a la que se conectará el sistema.</p></div>
<div style="font-size:2rem">⚙️</div>
</div>
<div class="form-group">
<label for="wifiSsid">Nombre de la red Wi‑Fi (SSID)</label>
<input id="wifiSsid" type="text" placeholder="Ejemplo: MiCasa_WiFi" autocomplete="off">
</div>
<div class="form-group">
<label for="wifiPass">Contraseña del Wi‑Fi</label>
<div class="pass-row">
<input id="wifiPass" type="password" placeholder="Ingresa la contraseña" autocomplete="new-password">
<button id="togglePass" class="pass-toggle" type="button">Mostrar</button>
</div>
</div>
<button id="saveWifiBtn" class="save-btn" type="button">Guardar configuración</button>
<div id="wifiMsg" class="success"></div>
<div class="current-wifi">
<span>Red configurada actualmente</span>
<strong id="currentSsid">Sin configurar</strong>
</div>
</div>
</section>

</main>

<div id="qrModal" class="qr-back hidden">
<div class="qr-modal">
<h3>Código QR de acceso</h3>
<p>Escanea este código desde otro dispositivo para abrir esta página.</p>
<div id="qrBox" class="qr-box"></div>
<div id="qrUrl" class="qr-url"></div>
<button id="qrCloseBtn" class="qr-close" type="button">Cerrar</button>
</div>
</div>

<script>
(function(){
"use strict";
var CFG={
t:{name:"Temperatura",desc:"Temperatura ambiente del invernadero",unit:"°C",dec:1,id:"5",icon:"🌡️",ok:[18,30],warn:[10,35]},
h:{name:"Humedad del aire",desc:"Humedad relativa del aire",unit:"%",dec:1,id:"4",icon:"💧",ok:[50,80],warn:[35,90]},
g:{name:"Gas MQ135",desc:"Nivel estimado detectado por el sensor MQ135",unit:"ppm",dec:0,id:"9",icon:"☁️",ok:[0,800],warn:[0,1500]}
};
var values={t:null,h:null,g:null};
var stats={t:{},h:{},g:{}};
var active=null;
function $(id){return document.getElementById(id)}
function stateFor(k,v){
var c=CFG[k];
if(v>=c.ok[0]&&v<=c.ok[1])return["ok","Óptimo"];
if(v>=c.warn[0]&&v<=c.warn[1])return["warn","Atención"];
return["bad","Fuera de rango"];
}
function updateVariable(k,v){
var c=CFG[k],s=stats[k],st=stateFor(k,v);
values[k]=v;
s.min=s.min===undefined?v:Math.min(s.min,v);
s.max=s.max===undefined?v:Math.max(s.max,v);
var menuId=k==="t"?"menuT":k==="h"?"menuH":"menuG";
var stateId=k==="t"?"stateT":k==="h"?"stateH":"stateG";
$(menuId).textContent=v.toFixed(c.dec);
$(stateId).textContent=st[1];
$(stateId).className="state "+st[0];
if(active===k)renderDetail(k);
}
function renderDetail(k){
var c=CFG[k],v=values[k],s=stats[k];
if(v===null)return;
var st=stateFor(k,v);
$("detailName").textContent=c.name;
$("detailDesc").textContent=c.desc;
$("detailIcon").textContent=c.icon;
$("detailValue").textContent=v.toFixed(c.dec);
$("detailUnit").textContent=c.unit;
$("detailUnitInfo").textContent=c.unit;
$("detailState").textContent=st[1];
$("detailState").className="badge "+st[0];
$("detailMin").textContent=s.min.toFixed(c.dec)+" "+c.unit;
$("detailMax").textContent=s.max.toFixed(c.dec)+" "+c.unit;
$("detailOptimal").textContent=c.ok[0]+" – "+c.ok[1]+" "+c.unit;
$("detailWarn").textContent=c.warn[0]+" – "+c.warn[1]+" "+c.unit;
$("detailId").textContent=c.id;
$("detailTime").textContent=new Date().toLocaleTimeString("es",{hour12:false});
}
function openDetail(k){
active=k;
$("menuView").classList.add("hidden");
$("detailView").classList.remove("hidden");
renderDetail(k);
}
function backMenu(){
active=null;
$("detailView").classList.add("hidden");
$("menuView").classList.remove("hidden");
}
document.querySelectorAll(".variable-btn").forEach(function(btn){
btn.addEventListener("click",function(){openDetail(btn.getAttribute("data-key"))});
});
$("backBtn").addEventListener("click",backMenu);
function simulate(){
updateVariable("t",20+Math.random()*14);
updateVariable("h",45+Math.random()*43);
updateVariable("g",300+Math.random()*1400);
}
function clock(){$("clock").textContent=new Date().toLocaleTimeString("es",{hour12:false})}
clock();simulate();setInterval(clock,1000);setInterval(simulate,2000);


function openProject(){
  $("menuView").classList.add("hidden");
  $("detailView").classList.add("hidden");
  $("settingsView").classList.add("hidden");
  $("projectView").classList.remove("hidden");
}
function closeProject(){
  $("projectView").classList.add("hidden");
  $("menuView").classList.remove("hidden");
}
$("projectBtn").addEventListener("click",openProject);
$("projectBackBtn").addEventListener("click",closeProject);

var WIFI_KEY="greenhouse_wifi_config";
function loadWifiConfig(){
  var cfg={ssid:"",pass:""};
  try{
    var saved=JSON.parse(localStorage.getItem(WIFI_KEY)||"{}");
    if(typeof saved.ssid==="string")cfg.ssid=saved.ssid;
    if(typeof saved.pass==="string")cfg.pass=saved.pass;
  }catch(e){}
  $("wifiSsid").value=cfg.ssid;
  $("wifiPass").value=cfg.pass;
  $("currentSsid").textContent=cfg.ssid||"Sin configurar";
}
function openSettings(){
  $("menuView").classList.add("hidden");
  $("detailView").classList.add("hidden");
  $("settingsView").classList.remove("hidden");
  loadWifiConfig();
}
function closeSettings(){
  $("settingsView").classList.add("hidden");
  $("menuView").classList.remove("hidden");
  $("wifiMsg").textContent="";
}
$("settingsBtn").addEventListener("click",openSettings);
$("settingsBackBtn").addEventListener("click",closeSettings);
$("togglePass").addEventListener("click",function(){
  var input=$("wifiPass");
  var show=input.type==="password";
  input.type=show?"text":"password";
  $("togglePass").textContent=show?"Ocultar":"Mostrar";
});
$("saveWifiBtn").addEventListener("click",function(){
  var ssid=$("wifiSsid").value.trim();
  var pass=$("wifiPass").value;
  if(!ssid){
    $("wifiMsg").style.color="var(--red)";
    $("wifiMsg").textContent="Debes ingresar el nombre de la red.";
    return;
  }
  try{
    localStorage.setItem(WIFI_KEY,JSON.stringify({ssid:ssid,pass:pass}));
    $("currentSsid").textContent=ssid;
    $("wifiMsg").style.color="var(--green)";
    $("wifiMsg").textContent="Configuración guardada correctamente.";
  }catch(e){
    $("wifiMsg").style.color="var(--red)";
    $("wifiMsg").textContent="No se pudo guardar la configuración.";
  }
});


function getPageUrl(){
  if(location.protocol==="http:"||location.protocol==="https:"){
    return location.origin+location.pathname;
  }
  return location.href;
}
function openQR(){
  var url=getPageUrl();
  $("qrUrl").textContent=url;
  $("qrBox").innerHTML="";
  var img=new Image();
  img.alt="Código QR de acceso";
  img.onload=function(){
    $("qrBox").innerHTML="";
    $("qrBox").appendChild(img);
  };
  img.onerror=function(){
    $("qrBox").innerHTML='<div class="qr-error">No se pudo generar el código QR.<br><br>Comprueba tu conexión a Internet.<br><br>'+url+'</div>';
  };
  img.src="https://api.qrserver.com/v1/create-qr-code/?size=220x220&margin=0&data="+encodeURIComponent(url);
  $("qrModal").classList.remove("hidden");
}
function closeQR(){
  $("qrModal").classList.add("hidden");
}
if($("qrBtn")) $("qrBtn").addEventListener("click",openQR);
if($("qrCloseBtn")) $("qrCloseBtn").addEventListener("click",closeQR);
$("qrModal").addEventListener("click",function(e){
  if(e.target===$("qrModal"))closeQR();
});
document.addEventListener("keydown",function(e){
  if(e.key==="Escape"&&!$("qrModal").classList.contains("hidden"))closeQR();
});

})();
</script>
</body>
</html>
)rawliteral";
