#pragma once

// --- KONFIGURASI KARYAWAN & PERANGKAT ---
#define INDEX_KARYAWAN "122140093"

// --- KONFIGURASI WIFI ---
#define WIFI_SSID "Kiwi Gejrot"
#define WIFI_PASS "11223344"

// --- KONFIGURASI PIN HARDWARE ---
// GPS (NEO-M8N) - Terhubung ke Serial1 ESP32
const int GPS_RX_PIN = 22;
const int GPS_TX_PIN = 21;
const int GPS_BAUD = 9600;

// SD Card (SPI)
const int SD_CS_PIN   = 5;
const int SD_MOSI_PIN = 13;
const int SD_MISO_PIN = 15;
const int SD_SCK_PIN  = 14;

// SIM800L (Jika digunakan, pin ini untuk T-Call)
// #define MODEM_RST      5
// #define MODEM_PWKEY    4
// #define MODEM_POWER_ON 23
// #define MODEM_TX       27
// #define MODEM_RX       26

// LED Indikator
const int GREEN_LED_PIN = 12;
const int RED_LED_PIN   = 25;

// --- KONFIGURASI JARINGAN & MQTT ---
const char* mqtt_server = "b1f52b909a3541e1971aae91fb7b08f4.s1.eu.hivemq.cloud";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "PKLGGF";
const char* mqtt_pass   = "PklGGF123";
const char* mqtt_topic  = "mandor/tracking/data";

// --- PENGATURAN INTERVAL ---
const unsigned long PUBLISH_INTERVAL_MS = 5000;    // Interval pengiriman data (ms)
const unsigned long SYNC_INTERVAL_MS = 60000;      // Interval sinkronisasi data offline (ms)
const unsigned long LED_BLINK_INTERVAL_MS = 1500;  // Interval kedip LED