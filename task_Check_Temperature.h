

void taskCheckTemperature(void *parameter){
  for (;;) {
  float setTemp = prefs.getFloat("set_temp", defaultSetTemp);
  if ( (lastTemperature) <  (setTemp - 0.5) ) {
    //turn ON humidifyer fan
      HTTPClient http;
      String url = String("http://" + String(shellyHosts[1]) + "/relay/0?turn=on");
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
      if (debug) Serial.println("Heater ON (Temperature to low)");
  } else {
    if (debug) Serial.println("Heater has noting to do (Current temperature: " + String(lastTemperature) + "°C vs. target temperature: " + String(setTemp) +"°C)");
  }

  delay(60000);
  }
}