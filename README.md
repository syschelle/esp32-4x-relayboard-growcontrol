# esp32-4x-relay-growtent

This is a vibe coding project!

A grow tent automation system using an ESP32 microcontroller, a 4x relay board, BME280 environmental sensor, and optional Shelly smart plugs. This project manages environmental conditions (temperature, humidity, VPD) and device scheduling for indoor gardening setups, with MQTT and Pushover notification support.

---

## Features

- 4-Controls up to 4 relays (fans, humidifier fan, etc.) via ESP32 GPIOs
- 4-Integrates with up to 5 Shelly Plug S Plus smart plugs (e.g. for humidifier, heater, lights, fans)
- 4-Reads temperature, humidity, and calculates VPD using a BME280 sensor (I2C)
- 4-Growth phase management: persists and recalls VPD targets and temperature setpoints for Seedling/Clone, Vegetative, and Flowering phases
- 4-Schedules relays (fans) based on precise NTP time and custom intervals
- 4-MQTT support: publishes sensor data (temperature, humidity, VPD) to a broker and allows for remote automation
- 4-Web interface for real-time monitoring (sensor data, relay and Shelly states), configuration of setpoints, growth phase, VPDs, relays, Shelly devices, Wi-Fi, MQTT, and notifications
- 4-Pushover notification support for status and critical events (relay changes, NTP sync, etc.)
- 4-Wi-Fi configuration and automatic reconnection support
- 4-Persistent storage of settings (phase, VPD targets, temperature, MQTT/Pushover settings, debug level, growlight schedule)
- 4-Automated growlight scheduling (start/end time, duration)
- 4-Daily NTP resync for reliable scheduling
- 4-Debug logging toggle via web interface
- 4-REST API endpoints for relay/shelly toggling and real-time JSON sensor updates

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
![image](https://github.com/user-attachments/assets/5641712c-899a-4e42-a10f-d25807bc7b32)

![image](https://github.com/user-attachments/assets/c7495449-d022-43d6-a074-bbe2eba1f06d)


- **Main Page:** Displays live temperature, humidity, VPD, and relay/shelly statuses.
- **Settings:** Adjust phase, VPD targets, temperature, MQTT, and notification settings.
- **Relay/Shelly Control:** Toggle outputs manually or let automation rules control them.


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

- Built with the Arduino ESP32 core, Adafruit BME280 library, and PubSubClient for MQTT.
- Inspired by the needs of home growers.
- chatgpt.com
