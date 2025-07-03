#pragma once

// Matikan fitur WiFi bawaan untuk menghemat memori
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "sd_utils.h"

// Inisialisasi modem pada Serial Port 1 ESP32
HardwareSerial simSerial(1);
TinyGsm modem(simSerial);

// Gunakan TinyGsmClientSecure untuk koneksi MQTT yang aman (port 8883)
TinyGsmClientSecure client(modem);
PubSubClient mqtt(client);

// Variabel untuk manajemen koneksi GPRS & MQTT
unsigned long lastReconnectAttempt = 0;
const long reconnectInterval = 30000; // Coba koneksi ulang setiap 30 detik jika gagal

void mqtt_callback(char* topic, byte* payload, unsigned int length) {}

/**
 * @brief Menginisialisasi modem SIM800L.
 */
bool init_modem() {
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    digitalWrite(MODEM_PWKEY, LOW);
    digitalWrite(MODEM_RST, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);

    simSerial.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    
    Serial.println("Initializing modem...");
    if (!modem.init()) {
        Serial.println("Failed to init modem. Check connections.");
        return false;
    }
    
    String modemInfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modemInfo);
    
    return true;
}

/**
 * @brief Menghubungkan ke jaringan GPRS.
 */
bool connect_gprs() {
    Serial.print("Connecting to GPRS...");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail");
        return false;
    }
    Serial.println(" success");
    Serial.print("Signal Quality: ");
    Serial.println(modem.getSignalQuality());
    return true;
}

/**
 * @brief Menghubungkan ke server MQTT.
 */
void mqtt_connect() {
    Serial.print("Connecting to MQTT broker...");
    if (mqtt.connect(INDEX_KARYAWAN, mqtt_user, mqtt_pass)) {
        Serial.println(" success");
    } else {
        Serial.print(" failed with state ");
        Serial.println(mqtt.state());
    }
}

/**
 * @brief Inisialisasi jaringan utama (Modem dan MQTT).
 */
void init_network() {
    if (!init_modem()) {
        Serial.println("Halting due to modem failure.");
        while(1);
    }
    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqtt_callback);
}

/**
 * @brief Fungsi loop untuk menjaga koneksi GPRS dan MQTT.
 */
void network_loop() {
    if (!modem.isNetworkConnected()) {
        Serial.println("GPRS disconnected. Attempting to reconnect...");
        connect_gprs();
    }
    
    if (!mqtt.connected()) {
        if (millis() - lastReconnectAttempt > reconnectInterval) {
            lastReconnectAttempt = millis();
            if (modem.isGprsConnected()) {
                mqtt_connect();
            }
        }
    }
    mqtt.loop();
}

/**
 * @brief Membuat payload JSON dan mempublikasikannya ke MQTT atau menyimpannya ke antrean.
 *
 * @param data Struct GpsData yang berisi informasi yang akan dikirim.
 */
void publish_data(const GpsData& data) {
    if (!data.isValid) return;

    // UPDATE: Gunakan JsonDocument yang direkomendasikan ArduinoJson v7+
    JsonDocument doc;
    doc["index_karyawan"] = INDEX_KARYAWAN;
    doc["lat"] = data.lat;
    doc["lng"] = data.lng;
    doc["device_timestamp"] = data.isoTimestamp;
    doc["hdop"] = data.hdop;
    doc["satellites"] = data.satellites;

    // UPDATE: Serialize ke objek String untuk manajemen memori yang lebih aman
    String payload;
    serializeJson(doc, payload);

    if (mqtt.connected()) {
        Serial.print("Publishing live data: ");
        Serial.println(payload);
        // Kirim data dengan mengonversinya ke const char*
        mqtt.publish(mqtt_topic, payload.c_str());
    } else {
        Serial.println("MQTT not connected. Queuing data.");
        // Simpan data dengan mengonversinya ke const char*
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