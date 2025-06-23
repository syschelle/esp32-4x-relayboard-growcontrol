# esp32-4x-relay-growtent

This is a vibe coding project!

A grow tent automation system using an ESP32 microcontroller, a 4x relay board, BME280 environmental sensor, and optional Shelly smart plugs. This project manages environmental conditions (temperature, humidity, and VPD), device scheduling, and automation for indoor gardening setups, with MQTT and Pushover notification support.

---

## Features

- Controls up to 4 relays (fans, humidifier, etc.) via ESP32 GPIOs
- Integrates with up to 5 Shelly Plug S Plus smart plugs (e.g. for humidifier, heater, lights, fans)
- Reads temperature, humidity, and calculates VPD using a BME280 sensor (I2C)
- Growth phase management: persists and recalls VPD targets and temperature setpoints for Seedling/Clone, Vegetative, and Flowering phases
- Schedules relays (fans/lights) based on precise NTP time and custom intervals
- MQTT support: publishes sensor data (temperature, humidity, VPD) to a broker and allows for remote automation
- Web interface for real-time monitoring (sensor data, relay and Shelly states), configuration (setpoints, growth phase, VPDs, relays, Shelly devices, Wi-Fi, MQTT, notifications)
- Pushover notification support for status and critical events (relay changes, NTP sync, etc.)
- Wi-Fi configuration and automatic reconnection support
- Persistent storage of settings (phase, VPD targets, temperature, MQTT/Pushover settings, debug level, growlight schedule)
- Automated growlight scheduling (start/end time, duration)
- Daily NTP resync for reliable scheduling
- Debug logging toggle via web interface
- REST API endpoints for relay/shelly toggling and real-time JSON sensor updates

---

## Hardware Required

### ESP32 4-channel relay module<br /><br />
  ![image](https://github.com/user-attachments/assets/a5d5d21a-6a74-4c8f-a11c-5d51f332e5ea)

### BME280 sensor (I2C)<br />
  ![BME280](https://github.com/user-attachments/assets/a87e921b-b051-4730-a849-2845959ca554)
    
### (Optional) HC-SR04 ultra sound sensor for Blumat® and Autopod®-Tank watering system.<br />
 
### Shelly Plug S Plus smart plugs
### Power supply
### wiring, and target devices (fans, humidifier, etc.)

---

## Getting Started

### 1. Wiring

- The relay module is connected to the ESP32 (default pins: 32, 33, 25, 26).
- Connect the BME280 sensor to the ESP32 I2C bus (default address: 0x76)(VCC = GPIO 3V3, GND = GND, SCL = GPIO 22, SDA = GPIO 21).
- (Optional) Connect the HC-SR04 (VCC = 5V, GND = GND, TRIG = GPIO 16, ECHO = GPIO 17.
- Wire relay outputs to fans, humidifier, etc.

### 2. Flashing

- Clone this repository.
- Open `GrowOS.ino` in the Arduino IDE or PlatformIO.
- **Install libraries:**
   - Arduino.h
   - Wire.h
   - WiFi.h
   - WebServer.h
   - Adafruit_BME280.h
   - HTTPClient.h
   - esp_http_client.h
   - ArduinoJson.h
   - Preferences.h
   - math.h
   - PubSubClient.h
   - NewPing.h
- Configure your Wi-Fi SSID and password in the source code at the variables.h.
- Flash the code to your ESP32.

### 3. Configuration

- Access the ESP32's web interface (find its IP from your router or serial monitor).
- Set growth phase, VPD targets, temperature setpoint, and relay names.
- Optionally configure MQTT and Pushover credentials.
- Add Shelly plug IPs if using smart plugs.

---

## Web Interface

- **Status Page:** Displays live temperature, humidity, VPD, and relay/shelly statuses. Toggle outputs manually or let automation rules control them.
![image](https://github.com/user-attachments/assets/213abe61-5c79-4e0a-91bf-66adcabba45d)

- **Settings Page:** Adjust controller name, time zone, one timeserver, phase, VPD targets, temperature, light setting, max min oft the HC-SR04 with warning level,  MQTT, and pushover notification settings.
![image](https://github.com/user-attachments/assets/9bbde8e6-ed5b-43c0-ae9a-ee5b15f89dc6)

- **Grow Diary Page:** Keep your little grow diary.
![image](https://github.com/user-attachments/assets/243c0d8a-2613-4154-b225-d6ce6f2e3d94)

- **Grow Guideline Page:** This guide is just a suggestion!
![image](https://github.com/user-attachments/assets/be412bb5-2e7f-4a43-a3b5-37d684bb6a2d)

- **Manual Page:** Shows the README.md of this poject on github.


---

## Automation Overview

- Fans and humidifier are scheduled based on time and environmental readings.
- Relay/fan operation adjusts automatically according to current and target VPD.
- Shelly plugs can be controlled for devices like humidifiers.
- NTP sync ensures accurate scheduling.

---

## Example MQTT Topics

- `growtent/temperature` – Current tent temperature
- `growtent/humidity` – Current humidity
- `growtent/vpd` – Current vapor pressure deficit

## Advanced Features

- **Growth Phase Tracking:** Store and recall phase-specific targets.
- **Pushover Alerts:** Get critical notifications on your phone.
- **Secure Access:** Web interface writes and credential storage use ESP32 Preferences.

---

## Contributing

Pull requests and feature suggestions are welcome!

---

## License

[MIT](./LICENSE)

---

## Credits

- This project uses the following libraries:\
  [Arduino core](https://www.arduino.cc/)\
  [Wire](https://www.arduino.cc/en/Reference/Wire)\
  [WiFi](https://www.arduino.cc/en/Reference/WiFi)\
  [WebServer (ESP32)](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer)\
  [Adafruit BME280](https://github.com/adafruit/Adafruit_BME280_Library)\
  [HTTPClient](https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient)\
  [ESP HTTP Client](https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/HTTPClient)\
  [ArduinoJson](https://arduinojson.org/)\
  [Preferences (ESP32)](https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences)\
  [PubSubClient](https://github.com/knolleary/pubsubclient)\
  [NewPing](https://bitbucket.org/teckel12/arduino-new-ping/)\
  [math.h](https://en.cppreference.com/w/c/numeric/math)
  
  Inspired by the needs of meself and chatgpt.com
