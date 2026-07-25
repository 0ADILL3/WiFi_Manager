# Custom WiFi_Manager library

Library khusus untuk ESP32 yang membungkus (wrapper) [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager) dan `Preferences.h`. Memungkinkan pengguna untuk mengatur kredensial WiFi dan parameter MQTT (Server, Port, Username, Password, Client ID, dan Topic) melalui *Captive Portal* / halaman web yang otomatis terbuka di HP.

## Fitur
- **Auto Connect WiFi:** Terhubung otomatis ke WiFi yang tersimpan.
- **Captive Portal:** Jika gagal terhubung, memancarkan Access Point (AP) untuk di-setting melalui web browser.
- **Custom Parameters:** Penambahan kolom input MQTT secara *built-in*.
- **Non-Volatile Storage:** Menyimpan data aman ke flash memori ESP32 menggunakan `Preferences`.

## Dependensi
Pastikan Anda sudah menginstal library berikut melalui Library Manager di Arduino IDE:
- `WiFiManager` oleh tzapu

## Instalasi
1. Download repository ini sebagai file `.zip`.
2. Buka Arduino IDE.
3. Masuk ke **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Pilih file zip yang sudah di-download.