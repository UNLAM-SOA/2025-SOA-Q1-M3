package com.pastillero.application;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import com.pastillero.application.mqtt.ConfigMQTT;
import com.pastillero.application.mqtt.MqttConnectionCallback;
import com.pastillero.application.mqtt.MqttHandler;
import com.pastillero.application.mqtt.MqttService;

import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private MqttHandler mqttHandler;
    private LocalBroadcastManager broadcastManager;

    private TextView nextDoseTimeText;
    private TextView actualStatusText;

    public IntentFilter filterReceiveNextDoseMessage;
    public IntentFilter filterReceiveActualStatusMessage;
    public IntentFilter filterReceivePillStatusMessage;

    public IntentFilter filterConnectionLost;

    private final NextDoseReceiver nextDoseReceiver = new NextDoseReceiver();
    private final ActualStatusReceiver actualStatusReceiver = new ActualStatusReceiver();
    private final PillStatusReceiver pillStatusReceiver = new PillStatusReceiver();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        nextDoseTimeText = findViewById(R.id.next_dose_time);
        actualStatusText = findViewById(R.id.actual_status);

        broadcastManager = LocalBroadcastManager.getInstance(this);

        configBroadcastReceivers();

        startMqttService();

        Log.i("MQTT", "Service started running");

    }

    private void startMqttService() {
        Intent serviceIntent = new Intent(this, MqttService.class);
        startForegroundService(serviceIntent);

    }

    @Override
    protected void onDestroy(){
        broadcastManager.unregisterReceiver(nextDoseReceiver);
        broadcastManager.unregisterReceiver(actualStatusReceiver);
        broadcastManager.unregisterReceiver(pillStatusReceiver);
        super.onDestroy();
    }


    /* The supress annotation is due to registerReceiver needs to know if the receiver listen to
    messages from other apps or only from your app */
    //TODO: do ContextCompat.registerReceiver works?
    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    private void configBroadcastReceivers() {
        filterReceiveNextDoseMessage = new IntentFilter(MqttHandler.NEXT_DOSE_MESSAGE_RECEIVED);
        filterReceiveActualStatusMessage = new IntentFilter(MqttHandler.ACTUAL_STATUS_MESSAGE_RECEIVED);
        filterReceivePillStatusMessage = new IntentFilter(MqttHandler.PILL_STATUS_MESSAGE_RECEIVED);


        broadcastManager.registerReceiver(nextDoseReceiver, filterReceiveNextDoseMessage);
        broadcastManager.registerReceiver(actualStatusReceiver, filterReceiveActualStatusMessage);
        broadcastManager.registerReceiver(pillStatusReceiver, filterReceivePillStatusMessage);
        Log.i("Pastillero", "Registered receivers");
    }

    @Override
    protected void onResume(){
        super.onResume();
        loadLastKnownState();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public class NextDoseReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String message = intent.getStringExtra("message");
            nextDoseTimeText.setText(message);

            Log.i("Pastillero", "Received next dose message: " + message);

        }
    }

    public class ActualStatusReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String message = intent.getStringExtra("message");
            int value = intent.getIntExtra("value", 1);

            Log.i("Pastillero", "Received actual status message: " + message + " with value: " + value);

            actualStatusText.setText(message);

            if(value == 0) {
                actualStatusText.setBackgroundColor(Color.GREEN);
            } else {
                actualStatusText.setBackgroundColor(Color.WHITE);
            }
        }
    }

    public class PillStatusReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {

        }
    }

    private void loadLastKnownState(){
        android.content.SharedPreferences prefs = getSharedPreferences("PastilleroPrefs", Context.MODE_PRIVATE);

        String lastNextDoseMessage = prefs.getString("last_next_dose_message", null);
        if (lastNextDoseMessage != null) {
            nextDoseTimeText.setText(lastNextDoseMessage);
        }

        String lastStatusMessage = prefs.getString("last_status_message", null);
        int lastStatusValue = prefs.getInt("last_status_value", -1);

        if (lastStatusMessage != null) {
            actualStatusText.setText(lastStatusMessage);
            if (lastStatusValue == 0) {
                actualStatusText.setBackgroundColor(Color.GREEN);
            } else {
                actualStatusText.setBackgroundColor(Color.WHITE);
            }
        }
    }
}