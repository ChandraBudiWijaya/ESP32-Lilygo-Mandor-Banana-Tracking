// =======================================================
//   MANDOR TRACKING SYSTEM - MAIN SKETCH
//   Versi: SIM800L + Fingerprint
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

// Variabel untuk melacak status dan waktu otentikasi
bool isAuthenticated = false;
unsigned long lastValidationMillis = 0;


/**
 * @brief Mengedipkan LED hijau sebagai indikator bahwa perangkat sedang aktif dan terotentikasi.
 */
void handle_status_led() {
  if (millis() - lastBlinkMillis > LED_BLINK_INTERVAL_MS) {
    lastBlinkMillis = millis();
    ledState = !ledState;
    if (isAuthenticated) {
      digitalWrite(GREEN_LED_PIN, ledState);
    }
  }
}

/**
 * @brief Fungsi setup utama, dieksekusi sekali saat perangkat menyala.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Mandor Tracking System (SIM800L + Fingerprint + Priority Sync & Offline Fix) ---");

  // Inisialisasi pin-pin LED
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Inisialisasi semua modul perangkat keras
  init_gps();
  init_network();
  WiFi.mode(WIFI_OFF);
  btStop();
  
  // Inisialisasi Kartu SD adalah langkah kritis.
  // Jika gagal, hentikan program karena mode offline tidak akan berfungsi.
  if (!init_sd_card()) {
    Serial.println("FATAL: SD Card failed. Halting.");
    digitalWrite(RED_LED_PIN, HIGH);
    while(1) {
      // Berhenti di sini, kedipkan LED merah sebagai tanda error fatal.
      digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
      delay(100);
    }
  }

  if (!init_fingerprint()) {
    Serial.println("FATAL: Fingerprint sensor failed. Halting.");
    digitalWrite(RED_LED_PIN, HIGH);
    while(1) {
      // Berhenti di sini, kedipkan LED merah sebagai tanda error fatal.
      digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
      delay(100);
    }
  }

  // --- TAHAP OTENTIKASI AWAL ---
  Serial.println("\n--- WAITING FOR INITIAL AUTHENTICATION ---");
  while (true) {
    digitalWrite(RED_LED_PIN, HIGH); // Nyalakan LED merah, menandakan butuh verifikasi
    if (validate_fingerprint()) {
      isAuthenticated = true; // Set status menjadi terotentikasi
      lastValidationMillis = millis(); // Mulai timer untuk validasi ulang
      digitalWrite(RED_LED_PIN, LOW); // Matikan LED merah
      
      // Beri sinyal visual bahwa otentikasi berhasil (kedip hijau 3x)
      for (int i=0; i<3; i++) {
        digitalWrite(GREEN_LED_PIN, HIGH); delay(150);
        digitalWrite(GREEN_LED_PIN, LOW); delay(150);
      }
      break; // Keluar dari loop otentikasi dan lanjutkan ke program utama
    } else {
      delay(1000); // Beri jeda singkat jika otentikasi gagal
    }
  }

  Serial.println("\nAuthentication successful. Setup complete. Starting main loop...");
}

/**
 * @brief Fungsi loop utama, dieksekusi berulang kali.
 */
void loop() {
  // --- TUGAS LATAR BELAKANG (SELALU BERJALAN) ---
  // Tugas-tugas ini penting untuk menjaga status dan konektivitas perangkat.
  handle_status_led();
  network_loop(); // Menangani koneksi SIM800L & MQTT secara otomatis.

  // --- LOGIKA VALIDASI ULANG SETIAP 1 JAM ---
  // Cek apakah sudah waktunya untuk meminta verifikasi sidik jari kembali
  if (isAuthenticated && (millis() - lastValidationMillis >= VALIDATION_INTERVAL_MS)) {
    Serial.println("\n--- Authentication Expired! Please re-validate. ---");
    isAuthenticated = false; // Kunci kembali perangkat
    digitalWrite(GREEN_LED_PIN, LOW); // Matikan LED status hijau
  }

  // Jika status tidak terotentikasi, hentikan semua operasi dan minta sidik jari.
  if (!isAuthenticated) {
    digitalWrite(RED_LED_PIN, HIGH);  // Nyalakan LED merah untuk meminta verifikasi

    if (validate_fingerprint()) {
        isAuthenticated = true; // Buka kunci perangkat
        lastValidationMillis = millis(); // Reset timer validasi
        digitalWrite(RED_LED_PIN, LOW);
        Serial.println("Re-validation successful!");
    }
    return; // Hentikan eksekusi loop() lebih lanjut sampai terotentikasi kembali
  }

  // --- OPERASI NORMAL (HANYA BERJALAN JIKA TEROTENTIKASI) ---

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