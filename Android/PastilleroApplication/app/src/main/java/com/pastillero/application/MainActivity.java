package com.pastillero.application;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import com.pastillero.application.mqtt.ConfigMQTT;
import com.pastillero.application.mqtt.MqttConnectionCallback;
import com.pastillero.application.mqtt.MqttHandler;

import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private MqttHandler mqttHandler;

    private TextView nextDoseTimeText;
    private TextView actualStatusText;

    public IntentFilter filterReceiveNextDoseMessage;
    public IntentFilter filterReceiveActualStatusMessage;
    public IntentFilter filterReceivePillStatusMessage;

    public IntentFilter filterConnectionLost;

    private final NextDoseReceiver nextDoseReceiver = new NextDoseReceiver();
    private final ActualStatusReceiver actualStatusReceiver = new ActualStatusReceiver();
    private final PillStatusReceiver pillStatusReceiver = new PillStatusReceiver();

    private final ConnectionLostReceiver connectionLostReceiver = new ConnectionLostReceiver();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        nextDoseTimeText = findViewById(R.id.next_dose_time);
        actualStatusText = findViewById(R.id.actual_status);

        mqttHandler = new MqttHandler(getApplicationContext());

        connectMQTT();

        configBroadcastReceivers();

    }

    @Override
    protected void onDestroy(){
        mqttHandler.disconnect();
        unregisterReceiver(nextDoseReceiver);
        unregisterReceiver(actualStatusReceiver);
        unregisterReceiver(pillStatusReceiver);
        unregisterReceiver(connectionLostReceiver);
        super.onDestroy();
    }

    private void connectMQTT() {
        mqttHandler.connect(ConfigMQTT.SERVER_URL, UUID.randomUUID().toString(), ConfigMQTT.USERNAME, ConfigMQTT.PASSWORD, new MqttConnectionCallback() {

            @Override
            public void onSuccess() {
                subscribeToTopic(ConfigMQTT.NEXT_DOSE_TOPIC);
                subscribeToTopic(ConfigMQTT.ACTUAL_STATUS_TOPIC);
                subscribeToTopic(ConfigMQTT.PILL_STATUS_TOPIC);
                Log.i("Pastillero", "Successfully connected");
            }

            @Override
            public void onFailure(Throwable exception) {
                Toast.makeText(MainActivity.this, "Error connecting to MQTT", Toast.LENGTH_SHORT).show();
            }
        });
    }


    /* The supress annotation is due to registerReceiver needs to know if the receiver listen to
    messages from other apps or only from your app */
    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    private void configBroadcastReceivers(){
        filterReceiveNextDoseMessage = new IntentFilter(MqttHandler.NEXT_DOSE_MESSAGE_RECEIVED);
        filterReceiveActualStatusMessage = new IntentFilter(MqttHandler.ACTUAL_STATUS_MESSAGE_RECEIVED);
        filterReceivePillStatusMessage = new IntentFilter(MqttHandler.PILL_STATUS_MESSAGE_RECEIVED);
        filterConnectionLost = new IntentFilter(MqttHandler.CONNECTION_LOST);

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU){
            registerReceiver(connectionLostReceiver, filterConnectionLost, Context.RECEIVER_NOT_EXPORTED);
            registerReceiver(nextDoseReceiver, filterReceiveNextDoseMessage, Context.RECEIVER_NOT_EXPORTED);
            registerReceiver(actualStatusReceiver, filterReceiveActualStatusMessage, Context.RECEIVER_NOT_EXPORTED);
            registerReceiver(pillStatusReceiver, filterReceivePillStatusMessage, Context.RECEIVER_NOT_EXPORTED);

            Log.i("Pastillero", "Registered receivers");
        } else {
            registerReceiver(connectionLostReceiver, filterConnectionLost);
            registerReceiver(nextDoseReceiver, filterReceiveNextDoseMessage);
            registerReceiver(actualStatusReceiver, filterReceiveActualStatusMessage);
            registerReceiver(pillStatusReceiver, filterReceivePillStatusMessage);
            Log.i("Pastillero", "Registered receivers");
        }
}

    private void subscribeToTopic(String topic){
        Toast.makeText(this, "Subscribing to topic "+ topic, Toast.LENGTH_SHORT).show();
        mqttHandler.subscribe(topic);
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

            if(value == 1) {
                actualStatusText.setBackgroundColor(ContextCompat.getColor(context, R.color.green));
            } else {
                actualStatusText.setBackgroundColor(ContextCompat.getColor(context, R.color.white));
            }
        }
    }

    public class ConnectionLostReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Toast.makeText(context, "Connection lost", Toast.LENGTH_SHORT).show();
            connectMQTT();
        }
    }

    public class PillStatusReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {

        }
    }
}