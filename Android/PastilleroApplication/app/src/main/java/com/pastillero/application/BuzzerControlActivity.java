package com.pastillero.application;

import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;


import androidx.appcompat.app.AppCompatActivity;

import com.pastillero.application.mqtt.ConfigMQTT;
import com.pastillero.application.mqtt.MqttService;

import org.json.JSONException;
import org.json.JSONObject;


public class BuzzerControlActivity extends AppCompatActivity implements SensorEventListener {
    private SensorManager sensorManager;
    private Sensor accelerometer;

    private static final float SHAKE_THRESHOLD = 10.0f; // if extra acceleration is greater than this, it is a shake
    private static final int SHAKE_TIME_THRESHOLD = 500; // ms
    private long lastShakeTime = 0;

    private boolean isBuzzerActive = false;

    private TextView statusText;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_buzzer_control);

        statusText = findViewById(R.id.status_text);

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
    }

    @Override
    protected void onResume(){
        super.onResume();

        sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void onPause(){
        super.onPause();

        sensorManager.unregisterListener(this);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
            float xAcceleration = event.values[0];
            float yAcceleration = event.values[1];
            float zAcceleration = event.values[2];

            // Extra acceleration after removing gravity
            float netAcceleration = (float) Math.sqrt(xAcceleration * xAcceleration + yAcceleration * yAcceleration + zAcceleration * zAcceleration) - SensorManager.GRAVITY_EARTH;

            long currentTime = System.currentTimeMillis();

            if(netAcceleration > SHAKE_THRESHOLD) {
                if(currentTime - lastShakeTime > SHAKE_TIME_THRESHOLD) {
                    lastShakeTime = currentTime;
                    toggleBuzzer();
                }
            }
        }
    }

    private void publishMessage(String message) {
        Intent intent = new Intent(this, MqttService.class);
        intent.setAction(MqttService.ACTION_PUBLISH);
        intent.putExtra(MqttService.EXTRA_TOPIC, ConfigMQTT.BUZZER_TOPIC);
        intent.putExtra(MqttService.EXTRA_MESSAGE, message);
        startService(intent);
    }


    private void toggleBuzzer(){
        try {
            JSONObject message = new JSONObject();
            JSONObject actualValues = new JSONObject();
            actualValues.put("value", isBuzzerActive ? 0 : 1);
            message.put("buzzer", actualValues);
            publishMessage(actualValues.toString());
            isBuzzerActive = !isBuzzerActive;
            updateStatus();
        } catch (JSONException e) {
            Toast.makeText(this, "Error sending message", Toast.LENGTH_SHORT).show();
        }
    }

    private void updateStatus(){
        String status = isBuzzerActive ? "Buzzer ON - Shake to turn off" : "Buzzer OFF - Shake to turn on";
        statusText.setText(status);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}