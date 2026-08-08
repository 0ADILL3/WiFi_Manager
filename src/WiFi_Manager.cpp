#include "WiFi_Manager.h"

WiFi_Manager *WiFi_Manager::instance_ = nullptr;

WiFi_Manager::WiFi_Manager(
  char (&mqtt_server)[MQTT_SERVER_MAX_LEN],
  char (&mqtt_port)[MQTT_PORT_MAX_LEN],
  char (&mqtt_username)[MQTT_USERNAME_MAX_LEN],
  char (&mqtt_password)[MQTT_PASSWORD_MAX_LEN],
  char (&mqtt_client_id)[MQTT_CLIENT_ID_MAX_LEN],
  char (&mqtt_topic)[MQTT_TOPIC_MAX_LEN],
  bool enable_config_portal,
  uint8_t config_portal_timeout,
  bool auto_connect,
  uint8_t connect_timeout,
  bool auto_reconnect,
  uint16_t reconnect_interval,
  uint8_t max_reconnect_attempts
) :
  MQTT_Server_(mqtt_server),
  MQTT_Port_(mqtt_port),
  MQTT_Username_(mqtt_username),
  MQTT_Password_(mqtt_password),
  MQTT_Client_ID_(mqtt_client_id),
  MQTT_Topic_(mqtt_topic),

  enable_config_portal_(enable_config_portal),
  config_portal_timeout_(config_portal_timeout),
  auto_connect_(auto_connect),
  connect_timeout_(connect_timeout),
  auto_reconnect_(auto_reconnect),
  reconnect_interval_(reconnect_interval),
  max_reconnect_attempts_(max_reconnect_attempts),

  Custom_MQTT_Server_("MQTT_Server", "MQTT Server", mqtt_server, MQTT_SERVER_MAX_LEN),
  Custom_MQTT_Port_("MQTT_Port", "MQTT Port", mqtt_port, MQTT_PORT_MAX_LEN),
  Custom_MQTT_Username_("MQTT_Username", "MQTT Username", mqtt_username, MQTT_USERNAME_MAX_LEN),
  Custom_MQTT_Password_("MQTT_Password", "MQTT Password", mqtt_password, MQTT_PASSWORD_MAX_LEN),
  Custom_MQTT_Client_ID_("MQTT_Client_ID", "MQTT Client ID", mqtt_client_id, MQTT_CLIENT_ID_MAX_LEN),
  Custom_MQTT_Topic_("MQTT_Topic", "MQTT Topic", mqtt_topic, MQTT_TOPIC_MAX_LEN)
{
  instance_ = this;
}

void WiFi_Manager::begin(const char *ap_name, const char *ap_password)
{
  strlcpy(AP_Name_, ap_name, sizeof(AP_Name_));
  strlcpy(AP_Password_, ap_password, sizeof(AP_Password_));

  WiFi.mode(WIFI_STA);

  load_config();
  
  WiFi_Manager_.setSaveConfigCallback(save_config_callback_);

  WiFi_Manager_.setEnableConfigPortal(enable_config_portal_);
  WiFi_Manager_.setConfigPortalTimeout(config_portal_timeout_);
  WiFi_Manager_.setConnectTimeout(connect_timeout_);

  if (!WiFi_Manager_.autoConnect(AP_Name_, AP_Password_))
  {
    Serial.println("[WiFi_Manager] Failed connect to WiFi");
    
    if (auto_connect_) 
    {
      Serial.println("[WiFi_Manager] Restarting...");
      Serial.println();
      ESP.restart();
    } 
    else 
    {
      Serial.println("[WiFi_Manager] Proceeding offline, will try to reconnect in background...");
      Serial.println();
    }
  }
  else
  {
    Serial.println();
    Serial.println("[WiFi_Manager] WiFi Connected");
    Serial.printf("[WiFi_Manager] IP address: %s\n", WiFi.localIP().toString().c_str());
  }
}

void WiFi_Manager::reconnect()
{
  if ((millis() - last_time_ > reconnect_interval_) && auto_reconnect_)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      reconnect_attempts_++;
      Serial.printf("\n[WiFi_Manager] Reconnecting...(%d attempt)\n", reconnect_attempts_);
      WiFi.reconnect();
      if (max_reconnect_attempts_ > 0 && reconnect_attempts_ >= max_reconnect_attempts_)
      {
        reconnect_attempts_ = 0;
        Serial.println();
        Serial.println("[WiFi_Manager] Reconnecting timeout, Restarting...");
        Serial.println();
        ESP.restart();
      }
    }
    else {reconnect_attempts_ = 0;}

    last_time_ = millis();
  }
}

void WiFi_Manager::load_config()
{
  prefs_.begin("MQTT_conf", true);
  if (prefs_.isKey("MQTT_Server"))    {prefs_.getString("MQTT_Server", MQTT_Server_, sizeof(MQTT_Server_));}
  if (prefs_.isKey("MQTT_Port"))      {prefs_.getString("MQTT_Port", MQTT_Port_, sizeof(MQTT_Port_));}
  if (prefs_.isKey("MQTT_Username"))  {prefs_.getString("MQTT_Username", MQTT_Username_, sizeof(MQTT_Username_));}
  if (prefs_.isKey("MQTT_Password"))  {prefs_.getString("MQTT_Password", MQTT_Password_, sizeof(MQTT_Password_));}
  if (prefs_.isKey("MQTT_Client_ID")) {prefs_.getString("MQTT_Client_ID", MQTT_Client_ID_, sizeof(MQTT_Client_ID_));}
  if (prefs_.isKey("MQTT_Topic"))     {prefs_.getString("MQTT_Topic", MQTT_Topic_, sizeof(MQTT_Topic_));}
  prefs_.end();
  
  Custom_MQTT_Server_.setValue(MQTT_Server_, MQTT_SERVER_MAX_LEN);
  Custom_MQTT_Port_.setValue(MQTT_Port_, MQTT_PORT_MAX_LEN);
  Custom_MQTT_Username_.setValue(MQTT_Username_, MQTT_USERNAME_MAX_LEN);
  Custom_MQTT_Password_.setValue(MQTT_Password_, MQTT_PASSWORD_MAX_LEN);
  Custom_MQTT_Client_ID_.setValue(MQTT_Client_ID_, MQTT_CLIENT_ID_MAX_LEN);
  Custom_MQTT_Topic_.setValue(MQTT_Topic_, MQTT_TOPIC_MAX_LEN);
  
  WiFi_Manager_.addParameter(&Custom_MQTT_Server_);
  WiFi_Manager_.addParameter(&Custom_MQTT_Port_);
  WiFi_Manager_.addParameter(&Custom_MQTT_Username_);
  WiFi_Manager_.addParameter(&Custom_MQTT_Password_);
  WiFi_Manager_.addParameter(&Custom_MQTT_Client_ID_);
  WiFi_Manager_.addParameter(&Custom_MQTT_Topic_);

  Serial.println();
  Serial.println("[WiFi_Manager] CONFIGURATION LOADED");
  Serial.printf("[WiFi_Manager] MQTT Server       : %s\n", MQTT_Server_);
  Serial.printf("[WiFi_Manager] MQTT Port         : %s\n", MQTT_Port_);
  Serial.printf("[WiFi_Manager] MQTT Username     : %s\n", MQTT_Username_);
  Serial.printf("[WiFi_Manager] MQTT Password     : %s\n", MQTT_Password_);
  Serial.printf("[WiFi_Manager] MQTT Client ID    : %s\n", MQTT_Client_ID_);
  Serial.printf("[WiFi_Manager] MQTT Topic        : %s\n", MQTT_Topic_);
}

void WiFi_Manager::save_config()
{
  strlcpy(MQTT_Server_, Custom_MQTT_Server_.getValue(), sizeof(MQTT_Server_));
  strlcpy(MQTT_Port_, Custom_MQTT_Port_.getValue(), sizeof(MQTT_Port_));
  strlcpy(MQTT_Username_, Custom_MQTT_Username_.getValue(), sizeof(MQTT_Username_));
  strlcpy(MQTT_Password_, Custom_MQTT_Password_.getValue(), sizeof(MQTT_Password_));
  strlcpy(MQTT_Client_ID_, Custom_MQTT_Client_ID_.getValue(), sizeof(MQTT_Client_ID_));
  strlcpy(MQTT_Topic_, Custom_MQTT_Topic_.getValue(), sizeof(MQTT_Topic_));

  prefs_.begin("MQTT_conf", false);
  prefs_.putString("MQTT_Server", MQTT_Server_);
  prefs_.putString("MQTT_Port", MQTT_Port_);
  prefs_.putString("MQTT_Username", MQTT_Username_);
  prefs_.putString("MQTT_Password", MQTT_Password_);
  prefs_.putString("MQTT_Client_ID", MQTT_Client_ID_);
  prefs_.putString("MQTT_Topic", MQTT_Topic_);
  prefs_.end();
  
  Serial.println();
  Serial.println("[WiFi_Manager] CONFIGURATION SAVED");
  Serial.printf("[WiFi_Manager] MQTT Server       : %s\n", MQTT_Server_);
  Serial.printf("[WiFi_Manager] MQTT Port         : %s\n", MQTT_Port_);
  Serial.printf("[WiFi_Manager] MQTT Username     : %s\n", MQTT_Username_);
  Serial.printf("[WiFi_Manager] MQTT Password     : %s\n", MQTT_Password_);
  Serial.printf("[WiFi_Manager] MQTT Client ID    : %s\n", MQTT_Client_ID_);
  Serial.printf("[WiFi_Manager] MQTT Topic        : %s\n", MQTT_Topic_);
}

void WiFi_Manager::reset_config()
{
  Serial.println();
  Serial.println("[WiFi_Manager] CONFIGURATION RESET");

  prefs_.begin("MQTT_conf", false);
  prefs_.clear();
  prefs_.end();

  WiFi_Manager_.resetSettings();

  Serial.println("[WiFi_Manager] Restarting...");
  Serial.println();
  ESP.restart();
}

WiFiManager &WiFi_Manager::get_WiFiManager() {return WiFi_Manager_;}

Preferences &WiFi_Manager::get_Preferences() {return prefs_;}