#pragma once
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include "config.h"

/**
 * @struct GpsData
 * @brief  Struktur untuk menyimpan data GPS yang sudah bersih dan ter-parsing.
 * Ini menyederhanakan pengiriman data antar fungsi.
 */
struct GpsData {
  bool isValid;          // Flag penanda apakah data yang dibaca valid atau tidak.
  double lat;            // Latitude dalam format desimal.
  double lng;            // Longitude dalam format desimal.
  float hdop;            // Horizontal Dilution of Precision (akurasi horizontal).
  uint32_t satellites;   // Jumlah satelit yang terdeteksi.
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  char isoTimestamp[25]; // Timestamp dalam format ISO 8601 (YYYY-MM-DDTHH:MM:SSZ).
};

// Inisialisasi objek TinyGPS++ untuk mem-parsing data NMEA dari GPS.
TinyGPSPlus gps;
// Menggunakan Serial Port 2 dari ESP32 untuk komunikasi dengan modul GPS.
HardwareSerial gpsSerial(2);

/**
 * @brief Menginisialisasi komunikasi serial dengan modul GPS.
 * Fungsi ini harus dipanggil sekali di dalam setup().
 */
void init_gps() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS Serial Initialized.");
}

/**
 * @brief Membaca dan mem-parsing data yang masuk dari modul GPS.
 * @return Sebuah struct GpsData yang berisi informasi lokasi, waktu, dan status.
 * Jika data tidak valid, field `isValid` akan bernilai false.
 */
GpsData read_gps_data() {
  // Baca semua data yang tersedia dari buffer serial GPS.
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  GpsData data = {0}; // Inisialisasi struct dengan nilai nol.

  // Validasi data: data dianggap valid hanya jika semua informasi penting
  // (lokasi, tanggal, waktu, hdop, satelit) berhasil didapatkan.
  data.isValid = gps.location.isValid() && 
                 gps.date.isValid() && 
                 gps.time.isValid() && 
                 gps.hdop.isValid() && 
                 gps.satellites.isValid();

  // Jika data valid, isi struct dengan nilai yang sesuai.
  if (data.isValid) {
    data.lat = gps.location.lat();
    data.lng = gps.location.lng();
    data.hdop = gps.hdop.value();
    data.satellites = gps.satellites.value();
    data.year = gps.date.year();
    data.month = gps.date.month();
    data.day = gps.date.day();
    data.hour = gps.time.hour();
    data.minute = gps.time.minute();
    data.second = gps.time.second();

    // Buat timestamp format ISO 8601 (UTC) untuk standar pengiriman data.
    snprintf(data.isoTimestamp, sizeof(data.isoTimestamp), 
             "%04d-%02d-%02dT%02d:%02d:%02dZ",
             data.year, data.month, data.day, 
             data.hour, data.minute, data.second);
  }
  
  return data;
}