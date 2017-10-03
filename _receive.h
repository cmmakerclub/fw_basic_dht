#include <Arduino.h>
#include <MqttConnector.h>

extern MqttConnector* mqtt;

extern String MQTT_CLIENT_ID;
extern String MQTT_PREFIX;

extern int relayPin;
extern int relayPinState;
extern char myName[];

extern String IFTTT_STRING;


void register_receive_hooks() {
  mqtt->on_subscribe([&](MQTT::Subscribe * sub) -> void {
    Serial.printf("myName = %s \r\n", myName);
    sub->add_topic(MQTT_PREFIX + myName + "/$/+");
    sub->add_topic(MQTT_PREFIX + MQTT_CLIENT_ID + "/$/+");
    sub->add_topic(MQTT_PREFIX + myName + "/$/command/ifttt");
    sub->add_topic(MQTT_PREFIX + MQTT_CLIENT_ID + "/$/command/ifttt");
  });

  mqtt->on_before_message_arrived_once([&](void) { });

  mqtt->on_message([&](const MQTT::Publish & pub) { });

  mqtt->on_after_message_arrived([&](String topic, String cmd, String payload) {
    Serial.println(topic);
    Serial.println(cmd);
    Serial.println(payload);
    Serial.printf("topic: %s\r\n", topic.c_str());
    Serial.printf("cmd: %s\r\n", cmd.c_str());
    Serial.printf("payload: %s\r\n", payload.c_str());

    if (cmd == "$/command/ifttt") {
      Serial.println("Remembered ifttt string");
      IFTTT_STRING = payload;
    }
    else if (cmd == "$/command") {
      if (payload == "ON") {
        Serial.println("turn led ON.");
        digitalWrite(relayPin, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
        relayPinState = 1;
      }
      else if (payload == "OFF") {
        Serial.println("turn led OFF.");
        digitalWrite(relayPin, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        relayPinState = 0;
      }
    }
    else if (cmd == "$/reboot") {
      ESP.reset();
    }
    else {
      // another message.
    }
  });
}
