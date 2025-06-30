#pragma once
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "config.h"

// Struct untuk menyimpan data GPS yang sudah diparsing
struct GpsData {
  bool isValid;
  double lat;
  double lng;
  float hdop;         // <-- BARU: Untuk menyimpan nilai HDOP
  uint32_t satellites; // <-- BARU: Untuk menyimpan jumlah satelit
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  char isoTimestamp[25]; // Format: YYYY-MM-DDTHH:MM:SSZ
};

HardwareSerial gpsSerial(1); // Gunakan Hardware Serial 1 untuk GPS
TinyGPSPlus gps;

// Inisialisasi GPS
void init_gps() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS Serial Initialized.");
}

// Membaca dan memproses data GPS
GpsData read_gps_data() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  GpsData data = {0}; // Inisialisasi struct
  // Tambahkan pengecekan validitas untuk hdop dan satelit
  data.isValid = gps.location.isValid() && gps.date.isValid() && gps.time.isValid() && gps.hdop.isValid() && gps.satellites.isValid();

  if (data.isValid) {
    data.lat = gps.location.lat();
    data.lng = gps.location.lng();
    data.hdop = gps.hdop.value();             // <-- BARU: Ambil nilai HDOP
    data.satellites = gps.satellites.value(); // <-- BARU: Ambil jumlah satelit
    data.year = gps.date.year();
    data.month = gps.date.month();
    data.day = gps.date.day();
    data.hour = gps.time.hour();
    data.minute = gps.time.minute();
    data.second = gps.time.second();

    // Buat timestamp format ISO 8601 (UTC)
    snprintf(data.isoTimestamp, sizeof(data.isoTimestamp), "%04d-%02d-%02dT%02d:%02d:%02dZ",
             data.year, data.month, data.day, data.hour, data.minute, data.second);
  }
  
  return data;
}
