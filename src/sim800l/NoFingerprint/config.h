#pragma once

// --- KONFIGURASI KARYAWAN & PERANGKAT ---
#define INDEX_KARYAWAN "122140093"

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

// LED Indikator
const int GREEN_LED_PIN = 12;
const int RED_LED_PIN   = 25;

// --- KONFIGURASI JARINGAN GPRS & MQTT ---
const char apn[]      = "M2MAUTOTRONIC"; // <-- GANTI SESUAI OPERATOR
const char gprsUser[] = "";
const char gprsPass[] = "";

const char* mqtt_server = "test.mosquitto.org";
const int   mqtt_port   = 1883;
const char* mqtt_user   = ""; // Harus string kosong
const char* mqtt_pass   = ""; // Harus string kosong
const char* mqtt_topic  = "mandor/tracking/data";

// --- PENGATURAN INTERVAL ---
const unsigned long PUBLISH_INTERVAL_MS = 10000;   // Interval pengiriman data (10 detik)
const unsigned long LED_BLINK_INTERVAL_MS = 1500;  // Interval kedip LED