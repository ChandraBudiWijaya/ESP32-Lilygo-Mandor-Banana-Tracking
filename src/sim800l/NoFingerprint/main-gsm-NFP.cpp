#include <Arduino.h>
#include "config.h"
#include "gps_utils.h"
#include "sd_utils.h"
#include "network_utils.h"

// === STATE & PEWAKTU GLOBAL ===
bool ledState = false;
unsigned long lastPublishMillis = 0;
unsigned long lastBlinkMillis = 0;

/**
 * @brief Mengedipkan LED hijau sebagai indikator visual bahwa perangkat sedang berjalan.
 */
void handle_status_led() {
  if (millis() - lastBlinkMillis > LED_BLINK_INTERVAL_MS) {
    lastBlinkMillis = millis();
    ledState = !ledState;
    digitalWrite(GREEN_LED_PIN, ledState);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Mandor Banana Tracker (SIM800L - No Fingerprint) ---");

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  // Inisialisasi semua modul
  init_network(); 
  init_gps();
  
  if (!init_sd_card()) {
    Serial.println("FATAL: SD Card failed. Halting.");
    digitalWrite(RED_LED_PIN, HIGH);
    while(1);
  }

  Serial.println("Setup complete. Starting main tracking loop...");
}

void loop() {
  // --- TUGAS LATAR BELAKANG ---
  handle_status_led();
  network_loop(); // Menjaga koneksi GPRS dan MQTT

  // --- OPERASI NORMAL ---
  if (millis() - lastPublishMillis >= PUBLISH_INTERVAL_MS) {
    lastPublishMillis = millis();
    GpsData currentGpsData = read_gps_data();
    
    if (currentGpsData.isValid) {
      Serial.println("\n--- Processing New GPS Data ---");
      Serial.printf("Satelit: %u | HDOP: %.2f\n", (unsigned int)currentGpsData.satellites, currentGpsData.hdop);
      
      // Selalu catat ke SD Card
      write_to_daily_log(currentGpsData);
      
      // Kirim atau antrekan data
      publish_data(currentGpsData);
    } else {
      Serial.println("GPS data not valid. Waiting for signal fix...");
    }
  }

  // --- LOGIKA SINKRONISASI ---
  if (mqtt.connected() && isOfflineQueueNotEmpty()) {
    Serial.println("Offline queue detected. Syncing...");
    sync_offline_data();
    delay(1000); // Beri jeda untuk stabilitas GPRS
  }
}