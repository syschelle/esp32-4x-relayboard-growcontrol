// soft ap configurationen
void startSoftAP() {
  Serial.println("Start SoftAP mode...");
  
  // Disconnect previous connections
  WiFi.disconnect(true, true);  
  delay(100);
  
  WiFi.mode(WIFI_AP_STA);

  // starting softAP
  const char* apSSID = "GrowtentAP";
  const char* apPASS = "growtent";
  //last parameter 'false' = do not hide SSID
  bool ok = WiFi.softAP(apSSID, apPASS, /*channel*/ 1, /*hidden*/ false);
  if (!ok) {
    Serial.println("Error starting the SoftAP!");
    return;
  }

  // 4) IP ausgeben
  IPAddress IP = WiFi.softAPIP();
  Serial.print("SoftAP is running IP: ");
  Serial.println(IP);
}

void resetFactory() {
  // open Preferences-Interface (namespace „growtent“)
  prefs.begin("growtent", false);
  // Delete all keys in this namespace
  prefs.clear();
  // close interface
  prefs.end();
  ESP.restart();
}

String convertDate(String dts) {
  // Convert "DD.MM.YYYY" → "YYYY-MM-DD" for the date input
  String iso = "";
  if (dts.length() == 10) {
    iso = dts.substring(6, 10) + "-" + dts.substring(3, 5) + "-" + dts.substring(0, 2);
    return iso;
  }
}

// calculate elapsed days and weeks from defined date
void calculateTimeSince(String startDate, int &days, int &weeks) {
  struct tm tmStart = { 0 };
  int y, m, d;
  sscanf(startDate.c_str(), "%d-%d-%d", &y, &m, &d);
  tmStart.tm_mday = d;
  tmStart.tm_mon = m - 1;
  tmStart.tm_year = y - 1900;
  tmStart.tm_hour = 0;
  tmStart.tm_min = 1;
  time_t startEpoch = mktime(&tmStart);
  time_t nowEpoch = time(nullptr);
  long diffSec = nowEpoch - startEpoch;
  days = diffSec / 86400;
  weeks = (days / 7) + 1;
}

// check HCSR04 sensor
void checkWaterlevel() {
  hcsr04Available = false;
  int tries = 3;
  for (int i = 0; i < tries; i++) {
    delay(100);
    unsigned int uS = sonar.ping();
    if (uS > 0 && uS < US_ROUNDTRIP_CM * HCSR04_MAX_CM) {
      hcsr04Available = true;
      //check tank fill amount
      distanceCM = 0.017 * uS;
      tank_percent = (distanceCM - tank_empty) * 100.0 / (tank_full - tank_empty);
      tank_percent = constrain(tank_percent, 0.0, 100.0);
      Serial.println("→ HC-SR05 distance measurement: " + String(distanceCM) + "cm");
      Serial.println("→ HC-SR05 fill amount: " + String(tank_percent) + "%");
      break;
    }
  }
  Serial.println(hcsr04Available
    ? "✔ HC-SR04 sensor found."
    : "✖ HC-SR04 sensor not found!");
}

void sendPushover(const char* message) {
  HTTPClient http;
  http.begin("https://api.pushover.net/1/messages.json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String post = "token=" + pushoverToken + "&user=" + pushoverUserKey + "&message=" + message;
  http.POST(post);
  http.end();
}