package com.pastillero.application.mqtt;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONException;
import org.json.JSONObject;

public class MqttHandler implements MqttCallback {

    public static final String NEXT_DOSE_MESSAGE_RECEIVED = "com.pastillero.application.mqtt.NEXT_DOSE_MESSAGE_RECEIVED";
    public static final String ACTUAL_STATUS_MESSAGE_RECEIVED = "com.pastillero.application.mqtt.ACTUAL_STATUS_MESSAGE_RECEIVED";
    public static final String PILL_STATUS_MESSAGE_RECEIVED = "com.pastillero.application.mqtt.PILL_STATUS_MESSAGE_RECEIVED";

    public static final String CONNECTION_LOST = "com.pastillero.application.mqtt.CONNECTION_LOST";


    private MqttClient client;
    private Context context;

    public MqttHandler(Context context) { this.context = context; }

    public void connect(String serverUrl, String clientId, String username, String password, MqttConnectionCallback callback) {
        try {
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setUserName(username);
            options.setPassword(password.toCharArray());

            MemoryPersistence persistance = new MemoryPersistence();
            client = new MqttClient(serverUrl, clientId, persistance);
            client.setCallback(this);
            client.connect(options);

            callback.onSuccess();
        } catch (Exception e) {
            Log.e("MQTT", e.getMessage());
            callback.onFailure(e);
        }
    }

    public void disconnect() {
        try {
            client.disconnect();
        } catch (Exception e) {
            Log.e("MQTT", e.getMessage());
        }
    }

    public void publish(String topic, String message) {
        try {
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            mqttMessage.setQos(2);
            client.publish(topic, mqttMessage);
        } catch (Exception e) {
            Log.e("MQTT", e.getMessage());
        }
    }

    public void subscribe(String topic) {
        try{
            client.subscribe(topic);
            Log.i("MQTT", "Topic subscribed " + topic);
        } catch (Exception e) {
            Log.e("MQTT", e.getMessage());
        }
    }

    @Override
    public void connectionLost(Throwable cause) {
        Log.e("MQTT", cause.getMessage());

        Intent i = new Intent(CONNECTION_LOST);
        context.sendBroadcast(i);
    }

    @Override
    public void messageArrived(String topic, MqttMessage message) {
        String payload = message.toString();

        switch (topic){
            case ConfigMQTT.NEXT_DOSE_TOPIC:
                nextDoseMessageReceived(payload);
                break;
            case ConfigMQTT.ACTUAL_STATUS_TOPIC:
                actualStatusMessageReceived(payload);
                break;
            case ConfigMQTT.PILL_STATUS_TOPIC:
                pillStatusMessageReceived(payload);
                break;
            default:
                Log.e("MQTT", "Topic not found");
        }

    }

    private void nextDoseMessageReceived(String payload) {
        try {
            JSONObject json = new JSONObject(payload);
            JSONObject payloadContext = json.getJSONObject("context");
            String message = payloadContext.getString("message");

            Intent i = new Intent(NEXT_DOSE_MESSAGE_RECEIVED);
            i.putExtra("message", message);
            context.sendBroadcast(i);

            Log.i("MQTT", "Message arrived " + payload );
            Log.i( "MQTT", "Broadcast was sent");


        } catch (JSONException e) {
            Log.e("MQTT", e.getMessage());
        }

    }

    private void actualStatusMessageReceived(String payload) {
        try {
            JSONObject json = new JSONObject(payload);
            int value = json.getInt("value");
            JSONObject payloadContext = json.getJSONObject("context");
            String message = payloadContext.getString("message");

            Intent i = new Intent(ACTUAL_STATUS_MESSAGE_RECEIVED);
            i.putExtra("value", value);
            i.putExtra("message", message);
            context.sendBroadcast(i);

            Log.i("MQTT", "Message arrived " + payload);
            Log.i( "MQTT", "Broadcast was sent");
        } catch (JSONException e) {
            Log.e("MQTT", e.getMessage());
        }
    }

    private void pillStatusMessageReceived(String payload) {

    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {

    }
}
