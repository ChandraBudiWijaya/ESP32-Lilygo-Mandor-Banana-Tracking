  #pragma once
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <PubSubClient.h>
  #include <ArduinoJson.h>
  #include "config.h"
  #include "sd_utils.h"

  // Inisialisasi objek client menggunakan WiFi
  WiFiClientSecure client; // Client untuk koneksi aman
  PubSubClient mqtt(client);

  void setup_wifi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA); // Set ESP32 sebagai WiFi Station
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nFailed to connect to WiFi.");
    }
  }

  // Fungsi callback saat ada pesan MQTT masuk (tidak digunakan di sini)
  void mqtt_callback(char* topic, byte* payload, unsigned int length) {}

  // Menghubungkan kembali ke MQTT
  void mqtt_reconnect() {
    int retries = 0;
    while (!mqtt.connected() && retries < 3) {
      retries++;
      Serial.print("Attempting MQTT connection...");
      if (mqtt.connect(INDEX_KARYAWAN, mqtt_user, mqtt_pass)) {
        Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }

  // Inisialisasi Jaringan (sekarang menggunakan WiFi)
  bool init_network() {
    setup_wifi();
    
    // Untuk koneksi aman ke port 8883, kita perlu melewati validasi sertifikat
    client.setInsecure(); 

    mqtt.setServer(mqtt_server, mqtt_port);
    mqtt.setCallback(mqtt_callback);
    
    return WiFi.status() == WL_CONNECTED;
  }

  // Loop utama untuk menjaga koneksi
  void network_loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected. Attempting reconnect...");
        setup_wifi();
        return;
    }
      
    if (!mqtt.connected()) {
      mqtt_reconnect();
    }
    mqtt.loop(); // Penting untuk menjaga koneksi MQTT
  }

  // Mem-publish data ke MQTT
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

  /**
  * @brief Fungsi untuk sinkronisasi data dari antrean offline (metode efisien).
  */
  void sync_offline_data() {
      if (!mqtt.connected()) return;

      // Membaca baris berikutnya dari antrean menggunakan fungsi yang benar.
      // Fungsi ini sudah otomatis memperbarui pointer progres.
      String payload_from_queue = read_next_line_from_queue();
      
      if (payload_from_queue.length() > 0) {
          Serial.print("Syncing offline data: ");
          Serial.println(payload_from_queue);
          
          bool success = mqtt.publish(mqtt_topic, payload_from_queue.c_str());
          
          if (!success) {
              // Jika publish gagal, KEMBALIKAN progres pointer ke posisi sebelumnya.
              Serial.println("Sync failed. Reverting progress pointer.");
              revert_progress(payload_from_queue);
          } else {
              Serial.println("Sync successful.");
          }
      }
  }
