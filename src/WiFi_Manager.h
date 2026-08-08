#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>

#define AP_NAME_MAX_LEN        32
#define AP_PASSWORD_MAX_LEN    16

#define MQTT_SERVER_MAX_LEN    40
#define MQTT_PORT_MAX_LEN      6
#define MQTT_USERNAME_MAX_LEN  32
#define MQTT_PASSWORD_MAX_LEN  32
#define MQTT_CLIENT_ID_MAX_LEN 64
#define MQTT_TOPIC_MAX_LEN     80

class WiFi_Manager
{
  private:
    WiFiManager WiFi_Manager_;
    Preferences prefs_;

    char AP_Name_[AP_NAME_MAX_LEN];
    char AP_Password_[AP_PASSWORD_MAX_LEN];
    
    char (&MQTT_Server_)[MQTT_SERVER_MAX_LEN];
    char (&MQTT_Port_)[MQTT_PORT_MAX_LEN];
    char (&MQTT_Username_)[MQTT_USERNAME_MAX_LEN];
    char (&MQTT_Password_)[MQTT_PASSWORD_MAX_LEN];
    char (&MQTT_Client_ID_)[MQTT_CLIENT_ID_MAX_LEN];
    char (&MQTT_Topic_)[MQTT_TOPIC_MAX_LEN];

    bool enable_config_portal_;
    uint8_t config_portal_timeout_;
    bool auto_connect_;
    uint8_t connect_timeout_;
    bool auto_reconnect_;
    uint16_t reconnect_interval_;
    uint8_t max_reconnect_attempts_;

    uint8_t reconnect_attempts_ = 0;
    unsigned long last_time_ = 0;

    WiFiManagerParameter Custom_MQTT_Server_;
    WiFiManagerParameter Custom_MQTT_Port_;
    WiFiManagerParameter Custom_MQTT_Username_;
    WiFiManagerParameter Custom_MQTT_Password_;
    WiFiManagerParameter Custom_MQTT_Client_ID_;
    WiFiManagerParameter Custom_MQTT_Topic_;

    static WiFi_Manager *instance_;
    static void save_config_callback_() {if (instance_) {instance_->save_config();}}
  
  public:
    // Initialize WiFi Manager and MQTT configuration parameters
    WiFi_Manager(
      char (&mqtt_server)[MQTT_SERVER_MAX_LEN],
      char (&mqtt_port)[MQTT_PORT_MAX_LEN],
      char (&mqtt_username)[MQTT_USERNAME_MAX_LEN],
      char (&mqtt_password)[MQTT_PASSWORD_MAX_LEN],
      char (&mqtt_client_id)[MQTT_CLIENT_ID_MAX_LEN],
      char (&mqtt_topic)[MQTT_TOPIC_MAX_LEN],
      bool enable_config_portal = true,
      uint8_t config_portal_timeout = 120,
      bool auto_connect = true,
      uint8_t connect_timeout = 10,
      bool auto_reconnect = false,
      uint16_t reconnect_interval = 5000,
      uint8_t max_reconnect_attempts = 0
    );

    // Initialize WiFi connection and web configuration portal
    void begin(const char *ap_name, const char *ap_password);
    // Reconnect to WiFi when disconnected
    void reconnect();
    // Load MQTT configuration from non-volatile storage
    void load_config();
    // Save MQTT configuration to non-volatile storage
    void save_config();
    // Reset WiFi and MQTT configuration to default values
    void reset_config();

    // Getters for WiFiManager instances
    WiFiManager &get_WiFiManager();
    // Getters for Preferences instances
    Preferences &get_Preferences();
};