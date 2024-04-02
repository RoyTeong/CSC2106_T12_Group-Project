#include <M5StickCPlus.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>

// WiFi Credentials
const char* ssid = "asus_wifi";
const char* password = "freewifi";

// MQTT Broker Settings
const char* mqttServer = "192.168.50.176";
const int mqttPort = 1883;
const char* mqttTopic = "device/toggle";
const char* mqttUser = "homeassistant";
const char* mqttPassword = "eu0Vabee3iegaeRee4Voo9ozohtorahZeighoosai7soovae2ohcu8quahTi4iej";

// Initialize WiFi and MQTT Client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Initialize HardwareSerial
HardwareSerial MySerial(1); // Using UART 1

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  MySerial.begin(115200, SERIAL_8N1, -1, 26); // RX not used, TX on GPIO 26
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("WIFI Set up done");
  
  // Setup MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);
  reconnectMQTT(); // Connect and subscribe to the MQTT broker
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0'; // Ensure null-terminated string

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(message);
  
  MySerial.println(message); // Send the message over UART
}

void reconnectMQTT() {
  // Loop until we're reconnected to the MQTT broker
  while (!mqttClient.connected()) {
    if (mqttClient.connect("M5StickC_Transmitter", mqttUser, mqttPassword)) {
      mqttClient.subscribe(mqttTopic);
    }
    delay(5000); // Wait before retrying if connection fails
  }
}
