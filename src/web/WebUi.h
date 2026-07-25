#pragma once
#include <pgmspace.h>

static const char WEB_UI_HTML[] PROGMEM = R"rawhtml(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>CYD Air Monitor</title>
<style>
:root {
  --accent: #c17f0a;
  --accent-light: #e8a832;
  --accent-dark: #8f5c06;
  --bg: #faf6f0;
  --surface: #fff;
  --text: #2c2416;
  --muted: #7a6f5e;
  --border: #e4d9c8;
  --ok: #2d8a4e;
  --ok-bg: #e8f5ec;
  --warn: #c17f0a;
  --warn-bg: #fef3e0;
  --bad: #c0392b;
  --bad-bg: #fdecea;
  --radius: 10px;
  --shadow: 0 2px 8px rgba(44,36,22,.08);
}
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--text);line-height:1.5;min-height:100vh}
.wrap{max-width:800px;margin:0 auto;padding:16px}
header{background:var(--surface);border-radius:var(--radius);box-shadow:var(--shadow);padding:16px 20px;margin-bottom:16px;display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:12px}
header h1{font-size:1.25rem;font-weight:700;color:var(--accent-dark)}
header h1 span{font-weight:400;color:var(--muted);font-size:.85rem;display:block}
.badge{display:inline-flex;align-items:center;gap:6px;padding:5px 12px;border-radius:20px;font-size:.8rem;font-weight:600;text-transform:capitalize}
.badge::before{content:'';width:8px;height:8px;border-radius:50%;background:currentColor}
.badge.ok{color:var(--ok);background:var(--ok-bg)}
.badge.warn{color:var(--warn);background:var(--warn-bg)}
.badge.bad{color:var(--bad);background:var(--bad-bg)}
nav{display:flex;gap:4px;background:var(--surface);border-radius:var(--radius);box-shadow:var(--shadow);padding:4px;margin-bottom:16px;overflow-x:auto}
nav button{flex:1;min-width:0;padding:10px 8px;border:none;background:transparent;color:var(--muted);font-size:.85rem;font-weight:600;border-radius:8px;cursor:pointer;white-space:nowrap;transition:background .15s,color .15s}
nav button:hover{background:var(--bg);color:var(--text)}
nav button.active{background:var(--accent);color:#fff}
.tab{display:none}
.tab.active{display:block}
.card{background:var(--surface);border-radius:var(--radius);box-shadow:var(--shadow);padding:20px;margin-bottom:16px}
.card h2{font-size:1rem;margin-bottom:12px;color:var(--accent-dark)}
.card h3{font-size:.9rem;margin:16px 0 8px;color:var(--text)}
.grid{display:grid;gap:12px}
.grid-2{grid-template-columns:1fr 1fr}
.grid-info{grid-template-columns:repeat(auto-fill,minmax(140px,1fr))}
@media(max-width:520px){.grid-2{grid-template-columns:1fr}}
.stat{background:var(--bg);border-radius:8px;padding:14px;text-align:center}
.stat .val{font-size:1.75rem;font-weight:700;color:var(--accent-dark);line-height:1.2}
.stat .lbl{font-size:.75rem;color:var(--muted);text-transform:uppercase;letter-spacing:.04em;margin-top:4px}
.stat .hint{font-size:.8rem;margin-top:6px;font-weight:500}
.hint-cool{color:#2980b9}.hint-comfort{color:var(--ok)}.hint-warm{color:var(--warn)}.hint-hot{color:var(--bad)}.hint-dry{color:#8e44ad}.hint-humid{color:#16a085}
.info-item{background:var(--bg);border-radius:8px;padding:10px 12px}
.info-item .k{font-size:.7rem;color:var(--muted);text-transform:uppercase;letter-spacing:.04em}
.info-item .v{font-size:.9rem;font-weight:600;margin-top:2px;word-break:break-all}
.info-item a.v{color:var(--accent);text-decoration:none}
.info-item a.v:hover{text-decoration:underline}
.btn{display:inline-flex;align-items:center;justify-content:center;gap:6px;padding:10px 18px;border:none;border-radius:8px;font-size:.9rem;font-weight:600;cursor:pointer;transition:background .15s,opacity .15s}
.btn:disabled{opacity:.5;cursor:not-allowed}
.btn-primary{background:var(--accent);color:#fff}
.btn-primary:hover:not(:disabled){background:var(--accent-dark)}
.btn-secondary{background:var(--bg);color:var(--text);border:1px solid var(--border)}
.btn-secondary:hover:not(:disabled){background:var(--border)}
.btn-danger{background:var(--bad-bg);color:var(--bad);border:1px solid #f5c6cb}
.btn-danger:hover:not(:disabled){background:#f5c6cb}
.btn-row{display:flex;flex-wrap:wrap;gap:8px;margin-top:12px}
.banner{padding:12px 16px;border-radius:8px;font-size:.9rem;margin-bottom:12px;display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:8px}
.banner.saved{background:var(--ok-bg);color:var(--ok);border:1px solid #b8dfc5}
.banner.saved strong{font-weight:700}
.banner.none{background:var(--bg);color:var(--muted);border:1px dashed var(--border)}
.hint-box{background:var(--warn-bg);border-left:3px solid var(--accent);padding:10px 14px;border-radius:0 8px 8px 0;font-size:.85rem;color:var(--text);margin-bottom:12px}
.hint-box p{margin:0}
.spinner{display:inline-block;width:16px;height:16px;border:2px solid var(--border);border-top-color:var(--accent);border-radius:50%;animation:spin .7s linear infinite;vertical-align:middle}
@keyframes spin{to{transform:rotate(360deg)}}
.net-list{list-style:none;display:flex;flex-direction:column;gap:6px;max-height:320px;overflow-y:auto}
.net-item{display:flex;align-items:center;gap:10px;padding:10px 12px;background:var(--bg);border-radius:8px;cursor:pointer;border:2px solid transparent;transition:border-color .15s,background .15s}
.net-item:hover{border-color:var(--accent-light);background:#fff}
.net-item.selected{border-color:var(--accent);background:#fff}
.net-ssid{flex:1;font-weight:600;font-size:.9rem;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.rssi-bar{display:flex;gap:2px;align-items:flex-end;height:16px}
.rssi-bar span{width:4px;background:var(--border);border-radius:1px}
.rssi-bar span.on{background:var(--accent)}
.rssi-val{font-size:.75rem;color:var(--muted);min-width:36px;text-align:right}
input[type=text],input[type=password],input[type=number]{width:100%;padding:10px 12px;border:1px solid var(--border);border-radius:8px;font-size:.9rem;background:#fff;color:var(--text)}
input:focus{outline:none;border-color:var(--accent);box-shadow:0 0 0 3px rgba(193,127,10,.15)}
label{display:block;font-size:.8rem;font-weight:600;color:var(--muted);margin-bottom:4px;margin-top:10px}
label:first-child{margin-top:0}
.form-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
@media(max-width:520px){.form-grid{grid-template-columns:1fr}}
.preview-wrap{display:flex;justify-content:center;margin:16px 0}
.preview-box{width:160px;height:100px;background:linear-gradient(135deg,var(--accent-light),var(--accent-dark));border-radius:8px;display:flex;align-items:center;justify-content:center;color:#fff;font-weight:700;font-size:1.1rem;box-shadow:var(--shadow);transition:transform .3s ease}
.rot-options{display:grid;grid-template-columns:1fr 1fr;gap:8px}
@media(max-width:520px){.rot-options{grid-template-columns:1fr}}
.rot-opt{padding:12px;background:var(--bg);border:2px solid transparent;border-radius:8px;cursor:pointer;text-align:center;transition:border-color .15s,background .15s}
.rot-opt:hover{border-color:var(--accent-light)}
.rot-opt.selected{border-color:var(--accent);background:#fff}
.rot-opt .deg{font-size:1.1rem;font-weight:700;color:var(--accent-dark)}
.rot-opt .desc{font-size:.75rem;color:var(--muted);margin-top:2px}
.touch-pad-wrap{display:flex;justify-content:center;margin:12px 0}
.touch-pad{position:relative;background:#1a1a2e;border-radius:8px;border:3px solid var(--border);overflow:hidden;touch-action:none}
.touch-pad .dot{position:absolute;width:14px;height:14px;background:var(--accent-light);border:2px solid #fff;border-radius:50%;transform:translate(-50%,-50%);display:none;box-shadow:0 0 8px rgba(232,168,50,.6);pointer-events:none}
.touch-pad .dot.visible{display:block}
.touch-readouts{display:grid;grid-template-columns:repeat(auto-fill,minmax(100px,1fr));gap:8px;margin:12px 0}
.touch-readout{background:var(--bg);border-radius:6px;padding:8px;text-align:center}
.touch-readout .k{font-size:.65rem;color:var(--muted);text-transform:uppercase}
.touch-readout .v{font-size:.95rem;font-weight:700;font-family:monospace;margin-top:2px}
.toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%) translateY(80px);background:var(--text);color:#fff;padding:10px 20px;border-radius:8px;font-size:.85rem;opacity:0;transition:transform .3s,opacity .3s;z-index:999;max-width:90vw;text-align:center}
.toast.show{transform:translateX(-50%) translateY(0);opacity:1}
.empty{text-align:center;padding:24px;color:var(--muted);font-size:.9rem}
.footer{text-align:center;font-size:.75rem;color:var(--muted);padding:8px 0 16px}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <h1>CYD Air Monitor <span id="hdrBoard">—</span></h1>
    <span id="wifiBadge" class="badge bad">offline</span>
  </header>
  <div id="setupBanner" class="hint-box" style="display:none">
    <p><strong>Setup mode.</strong> Join WiFi <strong id="setupApSsid">CYD-Setup</strong>, then open <strong id="setupUrl">http://192.168.4.1</strong> in your browser to configure WiFi and settings.</p>
  </div>
  <nav id="tabs">
    <button class="active" data-tab="dash">Dashboard</button>
    <button data-tab="wifi">WiFi</button>
    <button data-tab="display">Display</button>
    <button data-tab="touch">Touch</button>
  </nav>

  <!-- Dashboard -->
  <div id="tab-dash" class="tab active">
    <div class="card">
      <h2>Environment</h2>
      <div class="grid grid-2">
        <div class="stat">
          <div class="val" id="dTemp">—</div>
          <div class="lbl">Temperature °C</div>
          <div class="hint" id="dTempHint"></div>
        </div>
        <div class="stat">
          <div class="val" id="dHum">—</div>
          <div class="lbl">Humidity %</div>
          <div class="hint" id="dHumHint"></div>
        </div>
      </div>
    </div>
    <div class="card">
      <h2>Device Info</h2>
      <div class="grid grid-info" id="infoGrid">
        <div class="info-item"><div class="k">Board</div><div class="v" id="iBoard">—</div></div>
        <div class="info-item"><div class="k">Environment</div><div class="v" id="iEnv">—</div></div>
        <div class="info-item"><div class="k">Resolution</div><div class="v" id="iRes">—</div></div>
        <div class="info-item"><div class="k">IP Address</div><div class="v" id="iIp">—</div></div>
        <div class="info-item"><div class="k">Uptime</div><div class="v" id="iUp">—</div></div>
        <div class="info-item"><div class="k">Free RAM</div><div class="v" id="iRam">—</div></div>
        <div class="info-item"><div class="k">WiFi RSSI</div><div class="v" id="iRssi">—</div></div>
        <div class="info-item"><div class="k">Saved SSID</div><div class="v" id="iSsid">—</div></div>
      </div>
    </div>
  </div>

  <!-- WiFi -->
  <div id="tab-wifi" class="tab">
    <div class="card">
      <h2>WiFi Settings</h2>
      <div id="savedBanner" class="banner none">No saved network</div>
      <div class="hint-box">
        <p id="wifiSetupHint">ESP32 supports <strong>2.4 GHz</strong> WiFi only. 5 GHz networks will not appear in scans. Place the device within range of your router for best results.</p>
      </div>
      <div class="btn-row">
        <button class="btn btn-primary" id="btnScan"><span id="scanLabel">Scan Networks</span></button>
        <button class="btn btn-danger" id="btnForget">Forget Saved</button>
      </div>
      <h3>Available Networks</h3>
      <ul class="net-list" id="netList"><li class="empty">Tap Scan to discover networks</li></ul>
      <h3>Connect</h3>
      <label for="wifiSsid">Network (SSID)</label>
      <input type="text" id="wifiSsid" placeholder="Select or type SSID" autocomplete="off">
      <label for="wifiPass">Password</label>
      <input type="password" id="wifiPass" placeholder="WiFi password" autocomplete="off">
      <div class="btn-row">
        <button class="btn btn-primary" id="btnConnect">Connect</button>
      </div>
    </div>
  </div>

  <!-- Display -->
  <div id="tab-display" class="tab">
    <div class="card">
      <h2>Display Rotation</h2>
      <p style="font-size:.85rem;color:var(--muted);margin-bottom:8px">Choose how the panel is mounted. Touch coordinates follow rotation automatically.</p>
      <div class="preview-wrap">
        <div class="preview-box" id="rotPreview">CYD</div>
      </div>
      <div class="rot-options" id="rotOptions">
        <div class="rot-opt" data-rot="0"><div class="deg">0°</div><div class="desc">Normal — USB bottom</div></div>
        <div class="rot-opt" data-rot="1"><div class="deg">90°</div><div class="desc">Clockwise — USB left</div></div>
        <div class="rot-opt" data-rot="2"><div class="deg">180°</div><div class="desc">Upside down — USB top</div></div>
        <div class="rot-opt" data-rot="3"><div class="deg">270°</div><div class="desc">Counter-clockwise — USB right</div></div>
      </div>
      <div class="btn-row">
        <button class="btn btn-primary" id="btnApplyRot" disabled>Apply Rotation</button>
      </div>
    </div>
  </div>

  <!-- Touch -->
  <div id="tab-touch" class="tab">
    <div class="card">
      <h2>Touch Test</h2>
      <p style="font-size:.85rem;color:var(--muted);margin-bottom:8px">Press the panel to see live coordinates. <span id="touchTypeLabel">—</span></p>
      <div class="touch-pad-wrap">
        <div class="touch-pad" id="touchPad"><div class="dot" id="touchDot"></div></div>
      </div>
      <div class="touch-readouts">
        <div class="touch-readout"><div class="k">Raw X</div><div class="v" id="tRawX">—</div></div>
        <div class="touch-readout"><div class="k">Raw Y</div><div class="v" id="tRawY">—</div></div>
        <div class="touch-readout"><div class="k">Mapped X</div><div class="v" id="tMapX">—</div></div>
        <div class="touch-readout"><div class="k">Mapped Y</div><div class="v" id="tMapY">—</div></div>
        <div class="touch-readout"><div class="k">Pressed</div><div class="v" id="tPressed">no</div></div>
      </div>
    </div>
    <div class="card" id="calCard">
      <h2>Calibration</h2>
      <p style="font-size:.85rem;color:var(--muted);margin-bottom:8px">Adjust raw ADC min/max for resistive XPT2046 touch. Tap corners on the panel, note raw values, then set limits.</p>
      <div class="form-grid">
        <div><label for="calXMin">X Min</label><input type="number" id="calXMin"></div>
        <div><label for="calXMax">X Max</label><input type="number" id="calXMax"></div>
        <div><label for="calYMin">Y Min</label><input type="number" id="calYMin"></div>
        <div><label for="calYMax">Y Max</label><input type="number" id="calYMax"></div>
      </div>
      <div class="btn-row">
        <button class="btn btn-primary" id="btnCalSave">Save Calibration</button>
        <button class="btn btn-secondary" id="btnCalReset">Reset Defaults</button>
        <button class="btn btn-secondary" id="btnCalReload">Reload</button>
      </div>
    </div>
  </div>

  <div class="footer">Auto-refreshes every 2 s</div>
</div>
<div class="toast" id="toast"></div>

<script>
(function(){
  var statusTimer = null, touchTimer = null, scanTimer = null;
  var state = { status: null, settings: null, selectedRot: null, appliedRot: null, selectedSsid: null, scanning: false };
  var screenW = 320, screenH = 240;

  function $(id){ return document.getElementById(id); }
  function toast(msg){
    var el = $('toast');
    el.textContent = msg;
    el.classList.add('show');
    clearTimeout(el._t);
    el._t = setTimeout(function(){ el.classList.remove('show'); }, 3000);
  }
  function api(method, url, body){
    var opts = { method: method, headers: {} };
    if(body){ opts.headers['Content-Type'] = 'application/json'; opts.body = JSON.stringify(body); }
    return fetch(url, opts).then(function(r){ return r.json(); });
  }
  function fmtUptime(ms){
    var s = Math.floor(ms/1000), m = Math.floor(s/60), h = Math.floor(m/60), d = Math.floor(h/24);
    if(d>0) return d+'d '+(h%24)+'h';
    if(h>0) return h+'h '+(m%60)+'m';
    if(m>0) return m+'m '+(s%60)+'s';
    return s+'s';
  }
  function fmtBytes(b){
    if(b>=1048576) return (b/1048576).toFixed(1)+' MB';
    if(b>=1024) return (b/1024).toFixed(0)+' KB';
    return b+' B';
  }
  function tempHint(t){
    if(t==null||isNaN(t)) return {text:'',cls:''};
    if(t<18) return {text:'Cold',cls:'hint-cool'};
    if(t<21) return {text:'Cool',cls:'hint-cool'};
    if(t<=26) return {text:'Comfortable',cls:'hint-comfort'};
    if(t<=30) return {text:'Warm',cls:'hint-warm'};
    return {text:'Hot',cls:'hint-hot'};
  }
  function humHint(h){
    if(h==null||isNaN(h)) return {text:'',cls:''};
    if(h<30) return {text:'Dry',cls:'hint-dry'};
    if(h<=60) return {text:'Comfortable',cls:'hint-comfort'};
    if(h<=70) return {text:'Humid',cls:'hint-humid'};
    return {text:'Very humid',cls:'hint-humid'};
  }
  function wifiBadgeClass(st){
    if(st==='connected') return 'ok';
    if(st==='connecting') return 'warn';
    return 'bad';
  }
  function rssiBars(rssi){
    var n = 1;
    if(rssi>=-50) n=4; else if(rssi>=-60) n=3; else if(rssi>=-70) n=2;
    var html = '<div class="rssi-bar">';
    for(var i=1;i<=4;i++) html += '<span class="'+(i<=n?'on':'')+'"></span>';
    return html + '</div>';
  }

  /* Tabs */
  document.querySelectorAll('#tabs button').forEach(function(btn){
    btn.addEventListener('click', function(){
      document.querySelectorAll('#tabs button').forEach(function(b){ b.classList.remove('active'); });
      document.querySelectorAll('.tab').forEach(function(t){ t.classList.remove('active'); });
      btn.classList.add('active');
      $('tab-'+btn.dataset.tab).classList.add('active');
      if(btn.dataset.tab==='touch') startTouchPoll(); else stopTouchPoll();
    });
  });

  /* Status refresh */
  function refreshStatus(){
    api('GET','/api/status').then(function(d){
      state.status = d;
      screenW = d.screen_w || 320;
      screenH = d.screen_h || 240;
      resizeTouchPad();

      var ws = d.wifi ? d.wifi.status : 'disconnected';
      var badge = $('wifiBadge');
      badge.textContent = d.setup_portal ? 'setup' : ws;
      badge.className = 'badge '+(d.setup_portal ? 'warn' : wifiBadgeClass(ws));

      if(d.setup_portal){
        $('setupBanner').style.display='block';
        if(d.ap_ssid) $('setupApSsid').textContent = d.ap_ssid;
        if(d.setup_url) $('setupUrl').textContent = d.setup_url.replace('http://','');
        $('wifiSetupHint').innerHTML = 'You are connected to the board setup hotspot. Pick your home WiFi below — the board will join it and leave setup mode.';
      } else {
        $('setupBanner').style.display='none';
        $('wifiSetupHint').innerHTML = 'ESP32 supports <strong>2.4 GHz</strong> WiFi only. Or configure on the panel: <strong>Settings → WiFi</strong>.';
      }

      $('hdrBoard').textContent = d.board || '—';
      $('dTemp').textContent = d.temperature!=null ? d.temperature.toFixed(1) : '—';
      $('dHum').textContent = d.humidity!=null ? d.humidity.toFixed(1) : '—';
      var th = tempHint(d.temperature), hh = humHint(d.humidity);
      $('dTempHint').textContent = th.text; $('dTempHint').className = 'hint '+th.cls;
      $('dHumHint').textContent = hh.text; $('dHumHint').className = 'hint '+hh.cls;

      $('iBoard').textContent = d.board || '—';
      $('iEnv').textContent = d.env || '—';
      $('iRes').textContent = d.resolution || '—';
      var ipEl = $('iIp');
      if(d.wifi && d.wifi.ip){
        ipEl.innerHTML = '<a class="v" href="http://'+d.wifi.ip+'" target="_blank">'+d.wifi.ip+'</a>';
      } else { ipEl.textContent = '—'; }
      $('iUp').textContent = fmtUptime(d.uptime_ms||0);
      $('iRam').textContent = fmtBytes(d.free_heap||0);
      $('iRssi').textContent = (d.wifi && d.wifi.rssi!=null) ? d.wifi.rssi+' dBm' : '—';
      $('iSsid').textContent = (d.wifi && d.wifi.saved_ssid) ? d.wifi.saved_ssid : '—';

      if(d.display_rotation!=null && state.appliedRot==null){
        state.appliedRot = d.display_rotation;
        state.selectedRot = d.display_rotation;
        updateRotUI();
      }
      if(d.touch){
        $('touchTypeLabel').textContent = d.touch.type + (d.touch.resistive ? ' (calibratable)' : ' (fixed in firmware)');
        if(!d.touch.resistive){
          $('calCard').style.display = 'none';
        }
      }
      updateSavedBanner(d.wifi ? d.wifi.saved_ssid : '');
    }).catch(function(){});
  }

  function updateSavedBanner(ssid){
    var el = $('savedBanner');
    if(ssid){
      el.className = 'banner saved';
      el.innerHTML = 'Saved network: <strong>'+escHtml(ssid)+'</strong>';
    } else {
      el.className = 'banner none';
      el.textContent = 'No saved network';
    }
  }
  function escHtml(s){
    var d = document.createElement('div'); d.textContent = s; return d.innerHTML;
  }

  /* WiFi scan */
  function startScanPoll(){
    if(scanTimer) return;
    scanTimer = setInterval(pollNetworks, 1500);
    pollNetworks();
  }
  function stopScanPoll(){
    if(scanTimer){ clearInterval(scanTimer); scanTimer = null; }
  }
  function pollNetworks(){
    api('GET','/api/wifi/networks').then(function(d){
      var list = $('netList');
      if(d.status==='scanning'){
        list.innerHTML = '<li class="empty"><span class="spinner"></span> Scanning…</li>';
        return;
      }
      state.scanning = false;
      $('scanLabel').textContent = 'Scan Networks';
      if(d.status==='failed'){
        list.innerHTML = '<li class="empty">Scan failed — try again</li>';
        stopScanPoll();
        return;
      }
      if(d.status!=='ready'){
        list.innerHTML = '<li class="empty">Tap Scan to discover networks</li>';
        stopScanPoll();
        return;
      }
      stopScanPoll();
      if(!d.networks || !d.networks.length){
        list.innerHTML = '<li class="empty">No networks found</li>';
        return;
      }
      list.innerHTML = '';
      d.networks.forEach(function(n){
        var li = document.createElement('li');
        li.className = 'net-item'+(state.selectedSsid===n.ssid?' selected':'');
        li.innerHTML = rssiBars(n.rssi)+'<span class="net-ssid">'+escHtml(n.ssid)+'</span><span class="rssi-val">'+n.rssi+'</span>';
        li.addEventListener('click', function(){
          state.selectedSsid = n.ssid;
          $('wifiSsid').value = n.ssid;
          document.querySelectorAll('.net-item').forEach(function(x){ x.classList.remove('selected'); });
          li.classList.add('selected');
        });
        list.appendChild(li);
      });
    }).catch(function(){});
  }

  $('btnScan').addEventListener('click', function(){
    state.scanning = true;
    $('scanLabel').innerHTML = '<span class="spinner"></span> Scanning…';
    $('netList').innerHTML = '<li class="empty"><span class="spinner"></span> Scanning…</li>';
    api('POST','/api/wifi/scan').then(function(){
      startScanPoll();
    }).catch(function(){ toast('Scan request failed'); });
  });

  $('btnConnect').addEventListener('click', function(){
    var ssid = $('wifiSsid').value.trim();
    if(!ssid){ toast('Enter or select an SSID'); return; }
    api('POST','/api/wifi/connect',{ssid:ssid,password:$('wifiPass').value}).then(function(r){
      toast(r.message || (r.ok?'Connecting…':'Failed'));
      refreshStatus();
    }).catch(function(){ toast('Connect failed'); });
  });

  $('btnForget').addEventListener('click', function(){
    if(!confirm('Clear saved WiFi credentials?')) return;
    api('POST','/api/wifi/forget').then(function(r){
      toast(r.message || 'Cleared');
      $('wifiSsid').value = ''; $('wifiPass').value = '';
      state.selectedSsid = null;
      refreshStatus();
    }).catch(function(){ toast('Forget failed'); });
  });

  /* Display rotation */
  function updateRotUI(){
    var rot = state.selectedRot != null ? state.selectedRot : 0;
    document.querySelectorAll('.rot-opt').forEach(function(el){
      el.classList.toggle('selected', parseInt(el.dataset.rot)===rot);
    });
    var deg = rot * 90;
    $('rotPreview').style.transform = 'rotate('+deg+'deg)';
    $('btnApplyRot').disabled = (state.appliedRot != null && rot === state.appliedRot);
  }
  document.querySelectorAll('.rot-opt').forEach(function(el){
    el.addEventListener('click', function(){
      state.selectedRot = parseInt(el.dataset.rot);
      updateRotUI();
    });
  });
  $('btnApplyRot').addEventListener('click', function(){
    api('POST','/api/display/rotation',{rotation:state.selectedRot}).then(function(r){
      toast(r.message || 'Applied');
      state.appliedRot = state.selectedRot;
      updateRotUI();
      refreshStatus();
    }).catch(function(){ toast('Rotation failed'); });
  });

  /* Touch */
  function resizeTouchPad(){
    var pad = $('touchPad');
    var maxW = Math.min(360, pad.parentElement.clientWidth - 20);
    var aspect = screenW / screenH;
    var w, h;
    if(aspect >= 1){ w = maxW; h = maxW / aspect; }
    else { h = 200; w = 200 * aspect; }
    pad.style.width = Math.round(w)+'px';
    pad.style.height = Math.round(h)+'px';
  }
  function startTouchPoll(){
    if(touchTimer) return;
    touchTimer = setInterval(pollTouch, 120);
    pollTouch();
  }
  function stopTouchPoll(){
    if(touchTimer){ clearInterval(touchTimer); touchTimer = null; }
    var dot = $('touchDot');
    dot.classList.remove('visible');
    $('tRawX').textContent = '—'; $('tRawY').textContent = '—';
    $('tMapX').textContent = '—'; $('tMapY').textContent = '—';
    $('tPressed').textContent = 'no';
  }
  function pollTouch(){
    api('GET','/api/touch/sample').then(function(d){
      if(d.pressed){
        $('tRawX').textContent = d.rawX; $('tRawY').textContent = d.rawY;
        $('tMapX').textContent = d.mappedX; $('tMapY').textContent = d.mappedY;
        $('tPressed').textContent = 'yes';
        var pad = $('touchPad'), dot = $('touchDot');
        var pw = pad.clientWidth, ph = pad.clientHeight;
        dot.style.left = (d.mappedX / screenW * pw)+'px';
        dot.style.top = (d.mappedY / screenH * ph)+'px';
        dot.classList.add('visible');
      } else {
        $('tPressed').textContent = 'no';
        $('touchDot').classList.remove('visible');
      }
    }).catch(function(){});
  }

  function loadSettings(){
    api('GET','/api/settings').then(function(d){
      state.settings = d;
      if(d.display_rotation!=null){
        state.appliedRot = d.display_rotation;
        state.selectedRot = d.display_rotation;
        updateRotUI();
      }
      if(d.touch && d.touch.cal){
        $('calXMin').value = d.touch.cal.xMin;
        $('calXMax').value = d.touch.cal.xMax;
        $('calYMin').value = d.touch.cal.yMin;
        $('calYMax').value = d.touch.cal.yMax;
      }
      if(d.wifi) updateSavedBanner(d.wifi.saved_ssid || '');
    }).catch(function(){});
  }

  $('btnCalSave').addEventListener('click', function(){
    api('POST','/api/touch/cal',{
      xMin: parseInt($('calXMin').value)||0,
      xMax: parseInt($('calXMax').value)||0,
      yMin: parseInt($('calYMin').value)||0,
      yMax: parseInt($('calYMax').value)||0
    }).then(function(r){ toast(r.message || 'Saved'); }).catch(function(){ toast('Save failed'); });
  });
  $('btnCalReset').addEventListener('click', function(){
    if(!confirm('Reset touch calibration to defaults?')) return;
    api('POST','/api/touch/reset').then(function(r){
      toast(r.message || 'Reset');
      loadSettings();
    }).catch(function(){ toast('Reset failed'); });
  });
  $('btnCalReload').addEventListener('click', function(){ loadSettings(); toast('Reloaded'); });

  window.addEventListener('resize', resizeTouchPad);

  /* Init */
  refreshStatus();
  loadSettings();
  statusTimer = setInterval(refreshStatus, 2000);
})();
</script>
</body>
</html>)rawhtml";
