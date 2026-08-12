# WiFi_Manager library

Library khusus untuk ESP32 yang membungkus (wrapper) [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager) dan `Preferences.h`. Library ini memudahkan Anda mengatur koneksi WiFi dan **parameter custom dinamis apa pun** (seperti token API, server MQTT, threshold sensor, dll.) melalui *Captive Portal* di browser, lengkap dengan fitur penyimpanan memori non-volatile (NVS).

## Fitur
- **Dynamic Custom Parameters:** Tambahkan kolom input web sebanyak yang Anda butuhkan; nilai otomatis termuat dan tersimpan di NVS (`Preferences`).
- **Auto-Restart on Save:** Sinkronisasi data yang dijamin aman; perangkat otomatis me-restart setelah pengaturan disimpan untuk menerapkan variabel baru.
- **Non-Blocking / Asynchronous Portal:** Mendukung portal latar belakang. Program utama Anda tetap berjalan meskipun portal WiFi sedang dibuka.
- **Auto Reconnection:** Mekanisme sambung ulang pintar (*smart reconnect*) jika WiFi terputus di tengah operasi.
- **Auto Connect WiFi:** Langsung terhubung ke WiFi terakhir yang tersimpan saat perangkat menyala.

## 📦 Dependensi
Pastikan Anda sudah menginstal library berikut melalui **Library Manager** di Arduino IDE:
- `WiFiManager` oleh tzapu

## 🛠️ Instalasi
1. Download repository ini sebagai file `.zip`.
2. Buka Arduino IDE.
3. Masuk ke **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Pilih file zip yang sudah di-download.

## 🚀 Penggunaan Cepat
Silakan lihat folder `examples/` untuk referensi kode selengkapnya.