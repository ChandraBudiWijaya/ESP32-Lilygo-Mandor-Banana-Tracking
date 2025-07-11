#pragma once

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h> // Diperlukan untuk MAC Address dan mematikan WiFi
#include "config.h"
#include "sd_utils.h"

HardwareSerial simSerial(1);
TinyGsm modem(simSerial);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

// Variabel untuk menyimpan Client ID unik
String clientId;

unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 30000;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {}

void mqtt_connect() {
    Serial.print("Connecting to MQTT broker...");
    if (mqtt.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
        Serial.println(" success");
        
        // === WIFI DIMATIKAN DI SINI SETELAH MQTT BERHASIL TERHUBUNG ===
        Serial.println("MQTT connected. Turning off WiFi to save power.");
        WiFi.mode(WIFI_OFF);
        // =============================================================

    } else {
        Serial.print(" failed with state ");
        Serial.println(mqtt.state());
    }
}

void init_network() {
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    digitalWrite(MODEM_PWKEY, LOW);
    digitalWrite(MODEM_RST, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);

    simSerial.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    // Aktifkan WiFi hanya untuk mengambil MAC Address
    WiFi.mode(WIFI_STA);
    clientId = "MandorTracker-";
    clientId += WiFi.macAddress();
    Serial.print("Using unique Client ID: ");
    Serial.println(clientId);
    // Jangan matikan WiFi di sini, biarkan aktif sampai MQTT terhubung

    Serial.println("Initializing modem...");
    if (!modem.init()) {
        Serial.println("FATAL: Failed to init modem. Halting.");
        while(1);
    }

    Serial.println("Waiting for network...");
    if (!modem.waitForNetwork(60000L)) {
        Serial.println("Network connection failed.");
    } else {
        Serial.println("Network connected.");
    }
    
    Serial.print("Signal Quality: ");
    Serial.println(modem.getSignalQuality());

    Serial.print("Connecting to GPRS with APN: ");
    Serial.println(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println("GPRS connection failed. Will retry in loop.");
    } else {
        Serial.println("GPRS connected successfully!");
    }

    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqtt_callback);
    mqtt.setBufferSize(512);
}

void network_loop() {
    if (!modem.isGprsConnected()) {
        Serial.println("GPRS disconnected. Attempting to reconnect...");
        modem.gprsConnect(apn, gprsUser, gprsPass);
    }
    
    if (modem.isGprsConnected() && !mqtt.connected()) {
        if (millis() - lastReconnectAttempt > reconnectInterval) {
            lastReconnectAttempt = millis();
            mqtt_connect();
        }
    }
    
    mqtt.loop();
}

void publish_data(const GpsData& data) {
    if (!data.isValid) return;

    JsonDocument doc;
    doc["index_karyawan"] = INDEX_KARYAWAN;
    doc["lat"] = data.lat;
    doc["lng"] = data.lng;
    doc["device_timestamp"] = data.isoTimestamp;
    doc["hdop"] = data.hdop;
    doc["satellites"] = data.satellites;
    
    String payload;
    serializeJson(doc, payload);

    if (mqtt.connected()) {
        Serial.print("Publishing live data: ");
        Serial.println(payload);
        mqtt.publish(mqtt_topic, payload.c_str());
    } else {
        Serial.println("MQTT not connected. Queuing data.");
        add_to_offline_queue(payload.c_str());
    }
}

void sync_offline_data() {
    if (!mqtt.connected()) return;
    String payload_from_queue = read_next_line_from_queue();
    if (payload_from_queue.length() > 0) {
        Serial.print("Syncing offline data: ");
        Serial.println(payload_from_queue);
        if (!mqtt.publish(mqtt_topic, payload_from_queue.c_str())) {
            Serial.println("Sync failed. Reverting progress pointer.");
            revert_progress(payload_from_queue);
        } else {
            Serial.println("Sync successful.");
        }
    }
}