package com.pastillero.application;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;


import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.pastillero.application.mqtt.ConfigMQTT;
import com.pastillero.application.mqtt.MqttHandler;
import com.pastillero.application.mqtt.MqttService;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;


public class PillsStatusActivity extends AppCompatActivity implements SensorEventListener {
    private LocalBroadcastManager broadcastManager;
    public IntentFilter filterReceivePillsStatus;

    private PillScanReceiver pillScanReceiver = new PillScanReceiver();
    private SensorManager sensorManager;
    private Sensor accelerometer;

    private static final float SHAKE_THRESHOLD = 10.0f; // if extra acceleration is greater than this, it is a shake
    private static final int SHAKE_TIME_THRESHOLD = 500; // ms
    private long lastShakeTime = 0;

    private Button scanPillsButton;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pills_status);

        scanPillsButton = findViewById(R.id.scan_pills_button);

        scanPillsButton.setOnClickListener(v -> {
            try{
                JSONObject message = new JSONObject();
                JSONObject context = new JSONObject();
                context.put("type", "scan");
                message.put("value", 0);
                message.put("context", context);
                publishMessage(message.toString());
            } catch (JSONException e) {
                Log.e("Pastillero", e.getMessage());
            }

        });

        broadcastManager = LocalBroadcastManager.getInstance(this);
        configBroadcastReceivers();

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
                }
            }
        }
    }

    private void publishMessage(String message) {
        Intent intent = new Intent(this, MqttService.class);
        intent.setAction(MqttService.ACTION_PUBLISH);
        intent.putExtra(MqttService.EXTRA_TOPIC, ConfigMQTT.PILL_STATUS_TOPIC);
        intent.putExtra(MqttService.EXTRA_MESSAGE, message);
        startService(intent);
    }

    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    private void configBroadcastReceivers() {
        filterReceivePillsStatus = new IntentFilter(MqttHandler.PILL_SCAN_MESSAGE_RECEIVED);

        broadcastManager.registerReceiver(pillScanReceiver, filterReceivePillsStatus);
        Log.i("Pastillero", "Registered receivers");
    }

    public class PillScanReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String message = intent.getStringExtra("message");

            // TODO: mostrar en pantalla los dias y sus pills

            try{
                JSONArray pillsArray = new JSONArray(message);
                int pillsPerDay = 3;
                int days = 7;

                for (int day = 0; day < days; day++) {
                    for (int pill = 0; pill < pillsPerDay; pill++) {
                        // Using getIdentifier to BUILD it depending on day and period
                        int resId = getResources().getIdentifier(
                                "pill" + day + "_" + pill, "id", getPackageName());
                        TextView pillView = findViewById(resId);
                        int status = pillsArray.getInt(day * pillsPerDay + pill);
                        pillView.setText(status == 1 ? "🟩" : "🟥");
                    }
                }

            }catch (JSONException e) {
                Log.e("Pastillero", e.getMessage());
            }

        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}