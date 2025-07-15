# Mandor Tracking System

![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino-00979D)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-E7352C)
![License](https://img.shields.io/badge/License-MIT-blue)
![Version](https://img.shields.io/badge/Version-2.0-green)

**[🌍 Read in English & Bahasa Indonesia](README_BILINGUAL.md)** | **[📖 Dokumentasi Lengkap](README_BILINGUAL.md)**

**Mandor Tracking System** adalah solusi pelacakan GPS berbasis ESP32 yang dirancang khusus untuk supervisor (mandor) di lapangan. Sistem ini mencatat data lokasi secara real-time dan mengirimkannya ke server MQTT dengan dukungan offline caching yang canggih untuk memastikan tidak ada data yang hilang.

## 🎯 Mengapa Mandor Tracking System?

- **Reliability First:** Sistem prioritas sinkronisasi memastikan data lama terkirim sebelum data baru
- **Zero Data Loss:** Offline caching otomatis ke SD Card dengan format CSV harian
- **Flexible Connectivity:** Dukungan WiFi dan GPRS (SIM800L) dalam satu project
- **Security Ready:** Opsi autentikasi fingerprint dengan re-validation berkala
- **Production Ready:** Error handling lengkap dan monitoring visual dengan LED indicator

## 🚀 Arsitektur Sistem

Sistem ini menyediakan 4 varian konfigurasi yang dapat dipilih sesuai kebutuhan:

1. **WiFi + Fingerprint:** Koneksi WiFi dengan autentikasi biometrik
2. **WiFi Only:** Koneksi WiFi tanpa autentikasi tambahan  
3. **SIM800L + Fingerprint:** Koneksi seluler dengan autentikasi biometrik
4. **SIM800L Only:** Koneksi seluler tanpa autentikasi tambahan

## ✨ Fitur Utama

### 📍 GPS Tracking
- **High Precision:** Mencatat Latitude, Longitude, HDOP, dan jumlah satelit
- **Smart Validation:** Hanya memproses data GPS yang valid
- **Real-time Monitoring:** Update status satelit dan kualitas sinyal

### 🌐 Connectivity & Data Management  
- **Dual Connectivity:** WiFi dan GPRS (SIM800L) support
- **Smart Publishing:** Publikasi real-time ke server MQTT
- **Priority Sync:** Data offline diprioritaskan saat koneksi kembali
- **Network Loop:** Auto-reconnect dan connection health monitoring

### 💾 Data Persistence
- **Offline Caching:** Auto-queue ke SD Card saat koneksi terputus
- **Daily Logs:** CSV harian dengan struktur `Tahun/Bulan/Tanggal.csv`
- **Zero Data Loss:** Dual backup system (queue + daily log)
- **Auto Sync:** Sinkronisasi otomatis data offline saat online

### 🔐 Security Features (Optional)
- **Fingerprint Auth:** Autentikasi biometrik saat startup
- **Re-validation:** Validasi ulang setiap jam untuk security
- **Visual Feedback:** LED indicator untuk status autentikasi
- **Fail-safe Mode:** System halt jika sensor fingerprint error

## 🛠️ Hardware Requirements

### Core Components
| Component | Model | Description |
|-----------|-------|-------------|
| **Microcontroller** | ESP32 | LILYGO T-Display, LILYGO T-Call, atau compatible |
| **GPS Module** | NEO-M8N | GPS receiver dengan akurasi tinggi |
| **SD Card Module** | SPI Interface | Untuk data logging dan offline cache |
| **MicroSD Card** | Class 10+ | Minimum 4GB untuk optimal performance |

### Optional Components  
| Component | Model | Use Case |
|-----------|-------|----------|
| **Fingerprint Sensor** | R503/AS608 | Untuk varian dengan autentikasi biometrik |
| **GSM/GPRS Module** | SIM800L | Untuk varian dengan konektivitas seluler |
| **SIM Card** | 2G/3G/4G | Data plan untuk koneksi GPRS |

### LED Indicators
- **Green LED:** System status dan heartbeat
- **Red LED:** Error indication dan auth requirement

## 📂 Project Structure

Sistem menggunakan **PlatformIO Multi-Environment** untuk mengelola 4 varian konfigurasi dalam satu project:

```
📁 src/
├── 📁 sim800l/                 # GPRS/Cellular variants
│   ├── 📁 Fingerprint/         # SIM800L + Biometric auth
│   │   ├── main-gsm-FP.cpp
│   │   ├── config.h
│   │   ├── fingerprint_utils.h
│   │   ├── gps_utils.h
│   │   ├── network_utils.h
│   │   └── sd_utils.h
│   └── 📁 NoFingerprint/       # SIM800L only
│       ├── main-gsm-NFP.cpp
│       ├── config.h
│       ├── gps_utils.h
│       ├── network_utils.h
│       └── sd_utils.h
└── 📁 wifi/                    # WiFi variants  
    ├── 📁 Fingerprint/         # WiFi + Biometric auth
    │   ├── main-wifi-FP.cpp
    │   ├── config.h
    │   ├── fingerprint_utils.h
    │   ├── gps_utils.h
    │   ├── network_utils.h
    │   └── sd_utils.h
    └── 📁 NoFingerprint/       # WiFi only
        ├── main-wifi-NFP.cpp
        ├── config.h
        ├── gps_utils.h
        ├── network_utils.h
        └── sd_utils.h
```

### Environment Configuration (platformio.ini)
- `env:wifi_fingerprint` - WiFi + Fingerprint
- `env:wifi_nofingerprint` - WiFi only  
- `env:sim800l_fingerprint` - SIM800L + Fingerprint
- `env:sim800l_nofingerprint` - SIM800L only

## ⚙️ Quick Start Guide

### 1. 📥 Clone Repository
```bash
git clone https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking.git
cd ESP32-Lilygo-Mandor-Banana-Tracking
```

### 2. 🛠️ Setup Development Environment  
- Install [VS Code](https://code.visualstudio.com/)
- Install [PlatformIO Extension](https://platformio.org/install/ide?install=vscode)
- Open project folder in VS Code

### 3. 🎯 Select Environment
Di status bar bawah VS Code:
1. Klik pada environment name (default)
2. Pilih environment sesuai hardware setup:
   - `env:wifi_fingerprint` - WiFi + Biometric
   - `env:wifi_nofingerprint` - WiFi only
   - `env:sim800l_fingerprint` - Cellular + Biometric  
   - `env:sim800l_nofingerprint` - Cellular only

### 4. ⚙️ Configuration
Edit file `config.h` pada environment yang dipilih:

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

### 5. 🚀 Build & Deploy
1. **Build:** Click PlatformIO Build button atau `Ctrl+Alt+B`
2. **Upload:** Click Upload button atau `Ctrl+Alt+U`  
3. **Monitor:** Click Serial Monitor untuk melihat log

## 🔧 Advanced Configuration

### GPS Settings
```cpp
#define GPS_BAUD_RATE 9600
#define PUBLISH_INTERVAL_MS 30000      // 30 detik
#define LED_BLINK_INTERVAL_MS 1000     // 1 detik
```

### Security Settings (Fingerprint variants)
```cpp
#define VALIDATION_INTERVAL_MS 3600000 // 1 jam re-validation
#define MAX_FINGERPRINT_ATTEMPTS 3    // Max failed attempts
```

### Data Logging
```cpp
#define SD_CS_PIN 5                    // SD Card CS pin
#define QUEUE_FILE "/offline_queue.txt"
#define LOG_DIR "/GPS_LOGS"           // Daily logs directory
```

## 📊 System Monitoring

### LED Status Indicators
| LED Color | Pattern | Status |
|-----------|---------|--------|
| 🟢 Green | Blinking | System running normally |
| 🟢 Green | 3x Blink | Authentication successful |
| 🔴 Red | Solid ON | Authentication required |
| 🔴 Red | Fast Blink | Fatal error (SD/Fingerprint) |

### Serial Monitor Output
```
--- Mandor Tracking System Starting (Priority Sync & Offline Fix) ---
Setup complete. Starting main loop...

--- Processing New GPS Data ---
Satelit: 8 | HDOP: 1.20
GPS Data logged to: /GPS_LOGS/2025/07/15.csv
Data published to MQTT successfully

Offline queue detected. Prioritizing sync...
Offline data synced: 1 records remaining
```

### Data Flow Architecture
```
GPS Module → ESP32 → [Authentication?] → Data Processing
                                              ↓
                                    [Network Available?]
                                         ↙         ↘
                              YES: MQTT Publish    NO: SD Queue
                                         ↓              ↓
                                  Daily Log ←----→ Priority Sync
```

## 📱 MQTT Data Format

### Published Topics
- **Location Data:** `mandor/location/{DEVICE_ID}`
- **Status Updates:** `mandor/status/{DEVICE_ID}`

### JSON Payload Example
```json
{
  "device_id": "MTS_001",
  "timestamp": "2025-07-15T10:30:00Z",
  "gps": {
    "latitude": -6.200000,
    "longitude": 106.816666,
    "hdop": 1.20,
    "satellites": 8,
    "altitude": 15.5
  },
  "system": {
    "battery": 85,
    "signal_strength": -65,
    "authenticated": true
  }
}
```

## 🔍 Troubleshooting

### Common Issues

#### 🚫 GPS Not Getting Fix
```bash
# Check GPS wiring and antenna
# Verify in Serial Monitor:
GPS data not valid. Waiting for signal fix...
```
**Solutions:**
- Ensure GPS antenna has clear sky view
- Check GPS module wiring (TX/RX pins)
- Wait 2-5 minutes for cold start

#### 📶 Network Connection Failed  
```bash
# WiFi variants
WiFi connection failed. Retrying...

# SIM800L variants  
GPRS connection failed. Check APN settings
```
**Solutions:**
- Verify WiFi credentials in `config.h`
- Check SIM card and APN settings
- Ensure good signal strength

#### 💾 SD Card Error
```bash
FATAL: SD Card failed. Halting.
```
**Solutions:**
- Check SD card formatting (FAT32)
- Verify SPI wiring connections
- Test with different SD card

#### 👆 Fingerprint Issues
```bash
FATAL: Fingerprint sensor failed. Halting.
```
**Solutions:**
- Check sensor wiring (UART pins)
- Verify power supply (3.3V/5V)
- Re-enroll fingerprints if needed

### Debug Mode
Enable verbose logging in `config.h`:
```cpp
#define DEBUG_MODE 1
#define SERIAL_DEBUG_LEVEL 3    // 0=Error, 1=Warn, 2=Info, 3=Debug
```

## 📈 Performance Optimization

### Battery Life Tips
- Adjust `PUBLISH_INTERVAL_MS` for longer intervals
- Implement deep sleep mode (custom modification)
- Use lower GPS update rates

### Memory Management
- Monitor free heap: `ESP.getFreeHeap()`
- Optimize JSON buffer sizes
- Regular SD card cleanup

### Network Efficiency
- Batch multiple GPS readings
- Compress data before transmission
- Implement QoS levels for MQTT

## 🛡️ Security Considerations

### Production Deployment
- [ ] Change default MQTT credentials
- [ ] Enable MQTT SSL/TLS
- [ ] Implement device certificates
- [ ] Regular fingerprint re-enrollment
- [ ] Firmware update mechanism

### Data Privacy
- GPS data encryption in transit
- Local data retention policies  
- Access control for MQTT topics

## 🤝 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

### Development Guidelines
- Follow existing code style
- Add comments for complex logic
- Test on actual hardware
- Update documentation

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

### MIT License Summary
- ✅ **Commercial Use** - Use in commercial projects
- ✅ **Modification** - Modify and distribute modified versions
- ✅ **Distribution** - Distribute original or modified versions
- ✅ **Private Use** - Use privately without restrictions
- ❌ **Liability** - No warranty or liability from authors
- ❌ **Warranty** - Software provided "as is"

### Copyright Notice
```
Copyright (c) 2025 Chandra Budi Wijaya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

## 🙏 Acknowledgments

- **TinyGPS++** - GPS parsing library
- **PubSubClient** - MQTT client library  
- **ArduinoJson** - JSON handling
- **SdFat** - SD card operations
- **TinyGSM** - GSM/GPRS communication

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/issues)
- **Discussions:** [GitHub Discussions](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/discussions)
- **Documentation:** [Wiki](https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking/wiki)

---

## 📊 Project Status

![GitHub release (latest by date)](https://img.shields.io/github/v/release/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking)
![GitHub last commit](https://img.shields.io/github/last-commit/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking)
![GitHub issues](https://img.shields.io/github/issues/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking)
![GitHub stars](https://img.shields.io/github/stars/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking)

**Mandor Tracking System v2.0** - Built with ❤️ for field supervisors worldwide

### 🌟 Key Features Summary
- 🛰️ **Real-time GPS Tracking** with high precision
- 📶 **Dual Connectivity** (WiFi + Cellular)  
- 💾 **Zero Data Loss** with offline caching
- 🔐 **Biometric Security** (optional)
- 📊 **Production Ready** with comprehensive monitoring

### 💡 Use Cases
- Field supervisor tracking and monitoring
- Asset location management
- Fleet management systems
- Security and safety applications
- Remote worker monitoring

**Made in Indonesia 🇮🇩 | Open Source MIT License | ESP32 Powered**