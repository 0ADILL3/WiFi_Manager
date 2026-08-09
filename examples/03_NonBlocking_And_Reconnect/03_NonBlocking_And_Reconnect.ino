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
    true,        // auto connect
    true,        // aktifkan portal
    true,        // AKTIFKAN AUTO-RECONNECT
    10,          // timeout connect WiFi (detik)
    120,         // timeout portal (detik)
    false,       // SET NON-BLOCKING!
    5000,        // set reconnect interval
    0,           // max_reconnect_attempts (0 untuk tidak ada batas)
    "app_conf"   // namespace nvs
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