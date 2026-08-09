#include "WiFi_Manager.h"

WiFi_Manager *WiFi_Manager::instance_ = nullptr;

WiFi_Manager::WiFi_Manager() {instance_ = this;}

WiFi_Manager::~WiFi_Manager()
{
  for (auto& param : custom_params_) {delete param.wm_param;}
  custom_params_.clear();
}

void WiFi_Manager::add_parameter(const char *id, const char *label, char *buffer, size_t max_len)
{
  WiFiManagerParameter* new_param = new WiFiManagerParameter(id, label, buffer, max_len);
  
  custom_params_.push_back({id, label, buffer, max_len, new_param});
  
  WiFi_Manager_.addParameter(new_param);
}

void WiFi_Manager::begin(
  const char *ap_name,
  const char *ap_password,
  bool auto_connect,
  bool enable_config_portal,
  bool auto_reconnect,
  uint8_t connect_timeout,
  uint8_t config_portal_timeout,
  bool config_portal_blocking,
  uint16_t reconnect_interval,
  uint8_t max_reconnect_attempts,
  const char *nvs_namespace
)
{
  strlcpy(ap_name_, ap_name, sizeof(ap_name_));
  strlcpy(ap_password_, ap_password, sizeof(ap_password_));
  strlcpy(nvs_namespace_, nvs_namespace, sizeof(nvs_namespace_));

  auto_connect_ = auto_connect;
  enable_config_portal_ = enable_config_portal;
  auto_reconnect_ = auto_reconnect;

  connect_timeout_ = connect_timeout;
  config_portal_timeout_ = config_portal_timeout;
  config_portal_blocking_ = config_portal_blocking;
  reconnect_interval_ = reconnect_interval;
  max_reconnect_attempts_ = max_reconnect_attempts;

  WiFi.mode(WIFI_STA);

  load_config();
  
  WiFi_Manager_.setSaveConfigCallback(save_config_callback_);

  WiFi_Manager_.setEnableConfigPortal(enable_config_portal_);
  WiFi_Manager_.setConfigPortalTimeout(config_portal_timeout_);
  WiFi_Manager_.setConfigPortalBlocking(config_portal_blocking_);
  WiFi_Manager_.setConnectTimeout(connect_timeout_);

  if (!WiFi_Manager_.autoConnect(ap_name_, ap_password_))
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

void WiFi_Manager::handle()
{
  if (!config_portal_blocking_) {WiFi_Manager_.process();}
  reconnect();
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
  if (prefs_.begin(nvs_namespace_, true))
  {
    Serial.printf("\n[WiFi_Manager] Loading Configuration\n");

    for (auto& param : custom_params_)
    {
      if (prefs_.isKey(param.id))
      {
        prefs_.getString(param.id, param.buffer, param.max_len);
        param.wm_param->setValue(param.buffer, param.max_len);
        Serial.printf("[WiFi_Manager] %-15s : %s\n", param.label, param.buffer);
      }
      else {Serial.printf("[WiFi_Manager] %-15s : no saved key\n", param.label);}
    }
    
    Serial.printf("[WiFi_Manager] Configuration Loaded\n");
    prefs_.end();
  }
  else {Serial.printf("\n[WiFi_Manager] Failed to initialize NVS\n");}
}

void WiFi_Manager::save_config()
{
  if (prefs_.begin(nvs_namespace_, false))
  {
    Serial.printf("\n[WiFi_Manager] Saving Configuration\n");

    for (auto& param : custom_params_)
    {
      strlcpy(param.buffer, param.wm_param->getValue(), param.max_len);
      prefs_.putString(param.id, param.buffer);
      Serial.printf("[WiFi_Manager] %-15s : %s\n", param.label, param.buffer);
    }

    Serial.printf("[WiFi_Manager] Configuration Saved\n");
    prefs_.end();
  }
  else {Serial.printf("\n[WiFi_Manager] Failed to initialize NVS\n");}

  Serial.println("[WiFi_Manager] Restarting to apply changes...");
  Serial.println();
  delay(3000); 
  ESP.restart();
}

void WiFi_Manager::reset_config()
{
  if (prefs_.begin(nvs_namespace_, false))
  {
    if (prefs_.clear()) {Serial.printf("\n[WiFi_Manager] Configuration Reset Successfully\n");}
    else {Serial.printf("\n[WiFi_Manager] Configuration Reset Failed\n");}
    prefs_.end();
  }
  else {Serial.printf("\n[WiFi_Manager] Failed to initialize NVS\n");}

  WiFi_Manager_.resetSettings();
  Serial.println("[WiFi_Manager] WiFi Credentials Cleared");

  Serial.println("[WiFi_Manager] Restarting...");
  Serial.println();
  ESP.restart();
}

WiFiManager &WiFi_Manager::get_WiFiManager() {return WiFi_Manager_;}

Preferences &WiFi_Manager::get_Preferences() {return prefs_;}