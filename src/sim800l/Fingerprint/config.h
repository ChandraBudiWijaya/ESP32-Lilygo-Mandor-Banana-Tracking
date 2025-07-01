#pragma once

// --- KONFIGURASI KARYAWAN & PERANGKAT ---
#define INDEX_KARYAWAN "122140093"
#define FINGERPRINT_ID 1

// --- KONFIGURASI PIN HARDWARE ---
// Definisi pin untuk LILYGO T-Call SIM800L
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26

// GPS (NEO-M8N)
const int GPS_RX_PIN = 22;
const int GPS_TX_PIN = 21;
const int GPS_BAUD = 9600;

// SD Card (SPI)
const int SD_CS_PIN   = 5;
const int SD_MOSI_PIN = 13;
const int SD_MISO_PIN = 15;
const int SD_SCK_PIN  = 14;

// Fingerprint Sensor R503
const int FINGERPRINT_RX_PIN = 16;
const int FINGERPRINT_TX_PIN = 17;

// LED Indikator
const int GREEN_LED_PIN = 12;
const int RED_LED_PIN   = 25;

// --- KONFIGURASI JARINGAN GPRS & MQTT ---
const char apn[]      = "internet"; // <-- GANTI SESUAI OPERATOR (Telkomsel/Indosat: "internet")
const char gprsUser[] = "";
const char gprsPass[] = "";

const char* mqtt_server = "b1f52b909a3541e1971aae91fb7b08f4.s1.eu.hivemq.cloud";
const int   mqtt_port   = 8883; // Gunakan port 8883 untuk koneksi aman (SSL/TLS)
const char* mqtt_user   = "PKLGGF";
const char* mqtt_pass   = "PklGGF123";
const char* mqtt_topic  = "mandor/tracking/data";

// --- PENGATURAN INTERVAL ---
const unsigned long PUBLISH_INTERVAL_MS = 10000;   // Interval pengiriman data (10 detik)
const unsigned long VALIDATION_INTERVAL_MS = 3600000; // Interval validasi ulang (1 jam)
const unsigned long LED_BLINK_INTERVAL_MS = 1500;  // Interval kedip LED