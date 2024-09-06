/*
 * Hibiscus Sense - Exercise 01 Strobe Light
 * 
 * There is 1x blue LED.
 * This LED is connected to GPIO2 of ESP32 microcontroller.
 * The circuit is an active-low circuit, therefore we need to pull the pins LOW,
 * in order to complete the circuit to the GND.
 * 
 * Therefore in the sketch,
 * we need to call LOW state in the digitalWrite function, to light ON the LED,
 * while call HIGH state, to light OFF the LED.
 * 
 */

#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "YOUR_WIFI_SSID";
const char password[] = "YOUR_WIFI_PASSWORD";

String deviceId = "YOUR_PHONE_NUMBER";
String prefixTopic = "/Hibiscus-Sense-";
String ledTopic = "/led";
String statusTopic = "/status";

WiFiClient net;
MQTTClient mqtt;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi '" + String(ssid) + "' ...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming Status: " + payload);
  Serial.println();

  if(topic == prefixTopic + deviceId + ledTopic){
    digitalWrite(2, payload.toInt());
  }
  
}

void connectToMQTTBroker() {
  Serial.print("Connecting to MQTT broker ...");

  mqtt.begin("broker.hivemq.com", net);
  mqtt.onMessage(messageReceived);

  String uniqueString = String(ssid) + "-" + String(random(1, 98)) + String(random(99, 999));
  char uniqueClientID[uniqueString.length() + 1];

  uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);

  while (!mqtt.connect(uniqueClientID)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(" connected!");
  mqtt.publish(prefixTopic + deviceId + statusTopic, "Connected");

  Serial.println("Subscribe to: " + prefixTopic + deviceId + ledTopic);
  mqtt.subscribe(prefixTopic + deviceId + ledTopic);

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // declaring GPIO2 as an OUTPUT pin.
  pinMode(2, OUTPUT);

  // STEP 1: Initialized Wi-Fi conectivity
  connectToWiFi();
  // STEP 2: Initialized MQTT connection to Favoriot MQTT broker
  connectToMQTTBroker();

}

void loop() {
  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Check MQTT connection
  if (!mqtt.connected()) {
    connectToMQTTBroker();
  }

  mqtt.loop();
  delay(10);  // <- fixes some issues with WiFi stability

}
