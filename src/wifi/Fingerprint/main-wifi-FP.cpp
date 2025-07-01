// =======================================================
//   MANDOR BANANA TRACKING - MAIN SKETCH
//   Versi: WiFi + Fingerprint
// =======================================================

#include <Arduino.h>
#include "config.h"
#include "gps_utils.h"
#include "sd_utils.h"
#include "network_utils.h"
#include "fingerprint_utils.h"

// === STATE & PEWAKTU GLOBAL ===
bool ledState = false;
unsigned long lastPublishMillis = 0;
unsigned long lastBlinkMillis = 0;

void handle_status_led() {
  if (millis() - lastBlinkMillis > LED_BLINK_INTERVAL_MS) {
    lastBlinkMillis = millis();
    ledState = !ledState;
    digitalWrite(GREEN_LED_PIN, ledState);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Mandor Banana Tracker (WiFi + Fingerprint) ---");

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Inisialisasi modul-modul dasar
  init_gps();
  init_network();

  if (!init_sd_card()) {
    Serial.println("FATAL: SD Card failed. Halting.");
    while(1); // Hentikan jika SD Card gagal
  }

  // Inisialisasi Sensor Sidik Jari
  if (!init_fingerprint()) {
    Serial.println("FATAL: Fingerprint sensor not found. Halting.");
    while(1); // Hentikan jika sensor tidak ditemukan
  }

  // --- TAHAP OTENTIKASI ---
  Serial.println("\n--- WAITING FOR AUTHENTICATION ---");
  while (true) {
    digitalWrite(GREEN_LED_PIN, HIGH); // Nyalakan LED hijau, minta sidik jari
    if (validate_fingerprint()) {
      digitalWrite(GREEN_LED_PIN, LOW);
      // Beri sinyal sukses (kedip hijau 3x)
      for (int i=0; i<3; i++) {
        digitalWrite(GREEN_LED_PIN, HIGH); delay(150);
        digitalWrite(GREEN_LED_PIN, LOW); delay(150);
      }
      break; // Keluar dari loop otentikasi jika berhasil
    } else {
      // Jika gagal, beri sinyal error (merah menyala)
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
      delay(2000); // Tahan LED merah selama 2 detik
      digitalWrite(RED_LED_PIN, LOW);
    }
  }

  Serial.println("\nAuthentication successful. Starting main tracking loop...");
}

void loop() {
  // --- TUGAS LATAR BELAKANG ---
  handle_status_led();
  network_loop();

  // --- LOGIKA PENCATATAN & PENGIRIMAN DATA PERIODIK ---
  if (millis() - lastPublishMillis >= PUBLISH_INTERVAL_MS) {
    lastPublishMillis = millis();
    GpsData currentGpsData = read_gps_data();
    
    if (currentGpsData.isValid) {
      Serial.println("\n--- Processing New GPS Data ---");
      Serial.printf("Satelit: %u | HDOP: %.2f\n", (unsigned int)currentGpsData.satellites, currentGpsData.hdop);
      write_to_daily_log(currentGpsData);
      publish_data(currentGpsData);
    } else {
      Serial.println("GPS data not valid. Waiting for signal fix...");
    }
  }

  // --- LOGIKA SINKRONISASI PRIORITAS ---
  if (mqtt.connected() && isOfflineQueueNotEmpty()) {
    Serial.println("Offline queue detected. Prioritizing sync...");
    sync_offline_data();
    delay(200);
  }
}