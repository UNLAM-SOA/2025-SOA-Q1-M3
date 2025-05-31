#pragma once

#include <PubSubClient.h>
#include <WiFi.h>

const char *mqtt_server = "industrial.api.ubidots.com";
const int mqtt_port = 1883;
const char *mqtt_user = "BBUS-iZeDu5BkjABP2jOTnzoBciVizbcUAK";
const char *mqtt_pass = "";

const char *next_dose_time_topic = "/v1.6/devices/esp32/next_dose_time";
const char *actual_status_topic = "/v1.6/devices/esp32/actual_status";
const char *pill_status_topic = "/v1.6/devices/esp32/pill_status";

WiFiClient espClient;
PubSubClient client(espClient);

void mqtt_setup() { client.setServer(mqtt_server, mqtt_port); }

void mqtt_reconnect() {

  char clientId[50];

  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);

    if (client.connect(clientId, mqtt_user, mqtt_pass)) {
      Serial.print(clientId);
      Serial.println(" conectado");
      client.subscribe(next_dose_time_topic);
      client.subscribe(actual_status_topic);
      client.subscribe(pill_status_topic);
    }
  }
}

void mqtt_publish(const char *topic, const char *payload) {
  client.publish(topic, payload);
}


