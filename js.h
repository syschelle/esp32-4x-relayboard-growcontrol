const char HTML_JS[] PROGMEM = R"rawliteral(
<script>

//update vales from sensor data section

// function, load JSON from /sensordata
async function updateSensorValues() {
  try {
   const response = await fetch('/sensordata');
   if (!response.ok) {
     console.error('Error retrieving sensor data:', response.status);
       return;
   }
   const data = await response.json();
   // If the sensor is not available, possibly set to 'N/A'.
   if (data.temperature !== null) {
     document.getElementById('tempSpan').textContent = data.temperature.toFixed(1);
     document.getElementById('humSpan').textContent  = data.humidity.toFixed(1);
     document.getElementById('vpdSpan').textContent  = data.vpd.toFixed(2);
     // Should the target VPD potentially be retrieved again from Prefs? Alternatively: keep it static.
     // document.getElementById('vpdTargetSpan').textContent = data.vpdTarget.toFixed(2);
     
     // blinkElement after update 
     blinkElement('tempSpan');
     blinkElement('humSpan');
     blinkElement('vpdSpan');

   } else {
     document.getElementById('tempSpan').textContent = 'N/A';
     document.getElementById('humSpan').textContent  = 'N/A';
     document.getElementById('vpdSpan').textContent  = 'N/A';
   }
 } catch (error) {
   console.error('Exception in updateSensorValues():', error);
 }
}

//Retrieve again every X seconds (e.g. every 10 seconds)
setInterval(updateSensorValues, 10000); // 10000 ms = 10 seconds

//Call it directly when loading, so that fresh values are already present when opening.
window.addEventListener('load', updateSensorValues);

function blinkElement(id) {
  const el = document.getElementById(id);
  el.classList.add('blink-text');
  setTimeout(() => el.classList.remove('blink-text'), 1000);
}

//stausmessage section

//functions that query /autostatus
async function checkAutoStatus() {
  try {
    const res = await fetch('/autostatus');
    if (!res.ok) return;
    const j = await res.json();
    //If autoMessage is not empty, we display it:
    if (j.autoMessage && j.autoMessage.length > 0) {
      const s = document.getElementById('statusMsg');
      s.textContent = j.autoMessage;
      s.style.display = 'block';
      //Fade out after 2 seconds
      setTimeout(() => { s.style.display = 'none'; }, 2000);
    }
  } catch (err) {
   console.error('Error bei checkAutoStatus():', err);
  }
}

//Start polling every 5 seconds
setInterval(checkAutoStatus, 5000);
//Check directly during the first loading
window.addEventListener('load', checkAutoStatus);

async function loadSection(path) {
 const res = await fetch(path);
 if (!res.ok) return console.error('Failed to load', path);
 document.getElementById('content').innerHTML = await res.text();
}

document.getElementById('nav-status').addEventListener('click', e => {
  e.preventDefault();
  loadSection('/status');
});
document.getElementById('nav-settings').addEventListener('click', e => {
  e.preventDefault();
  loadSection('/settings');
});
document.getElementById('nav-diary').addEventListener('click', e => {
  e.preventDefault();
  loadSection('/diary');
});
document.getElementById('nav-guide').addEventListener('click', e => {
  e.preventDefault();
  loadSection('/guide');
});

// initial view
window.addEventListener('load', () => loadSection('/status'));

const d = new Date();
document.getElementById('datum').textContent = d.toLocaleDateString('de-DE', {
  year: 'numeric',
  month: 'long',
  day: 'numeric'
});

// Populate dropdown with timezones
const timezoneSelect = document.getElementById('timezone-select');
const timezones = Intl.supportedValuesOf('timeZone'); // Fetch supported timezones
timezones.forEach(tz => {
  const option = document.createElement('option');
  option.value = tz;
  option.textContent = tz;
  timezoneSelect.appendChild(option);
});
</script>
)rawliteral";