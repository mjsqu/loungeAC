/*
Subscribes to MQTT at miltonhub (local)

- publishes on lounge/temperature every 5 seconds
- subscribes to lounge/ac and can receive:
  - ON/OFF/16-26
 */
#include <Arduino.h>
// Wifi/Web
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
// Infrared
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>

// Wifi
#include <WiFiClient.h>
// MQTT
#include <PubSubClient.h>
// OneWire
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Wifi variables
const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASS;
const char* mqtt_server = SECRET_MQTT_SERVER;
const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_pass = SECRET_MQTT_PASS;
MDNSResponder mdns;

#undef HOSTNAME
#define HOSTNAME "loungeAC"

const uint16_t kIrLed = 14;  // ESP GPIO pin to use. Recommended: 4 (D2).

IRMitsubishiAC ac(kIrLed);  // Set the GPIO used for sending messages.

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String strPayload = (char*)payload;
  int intPayload = strPayload.toInt();
  /* Valid values: ON,OFF,L,16,17,18,19,20,21,22,23,24,25,26,H */
  if (intPayload >= 16 and intPayload <= 26) {
    setLoungeTemp(intPayload);
  }
  // Add cases
  else {
    Serial.print("Pushing AC Button:");
    Serial.println(strPayload);
    pushACButton((char*)payload);
  }

}

void setLoungeTemp(int temp) {
  Serial.println("Setting desired state for A/C.");
  ac.on();
  ac.setFan(0);
  ac.setMode(kMitsubishiAcHeat);
  ac.setTemp(temp);
  ac.setVane(kMitsubishiAcVaneAuto);
  ac.send();
}

char* getLoungeTemp() {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    char* charTemperature = "00.00";
    dtostrf(temperature, 5, 2, charTemperature);
    return charTemperature;
}

void pushACButton(char* btn) {
  if (strncmp(btn,"ON",2) == 0) {
    ac.on();
    ac.send();
  }
  else if (strncmp(btn,"OFF",3) == 0) {
    ac.off();
    ac.send();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("connected");
        for (int i = 0; i <= 10; i++) {
          digitalWrite(LED_BUILTIN, HIGH); // flash the LED
          delay(100);
          digitalWrite(LED_BUILTIN, LOW); // LED off
      }

      // Once connected, publish an announcement...
      client.publish("lounge/temperature", "init");
      // ... and resubscribe
      client.subscribe("lounge/ac");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED_BUILTIN, LOW);   // Turn it off
  Serial.begin(9600);
  sensors.begin();
  ac.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  char* temprt;
  // Set the value of now
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    // Reset the value of lastMsg to be compared against now in the next iteration
    lastMsg = now;
    temprt = getLoungeTemp();
    Serial.println("temperature in loop:");
    Serial.println(temprt);
    client.publish("lounge/temperature",temprt);
  }
}
