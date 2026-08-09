#include <Arduino.h>
#include <WiFi_Manager.h>

WiFi_Manager wifiManager;

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);

  // Set argumen secara berurutan:
  // ap_name, ap_password, nvs_namespace, enable_config_portal, config_portal_timeout, 
  // config_portal_blocking, auto_connect, connect_timeout, auto_reconnect
  wifiManager.begin(
    "ESP32_Background", 
    "12345678", 
    "app_conf",  // namespace nvs
    true,        // aktifkan portal
    120,         // timeout portal (detik)
    false,       // SET NON-BLOCKING!
    true,        // auto connect
    10,          // timeout connect WiFi (detik)
    true         // AKTIFKAN AUTO-RECONNECT
  );

  Serial.println("\nSetup selesai, masuk ke loop() tanpa hambatan!");
}

void loop() {
  // PENTING: Panggil handle() secara terus-menerus di dalam loop
  // Fungsi ini mengurus pemrosesan web portal dan auto-reconnect
  wifiManager.handle();

  // Bukti bahwa main loop tidak terblokir
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    Serial.print("Program utama tetap berjalan... Status WiFi: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "TERKONEKSI" : "TERPUTUS");
  }
}