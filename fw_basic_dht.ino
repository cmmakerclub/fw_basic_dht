#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <MqttConnector.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "CMMC_Blink.hpp"

#include "ButtonTask.h"
#include <Task.h>

#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"

MqttConnector *mqtt;
CMMC_Blink *blinker;

#define LED 16

/* WIFI INFO */
String WIFI_SSID        = "@ESPertAP_001";
String WIFI_PASSWORD    = "espertap";

String MQTT_HOST        = "mqtt.cmmc.io";
String MQTT_USERNAME    = "";
String MQTT_PASSWORD    = "";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "MARU/";
int    MQTT_PORT        = 1883;
int PUBLISH_EVERY       = 5000;
String IFTTT_STRING     = "http://cmmc.io/";

String DEVICE_NAME      = "DEPA-005";
int relayPin            = 15;
int MQTT_CONNECT_TIMEOUT = 5000;
char myName[40];
int relayPinState;

void init_hardware()
{
  blinker = new CMMC_Blink();
  blinker->init();
  pinMode(12, INPUT_PULLUP);

  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);

  digitalWrite(14, HIGH);
  digitalWrite(15, LOW);

  pinMode(relayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED, HIGH);

  // serial port initialization
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
}

void init_wifi() {
  blinker->blink(50, LED);

  WiFi.disconnect();
  delay(20);
  WiFi.mode(WIFI_STA);
  delay(50);
  const char* ssid =  WIFI_SSID.c_str();
  const char* pass =  WIFI_PASSWORD.c_str();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", ssid, pass);
    delay(300);
  }
  Serial.println("WiFi Connected.");
  digitalWrite(LED_BUILTIN, HIGH);
  blinker->detach();

}

TaskManager taskManager;
ButtonTask buttonAsyncTask(HandleGPIO0Pressed, 0);
PubSubClient *mqttClient;

void HandleGPIO0Pressed(ButtonState state)
{
  // apply on press
  if (state & ButtonState_Pressed) {
    Serial.println("PRESSED");
    digitalWrite(LED, LOW);
    doHttpGet(IFTTT_STRING);
    if (mqttClient) {
      //mqttClient->publish("HELOO", "TEST");
    }
    digitalWrite(LED, HIGH);
  }
  else {

  }
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();

  taskManager.StartTask(&buttonAsyncTask);

}

void loop()
{
  mqtt->loop();
  if (mqttClient == NULL) {
    mqttClient = mqtt->getPubSubClient();
  }

  taskManager.Loop();
}


void doHttpGet(String url) {
  HTTPClient http;
  http.begin(url); //HTTP
  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
  }

  http.end();
}



