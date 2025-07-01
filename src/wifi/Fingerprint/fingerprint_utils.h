#pragma once
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "config.h"

// Gunakan Hardware Serial 2 untuk sensor sidik jari
HardwareSerial fingerprintSerial(2); 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

// Timeout untuk validasi sidik jari (dalam milidetik)
const unsigned long FINGERPRINT_TIMEOUT_MS = 15000; // 15 detik

/**
 * @brief Menginisialisasi sensor sidik jari.
 * @return true jika sensor ditemukan, false jika tidak.
 */
bool init_fingerprint() {
  fingerprintSerial.begin(57600, SERIAL_8N1, FINGERPRINT_RX_PIN, FINGERPRINT_TX_PIN);
  delay(100);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor found!");
    return true;
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    return false;
  }
}

/**
 * @brief Mencari dan memvalidasi sidik jari.
 * Memiliki timeout untuk mencegah program macet.
 * @return true jika ID yang ditemukan cocok, false jika salah atau timeout.
 */
bool validate_fingerprint() {
  Serial.println("Waiting for valid finger...");
  
  unsigned long startTime = millis();
  // Tunggu hingga jari terdeteksi atau timeout
  while (finger.getImage() != FINGERPRINT_OK) {
    if (millis() - startTime > FINGERPRINT_TIMEOUT_MS) {
        Serial.println("Timeout! No finger detected.");
        return false;
    }
    delay(50); // Beri jeda agar tidak membebani prosesor
  }

  // Konversi gambar sidik jari
  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Image conversion failed");
    return false;
  }

  // Cari sidik jari di database sensor
  if (finger.fingerSearch() != FINGERPRINT_OK) {
    Serial.println("Finger not found in database");
    return false;
  }

  // Sidik jari ditemukan, sekarang validasi
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence "); Serial.println(finger.confidence);

  // Bandingkan ID yang ditemukan dengan ID yang valid di config
  if (finger.fingerID == FINGERPRINT_ID) {
    Serial.println("Access Granted!");
    return true;
  } else {
    Serial.println("Access Denied: Wrong Finger");
    return false;
  }
}