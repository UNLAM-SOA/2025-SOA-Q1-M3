package com.pastillero.application.mqtt;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import java.util.UUID;


public class MqttService extends Service {

    private static final String CHANNEL_ID = "MqttServiceChannel";
    private static final int NOTIFICATION_ID = 1;

    public static final String ACTION_PUBLISH = "com.pastillero.application.mqtt.ACTION_PUBLISH";
    public static final String EXTRA_TOPIC = "com.pastillero.application.mqtt.EXTRA_TOPIC";
    public static final String EXTRA_MESSAGE = "com.pastillero.application.mqtt.EXTRA_MESSAGE";

    private MqttHandler mqttHandler;


    private final Handler reconnectionHandler = new Handler(Looper.getMainLooper());
    private static final int RECONNECTION_DELAY = 5000;

    private final Runnable reconnectRunnable = new Runnable() {
        @Override
        public void run() {
            if (mqttHandler != null) {
                Log.i("MQTT", "Trying to reconnect MQTT...");
                connectMQTT();
            }

            reconnectionHandler.postDelayed(this, RECONNECTION_DELAY);
        }
    };


    @Override
    public void onCreate() {
        super.onCreate();

        createNotificationChannel();
        startForeground(NOTIFICATION_ID, createNotification());

        mqttHandler = new MqttHandler(getApplicationContext());

        mqttHandler.setConnectionLostCallback(() -> {
            Log.i("Pastillero", "Conexión MQTT perdida, iniciando reintentos...");

            startReconnectionAttempts();
        });

        connectMQTT();

    }

    private void startReconnectionAttempts() {
        reconnectionHandler.removeCallbacks(reconnectRunnable);
        reconnectionHandler.post(reconnectRunnable);
    }

    private void stopReconnectionAttempts() {
        reconnectionHandler.removeCallbacks(reconnectRunnable);
    }


    private void connectMQTT() {
        mqttHandler.connect(ConfigMQTT.SERVER_URL, UUID.randomUUID().toString(), ConfigMQTT.USERNAME, ConfigMQTT.PASSWORD, new MqttConnectionCallback() {

            @Override
            public void onSuccess() {
                mqttHandler.subscribe(ConfigMQTT.NEXT_DOSE_TOPIC);
                mqttHandler.subscribe(ConfigMQTT.ACTUAL_STATUS_TOPIC);
                mqttHandler.subscribe(ConfigMQTT.PILL_SCAN_TOPIC);

                stopReconnectionAttempts();

                Log.i("MQTT", "Service successfully connected to MQTT");
            }

            @Override
            public void onFailure(Throwable exception) {
               Log.e("MQTT", "Service failed to connect to MQTT", exception);
               startReconnectionAttempts();
            }
        });
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createNotificationChannel() {
        NotificationChannel serviceChannel = new NotificationChannel(
                CHANNEL_ID,
                "Pastillero Service Channel",
                NotificationManager.IMPORTANCE_LOW
        );

        NotificationManager manager = getSystemService(NotificationManager.class);
        manager.createNotificationChannel(serviceChannel);
    }


    private Notification createNotification() {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Pastillero")
                .setContentText("Listening for MQTT messages")
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setOngoing(true);

        return builder.build();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if(intent != null && ACTION_PUBLISH.equals(intent.getAction())) {
            String topic = intent.getStringExtra(EXTRA_TOPIC);
            String message = intent.getStringExtra(EXTRA_MESSAGE);
            if(topic != null && message != null) { mqttHandler.publish(topic, message); }
        }

        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        stopReconnectionAttempts();

        if(mqttHandler != null){
            mqttHandler.disconnect();
        }

        super.onDestroy();
    }


}
