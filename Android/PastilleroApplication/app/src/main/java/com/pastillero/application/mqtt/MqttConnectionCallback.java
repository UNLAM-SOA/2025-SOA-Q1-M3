package com.pastillero.application.mqtt;

public interface MqttConnectionCallback {
    void onSuccess();
    void onFailure(Throwable exception);
}
