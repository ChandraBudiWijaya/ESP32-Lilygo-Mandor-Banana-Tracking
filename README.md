# Mandor Tracking System

![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino-00979D)
![Hardware](https://img.shields.io/badge/Hardware-ESP32-E7352C)
![License](https://img.shields.io/badge/License-MIT-blue)
![Version](https://img.shields.io/badge/Version-2.0-green)

**[🌍 English & Bahasa Indonesia](README_BILINGUAL.md)**

---

## 🚩 Apa Itu Mandor Tracking System?
Mandor Tracking System adalah alat pelacak GPS berbasis ESP32 untuk supervisor lapangan. Alat ini mencatat lokasi secara otomatis, menyimpan data jika offline, dan mengirim data ke server saat koneksi tersedia. Sistem ini juga bisa memakai autentikasi sidik jari (opsional) agar lebih aman.

---

## 🔥 Fitur Utama
- **Pelacakan GPS Otomatis:** Data lokasi dicatat secara berkala.
- **Koneksi Ganda:** Bisa pakai WiFi atau SIM800L (GPRS).
- **Anti Data Hilang:** Data tetap aman di SD Card saat offline, lalu dikirim otomatis saat online.
- **Sinkronisasi Prioritas:** Data lama dikirim lebih dulu sebelum data baru.
- **Log Harian:** Semua data GPS valid dicatat ke file CSV harian.
- **Autentikasi Sidik Jari (Opsional):** Hanya supervisor terdaftar yang bisa mengaktifkan alat.
- **Indikator LED:** Status alat mudah dipantau (hijau = normal, merah = error/harus autentikasi).

---

## 🧭 Cara Kerja Singkat
1. **Alat menyala:** Cek SD Card, sensor, dan autentikasi (jika ada).
2. **Ambil data GPS:** Setiap interval tertentu, alat membaca lokasi.
3. **Cek koneksi:**
   - Jika online, data langsung dikirim ke server.
   - Jika offline, data disimpan di SD Card (queue/antrian).
4. **Saat online lagi:** Data lama di SD Card dikirim satu per satu ke server (sinkronisasi).
5. **Semua proses tercatat di log harian.**

---

## 🛠️ Kebutuhan Hardware
| Komponen              | Contoh/Model      | Keterangan                        |
|----------------------|-------------------|------------------------------------|
| ESP32                | LILYGO T-Display  | Mikrokontroler utama               |
| Modul GPS            | NEO-M8N           | Pembaca lokasi                     |
| Modul SD Card        | SPI Interface     | Penyimpanan data offline           |
| MicroSD Card         | Class 10+         | Minimal 4GB                        |
| Sensor Sidik Jari*   | R503/AS608        | Opsional, untuk autentikasi         |
| Modul SIM800L*       | SIM800L           | Opsional, untuk koneksi seluler     |
| Kartu SIM*           | 2G/3G/4G          | Opsional, untuk GPRS                |

---

## 🗂️ Struktur Folder
```
src/
├─ sim800l/
│  ├─ Fingerprint/      # SIM800L + Sidik Jari
│  └─ NoFingerprint/    # SIM800L Saja
└─ wifi/
   ├─ Fingerprint/      # WiFi + Sidik Jari
   └─ NoFingerprint/    # WiFi Saja
```

---

## ⚡ Alur Data & Sinkronisasi (Gampangnya)
- **Online:** Data GPS → Server MQTT → Selesai
- **Offline:** Data GPS → SD Card (queue)
- **Online lagi:** Data lama di SD Card → Server MQTT (satu per satu)
- **Semua data valid juga dicatat ke file log harian di SD Card**

---

## 📝 Cara Pakai Singkat
1. **Clone repo:**
   ```bash
   git clone https://github.com/ChandraBudiWijaya/ESP32-Lilygo-Mandor-Banana-Tracking.git
   cd ESP32-Lilygo-Mandor-Banana-Tracking
   ```
2. **Buka di VS Code + PlatformIO**
3. **Pilih environment** sesuai hardware (lihat status bar bawah VS Code)
4. **Edit file `config.h`** di folder environment yang dipilih (isi ID, WiFi, APN, MQTT, dsb)
5. **Build & Upload** ke ESP32
6. **Pantau log di Serial Monitor**

---

## 💡 Info Penting Lain
- **Semua data GPS valid dicatat ke file log harian (format CSV, folder per tahun/bulan/tanggal).**
- **Jika alat offline, data tetap aman di SD Card dan akan dikirim otomatis saat online.**
- **Autentikasi sidik jari bisa diaktifkan atau dimatikan sesuai kebutuhan.**
- **Koneksi bisa pakai WiFi atau SIM800L, tinggal pilih environment.**
- **Konfigurasi MQTT, WiFi, APN, dsb ada di file `config.h` masing-masing environment.**

---

## ❓ FAQ
**Q: Kalau alat mati tiba-tiba, data hilang?**
A: Tidak, data tetap aman di SD Card dan akan dikirim saat alat online lagi.

**Q: Bisa pakai tanpa sidik jari?**
A: Bisa, pilih environment "NoFingerprint".

**Q: Bisa pakai WiFi dan SIM800L sekaligus?**
A: Tidak bersamaan, tapi bisa ganti environment sesuai kebutuhan.

**Q: Data GPS selalu dikirim ke server?**
A: Ya, jika koneksi ada. Jika tidak, data akan dikirim otomatis saat koneksi kembali.

---

## 📄 Lisensi
MIT License (lihat file LICENSE)

---

**Mandor Tracking System v2.0** - Mudah, Aman, dan Andal untuk supervisor lapangan!

---

Untuk dokumentasi lengkap & bilingual, cek [README_BILINGUAL.md](README_BILINGUAL.md)