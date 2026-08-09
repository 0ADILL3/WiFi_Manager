#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <vector>

#define AP_NAME_MAX_LEN        32
#define AP_PASSWORD_MAX_LEN    16
#define NVS_NAMESPACE_MAX_LEN  16

struct custom_parameter
{
  const char *id;
  const char *label;
  char *buffer;
  size_t max_len;
  WiFiManagerParameter* wm_param;
};

class WiFi_Manager
{
  private:
    WiFiManager WiFi_Manager_;
    Preferences prefs_;
    
    char ap_name_[AP_NAME_MAX_LEN];
    char ap_password_[AP_PASSWORD_MAX_LEN];
    char nvs_namespace_[NVS_NAMESPACE_MAX_LEN];

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
    // Constructor
    WiFi_Manager();
    // Destructor
    ~WiFi_Manager();
    
    // Add custom parameters (call before begin)
    void add_parameter(const char *id, const char *label, char *buffer, size_t max_len);

    // Initialize WiFi connection and web configuration portal
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
    // Handle non-blocking config portal (if config_portal_blocking = false) and WiFi reconnection
    void handle();
    // WiFi reconnection handler
    void reconnect();
    // Load configuration from non-volatile storage
    void load_config();
    // Save configuration to non-volatile storage
    void save_config();
    // Reset WiFi and configuration to default values
    void reset_config();

    // Getters for WiFiManager instances
    WiFiManager &get_WiFiManager();
    // Getters for Preferences instances
    Preferences &get_Preferences();
};