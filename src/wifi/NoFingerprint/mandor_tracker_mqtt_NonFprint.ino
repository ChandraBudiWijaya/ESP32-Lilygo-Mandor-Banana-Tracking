// =======================================================
//   MANDOR BANANA TRACKING - MAIN SKETCH
//   Versi: WiFi, Tanpa Fingerprint, Prioritas Sinkronisasi
//   Logika Offline Diperbaiki
// =======================================================

#include "config.h"
#include "gps_utils.h" // Asumsi GpsData struct didefinisikan di sini
#include "sd_utils.h"
#include "network_utils.h"

// State (Status) Global
bool ledState = false;

// Pewaktu (Timer)
unsigned long lastPublishMillis = 0;
unsigned long lastBlinkMillis = 0;

// Fungsi untuk mengedipkan LED hijau sebagai tanda alat aktif
void handle_status_led() {
  if (millis() - lastBlinkMillis > LED_BLINK_INTERVAL_MS) {
    lastBlinkMillis = millis();
    ledState = !ledState;
    digitalWrite(GREEN_LED_PIN, ledState);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Mandor Banana Tracker Starting (Priority Sync & Offline Fix) ---");

  pinMode(GREEN_LED_PIN, OUTPUT);
  
  init_gps();
  if (!init_sd_card()) {
    Serial.println("FATAL: SD Card failed. Halting.");
    while(1); 
  }
  
  init_network();
}

void loop() {
  // --- TUGAS LATAR BELAKANG (SELALU BERJALAN) ---
  handle_status_led();
  network_loop(); // Fungsi ini akan mencoba menjaga koneksi.

  // --- LOGIKA PENCATATAN & PENGIRIMAN DATA PERIODIK ---
  // Blok ini sekarang akan selalu dieksekusi, tidak peduli status koneksi.
  if (millis() - lastPublishMillis >= PUBLISH_INTERVAL_MS) {
    lastPublishMillis = millis();
    GpsData currentGpsData = read_gps_data();
    
    if (currentGpsData.isValid) {
      Serial.println("\n--- Interval Reached: Processing Data ---");
      
      // --- START: TAMBAHAN UNTUK MENAMPILKAN SATELIT DAN HDOP ---
      Serial.print("GPS Data - ");
      Serial.print("Satelit: ");
      // Ganti 'currentGpsData.numSatellites' dengan nama anggota yang benar untuk jumlah satelit di struct GpsData Anda
      // Contoh: currentGpsData.satellites.value() jika Anda menggunakan TinyGPSPlus
      Serial.print(currentGpsData.satellites); // <-- Ini bagian yang perlu Anda konfirmasi namanya
      
      Serial.print(" | HDOP: ");
      // Ganti 'currentGpsData.hdop' dengan nama anggota yang benar untuk HDOP di struct GpsData Anda
      // Contoh: currentGpsData.hdop.value() jika Anda menggunakan TinyGPSPlus
      Serial.println(currentGpsData.hdop); // <-- Ini bagian yang perlu Anda konfirmasi namanya
      // --- END: TAMBAHAN ---

      // 1. Selalu tulis ke master log di SD Card, tidak peduli koneksi.
      //    Ini adalah langkah krusial untuk memastikan tidak ada data hilang.
      write_to_daily_log(currentGpsData);
      
      // 2. Coba kirim data. Fungsi publish_data() sudah pintar:
      //    - Jika online, ia akan mengirim ke MQTT.
      //    - Jika offline, ia akan memanggil add_to_offline_queue().
      publish_data(currentGpsData);

    } else {
      Serial.println("GPS data not valid. Waiting for signal fix...");
    }
  }

  // --- LOGIKA SINKRONISASI (HANYA BERJALAN JIKA TERHUBUNG) ---
  // Jika ada koneksi dan ada data di antrean, prioritaskan pengiriman data lama.
  if (mqtt.connected() && isOfflineQueueNotEmpty()) {
    Serial.println("Offline queue detected. Prioritizing sync...");
    sync_offline_data(); // Coba kirim satu data dari antrean.
    delay(200); // Beri jeda singkat agar tidak membebani jaringan.
  }
}