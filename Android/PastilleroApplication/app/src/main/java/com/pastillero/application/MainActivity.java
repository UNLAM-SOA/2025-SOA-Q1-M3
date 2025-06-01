package com.pastillero.application;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;


import androidx.appcompat.app.AppCompatActivity;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


import com.pastillero.application.mqtt.MqttHandler;
import com.pastillero.application.mqtt.MqttService;



public class MainActivity extends AppCompatActivity {

    private LocalBroadcastManager broadcastManager;

    private TextView nextDoseTimeText;
    private TextView actualStatusText;

    private EditText volumeInput;

    private Button setVolumeButton;

    private Button activateBuzzerButton;
    private Button deactivateBuzzerButton;

    private Button goToBuzzerControlButton;
    public IntentFilter filterReceiveNextDoseMessage;
    public IntentFilter filterReceiveActualStatusMessage;
    public IntentFilter filterReceivePillStatusMessage;

    public IntentFilter filterConnectionLost;

    private final NextDoseReceiver nextDoseReceiver = new NextDoseReceiver();
    private final ActualStatusReceiver actualStatusReceiver = new ActualStatusReceiver();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        nextDoseTimeText = findViewById(R.id.next_dose_time);
        actualStatusText = findViewById(R.id.actual_status);

        volumeInput = findViewById(R.id.volume_input);
        setVolumeButton = findViewById(R.id.set_volume_button);
        activateBuzzerButton = findViewById(R.id.activate_buzzer_button);
        deactivateBuzzerButton = findViewById(R.id.deactivate_buzzer_button);

        goToBuzzerControlButton = findViewById(R.id.buzzer_control_button);

        goToBuzzerControlButton.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, BuzzerControlActivity.class);
            startActivity(intent);
        });

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
        super.onDestroy();
    }


    /* The supress annotation is due to registerReceiver needs to know if the receiver listen to
    messages from other apps or only from your app */
    //TODO: do ContextCompat.registerReceiver works?
    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    private void configBroadcastReceivers() {
        filterReceiveNextDoseMessage = new IntentFilter(MqttHandler.NEXT_DOSE_MESSAGE_RECEIVED);
        filterReceiveActualStatusMessage = new IntentFilter(MqttHandler.ACTUAL_STATUS_MESSAGE_RECEIVED);


        broadcastManager.registerReceiver(nextDoseReceiver, filterReceiveNextDoseMessage);
        broadcastManager.registerReceiver(actualStatusReceiver, filterReceiveActualStatusMessage);
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
                actualStatusText.setTextColor(Color.RED);
            } else {
                actualStatusText.setTextColor(Color.rgb(59,59,26));
            }
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

    // It creates an Intent to trigger the onStartCommand on the MQTT service
    // so it publish the message using the service
    // TODO: done for the 2nd deliverable
    private void publishMessage(String topic, String message) {
        Intent intent = new Intent(this, MqttService.class);
        intent.setAction(MqttService.ACTION_PUBLISH);
        intent.putExtra(MqttService.EXTRA_TOPIC, topic);
        intent.putExtra(MqttService.EXTRA_MESSAGE, message);
        startService(intent);
    }

}