// ====== Wi-Fi Configuration ======
const char* WIFI_SSID = "ssid";
const char* WIFI_PASS = "pwd";

// ====== MQTT placeholder variable ======
String mqtt_Broker;  // loaded from prefs
String mqtt_Port;    // loaded from prefs
String mqtt_User;    // loaded from prefs
String mqtt_Pass;    // loaded from prefs

#define NUM_SHELLY 5

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ====== Pushover placeholder variable ======
String pushoverToken;    // loaded from prefs
String pushoverUserKey;  // loaded from prefsControllerName

// ====== Last published sensor values ======
#include <cmath>
float lastTemperature = NAN;
float lastHumidity = NAN;
float lastVPD = NAN;

String ControllerName;  // choose the name of your controller

// ====== Shelly Plug S Plus Configuration ======
#define NUM_SHELLY 5
const char* shellyHosts[NUM_SHELLY] = {"192.168.178.49","192.168.1.101","192.168.178.71","192.168.1.103","192.168.1.104"}; //please set your ip addresses from your shellys
const char* shellyNames[NUM_SHELLY] = {"Humidifier","Heater","Light","Exthaust","Profan"};

// ====== NTP Configuration default europe/Berlin ======
#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"  // Western European Time

// ====== HC-SR04 Configuration ======
#define HCSR04_TRIG_PIN 16
#define HCSR04_ECHO_PIN 17
#define HCSR04_MAX_CM   300  // maximum range in cm
#define US_ROUNDTRIP_CM 29.1
NewPing sonar(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN, HCSR04_MAX_CM);

// ====== Relay Configuration ======
#define NUM_RELAYS 4
const int relayPins[NUM_RELAYS] = { 32, 33, 25, 26 };
// NEU: menschliche Bezeichnungen für die Relais
const char* relayNames[NUM_RELAYS] = {
  "left fans",
  "right fans",
  "pod fans",
  "humidifier fan"
};

// ====== BME280 Sensor Configuration ======
#define BME_ADDR 0x76
Adafruit_BME280 bme;
bool bmeAvailable = false;

// ====== Web Server ======
WebServer server(80);

// ====== Shelly Status Cache ======
bool shellyAvailable[NUM_SHELLY] = { false };
bool shellyState[NUM_SHELLY] = { false };
unsigned long lastShellyPoll = 0;
const unsigned long SHELLY_POLL_INTERVAL = 10000;  // poll every 10 seconds

// ====== initial freferences ======
Preferences prefs;
const char* phaseNames[5] = { "", "Seedling/Clone", "Vegetative", "Flowering", "Harvest"};
int curPhase;
// Default VPD targets per phase
const float defaultVPDs[5] = { 0.0f, 0.8f, 1.2f, 1.4f, 1.0f };
float targetVPD;
// Default targets temperature
const float defaultSetTemp = 22.0f;
// Default mqtt disabled
bool mqtt;
// Default pushover disabled
bool pushover;
// Default debug level disabled
bool debug;
// Default flowering disabled
bool flowering = false;
//date and tme
struct tm now;
// Default growlight setting
char actualDate[10];
String startDate;
String startFlowering;
const char* DEFAULT_LIGHT_START = "03:00";
const uint8_t DEFAULT_LIGHT_HOURS = 18;
char* lightEnd;
// watering reminder Defaults und Status
bool watering;
const uint8_t DEFAULT_WATER_INTERVAL = 3;      // Standard: watering every 3 days
uint8_t waterInterval;                         // program variable
RTC_DATA_ATTR int8_t lastWaterCheckDay = -1;   // Day for daily exam
//growdiary
const char* ns = "growdiary";  // Namespace
const int maxEntries = 10;

//Default water tank setting
float distanceCM;
bool hcsr04Available;
#define US_ROUNDTRIP_CM 29.1
bool tank;
int tank_full;
int tank_empty;
int tank_warning;
long tank_percent;

// ====== Scheduling =====
int lastScheduleMinute = -1;
RTC_DATA_ATTR int lastSyncDay = -1;  // Day of month for last NTP sync

String lastAutoMessage = "";
