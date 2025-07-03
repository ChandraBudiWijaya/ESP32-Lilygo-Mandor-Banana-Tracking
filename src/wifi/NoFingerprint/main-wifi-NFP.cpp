// =======================================================
//   MANDOR BANANA TRACKING - MAIN SKETCH
//   Versi: WiFi Tanpa Fingerprint
// =======================================================

#include <Arduino.h>
#include "config.h"
#include "gps_utils.h"
#include "sd_utils.h"
#include "network_utils.h"

// === STATE & PEWAKTU GLOBAL ===
bool ledState = false;
unsigned long lastPublishMillis = 0;
unsigned long lastBlinkMillis = 0;


// === FUNGSI-FUNGSI BANTU ===

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


// === FUNGSI UTAMA ===

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Mandor Banana Tracker Starting (Priority Sync & Offline Fix) ---");

  // Inisialisasi pin untuk LED status
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  // Inisialisasi modul-modul utama
  init_gps();
  init_network();

  // Inisialisasi Kartu SD adalah langkah kritis.
  // Jika gagal, hentikan program karena mode offline tidak akan berfungsi.
  if (!init_sd_card()) {
    Serial.println("FATAL: SD Card failed. Halting.");
    while(1) {
      // Berhenti di sini, kedipkan LED merah sebagai tanda error fatal.
      digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN));
      delay(100);
    }
  }
  
  Serial.println("\nSetup complete. Starting main loop...");
}

void loop() {
  // --- TUGAS LATAR BELAKANG (SELALU BERJALAN) ---
  // Tugas-tugas ini penting untuk menjaga status dan konektivitas perangkat.
  handle_status_led();
  network_loop(); // Menangani koneksi WiFi & MQTT secara otomatis.


  // --- LOGIKA PENCATATAN & PENGIRIMAN DATA PERIODIK ---
  // Blok ini dieksekusi setiap interval PUBLISH_INTERVAL_MS untuk memproses data GPS.
  if (millis() - lastPublishMillis >= PUBLISH_INTERVAL_MS) {
    lastPublishMillis = millis();
    GpsData currentGpsData = read_gps_data();
    
    if (currentGpsData.isValid) {
      Serial.println("\n--- Processing New GPS Data ---");
      Serial.printf("Satelit: %u | HDOP: %.2f\n", (unsigned int)currentGpsData.satellites, currentGpsData.hdop);

      // 1. SIMPAN: Selalu tulis data ke log harian di SD Card.
      //    Ini adalah jaring pengaman utama agar tidak ada data yang hilang.
      write_to_daily_log(currentGpsData);
      
      // 2. KIRIM/ANTREKAN: Fungsi publish_data() sudah cerdas:
      //    - Jika online, data langsung dikirim ke server MQTT.
      //    - Jika offline, data otomatis disimpan ke file antrean di SD Card.
      publish_data(currentGpsData);

    } else {
      Serial.println("GPS data not valid. Waiting for signal fix...");
    }
  }


  // --- LOGIKA SINKRONISASI PRIORITAS ---
  // Jika perangkat online dan ada data di antrean offline,
  // prioritaskan pengiriman data lama terlebih dahulu.
  if (mqtt.connected() && isOfflineQueueNotEmpty()) {
    Serial.println("Offline queue detected. Prioritizing sync...");
    sync_offline_data(); // Coba kirim satu data dari antrean.
    delay(750); // Beri jeda singkat untuk memberi napas pada jaringan.
  }
}