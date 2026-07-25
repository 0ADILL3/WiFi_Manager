#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "WiFi_Manager.h"

#define RESET_PIN 15

char MQTT_Server[MQTT_SERVER_MAX_LEN]       = "192.168.1.100"; 
char MQTT_Port[MQTT_PORT_MAX_LEN]           = "1883";                 
char MQTT_Username[MQTT_USERNAME_MAX_LEN]   = "";                     
char MQTT_Password[MQTT_PASSWORD_MAX_LEN]   = "";                     
char MQTT_Client_ID[MQTT_CLIENT_ID_MAX_LEN] = "WiFi_Manager_Client";  
char MQTT_Topic[MQTT_TOPIC_MAX_LEN]         = "device/node_1";        

WiFi_Manager WiFi_Config(MQTT_Server, MQTT_Port, MQTT_Username, MQTT_Password, MQTT_Client_ID, MQTT_Topic);

WiFiClient ESP32_Client;
PubSubClient MQTT_Client(ESP32_Client);

void setup() {
  Serial.begin(115200);
  pinMode(RESET_PIN, INPUT_PULLUP);

  Serial.println("\n--- Memulai Sistem ---");

  // Jalankan WiFi_Manager
  // Jika gagal terhubung ke WiFi sebelumnya, alat akan memancarkan Access Point (AP)
  WiFi_Config.begin("AP WiFi_Manager", "");

  // --- Integrasi dengan MQTT ---
  // Mengambil data kredensial dari objek WiFi_Config menggunakan getter
  MQTT_Client.setServer(MQTT_Server, atoi(MQTT_Port));
  
  Serial.println("\n[MQTT Config Siap]");
  Serial.printf("Target Server : %s:%d\n", MQTT_Server, atoi(MQTT_Port));
  Serial.printf("Target Topic  : %s\n", MQTT_Topic);
}

void loop() {
  // Cek apakah tombol reset ditekan
  if (digitalRead(RESET_PIN) == 0) {
    delay(50); // Debounce
    if (digitalRead(RESET_PIN) == 0) {
      WiFi_Config.reset_config(); // Akan mereset konfigurasi dan me-restart ESP32
    }
  }

  // Taruh program utama (sensor, publish MQTT, dll) di sini
}