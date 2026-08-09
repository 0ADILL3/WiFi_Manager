#include <Arduino.h>
#include <WiFi_Manager.h>

WiFi_Manager wifiManager;

void setup() {
  Serial.begin(115200);
  
  // Parameter: (AP_Name, AP_Password)
  // Pengaturan default: Portal Aktif, Blocking mode
  wifiManager.begin("ESP32_Setup", "12345678");
  
  Serial.println("Program utama berjalan!");
}

void loop() {
  // Karena mode default adalah blocking, loop() hanya akan berjalan
  // setelah WiFi berhasil terhubung atau timeout konfigurasi habis.
  delay(1000);
}