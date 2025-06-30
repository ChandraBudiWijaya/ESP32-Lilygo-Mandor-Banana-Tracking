#pragma once
#include <SPI.h>
#include <SD.h>
#include <time.h>
#include "FS.h"
#include "config.h"
#include "gps_utils.h"

// Nama file untuk antrean dan progres
#define QUEUE_FILE "/offline_queue.txt"
#define PROGRESS_FILE "/queue_progress.txt"

// =======================================================
//   FUNGSI KUSTOM timegm
//   (Didefinisikan di sini SEBELUM dipanggil)
// =======================================================
/**
 * @brief Implementasi kustom dari timegm() untuk mengkonversi struct tm (UTC) ke Unix timestamp.
 * Fungsi ini dibutuhkan karena timegm() tidak standar dan tidak tersedia di semua platform.
 * @param tm Pointer ke struct tm yang berisi waktu dalam UTC.
 * @return time_t Unix timestamp (detik sejak 1970-01-01 00:00:00 UTC).
 */
time_t timegm_custom(struct tm *tm) {
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long year = tm->tm_year + 1900;
    long days = 0;
    
    // Hitung jumlah hari dari tahun 1970 hingga tahun sebelum tahun target.
    for (int y = 1970; y < year; ++y) {
        days += 365;
        // Tambah satu hari jika tahun kabisat.
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            days++;
        }
    }
    
    // Hitung jumlah hari dari bulan-bulan sebelum bulan target.
    for (int m = 0; m < tm->tm_mon; ++m) {
        days += days_in_month[m];
        // Tambah satu hari jika bulan Februari pada tahun kabisat.
        if (m == 1 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
            days++;
        }
    }
    
    // Tambahkan jumlah hari dalam bulan ini.
    days += tm->tm_mday - 1;
    
    // Konversi total hari ke detik dan tambahkan waktu.
    return (days * 86400L) + (tm->tm_hour * 3600L) + (tm->tm_min * 60L) + tm->tm_sec;
}


// =======================================================
//   FUNGSI-FUNGSI LAINNYA
// =======================================================

/**
 * @brief Membaca posisi byte terakhir dari file progres.
 */
unsigned long read_progress() {
  File file = SD.open(PROGRESS_FILE, FILE_READ);
  if (file) {
    unsigned long pos = file.readString().toInt();
    file.close();
    return pos;
  }
  return 0;
}

/**
 * @brief Menulis posisi byte baru ke file progres.
 */
void write_progress(unsigned long position) {
  File file = SD.open(PROGRESS_FILE, FILE_WRITE);
  if (file) {
    file.print(position);
    file.close();
  }
}

/**
 * @brief Menginisialisasi modul SD Card.
 */
bool init_sd_card() {
  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card Mount Failed");
    return false;
  }
  Serial.println("SD Card mounted successfully.");
  return true;
}

/**
 * @brief Memeriksa apakah file antrean offline ada.
 */
bool isOfflineQueueNotEmpty() {
  return SD.exists(QUEUE_FILE);
}

/**
 * @brief Menulis data GPS ke log harian di SD Card dalam format waktu WIB (UTC+7).
 */
void write_to_daily_log(const GpsData& data) {
  if (!data.isValid) return;

  struct tm utc_tm = {0};
  utc_tm.tm_year = data.year - 1900;
  utc_tm.tm_mon = data.month - 1;
  utc_tm.tm_mday = data.day;
  utc_tm.tm_hour = data.hour;
  utc_tm.tm_min = data.minute;
  utc_tm.tm_sec = data.second;

  // Sekarang pemanggilan ini valid karena fungsinya sudah didefinisikan di atas.
  time_t utc_time = timegm_custom(&utc_tm);
  time_t wib_time = utc_time + (7 * 3600);
  struct tm* wib_tm_ptr = gmtime(&wib_time);

  const char* monthNames[12] = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
  
  char yearPath[6];
  sprintf(yearPath, "/%d", wib_tm_ptr->tm_year + 1900);
  if (!SD.exists(yearPath)) { SD.mkdir(yearPath); }

  char monthPath[20];
  sprintf(monthPath, "%s/%s", yearPath, monthNames[wib_tm_ptr->tm_mon]);
  if (!SD.exists(monthPath)) { SD.mkdir(monthPath); }

  char filePath[30];
  sprintf(filePath, "%s/%02d.csv", monthPath, wib_tm_ptr->tm_mday);

  File file = SD.open(filePath, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open daily log for appending");
    return;
  }

  if (file.size() == 0) {
    file.println("timestamp_wib,index_karyawan,latitude,longitude,hdop,satellites");
  }
  
  char wibTimestampStr[30];
  strftime(wibTimestampStr, sizeof(wibTimestampStr), "%Y-%m-%dT%H:%M:%S+07:00", wib_tm_ptr);

  char csvLine[150];
  snprintf(csvLine, sizeof(csvLine), "%s,%s,%.6f,%.6f,%.2f,%lu",
           wibTimestampStr, INDEX_KARYAWAN, data.lat, data.lng, data.hdop, data.satellites);

  if (!file.println(csvLine)) {
    Serial.println("Append to daily log failed");
  }
  file.close();
}


/**
 * @brief Menambahkan satu baris payload JSON ke file antrean offline.
 */
void add_to_offline_queue(const char* payload) {
  File file = SD.open(QUEUE_FILE, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open offline_queue.txt for writing");
    return;
  }
  file.println(payload);
  file.close();
  Serial.println("Data added to offline queue.");
}


/**
 * @brief Membaca baris berikutnya dari antrean berdasarkan pointer progres.
 */
String read_next_line_from_queue() {
    File file = SD.open(QUEUE_FILE, FILE_READ);
    if (!file) {
        Serial.println("Cannot open queue file for reading.");
        return "";
    }

    unsigned long currentPos = read_progress();
    if (currentPos >= file.size()) {
        // Jika progres sudah di akhir file, bersihkan.
        Serial.println("Queue is fully synced. Cleaning up files.");
        file.close();
        SD.remove(QUEUE_FILE);
        SD.remove(PROGRESS_FILE);
        return "";
    }

    file.seek(currentPos); // Lompat ke posisi terakhir yang berhasil
    String line = file.readStringUntil('\n');
    line.trim();
    
    // Simpan posisi baru untuk percobaan berikutnya
    write_progress(file.position());
    
    file.close();
    return line;
}

/**
 * @brief Mengembalikan pointer ke posisi sebelumnya jika pengiriman gagal.
 */
void revert_progress(const String& line) {
  unsigned long currentPos = read_progress();
  // Hitung posisi sebelum membaca baris ini.
  unsigned long previousPos = currentPos - (line.length() + 2); // Tambahkan 2 untuk karakter \r\n
  if (currentPos < (line.length() + 2)) {
    previousPos = 0;
  }
  write_progress(previousPos);
  Serial.println("Progress reverted due to send failure.");
}

