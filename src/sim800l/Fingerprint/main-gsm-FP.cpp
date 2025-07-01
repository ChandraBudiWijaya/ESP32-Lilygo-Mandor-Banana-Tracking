// =======================================================
//   MANDOR BANANA TRACKING - MAIN SKETCH
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
  Serial.println("\n--- Mandor Banana Tracker (SIM800L + Fingerprint + Re-Validation) ---");

  // Inisialisasi pin-pin LED
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Inisialisasi semua modul perangkat keras
  init_network(); 
  init_gps();
  
  if (!init_sd_card() || !init_fingerprint()) {
    Serial.println("FATAL: SD Card or Fingerprint sensor failed. Halting.");
    digitalWrite(RED_LED_PIN, HIGH);
    while(1); // Hentikan program jika ada kegagalan kritis
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

  Serial.println("\nAuthentication successful. Starting main tracking loop...");
}

/**
 * @brief Fungsi loop utama, dieksekusi berulang kali.
 */
void loop() {
  // --- TUGAS LATAR BELAKANG (SELALU BERJALAN) ---
  handle_status_led();
  network_loop(); // Selalu coba jaga koneksi GPRS dan MQTT

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

  // 1. Proses & Kirim Data GPS secara Periodik
  if (millis() - lastPublishMillis >= PUBLISH_INTERVAL_MS) {
    lastPublishMillis = millis();
    GpsData currentGpsData = read_gps_data();
    
    if (currentGpsData.isValid) {
      Serial.println("\n--- Processing New GPS Data ---");
      Serial.printf("Satelit: %u | HDOP: %.2f\n", (unsigned int)currentGpsData.satellites, currentGpsData.hdop);
      
      // Pencatatan ke SD Card tetap berjalan untuk backup data
      write_to_daily_log(currentGpsData);
      
      // Pengiriman data HANYA dilakukan jika terotentikasi.
      publish_data(currentGpsData);

    } else {
      Serial.println("GPS data not valid. Waiting for signal fix...");
    }
  }

  // 2. Sinkronisasi Data Offline dari Antrean
  if (mqtt.connected() && isOfflineQueueNotEmpty()) {
    Serial.println("Offline queue detected. Syncing...");
    sync_offline_data();
    delay(1000); // Beri jeda lebih lama setelah sinkronisasi untuk stabilitas GPRS
  }
}