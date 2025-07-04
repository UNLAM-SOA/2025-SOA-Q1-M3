#pragma once

#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Queue.h"

#define NO_VALUE 0
#define AWAITING 1
#define TIME_TO_TAKE_PILL 2
#define NO_PILLS 0
#define PILLS_DETECTED 1
#define MAX_MSG_AGE 10 // seconds

const char *mqtt_server = "industrial.api.ubidots.com";
const int mqtt_port = 1883;
const char *mqtt_user = "BBUS-iZeDu5BkjABP2jOTnzoBciVizbcUAK";
const char *mqtt_pass = "";

const char *next_dose_time_topic = "/v1.6/devices/esp32/next_dose_time";
const char *actual_status_topic = "/v1.6/devices/esp32/actual_status";
const char *pill_status_topic = "/v1.6/devices/esp32/pill_status";
const char *pill_scan_topic = "/v1.6/devices/esp32/pill_scan";

const char *set_volume_topic = "/v1.6/devices/esp32/volume";

const char *skip_pill_topic = "/v1.6/devices/esp32/skip";

WiFiClient espClient;
PubSubClient client(espClient);
JsonQueue messagesQueue;

void callback(char *topic, byte *payload, unsigned int length)
{
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';


  StaticJsonDocument<JSON_DOC_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Serial.print("Error parsing json: ");
    Serial.println(error.f_str());
    return;
  }

  if (doc.containsKey("timestamp")) {
    
    int64_t msgTimestamp = doc["timestamp"];
    time_t now = time(nullptr);  

    if (now == -1) {
      Serial.println("RTC time not available, no timestamp filtering");
    } else {
      int64_t diff = now - (msgTimestamp/1000);
      if (diff > MAX_MSG_AGE) {
        return;
      }
    }
  } else {
    return;
  }

  json_queue_enqueue(&messagesQueue, doc);
}

void mqtt_setup()
{
  json_queue_init(&messagesQueue);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void mqtt_reconnect()
{
  char clientId[50] = {0};

  while (!client.connected())
  {
    Serial.print("Intentando conexión MQTT...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);

    if (client.connect(clientId, mqtt_user, mqtt_pass))
    {
      Serial.print(clientId);
      Serial.println(" conectado");
      client.subscribe(pill_status_topic);
      client.subscribe(set_volume_topic);
      client.subscribe(skip_pill_topic);
    }
  }
}

void mqtt_publish_message(const char *topic, int value, const char *context = nullptr)
{
  StaticJsonDocument<JSON_DOC_SIZE> doc;

  doc["value"] = value;

  JsonObject contextObj = doc.createNestedObject("context");

  contextObj["message"] = context;

  String buffer;

  serializeJson(doc, buffer);

  Serial.println("Sending message: " + buffer);

  bool published = client.publish(topic, (const uint8_t*) buffer.c_str(), buffer.length(), false);
  Serial.print("SEND STATE:  ");
  Serial.println(published);
}
