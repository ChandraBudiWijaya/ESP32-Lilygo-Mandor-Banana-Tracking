# Mandor Tracking System | Sistem Pelacakan Mandor

![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino-00979D)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-E7352C)
![License](https://img.shields.io/badge/License-MIT-blue)
![Version](https://img.shields.io/badge/Version-2.0-green)

**[🇺🇸 English](#english)** | **[🇮🇩 Bahasa Indonesia](#bahasa-indonesia)**

---

## 🇺🇸 English

**Mandor Tracking System** is an ESP32-based GPS tracking solution specifically designed for field supervisors. This system records location data in real-time and sends it to MQTT servers with advanced offline caching support to ensure no data is lost.

### 🎯 Why Mandor Tracking System?

- **Reliability First:** Priority sync system ensures old data is sent before new data
- **Zero Data Loss:** Automatic offline caching to SD Card with daily CSV format
- **Flexible Connectivity:** WiFi and GPRS (SIM800L) support in one project
- **Security Ready:** Optional fingerprint authentication with periodic re-validation
- **Production Ready:** Complete error handling and visual monitoring with LED indicators

### 🚀 System Architecture

This system provides 4 configuration variants that can be selected according to needs:

1. **WiFi + Fingerprint:** WiFi connection with biometric authentication
2. **WiFi Only:** WiFi connection without additional authentication
3. **SIM800L + Fingerprint:** Cellular connection with biometric authentication
4. **SIM800L Only:** Cellular connection without additional authentication

### ✨ Main Features

#### 📍 GPS Tracking
- **High Precision:** Records Latitude, Longitude, HDOP, and satellite count
- **Smart Validation:** Only processes valid GPS data
- **Real-time Monitoring:** Satellite status and signal quality updates

#### 🌐 Connectivity & Data Management
- **Dual Connectivity:** WiFi and GPRS (SIM800L) support
- **Smart Publishing:** Real-time publishing to MQTT server
- **Priority Sync:** Offline data prioritized when connection returns
- **Network Loop:** Auto-reconnect and connection health monitoring

#### 💾 Data Persistence
- **Offline Caching:** Auto-queue to SD Card when connection is lost
- **Daily Logs:** Daily CSV with `Year/Month/Date.csv` structure
- **Zero Data Loss:** Dual backup system (queue + daily log)
- **Auto Sync:** Automatic offline data synchronization when online

#### 🔐 Security Features (Optional)
- **Fingerprint Auth:** Biometric authentication at startup
- **Re-validation:** Hourly re-validation for security
- **Visual Feedback:** LED indicator for authentication status
- **Fail-safe Mode:** System halt if fingerprint sensor error

### 🛠️ Hardware Requirements

#### Core Components
| Component | Model | Description |
|-----------|-------|-------------|
| **Microcontroller** | ESP32 | LILYGO T-Display, LILYGO T-Call, or compatible |
| **GPS Module** | NEO-M8N | High precision GPS receiver |
| **SD Card Module** | SPI Interface | For data logging and offline cache |
| **MicroSD Card** | Class 10+ | Minimum 4GB for optimal performance |

#### Optional Components
| Component | Model | Use Case |
|-----------|-------|----------|
| **Fingerprint Sensor** | R503/AS608 | For variants with biometric authentication |
| **GSM/GPRS Module** | SIM800L | For variants with cellular connectivity |
| **SIM Card** | 2G/3G/4G | Data plan for GPRS connection |

### ⚙️ Quick Start Guide

1. **Clone Repository**
```bash
git clone https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking.git
cd ESP32-Lilygo-Mandor-Banana-Tracking
```

2. **Setup Development Environment**
- Install [VS Code](https://code.visualstudio.com/)
- Install [PlatformIO Extension](https://platformio.org/install/ide?install=vscode)
- Open project folder in VS Code

3. **Select Environment**
In VS Code bottom status bar:
- Click on environment name (default)
- Select environment according to hardware setup:
  - `env:wifi_fingerprint` - WiFi + Biometric
  - `env:wifi_nofingerprint` - WiFi only
  - `env:sim800l_fingerprint` - Cellular + Biometric
  - `env:sim800l_nofingerprint` - Cellular only

4. **Configuration**
Edit `config.h` file in selected environment:

```cpp
// === DEVICE IDENTIFICATION ===
#define INDEX_KARYAWAN "MTS_001"        // Unique device ID
#define FINGERPRINT_ID 1               // Fingerprint ID (if used)

// === NETWORK CONFIGURATION ===
// For WiFi variants:
#define WIFI_SSID "YourWiFiName" 
#define WIFI_PASS "YourPassword"

// For SIM800L variants:
const char apn[] = "internet";         // Your carrier APN

// === MQTT SERVER ===
#define MQTT_SERVER "your-mqtt-broker.com"
#define MQTT_PORT 1883
#define MQTT_USER "username"
#define MQTT_PASS "password"
```

5. **Build & Deploy**
- **Build:** Click PlatformIO Build button or `Ctrl+Alt+B`
- **Upload:** Click Upload button or `Ctrl+Alt+U`
- **Monitor:** Click Serial Monitor to view logs

---

## 🇮🇩 Bahasa Indonesia

**Sistem Pelacakan Mandor** adalah solusi pelacakan GPS berbasis ESP32 yang dirancang khusus untuk supervisor lapangan. Sistem ini mencatat data lokasi secara real-time dan mengirimkannya ke server MQTT dengan dukungan offline caching yang canggih untuk memastikan tidak ada data yang hilang.

### 🎯 Mengapa Sistem Pelacakan Mandor?

- **Keandalan Utama:** Sistem sinkronisasi prioritas memastikan data lama terkirim sebelum data baru
- **Tanpa Kehilangan Data:** Offline caching otomatis ke SD Card dengan format CSV harian
- **Konektivitas Fleksibel:** Dukungan WiFi dan GPRS (SIM800L) dalam satu proyek
- **Siap Keamanan:** Opsi autentikasi sidik jari dengan validasi ulang berkala
- **Siap Produksi:** Penanganan error lengkap dan monitoring visual dengan indikator LED

### 🚀 Arsitektur Sistem

Sistem ini menyediakan 4 varian konfigurasi yang dapat dipilih sesuai kebutuhan:

1. **WiFi + Sidik Jari:** Koneksi WiFi dengan autentikasi biometrik
2. **WiFi Saja:** Koneksi WiFi tanpa autentikasi tambahan
3. **SIM800L + Sidik Jari:** Koneksi seluler dengan autentikasi biometrik
4. **SIM800L Saja:** Koneksi seluler tanpa autentikasi tambahan

### ✨ Fitur Utama

#### 📍 Pelacakan GPS
- **Presisi Tinggi:** Mencatat Latitude, Longitude, HDOP, dan jumlah satelit
- **Validasi Cerdas:** Hanya memproses data GPS yang valid
- **Monitoring Real-time:** Update status satelit dan kualitas sinyal

#### 🌐 Konektivitas & Manajemen Data
- **Konektivitas Ganda:** Dukungan WiFi dan GPRS (SIM800L)
- **Publikasi Cerdas:** Publikasi real-time ke server MQTT
- **Sinkronisasi Prioritas:** Data offline diprioritaskan saat koneksi kembali
- **Loop Jaringan:** Auto-reconnect dan monitoring kesehatan koneksi

#### 💾 Persistensi Data
- **Offline Caching:** Auto-queue ke SD Card saat koneksi terputus
- **Log Harian:** CSV harian dengan struktur `Tahun/Bulan/Tanggal.csv`
- **Tanpa Kehilangan Data:** Sistem backup ganda (queue + log harian)
- **Sinkronisasi Otomatis:** Sinkronisasi otomatis data offline saat online

#### 🔐 Fitur Keamanan (Opsional)
- **Autentikasi Sidik Jari:** Autentikasi biometrik saat startup
- **Validasi Ulang:** Validasi ulang setiap jam untuk keamanan
- **Umpan Balik Visual:** Indikator LED untuk status autentikasi
- **Mode Fail-safe:** Sistem berhenti jika sensor sidik jari error

### 🛠️ Kebutuhan Perangkat Keras

#### Komponen Inti
| Komponen | Model | Deskripsi |
|----------|-------|-----------|
| **Mikrokontroler** | ESP32 | LILYGO T-Display, LILYGO T-Call, atau kompatibel |
| **Modul GPS** | NEO-M8N | GPS receiver dengan akurasi tinggi |
| **Modul SD Card** | Interface SPI | Untuk logging data dan cache offline |
| **Kartu MicroSD** | Class 10+ | Minimum 4GB untuk performa optimal |

#### Komponen Opsional
| Komponen | Model | Kasus Penggunaan |
|----------|-------|------------------|
| **Sensor Sidik Jari** | R503/AS608 | Untuk varian dengan autentikasi biometrik |
| **Modul GSM/GPRS** | SIM800L | Untuk varian dengan konektivitas seluler |
| **Kartu SIM** | 2G/3G/4G | Paket data untuk koneksi GPRS |

### ⚙️ Panduan Memulai Cepat

1. **Clone Repository**
```bash
git clone https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking.git
cd ESP32-Lilygo-Mandor-Banana-Tracking
```

2. **Setup Lingkungan Pengembangan**
- Install [VS Code](https://code.visualstudio.com/)
- Install [Ekstensi PlatformIO](https://platformio.org/install/ide?install=vscode)
- Buka folder proyek di VS Code

3. **Pilih Environment**
Di status bar bawah VS Code:
- Klik pada nama environment (default)
- Pilih environment sesuai setup hardware:
  - `env:wifi_fingerprint` - WiFi + Biometrik
  - `env:wifi_nofingerprint` - WiFi saja
  - `env:sim800l_fingerprint` - Seluler + Biometrik
  - `env:sim800l_nofingerprint` - Seluler saja

4. **Konfigurasi**
Edit file `config.h` pada environment yang dipilih:

```cpp
// === IDENTIFIKASI PERANGKAT ===
#define INDEX_KARYAWAN "MTS_001"        // ID perangkat unik
#define FINGERPRINT_ID 1               // ID sidik jari (jika digunakan)

// === KONFIGURASI JARINGAN ===
// Untuk varian WiFi:
#define WIFI_SSID "NamaWiFiAnda" 
#define WIFI_PASS "PasswordAnda"

// Untuk varian SIM800L:
const char apn[] = "internet";         // APN provider Anda

// === SERVER MQTT ===
#define MQTT_SERVER "mqtt-broker-anda.com"
#define MQTT_PORT 1883
#define MQTT_USER "username"
#define MQTT_PASS "password"
```

5. **Build & Deploy**
- **Build:** Klik tombol PlatformIO Build atau `Ctrl+Alt+B`
- **Upload:** Klik tombol Upload atau `Ctrl+Alt+U`
- **Monitor:** Klik Serial Monitor untuk melihat log

---

## 📄 License | Lisensi

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

Proyek ini dilisensikan di bawah **Lisensi MIT** - lihat file [LICENSE](LICENSE) untuk detail.

## 🙏 Acknowledgments | Penghargaan

- **TinyGPS++** - GPS parsing library | Library parsing GPS
- **PubSubClient** - MQTT client library | Library klien MQTT
- **ArduinoJson** - JSON handling | Penanganan JSON
- **SdFat** - SD card operations | Operasi kartu SD
- **TinyGSM** - GSM/GPRS communication | Komunikasi GSM/GPRS

## 📞 Support | Dukungan

- **Issues:** [GitHub Issues](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/issues)
- **Discussions:** [GitHub Discussions](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/discussions)
- **Documentation:** [Wiki](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/wiki)

---

**Mandor Tracking System v2.0** - Built with ❤️ for field supervisors worldwide  
**Sistem Pelacakan Mandor v2.0** - Dibuat dengan ❤️ untuk supervisor lapangan di seluruh dunia

**Made in Indonesia 🇮🇩 | Open Source MIT License | ESP32 Powered**
