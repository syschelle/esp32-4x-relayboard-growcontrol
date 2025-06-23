//check ever 30 seconds the current vpd with target vpd, if current vpd higer than taget vpd then power on the humidifyer shelly
//after 11 second the shelly for the humidifyer turns automaticly off. Configure that in the Webinterface oft the shelly (auto off).

void taskCheckVpd(void *parameter){
   
  for (;;) {
    static unsigned long lastVPDcheck = 0;
    int curPhase = prefs.getUInt("phase");
    String key = String("vpd_") + curPhase;
    float targetVPD = prefs.getFloat(key.c_str(), defaultVPDs[curPhase]);
    // current VPD already calculated as 'vpd'
    if (lastVPD > (targetVPD)) {
      //turn ON humidifyer fan
      digitalWrite(relayPins[3], HIGH);
      HTTPClient http;
      String url = String("http://" + String(shellyHosts[0]) + "/relay/0?turn=on");
      if (debug) Serial.println( "URL: " + url);
      http.begin(url);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        if (debug) Serial.print("HTTP Response code: ");
        if (debug) Serial.println(httpResponseCode);
        String payload = http.getString();
        if (debug) Serial.println("Response payload: " + payload);
      } else {
        if (debug) Serial.print("Error code: ");
        if (debug) Serial.println( httpResponseCode);
        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          if (debug) Serial.println("HTTP Response code: " + httpResponseCode);
          String payload = http.getString();
          if (debug) Serial.println("Response payload: " + payload);
        }
      }
      http.end();
      if (debug) Serial.println("Humidifier ON (VPD too high)");
    }
    delay(30000); 
  }
}