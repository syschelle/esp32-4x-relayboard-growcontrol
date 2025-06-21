const char HTML_HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html lang="de">
<html class='dark'>
<head>
  <meta charset='UTF-8' />
  <meta name='viewport' content='width=device-width, initial-scale=1.0' />
  <title>esp grow control</title>
  <style>
    :root {
      --bg: #121212;
      --fg: #f0f0f0;
      --header-bg: #1e1e1e;
      --statusMsg-bg: #003366;
      --statusMsg-text: #dcefff;
      --link-color: #66aaff;
      --gray: #888;
      --bg: #111827;
      --grid-with: 800;
    }

    body {
      margin: 0;
      font-family: sans-serif;
      background-color: var(--bg);
      color: var(--fg);
      padding: 1rem;
    }

    header {
      background-color: var(--header-bg);
      border-radius: 16px;
      padding: 1rem;
      margin-bottom: 1rem;
    }

    .header-grid {
      display: grid;
      grid-template-columns: 1fr 2fr 1fr;
      align-items: center;
    }

    .center-text {
      text-align: center;
    }

    .center-text .title {
      font-weight: bold;
      font-size: 1.2rem;
    }

    .center-text .subtitle {
      font-size: 0.9rem;
      color: var(--gray);
    }

    .right-align {
      text-align: right;
      font-size: 0.9rem;
    }

    .statusMsg {
      background-color: var(--statusMsg-bg);
      color: var(--statusMsg-text);
	    height: 20px;
      text-align: center;
      padding: 0.5rem;
      border-radius: 8px;
      margin-top: 1rem;
    }

    nav {
      margin-top: 1rem;
      display: flex;
      justify-content: center;
      gap: 2rem;
      border-top: 1px solid #333;
      padding-top: 0.5rem;
    }

    nav a {
      text-decoration: none;
      color: var(--link-color);
      font-weight: 500;
    }

    nav a:hover {
      text-decoration: underline;
      text-color: underline;
    }

    body {
      font-family: Arial, sans-serif;
      background-color: var(--bg);
      padding: 20px;
    }

    h2 {
      text-align: center;
      color: var(--link-color);
    }

    h3 {
      text-align: center;
      color: var(--link-color);
      text-decoration: underline;
    }
    
    .blink-text {
    animation: text-blink 0.5s ease;
    }

    @keyframes text-blink {
      from { color: #ff9900; }
      to   { color: inherit; }
    }

    .logo {
      font-size: 30px;
      color: #ff9900;
    }

    .grid-1x1 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(1, 1fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-3x1 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(3, 1fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-1x3 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(1, 3fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }

    .grid-1x4 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(1, 4fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-4x1 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(4, 1fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-4x2 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(4, 2fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-5x1 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(5, 1fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }

    .grid-1x5 {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(1, 5fr);
      gap: 15px;
      max-width: var(--grid-with);
      margin: auto;
      align-items: center;
    }
    
    .grid-settings {
      display: grid;
      justify-content: center;
      grid-template-columns: repeat(1, 5fr);
      gap: 15px;
      max-width: 400;
      margin: auto;
      align-items: right;
    }

    .tile {
      background-color: var(--header-bg);
      padding: 20px;
      text-align: center;
      border-radius: 12px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      font-size: 1.1em;
    }

    .tile-left {
      background-color: var(--header-bg);
      padding: 20px;
      text-align: left;
      border-radius: 12px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      font-size: 1.1em;
    }

    .tile-right {
      background-color: var(--header-bg);
      padding: 20px;
      text-align: right;
      border-radius: 12px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      font-size: 1.1em;
    }

    .tile-right-settings {
      background-color: var(--header-bg);
      padding: 20px;
      text-align: right;
      border-radius: 12px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      font-size: 1.1em;
    }

    button {
      background-color: var(--header-bg);
      cursor: pointer;
      text-transform: uppercase;
      color: white;
      padding: 10px 10px;
      margin: 10px;
      border: 2px solid #ff9900;
      border-radius: 8px;
      font-size: 1em;
      box-shadow: 0 3px 6px rgba(0,0,0,0.1);
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: var(--statusMsg-bg);
    }

    input, textarea, select {
      font-family: sans-serif;
      padding: 0.5rem;
      margin: 0.5rem 0;
      background: var(--bg);
      color: #f0f0f0;
      border: 1px solid #444;
      border-radius: 6px;
      cursor: pointer;
      color-scheme: dark;
    }
  </style>
</head>
<body>
<header>
    <div class='header-grid'>
      <div class='logo'>%CONTROLLERNAME%</div>
      <div class='center-text'>
        <div class='title' style='color:#ff9900;'>%ELAPSEDGROW% current phase: %CURRPHAES%</div>
        <div class='title' style='color:#D20103;'>%ELAPSEDFLOWERING%</div>
      </div>
      <div class='right-align' id='datum'></div>
    </div>

    <div class='statusMsg'>
	    <!-- Status updates are written here. -->
    </div>

    <nav>
      <a href='#' id='nav-status' title='Status'>📋</a>
      <a href='#' id='nav-settings' title='Settings'>🛠️</a>
      <a href='#' id='nav-diary' title='Grow Diary'>🌱</a>
      <a href='#' id='nav-guide' title='Grow Guide'>📙</a>
      <a href='https://github.com/syschelle/esp32-4x-relayboard-growcontrol/blob/main/README.md' target='_blank' title='Manual'>📑</a>
    </nav>
  </header>

  <div id='content'>
   <!-- dynamic content will be loaded here -->
  </div>
)rawliteral";

const char HTML_BME280[] PROGMEM = R"rawliteral(
<h2>📋 Status</h2>
<h3>Messurments</h3>
 <div class='grid-3x1'>
  <div class='tile'>Current Temperature:<BR><span id='tempSpan'>%CTEMP%</span> °C</div>
  <div class='tile'>Current VPD:<BR><span id='vpdSpan'>%CVPD%</span> kPa</div>
  <div class='tile'>Humidity:<BR><span id='humSpan'>%HUMIDITY%</span> %</span></div>
  <div class='tile'>Target Temperature:<BR>%TTEMP% °C</div>
  <div class='tile'>Target VPD:<BR>%TVPD% kPa</div>
)rawliteral";

const char HTML_HCSR04[] PROGMEM = R"rawliteral(
 <div class='tile'>Water Level:<BR>%WATERLEVEL%</div>
 </div>
 <div class='grid-1x1'><button id='waterlevelBtn' type='button' type='button' onclick=\"window.location.href='/waterlevel'\">Ping Water Tank Sonar</button></div>
)rawliteral";

const char HTML_END[] PROGMEM = R"rawliteral(
  </body>
</html>
)rawliteral";

const char HTML_SETTINGS_START[] PROGMEM = R"rawliteral(
<h2>🛠️ Settings</h2><form action='/save' method='post'>
 <div class='grid-settings'>
    <div class='tile-right-settings'>Set Controller Name: <input name='webControllerName' type='Text' value='%CONTRALLERNAME%' maxlength='20' placeholder='max leght 20'>
    <br>Start Grow Date: <input name='webGrowStart' style='width: 120px;' type='date' value='%GROWSTARTDATE%'>
    <br>Sart Flowering Date: <input name='webFloweringStart' style='width: 120px;' type='date' value='%FLOWERINGSTARTDATE%'></div>
    <div class='tile-right-settings'>Target Temperature: <input name='set_temp' style='width: 70px;' type='number' step='0.5' min='18' max='30' value='%TARGETTEMPERATURE%'>°C</div>
)rawliteral";

const char HTML_SETTINGS_END[] PROGMEM = R"rawliteral(
 <button type='submit'>Save Settings</button></form><div class='form-actions'><button id='rebootBtn' type='button' type='button' onclick=\"window.location.href='/reboot'\">Reboot Controller</button></div>
)rawliteral";

const char HTML_DIARYTOP[] PROGMEM = R"rawliteral(
<h2>🌱 Grow Diary</h2>
<div class='grid-1x4' >
  <div class='tile' >Date:<br>
    <input type='date' id='notedate'  value='%ACTUALDATE%' /></div>
)rawliteral";

const char HTML_DIARYBOTTOM[] PROGMEM = R"rawliteral(
    <div class='tile'>Note:<br>
    <input type=text id='note' style='width: 360px;' placeholder='What happened? Max. 50 characters!'></div>
    <div class='tile'>
      <button onclick='addEntry()'>Add Entry</button>
      <button onclick='loadEntries()'>Load Entries</button>
      <button onclick='clearEntries()'>Clear Entries</button>
    </div>
  </div>
)rawliteral";

const char HTML_GUIDE[] PROGMEM = R"rawliteral(
<h2>📙 Grow Guideline</h2>
 <div class='grid-1x4'>
  <div class='tile'><font color='red'><b>This guide is just a suggestion!<br>You are going on your own journey.</b></font></div>
  <div class='tile'><font color='green'><b>Seedling/Clone Phase</b><br><br>
  <b>VPD:</b><br>0.2 – 0.8 kPa<br><br>
  <b>Light:</b><br><b>100 – 300 μmol/m2.s<br>20 mol/m²/day<br>100 - 200 PPFD<br><br>
  <b>Temperature Day:</b><br>20–25 °C<br><br>
  <b>Temperature Night:</b><br>constant, not below 20 °C</font></div>
  
  <div class='tile'><font color='orange'>Vegetative Phase</b><br><br>
  <b>VPD:</b><br>0.8 – 1.4 kPa<br><br><br>
  <b>Light:</b><br>400 – 600 μmol/m2.s<br>30-40 mol/m²/day<br>400 - 600 PPFD<br><br>
  <b>Temperature Day:</b><br>22–28 °C (range 20°C - 30°C)<br><br>
  <b>Temperature Night:</b><br>approximately 2–4 °C cooler (20°C – 24°C)</font></div>

  <div class='tile'><font color='red'>Flowering Phase</b><br><br>
  <b>VPD:</b><br>1.2 – 1.5 kPa<br><br><br>
  <b>Light:</b><br>700 – 1.000 μmol/m2.s<br>40-50 mol/m²/day<br>800 - 1000 PPFD<br><br>
  <b>Temperature Day:</b> 20–26 °C (last 2 weeks 19°C - 24°C)<br><br>
  <b>Temperature Night:</b><br>approximately 2–10 °C cooler (16°C – 21°C)</font></div>
 </div>
)rawliteral";
