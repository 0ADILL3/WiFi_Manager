#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <vector>

#define WIFI_MANAGER_DEBUG 1

#if WIFI_MANAGER_DEBUG
  #define WIFI_MANAGER_LOG_F(fmt, ...) do {Serial.printf("\n[WiFi_Manager] " fmt, ##__VA_ARGS__);} while (0)
#else
  #define WIFI_MANAGER_LOG_F(...) do {} while (0)
#endif

#ifndef WIFI_MANAGER_AP_NAME_MAX_LEN
  #define WIFI_MANAGER_AP_NAME_MAX_LEN        32
#endif
#ifndef WIFI_MANAGER_AP_PASSWORD_MAX_LEN
  #define WIFI_MANAGER_AP_PASSWORD_MAX_LEN    16
#endif
#ifndef WIFI_MANAGER_NVS_NAMESPACE_MAX_LEN
  #define WIFI_MANAGER_NVS_NAMESPACE_MAX_LEN  16
#endif

/**
 * @struct custom_parameter
 * @brief Struktur data untuk menyimpan konfigurasi parameter kustom WiFi_Manager.
 * @param id Identifier unik untuk parameter (kunci NVS & ID HTML).
 * @param label Label teks yang ditampilkan pada antarmuka Web UI.
 * @param buffer Pointer ke array karakter untuk menyimpan nilai input.
 * @param max_len Kapasitas alokasi maksimal dari buffer.
 * @param wm_param Pointer ke objek WiFiManagerParameter bawaan library.
 */
struct custom_parameter
{
  /** @brief Identifier unik untuk parameter (kunci NVS & ID HTML). */
  const char *id;
  /** @brief Label teks yang ditampilkan pada antarmuka Web UI. */
  const char *label;
  /** @brief Pointer ke array karakter untuk menyimpan nilai input. */
  char *buffer;
  /** @brief Kapasitas alokasi maksimal dari buffer. */
  size_t max_len;
  /** @brief Pointer ke objek WiFiManagerParameter bawaan library. */
  WiFiManagerParameter* wm_param;
};

/**
 * @brief Kelas wrapper (pembungkus) untuk mengelola koneksi WiFi, Captive Portal, dan penyimpanan parameter.
 * 
 * Kelas ini mengintegrasikan library `WiFiManager` dengan library `Preferences` (NVS) pada platform ESP. 
 * Sistem ini dirancang untuk memfasilitasi pengaturan jaringan dan variabel kustom melalui antarmuka web, 
 * serta menyimpannya secara persisten.
 * 
 * Fitur utama dari kelas ini meliputi:
 * 
 * - Eksekusi Captive Portal dalam mode blocking maupun non-blocking (cocok untuk multitasking).
 * 
 * - Pengelolaan memori otomatis secara dinamis (pencegahan memory leak) untuk elemen parameter kustom.
 * 
 * - Sinkronisasi nilai otomatis antara Web UI, variabel lokal, dan Non-Volatile Storage (NVS).
 * 
 * - Fitur auto-reconnect di latar belakang berbasis waktu, lengkap dengan batasan percobaan (max attempts) 
 *   dan fallback ke auto-restart.
 */
class WiFi_Manager
{
  private:
    WiFiManager WiFi_Manager_;
    Preferences prefs_;
    
    char ap_name_[WIFI_MANAGER_AP_NAME_MAX_LEN];
    char ap_password_[WIFI_MANAGER_AP_PASSWORD_MAX_LEN];
    char nvs_namespace_[WIFI_MANAGER_NVS_NAMESPACE_MAX_LEN];

    std::vector<custom_parameter> custom_params_;

    bool auto_connect_;
    bool enable_config_portal_;
    bool auto_reconnect_;

    uint8_t connect_timeout_;
    uint8_t config_portal_timeout_;
    bool config_portal_blocking_;
    uint16_t reconnect_interval_;
    uint8_t max_reconnect_attempts_;

    uint8_t reconnect_attempts_ = 0;
    unsigned long last_time_ = 0;

    static WiFi_Manager *instance_;
    static void save_config_callback_() {if (instance_) {instance_->save_config();}}
  
  public:
    /**
     * @brief Inisialisasi objek WiFi_Manager.
     * @note Hanya mendukung 1 instance aktif dalam satu waktu karena menyimpan referensi instansiasi ini (pointer) untuk menangani fungsi callback secara statis pada saat menyimpan konfigurasi.
     */
    WiFi_Manager();

    /**
     * @brief Destruktor untuk membersihkan memori.
     * @note Secara otomatis akan menghapus semua memori dinamis (pointer) yang dialokasikan untuk parameter kustom di dalam vector saat objek dihancurkan agar tidak terjadi memory leak.
     */
    ~WiFi_Manager();
    
    /**
     * @brief Menambahkan parameter kustom ke dalam WiFiManager Web UI.
     * @note Fungsi ini HARUS dipanggil sebelum fungsi `begin()`. Memori parameter kustom dikelola dan disimpan secara otomatis ke dalam antrian.
     * @param id String identifier unik untuk parameter (digunakan sebagai kunci NVS dan ID elemen HTML).
     * @param label Teks label yang akan ditampilkan pada formulir Web UI.
     * @param buffer Pointer ke array karakter (char array) untuk menyimpan nilai parameter kustom.
     * @param max_len Panjang maksimal dari buffer string.
     */
    void add_parameter(const char *id, const char *label, char *buffer, size_t max_len);

    /**
     * @brief Menginisialisasi koneksi WiFi, memuat konfigurasi NVS, dan menjalankan Captive Portal.
     * @param ap_name Nama SSID (Access Point) saat ESP berada dalam mode konfigurasi.
     * @param ap_password Kata sandi untuk Access Point.
     * @param auto_connect Jika true, perangkat akan melakukan restart otomatis apabila gagal terhubung ke WiFi di awal.
     * @param enable_config_portal Jika true, mengaktifkan portal konfigurasi web.
     * @param auto_reconnect Jika true, secara otomatis mencoba menghubungkan ulang saat WiFi terputus pada loop.
     * @param connect_timeout Batas waktu (dalam detik) saat mencoba menyambung ke WiFi.
     * @param config_portal_timeout Batas waktu (dalam detik) portal web konfigurasi aktif.
     * @param config_portal_blocking Jika true, eksekusi baris kode selanjutnya akan tertahan (blocking) hingga proses Captive Portal selesai.
     * @param reconnect_interval Interval jeda waktu (dalam milidetik) untuk mencoba koneksi ulang saat terputus.
     * @param max_reconnect_attempts Jumlah maksimal percobaan koneksi ulang (0 = tidak terbatas). Jika batas terlampaui, perangkat akan otomatis restart.
     * @param nvs_namespace Nama namespace NVS untuk menyimpan dan memuat konfigurasi parameter kustom.
     */
    void begin(
      const char *ap_name,
      const char *ap_password,
      bool auto_connect = true,
      bool enable_config_portal = true,
      bool auto_reconnect = false,
      uint8_t connect_timeout = 10,
      uint8_t config_portal_timeout = 120,
      bool config_portal_blocking = true,
      uint16_t reconnect_interval = 5000,
      uint8_t max_reconnect_attempts = 0,
      const char *nvs_namespace = "app_conf"
    );

    /**
     * @brief Menangani proses berjalan untuk portal web non-blocking dan fungsi pemulihan koneksi (reconnect).
     * @note Wajib dipanggil secara terus-menerus di dalam `loop()` utama jika `config_portal_blocking` bernilai false.
     */
    void handle();

    /**
     * @brief Menangani logika koneksi ulang WiFi yang terputus di latar belakang.
     * @note Dipanggil secara otomatis oleh metode `handle()`. Proses ini berbasis interval millis(), dan akan merestart sistem (ESP) jika upaya melebihi batas `max_reconnect_attempts`.
     */
    void reconnect();

    /**
     * @brief Memuat parameter kustom yang tersimpan dari Non-Volatile Storage (NVS).
     * @note Fungsi ini membaca data berdasarkan array `custom_params_` dan dieksekusi secara otomatis oleh metode `begin()`.
     */
    void load_config();

    /**
     * @brief Menyimpan parameter kustom dari antarmuka Web UI ke dalam Non-Volatile Storage (NVS).
     * @note Digunakan secara internal sebagai fungsi callback yang akan dipicu oleh WiFiManager ketika pengguna menyimpan konfigurasi (klik tombol "Save").
     */
    void save_config();

    /**
     * @brief Menghapus semua pengaturan (reset) secara keseluruhan dan melakukan reboot.
     * @note Sangat berguna untuk fungsi Factory Reset. Fungsi ini membersihkan kunci di dalam namespace NVS, mereset kredensial WiFi bawaan, lalu merestart perangkat.
     */
    void reset_config();

    /**
     * @brief Mengambil referensi instansiasi objek WiFiManager.
     * @return Referensi (reference) ke objek `WiFiManager` bawaan, memungkinkan pemanggilan fungsi native dari library WiFiManager.
     */
    WiFiManager &get_WiFiManager();

    /**
     * @brief Mengambil referensi instansiasi objek Preferences.
     * @return Referensi (reference) ke objek `Preferences` (NVS) bawaan, memungkinkan penyimpanan/pembacaan variabel lain di luar alur web konfigurasi.
     */
    Preferences &get_Preferences();
};