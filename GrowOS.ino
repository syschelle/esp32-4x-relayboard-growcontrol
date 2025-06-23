// GrowtentController.ino
// Control of 4 relays (ESP32), BME280 sensor, 5 Shelly Plug S Plus,
// growth phase VPD targets persisted, NTP time, scheduled fans, MQTT, temperature setpoint control, Pushover notifications
// and I2C LCD 2004 module

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_BME280.h>
#include <HTTPClient.h>
#include <esp_http_client.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <math.h>
#include <PubSubClient.h>
#include <cmath>
#include <NewPing.h>
#include <variables.h>
#include <function.h>
#include <html.h>
#include <js.h>

// tasks
#include <task_Check_Vpd.h>
#include <task_Check_Temperature.h>

// ====== Function Prototypes ======
void loadPreferences();
void savePreferences();
void handleRoot();
void handleRelay();
void handleShelly();
void pollShellyStatuses();
void checkWaterlevel();
bool mqttConnect();
void sendPushover(const char* message);

// this function is all prefs loading
void loadPreferences() {
   
  // Open preferences namespace
  prefs.begin("growtent", false);

  // Debug if enabled
  debug = prefs.getBool("debug", false);

  // Load start date from prefs
  startDate = prefs.getString("start_date", "");
  tm tmStart = { 0 };
  int d, m, y;
  sscanf(startDate.c_str(), "%d.%d.%d", &d, &m, &y);
  tmStart.tm_mday = d;
  tmStart.tm_mon = m - 1;
  tmStart.tm_year = y - 1900;
  time_t startEpoch = mktime(&tmStart);
  time_t nowEpoch = mktime(&now);
  long diffSec = nowEpoch - startEpoch;
  int diffDays = diffSec / 86400;
  int diffWeeks = (diffDays / 7) + 1;

  startFlowering = prefs.getString("flowering_start");
  startDrying = prefs.getString("drying_start");

  // Load current phase, VPD targets from prefs
  int curPhase = prefs.getUInt("phase", 1);
  float vpdTargets[4];
  for (int i = 1; i <= 3; i++) {
    char key[16];
    snprintf(key, sizeof(key), "vpd_%d", i);
    vpdTargets[i] = prefs.getFloat(key, defaultVPDs[i]);
  }

  // Load current tempTarget from prefs
  float setTemp = prefs.getFloat("set_temp", defaultSetTemp);
  
  // MQTT if enabled
  mqtt = prefs.getBool("mqtt", false);
  // Load Pushover credentials or defaults
  mqtt_Broker = prefs.getString("mqtt_broker", "127.0.0.1");
  mqtt_Port = prefs.getString("mqtt_port", "1883");
  mqtt_User = prefs.getString("mqtt_user", "");
  mqtt_Pass = prefs.getString("mqtt_pass", "");

  // Read sensor temperatur, humidity and vpd
  float temperature = 0, humidity = 0, currentVPD = 0;
  if (bmeAvailable) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    float svp = 0.6108f * exp((17.27f * temperature) / (temperature + 237.3f));
    currentVPD = svp - (humidity / 100.0f) * svp;
  }

  // Load watering interval from prefs
  // MQTT if enabled
  watering = prefs.getBool("watering", false);
  waterInterval = prefs.getUInt("water_interval", DEFAULT_WATER_INTERVAL);

  // Load water tank settings from prefs
  tank = prefs.getBool("tank", false);
  tank_full = prefs.getUInt("tank_full", 10);
  tank_empty = prefs.getUInt("tank_empty", 300);
  tank_warning = prefs.getUInt("tank_warning", 30);

  // Load growlight from prefs
  String lightStart = prefs.getString("light_start", DEFAULT_LIGHT_START);
  uint8_t lightHours = prefs.getUInt("light_hours", DEFAULT_LIGHT_HOURS);

  //Decomposing 'HH:MM' into sh (start hour) and sm (start minute)
  int sh = lightStart.substring(0, 2).toInt();
  int sm = lightStart.substring(3, 5).toInt();

  // Calculate end time and limit to 0–23
  int eh = (sh + lightHours) % 24;
  int em = sm;  // Minuten bleiben gleich

   // Format as "HH:MM" (two digits, leading zeros)
  char buf[6];
  sprintf(buf, "%02d:%02d", eh, em);
  lightEnd = buf;

  // Load start time from prefs
  String stored = prefs.getString("start_date", "");
  // Convert "DD.MM.YYYY" → "YYYY-MM-DD" for the date input
  String iso = "";
  if (stored.length() == 10) {
    iso = stored.substring(6, 10) + "-" + stored.substring(3, 5) + "-" + stored.substring(0, 2);
  }

  // Load start time from prefs
  String storedfl = prefs.getString("flowering_date", "");
  // Convert "DD.MM.YYYY" → "YYYY-MM-DD" for the date input
  String isofl = "";
  if (stored.length() == 10) {
    isofl = stored.substring(6, 10) + "-" + stored.substring(3, 5) + "-" + stored.substring(0, 2);
  }

  // MQTT if enabled
  flowering = prefs.getBool("flowering", false);

}

// Serve the “Settings” section: all your forms
void handleSettings() {

  String html;

  // Read sensor temperatur, humidity and vpd
  float temperature = 0, humidity = 0, currentVPD = 0;
  if (bmeAvailable) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    float svp = 0.6108f * exp((17.27f * temperature) / (temperature + 237.3f));
    currentVPD = svp - (humidity / 100.0f) * svp;
  }

  // Load start date from prefs
  String startDate = prefs.getString("start_date", "");
  struct tm tmStart = { 0 };
  int d, m, y;
  sscanf(startDate.c_str(), "%d.%d.%d", &d, &m, &y);
  tmStart.tm_mday = d;
  tmStart.tm_mon = m - 1;
  tmStart.tm_year = y - 1900;
  time_t startEpoch = mktime(&tmStart);
  time_t nowEpoch = mktime(&now);
  long diffSec = nowEpoch - startEpoch;
  int diffDays = diffSec / 86400;
  int diffWeeks = (diffDays / 7) + 1;

  // Load current phase, VPD targets from prefs
  int curPhase = prefs.getUInt("phase", 1);
  float vpdTargets[5];
  for (int i = 1; i <= 4; i++) {
    char key[16];
    snprintf(key, sizeof(key), "vpd_%d", i);
    vpdTargets[i] = prefs.getFloat(key, defaultVPDs[i]);
  }

  // Load current tempTarget from prefs
  float setTemp = prefs.getFloat("set_temp", defaultSetTemp);


  // Load growlight from prefs
  String lightStart = prefs.getString("light_start", DEFAULT_LIGHT_START);
  uint8_t lightHours = prefs.getUInt("light_hours", DEFAULT_LIGHT_HOURS);

  //Decomposing 'HH:MM' into sh (start hour) and sm (start minute)
  int sh = lightStart.substring(0, 2).toInt();
  int sm = lightStart.substring(3, 5).toInt();

  // Calculate end time and limit to 0–23
  int eh = (sh + lightHours) % 24;
  int em = sm;  // Minuten bleiben gleich

  // Format as "HH:MM" (two digits, leading zeros)
  char buf[6];
  sprintf(buf, "%02d:%02d", eh, em);
  lightEnd = buf;

  // Load start date and flowering date from prefs
  //String isoStart = prefs.getString("start_date", "");
  //String isoFlowering = prefs.getString("flowering_start", "");

  // setting section
  html += FPSTR(HTML_SETTINGS_START);
  
  // setting controller name
  html.replace("%CONTRALLERNAME%", prefs.getString("controller_name", ""));
  // setting ntp server
  html.replace("%TIMEZOEN%", tzInfo);
  // setting timezone (POSIX-String)
  html.replace("%NTPSERVER%",ntpServer);
  // setting grow start date
  html.replace("%GROWSTARTDATE%", prefs.getString("start_date", ""));
  //setting flowering date
  html.replace("%FLOWERINGSTARTDATE%", prefs.getString("flowering_start", ""));
  //setting drying date
  html.replace("%DRYINGSTARTDATE%", prefs.getString("drying_start", ""));
  //setting target temperature
  html.replace("%TARGETTEMPERATURE%", String(setTemp));

  // setting section growphase and temperature
  html += "<div class='tile-right-settings'>Set Cur. Phase: <select name='phase'>";
  for (int i = 1; i <= 4; i++) {
    html += String("<option value='") + i + "'" + (i == curPhase ? " selected" : "") + ">" + phaseNames[i] + "</option>";
  }
  html += "</select></label><br>";
  for (int i = 1; i <= 4; i++) {
    html += String("<label>Set ") + phaseNames[i] + " VPD: <input name='vpd_" + String(i) + "' type='number' style='width: 50px;' step='0.01' value='" + String(vpdTargets[i], 2) + "'>kPa</label><br>";
  }
  html +="</div>";
  //setting Growlight
  html += "<div class='tile-right-settings'>Light on at (time): <input type='time' name='light_start' value='" + lightStart + "' required><br>";

  //growlight: duration (10–20 hours)
  html +="Day duration (hour): <select name='light_hours'>";
  for (uint8_t h = 10; h <= 20; h++) {
    html += String("<option value='") + h + "'"
         + (h == lightHours ? " selected" : "") + ">"
         + String(h) + "</option>";
  }
  html +="</select></br>";
  html +="Light off at (time): <input type='time' id='offTimeInput' name='light_end' value='" + String(lightEnd) + "' readonly><br>";
  html +="Night duration (hour): <input type='number' style='width: 60px;' name='night_hours' step='0.5' min='10' max='20' value='" + String(24 - lightHours) + "' readonly></br>";
  html +="</div>";
  html +="<div class='tile-right-settings'>";
  if (watering) {
    html += "Enable Watering Nofification: <input type='checkbox' name='watering' checked></label><br>";
  } else {
    html += "Enable Watering Nofification: <input type='checkbox' name='watering'></br>";
  }
  html +="Watering every (day): <select name='water_interval'>";
  for (uint8_t wti = 1; wti <= 10; wti++) {
    html += String("<option value='") + wti + "'"
         + (wti == waterInterval ? " selected" : "") + ">"
         + String(wti) + "</option>";
  }
  html +="</select></br>";
  html +="<label style='color:red;'>Pushover must be activated!</label>";
  html +="</div>";
  if (tank) {
    html += "<div class='tile-right-settings'>Enable Water Tank: <input type='checkbox' name='tank' checked></label><br>";
  } else {
    html += "<div class='tile-right-settings'>Enable Water Tank: <input type='checkbox' name='tank'></label><br>";
  }
  html +="Tank Full (cm): <input name='tank_full' type='number' style='width: 50px;' step='1' value='" + String(tank_full) + "'><br>";
  html +="Tank Empty (cm): <input name='tank_empty' type='number' style='width: 50px;' step='1' value='" + String(tank_empty) + "'><br>";
  html +="Warning at (%): <input name='tank_warning' type='number' style='width: 50px;' min='10' max='50' step='5' value='" + String(tank_warning) + "'><br>";
  html +="<label style='color:red;'>Pushover must be activated!</label>";
  html +="</div>";
  // setting section mqtt
  if (mqtt) {
    html += "<div class='tile-right-settings'>Enable MQTT: <input type='checkbox' name='mqtt' checked>";
  } else {
    html += "<div class='tile-right-settings'>Enable MQTT: <input type='checkbox' name='mqtt'>";
  }
  html += "<br>MQTT Broker: <input name='mqtt_broker' type='password' value='" + prefs.getString("mqtt_broker", "") + "' placeholder='127.0.0.1'>";
  html += "<br>MQTT Port: <input name='mqtt_port' type='text' value='" + prefs.getString("mqtt_port", "1883") + "'>";
  html += "<br>MQTT User: <input name='mqtt_user' type='password' value='" + prefs.getString("mqtt_user", "") + "'>";
  html += "<br>MQTT Pass: <input name='mqtt_pass' type='password' value='" + prefs.getString("mqtt_pass", "") + "'>";
  html += "</div>";
  // setting section pushover
  if (pushover) {
    html += "<div class='tile-right-settings'>Enable Pushover: <input type='checkbox' name='pushover' checked>";
  } else {
    html += "<div class='tile-right-settings'>Enable Pushover: <input type='checkbox' name='pushover'>";
  }
  html += "<br>Pushover Token: <input name='API Token' type='password' value='" + prefs.getString("pushover_token", "") + "'>";
  html += "<br>Pushover User: <input name='User Key' type='password' value='" + prefs.getString("pushover_user", "") + "'>";
  html += "</div>";
  // setting section debug
  if (debug) {
    html += "<div class='tile-right-settings'>Enable Debuglevel: <input type='checkbox' name='debug' checked></div>";
  } else {
    html += "<div class='tile-right-settings'>Enable Debuglevel: <input type='checkbox' name='debug'></div>";
  }
  
  html += FPSTR(HTML_SETTINGS_END);

  server.send(200, "text/html", html);
}

void handleDiary() {
  String html;

  int curPhase = prefs.getUInt("phase", 1);
  
  html += FPSTR(HTML_DIARYTOP);
  
  html.replace("%ACTUALDATE%", String(actualDate));

  html += "<div class='tile'>Phase:<br>";
  html += "<select name='phase'>";
  for (int i = 1; i <= 4; i++) {
    html += "<option value='" + String(phaseNames[i]) + "'" + (i == curPhase ? " selected" : "") + ">" + String(phaseNames[i]) + "</option>";
  }
  html += "</select></div>";

  html += FPSTR(HTML_DIARYBOTTOM);

  server.send(200, "text/html", html);
}

void handleGuide() {
  String html;

  html += FPSTR(HTML_GUIDE);

  server.send(200, "text/html", html);
}

void handleManual() {
  String html;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  prefs.begin("growtent", false);

  //Load WIFI access data from prefs
  String ssid = prefs.getString("wifi_ssid", DEFAULT_WIFI_SSID);
  String pass = prefs.getString("wifi_pass", DEFAULT_WIFI_PASS);

  // In setup() nach prefs.begin(...)
  tzInfo    = prefs.getString("tz_info", DEFAULT_TZ_INFO);
  ntpServer = prefs.getString("ntp_server", DEFAULT_NTP_SERVER);

  
  if (ssid.length() && pass.length()) {
    // connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("Connecting to Wifi");
    Serial.print(ssid);
    unsigned long start = millis();
    const unsigned long timeout = 10000; // 10 seconds
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
      delay(500);
      Serial.print('.');
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.print("Connected, IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println();
      Serial.println("WIFI connection failed!");
      softAp = true;
      startSoftAP();
    }
  } else {
    // No data stored → SoftAP
    softAp = true;
    startSoftAP();
  }
  
  if (!softAp) {
    // initialize BME280 sensor
    unsigned long startTime = millis();

    while (millis() - startTime < 10000) {
      if (bme.begin(BME_ADDR)) {
        Serial.println("✔ BME280 successfully initialized!");
        bmeAvailable = true;
        break;
      } else {
        Serial.println("✖ BME280 not found, retrying in 500 ms");
        delay(500);
      }
    }

    // Initialize relay outputs (LOW = OFF)
    for (int i = 0; i < NUM_RELAYS; i++) {
      pinMode(relayPins[i], OUTPUT);
      digitalWrite(relayPins[i], LOW);
    }

    // load all preferences
    loadPreferences();

    // syncing NTP time
    Serial.println("syncing NTP time");
    struct tm local;
    configTzTime(tzInfo.c_str(), ntpServer.c_str());  // Synchronizing ESP32 system time with NTP
    getLocalTime(&local, 10000);        // Try to synchronize 10 s
    getLocalTime(&local);
    //set actual date in global variable actualDate
    char readDate[11]; // YYYY-MM-DD + null
    strftime(actualDate, sizeof(readDate), "%Y-%m-%d", &local);
    Serial.println(&local, "now: %d.%m.%y  Zeit: %H:%M:%S");  // Format date print output

    // Persistent start date
    if (debug) Serial.println("Start Date: " + startDate );
    
    if (debug) Serial.println("Flowering Date: " + startFlowering );
    //enable mqtt if pref variable mqtt ist true
    
    if (server.hasArg("mqtt")) {
      //uint16_t mqttPort = mqttPortStr.toInt();            // String → Integer
      mqttClient.setServer(mqtt_Broker.c_str(), mqtt_Port.toInt());  // String → const char*putFloat("set_temp"
      mqttConnect();
    }

    xTaskCreatePinnedToCore(
      taskCheckVpd,                   // Task function
      "Check VPD every 30 seconds",   // Task name
      8192,                           // Stack size
      NULL,                           // Task input parameters
      1,                              // Task priority, be carefull when changing this
      NULL,                           // Task handle, add one if you want control over the task (resume or suspend the task)
      1                               // Core to run the task on
    );

    xTaskCreatePinnedToCore(
      taskCheckTemperature,                   // Task function
      "Check Temperature every 60 seconds",   // Task name
      8192,                                   // Stack size
      NULL,                                   // Task input parameters
      1,                                      // Task priority, be carefull when changing this
      NULL,                                   // Task handle, add one if you want control over the task (resume or suspend the task)
      1                                       // Core to run the task on
    );

  }
  
  // Define routes
  //route for factory reset
  server.on("/factoryReset", HTTP_GET, [](){
  resetFactory();
  // send 303 redirect not nötig, weil wir per ESP.restart neu starten
  });
  
  //route to store Wifi ssid and passord
  server.on("/wifi", HTTP_POST, [](){
    // load parameters
    String ssid = server.arg("webWifiSsid");
    String pass = server.arg("WebWifiPass");
    // Speichere in prefs
    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);
    Serial.println("save wifi data: " + ssid + " and " + pass );
    //prefs.end();
    server.sendHeader("Location", "/");
    server.send(303);
    delay(1000);
    ESP.restart();
  });
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/diary", HTTP_GET, handleDiary);
  server.on("/guide", HTTP_GET, handleGuide);
  server.on("/manual", HTTP_GET, handleManual);  
  server.on("/waterlevel", HTTP_GET, [](){
    checkWaterlevel();
    server.sendHeader("Location","/"); 
    server.send(303);              // redirect back to the status page
  });
  server.on("/save", HTTP_POST, savePreferences);
  server.on("/reboot", HTTP_GET, []() {
    server.send(200, "text/plain", "Rebooting...");
    delay(100);
    ESP.restart();
  });
  server.on("/autostatus", HTTP_GET, []() {
    DynamicJsonDocument doc(128);
    doc["autoMessage"] = lastAutoMessage;
    // If you only want to give the message to the client once, then clear afterward:
    lastAutoMessage = "";
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
  });
  //api interface
  server.on("/api", HTTP_GET, [](){
    // build XML
    String xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">";
    xml += "<Growtent>\n";
    xml += "  <Temperature unit=\"°C\">" + String(lastTemperature) + "</Temperature>\n";
    xml += "  <Humidity    unit=\"%\">"   + String(lastHumidity) + "</Humidity>\n";
    xml += "  <VPD         unit=\"kPa\">"  + String(lastVPD) + "</VPD>\n";
    if (distanceCM >= 0) {
      xml += "  <Distance    unit=\"cm\">"   + String(distanceCM) + "</Distance>\n";
    } else {
      xml += "  <Distance    unit=\"cm\"/>\n";
    }
    xml += "</Growtent>\n";
    xml += "</xsl:stylesheet>";

    // Sende mit XML-Content-Type
    server.send(200, "application/xml", xml);
  });

  // route for pure sensor data. for update on the fly in the web page.
  server.on("/sensordata", HTTP_GET, []() {
    // JSON building: { "temperature": 21.5, "humidity": 45.3, "vpd": 1.23 }
    DynamicJsonDocument doc(128);

    if (bmeAvailable) {
      float t = bme.readTemperature();
      float h = bme.readHumidity();
      float svp = 0.6108f * exp((17.27f * t) / (t + 237.3f));
      float vpd = svp - (h / 100.0f) * svp;

      doc["temperature"] = t;
      doc["humidity"]    = h;
      doc["vpd"]         = vpd;
    } else {
      doc["temperature"] = nullptr;
      doc["humidity"]    = nullptr;
      doc["vpd"]         = nullptr;
    }

    String payload;
    serializeJson(doc, payload);

    server.send(200, "application/json", payload);
  });
  
  //check HC-SR04 for the first time
  checkWaterlevel();

  // Start webserver
  server.begin();
  Serial.println("Web server started");

  lastShellyPoll = 0;

}

void loop() {
  server.handleClient();

  

  if (mqtt) {
    if (!mqttClient.connected()) mqttConnect();
    mqttClient.loop();
  }
  

  // Daily NTP resync at 01:00
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    if (timeinfo.tm_hour == 1 && timeinfo.tm_min == 0 && timeinfo.tm_mday != lastSyncDay) {
      if (debug) Serial.println("Performing daily NTP sync...");
      if (pushover) sendPushover("Performing daily NTP sync...");
      configTzTime(tzInfo.c_str(), ntpServer.c_str());
      lastSyncDay = timeinfo.tm_mday;
    }
  }

  // hourly water level check
  if (getLocalTime(&timeinfo)) {
    static int lastHour = -1;
    if (now.tm_min == 0 && now.tm_hour != lastHour) {
      lastHour = now.tm_hour;
      checkWaterlevel();
      if (debug) {
        Serial.println("Executed hourly water level check.Level: " + String(tank_percent) + "%");
      }
    }
  }

  /*
  if (millis() - lastShellyPoll >= SHELLY_POLL_INTERVAL) {
    pollShellyStatuses();
    lastShellyPoll = millis();
  }
  */

  // Publish sensor MQTT
  if (bmeAvailable) {
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float svp = 0.6108f * exp((17.27f * t) / (t + 237.3f));
    float vpd = svp - (h / 100.0f) * svp;
    char buf[32];
    if (isnan(lastTemperature) || fabs(t - lastTemperature) >= 0.3) {
      snprintf(buf, sizeof(buf), "%.1f", t);
      if (server.hasArg("mqtt")) mqttClient.publish("growtent/temperature", buf);
      lastTemperature = t;
    }
    if (isnan(lastHumidity) || fabs(h - lastHumidity) >= 0.5) {
      snprintf(buf, sizeof(buf), "%.1f", h);
      if (server.hasArg("mqtt")) mqttClient.publish("growtent/humidity", buf);
      lastHumidity = h;
    }
    if (isnan(lastVPD) || fabs(vpd - lastVPD) >= 0.03) {
      snprintf(buf, sizeof(buf), "%.2f", vpd);
      if (server.hasArg("mqtt")) mqttClient.publish("growtent/vpd", buf);
      lastVPD = vpd;
    }
  }

  //upper fans automation
  struct tm schedTime;
  if (getLocalTime(&schedTime)) {
    int m = schedTime.tm_min;
    if (m != lastScheduleMinute) {
      // Pushover if enabled
      pushover = prefs.getBool("pushover", false);
      // Load Pushover credentials or defaults
      pushoverToken = prefs.getString("pushover_token", "");
      pushoverUserKey = prefs.getString("pushover_user", "");
      // Left fans ON at :05, OFF at :25
      if (m >= 5 && m <= 25) {
        if (digitalRead(relayPins[0]) != HIGH) {
          digitalWrite(relayPins[0], HIGH);
          if (debug) Serial.println("relay1 set to HIGH");
        }  // verify
      }
      if (m > 25 || m < 5) {
        if (digitalRead(relayPins[0]) != LOW) {
          digitalWrite(relayPins[0], LOW);
          if (debug) Serial.println("relay1 set to LOW");
        }  // verify
      }
      // Right fans ON at :30, OFF at :55
      if (m >= 30 && m <= 55) {
        if (digitalRead(relayPins[1]) != HIGH) {
          digitalWrite(relayPins[1], HIGH);
          if (debug) Serial.println("relay2 set to HIGH");
        }
      }
      if (m > 55 || m < 30) {
        if (digitalRead(relayPins[1]) != LOW) {
          digitalWrite(relayPins[1], LOW);
          if (debug) Serial.println("relay1 set to LOW");
        }
      }
      lastScheduleMinute = m;
    }
  }
}
//END loop

bool mqttConnect() {
  if (mqtt) {
    if (mqttClient.connect("ESP32Client", mqtt_Broker.c_str(), mqtt_Port.c_str())) {
      if (debug) Serial.println("MQTT connected");
      return true;
      if (debug) Serial.print("MQTT connect failed, rc=");
      if (debug) Serial.println(mqttClient.state());
      delay(5000);
      return false;
    }
  } else {
    if (debug) Serial.print("MQTT disabled.");
  }
}

// Poll each Shelly for its state
void pollShellyStatuses() {
  HTTPClient http;
  DynamicJsonDocument doc(256);
  for (int i = 0; i < NUM_SHELLY; i++) {
    String url = String("http://") + shellyHosts[i] + "/relay/0";
    http.begin(url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      if (deserializeJson(doc, payload) == DeserializationError::Ok && doc.containsKey("ison")) {
        shellyAvailable[i] = true;
        shellyState[i] = doc["ison"];
        if (debug) Serial.print("OK, " + String(shellyNames[i]) + " is ");
        if (debug) Serial.println(shellyState[i] ? "ON" : "OFF");
      } else {
        shellyAvailable[i] = false;
        if (debug) Serial.println("ERROR parsing JSON");
      }
    } else {
      shellyAvailable[i] = false;
      if (debug) Serial.print(String(shellyNames[i]) + ": HTTP ");
      if (debug) Serial.print(code);
      if (debug) Serial.println(" failed");
      lastAutoMessage = String(shellyNames[i]) + " not reachable!";
    }
    http.end();
    //if the status of the humidifier is checked and it is off, then turn off the humidifier fan
    if ( i == 0 ) {
      if ( String(shellyState[i] ? "ON" : "OFF") == "OFF") {
        digitalWrite(relayPins[3], LOW);
        if (debug) Serial.println(String(relayNames[3]) + "relay OFF");
      }
    }
  }
}

void handleStatus() {
  String html;

  // Read sensor temperatur, humidity and vpd
  float temperature = 0, humidity = 0, currentVPD = 0;
  if (bmeAvailable) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    float svp = 0.6108f * exp((17.27f * temperature) / (temperature + 237.3f));
    currentVPD = svp - (humidity / 100.0f) * svp;
  }
  
  // Load current phase, VPD targets from prefs
  int curPhase = prefs.getUInt("phase", 1);
  float vpdTargets[4];
  for (int i = 1; i <= 3; i++) {
    char key[16];
    snprintf(key, sizeof(key), "vpd_%d", i);
    vpdTargets[i] = prefs.getFloat(key, defaultVPDs[i]);
  }

  // Load current tempTarget from prefs
  float setTemp = prefs.getFloat("set_temp", defaultSetTemp);

  // Sensor readings
  if (!bmeAvailable) html += "<p style='color:red;'>BME280 Sensor not connected</p>";
  else {
    html += FPSTR(HTML_BME280);

    html.replace("%CTEMP%", String(temperature, 1));
    html.replace("%CVPD%", String(currentVPD, 2));
    html.replace("%HUMIDITY%", String(humidity, 1));
    html.replace("%TTEMP%", String(setTemp, 1));
    html.replace("%TVPD%", String(vpdTargets[curPhase], 2));
  }
  
  if (tank) {
    if (!hcsr04Available) html += "<div class='tile'><font color='red'>HC-SR04 Sensor<br>not connected</div></div><font color='with'>";
    else {
      html += FPSTR(HTML_HCSR04);

      html.replace("%WATERLEVEL%", String(tank_percent) + "% - " +  String(distanceCM) + "cm");
    }
  } else html += "</div>";

  // Relays
  html += "<h3>Relays</h3>";
  html += "<div class='grid-4x1'>";
  for (int i = 0; i < NUM_RELAYS; i++) {
    bool st = digitalRead(relayPins[i]);
    html += String("<div class='tile'>") + relayNames[i] + ":<br><span class='status'>" + (st ? "ON" : "OFF") + "</span><br>"
            + "<button onclick=\"fetch('/relay?id=" + String(i + 1) + "&ajax=1')"
            + ".then(r=>r.json()).then(j=>document.getElementById('relay-status-"
            + String(i + 1) + "').textContent=j.state)\">Toggle</button></div>";
  }
  html += "</div>";

  // Shelly devices
  html += "<h3>Shellys</h3>";
  html += "<div class='grid-5x1'>";
  for (int i = 0; i < NUM_SHELLY; i++) {
    String s = shellyAvailable[i] ? (shellyState[i] ? "ON" : "OFF") : "?";
    html += String("<div class='tile'>") + shellyNames[i] + ": <span class='status'>" + s + "</span><br>"
            + "<button onclick=\"fetch('/shelly?id=" + String(i + 1) + "&state=on&ajax=1').then(r=>r.json()).then(j=>document.getElementById('shelly-status-" + String(i + 1) + "').textContent=j.state)\">ON</button>";
    html += String("<button onclick=\"fetch('/shelly?id=") + String(i + 1) + "&state=off&ajax=1').then(r=>r.json()).then(j=>document.getElementById('shelly-status-" + String(i + 1) + "').textContent=j.state)\">OFF</button><br>" + String(shellyHosts[i]) + "</div>";
  }
  html += "</div>";

  server.send(200, "text/html", html);
}


// Serve main page
void handleRoot() {
  // Fetch local time
  struct tm now;
  String timeString;
  String dateString;
  if (getLocalTime(&now)) {
    char buf[32];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", &now);
    timeString = buf;
    strftime(buf, sizeof(buf), "%d.%m.%Y", &now);
    dateString = buf;
  } else {
    timeString = "Time N/A";
  }

  // Load start date from prefs
  String startDate = prefs.getString("start_date", "");
  struct tm tmStart = { 0 };
  int y, m, d;
  sscanf(startDate.c_str(), "%d-%d-%d", &y, &m, &d);
  tmStart.tm_mday = d;
  tmStart.tm_mon = m - 1;
  tmStart.tm_year = y - 1900;
  time_t startEpoch = mktime(&tmStart);
  time_t nowEpoch = mktime(&now);
  long diffSec = nowEpoch - startEpoch;
  int diffDays = diffSec / 86400;
  int diffWeeks = (diffDays / 7) + 1;

  // Load current phase, VPD targets from prefs
  int curPhase = prefs.getUInt("phase", 1);
  float vpdTargets[4];
  for (int i = 1; i <= 3; i++) {
    char key[16];
    snprintf(key, sizeof(key), "vpd_%d", i);
    vpdTargets[i] = prefs.getFloat(key, defaultVPDs[i]);
  }

  // Load current tempTarget from prefs
  float setTemp = prefs.getFloat("set_temp", defaultSetTemp);

  // Read sensor temperatur, humidity and vpd
  float temperature = 0, humidity = 0, currentVPD = 0;
  if (bmeAvailable) {
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    float svp = 0.6108f * exp((17.27f * temperature) / (temperature + 237.3f));
    currentVPD = svp - (humidity / 100.0f) * svp;
  }

  // Load growlight from prefs
  String lightStart = prefs.getString("light_start", DEFAULT_LIGHT_START);
  uint8_t lightHours = prefs.getUInt("light_hours", DEFAULT_LIGHT_HOURS);

  //Decomposing 'HH:MM' into sh (start hour) and sm (start minute)
  int sh = lightStart.substring(0, 2).toInt();
  int sm = lightStart.substring(3, 5).toInt();

  // Calculate end time and limit to 0–23
  int eh = (sh + lightHours) % 24;
  int em = sm;  // Minuten bleiben gleich

  // Format as "HH:MM" (two digits, leading zeros)
  char buf[6];
  sprintf(buf, "%02d:%02d", eh, em);
  lightEnd = buf;

  // Load start time from prefs
  String stored = prefs.getString("start_date", "");
  // Convert "DD.MM.YYYY" → "YYYY-MM-DD" for the date input
  String iso = "";
  if (stored.length() == 10) {
    iso = stored.substring(6, 10) + "-" + stored.substring(3, 5) + "-" + stored.substring(0, 2);
  }
  
  curPhase = prefs.getUInt("phase", 1);
  
  int dayhour = prefs.getUInt("light_hours");
  int nighthour = 24 - dayhour;
  
  if (softAp) {
  
    String html = FPSTR(HTML_WIFI);
    
    html.replace("%WIFISSID%",  String(prefs.getString("wifi_ssid", "")));
    html.replace("%WIFIPWD%",  String(prefs.getString("wifi_pass", "")));
    
    server.send(200, "text/html", html);
  } else {  
    // Build HTML
    String html = FPSTR(HTML_HEADER);
  
    html.replace("%ELAPSEDGROW%", "Grow since " + String(diffDays) + " days ( " + String(diffWeeks) + "th week )");

    html += FPSTR(HTML_JS);
    html += FPSTR(HTML_END);
    // Replace placeholder
    html.replace("%CONTROLLERNAME%",  prefs.getString("controller_name", ""));
    html.replace("%CURRPHAES%",  phaseNames[curPhase]);  
    html.replace("%LIGHTCYCLE%", String(dayhour) + "/" + String(nighthour)); 
  
  
    if (curPhase == 3) {
      int daysSinceFlowering;
      int weeksSinceFlowering;

      calculateTimeSince(prefs.getString("flowering_start", ""), daysSinceFlowering, weeksSinceFlowering);
      html.replace("%ELAPSEDFLOWERING%", "Flowering since " + String(daysSinceFlowering) + " days ( " + String(weeksSinceFlowering) + "th week )");

    } else html.replace("%ELAPSEDFLOWERING%", " ");

    if (curPhase == 4) {
      int daysSinceDrying;
      int weeksSinceDrying;

      calculateTimeSince(prefs.getString("drying_start", ""), daysSinceDrying, weeksSinceDrying);
      html.replace("%ELAPSEDDRYING%", "Drying since " + String(daysSinceDrying) + " days ( " + String(weeksSinceDrying) + "th week )");

    } else html.replace("%ELAPSEDDRYING%", " ");

    server.send(200, "text/html", html);
  }
}

// Handler for relay toggles
void handleRelay() {
  if (!server.hasArg("id")) {
    server.send(400);
    return;
  }
  int i = server.arg("id").toInt() - 1;
  if (i < 0 || i >= NUM_RELAYS) {
    server.send(400);
    return;
  }
  digitalWrite(relayPins[i], digitalRead(relayPins[i]) ? LOW : HIGH);
  bool st = digitalRead(relayPins[i]);
  if (server.hasArg("ajax")) {
    server.send(200, "application/json", String("{\"id\":") + (i + 1) + ",\"state\":\"" + (st ? "ON" : "OFF") + "\"}");
  } else {
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

// Handler for Shelly toggles
void handleShelly() {
  if (!server.hasArg("id") || !server.hasArg("state")) {
    server.send(400);
    return;
  }
  int i = server.arg("id").toInt() - 1;
  if (i < 0 || i >= NUM_SHELLY) {
    server.send(400);
    return;
  }
  String cmd = server.arg("state");
  HTTPClient http;
  String url = String("http://") + shellyHosts[i] + "/relay/0?turn=" + cmd;
  if (debug) Serial.println("actioncall: " + url);
  http.begin(url);
  http.GET();
  http.end();
  pollShellyStatuses();
  if (server.hasArg("ajax")) {
    server.send(200, "application/json", String("{\"id\":") + (i + 1) + ",\"state\":\"" + (shellyState[i] ? "ON" : "OFF") + "\"}");
  } else {
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

// Handler for saving phase and VPD targets
void savePreferences() {
  // Save grow start date if provided
  if (server.hasArg("webGrowStart")) {
    prefs.putString("start_date", server.arg("webGrowStart"));  // stores as "YYYY-MM-DD"
  }
  
  // Save flowering start date if provided
  if (server.hasArg("webFloweringStart")) {
    prefs.putString("flowering_start", server.arg("webFloweringStart"));  // stores as "YYYY-MM-DD"
  }

  // Save drying start date if provided
  if (server.hasArg("webDryingStart")) {
    prefs.putString("drying_start", server.arg("webDryingStart"));  // stores as "YYYY-MM-DD"
  }

  // Save growth phase and VPD targets as befor
  if (!server.hasArg("phase")) {
    server.send(400);
    return;
  }
  int ph = server.arg("phase").toInt();
  if (ph < 1 || ph > 4) {
    server.send(400);
    return;
  }
  prefs.putUInt("phase", ph);
  for (int i = 1; i <= 4; i++) {
    char key[16];
    snprintf(key, sizeof(key), "vpd_%d", i);
    if (server.hasArg(key)) prefs.putFloat(key, server.arg(key).toFloat());
  }

  // 3) Save set_temp
  if (server.hasArg("set_temp")) prefs.putFloat("set_temp", server.arg("set_temp").toFloat());
  
  // 3) Save growlight setting
  if (server.hasArg("light_start")) {
    String ls = server.arg("light_start");  // Format "HH:MM"
    prefs.putString("light_start", ls);
  } else {
    prefs.putString("light_start", String(DEFAULT_LIGHT_START));
  }

  if (server.hasArg("light_hours")) {
    uint8_t hours = server.arg("light_hours").toInt();
    if (hours < 10) hours = 10;
    if (hours > 20) hours = 20;
    prefs.putUInt("light_hours", hours);
  } else {
    // fallback to Default
    prefs.putUInt("light_hours", DEFAULT_LIGHT_HOURS);
  }

  // save watering notification intervall
  if (server.hasArg("watering")) prefs.putBool("watering", true);
  else prefs.putBool("watering", false);
  if (server.hasArg("water_interval")) {
    int wi = server.arg("water_interval").toInt();
    if (wi < 1) wi = 1;
    if (wi > 30) wi = 30;
    prefs.putUInt("water_interval", wi);
  } else {
    prefs.putUInt("water_interval", DEFAULT_WATER_INTERVAL);
  }

  // save tank settings
  if (server.hasArg("tank")) prefs.putBool("tank", true);
  else prefs.putBool("tank", false);
  if (server.hasArg("tank_full")) prefs.putUInt("tank_full", server.arg("tank_full").toInt());
  if (server.hasArg("tank_empty")) prefs.putUInt("tank_empty", server.arg("tank_empty").toInt());
  if (server.hasArg("tank_warning")) prefs.putUInt("tank_warning", server.arg("tank_warning").toInt());

  // 4) Save mqtt settings
  if (server.hasArg("mqtt")) prefs.putBool("mqtt", true);
  else prefs.putBool("mqtt", false);
  if (server.hasArg("mqtt_broker")) {
    prefs.putString("mqtt_broker", server.arg("mqtt_broker"));
    pushoverToken = prefs.getString("mqtt_broker", "");
  }
  if (server.hasArg("mqtt_port")) {
    prefs.putString("mqtt_port", server.arg("mqtt_port"));
    pushoverToken = prefs.getString("mqtt_port", "");
  }
  if (server.hasArg("mqtt_user")) {
    prefs.putString("mqtt_user", server.arg("mqtt_user"));
    pushoverUserKey = prefs.getString("mqtt_user", "");
  }
  if (server.hasArg("mqtt_pass")) {
    prefs.putString("mqtt_pass", server.arg("mqtt_pass"));
    pushoverUserKey = prefs.getString("mqtt_pass", "");
  }

  // 4) Save pushover settings
  if (server.hasArg("pushover")) prefs.putBool("pushover", true);
  else prefs.putBool("pushover", false);
  if (server.hasArg("pushover_token")) {
    prefs.putString("pushover_token", server.arg("pushover_token"));
    pushoverToken = prefs.getString("pushover_token", "");
  }
  if (server.hasArg("pushover_user")) {
    prefs.putString("pushover_user", server.arg("pushover_user"));
    pushoverUserKey = prefs.getString("pushover_user", "");
  }

  if (server.hasArg("debug")) prefs.putBool("debug", true);
  else prefs.putBool("debug", false);

  // 6) Redirect back to the main page
  server.sendHeader("Location", "/");
  server.send(303);

  Serial.print("Pref updated.");

  loadPreferences();
}