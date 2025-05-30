#pragma once

#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Queue.h"


const char *mqtt_server = "industrial.api.ubidots.com";
const int mqtt_port = 1883;
const char *mqtt_user = "BBUS-iZeDu5BkjABP2jOTnzoBciVizbcUAK";
const char *mqtt_pass = "";

const char *next_dose_time_topic = "/v1.6/devices/esp32/next_dose_time";
const char *actual_status_topic = "/v1.6/devices/esp32/actual_status";
const char *pill_status_topic = "/v1.6/devices/esp32/pill_status";

const char *set_volume_topic = "/v1.6/devices/esp32/volume"
const char *start_buzzer_topic = "/v1.6/devices/esp32/buzzer"

WiFiClient espClient;
PubSubClient client(espClient);
JsonQueue messagesQueue;

void enqueueMessage()
{
  
}

void mqtt_setup()
{ 
  json_queue_init(&messagesQueue);
  client.setServer(mqtt_server, mqtt_port); 
}

void mqtt_reconnect() 
{

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

void mqtt_publish_message(const char *topic, int value, const char *context = nullptr)
{
  StaticJsonDocument<JSON_DOC_SIZE> doc;
  JsonObject variableObj = doc.createNestedObject("status");

  variableObj["value"] = value;

  JsonObject context = variableObj.createNestedObject("context");

  context["message"] = message;
  
  String buffer;

  serializeJson(doc, buffer);

  client.publish(topic, buffer);
}

void callback(char *topic, byte *payload, unsigned int length)
{
 char message[length + 1];
 memcpy(message, payload, length);
 message[length] = '\0';

 Serial.print("Payload received: ");
 Serial.println(message);

  
 StaticJsonDocument<JSON_DOC_SIZE> doc;
 DeserializationError error = deserializeJson(doc, message);

 if (error) {
   Serial.print("Error parsing json: ");
   Serial.println(error.f_str());
   return;
 }

 json_queue_enqueue(&messagesQueue, &doc);
 
}

