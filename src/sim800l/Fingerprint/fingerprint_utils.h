#pragma once
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include "config.h"

// Gunakan Hardware Serial 2 untuk sensor sidik jari
HardwareSerial fingerprintSerial(2); 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);

// Inisialisasi sensor sidik jari
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

// Mencari dan memvalidasi sidik jari
// Mengembalikan true jika ID yang ditemukan cocok dengan FINGERPRINT_ID di config
bool validate_fingerprint() {
  Serial.println("Waiting for valid finger...");
  
  // Tunggu hingga jari terdeteksi
  while (finger.getImage() != FINGERPRINT_OK);

  // Konversi gambar
  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Image conversion failed");
    return false;
  }

  // Cari sidik jari di database sensor
  if (finger.fingerSearch() != FINGERPRINT_OK) {
    Serial.println("Finger not found in database");
    return false;
  }

  // Sidik jari ditemukan!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence "); Serial.println(finger.confidence);

  // Bandingkan ID yang ditemukan dengan ID yang valid
  if (finger.fingerID == FINGERPRINT_ID) {
    Serial.println("Access Granted!");
    return true;
  } else {
    Serial.println("Access Denied: Wrong Finger");
    return false;
  }
}

