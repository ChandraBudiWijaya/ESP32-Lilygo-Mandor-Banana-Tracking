#pragma once
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "sd_utils.h"

WiFiClientSecure client;
PubSubClient mqtt(client);

// --- Variabel untuk manajemen koneksi WiFi (Exponential Backoff) ---
unsigned long previousWifiMillis = 0;
unsigned long wifiRetryDelay = 5000; // Mulai dengan jeda 5 detik
const unsigned long maxWifiRetryDelay = 300000; // Jeda maksimal 5 menit

// --- Variabel untuk manajemen koneksi MQTT (Exponential Backoff) ---
unsigned long previousMqttMillis = 0;
unsigned long mqttRetryDelay = 5000;
const unsigned long maxMqttRetryDelay = 60000; // Jeda maksimal 1 menit

void mqtt_callback(char* topic, byte* payload, unsigned int length) {}

// --- Fungsi Koneksi Cerdas ---

void handle_wifi_connection() {
    if (WiFi.status() == WL_CONNECTED) {
        return; // Sudah konek, tidak perlu apa-apa
    }
    // Hanya coba hubungkan jika sudah waktunya
    if (millis() - previousWifiMillis >= wifiRetryDelay) {
        previousWifiMillis = millis();
        Serial.println("WiFi Disconnected. Attempting to reconnect...");
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);

        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.printf("Failed to connect. Retrying in %lu seconds.\n", wifiRetryDelay / 1000);
            wifiRetryDelay *= 2; // Gandakan jeda
            if (wifiRetryDelay > maxWifiRetryDelay) {
                wifiRetryDelay = maxWifiRetryDelay;
            }
        } else {
            Serial.println("\nWiFi reconnected!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            wifiRetryDelay = 5000; // Reset jeda jika berhasil
        }
    }
}

void mqtt_reconnect() {
    if (!mqtt.connected() && millis() - previousMqttMillis >= mqttRetryDelay) {
        previousMqttMillis = millis();
        Serial.print("Attempting MQTT connection...");
        if (mqtt.connect(INDEX_KARYAWAN, mqtt_user, mqtt_pass)) {
            Serial.println("connected");
            mqttRetryDelay = 5000; // Reset jika berhasil
        } else {
            Serial.printf("failed, rc=%d. Retrying in %lu sec\n", mqtt.state(), mqttRetryDelay/1000);
            mqttRetryDelay *= 2; // Gandakan jeda
            if (mqttRetryDelay > maxMqttRetryDelay) {
                mqttRetryDelay = maxMqttRetryDelay;
            }
        }
    }
}

// --- Fungsi Utama Jaringan ---

void init_network() {
    client.setInsecure(); // Untuk koneksi aman ke port 8883, lewati validasi sertifikat
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqtt_callback);
    handle_wifi_connection(); // Lakukan percobaan koneksi pertama kali
}

void network_loop() {
    handle_wifi_connection(); // Selalu cek dan tangani koneksi WiFi
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqtt.connected()) {
            mqtt_reconnect();
        }
        mqtt.loop(); // Penting untuk menjaga koneksi MQTT
    }
}

void publish_data(const GpsData& data) {
    if (!data.isValid) return;

    StaticJsonDocument<256> doc;
    doc["index_karyawan"] = INDEX_KARYAWAN;
    doc["lat"] = data.lat;
    doc["lng"] = data.lng;
    doc["timestamp"] = data.isoTimestamp;
    doc["hdop"] = data.hdop;
    doc["satellites"] = data.satellites;
    
    char payload[256];
    serializeJson(doc, payload);

    if (mqtt.connected()) {
        Serial.print("Publishing live data: ");
        Serial.println(payload);
        mqtt.publish(mqtt_topic, payload);
    } else {
        Serial.println("MQTT not connected. Queuing data.");
        add_to_offline_queue(payload);
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