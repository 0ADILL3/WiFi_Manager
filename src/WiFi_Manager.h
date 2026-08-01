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
    WiFiManager _WiFi_Manager;
    Preferences _prefs;

    char _AP_Name[AP_NAME_MAX_LEN];
    char _AP_Password[AP_PASSWORD_MAX_LEN];
    
    char (&_MQTT_Server)[MQTT_SERVER_MAX_LEN];
    char (&_MQTT_Port)[MQTT_PORT_MAX_LEN];
    char (&_MQTT_Username)[MQTT_USERNAME_MAX_LEN];
    char (&_MQTT_Password)[MQTT_PASSWORD_MAX_LEN];
    char (&_MQTT_Client_ID)[MQTT_CLIENT_ID_MAX_LEN];
    char (&_MQTT_Topic)[MQTT_TOPIC_MAX_LEN];

    bool _enable_portal;
    uint8_t _config_portal_timeout;
    bool _auto_connect;
    uint8_t _connect_timeout;
    bool _auto_reconnect;
    uint16_t _reconnect_interval;
    uint8_t _max_reconnect_attempts;

    uint8_t _reconnect_attempts = 0;
    unsigned long _last_time = 0;

    WiFiManagerParameter _Custom_MQTT_Server;
    WiFiManagerParameter _Custom_MQTT_Port;
    WiFiManagerParameter _Custom_MQTT_Username;
    WiFiManagerParameter _Custom_MQTT_Password;
    WiFiManagerParameter _Custom_MQTT_Client_ID;
    WiFiManagerParameter _Custom_MQTT_Topic;

    static WiFi_Manager *instance;
    static void save_config_callback() {if (instance) {instance->save_config();}}
  
  public:
    // Initialize WiFi Manager and MQTT configuration parameters
    WiFi_Manager(
      char (&server)[MQTT_SERVER_MAX_LEN],
      char (&port)[MQTT_PORT_MAX_LEN],
      char (&username)[MQTT_USERNAME_MAX_LEN],
      char (&password)[MQTT_PASSWORD_MAX_LEN],
      char (&client_id)[MQTT_CLIENT_ID_MAX_LEN],
      char (&topic)[MQTT_TOPIC_MAX_LEN],
      bool enable_portal = true,
      uint8_t config_portal_timeout = 120,
      bool auto_connect = true,
      uint8_t connect_timeout = 10,
      bool auto_reconnect = false,
      uint16_t reconnect_interval = 5000,
      uint8_t max_reconnect_attempts = 5
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
};