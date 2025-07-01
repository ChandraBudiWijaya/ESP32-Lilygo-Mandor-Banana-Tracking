# ESP32 GPS Tracker untuk Mandor Lapangan

![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino-00979D)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-E7352C)

Proyek ini adalah perangkat pelacak GPS berbasis ESP32 yang dirancang untuk supervisor (mandor) di lapangan. Perangkat ini mencatat data lokasi secara periodik dan mengirimkannya ke server MQTT. Dilengkapi dengan kemampuan *offline-caching*, perangkat akan menyimpan data ke kartu SD jika koneksi internet terputus dan akan menyinkronkannya kembali secara otomatis saat terhubung.

Proyek ini memiliki beberapa skema yang dapat disesuaikan dengan kebutuhan perangkat keras:
1.  **Konektivitas:** WiFi atau GPRS (menggunakan modul SIM800L).
2.  **Keamanan:** Dengan atau tanpa verifikasi sidik jari.

## ✨ Fitur Utama

- **Pelacakan GPS:** Mencatat data Latitude, Longitude, HDOP, dan jumlah satelit.
- **Publikasi Real-time:** Mengirim data ke server MQTT melalui koneksi WiFi atau GPRS.
- **Offline Caching:** Jika tidak ada koneksi, data disimpan dalam antrean di kartu SD.
- **Sinkronisasi Otomatis:** Mengirimkan data yang tersimpan di antrean secara otomatis saat koneksi kembali pulih.
- **Log Harian:** Semua data GPS yang valid juga dicatat ke dalam file CSV harian di kartu SD, diatur dalam folder `Tahun/Bulan/Tanggal.csv`.
- **Otentikasi Sidik Jari (Opsional):** Memerlukan verifikasi sidik jari saat startup dan validasi ulang setiap jam untuk dapat mengirim data.

## 🛠️ Perangkat Keras yang Dibutuhkan

- **Mikrokontroler:** ESP32 (contoh: LILYGO T-Display, LILYGO T-Call).
- **Modul GPS:** NEO-M8N atau sejenisnya.
- **Modul Kartu SD:** Dihubungkan melalui SPI.
- **Kartu MicroSD.**
- **(Opsional) Sensor Sidik Jari:** R503 atau sejenisnya.
- **(Opsional) Modul GSM/GPRS:** SIM800L (jika menggunakan skema seluler).

## 📂 Struktur Proyek

Proyek ini menggunakan **PlatformIO Environments** untuk mengelola berbagai skema perangkat keras. Ini memungkinkan Anda untuk beralih antar versi dengan mudah tanpa mengubah kode secara manual.

```
src
├── sim800l
│   ├── Fingerprint     # Skema SIM800L + Sidik Jari
│   └── NoFingerprint   # Skema SIM800L Saja
└── wifi
    ├── Fingerprint     # Skema WiFi + Sidik Jari
    └── NoFingerprint   # Skema WiFi Saja
```

## ⚙️ Konfigurasi & Penggunaan

1.  **Clone Repositori:**
    ```bash
    git clone [URL_GITHUB_ANDA]
    ```

2.  **Buka dengan VS Code & PlatformIO.**

3.  **Pilih Environment:**
    Di bilah status bawah VS Code, klik pada nama environment default dan pilih salah satu dari environment yang tersedia sesuai kebutuhan Anda:
    - `env:wifi_fingerprint`
    - `env:wifi_nofingerprint`
    - `env:sim800l_fingerprint`
    - `env:sim800l_nofingerprint`

4.  **Konfigurasi File `config.h`:**
    Buka file `config.h` di dalam folder environment yang Anda pilih (misalnya `src/wifi/Fingerprint/config.h`) dan sesuaikan parameter berikut:
    - `INDEX_KARYAWAN`: ID unik untuk perangkat/pengguna.
    - `FINGERPRINT_ID`: ID sidik jari yang valid (jika digunakan).
    - `WIFI_SSID` & `WIFI_PASS`: Untuk skema WiFi.
    - `apn`: APN provider seluler Anda untuk skema SIM800L.
    - Konfigurasi pin perangkat keras jika berbeda.
    - Konfigurasi server MQTT.

5.  **Build & Upload:**
    Gunakan tombol "Build" dan "Upload" dari PlatformIO untuk mengompilasi dan mengunggah kode ke perangkat ESP32 Anda.