# esp32-4x-relay-growtent

This is a vibe coding project!

A grow tent automation system using an ESP32 microcontroller, a 4x relay board, BME280 environmental sensor, and optional Shelly smart plugs. This project manages environmental conditions (temperature, humidity, VPD) and device scheduling for indoor gardening setups, with MQTT and Pushover notification support.

---

## Features

- **4-Relay Control:** Independently operate fans and humidifier via relays.
- **BME280 Sensor Integration:** Read temperature, humidity, and calculate VPD.
- **Shelly Smart Plug Support:** Control Shelly Plug S Plus devices for additional automation (e.g., humidifier).
- **Growth Phase Profiles:** Set and persist VPD targets and temperature per plant development phase (Seedling/Clone, Vegetative, Flowering).
- **Scheduling:** Timed fan automation based on NTP time and custom intervals.
- **MQTT Integration:** Publish sensor data to your MQTT broker.
- **Web Interface:** Configure setpoints, phases, relays, and Shelly devices from a browser.
- **Pushover Notifications:** Optional alerts for significant actions (e.g., relay changes, NTP sync).
- **Wi-Fi Configuration:** Connects to your Wi-Fi for remote access and MQTT.

---

## Hardware Required

- ESP32 development board
- 4-channel relay module
- BME280 sensor (I2C)
- (Optional) Shelly Plug S Plus smart plugs
- Power supply, wiring, and target devices (fans, humidifier, etc.)

---

## Getting Started

### 1. Wiring

- Connect the relay module to the ESP32 (default pins: 32, 33, 25, 26).
- Connect the BME280 sensor to the ESP32 I2C bus (default address: 0x76).
- Wire relay outputs to fans, humidifier, etc.

### 2. Flashing

- Clone this repository.
- Open `GrowOS.ino` in the Arduino IDE or PlatformIO.
- Configure your Wi-Fi SSID and password in the source code.
- Flash the code to your ESP32.

### 3. Configuration

- Access the ESP32's web interface (find its IP from your router or serial monitor).
- Set growth phase, VPD targets, temperature setpoint, and relay names.
- Optionally configure MQTT and Pushover credentials.
- Add Shelly plug IPs if using smart plugs.

---

## Web Interface

- **Main Page:** Displays live temperature, humidity, VPD, and relay/shelly statuses.
- **Settings:** Adjust phase, VPD targets, temperature, MQTT, and notification settings.
- **Relay/Shelly Control:** Toggle outputs manually or let automation rules control them.

![image](https://github.com/user-attachments/assets/d3438c79-6b9c-4c23-b756-a7ed23ddbcb2)

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

---

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

- Built with the Arduino ESP32 core, Adafruit BME280 library, and PubSubClient for MQTT.
- Inspired by the needs of home growers.
