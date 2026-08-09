#include <Arduino.h>
#include <WiFi_Manager.h>

WiFi_Manager wifiManager;

// Siapkan buffer untuk menampung nilai parameter
char mqtt_server[40] = "broker.hivemq.com";
char mqtt_port[6]    = "1883";
char api_token[32]   = "";

void setup() {
  Serial.begin(115200);

  // 1. Daftarkan parameter SEBELUM memanggil begin()
  // Format: add_parameter(ID_Unik, Label_Web, Buffer, Max_Length)
  wifiManager.add_parameter("server", "MQTT Server", mqtt_server, 40);
  wifiManager.add_parameter("port", "MQTT Port", mqtt_port, 6);
  wifiManager.add_parameter("token", "API Token", api_token, 32);

  // 2. Mulai WiFi Manager
  wifiManager.begin("ESP32_Config", "password123");

  // 3. Tampilkan nilai yang berhasil dimuat dari NVS (atau default)
  Serial.println("\n--- Konfigurasi Terkini ---");
  Serial.printf("MQTT Server : %s\n", mqtt_server);
  Serial.printf("MQTT Port   : %s\n", mqtt_port);
  Serial.printf("API Token   : %s\n", api_token);
}

void loop() {
  // Lakukan koneksi MQTT menggunakan variabel mqtt_server dll.
  delay(1000);
}